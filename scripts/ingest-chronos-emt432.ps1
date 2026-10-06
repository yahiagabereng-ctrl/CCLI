# Ingest Chronos EMT432 manuals + Modbus map into CCLI corpus.
param(
    [string]$RefDir = "",
    [switch]$SkipDownload,
    [switch]$SkipExtract
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
if (-not $RefDir) {
    $RefDir = Join-Path $Repo "knowledge-base\08-engineering\reference\vendor\chronos"
}

$Urls = @{
    "EMT432_MI-ENG.pdf" = "https://static.chronos-tech.it/EMT_432_MI_ENG_3d382e57d1.pdf"
    "EMT432_MI-ITA.pdf" = "https://static.chronos-tech.it/EMT_432_MI_ITA_9fcf8aaff7.pdf"
    "EMT432_FL-ITA.pdf" = "https://static.chronos-tech.it/EMT_432_FL_ITA_8f0283606e.pdf"
    "EMT430_MR.pdf"     = "https://static.chronos-tech.it/EMT_430_MR_cb7f1d080e.pdf"
}

New-Item -ItemType Directory -Force -Path $RefDir | Out-Null

if (-not $SkipDownload) {
    Write-Host "=== Download Chronos EMT432 PDFs (static CDN) ===" -ForegroundColor Cyan
    foreach ($kv in $Urls.GetEnumerator()) {
        $dest = Join-Path $RefDir $kv.Key
        Invoke-WebRequest -Uri $kv.Value -OutFile $dest -UseBasicParsing
        $magic = [System.Text.Encoding]::ASCII.GetString([System.IO.File]::ReadAllBytes($dest)[0..3])
        if ($magic -ne "%PDF") {
            Write-Error "Not a PDF after download: $dest (got HTML?)"
        }
        Write-Host "  OK $($kv.Key) $((Get-Item $dest).Length) bytes"
    }
}

if (-not $SkipExtract) {
    Write-Host "=== Extract engineering corpus ===" -ForegroundColor Cyan
    python (Join-Path $Repo "scripts\extract-chronos-emt432.py") --ref-dir $RefDir
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

Write-Host "Done. Extract: knowledge-base/08-engineering/CCI_Chronos_EMT432_Extract.md" -ForegroundColor Green
Write-Host "Map: apps/ccli/config/modbus/chronos_emt432_map.yaml" -ForegroundColor Green
