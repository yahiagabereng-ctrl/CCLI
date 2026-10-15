# Verify how ccli is installed on DUT (APK vs raw binary) and identity vs Oct-08 PASS.
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\verify-dut-ccli-install.ps1 -HostAddr 192.168.1.130

param(
    [string]$HostAddr = "192.168.1.130"
)

$ErrorActionPreference = "Stop"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$pw = $env:CCLI_TG544_PW
if (-not $pw) { throw "Source lab-env.ps1" }

$remote = @'
echo === NETWORK ===
ip -4 addr show | grep 'inet ' | grep -E '192.168.(10|1|30)'
echo === CCLI IDENTITY ===
/usr/sbin/ccli --version 2>/dev/null || true
sha256sum /usr/sbin/ccli 2>/dev/null || true
ls -la /usr/sbin/ccli
echo === PACKAGE (apk) ===
command -v apk >/dev/null && apk list-installed 2>/dev/null | grep -i ccli || echo 'no ccli apk line'
opkg list-installed 2>/dev/null | grep -i ccli || true
echo === STRINGS (session/AARE) ===
strings /usr/sbin/ccli 2>/dev/null | grep 'iso session CONNECT' | head -2
strings /usr/sbin/ccli 2>/dev/null | grep 'AARE responder auth' | head -2
strings /usr/sbin/ccli 2>/dev/null | grep 'P3-07-AARE' | head -2
echo === DEPLOY MANIFEST ===
head -15 /etc/ccli/DEPLOY_MANIFEST.md 2>/dev/null || echo 'no manifest'
echo === MMS LISTEN ===
netstat -tlnp 2>/dev/null | grep 3782 || true
echo === RECENT CONNECT LOG ===
logread -e ccli 2>/dev/null | grep -E 'CONNECT count|iso session|ACSE auth|handshake failed|closed gracefully|listening on' | tail -20
'@ -replace "`r`n", "; "

Write-Host "DUT ccli install check on $HostAddr ..." -ForegroundColor Cyan
& $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch $remote

Write-Host "`nExpected Oct-08 PASS (evidence):" -ForegroundColor Yellow
Write-Host "  ccli-0.1.0-r51.apk OR binary with --version showing r51 / P3-07-AARE-SIGN-RAW-GT"
Write-Host "  strings: iso session CONNECT, AARE responder auth [10]EXTERNAL"
Write-Host "`nRepo note: no ccli-0.1.0-r51.apk on disk; git ccli-bin.r51 at 4c73add reports r30 in --version."
Write-Host "deploy-ccli-session-fix.ps1 copies ccli-bin (NOT apk add)."
