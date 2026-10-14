# DUT preflight for Test Suite Pro Connect (single ccli, :3782, TesPro stack off).
# Usage: . .\lab\tg544-openwrt\lab-env.ps1; powershell -File scripts\dut-mms-tsp-preflight.ps1

param(
    [string]$HostAddr = "192.168.1.130"
)

$ErrorActionPreference = "Stop"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$pw = $env:CCLI_TG544_PW
if (-not $pw) { throw "Set CCLI_TG544_PW (source lab-env.ps1)" }

$remote = "/etc/init.d/iec61850service stop; /etc/init.d/iec61850service disable; /etc/init.d/iec61850-mmsd stop 2>/dev/null; /etc/init.d/iec61850-mmsd disable 2>/dev/null; killall iec61850d; /etc/init.d/ccli stop; killall ccli; sleep 2; /etc/init.d/ccli start; sleep 2; echo PROCS; ps w | grep ccli | grep -v grep; echo IEC; ps w | grep iec61850 | grep -v grep; echo LISTEN; netstat -tlnp | grep 3782; echo MD5; md5sum /etc/ccli/tls/root_CA.pem /etc/ccli/tls/client_tls.pem /etc/ccli/tls/client.pem; ccli --version | head -1"

Write-Host "DUT MMS TSP preflight on $HostAddr ..." -ForegroundColor Cyan
& $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch $remote
