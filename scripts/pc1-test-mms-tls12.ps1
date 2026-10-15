# PC1 preflight: mutual TLS to DUT :3782 using TLS 1.2 only (matches PIXIT / 62351-3).
# Usage:
#   powershell -File scripts\pc1-test-mms-tls12.ps1
#   powershell -File scripts\pc1-test-mms-tls12.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls

param(
    [string]$TlsDir = "C:\CCLI_TSP_FIELD_PACK\tls",
    [string]$DutHost = "192.168.10.1",
    [int]$Port = 3782
)

$ErrorActionPreference = "Stop"
if (-not (Test-Path $TlsDir)) {
    throw "Missing TlsDir: $TlsDir (run setup-pc1-testpro-from-git.ps1 first)"
}

$cert = Join-Path $TlsDir "client_tls.pem"
$key = Join-Path $TlsDir "client_tsp.key"
$ca = Join-Path $TlsDir "root_CA.pem"
foreach ($f in @($cert, $key, $ca)) {
    if (-not (Test-Path $f)) { throw "Missing $f" }
}

Write-Host "OpenSSL TLS 1.2 mutual auth -> ${DutHost}:${Port}" -ForegroundColor Cyan
$out = cmd /c "openssl s_client -connect ${DutHost}:${Port} -cert `"$cert`" -key `"$key`" -CAfile `"$ca`" -tls1_2 -brief <nul 2>&1"
Write-Host $out

if ($out -match "CONNECTION ESTABLISHED" -and $out -match "TLSv1\.2") {
    Write-Host "PASS: TLS 1.2 handshake" -ForegroundColor Green
    exit 0
}
if ($out -match "CONNECTION ESTABLISHED") {
    Write-Host "WARN: connected but not TLSv1.2 in output - check DUT runs ccli r56+ (max TLS 1.2)" -ForegroundColor Yellow
    exit 1
}
Write-Host "FAIL: no TLS connection" -ForegroundColor Red
exit 1
