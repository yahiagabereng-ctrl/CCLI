# PC1: git pull companion — stage field pack + TLS readme for Test Suite Pro.
# Usage:
#   cd CCLI
#   git pull origin main
#   powershell -File scripts\setup-pc1-testpro-from-git.ps1
#   powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK

param(
    [string]$Dest = "D:\CCLI_TSP_FIELD_PACK"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot

Write-Host "=== PC1 TestPro setup (CID + TLS field pack) ===" -ForegroundColor Cyan
Write-Host "Repo: $Repo"

& (Join-Path $Repo "scripts\stage-tsp-field-pack.ps1") -Dest $Dest

# Overwrite tls\ with full DUT-aligned PEM set + DUT_TLS_MANIFEST.json (includes revoked, viewer, CRL).
$dutVer = "0.1.0-r55"
$verFile = Join-Path $Repo "apps\ccli\VERSION"
if (Test-Path $verFile) {
    $line = Get-Content $verFile -Raw
    if ($line -match 'PKG_RELEASE:=\s*(\d+)') { $dutVer = "0.1.0-r$($Matches[1])" }
}
& (Join-Path $Repo "scripts\stage-pc1-tls-dut-bundle.ps1") -Dest (Join-Path $Dest "tls") -DutRelease $dutVer

$testProReadme = Join-Path $Repo "lab\testpro\PC1_TSP_IMPORT_CHECKLIST.md"
if (Test-Path $testProReadme) {
    Copy-Item -Force $testProReadme (Join-Path $Dest "00i_PC1_TSP_IMPORT_CHECKLIST.md")
    Write-Host "  00i_PC1_TSP_IMPORT_CHECKLIST.md"
}
$tlsBundleDoc = Join-Path $Repo "lab\testpro\PC1_TLS_BUNDLE_DUT_ALIGNED.md"
if (Test-Path $tlsBundleDoc) {
    Copy-Item -Force $tlsBundleDoc (Join-Path $Dest "00j_PC1_TLS_BUNDLE_DUT_ALIGNED.md")
    Write-Host "  00j_PC1_TLS_BUNDLE_DUT_ALIGNED.md"
}
$masomehFix = Join-Path $Repo "lab\testpro\MASOMEH_CONNECT_FIX_2026-10-14.md"
if (Test-Path $masomehFix) {
    Copy-Item -Force $masomehFix (Join-Path $Dest "00k_MASOMEH_CONNECT_FIX_2026-10-14.md")
    Write-Host "  00k_MASOMEH_CONNECT_FIX_2026-10-14.md"
}
$triage = Join-Path $Repo "lab\evidence\testsuite-pro\inbox\TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md"
if (Test-Path $triage) {
    Copy-Item -Force $triage (Join-Path $Dest "00l_TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md")
    Write-Host "  00l_TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md"
}

$json = Join-Path $Repo "lab\testpro\tsp_tls_and_cid.json"
if (Test-Path $json) {
    Copy-Item -Force $json (Join-Path $Dest "testpro\tsp_tls_and_cid.json")
    Write-Host "  testpro\tsp_tls_and_cid.json"
}

Write-Host ""
Write-Host "OK: $Dest" -ForegroundColor Green
Write-Host "Open: 00i_PC1_TSP_IMPORT_CHECKLIST.md -> 00h_PC1_LOG_CAPTURE_REQUIREMENTS.md -> 00_UNIFIED_CERT_TEST_MATRIX.md"
Write-Host "TSP TLS folder: $Dest\tls\  (see tls\TSP_STAGE_README.txt)"
Write-Host "TSP CID:        $Dest\cid\lab_tg544_eth_a.cid"
