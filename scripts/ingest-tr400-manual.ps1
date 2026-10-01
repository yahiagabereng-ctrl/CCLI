# Ingest TesPro TR400 User Manual into CCLI corpus + RAG.
param(
    [string] $PdfPath = "",
    [string] $RagBase = "http://localhost:8001",
    [switch] $DryRun,
    [switch] $SkipRag,
    [switch] $SyncOnly
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Kb = Join-Path $Repo "knowledge-base\08-engineering"
$RefDir = Join-Path $Kb "reference"
$DestPdf = Join-Path $RefDir "TR400_User_Manual_EN_v1.0.pdf"
$Extract = Join-Path $Kb "CCI_TR400_Extract.md"
$Capture = Join-Path $Kb "CCI_TR400_Capture_Plan.md"
$TelEng = "C:\Yahia\projects\Telematry System\documents\projects\ccli\engineering"
$TelRef = "C:\Yahia\projects\Telematry System\documents\projects\ccli\reference"

New-Item -ItemType Directory -Path $RefDir, $TelEng, $TelRef -Force | Out-Null

function Sync-MarkdownCorpus {
    $IoPeripheral = Join-Path $Kb "CCI_TR500_IO_Peripheral.md"
    Copy-Item $Extract (Join-Path $TelEng "CCI_TR400_Extract.md") -Force
    Copy-Item $Capture (Join-Path $TelEng "CCI_TR400_Capture_Plan.md") -Force
    if (Test-Path $IoPeripheral) {
        Copy-Item $IoPeripheral (Join-Path $TelEng "CCI_TR500_IO_Peripheral.md") -Force
    }
    Write-Host "Synced TR400/TR500 engineering markdown to Telematry"
}

function Find-Tr400Pdf {
    param([string[]] $Paths)
    foreach ($c in $Paths) {
        if (-not $c) { continue }
        if (-not (Test-Path $c)) { continue }
        $len = (Get-Item $c).Length
        if ($len -gt 1000) { return $c }
        Write-Warning "Skip empty or small PDF ($len bytes): $c"
    }
    return $null
}

if ($SyncOnly) {
    Sync-MarkdownCorpus
    if ($SkipRag) { exit 0 }
}
else {
    $searchPaths = @(
        $PdfPath,
        $DestPdf,
        (Join-Path $env:USERPROFILE "OneDrive\Desktop\TR400_User_Manual_EN_v1.0.pdf"),
        (Join-Path $env:USERPROFILE "Desktop\TR400_User_Manual_EN_v1.0.pdf")
    )
    $src = Find-Tr400Pdf -Paths $searchPaths
    if (-not $src) {
        Write-Error "No usable TR400 PDF (>1 KB). Download OneDrive file locally, then re-run."
    }

    Write-Host "TR400 manual ingest" -ForegroundColor Cyan
    Write-Host "Source PDF: $src"

    Copy-Item -LiteralPath $src -Destination $DestPdf -Force
    $py = Get-Command python -ErrorAction SilentlyContinue
    if (-not $py) { throw "python not found" }
    & $py (Join-Path $Repo "scripts\extract-tr400-manual.py") --pdf $DestPdf --out $Extract
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    Copy-Item $DestPdf (Join-Path $TelRef "TR400_User_Manual_EN_v1.0.pdf") -Force
    Sync-MarkdownCorpus
}

if ($SkipRag) {
    Write-Host "SkipRag: corpus updated only." -ForegroundColor Yellow
    exit 0
}

if (-not $DryRun) {
    try {
        $health = Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 8
        Write-Host "RAG status: $($health.status)" -ForegroundColor Green
    }
    catch {
        Write-Warning "RAG not reachable. Corpus updated; run Telematry ingest later."
        exit 0
    }
}

function Submit-IngestJob {
    param(
        [string] $SourceId,
        [string] $ContainerPath
    )
    if ($DryRun) {
        Write-Host "[dry-run] $SourceId"
        return
    }
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $SourceId
        document_id      = $SourceId
        document_version = "1"
        file_path        = $ContainerPath
        idempotency_key  = "$SourceId-v1-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = @{ project_id = "ccli"; doc_type = "hw_user_manual"; platform = "tespro-tr400" }
    } | ConvertTo-Json -Depth 4
    Write-Host "Enqueue: $SourceId"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json"
}

if ((-not $SyncOnly) -and (Test-Path $DestPdf) -and ((Get-Item $DestPdf).Length -gt 1000)) {
    Submit-IngestJob "ccli-tr400-user-manual" "/documents/projects/ccli/reference/TR400_User_Manual_EN_v1.0.pdf"
}
Submit-IngestJob "ccli-tr400-user-manual-extract" "/documents/projects/ccli/engineering/CCI_TR400_Extract.md"
Submit-IngestJob "ccli-tr400-capture-plan" "/documents/projects/ccli/engineering/CCI_TR400_Capture_Plan.md"
Submit-IngestJob "ccli-tr500-io-peripheral" "/documents/projects/ccli/engineering/CCI_TR500_IO_Peripheral.md"

Write-Host "Done." -ForegroundColor Green
