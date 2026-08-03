param(
    [string]$DestinationRoot
)

$ErrorActionPreference = 'Stop'

$scriptRoot = $PSScriptRoot
$repoRoot = Split-Path -Parent (Split-Path -Parent $scriptRoot)
if ([string]::IsNullOrWhiteSpace($DestinationRoot)) {
    $DestinationRoot = Join-Path $repoRoot 'BUILD\dotnet-toolchain'
}

$sdkName = 'wasi-sdk-29.0-x86_64-windows'
$sdkPath = Join-Path $DestinationRoot $sdkName
$archivePath = Join-Path $DestinationRoot "$sdkName.tar.gz"
$url = 'https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-29/wasi-sdk-29.0-x86_64-windows.tar.gz'
$expectedSha256 = '6c19b820577486f00332ad8d04ac506da67b0892316b8d485371a58cbf216dee'

if (Test-Path -LiteralPath (Join-Path $sdkPath 'bin\clang.exe')) {
    Write-Output $sdkPath
    exit 0
}

New-Item -ItemType Directory -Path $DestinationRoot -Force | Out-Null
if (-not (Test-Path -LiteralPath $archivePath)) {
    & curl.exe --fail --location --retry 5 --continue-at - `
        --output $archivePath $url
    if ($LASTEXITCODE -ne 0) { throw "Failed to download WASI SDK: $LASTEXITCODE" }
}

$actualSha256 = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actualSha256 -ne $expectedSha256) {
    throw "WASI SDK checksum mismatch: expected $expectedSha256, got $actualSha256"
}

& tar.exe -xzf $archivePath -C $DestinationRoot
if ($LASTEXITCODE -ne 0) { throw "Failed to extract WASI SDK: $LASTEXITCODE" }
if (-not (Test-Path -LiteralPath (Join-Path $sdkPath 'bin\clang.exe'))) {
    throw "WASI SDK archive did not produce $sdkPath"
}

Write-Output $sdkPath
