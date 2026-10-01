# Build CCI Master Reference PDF from canonical Markdown sources.
# Usage (repo root): .\scripts\build-master-reference.ps1

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $Root "knowledge-base\_build"
$OutPdf = Join-Path $Root "knowledge-base\CCI_Master_Reference.pdf"
$OutTex = Join-Path $Root "knowledge-base\CCI_Master_Reference.tex"

Write-Host "CCI Master Reference build" -ForegroundColor Cyan
Write-Host "  Manifest: scripts\master-reference-manifest.json"
Write-Host "  Output:   knowledge-base\CCI_Master_Reference.pdf"
Write-Host "            knowledge-base\CCI_Master_Reference.tex"
Write-Host ""

python (Join-Path $Root "scripts\assemble_master_reference.py")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (-not (Test-Path $OutPdf)) {
    Write-Error "PDF not produced at $OutPdf"
}
if (-not (Test-Path $OutTex)) {
    Write-Error "LaTeX not produced at $OutTex"
}

$info = Get-Item $OutPdf
Write-Host ""
Write-Host "Done." -ForegroundColor Green
Write-Host "  PDF: $($info.FullName)"
Write-Host "  TEX: $OutTex"
Write-Host "  Size: $([math]::Round($info.Length / 1KB, 1)) KB"
Write-Host "  Modified: $($info.LastWriteTime)"
Write-Host ""
Write-Host "To update: edit sources listed in master-reference-manifest.json, then re-run this script." -ForegroundColor DarkGray
