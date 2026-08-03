param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo')]
    [string]$Configuration = 'Release'
)

$ErrorActionPreference = 'Stop'

$benchmarkRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$repoRoot = Split-Path -Parent $benchmarkRoot
$luaRepository = Join-Path (Split-Path -Parent $repoRoot) 'lua-to-asm'
if (-not (Test-Path -LiteralPath (Join-Path $luaRepository 'CMakePresets.json'))) {
    throw "The sibling Lua compiler repository is missing: $luaRepository"
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
$resourceDirectory = Join-Path $luaRepository 'build\wasm-tools\resource'
Push-Location $luaRepository
try {
    $configureArguments = @(
        '--preset', $preset,
        "-DLUARC_WASI_SYSROOT=$($wasiSysroot[0])"
    )
    if (Test-Path -LiteralPath $resourceDirectory -PathType Container) {
        $configureArguments += "-DLUARC_WASM_CLANG_RESOURCE_DIR=$resourceDirectory"
    }
    & cmake @configureArguments
    if ($LASTEXITCODE -ne 0) { throw "Lua CMake configure failed: $LASTEXITCODE" }
    & cmake --build --preset $preset
    if ($LASTEXITCODE -ne 0) { throw "Lua compiler build failed: $LASTEXITCODE" }
} finally {
    Pop-Location
}

$outputDirectory = Join-Path $luaRepository "build\$preset\$Configuration"
$compiler = $null
foreach ($candidate in @(Get-ChildItem -LiteralPath $outputDirectory -Filter '*.exe' -File)) {
    $capabilities = (& $candidate.FullName --help 2>&1 | Out-String)
    if ($LASTEXITCODE -eq 0 -and $capabilities -match '--profile NAME' -and
        $capabilities -match 'dump-bytecode') {
        $compiler = $candidate.FullName
        break
    }
}
if (-not $compiler) {
    throw "Could not locate the Lua compiler in $outputDirectory"
}

$entrypoint = Join-Path $PSScriptRoot 'entry.lua'
$kernels = Join-Path $benchmarkRoot 'shared\lua\benchmark_kernels.lua'
$destinationDirectory = Join-Path $benchmarkRoot 'artifacts\lua-wasmtime'
$destination = Join-Path $destinationDirectory 'module.wasm'
New-Item -ItemType Directory -Path $destinationDirectory -Force | Out-Null
& $compiler compile --profile standalone -O2 --strip-debug $entrypoint `
    --module "benchmark_kernels=$kernels" -o $destination
if ($LASTEXITCODE -ne 0) { throw "Lua benchmark compilation failed: $LASTEXITCODE" }
