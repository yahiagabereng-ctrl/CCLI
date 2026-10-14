# Enable TG544 DIO + restart ccli for full-cycle actuation (Wlim -> DIO1, permissive DIO2).
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   .\lab\enable-dut-gpio-full-cycle.ps1
#   .\lab\enable-dut-gpio-full-cycle.ps1 -HostAddr 192.168.10.1

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$User = "root"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Script = Join-Path $Repo "lab\tg544-openwrt\enable-dut-gpio.sh"

if (-not $env:CCLI_TG544_PW) { throw "Source lab\tg544-openwrt\lab-env.ps1 first" }
if (-not (Test-Path $Plink)) { throw "Install PuTTY plink" }
if (-not (Test-Path $Script)) { throw "Missing $Script" }

Write-Host "=== Enable GPIO on $HostAddr ===" -ForegroundColor Cyan
& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $Script "${User}@${HostAddr}:/tmp/enable-dut-gpio.sh"
& $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "sed -i 's/\r$//' /tmp/enable-dut-gpio.sh; chmod +x /tmp/enable-dut-gpio.sh; sh /tmp/enable-dut-gpio.sh"

Write-Host ""
Write-Host "Full cycle: close permissive (DIO2), Operate Wlim on LAN1, then:" -ForegroundColor Yellow
Write-Host "  ssh: ubus call dido_v2 status | grep -A6 channel.: 1"
Write-Host "  or:  logread | grep -E 'curtail_do=|curtail_on' | tail -5"
