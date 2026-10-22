# Power GNSS on TG544 and confirm sim-manager + chrony refclock (REQ-TIM-002 / T.3.3.4.5).
# YAML already sets mms.gnss_poll_s; this is the required DUT ubus step (antenna must see sky).
#
# Usage (logger PC, LAN2):
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File lab\tg544-openwrt\enable-dut-gnss.ps1
#   powershell -File lab\tg544-openwrt\enable-dut-gnss.ps1 -WaitForFixSec 120

param(
    [string]$HostAddr = "192.168.1.130",
    [int]$WaitForFixSec = 0
)

$ErrorActionPreference = "Stop"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set `$env:CCLI_TG544_PW (source lab-env.ps1)."
}

$startCmd = "ubus call sim-manager gnss_start 2>/dev/null; ubus call sim-manager gnss_check 2>/dev/null; echo '---'; ps w | grep -E 'gnss_chrony_sock|chronyd' | grep -v grep; echo '---'; ubus call sim-manager gnss_get_position 2>/dev/null"

Write-Host "Enabling GNSS on $HostAddr (ubus gnss_start)..."
$out = & $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $startCmd 2>&1
Write-Host ($out | Out-String)

if ($WaitForFixSec -gt 0) {
    Write-Host "Waiting up to ${WaitForFixSec}s for fix..."
    $deadline = (Get-Date).AddSeconds($WaitForFixSec)
    while ((Get-Date) -lt $deadline) {
        Start-Sleep -Seconds 5
        $pos = & $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
            "ubus call sim-manager gnss_get_position 2>/dev/null" 2>&1 | Out-String
        if ($pos -match '"fix":\s*true') {
            Write-Host "GNSS fix acquired."
            Write-Host $pos
            break
        }
    }
}

Write-Host ""
Write-Host "Next: verify time quality (after fix + ~15s ccli poll):"
Write-Host "  powershell -File lab\tg544-openwrt\verify-gnss-time-quality.ps1 -HostAddr 192.168.10.1"
Write-Host "Physical: GNSS antenna on modem; clear sky; 1-5 min typical cold start."
