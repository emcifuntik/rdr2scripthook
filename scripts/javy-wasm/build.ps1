[CmdletBinding()]
param(
    [switch] $RebuildJavy
)

$ErrorActionPreference = "Stop"
$JavyVersion = "9.1.0"
$WasmTarget = "wasm32-wasip1"

$ScriptRoot = $PSScriptRoot
$RepositoryRoot = (Resolve-Path (Join-Path $ScriptRoot "..\..")).Path
$BuildRoot = Join-Path $RepositoryRoot "BUILD\Javy"
$JavySource = Join-Path $RepositoryRoot "BUILD\javy-src"
$JavyTargetRoot = Join-Path $JavySource "target"
$JavyExecutable = Join-Path $JavyTargetRoot "release\javy.exe"
$JavyDefaultPlugin = Join-Path $JavyTargetRoot "$WasmTarget\release\plugin.wasm"
$JavyPatch = Join-Path $ScriptRoot "javy-9.1-runtime.patch"
$PluginManifest = Join-Path $ScriptRoot "plugin\Cargo.toml"
$PluginTargetRoot = if ($env:CARGO_TARGET_DIR) {
    [IO.Path]::GetFullPath($env:CARGO_TARGET_DIR)
} else {
    Join-Path $ScriptRoot "plugin\target"
}
$PluginWasm = Join-Path $PluginTargetRoot "$WasmTarget\release\rdr2_javy_plugin.wasm"
$InitializedPlugin = Join-Path $BuildRoot "rdr2_javy_plugin.wasm"
$CombinedSource = Join-Path $BuildRoot "trainer.js"
$OutputWasm = Join-Path $BuildRoot "trainer.wasm"
$StaticMod = Join-Path $RepositoryRoot "shared\static\mods\javy-trainer"
$LocalWasiSdk = Join-Path $RepositoryRoot "BUILD\tools\wasi-sdk-20"

if (-not $env:WASI_SDK -and (Test-Path (Join-Path $LocalWasiSdk "bin\clang.exe"))) {
    $env:WASI_SDK = $LocalWasiSdk
}

New-Item -ItemType Directory -Force $BuildRoot | Out-Null

if (-not (Test-Path (Join-Path $JavySource ".git"))) {
    if (Test-Path $JavySource) {
        throw "Javy source path exists but is not a Git checkout: $JavySource"
    }

    Write-Host "Cloning Javy v$JavyVersion..."
    & git clone --branch "v$JavyVersion" --depth 1 https://github.com/bytecodealliance/javy.git $JavySource
    if ($LASTEXITCODE -ne 0) { throw "Failed to clone Javy v$JavyVersion" }
}

$CheckedOutVersion = (& git -C $JavySource describe --tags --exact-match 2>$null).Trim()
if ($LASTEXITCODE -ne 0 -or $CheckedOutVersion -ne "v$JavyVersion") {
    throw "Expected Javy v$JavyVersion at $JavySource, found '$CheckedOutVersion'"
}

$PatchApplied = $false
$SavedErrorActionPreference = $ErrorActionPreference
$ErrorActionPreference = "Continue"
& git -C $JavySource apply --check $JavyPatch 2>$null
$PatchCanApply = $LASTEXITCODE -eq 0
$ErrorActionPreference = $SavedErrorActionPreference
if ($PatchCanApply) {
    & git -C $JavySource apply $JavyPatch
    if ($LASTEXITCODE -ne 0) { throw "Failed to apply the RDR2 Javy runtime patch" }
    $PatchApplied = $true
} else {
    $ErrorActionPreference = "Continue"
    & git -C $JavySource apply --reverse --check $JavyPatch 2>$null
    $PatchCanReverse = $LASTEXITCODE -eq 0
    $ErrorActionPreference = $SavedErrorActionPreference
    if (-not $PatchCanReverse) {
        throw "Javy checkout contains changes incompatible with $JavyPatch"
    }
}

$InstalledTargets = & rustup target list --installed
if ($InstalledTargets -notcontains $WasmTarget) {
    & rustup target add $WasmTarget
    if ($LASTEXITCODE -ne 0) { throw "Failed to install Rust target $WasmTarget" }
}

