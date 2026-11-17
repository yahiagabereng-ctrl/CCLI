<#
.SYNOPSIS
Signs the usb-test startup APK authorization and copies the verified bundle to USB.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidatePattern('^[A-Z]$')]
    [string]$UsbDriveLetter,

    [ValidateRange(1, 1440)]
    [int]$ValidForMinutes = 240
)

$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
$bundleRoot = 'C:\Users\Administrator\Desktop\Firmware Update\bundles'
$package = Join-Path $repo 'tespro-usb-test\dist\usb-test-1.0.2-r1.apk'
$certificate = 'C:\Users\Administrator\Desktop\Firmware Update\operator-certificate.pem'
$privateKey = 'C:\Users\Administrator\Desktop\Firmware Update\operator-private-key.pem'
$builder = Join-Path $PSScriptRoot 'new-firmware-update-bundle.ps1'
$usbVolume = Get-Volume -DriveLetter $UsbDriveLetter -ErrorAction Stop
if ($usbVolume.DriveType -ne 'Removable') {
    throw "Drive ${UsbDriveLetter}: is not a removable USB volume."
}
foreach ($inputPath in @($package, $certificate, $privateKey, $builder)) {
    if (-not (Test-Path -LiteralPath $inputPath -PathType Leaf)) {
        throw "Missing input file: $inputPath"
    }
}

$env:Path = 'C:\Program Files\Git\usr\bin;' + $env:Path
if (-not (Get-Command openssl -ErrorAction SilentlyContinue)) {
    throw 'OpenSSL is not available.'
}
New-Item -ItemType Directory -Path $bundleRoot -Force | Out-Null
$before = @(Get-ChildItem -LiteralPath $bundleRoot -Directory | ForEach-Object { $_.FullName })

Write-Host 'Enter the operator-private-key.pem passphrase when OpenSSL prompts.'
& $builder -Package $package -OperatorCertificate $certificate `
    -OperatorPrivateKey $privateKey -DeviceId 'CCI-0008CCFD041F' `
    -OutputDirectory $bundleRoot -ValidForMinutes $ValidForMinutes
if (-not $?) { throw 'Bundle generation failed.' }

$created = @(Get-ChildItem -LiteralPath $bundleRoot -Directory |
    Where-Object { $before -notcontains $_.FullName })
if ($created.Count -ne 1) { throw 'Could not identify exactly one newly created bundle.' }
$source = $created[0].FullName
$expected = @('usb-test-1.0.2-r1.apk', 'operator-certificate.pem',
              'update-manifest.json', 'update-manifest.json.sig')
$actual = @(Get-ChildItem -LiteralPath $source -File | ForEach-Object { $_.Name } | Sort-Object)
if (@(Compare-Object ($expected | Sort-Object) $actual).Count -ne 0) {
    throw 'New bundle does not contain exactly the expected four files.'
}

$usbRoot = "${UsbDriveLetter}:\CCI-STARTUP-TEST"
New-Item -ItemType Directory -Path $usbRoot -Force | Out-Null
$destination = Join-Path $usbRoot $created[0].Name
if (Test-Path -LiteralPath $destination) { throw "USB destination already exists: $destination" }
Copy-Item -LiteralPath $source -Destination $destination -Recurse

foreach ($name in $expected) {
    $sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $source $name)).Hash
    $usbHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $destination $name)).Hash
    if ($sourceHash -ne $usbHash) { throw "USB copy mismatch: $name" }
}

Write-Host "Verified USB bundle: $destination" -ForegroundColor Green
$manifest = Get-Content -Raw -LiteralPath (Join-Path $source 'update-manifest.json') | ConvertFrom-Json
$expiry = [DateTimeOffset]::FromUnixTimeSeconds([long]$manifest.expires_epoch)
Write-Host "Expires UTC: $($expiry.ToString('u'))"
