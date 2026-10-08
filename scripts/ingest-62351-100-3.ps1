# Sync IEC TS 62351-100-3 extract + OCR corpus to Telematry; enqueue RAG (-ProjectOnly).
param(
    [string]$TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string]$RagBase = "http://localhost:8001",
    [switch]$SkipRag,
    [switch]$SyncOnly,
    [switch]$ReExtract
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$ExtractPy = Join-Path $Repo "scripts\extract-62351-100-3.py"
$Eng = Join-Path $Repo "knowledge-base\08-engineering"
$Ref = Join-Path $Eng "reference\iec62351"
$Corpus = Join-Path $Ref "_extract_62351-100-3\full.txt"

if ($ReExtract -or -not (Test-Path $Corpus)) {
    Write-Host "Extracting IEC TS 62351-100-3 PDF (text layer + OCR)..." -ForegroundColor Cyan
    python $ExtractPy
}

$TelEng = Join-Path $TelematryRoot "documents\projects\ccli\engineering"
$TelRef = Join-Path $TelematryRoot "documents\projects\ccli\reference\iec62351"
New-Item -ItemType Directory -Force -Path $TelEng, $TelRef | Out-Null

$files = @(
    @{ Src = Join-Path $Eng "CCI_62351-100-3_Extract.md"; Dst = Join-Path $TelEng "CCI_62351-100-3_Extract.md" },
    @{ Src = $Corpus; Dst = Join-Path $TelRef "IEC_TS_62351-100-3_2020_full.txt" }
)
foreach ($f in $files) {
    if (Test-Path $f.Src) {
        Copy-Item -Force $f.Src $f.Dst
        Write-Host "Synced $($f.Dst)"
    }
    else {
        Write-Warning "Missing $($f.Src)"
    }
}

& (Join-Path $Repo "scripts\sync-ccli-corpus-to-telematry.ps1") -TelematryRoot $TelematryRoot

if ($SyncOnly -or $SkipRag) {
    Write-Host "SyncOnly: RAG ingest skipped." -ForegroundColor Yellow
    exit 0
}

$ingest = Join-Path $TelematryRoot "scripts\ingest-ccli.ps1"
if (-not (Test-Path $ingest)) {
    Write-Warning "Missing $ingest"
    exit 0
}
try {
    Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 5 | Out-Null
    & $ingest -ProjectOnly
    Write-Host "RAG ingest triggered." -ForegroundColor Green
}
catch {
    Write-Warning "RAG offline at $RagBase. Run later: cd `"$TelematryRoot`"; .\scripts\ingest-ccli.ps1 -ProjectOnly"
}
