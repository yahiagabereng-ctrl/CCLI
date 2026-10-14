# Live 20s DIO check while you press/hold permissive (DIO2). Listen for relay click at end.
# Usage: . .\lab\tg544-openwrt\lab-env.ps1; .\lab\monitor-button-relay-20s.ps1

param([string]$HostAddr = "192.168.1.130")

$ErrorActionPreference = "Stop"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Repo = Split-Path -Parent $PSScriptRoot
$Sh = Join-Path $Repo "lab\tr400-button-relay-monitor-20s.sh"
if (-not $env:CCLI_TG544_PW) { throw "Source lab\tg544-openwrt\lab-env.ps1" }

Write-Host ""
Write-Host ">>> START NOW: press and HOLD permissive button on DIO2 for the next 20 seconds <<<" -ForegroundColor Yellow
Write-Host ">>> Watch relay on DIO1; script ends with one ubus click (ch1 ON then OFF) <<<" -ForegroundColor Yellow
Write-Host ""

& "C:\Program Files\PuTTY\pscp.exe" -scp -batch -pw $env:CCLI_TG544_PW -hostkey $hk $Sh "root@${HostAddr}:/tmp/monitor20.sh"
& "C:\Program Files\PuTTY\plink.exe" -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $hk -batch `
    "sed -i 's/\r$//' /tmp/monitor20.sh; sh /tmp/monitor20.sh"

Write-Host ""
Write-Host "Expect: di[1]=1 and note closed/OK while button held; ch1=1 only if PF2/Wlim also commands curtail." -ForegroundColor Cyan
