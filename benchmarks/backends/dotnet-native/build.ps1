param(
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release'
)

$ErrorActionPreference = 'Stop'

$backendRoot = $PSScriptRoot
$benchmarkRoot = Split-Path -Parent (Split-Path -Parent $backendRoot)
$repoRoot = Split-Path -Parent $benchmarkRoot
$project = Join-Path $backendRoot 'DotNetNativeBench.csproj'
$nugetConfig = Join-Path $repoRoot 'scripts\dotnet-wasm\NuGet.Config'
$publishDirectory = Join-Path $repoRoot "BUILD\benchmarks\dotnet-native\$Configuration"
$artifactDirectory = Join-Path $benchmarkRoot 'artifacts\dotnet-nativeaot'
$artifact = Join-Path $artifactDirectory 'dotnet-native-bench.exe'

Push-Location $benchmarkRoot
try {
    & dotnet restore $project --configfile $nugetConfig --runtime win-x64
    if ($LASTEXITCODE -ne 0) { throw "NativeAOT restore failed: $LASTEXITCODE" }

    & dotnet publish $project --configuration $Configuration `
        --runtime win-x64 --no-restore --output $publishDirectory
    if ($LASTEXITCODE -ne 0) { throw "NativeAOT publish failed: $LASTEXITCODE" }
} finally {
    Pop-Location
}

New-Item -ItemType Directory -Path $artifactDirectory -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $publishDirectory 'dotnet-native-bench.exe') `
    -Destination $artifact -Force
Write-Host "Built $artifact"
