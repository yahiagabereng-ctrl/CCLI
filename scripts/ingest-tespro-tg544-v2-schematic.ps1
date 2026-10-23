# Ingest TG-544 V2 schematic text extract into Telematry corpus (run from repo root).
param(
    [switch]$ExtractOnly
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Pdf = Join-Path $Root "knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/TG544_V2_Schematic.pdf"

if (-not (Test-Path $Pdf)) {
    Write-Error "Missing canonical PDF: $Pdf`nCopy supplier drop to that path first."
}

python (Join-Path $Root "scripts/extract-tespro-tg544-v2-schematic.py")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

python (Join-Path $Root "scripts/extract-tg544-v2-datasheets.py")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if ($ExtractOnly) {
    Write-Host "Extract only — skip RAG sync."
    exit 0
}

$Sync = Join-Path $Root "scripts/sync-ccli-corpus-to-telematry.ps1"
if (Test-Path $Sync) {
    Write-Host "Run RAG sync when Telematry is up:"
    Write-Host "  powershell -File scripts/sync-ccli-corpus-to-telematry.ps1"
} else {
    Write-Host "Sync script not found — ingest extract via your RAG pipeline manually."
}

Write-Host "Done. Review: lab/evidence/hw/TG544_V2_SCHEMATIC_REVIEW_2026-10-23.md"
