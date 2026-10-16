# Regenerate Oct-08-style connect PKI (single client.key for transport + MMS) and verify.
# Output: apps/ccli/config/tls/p3_07_connect/  (does NOT overwrite EJBCA apps/ccli/config/tls/)
#
# Usage:
#   powershell -File scripts\publish-p3-07-connect-profile.ps1

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Dest = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"
$Gen = Join-Path $Repo "apps\ccli\config\tls\gen_lab_pki.py"

Write-Host "P3-07 connect PKI -> $Dest" -ForegroundColor Cyan
python $Gen --dest $Dest
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& (Join-Path $Repo "scripts\verify-tsp-tls-key-match.ps1") -TlsDir $Dest -Profile P3-07
& (Join-Path $Repo "scripts\sync-tsp-connect-min-handoff.ps1")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Oct-08 TSP: client_tsp.key must match client.pem (same as client.key)
$cm = (openssl x509 -noout -modulus -in (Join-Path $Dest "client.pem") | openssl md5).Trim()
$km = (openssl rsa -noout -modulus -in (Join-Path $Dest "client_tsp.key") | openssl md5).Trim()
if ($cm -ne $km) {
    Write-Host "FAIL Oct-08 profile: client.pem must match client_tsp.key" -ForegroundColor Red
    exit 1
}
Write-Host "PASS Oct-08 single-key operator profile (client.pem + client_tsp.key same modulus)" -ForegroundColor Green

$readme = @"
# P3-07 Connect session PKI (gen_lab_pki)

**Not** the EJBCA unified-cert tree. Use only while debugging TestPro Connect (Oct-08 PASS profile).

| TSP slot | Files |
|----------|--------|
| CA | root_CA.pem |
| Transport | client_tls.pem + client_tsp.key |
| MMS | client.pem + client_tsp.key (**same key both slots**) |

DUT yaml: ``lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml`` — **crl=off**, no revoked allowlist.

Regenerate: ``powershell -File scripts\publish-p3-07-connect-profile.ps1``
"@
Set-Content -Path (Join-Path $Dest "README.md") -Value $readme -Encoding UTF8
Write-Host "OK: $Dest"
