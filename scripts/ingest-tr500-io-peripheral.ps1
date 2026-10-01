# Ingest TR500 I/O peripheral map into CCLI corpus + RAG.
param(
    [string] $RagBase = "http://localhost:8001",
    [switch] $DryRun,
    [switch] $SkipRag
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Kb = Join-Path $Repo "knowledge-base\08-engineering"
$Doc = Join-Path $Kb "CCI_TR500_IO_Peripheral.md"
$TelEng = "C:\Yahia\projects\Telematry System\documents\projects\ccli\engineering"

if (-not (Test-Path $Doc)) {
    Write-Error "Missing corpus doc: $Doc"
}

New-Item -ItemType Directory -Path $TelEng -Force | Out-Null
Copy-Item $Doc (Join-Path $TelEng "CCI_TR500_IO_Peripheral.md") -Force
Write-Host "Synced CCI_TR500_IO_Peripheral.md to Telematry" -ForegroundColor Cyan

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
        Write-Warning "RAG not reachable. Corpus updated; run ingest later."
        exit 0
    }
}

function Submit-IngestJob {
    param(
        [string] $SourceId,
        [string] $ContainerPath
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
        document_version = "1"
        file_path        = $ContainerPath
        idempotency_key  = "$SourceId-v1-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = @{
            project_id = "ccli"
            doc_type   = "hw_io_peripheral"
            platform   = "tespro-tr500"
        }
    } | ConvertTo-Json -Depth 4
    Write-Host "Enqueue: $SourceId"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json"
}

Submit-IngestJob "ccli-tr500-io-peripheral" "/documents/projects/ccli/engineering/CCI_TR500_IO_Peripheral.md"
Write-Host "Done." -ForegroundColor Green
