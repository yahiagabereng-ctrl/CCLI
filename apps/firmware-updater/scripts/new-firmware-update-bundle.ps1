<#
.SYNOPSIS
Creates an offline, one-time TesPro USB update authorization bundle.

.DESCRIPTION
The operator certificate is public and is copied to the USB bundle. The
operator private key is used only to sign update-manifest.json and is never
copied to the output directory.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string[]]$Package,

    [Parameter(Mandatory)]
    [string]$OperatorCertificate,

    [Parameter(Mandatory)]
    [string]$OperatorPrivateKey,

    [Parameter(Mandatory)]
    [ValidatePattern('^[A-Za-z0-9_.:-]{3,96}$')]
    [string]$DeviceId,

    [Parameter(Mandatory)]
    [string]$OutputDirectory,

    [ValidateRange(1, 1440)]
    [int]$ValidForMinutes = 60,

    [ValidatePattern('^[A-Za-z0-9_.:-]{3,64}$')]
    [string]$OperatorRole = 'firmware-update',

    [ValidatePattern('^[A-Z]$')]
    [string]$UsbDriveLetter = '',

    [ValidatePattern('^[A-Za-z0-9_.-]+$')]
    [string]$UsbUpdateDirectory = 'updates',

    [System.Security.SecureString]$PrivateKeyPassphrase
)

$ErrorActionPreference = 'Stop'

if ($PSVersionTable.PSVersion.Major -lt 7) {
    throw 'Use PowerShell 7 or apps/firmware-updater/scripts/new-bundle.sh from WSL.'
}
$verifier = (& wsl wslpath -a -u (Join-Path $PSScriptRoot 'verify-apk.sh')).Trim()
if ($LASTEXITCODE -ne 0) { throw 'WSL path conversion failed.' }
foreach ($apk in $Package) {
    $linuxApk = (& wsl wslpath -a -u (Resolve-Path -LiteralPath $apk).Path).Trim()
    if ($LASTEXITCODE -ne 0) { throw 'WSL APK path conversion failed.' }
    & wsl bash $verifier $linuxApk
    if ($LASTEXITCODE -ne 0) { throw "Manufacturer APK signature verification failed: $apk" }
}

$certificatePath = (Resolve-Path -LiteralPath $OperatorCertificate).Path
$privateKeyPath = (Resolve-Path -LiteralPath $OperatorPrivateKey).Path
$packagePaths = @($Package | ForEach-Object { (Resolve-Path -LiteralPath $_).Path })

# Keep bundle creation self-contained.  .NET reads PEM certificates and RSA
# private keys directly, so no host-specific OpenSSL installation is required.
try {
    # OpenSSL exports may have harmless "Bag Attributes" before the PEM
    # blocks.  Extract only the encoded blocks so both OpenSSL and .NET style
    # exports work without rewriting the operator material.
    $certificatePem = [regex]::Match(
        [IO.File]::ReadAllText($certificatePath),
        '-----BEGIN CERTIFICATE-----[\s\S]+?-----END CERTIFICATE-----').Value
    $privateKeyPem = [regex]::Match(
        [IO.File]::ReadAllText($privateKeyPath),
        '-----BEGIN (?:ENCRYPTED |RSA )?PRIVATE KEY-----[\s\S]+?-----END (?:ENCRYPTED |RSA )?PRIVATE KEY-----').Value
    if ([string]::IsNullOrWhiteSpace($certificatePem) -or [string]::IsNullOrWhiteSpace($privateKeyPem)) {
        throw 'A PEM certificate and an unencrypted PEM private-key block are required.'
    }
    $certificate = [System.Security.Cryptography.X509Certificates.X509Certificate2]::CreateFromPem($certificatePem)
    if ($privateKeyPem.StartsWith('-----BEGIN ENCRYPTED PRIVATE KEY-----')) {
        if ($null -eq $PrivateKeyPassphrase) {
            $PrivateKeyPassphrase = Read-Host -Prompt 'Operator private-key passphrase' -AsSecureString
        }
        $passphraseBstr = [Runtime.InteropServices.Marshal]::SecureStringToBSTR($PrivateKeyPassphrase)
        try {
            $passphrase = [Runtime.InteropServices.Marshal]::PtrToStringBSTR($passphraseBstr)
            $signingCertificate = [System.Security.Cryptography.X509Certificates.X509Certificate2]::CreateFromEncryptedPem(
                $certificatePem, $privateKeyPem, $passphrase)
        } finally {
            [Runtime.InteropServices.Marshal]::ZeroFreeBSTR($passphraseBstr)
            $passphrase = $null
        }
    } else {
        $signingCertificate = [System.Security.Cryptography.X509Certificates.X509Certificate2]::CreateFromPem(
            $certificatePem, $privateKeyPem)
    }
    $privateKey = [System.Security.Cryptography.X509Certificates.RSACertificateExtensions]::GetRSAPrivateKey($signingCertificate)
    $publicKey = [System.Security.Cryptography.X509Certificates.RSACertificateExtensions]::GetRSAPublicKey($certificate)
    if ($null -eq $privateKey -or $null -eq $publicKey) {
        throw 'The certificate and private key must use RSA.'
    }
} catch {
    throw "Could not load the RSA operator certificate/private key from PEM: $($_.Exception.Message)"
}

