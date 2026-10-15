# DUT preflight for Test Suite Pro Connect (single ccli, :3782, TesPro stack off).
# Usage: . .\lab\tg544-openwrt\lab-env.ps1; powershell -File scripts\dut-mms-tsp-preflight.ps1

param(
    # PC-B (Masomeh / LAN1): 192.168.10.1 · logger PC (LAN2): 192.168.1.130
    [string]$HostAddr = "192.168.10.1"
)

$ErrorActionPreference = "Stop"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$pw = $env:CCLI_TG544_PW
if (-not $pw) { throw "Set CCLI_TG544_PW (source lab-env.ps1)" }

$remote = @'
/etc/init.d/iec61850service stop 2>/dev/null; /etc/init.d/iec61850service disable 2>/dev/null
/etc/init.d/iec61850-mmsd stop 2>/dev/null; /etc/init.d/iec61850-mmsd disable 2>/dev/null
/etc/init.d/iec61850 stop 2>/dev/null; /etc/init.d/iec61850server stop 2>/dev/null
killall iec61850d 2>/dev/null; killall iec61850-mmsd 2>/dev/null
/etc/init.d/ccli stop 2>/dev/null; killall ccli 2>/dev/null; sleep 2
/etc/init.d/ccli start; sleep 8
echo PROCS
ps w | grep '[c]cli'
echo IEC
ps w | grep iec61850 | grep -v grep || true
echo LISTEN
netstat -tlnp | grep 3782 || true
if netstat -tlnp 2>/dev/null | grep 3782 | grep -q '/ccli'; then echo PREFLIGHT_PASS_3782_CCLI; else echo PREFLIGHT_FAIL_3782_NOT_CCLI; fi
echo MD5
md5sum /etc/ccli/tls/root_CA.pem /etc/ccli/tls/client_tls.pem /etc/ccli/tls/client.pem
ccli --version | head -1
'@ -replace "`r`n", "; "

Write-Host "DUT MMS TSP preflight on $HostAddr ..." -ForegroundColor Cyan
$out = & $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch $remote 2>&1 | Out-String
Write-Host $out
if ($out -notmatch 'PREFLIGHT_PASS_3782_CCLI') {
    Write-Host "FAIL: 192.168.10.1:3782 is not owned by ccli (see LISTEN line - ubus/OEM or stale ccli)." -ForegroundColor Red
    exit 1
}
Write-Host "PASS: single ccli listening on :3782" -ForegroundColor Green
