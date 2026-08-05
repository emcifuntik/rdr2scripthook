param(
    [string]$CompilerPath = $env:LUA_COMPILER_EXECUTABLE,
    [ValidateRange(0, 3)]
    [int]$OptimizationLevel = 2,
    [switch]$InstallRelease
)

$ErrorActionPreference = 'Stop'

$scriptRoot = $PSScriptRoot
$repoRoot = Split-Path -Parent (Split-Path -Parent $scriptRoot)
$luaRepository = Join-Path (Split-Path -Parent $repoRoot) 'lua-to-asm'
$source = Join-Path $scriptRoot 'frontier-watch\main.lua'
$staticMod = Join-Path $repoRoot 'shared\static\mods\frontier-watch'
$artifact = Join-Path $staticMod 'main.wasm'

if ([string]::IsNullOrWhiteSpace($CompilerPath)) {
    $candidates = @(
        (Join-Path $luaRepository 'build\release\Release\luarc.exe'),
        (Join-Path $luaRepository 'build\relwithdebinfo\RelWithDebInfo\luarc.exe')
    )
    $CompilerPath = $candidates |
        Where-Object { Test-Path -LiteralPath $_ -PathType Leaf } |
        Select-Object -First 1
}
if ([string]::IsNullOrWhiteSpace($CompilerPath) -or
    -not (Test-Path -LiteralPath $CompilerPath -PathType Leaf)) {
    throw 'Set LUA_COMPILER_EXECUTABLE or build the sibling lua-to-asm repository'
}

New-Item -ItemType Directory -Path $staticMod -Force | Out-Null
& $CompilerPath compile --profile rdr2 "-O$OptimizationLevel" --strip-debug `
    $source -o $artifact
if ($LASTEXITCODE -ne 0) { throw "Lua compilation failed: $LASTEXITCODE" }

Write-Host "Compiled $source"
Write-Host "Deployed $artifact"
Write-Host "SHA-256 $((Get-FileHash -LiteralPath $artifact -Algorithm SHA256).Hash.ToLowerInvariant())"

if ($InstallRelease) {
    $outputMod = Join-Path $repoRoot 'BIN\Release\mods\frontier-watch'
    New-Item -ItemType Directory -Path $outputMod -Force | Out-Null
    Copy-Item -LiteralPath $artifact `
        -Destination (Join-Path $outputMod 'main.wasm') -Force
    Copy-Item -LiteralPath (Join-Path $staticMod 'mod.toml.disabled') `
        -Destination (Join-Path $outputMod 'mod.toml') -Force
    Write-Host "Installed $outputMod"
}
