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

$testProReadme = Join-Path $Repo "lab\testpro\PC1_TSP_IMPORT_CHECKLIST.md"
if (Test-Path $testProReadme) {
    Copy-Item -Force $testProReadme (Join-Path $Dest "00i_PC1_TSP_IMPORT_CHECKLIST.md")
    Write-Host "  00i_PC1_TSP_IMPORT_CHECKLIST.md"
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
