param(
    [string]$CompilerRepository,
    [string]$OutputPath,
    [switch]$FailOnMissing
)

$ErrorActionPreference = 'Stop'

$benchmarkRoot = $PSScriptRoot
$repoRoot = Split-Path -Parent $benchmarkRoot
if ([string]::IsNullOrWhiteSpace($CompilerRepository)) {
    $CompilerRepository = Join-Path (Split-Path -Parent $repoRoot) 'lua-to-asm'
}
$CompilerRepository = (Resolve-Path -LiteralPath $CompilerRepository).Path

$compilerCandidates = @(
    (Join-Path $CompilerRepository 'build\release\Release\luarc.exe'),
    (Join-Path $CompilerRepository 'build\Release\luarc.exe')
)
$compiler = @($compilerCandidates | Where-Object {
    Test-Path -LiteralPath $_ -PathType Leaf
}) | Select-Object -First 1
if (-not $compiler) {
    throw "Could not find luarc.exe under '$CompilerRepository'"
}

$source = Join-Path $benchmarkRoot 'shared\lua\benchmark_kernels.lua'
$dump = @(& $compiler dump-bytecode $source 2>&1)
if ($LASTEXITCODE -ne 0) {
    throw "Could not dump benchmark bytecode:`n$($dump -join [Environment]::NewLine)"
}

$opcodeHeader = Join-Path $CompilerRepository 'external\lua\lopcodes.h'
$header = @(Get-Content -LiteralPath $opcodeHeader)
$enumStart = ($header | Select-String '^typedef enum \{' | Select-Object -Last 1).LineNumber
$enumEnd = ($header | Select-String '^\} OpCode;' | Select-Object -First 1).LineNumber
if (-not $enumStart -or -not $enumEnd -or $enumEnd -le $enumStart) {
    throw "Could not parse the opcode enum in '$opcodeHeader'"
}

$allOpcodes = @($header[$enumStart..($enumEnd - 2)] | ForEach-Object {
    if ($_ -match '^\s*(OP_[A-Z0-9_]+)') { $Matches[1] }
})
$coveredOpcodes = @($dump | ForEach-Object {
    if ($_ -match '\]\s+([A-Z][A-Z0-9_]*)\s') { "OP_$($Matches[1])" }
} | Sort-Object -Unique)

$structuralOpcodes = @{
    OP_LOADKX = 'Only emitted after exceeding the regular constant-index field; impractical for a timed kernel.'
}
$rows = foreach ($opcode in $allOpcodes) {
    $covered = $coveredOpcodes -contains $opcode
    [pscustomobject]@{
        opcode = $opcode
        status = if ($covered) { 'covered' } elseif ($structuralOpcodes.ContainsKey($opcode)) { 'structural' } else { 'missing' }
        note = if ($structuralOpcodes.ContainsKey($opcode)) { $structuralOpcodes[$opcode] } else { '' }
    }
}

$coveredCount = @($rows | Where-Object status -eq 'covered').Count
$missing = @($rows | Where-Object status -eq 'missing')
$structuralCount = @($rows | Where-Object status -eq 'structural').Count

if ([string]::IsNullOrWhiteSpace($OutputPath)) {
    $resultsDirectory = Join-Path $benchmarkRoot 'results'
    New-Item -ItemType Directory -Force -Path $resultsDirectory | Out-Null
    $OutputPath = Join-Path $resultsDirectory 'lua-bytecode-coverage.md'
}

$markdown = [System.Collections.Generic.List[string]]::new()
$markdown.Add('# Lua benchmark bytecode coverage')
$markdown.Add('')
$markdown.Add("Generated from ``benchmark_kernels.lua`` with ``$([IO.Path]::GetFileName($compiler))``.")
$markdown.Add('')
$markdown.Add("- Covered: $coveredCount / $($allOpcodes.Count)")
$markdown.Add("- Structural-only: $structuralCount")
$markdown.Add("- Missing: $($missing.Count)")
$markdown.Add('')
$markdown.Add('| Opcode | Status | Note |')
$markdown.Add('|---|---|---|')
foreach ($row in $rows) {
    $markdown.Add("| $($row.opcode) | $($row.status) | $($row.note) |")
}
[IO.File]::WriteAllLines((Join-Path (Resolve-Path (Split-Path -Parent $OutputPath)).Path (Split-Path -Leaf $OutputPath)), $markdown)

$rows | Format-Table opcode, status, note -AutoSize
Write-Host "Covered $coveredCount/$($allOpcodes.Count) opcodes; structural=$structuralCount; missing=$($missing.Count)"
Write-Host "Report: $OutputPath"

if ($FailOnMissing -and $missing.Count -gt 0) {
    throw "Missing benchmark coverage: $($missing.opcode -join ', ')"
}