if ($RebuildJavy -or $PatchApplied -or -not (Test-Path $JavyExecutable)) {
    # javy-cli's build.rs reads the default plugin from the workspace target
    # directory and embeds it in the executable. Cargo does not infer that
    # dependency, so this must be built first on a clean machine.
    Write-Host "Building the pinned Javy v$JavyVersion default plugin..."
    & cargo build `
        --manifest-path (Join-Path $JavySource "Cargo.toml") `
        --target-dir $JavyTargetRoot `
        --release `
        --locked `
        --package javy-plugin `
        --target $WasmTarget
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path $JavyDefaultPlugin)) {
        throw "Failed to build the Javy v$JavyVersion default plugin"
    }

    Write-Host "Building the pinned Javy v$JavyVersion compiler..."
    $PreviousReleaseLto = $env:CARGO_PROFILE_RELEASE_LTO
    $JavyBuildExitCode = 0
    try {
        # This matches upstream's CLI target and avoids spending several
        # minutes applying LTO to a compiler that is not part of the runtime.
        $env:CARGO_PROFILE_RELEASE_LTO = "off"
        & cargo build `
            --manifest-path (Join-Path $JavySource "Cargo.toml") `
            --target-dir $JavyTargetRoot `
            --release `
            --locked `
            --package javy-cli `
            --bin javy
        $JavyBuildExitCode = $LASTEXITCODE
    } finally {
        if ($null -eq $PreviousReleaseLto) {
            Remove-Item Env:CARGO_PROFILE_RELEASE_LTO -ErrorAction SilentlyContinue
        } else {
            $env:CARGO_PROFILE_RELEASE_LTO = $PreviousReleaseLto
        }
    }
    if ($JavyBuildExitCode -ne 0) { throw "Failed to build Javy v$JavyVersion" }
}

Write-Host "Building the RDR2 Javy host plugin..."
& cargo build --manifest-path $PluginManifest --target $WasmTarget --release --locked
if ($LASTEXITCODE -ne 0) { throw "Failed to build the RDR2 Javy host plugin" }

& $JavyExecutable init-plugin $PluginWasm -o $InitializedPlugin
if ($LASTEXITCODE -ne 0) { throw "Failed to initialize the RDR2 Javy host plugin" }

$Catalog = [IO.File]::ReadAllText((Join-Path $ScriptRoot "src\catalog.js"))
$Trainer = [IO.File]::ReadAllText((Join-Path $ScriptRoot "src\main.js"))
$Entrypoints = [IO.File]::ReadAllText((Join-Path $ScriptRoot "src\entrypoints.js"))
[IO.File]::WriteAllText(
    $CombinedSource,
    "if (!globalThis.__rdr2JavyTrainer) {`n$Catalog`n$Trainer`n}`n$Entrypoints",
    [Text.UTF8Encoding]::new($false)
)

Write-Host "Compiling the JavaScript trainer to WebAssembly..."
& $JavyExecutable build $CombinedSource `
    -C "plugin=$InitializedPlugin" `
    -C "wit=$(Join-Path $ScriptRoot 'exports.wit')" `
    -C "wit-world=trainer" `
    -C "source=compressed" `
    -o $OutputWasm
if ($LASTEXITCODE -ne 0) { throw "Failed to compile the JavaScript trainer" }

New-Item -ItemType Directory -Force $StaticMod | Out-Null
Copy-Item $OutputWasm (Join-Path $StaticMod "main.wasm") -Force

foreach ($Configuration in @("Debug", "Release")) {
    $OutputDirectory = Join-Path $RepositoryRoot "BIN\$Configuration"
    if (-not (Test-Path $OutputDirectory)) { continue }

    $DeployedMod = Join-Path $OutputDirectory "mods\javy-trainer"
    New-Item -ItemType Directory -Force $DeployedMod | Out-Null
    Copy-Item $OutputWasm (Join-Path $DeployedMod "main.wasm") -Force
    Copy-Item (Join-Path $StaticMod "mod.toml") (Join-Path $DeployedMod "mod.toml") -Force
}

$Size = (Get-Item $OutputWasm).Length
Write-Host "Built $OutputWasm ($Size bytes)"
