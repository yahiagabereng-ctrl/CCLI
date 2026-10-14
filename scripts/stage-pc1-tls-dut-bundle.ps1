# Stage TLS on PC1: files deployed to DUT that Test Suite Pro / UCA bench needs (T1 + Tier 3).
# Same trust + client/viewer/revoked material as deploy-ccli-session-fix.ps1, excluding DUT-server-only keys/certs.
#
# Usage:
#   powershell -File scripts\stage-pc1-tls-dut-bundle.ps1
#   powershell -File scripts\stage-pc1-tls-dut-bundle.ps1 -Dest D:\CCLI_TSP_FIELD_PACK\tls

param(
    [string]$Dest = "",
    [string]$Src = "",
    [string]$DutRelease = "0.1.0-r55"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $Src) {
    $Src = Join-Path $RepoRoot "apps\ccli\config\tls"
}
if (-not $Dest) {
    $Dest = Join-Path $RepoRoot "lab\testpro\staged\tls_dut_aligned"
}

New-Item -ItemType Directory -Force -Path $Dest | Out-Null

# Intersection: on DUT after deploy + required for cert/TSP (LAB_REQUIREMENT_TEST_MATRIX T1-01, T3-01..04).
$pc1CertTlsFiles = @(
    "root_CA.pem",
    "client.pem", "client_tls.pem", "client.key", "client_acse.key", "client_tsp.key",
    "client_acse_tsp.key",
    "viewer.pem", "viewer_tls.pem", "viewer.key",
    "revoked.pem", "revoked_tls.pem", "revoked.key", "revoked_acse.key", "revoked_tsp.key",
    "lab_crl.pem"
)

function Copy-IfExists([string]$Name) {
    $from = Join-Path $Src $Name
    if (-not (Test-Path $from)) {
        Write-Warning "Missing: $Name"
        return $null
    }
    Copy-Item -Force $from (Join-Path $Dest $Name)
    return $Name
}

Write-Host "PC1 TLS (DUT deploy cap + cert bench) -> $Dest" -ForegroundColor Cyan
Write-Host "Source: $Src" -ForegroundColor DarkGray
Write-Host "DUT release pin: $DutRelease" -ForegroundColor DarkGray

foreach ($f in $pc1CertTlsFiles) {
    Copy-IfExists $f | Out-Null
}

$clientTsp = Join-Path $Dest "client_tsp.key"
if (-not (Test-Path $clientTsp) -and (Test-Path (Join-Path $Src "client.key"))) {
    Copy-Item -Force (Join-Path $Src "client.key") $clientTsp
}

$revokedTsp = Join-Path $Dest "revoked_tsp.key"
if (-not (Test-Path $revokedTsp) -and (Test-Path (Join-Path $Src "revoked.key"))) {
    Copy-Item -Force (Join-Path $Src "revoked.key") $revokedTsp
}

$revokedAcseTsp = Join-Path $Dest "revoked_acse_tsp.key"
if (-not (Test-Path $revokedAcseTsp) -and (Test-Path (Join-Path $Src "revoked_acse.key"))) {
    Copy-Item -Force (Join-Path $Src "revoked_acse.key") $revokedAcseTsp
}

$entries = @()
foreach ($f in (Get-ChildItem $Dest -File | Sort-Object Name)) {
    $md5 = (Get-FileHash $f.FullName -Algorithm MD5).Hash.ToLower()
    $entries += [ordered]@{
        file = $f.Name
        bytes = $f.Length
        md5 = $md5
    }
}

$manifest = [ordered]@{
    dut_release = $DutRelease
    dut_tls_path = "/etc/ccli/tls/"
    scope = "PC1 cert bench only (not full DUT tls tree)"
    tsp_rows = @("T1-01 Connect", "T3-01 no client cert", "T3-02/03 viewer RBAC", "T3-04 revoked + CRL")
    pki_source = "apps/ccli/config/tls"
    generated_utc = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    verify_on_dut = "md5sum /etc/ccli/tls/<file>  # same filename on DUT"
    not_pushed_to_pc1 = @(
        "root_CA.key",
        "server.pem", "server_tls.pem", "server_combined.pem",
        "server.key", "server_acse.key", "server_combined.key",
        "gen_lab_pki.py", "README.md", "ejbca/", "lab_crl.pem.BAD_SIG"
    )
    handling = "Lab PC1 / USB - client-side private keys only; not for accredited lab email"
    files = $entries
}

$manifestPath = Join-Path $Dest "DUT_TLS_MANIFEST.json"
$manifest | ConvertTo-Json -Depth 5 | Set-Content -Path $manifestPath -Encoding UTF8

Write-Host ""
Write-Host "Copied $($entries.Count) files. Manifest: DUT_TLS_MANIFEST.json" -ForegroundColor Green
Write-Host "Run: powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir `"$Dest`""
