# Deploy Annex M SMS poller + trip scripts to TG544 DUT (plink/base64 — no SFTP).
param(
    [string]$DutAddr = "192.168.10.1"
)

$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$LabDir = Join-Path $Repo "lab\tg544-openwrt"
$RemoteDir = "/tmp/ccli-annex-m-sms"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set env:CCLI_TG544_PW"
}

function Push-DutFile([string]$LocalPath, [string]$RemotePath) {
    $body = (Get-Content $LocalPath -Raw) -replace "`r`n", "`n"
    $b64 = [Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($body))
    & $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
        "mkdir -p $RemoteDir && echo $b64 | openssl base64 -d -A > $RemotePath" 2>&1 | ForEach-Object { Write-Host $_ }
}

Write-Host "=== Deploy Annex M SMS to $DutAddr ==="

$scripts = @(
    "annex-m-trip.sh",
    "annex-m-sms-poller.sh",
    "annex-m-sms-number.sh",
    "annex-m-sms-send.sh",
    "annex-m-sms-reply.sh",
    "annex-m-sms-install.sh"
)
foreach ($f in $scripts) {
    $local = Join-Path $LabDir $f
    if (-not (Test-Path $local)) { Write-Error "Missing $local" }
    Push-DutFile $local "$RemoteDir/$f"
}

$factory = Join-Path $Repo "lab\tr400-dio-factory.cmds"
if (-not (Test-Path $factory)) { Write-Error "Missing $factory" }
Push-DutFile $factory "$RemoteDir/tr400-dio-factory.cmds"

$cmd = "cd $RemoteDir && sed -i 's/\r$//' *.sh *.cmds && chmod +x *.sh && sh tr400-dio-factory.cmds && sh annex-m-sms-install.sh && /usr/sbin/annex-m-sms-poller.sh && tail -5 /tmp/annex-m-sms.log 2>/dev/null || true"

& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $cmd 2>&1 | ForEach-Object { Write-Host $_ }

Write-Host ""
Write-Host "Done. On DUT: annex-m-sms-number.sh"
Write-Host "SMS from mobile: TRIP  or  CLEAR"
