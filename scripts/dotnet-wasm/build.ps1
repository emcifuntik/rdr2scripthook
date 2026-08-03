param(
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    [string]$WasiSdkPath = $env:WASI_SDK_PATH,
    [string]$WasiSysroot,
    [switch]$LockedMode
)

$ErrorActionPreference = 'Stop'

$scriptRoot = $PSScriptRoot
$repoRoot = Split-Path -Parent (Split-Path -Parent $scriptRoot)
$project = Join-Path $scriptRoot 'example\Rdr2.DotNet.Example.csproj'
$nugetConfig = Join-Path $scriptRoot 'NuGet.Config'
$publishDir = Join-Path $repoRoot "BUILD\dotnet-wasm\$Configuration"
$destination = Join-Path $repoRoot 'shared\static\mods\dotnet-example\main.wasm'

if ([string]::IsNullOrWhiteSpace($WasiSdkPath) -and
    [string]::IsNullOrWhiteSpace($WasiSysroot)) {
    $WasiSdkPath = & (Join-Path $scriptRoot 'bootstrap-wasi-sdk.ps1')
    if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($WasiSdkPath)) {
        throw 'Failed to bootstrap the pinned WASI SDK'
    }
}

$toolchainArguments = @(
    # The current NativeAOT-LLVM package defaults wasi-wasm to WASI Preview 2.
    # ScriptHook embeds Wasmtime's core-module C API, so deliberately emit a
    # Preview 1 reactor and skip component metadata/linking.
    '-p:IlcLlvmTarget=wasm32-wasi',
    '-p:LinkerFlavor=lld'
)
if (-not [string]::IsNullOrWhiteSpace($WasiSdkPath)) {
    $resolvedSdk = (Resolve-Path -LiteralPath $WasiSdkPath).Path
    $toolchainArguments += "-p:WASI_SDK_PATH=$resolvedSdk"
} elseif (-not [string]::IsNullOrWhiteSpace($WasiSysroot)) {
    $resolvedSysroot = (Resolve-Path -LiteralPath $WasiSysroot).Path.Replace('\', '/')
    foreach ($tool in 'clang', 'llvm-ar') {
        if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
            throw "$tool is required when building from a standalone WASI sysroot"
        }
    }
    # NativeAOT-LLVM only uses WASI_SDK_PATH as an SDK presence/version
    # marker. Point the actual linker at the standalone sysroot and use the
    # LLVM tools already on PATH. This is primarily the local lua-to-asm
    # synchronization path; CI uses a complete pinned WASI SDK.
    $toolchainArguments += "-p:WASI_SDK_PATH=$resolvedSysroot"
    $toolchainArguments += "-p:Rdr2WasiSysroot=$resolvedSysroot"
    $siblingResourceDir = Join-Path (Split-Path -Parent $repoRoot) `
        'lua-to-asm\build\wasm-tools\resource'
    if (Test-Path -LiteralPath (Join-Path $siblingResourceDir 'lib')) {
        $resolvedResourceDir = (Resolve-Path -LiteralPath $siblingResourceDir).Path.Replace('\', '/')
        $toolchainArguments += "-p:Rdr2ClangResourceDir=$resolvedResourceDir"
    }
    $toolchainArguments += '-p:CppCompiler=clang'
    $toolchainArguments += '-p:CppLinker=clang'
    $toolchainArguments += '-p:CppLibCreator=llvm-ar'
} else {
    throw 'Set WASI_SDK_PATH to WASI SDK 29.0 or pass -WasiSysroot'
}

$restoreArguments = @(
    'restore', $project,
    '--configfile', $nugetConfig,
    '--runtime', 'wasi-wasm'
)
if ($LockedMode) { $restoreArguments += '--locked-mode' }

Push-Location $scriptRoot
try {
    & dotnet @restoreArguments
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to restore .NET Wasm guest: $LASTEXITCODE"
    }

    & dotnet publish $project `
        --configuration $Configuration `
        --runtime wasi-wasm `
        --no-restore `
        --output $publishDir `
        @toolchainArguments
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to publish .NET Wasm guest: $LASTEXITCODE"
    }
} finally {
    Pop-Location
}

$module = Get-ChildItem -LiteralPath $publishDir -Filter '*.wasm' -File |
    Where-Object { $_.Name -notlike '*.symbols.wasm' } |
    Sort-Object Length -Descending |
    Select-Object -First 1
if (-not $module) { throw "NativeAOT-LLVM produced no Wasm module in $publishDir" }

New-Item -ItemType Directory -Path (Split-Path -Parent $destination) -Force |
    Out-Null
Copy-Item -LiteralPath $module.FullName -Destination $destination -Force

Write-Host "Published $($module.FullName)"
Write-Host "Deployed  $destination"

# Refresh only already-provisioned output mods. The source example stays
# opt-in through mod.toml.disabled, while a developer's active mod.toml in BIN
# is preserved across rebuilds.
foreach ($outputName in 'Release', 'Debug') {
    $outputMod = Join-Path $repoRoot "BIN\$outputName\mods\dotnet-example"
    if (Test-Path -LiteralPath (Join-Path $outputMod 'mod.toml')) {
        $outputModule = Join-Path $outputMod 'main.wasm'
        Copy-Item -LiteralPath $destination -Destination $outputModule -Force
        Write-Host "Updated   $outputModule"
    }
}
