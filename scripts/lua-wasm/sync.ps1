param(
    [string]$CompilerPath = $env:LUA_COMPILER_EXECUTABLE,
    [ValidateRange(0, 3)]
    [int]$OptimizationLevel = 2
)

$ErrorActionPreference = 'Stop'

$scriptRoot = $PSScriptRoot
$repoRoot = Split-Path -Parent (Split-Path -Parent $scriptRoot)
$luaRepository = Join-Path (Split-Path -Parent $repoRoot) 'lua-to-asm'
$source = Join-Path $scriptRoot 'example\main.lua'
$modDirectory = Join-Path $repoRoot 'shared\static\mods\lua-example'
$destination = Join-Path $modDirectory 'main.wasm'
$lockPath = Join-Path $scriptRoot 'compiler.lock.json'

function Get-GitSourceFingerprint([string]$Repository) {
    $files = @(& git -C $Repository -c core.quotepath=false ls-files `
        --cached --others --exclude-standard)
    if ($LASTEXITCODE -ne 0) {
        throw 'Could not enumerate the Lua compiler source tree'
    }
    $sourceFiles = @($files | Sort-Object -CaseSensitive | Where-Object {
        -not (Test-Path -LiteralPath (Join-Path $Repository $_) `
            -PathType Container)
    })
    $blobs = @($sourceFiles | & git -C $Repository hash-object --stdin-paths)
    if ($LASTEXITCODE -ne 0 -or $blobs.Count -ne $sourceFiles.Count) {
        throw 'Could not hash the Lua compiler source tree'
    }
    $entries = for ($index = 0; $index -lt $sourceFiles.Count; ++$index) {
        "$($blobs[$index]) $($sourceFiles[$index].Replace('\', '/'))"
    }
    $bytes = [System.Text.Encoding]::UTF8.GetBytes(($entries -join "`n"))
    $sha256 = [System.Security.Cryptography.SHA256]::Create()
    try {
        return -join ($sha256.ComputeHash($bytes) |
            ForEach-Object { $_.ToString('x2') })
    } finally {
        $sha256.Dispose()
    }
}

if ([string]::IsNullOrWhiteSpace($CompilerPath)) {
    $candidateDirectories = @(
        (Join-Path $luaRepository 'build\relwithdebinfo\RelWithDebInfo'),
        (Join-Path $luaRepository 'build\release\Release')
    )
    $candidates = @($candidateDirectories |
        Where-Object { Test-Path -LiteralPath $_ -PathType Container } |
        ForEach-Object {
            Get-ChildItem -LiteralPath $_ -Filter '*.exe' -File |
                Sort-Object Name |
                Select-Object -ExpandProperty FullName
        })
    foreach ($candidate in $candidates) {
        $capabilities = (& $candidate --help 2>&1 | Out-String)
        if ($LASTEXITCODE -eq 0 -and
            $capabilities -match '--profile NAME' -and
            $capabilities -match 'dump-bytecode') {
            $CompilerPath = $candidate
            break
        }
    }
}
if ([string]::IsNullOrWhiteSpace($CompilerPath) -or
    -not (Test-Path -LiteralPath $CompilerPath)) {
    throw 'Set LUA_COMPILER_EXECUTABLE or build the sibling lua-to-asm repository'
}

New-Item -ItemType Directory -Path $modDirectory -Force | Out-Null
& $CompilerPath compile --profile rdr2 "-O$OptimizationLevel" --strip-debug `
    $source -o $destination
if ($LASTEXITCODE -ne 0) { throw "Lua compilation failed: $LASTEXITCODE" }

$sourceCommit = 'external-release'
$sourceDirty = $false
$sourceFingerprint = $null
if (Test-Path -LiteralPath (Join-Path $luaRepository '.git')) {
    $sourceCommit = (& git -C $luaRepository rev-parse HEAD).Trim()
    if ($LASTEXITCODE -ne 0) { throw 'Could not read the Lua compiler source commit' }
    $sourceDirty = [bool](& git -C $luaRepository status --porcelain)
    $sourceFingerprint = Get-GitSourceFingerprint $luaRepository
}

$lock = [ordered]@{
    schema_version = 1
    compiler_repository = 'lua-to-asm'
    compiler_commit = $sourceCommit
    compiler_worktree_dirty = $sourceDirty
    compiler_source_sha256 = $sourceFingerprint
    compiler_abi_version = 1
    scripthook_abi_version = 1
    artifact = 'shared/static/mods/lua-example/main.wasm'
    artifact_sha256 = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant()
}
$lock | ConvertTo-Json | Set-Content -LiteralPath $lockPath -Encoding utf8

Write-Host "Compiled $source"
Write-Host "Deployed $destination"
Write-Host "Locked   $lockPath"

# Refresh only outputs where the developer explicitly enabled this example.
foreach ($outputName in 'Release', 'Debug') {
    $outputMod = Join-Path $repoRoot "BIN\$outputName\mods\lua-example"
    if (Test-Path -LiteralPath (Join-Path $outputMod 'mod.toml')) {
        $outputModule = Join-Path $outputMod 'main.wasm'
        Copy-Item -LiteralPath $destination -Destination $outputModule -Force
        Write-Host "Updated  $outputModule"
    }
}
