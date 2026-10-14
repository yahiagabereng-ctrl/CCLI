# Stage CCLI lab TLS PEMs for Triangle MicroWorks Test Suite Pro (PC-side)
# Does NOT Connect to DUT. Copies identity material into ProgramData for TSP UI browse.
#
# Usage:
#   powershell -File scripts\stage-tsp-tls.ps1
#   powershell -File scripts\stage-tsp-tls.ps1 -Dest "D:\lab\tsp-tls"

param(
    [string]$Dest = "C:\ProgramData\Triangle MicroWorks\61850 Test Suite Pro\CCLI_tls",
    [string]$AltDest = "C:\CCLI_tls",
    [string]$ProductDest = "C:\CCLI_product_tls",
    [string]$Src = ""
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $Src) {
    $legacy = Join-Path $RepoRoot "apps\ccli\config\tls"
    $ejbca = Join-Path $RepoRoot "apps\ccli\config\tls\ejbca"
    # Same default as deploy-ccli-session-fix.ps1 ($TlsDir = config/tls). Use -Src ...\ejbca for EJBCA regen sets.
    $Src = if (Test-Path (Join-Path $legacy "server.pem")) { $legacy }
           elseif (Test-Path (Join-Path $ejbca "client.pem")) { $ejbca }
           else { $legacy }
}

if (-not (Test-Path $Src)) {
    throw "TLS source missing: $Src"
}

New-Item -ItemType Directory -Force -Path $Dest | Out-Null

$files = @(
    "root_CA.pem",
    "client.pem", "client.key", "client_acse.key", "client_acse_tsp.key",
    "client_tls.pem", "client_tsp.key",
    "viewer.pem", "viewer_tls.pem", "viewer.key",
    "revoked.pem", "revoked.key", "revoked_acse.key", "revoked_acse_tsp.key",
    "revoked_tls.pem", "revoked_tsp.key",
    "lab_crl.pem",
    "README.md"
)

function Stage-TspTlsFolder {
    param([string]$Target)
    New-Item -ItemType Directory -Force -Path $Target | Out-Null
    foreach ($f in $files) {
        $from = Join-Path $Src $f
        if (-not (Test-Path $from)) { Write-Warning "Missing $f"; continue }
        Copy-Item -Force $from (Join-Path $Target $f)
        Write-Host "staged $f -> $Target"
    }
    $tspKey = Join-Path $Target "client_tsp.key"
    if (-not (Test-Path $tspKey) -and (Test-Path (Join-Path $Src "client.key"))) {
        Copy-Item -Force (Join-Path $Src "client.key") $tspKey
        Write-Host "staged client_tsp.key (from client.key) -> $Target"
    }
    $revokedTspKey = Join-Path $Target "revoked_tsp.key"
    if (-not (Test-Path $revokedTspKey) -and (Test-Path (Join-Path $Src "revoked.key"))) {
        Copy-Item -Force (Join-Path $Src "revoked.key") $revokedTspKey
        Write-Host "staged revoked_tsp.key (from revoked.key) -> $Target"
    }
}

Write-Host "TLS source: $Src" -ForegroundColor Cyan
Stage-TspTlsFolder -Target $Dest
if ($AltDest -and ($AltDest -ne $Dest)) {
    Stage-TspTlsFolder -Target $AltDest
}
if ($ProductDest -and ($ProductDest -ne $Dest) -and ($ProductDest -ne $AltDest)) {
    Stage-TspTlsFolder -Target $ProductDest
}

$readme = @"
# CCLI TSP TLS stage (Annex / EJBCA lab)

Copied from ``$Src`` on $(Get-Date -Format 'yyyy-MM-dd HH:mm').

## Wire in Test Suite Pro (other PC OK — copy this folder)

| Role | Files |
|------|-------|
| Trust anchor | root_CA.pem |
| Transport TLS (Cert A) | client_tls.pem + **client_tsp.key** |
| MMS Security (Cert B) | client.pem + **client_acse.key** (or client_acse_tsp.key) — **not** client_tsp.key |
| Viewer / RBAC negative | viewer.pem + viewer.key |
| CRL negative (T3-04 / P3-09) | revoked_tls.pem + revoked.pem + **revoked_tsp.key** (both key slots) + import **lab_crl.pem** as CRL |

Connection: 192.168.10.1 port **3782** TLS — not cleartext 102.

### T3-04 revoked connect (swap from DSO client)

1. Enable / browse **lab_crl.pem** on the connection (DUT runs with ``crl=on``).
2. Replace Transport cert with **revoked_tls.pem** and MMS Security with **revoked.pem**.
3. Transport key **revoked_tsp.key** · MMS key **revoked_acse.key** (dual cert, same as DSO client).
4. Connect must **fail** (revoked cert rejected). Restore **client_*** before T1 rows.

## OpenSSL on TestPro PC (preflight)

Install OpenSSL 3.x Win64; then from this folder:

  openssl verify -CAfile root_CA.pem client.pem client_tls.pem
  openssl rsa -in client_tsp.key -check -noout

If TSP rejects the key: ``openssl rsa -in client.key -out client_tsp.key``

See: handoff/masomeh95/PC1_LOG_CAPTURE_REQUIREMENTS.md (pack: 00h_...)

See: lab/EJBCA_LAB_SETUP.md section 9–10
"@
Set-Content -Path (Join-Path $Dest "TSP_STAGE_README.txt") -Value $readme -Encoding UTF8
if (Test-Path $ProductDest) {
    Set-Content -Path (Join-Path $ProductDest "TSP_STAGE_README.txt") -Value $readme -Encoding UTF8
}

Write-Host ""
Write-Host "OK -> $Dest"
if ($AltDest -and ($AltDest -ne $Dest)) { Write-Host "OK -> $AltDest" }
if ($ProductDest) { Write-Host "OK -> $ProductDest  (copy this folder to the TestPro PC)" }
Write-Host "Next: TSP System Status / IED Connection -> TLS paths above."
Write-Host "Use client_tsp.key (Transport) + client_acse.key (MMS Security). Run scripts/verify-tsp-tls-key-match.ps1"
Write-Host "Keep Disconnected until DUT is up (Phase B)."
