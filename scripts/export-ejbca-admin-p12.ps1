# Export / locate EJBCA SuperAdmin P12 for REST mTLS (lab).
# REST Certificate Management requires a client certificate — not an API key.
param(
    [string]$Container = "ccli-ejbca-ce",
    [string]$OutDir = "",
    [string]$Password = "foo123"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Dest = if ($OutDir) { $OutDir } else { Join-Path $Repo "lab\ejbca-staging" }
New-Item -ItemType Directory -Force -Path $Dest | Out-Null

$candidates = @(
    "/opt/keyfactor/secrets/superadmin.p12",
    "/opt/keyfactor/secrets/persistent/superadmin.p12",
    "/opt/keyfactor/p12/superadmin.p12",
    "/home/wildfly/superadmin.p12"
)

$found = $null
foreach ($p in $candidates) {
    $exists = docker exec $Container test -f $p 2>$null
    if ($LASTEXITCODE -eq 0) {
        $found = $p
        break
    }
}

if (-not $found) {
    Write-Host @"
No SuperAdmin P12 found in container (common with TLS_SETUP_ENABLED=simple).

Create one in Admin UI, then place it under lab\ejbca-staging\superadmin.p12:

  1. https://localhost:8443/ejbca/adminweb/
  2. RA Functions → Add End Entity → SuperAdmin (or use Management CA enroll)
  3. Create PKCS#12, download
  4. Save as: $Dest\superadmin.p12

Also enable: System Configuration → Protocol Configuration → REST Certificate Management = Enable

Profiles required before REST issue (see lab/EJBCA_LAB_SETUP.md §§3–5):
  CCLI-Lab-CA, CCLI-Cert-A-TLS, CCLI-Cert-B-E2E-MMS,
  CCLI-Server-Profile, CCLI-Operator-Profile
"@ -ForegroundColor Yellow
    exit 2
}

$out = Join-Path $Dest "superadmin.p12"
docker cp "${Container}:${found}" $out
Write-Host "Exported: $out" -ForegroundColor Green
Write-Host "Default P12 password is often '$Password' (change if you set another)."
Write-Host ""
Write-Host "Issue PEMs:"
Write-Host "  python scripts\issue_ejbca_rest_pki.py --admin-p12 `"$out`" --admin-p12-password $Password --insecure --verify-after"
