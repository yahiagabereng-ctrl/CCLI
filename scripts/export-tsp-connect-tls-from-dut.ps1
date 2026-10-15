# Minimal mutual-TLS + MMS Security set for Test Suite Pro T1-01 Connect (pulled from live DUT).
# For ALL certificates in one ZIP (Connect + revoked + CRL + viewer + server): use
#   scripts\export-pc1-tls-for-masomeh.ps1
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\export-tsp-connect-tls-from-dut.ps1
#   powershell -File scripts\export-tsp-connect-tls-from-dut.ps1 -Dest C:\Yahia\TSP_CONNECT_TLS

param(
    [string]$Dest = "C:\Yahia\TSP_CONNECT_TLS",
    [string]$HostAddr = "192.168.1.130"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$pw = $env:CCLI_TG544_PW
if (-not $pw) { throw "Set CCLI_TG544_PW (source lab-env.ps1)" }

# Exactly what TSP IED Connection needs for CCI016_01 @ 192.168.10.1:3782
$connectFiles = @(
    "root_CA.pem",
    "client_tls.pem",
    "client_tsp.key",
    "client.pem",
    "client_acse.key"
)

New-Item -ItemType Directory -Force -Path $Dest | Out-Null

$dutVer = (& $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch "ccli --version | head -1").Trim()
Write-Host "TSP Connect TLS from DUT $HostAddr ($dutVer)" -ForegroundColor Cyan
Write-Host "Dest: $Dest" -ForegroundColor Cyan

foreach ($f in $connectFiles) {
    & $Pscp -scp -batch -pw $pw -hostkey $hk "root@${HostAddr}:/etc/ccli/tls/$f" (Join-Path $Dest $f)
    if (-not (Test-Path (Join-Path $Dest $f))) {
        if ($f -eq "client_tsp.key") {
            & $Pscp -scp -batch -pw $pw -hostkey $hk "root@${HostAddr}:/etc/ccli/tls/client.key" (Join-Path $Dest "client_tsp.key")
        }
        if (-not (Test-Path (Join-Path $Dest $f))) { throw "Missing on DUT: $f" }
    }
    Write-Host "  OK $f"
}

& (Join-Path $RepoRoot "scripts\verify-tsp-tls-key-match.ps1") -TlsDir $Dest
if ($LASTEXITCODE -ne 0) { throw "Key/cert pairing failed" }

$md5Rows = @()
foreach ($f in $connectFiles) {
    $md5Rows += [ordered]@{
        file = $f
        md5  = (Get-FileHash (Join-Path $Dest $f) -Algorithm MD5).Hash.ToLower()
    }
}

$readme = @"
# Test Suite Pro T1-01 Connect — exact files (from DUT)

DUT: $dutVer
Host: 192.168.10.1  Port: 3782  TLS: ON (prefer TLS 1.2 in TSP)
IED: CCI016_01

| TSP field              | File in this folder   |
|------------------------|------------------------|
| Trusted CA             | root_CA.pem            |
| Transport certificate  | client_tls.pem         |
| Transport private key  | client_tsp.key         |
| MMS Security certificate | client.pem           |
| MMS Security private key | client_acse.key      |

Do NOT use client_tsp.key on the MMS Security tab.
Do NOT use server*.pem or server*.key here (those stay on the DUT only).

After extract, re-browse every path in TSP (cached paths break Connect).
"@
Set-Content -Path (Join-Path $Dest "TSP_CONNECT_README.txt") -Value $readme -Encoding UTF8

[ordered]@{
    purpose = "Test Suite Pro T1-01 mutual TLS + MMS Security"
    dut_ccli = $dutVer
    dut_host = $HostAddr
    generated_utc = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    files = $md5Rows
} | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $Dest "TSP_CONNECT_TLS_PIN.json") -Encoding UTF8

Write-Host ""
Write-Host "READY: WinRAR this folder -> $Dest" -ForegroundColor Green
