# Stage TLS material on PC1 aligned with DUT /etc/ccli/tls/ (full deploy-ccli-session-fix.ps1 PKI set).
# Includes server + CA private keys for lab PC1 mirror. Do not commit Dest to git or send over public email.
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

# Same PEM/key set pushed to DUT (deploy-ccli-session-fix.ps1) + root_CA.key for full PC1 mirror.
$dutTlsFiles = @(
    "root_CA.pem", "root_CA.key",
    "server.pem", "server_tls.pem", "server.key", "server_acse.key",
    "server_combined.pem", "server_combined.key",
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

Write-Host "PC1 TLS DUT-aligned bundle -> $Dest" -ForegroundColor Cyan
Write-Host "Source: $Src" -ForegroundColor DarkGray
Write-Host "DUT release pin: $DutRelease" -ForegroundColor DarkGray

$copied = @()
foreach ($f in $dutTlsFiles) {
    $n = Copy-IfExists $f
    if ($n) { $copied += $n }
}

$clientTsp = Join-Path $Dest "client_tsp.key"
if (-not (Test-Path $clientTsp) -and (Test-Path (Join-Path $Src "client.key"))) {
    Copy-Item -Force (Join-Path $Src "client.key") $clientTsp
    if ("client_tsp.key" -notin $copied) { $copied += "client_tsp.key" }
}

$revokedTsp = Join-Path $Dest "revoked_tsp.key"
if (-not (Test-Path $revokedTsp) -and (Test-Path (Join-Path $Src "revoked.key"))) {
    Copy-Item -Force (Join-Path $Src "revoked.key") $revokedTsp
    if ("revoked_tsp.key" -notin $copied) { $copied += "revoked_tsp.key" }
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
    pki_source = "apps/ccli/config/tls (EJBCA sync per PKI_SYNC_MANIFEST.json)"
    generated_utc = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    verify_on_dut = "md5sum /etc/ccli/tls/<file>  # must match md5 below for each PEM/key"
    full_dut_mirror = $true
    handling = "Lab PC1 / USB only - contains private keys; never commit staged folder or attach to public tickets"
    files = $entries
}

$manifestPath = Join-Path $Dest "DUT_TLS_MANIFEST.json"
$manifest | ConvertTo-Json -Depth 5 | Set-Content -Path $manifestPath -Encoding UTF8

Write-Host ""
Write-Host "Copied $($entries.Count) files. Manifest: DUT_TLS_MANIFEST.json" -ForegroundColor Green
Write-Host "Run: powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir `"$Dest`""
