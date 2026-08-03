param(
    [ValidateRange(0, 100)]
    [int]$Warmup = 2,
    [ValidateRange(1, 1000)]
    [int]$Samples = 5,
    [ValidateRange(1, 1000)]
    [int]$FuelSamples = 1,
    [switch]$NoBuild,
    [switch]$SkipFuel
)

$ErrorActionPreference = 'Stop'

$run = Join-Path $PSScriptRoot 'run.ps1'
$coverage = Join-Path $PSScriptRoot 'lua-bytecode-coverage.ps1'

Write-Host 'Verifying Lua bytecode coverage...'
& $coverage -FailOnMissing

Write-Host 'Running differential correctness smoke test...'
$smoke = @{
    Backend = @('lua-native', 'lua-wasmtime')
    Warmup = 0
    Samples = 1
    Fuel = 'off'
}
if ($NoBuild) { $smoke.NoBuild = $true }
& $run @smoke

Write-Host 'Running stable native-versus-Wasmtime measurements...'
& $run -Backend lua-native,lua-wasmtime -Warmup $Warmup `
    -Samples $Samples -Fuel off -NoBuild

if (-not $SkipFuel) {
    Write-Host 'Running Wasmtime fuel audit...'
    & $run -Backend lua-wasmtime -Warmup 1 -Samples $FuelSamples `
        -Fuel on -NoBuild
}

Write-Host 'Lua stress matrix completed successfully.'
