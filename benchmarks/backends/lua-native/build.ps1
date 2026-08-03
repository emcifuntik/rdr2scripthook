param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo')]
    [string]$Configuration = 'Release'
)

$ErrorActionPreference = 'Stop'

$benchmarkRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$repoRoot = Split-Path -Parent $benchmarkRoot
$luaRepository = Join-Path (Split-Path -Parent $repoRoot) 'lua-to-asm'
if (-not (Test-Path -LiteralPath (Join-Path $luaRepository 'CMakePresets.json'))) {
    throw "The sibling Lua runtime repository is missing: $luaRepository"
}

$preset = $Configuration.ToLowerInvariant()
$wasiCandidates = @()
if ($env:WASI_SDK_PATH) {
    $wasiCandidates += Join-Path $env:WASI_SDK_PATH 'share\wasi-sysroot'
}
$localSysroots = Join-Path $luaRepository 'build\wasm-tools\sysroot'
if (Test-Path -LiteralPath $localSysroots -PathType Container) {
    $wasiCandidates += @(Get-ChildItem -LiteralPath $localSysroots -Directory |
        Select-Object -ExpandProperty FullName)
}
$wasiSysroot = @($wasiCandidates | Where-Object {
    Test-Path -LiteralPath (Join-Path $_ 'include') -PathType Container
} | Select-Object -First 1)
if ($wasiSysroot.Count -ne 1) {
    throw 'Set WASI_SDK_PATH or provision the sibling repository WASI sysroot'
}
Push-Location $luaRepository
try {
    & cmake --preset $preset "-DLUARC_WASI_SYSROOT=$($wasiSysroot[0])"
    if ($LASTEXITCODE -ne 0) { throw "Lua CMake configure failed: $LASTEXITCODE" }
    & cmake --build --preset $preset --target lua_benchmark_runner
    if ($LASTEXITCODE -ne 0) { throw "Native Lua build failed: $LASTEXITCODE" }
} finally {
    Pop-Location
}

$source = Join-Path $luaRepository `
    "build\$preset\$Configuration\lua_benchmark_runner.exe"
if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
    throw "Native Lua executable is missing after the build: $source"
}
$destinationDirectory = Join-Path $benchmarkRoot 'artifacts\lua-native'
New-Item -ItemType Directory -Path $destinationDirectory -Force | Out-Null
Copy-Item -LiteralPath $source `
    -Destination (Join-Path $destinationDirectory 'lua-native-bench.exe') -Force
