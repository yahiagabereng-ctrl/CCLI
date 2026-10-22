# Restore P3-07 Connect TLS on DUT (matches TestPro field pack pins in LAB_ACCESS_FREEZE.md).
# Does not replace ccli binary.
#
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File lab\tg544-openwrt\push-p3-07-tls-to-dut.ps1

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$User = "root"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$TlsSrc = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"

if (-not $env:CCLI_TG544_PW) { throw "Source lab-env.ps1" }
if (-not (Test-Path $TlsSrc)) { throw "Missing $TlsSrc" }

$files = @(
    "root_CA.pem", "server.pem", "server_tls.pem", "server.key", "server_acse.key",
    "server_combined.pem", "server_combined.key",
    "client.pem", "client_tls.pem", "client.key", "client_tsp.key",
    "viewer.pem", "viewer_tls.pem", "viewer.key",
    "revoked.pem", "revoked_tls.pem", "revoked.key"
)

Write-Host "=== Push P3-07 TLS -> $HostAddr ===" -ForegroundColor Cyan
& $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch "mkdir -p /tmp/p3_07.tls /etc/ccli/tls"
foreach ($f in $files) {
    $src = Join-Path $TlsSrc $f
    if (Test-Path $src) {
        & $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $src "${User}@${HostAddr}:/tmp/p3_07.tls/$f" | Out-Null
    }
}

& $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "cp -f /tmp/p3_07.tls/* /etc/ccli/tls/; rm -f /etc/ccli/tls/lab_crl.pem /etc/ccli/tls/client_acse.key; md5sum /etc/ccli/tls/root_CA.pem /etc/ccli/tls/client_tls.pem /etc/ccli/tls/client.pem"

Write-Host "Done. Restart ccli after pushing matching yaml (product_tr400_p3_07_connect.yaml)." -ForegroundColor Green
