# Windows entry point. Pass Linux/WSL paths for certificate/key/report arguments.
$ErrorActionPreference = 'Stop'
$clientArgs = @($args)
$profileName = 'client'
if ($clientArgs.Count -ge 2 -and $clientArgs[0] -eq '--profile') {
    $profileName = $clientArgs[1]
    if ($profileName -notmatch '^[a-zA-Z0-9_-]+$') { throw 'Invalid local profile name' }
    $clientArgs = @($clientArgs | Select-Object -Skip 2)
}
$localScript = Join-Path $PSScriptRoot 'client.py'
$linuxScript = (& wsl.exe wslpath -a $localScript).Trim()
if ($LASTEXITCODE -ne 0) { throw 'Cannot resolve client path in WSL' }
$profileArgs = @()
$profilePath = Join-Path $PSScriptRoot ($profileName + '.local.json')
if (Test-Path -LiteralPath $profilePath) {
    $profileData = Get-Content -LiteralPath $profilePath -Raw | ConvertFrom-Json
    $profileArgs = @('--identity-cert', $profileData.identity_cert, '--identity-key', $profileData.identity_key)
    foreach ($field in @('tls_cert', 'tls_key', 'auth_mode', 'mms_ca')) {
        if ($profileData.$field) {
            $profileArgs += ('--' + $field.Replace('_', '-'))
            $profileArgs += [string]$profileData.$field
        }
    }
} elseif ($profileName -ne 'client') {
    throw "Missing local credential profile: $profilePath"
}
& wsl.exe python3 $linuxScript @profileArgs @clientArgs
exit $LASTEXITCODE
