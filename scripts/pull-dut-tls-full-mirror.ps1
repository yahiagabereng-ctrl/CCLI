# Pull full /etc/ccli/tls deploy mirror from DUT to local folder (authoritative for Masomeh USB).
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\pull-dut-tls-full-mirror.ps1 -Dest D:\handoff\PC1_tls_from_DUT

param(
    [string]$Dest = "",
    [string]$HostAddr = "192.168.1.130"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$pw = $env:CCLI_TG544_PW
if (-not $pw) { throw "Set CCLI_TG544_PW (source lab-env.ps1)" }

if (-not $Dest) {
    $Dest = Join-Path $RepoRoot "lab\testpro\staged\pc1_tls_from_dut_$(Get-Date -Format 'yyyy-MM-dd_HHmmss')"
}
New-Item -ItemType Directory -Force -Path $Dest | Out-Null

# Same set as deploy-ccli-session-fix.ps1 upload + root_CA.key when on DUT.
$files = @(
    "root_CA.pem", "root_CA.key",
    "server.pem", "server_tls.pem", "server.key", "server_acse.key",
    "server_combined.pem", "server_combined.key",
    "client.pem", "client_tls.pem", "client.key", "client_acse.key", "client_tsp.key",
    "viewer.pem", "viewer_tls.pem", "viewer.key",
    "revoked.pem", "revoked_tls.pem", "revoked.key", "revoked_acse.key",
    "lab_crl.pem"
)

Write-Host "Pull DUT TLS mirror from ${HostAddr} -> $Dest" -ForegroundColor Cyan

$dutVer = (& $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch "ccli --version | head -1").Trim()
$md5Lines = & $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch (
    "cd /etc/ccli/tls && md5sum " + ($files -join " ") + " 2>/dev/null"
)

$pulled = @()
foreach ($f in $files) {
    $remote = "root@${HostAddr}:/etc/ccli/tls/$f"
    $local = Join-Path $Dest $f
    & $Pscp -scp -batch -pw $pw -hostkey $hk $remote $local 2>$null
    if (Test-Path $local) {
        $pulled += $f
        Write-Host "  got $f"
    } else {
        Write-Warning "  skip (not on DUT): $f"
    }
}

# TSP aliases
$ck = Join-Path $Dest "client.key"
if ((Test-Path $ck) -and -not (Test-Path (Join-Path $Dest "client_tsp.key"))) {
    Copy-Item -Force $ck (Join-Path $Dest "client_tsp.key")
}
$rk = Join-Path $Dest "revoked.key"
if ((Test-Path $rk) -and -not (Test-Path (Join-Path $Dest "revoked_tsp.key"))) {
    Copy-Item -Force $rk (Join-Path $Dest "revoked_tsp.key")
}
$rak = Join-Path $Dest "revoked_acse.key"
if ((Test-Path $rak) -and -not (Test-Path (Join-Path $Dest "revoked_acse_tsp.key"))) {
    Copy-Item -Force $rak (Join-Path $Dest "revoked_acse_tsp.key")
}

$entries = @()
foreach ($line in $md5Lines) {
    if ($line -match '^([0-9a-f]{32})\s+(.+)$') {
        $entries += [ordered]@{ file = $Matches[2].Trim(); md5 = $Matches[1].ToLower() }
    }
}

$manifest = [ordered]@{
    source = "DUT /etc/ccli/tls/ (full deploy mirror)"
    dut_host = $HostAddr
    dut_ccli = $dutVer
    scope = "full PKI for PC1 lab - USB to Masomeh only"
    tsp_connect_uses = "root_CA.pem, client_tls.pem+client_tsp.key, client.pem+client_acse.key"
    generated_utc = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    files = $entries
}
$manifest | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $Dest "DUT_TLS_MANIFEST.json") -Encoding UTF8

Copy-Item -Force (Join-Path $RepoRoot "lab\testpro\MASOMEH_CONNECT_FIX_2026-10-14.md") (Join-Path $Dest "READ_ME_CONNECT_FIX.md") -ErrorAction SilentlyContinue

Write-Host ""
Write-Host "Pulled $($pulled.Count) files. USB folder: $Dest" -ForegroundColor Green
Write-Host "Run: verify-tsp-tls-key-match.ps1 -TlsDir `"$Dest`""
