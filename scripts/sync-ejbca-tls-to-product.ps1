# Copy EJBCA-issued lab PEMs from apps/ccli/config/tls/ejbca -> apps/ccli/config/tls
# (DUT deploy + stage-tsp-field-pack / PC1 TestPro).
#
# Usage (after issue + CRL export):
#   powershell -File scripts\sync-ejbca-tls-to-product.ps1
#   powershell -File scripts\sync-ejbca-tls-to-product.ps1 -Verify

param(
    [string]$Src = "",
    [string]$Dest = "",
    [switch]$Verify,
    [switch]$WhatIf
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
if (-not $Src) { $Src = Join-Path $Repo "apps\ccli\config\tls\ejbca" }
if (-not $Dest) { $Dest = Join-Path $Repo "apps\ccli\config\tls" }

if (-not (Test-Path (Join-Path $Src "root_CA.pem"))) {
    throw "Missing EJBCA output at $Src; run provision-ejbca-ccli-pki.ps1 -UseEjbcaCli first."
}

$copy = @(
    "root_CA.pem",
    "server.pem", "server.key", "server_tls.pem", "server_acse.key",
    "client.pem", "client.key", "client_acse.key", "client_tls.pem",
    "viewer.pem", "viewer.key", "viewer_tls.pem",
    "revoked.pem", "revoked.key", "revoked_acse.key", "revoked_tls.pem",
    "lab_crl.pem"
)

Write-Host "Sync EJBCA lab PKI" -ForegroundColor Cyan
Write-Host "  from: $Src"
Write-Host "  to:   $Dest"

$copied = @()
foreach ($f in $copy) {
    $from = Join-Path $Src $f
    if (-not (Test-Path $from)) {
        Write-Warning "Skip (not in ejbca): $f"
        continue
    }
    $to = Join-Path $Dest $f
    if ($WhatIf) {
        Write-Host "  [WhatIf] $f"
    } else {
        Copy-Item -Force $from $to
        $copied += $f
    }
}

function Copy-KeyAlias {
    param([string]$FromKey, [string]$AliasName)
    $from = Join-Path $Src $FromKey
    if (-not (Test-Path $from)) { return }
    $to = Join-Path $Dest $AliasName
    if ($WhatIf) {
        Write-Host "  [WhatIf] $AliasName from $FromKey"
    } else {
        Copy-Item -Force $from $to
        if ($copied -notcontains $AliasName) { $copied += $AliasName }
    }
}

Copy-KeyAlias -FromKey "client.key" -AliasName "client_tsp.key"
Copy-KeyAlias -FromKey "client_acse.key" -AliasName "client_acse_tsp.key"
Copy-KeyAlias -FromKey "revoked.key" -AliasName "revoked_tsp.key"
Copy-KeyAlias -FromKey "revoked_acse.key" -AliasName "revoked_acse_tsp.key"

$combined = @("server_combined.pem", "server_combined.key")
foreach ($f in $combined) {
    $ejbcaPath = Join-Path $Src $f
    $destPath = Join-Path $Dest $f
    if (Test-Path $ejbcaPath) {
        if (-not $WhatIf) { Copy-Item -Force $ejbcaPath $destPath; $copied += $f }
    } elseif (Test-Path $destPath) {
        Write-Host "  keep existing (not from EJBCA): $f" -ForegroundColor DarkGray
    } else {
        Write-Warning "Missing $f; DUT Solution C yaml may need gen_lab_pki.py or manual combined cert."
    }
}

if (-not $WhatIf) {
    $manifest = [ordered]@{
        pki_source   = "ejbca"
        synced_utc   = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
        source_dir   = $Src
        product_dir  = $Dest
        files        = $copied
        next_steps   = @(
            "Deploy DUT: lab/tg544-openwrt/deploy-ccli-bench-lan2.ps1",
            "PC1: git pull; scripts/setup-pc1-testpro-from-git.ps1"
        )
    }
    $manifestPath = Join-Path $Dest "PKI_SYNC_MANIFEST.json"
    ($manifest | ConvertTo-Json -Depth 4) | Set-Content -Path $manifestPath -Encoding UTF8
    Write-Host "Wrote $manifestPath" -ForegroundColor Green
}

if ($Verify -and -not $WhatIf) {
    $VerifyPy = Join-Path $Repo "scripts\verify_annex_pki.py"
    if (-not (Test-Path $VerifyPy)) { throw "Missing $VerifyPy" }
    Write-Host "`n=== verify_annex_pki.py (product tls dir) ===" -ForegroundColor Cyan
    python $VerifyPy --dir $Dest
    if ($LASTEXITCODE -ne 0) { throw "Annex verify failed on $Dest" }
}

Write-Host "OK: product TLS dir ready for deploy and TSP field pack." -ForegroundColor Green
