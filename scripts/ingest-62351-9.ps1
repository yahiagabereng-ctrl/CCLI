# Ingest IEC 62351-9:2023 PDF + engineering extract into CCLI corpus + RAG.
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
$DestPdf = Join-Path $RefDir "IEC_62351-9_2023.pdf"
$Extract = Join-Path $Kb "CCI_62351-9_Extract.md"
$Capture = Join-Path $Kb "CCI_62351-9_Capture_Plan.md"
$TelEng = "C:\Yahia\projects\Telematry System\documents\projects\ccli\engineering"
$TelRef = "C:\Yahia\projects\Telematry System\documents\projects\ccli\reference"

New-Item -ItemType Directory -Path $RefDir, $TelEng, $TelRef -Force | Out-Null

function Sync-MarkdownCorpus {
    Copy-Item $Extract (Join-Path $TelEng "CCI_62351-9_Extract.md") -Force
    Copy-Item $Capture (Join-Path $TelEng "CCI_62351-9_Capture_Plan.md") -Force
    Write-Host "Synced 62351-9 engineering markdown to Telematry"
}

function Find-62351Pdf {
    param([string[]] $Paths)
    foreach ($c in $Paths) {
        if (-not $c) { continue }
        if (-not (Test-Path -LiteralPath $c)) { continue }
        $len = (Get-Item -LiteralPath $c).Length
        if ($len -gt 1MB) { return $c }
        Write-Warning "Skip small PDF ($len bytes): $c"
    }
    return $null
}

if ($SyncOnly) {
    Sync-MarkdownCorpus
    if (Test-Path $DestPdf) {
        Copy-Item $DestPdf (Join-Path $TelRef "IEC_62351-9_2023.pdf") -Force
    }
    if ($SkipRag) { exit 0 }
}
else {
    $desktop = [Environment]::GetFolderPath("Desktop")
    $oneDriveDesktop = Join-Path $env:USERPROFILE "OneDrive\Desktop"
    $searchPaths = @($PdfPath, $DestPdf)
    foreach ($dir in @($oneDriveDesktop, $desktop)) {
        if (-not (Test-Path $dir)) { continue }
        Get-ChildItem -LiteralPath $dir -Filter "IEC 62351-9*.pdf" -ErrorAction SilentlyContinue |
            ForEach-Object { $searchPaths += $_.FullName }
    }
    $src = Find-62351Pdf -Paths $searchPaths
    if (-not $src) {
        Write-Error "No usable 62351-9 PDF (>1 MB). Place PDF on Desktop or pass -PdfPath."
    }

    Write-Host "62351-9 ingest" -ForegroundColor Cyan
    Write-Host "Source PDF: $src"

    $srcFull = (Get-Item -LiteralPath $src).FullName
    $destFull = (Get-Item -LiteralPath $DestPdf -ErrorAction SilentlyContinue).FullName
    if (-not $destFull) {
        Copy-Item -LiteralPath $src -Destination $DestPdf -Force
    }
    elseif ($srcFull.ToLowerInvariant() -ne $destFull.ToLowerInvariant()) {
        Copy-Item -LiteralPath $src -Destination $DestPdf -Force
    }
    Copy-Item $DestPdf (Join-Path $TelRef "IEC_62351-9_2023.pdf") -Force
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
        Write-Warning "RAG not reachable at $RagBase. Corpus synced only. Start Telematry and run:"
        Write-Host "  cd `"C:\Yahia\projects\Telematry System`"; .\scripts\ingest-ccli.ps1 -ProjectOnly"
        exit 0
    }
}

function Submit-IngestJob {
    param(
        [string] $SourceId,
        [string] $ContainerPath,
        [string] $DocType
    )
    if ($DryRun) {
        Write-Host "[dry-run] $SourceId -> $ContainerPath"
        return
    }
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $SourceId
        document_id      = $SourceId
        document_version = "2"
        file_path        = $ContainerPath
        idempotency_key  = "$SourceId-v2-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = @{ project_id = "ccli"; doc_type = $DocType; standard = "IEC 62351-9:2023" }
    } | ConvertTo-Json -Depth 4
    Write-Host "Enqueue: $SourceId"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json"
}

if ((Test-Path $DestPdf) -and ((Get-Item $DestPdf).Length -gt 1MB)) {
    Submit-IngestJob "ccli-62351-9-pdf" "/documents/projects/ccli/reference/IEC_62351-9_2023.pdf" "standard_pdf"
}
Submit-IngestJob "ccli-62351-9-extract" "/documents/projects/ccli/engineering/CCI_62351-9_Extract.md" "engineering_extract"
Submit-IngestJob "ccli-62351-9-capture-plan" "/documents/projects/ccli/engineering/CCI_62351-9_Capture_Plan.md" "capture_plan"

Write-Host "Done." -ForegroundColor Green
