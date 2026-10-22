# Sync lab-validated cyber artefacts -> product/apps/ccli (single cert specimen).
# Usage (CCLI Lab root):
#   powershell -File scripts\sync-frozen-cyber-to-product.ps1
#   powershell -File scripts\sync-frozen-cyber-to-product.ps1 -IncludeProgram

param(
    [switch]$IncludeProgram,
    [switch]$WhatIf
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Lab = Join-Path $Repo "apps\ccli"
$Prod = Join-Path $Repo "product\apps\ccli"

if (-not (Test-Path $Prod)) {
    throw "Product tree missing: $Prod"
}

$files = @(
    "config\product_tr400.yaml",
    "config\product_tr400_emt432_5ln.yaml",
    "config\lab_tr400_phase4_eth_b_ttyS1_5ln.yaml",
    "config\icd\product\product_tr400_eth_a_5ln.cid",
    "config\icd\product\product_tr400_eth_a_5ln.cfg",
    "core\config\product_cyber_gate.hpp",
    "VERSION"
)

Write-Host "Sync frozen cyber: $Lab -> $Prod" -ForegroundColor Cyan
foreach ($rel in $files) {
    $src = Join-Path $Lab $rel
    $dst = Join-Path $Prod $rel
    if (-not (Test-Path $src)) {
        Write-Warning "Skip missing: $rel"
        continue
    }
    $dir = Split-Path $dst -Parent
    if (-not $WhatIf) {
        New-Item -ItemType Directory -Force -Path $dir | Out-Null
        Copy-Item -Force $src $dst
    }
    Write-Host "  $rel"
}

if ($IncludeProgram) {
    Write-Host "Sync program/ (SK0153) ..." -ForegroundColor Cyan
    $progSrc = Join-Path $Lab "program"
    if (-not (Test-Path $progSrc)) {
        throw "Missing lab program/: $progSrc"
    }
    $progArgs = @(
        $progSrc, (Join-Path $Prod "program"),
        "/E", "/NFL", "/NDL", "/NJH", "/NJS"
    )
    if ($WhatIf) {
        Write-Host "[WhatIf] robocopy program/"
    } else {
        robocopy @progArgs | Out-Null
    }
}

Write-Host "Done. Rebuild lab + product; deploy PROFILE PRODUCT-5LN-CERT or UNIFIED-5LN-CERT." -ForegroundColor Green
