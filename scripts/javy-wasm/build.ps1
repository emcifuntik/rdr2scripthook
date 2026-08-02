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
$JavyExecutable = Join-Path $JavySource "target\release\javy.exe"
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

if ($RebuildJavy -or $PatchApplied -or -not (Test-Path $JavyExecutable)) {
    Write-Host "Building the pinned Javy v$JavyVersion compiler..."
    & cargo build --manifest-path (Join-Path $JavySource "Cargo.toml") --release --locked --bin javy
    if ($LASTEXITCODE -ne 0) { throw "Failed to build Javy v$JavyVersion" }
}

$InstalledTargets = & rustup target list --installed
if ($InstalledTargets -notcontains $WasmTarget) {
    & rustup target add $WasmTarget
    if ($LASTEXITCODE -ne 0) { throw "Failed to install Rust target $WasmTarget" }
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
