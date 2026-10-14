# Stage TLS for PC1: full DUT deploy mirror from repo (prefer pull-dut-tls-full-mirror.ps1 from live DUT).
#
# Usage:
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

Write-Host "PC1 TLS full DUT mirror (from repo) -> $Dest" -ForegroundColor Cyan
Write-Host "For live DUT match use: pull-dut-tls-full-mirror.ps1" -ForegroundColor DarkGray

foreach ($f in $dutTlsFiles) {
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
    $entries += [ordered]@{ file = $f.Name; bytes = $f.Length; md5 = $md5 }
}

$manifest = [ordered]@{
    dut_release = $DutRelease
    dut_tls_path = "/etc/ccli/tls/"
    scope = "Full DUT PKI deploy mirror for PC1 (includes server + CA private keys)"
    handling = "Lab USB to Masomeh only - never email or commit this folder"
    files = $entries
}
$manifest | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $Dest "DUT_TLS_MANIFEST.json") -Encoding UTF8

Write-Host "Copied $($entries.Count) files."
