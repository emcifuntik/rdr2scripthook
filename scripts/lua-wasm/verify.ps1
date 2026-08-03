$ErrorActionPreference = 'Stop'

$scriptRoot = $PSScriptRoot
$repoRoot = Split-Path -Parent (Split-Path -Parent $scriptRoot)
$lockPath = Join-Path $scriptRoot 'compiler.lock.json'
$lock = Get-Content -LiteralPath $lockPath -Raw | ConvertFrom-Json
$artifact = Join-Path $repoRoot ($lock.artifact -replace '/', '\')
$luaRepository = Join-Path (Split-Path -Parent $repoRoot) 'lua-to-asm'

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

if ($lock.compiler_abi_version -ne 1 -or $lock.scripthook_abi_version -ne 1) {
    throw 'Unsupported Lua/ScriptHook ABI lock'
}
if (-not (Test-Path -LiteralPath $artifact)) {
    throw "Locked Lua artifact is missing: $artifact"
}
$actual = (Get-FileHash -LiteralPath $artifact -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actual -ne $lock.artifact_sha256) {
    throw "Lua artifact checksum mismatch: expected $($lock.artifact_sha256), got $actual"
}
if ($lock.compiler_source_sha256 -and
    (Test-Path -LiteralPath (Join-Path $luaRepository '.git'))) {
    $sourceFingerprint = Get-GitSourceFingerprint $luaRepository
    if ($sourceFingerprint -ne $lock.compiler_source_sha256) {
        throw 'The sibling Lua compiler source tree changed; run sync.ps1 again'
    }
}
Write-Host "Verified Lua artifact $actual"
