# Restart ccli + TesPro IEC61850 stack when northbound Connected but Sent=0.
# Root cause: stale MMS sessions to :102 (IedClientError=5 / connect -3).
# Usage:
#   .\lab\tg544-openwrt\restart-tespro-northbound.ps1
#   .\lab\tg544-openwrt\restart-tespro-northbound.ps1 -Verify

param(
    [string]$DutAddr = "192.168.10.1",
    [switch]$Verify
)

$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$LabEnv = Join-Path $PSScriptRoot "lab-env.ps1"
$VerifyNb = Join-Path $PSScriptRoot "verify-tespro-northbound.ps1"

function Log($msg) { Write-Host "[$(Get-Date -Format 'HH:mm:ss')] $msg" }

if (Test-Path $LabEnv) { . $LabEnv }
if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set CCLI_TG544_PW in lab-env.ps1"
}

Log "=== Restart TesPro northbound stack on DUT ==="

$cmd = @"
killall -9 ccli 2>/dev/null
sleep 2
/usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 &
sleep 3
/etc/init.d/iec61850-mmsd restart
sleep 2
/etc/init.d/iec61850service restart
sleep 8
logread | grep iec61850d | tail -6
"@

& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $cmd 2>&1 |
    ForEach-Object { Log "  $_" }

Log "Done. LuCI Northbound should show Connected and Sent increasing within ~8 s."

if ($Verify) {
    Log "Running verify-tespro-northbound.ps1 ..."
    & $VerifyNb -ListenSeconds 12
}