if ($packagePaths.Count -eq 0 -or $packagePaths.Count -gt 32) {
    throw 'The bundle must contain between 1 and 32 APK packages.'
}

$packageNames = @{}
foreach ($path in $packagePaths) {
    $name = [IO.Path]::GetFileName($path)
    if ($name -notmatch '^[A-Za-z0-9_.:-]+\.apk$') {
        throw "Unsafe APK filename: $name"
    }
    if ($packageNames.ContainsKey($name)) {
        throw "Duplicate APK filename: $name"
    }
    $packageNames[$name] = $true
}

$basicConstraints = $certificate.Extensions | Where-Object { $_ -is [System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension] } | Select-Object -First 1
$keyUsage = $certificate.Extensions | Where-Object { $_ -is [System.Security.Cryptography.X509Certificates.X509KeyUsageExtension] } | Select-Object -First 1
if ($null -eq $basicConstraints -or $basicConstraints.CertificateAuthority -or $null -eq $keyUsage -or
    -not ($keyUsage.KeyUsages -band [System.Security.Cryptography.X509Certificates.X509KeyUsageFlags]::DigitalSignature)) {
    throw 'The operator certificate must be a non-CA certificate with digitalSignature key usage.'
}
if ($certificate.Subject -notmatch '(^|,)\s*OU=Firmware Update Operators(,|$)') {
    throw 'The operator certificate must contain OU=Firmware Update Operators.'
}
if ($certificate.NotAfter.ToUniversalTime() -le [DateTime]::UtcNow.AddMinutes($ValidForMinutes)) {
    throw 'The operator certificate expires before this authorization does.'
}

$transactionId = [guid]::NewGuid().ToString()
$bundleDirectory = Join-Path ([IO.Path]::GetFullPath($OutputDirectory)) "update-$transactionId"
New-Item -ItemType Directory -Path $bundleDirectory -Force | Out-Null

$packageManifest = @()
foreach ($path in $packagePaths) {
    $name = [IO.Path]::GetFileName($path)
    Copy-Item -LiteralPath $path -Destination (Join-Path $bundleDirectory $name)
    $packageManifest += [ordered]@{
        file = $name
        sha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $path).Hash.ToLowerInvariant()
    }
}

$now = [DateTimeOffset]::UtcNow
$manifest = [ordered]@{
    schema_version = 1
    transaction_id = $transactionId
    device_id = $DeviceId
    operator_role = $OperatorRole
    not_before_epoch = $now.ToUnixTimeSeconds()
    expires_epoch = $now.AddMinutes($ValidForMinutes).ToUnixTimeSeconds()
    packages = $packageManifest
}

$manifestPath = Join-Path $bundleDirectory 'update-manifest.json'
$signaturePath = Join-Path $bundleDirectory 'update-manifest.json.sig'
$bundleCertificatePath = Join-Path $bundleDirectory 'operator-certificate.pem'
$utf8NoBom = New-Object System.Text.UTF8Encoding($false)
[IO.File]::WriteAllText($manifestPath, ($manifest | ConvertTo-Json -Depth 5), $utf8NoBom)
Copy-Item -LiteralPath $certificatePath -Destination $bundleCertificatePath

$manifestBytes = [IO.File]::ReadAllBytes($manifestPath)
$signature = $privateKey.SignData(
    $manifestBytes,
    [System.Security.Cryptography.HashAlgorithmName]::SHA256,
    [System.Security.Cryptography.RSASignaturePadding]::Pkcs1)
[IO.File]::WriteAllBytes($signaturePath, $signature)
if (-not $publicKey.VerifyData(
        $manifestBytes,
        $signature,
        [System.Security.Cryptography.HashAlgorithmName]::SHA256,
        [System.Security.Cryptography.RSASignaturePadding]::Pkcs1)) {
    throw 'The private key does not match the operator certificate.'
}

Write-Host "USB update bundle created:" -ForegroundColor Green
Write-Host $bundleDirectory
Write-Host "Transaction: $transactionId"
Write-Host "Device:      $DeviceId"
Write-Host "Expires UTC: $($now.AddMinutes($ValidForMinutes).ToString('u'))"
if ($UsbDriveLetter) {
    $volume = Get-Volume -DriveLetter $UsbDriveLetter -ErrorAction Stop
    if ($volume.DriveType -ne 'Removable') {
        throw "Drive ${UsbDriveLetter}: is not a removable USB volume."
    }
    $usbParent = Join-Path "${UsbDriveLetter}:\" $UsbUpdateDirectory
    New-Item -ItemType Directory -Path $usbParent -Force | Out-Null
    $usbBundle = Join-Path $usbParent (Split-Path -Leaf $bundleDirectory)
    if (Test-Path -LiteralPath $usbBundle) {
        throw "USB destination already exists: $usbBundle"
    }
    Copy-Item -LiteralPath $bundleDirectory -Destination $usbBundle -Recurse
    Get-ChildItem -LiteralPath $bundleDirectory -File | ForEach-Object {
        $sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
        $usbHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $usbBundle $_.Name)).Hash
        if ($sourceHash -ne $usbHash) {
            throw "USB copy mismatch: $($_.Name)"
        }
    }
    Write-Host "Verified USB bundle: $usbBundle" -ForegroundColor Green
} else {
    Write-Host 'Copy this entire update-* directory to the USB drive, or pass -UsbDriveLetter.'
}
