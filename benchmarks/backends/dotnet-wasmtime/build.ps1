param(
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    [string]$WasiSdkPath = $env:WASI_SDK_PATH,
    [string]$WasiSysroot
)

$ErrorActionPreference = 'Stop'

$backendRoot = $PSScriptRoot
$benchmarkRoot = Split-Path -Parent (Split-Path -Parent $backendRoot)
$repoRoot = Split-Path -Parent $benchmarkRoot
$project = Join-Path $backendRoot 'DotNetWasmtimeBench.csproj'
$nugetConfig = Join-Path $repoRoot 'scripts\dotnet-wasm\NuGet.Config'
$publishDirectory = Join-Path $repoRoot "BUILD\benchmarks\dotnet-wasmtime\$Configuration"
$artifactDirectory = Join-Path $benchmarkRoot 'artifacts\dotnet-wasmtime'
$artifact = Join-Path $artifactDirectory 'module.wasm'

if ([string]::IsNullOrWhiteSpace($WasiSdkPath) -and
    [string]::IsNullOrWhiteSpace($WasiSysroot)) {
    $WasiSdkPath = & (Join-Path $repoRoot 'scripts\dotnet-wasm\bootstrap-wasi-sdk.ps1')
    if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($WasiSdkPath)) {
        throw 'Failed to bootstrap the pinned WASI SDK'
    }
}

$toolchainArguments = @(
    '-p:IlcLlvmTarget=wasm32-wasi',
    '-p:LinkerFlavor=lld'
)
if (-not [string]::IsNullOrWhiteSpace($WasiSdkPath)) {
    $resolvedSdk = (Resolve-Path -LiteralPath $WasiSdkPath).Path
    $toolchainArguments += "-p:WASI_SDK_PATH=$resolvedSdk"
} elseif (-not [string]::IsNullOrWhiteSpace($WasiSysroot)) {
    $resolvedSysroot = (Resolve-Path -LiteralPath $WasiSysroot).Path.Replace('\', '/')
    $toolchainArguments += "-p:WASI_SDK_PATH=$resolvedSysroot"
    $toolchainArguments += "-p:Rdr2WasiSysroot=$resolvedSysroot"
    $resourceDirectory = Join-Path (Split-Path -Parent $repoRoot) `
        'lua-to-asm\build\wasm-tools\resource'
    if (Test-Path -LiteralPath (Join-Path $resourceDirectory 'lib')) {
        $resolvedResource = (Resolve-Path -LiteralPath $resourceDirectory).Path.Replace('\', '/')
        $toolchainArguments += "-p:Rdr2ClangResourceDir=$resolvedResource"
    }
    $toolchainArguments += '-p:CppCompiler=clang'
    $toolchainArguments += '-p:CppLinker=clang'
    $toolchainArguments += '-p:CppLibCreator=llvm-ar'
} else {
    throw 'Set WASI_SDK_PATH or pass -WasiSysroot'
}

$restoreArguments = @(
    'restore', $project,
    '--configfile', $nugetConfig,
    '--runtime', 'wasi-wasm'
)
if (Test-Path -LiteralPath (Join-Path $backendRoot 'packages.lock.json')) {
    $restoreArguments += '--locked-mode'
}

Push-Location $benchmarkRoot
try {
    & dotnet @restoreArguments
    if ($LASTEXITCODE -ne 0) { throw "Wasm restore failed: $LASTEXITCODE" }

    & dotnet publish $project --configuration $Configuration `
        --runtime wasi-wasm --no-restore --output $publishDirectory `
        @toolchainArguments
    if ($LASTEXITCODE -ne 0) { throw "Wasm publish failed: $LASTEXITCODE" }
} finally {
    Pop-Location
}

$module = Get-ChildItem -LiteralPath $publishDirectory -Filter '*.wasm' -File |
    Where-Object { $_.Name -notlike '*.symbols.wasm' } |
    Sort-Object Length -Descending |
    Select-Object -First 1
if (-not $module) { throw 'NativeAOT-LLVM produced no Wasm module' }

New-Item -ItemType Directory -Path $artifactDirectory -Force | Out-Null
Copy-Item -LiteralPath $module.FullName -Destination $artifact -Force
Write-Host "Built $artifact"
