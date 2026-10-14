# Sync IEC TS 62351-100-4 extract + OCR corpus to Telematry; enqueue RAG.
param(
    [string]$TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string]$RagBase = "http://localhost:8001",
    [switch]$SkipRag,
    [switch]$SyncOnly,
    [switch]$SkipSync,
    [switch]$ReExtract,
    [switch]$ForceOcr
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$ExtractPy = Join-Path $Repo "scripts\extract-62351-100-4.py"
$Eng = Join-Path $Repo "knowledge-base\08-engineering"
$Ref = Join-Path $Eng "reference\iec62351"
$Corpus = Join-Path $Ref "_extract_62351-100-4\full.txt"
$CorpusAlias = Join-Path $Ref "IEC_TS_62351-100-4_2023_full.txt"

if ($ReExtract -or -not (Test-Path $Corpus)) {
    Write-Host "Extracting IEC TS 62351-100-4 PDF..." -ForegroundColor Cyan
    if ($ForceOcr) {
        python $ExtractPy --force-ocr
    }
    else {
        python $ExtractPy
        # doc88 viewer PDFs need OCR; auto-retry if corpus looks like site chrome only
        $sample = Get-Content (Join-Path $Ref "_extract_62351-100-4\p010.txt") -Raw -ErrorAction SilentlyContinue
        if ($sample -match "相关 目录" -and $sample -notmatch "IEC TS 62351-100-4") {
            Write-Host "Detected doc88 text layer; re-running with --force-ocr" -ForegroundColor Yellow
            python $ExtractPy --force-ocr
        }
    }
}

if (Test-Path $Corpus) {
    Copy-Item -Force $Corpus $CorpusAlias
}

$TelEng = Join-Path $TelematryRoot "documents\projects\ccli\engineering"
$TelRef = Join-Path $TelematryRoot "documents\projects\ccli\reference\iec62351"
New-Item -ItemType Directory -Force -Path $TelEng, $TelRef | Out-Null

$files = @(
    @{ Src = Join-Path $Eng "CCI_62351-100-4_Extract.md"; Dst = Join-Path $TelEng "CCI_62351-100-4_Extract.md" },
    @{ Src = Join-Path $Eng "CCI_62351-100-4_OCR_Corpus.md"; Dst = Join-Path $TelEng "CCI_62351-100-4_OCR_Corpus.md" },
    @{ Src = $CorpusAlias; Dst = Join-Path $TelRef "IEC_TS_62351-100-4_2023_full.txt" }
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

if (-not $SkipSync) {
    & (Join-Path $Repo "scripts\sync-ccli-corpus-to-telematry.ps1") -TelematryRoot $TelematryRoot
}

if ($SyncOnly -or $SkipRag) {
    Write-Host "SyncOnly: RAG ingest skipped." -ForegroundColor Yellow
    exit 0
}

$jobs = @(
    @{ id = "ccli-62351-100-4-extract"; rel = "projects/ccli/engineering/CCI_62351-100-4_Extract.md" },
    @{ id = "ccli-62351-100-4-ocr-corpus"; rel = "projects/ccli/reference/iec62351/IEC_TS_62351-100-4_2023_full.txt" },
    @{ id = "ccli-62351-100-4-ocr-index"; rel = "projects/ccli/engineering/CCI_62351-100-4_OCR_Corpus.md" }
)

$docsRoot = Join-Path $TelematryRoot "documents"
try {
    $h = Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 8
    if ($h.status -ne "ok") { throw "health not ok" }
}
catch {
    Write-Warning "RAG offline at $RagBase. Files synced; run later with -SkipSync only."
    exit 0
}

foreach ($j in $jobs) {
    $local = Join-Path $docsRoot ($j.rel -replace '/', '\')
    if (-not (Test-Path $local)) {
        Write-Warning "Skip $($j.id) - missing $local"
        continue
    }
    $container = "/documents/$($j.rel -replace '\\', '/')"
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $j.id
        document_id      = $j.id
        document_version = "1"
        file_path        = $container
        idempotency_key  = "$($j.id)-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = @{ project_id = "ccli"; standard = "62351-100-4"; edition = "2023" }
    } | ConvertTo-Json -Depth 4
    Write-Host "Enqueue: $($j.id)"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json" | Out-Null
}

$ingest = Join-Path $TelematryRoot "scripts\ingest-ccli.ps1"
if (Test-Path $ingest) {
    & $ingest -ProjectOnly
}

Write-Host "62351-100-4 RAG ingest complete." -ForegroundColor Green
