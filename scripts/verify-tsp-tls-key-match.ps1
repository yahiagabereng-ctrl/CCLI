# Verify EJBCA dual-cert key pairing for Test Suite Pro (62351-4 G.2).
# FAIL if client.pem modulus != client_acse.key (common cause of
# "Initiate Response Failed: Transport connection closed").
#
# Usage:
#   powershell -File scripts\verify-tsp-tls-key-match.ps1
#   powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls

param(
    [string]$TlsDir = ""
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
if (-not $TlsDir) {
    $TlsDir = Join-Path $Repo "apps\ccli\config\tls"
}

function Get-ModulusMd5([string]$Path, [string]$Kind) {
    if (-not (Test-Path $Path)) { return $null }
    if ($Kind -eq "cert") {
        $out = & openssl x509 -in $Path -noout -modulus 2>$null
    } else {
        $out = & openssl rsa -in $Path -noout -modulus 2>$null
    }
    if (-not $out) { return $null }
    ($out | openssl md5).Trim()
}

Write-Host "TSP TLS key/cert pairing: $TlsDir" -ForegroundColor Cyan

$checks = @(
    @{ Label = "Transport Cert A"; Cert = "client_tls.pem"; Key = "client_tsp.key"; AltKey = "client.key" },
    @{ Label = "MMS Cert B"; Cert = "client.pem"; Key = "client_acse.key"; AltKey = "" },
    @{ Label = "Revoked Cert A"; Cert = "revoked_tls.pem"; Key = "revoked_tsp.key"; AltKey = "revoked.key" },
    @{ Label = "Revoked Cert B"; Cert = "revoked.pem"; Key = "revoked_acse.key"; AltKey = "" }
)

$fail = 0
foreach ($c in $checks) {
    $certPath = Join-Path $TlsDir $c.Cert
    $keyPath = Join-Path $TlsDir $c.Key
    if (-not (Test-Path $certPath)) {
        Write-Host "SKIP $($c.Label): missing $($c.Cert)" -ForegroundColor DarkGray
        continue
    }
    $cm = Get-ModulusMd5 $certPath "cert"
    $km = Get-ModulusMd5 $keyPath "key"
    if (-not $km -and $c.AltKey) {
        $keyPath = Join-Path $TlsDir $c.AltKey
        $km = Get-ModulusMd5 $keyPath "key"
    }
    if (-not $cm -or -not $km) {
        Write-Host "FAIL $($c.Label): missing key for $($c.Cert) (need $($c.Key))" -ForegroundColor Red
        $fail++
        continue
    }
    if ($cm -eq $km) {
        Write-Host "PASS $($c.Label): $($c.Cert) + $(Split-Path $keyPath -Leaf)" -ForegroundColor Green
    } else {
        Write-Host "FAIL $($c.Label): $($c.Cert) does not match $(Split-Path $keyPath -Leaf)" -ForegroundColor Red
        Write-Host "      Re-run EJBCA issue (dual_key on client e2e) or use client_tls.pem in BOTH TSP cert slots until fixed." -ForegroundColor Yellow
        $fail++
    }
}

if ($fail -gt 0) {
    Write-Host "`nResult: FAIL ($fail) - fix before TestPro Connect" -ForegroundColor Red
    exit 1
}
Write-Host "`nResult: PASS - Transport=client_tls+client_tsp.key, MMS=client.pem+client_acse.key" -ForegroundColor Green
exit 0
