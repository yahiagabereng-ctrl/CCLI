# Run DIO map checks on TG544 via plink (non-interactive).
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   .\lab\tr400-io-verify-remote.ps1

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$User = "root"
)

$ErrorActionPreference = "Stop"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Plink = "C:\Program Files\PuTTY\plink.exe"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set `$env:CCLI_TG544_PW then re-run."
}
if (-not (Test-Path $Plink)) {
    Write-Error "Install PuTTY plink at: $Plink"
}

function Invoke-Remote {
    param([string]$Cmd)
    & $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $Cmd
}

Write-Host "=== Ping $HostAddr ===" -ForegroundColor Cyan
ping -n 1 $HostAddr | Out-Null

Write-Host "=== SSH verify ===" -ForegroundColor Cyan
Invoke-Remote "echo '=== hostname ==='; cat /proc/sys/kernel/hostname"
Invoke-Remote "echo '=== UCI channel modes ==='; uci get didoservice_v2.channel_1.mode; uci get didoservice_v2.channel_2.mode"
Invoke-Remote "echo '=== lab.yaml io ==='; grep permissive_bypass /etc/ccli/lab.yaml; grep gpio /etc/ccli/lab.yaml"
Invoke-Remote "ubus call dido_v2 set_relay '{`"channel`":1,`"value`":0}'; ubus call dido_v2 set_relay '{`"channel`":1,`"value`":1}'; sleep 1; ubus call dido_v2 set_relay '{`"channel`":1,`"value`":0}'"
Invoke-Remote "echo '=== gd32.status ==='; ubus call dido_v2 gd32.status '{}'"
Invoke-Remote "echo '=== gd32.read_di ==='; ubus call dido_v2 gd32.read_di '{}'"
