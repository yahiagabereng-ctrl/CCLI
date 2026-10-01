# Verify TesPro IEC 61850 packages and service on TG544 via SSH.
# Usage:
#   $env:CCLI_TG544_PW = "root-password"
#   .\lab\tg544-openwrt\check-tespro-61850-remote.ps1
# Optional (LAN2 Eth_B):
#   .\lab\tg544-openwrt\check-tespro-61850-remote.ps1 -HostAddr 192.168.1.130

param(
    [string]$HostAddr = "192.168.10.1",
    [string]$User = "root"
)

$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set `$env:CCLI_TG544_PW then re-run."
    exit 1
}
if (-not (Test-Path $Plink)) {
    Write-Error "Install PuTTY plink at: $Plink"
    exit 1
}

$remote = @'
echo "=== TesPro IEC 61850 health check"
date
echo "--- apk packages (TesproOS) ---"
apk list -I 2>/dev/null | grep -E 'open62541|iec61850' || opkg list-installed 2>/dev/null | grep -E 'open62541|iec61850' || echo "(none)"
echo "--- service status ---"
/etc/init.d/iec61850-mmsd status 2>&1
/etc/init.d/iec61850service status 2>&1
echo "--- processes ---"
ps w | grep -E '61850|mmsd' | grep -v grep || echo "(no 61850/mmsd process)"
echo "--- init scripts ---"
ls -la /etc/init.d/ 2>/dev/null | grep -E '61850|mms' || echo "(no init scripts matched)"
echo "--- libraries ---"
ls -la /usr/lib/libiec61850* /usr/lib/libopen62541* 2>/dev/null || echo "(libs not found)"
echo "--- listen ports ---"
netstat -tln 2>/dev/null | grep -E '3782|:102|:2404' || ss -tlnp 2>/dev/null | grep -E '3782|:102|:2404' || echo "(none on 3782/102/2404)"
echo "--- ccli coexistence ---"
pgrep -a ccli || echo "(ccli not running)"
echo "--- opkg lock ---"
ls -la /var/lock/opkg.lock /var/lib/opkg/lock 2>/dev/null || echo "(no lock files)"
echo "=== end ==="
'@

& $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $remote
