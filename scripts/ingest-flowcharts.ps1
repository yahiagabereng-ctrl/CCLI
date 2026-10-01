# Sync Flowcharts/ + draw.io RAG extract to Telematry; enqueue RAG ingestion jobs.
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string] $RagBase = "http://localhost:8001",
    [switch] $DryRun,
    [switch] $SkipRag,
    [switch] $SkipExtract
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Flow = Join-Path $Repo "Flowcharts"
$Kb = Join-Path $Repo "knowledge-base\08-engineering"
$Extract = Join-Path $Kb "CCI_Flowcharts_Drawio_Extract.md"
$TelFlow = Join-Path $TelematryRoot "documents\projects\ccli\flowcharts"
$TelEng = Join-Path $TelematryRoot "documents\projects\ccli\engineering"

New-Item -ItemType Directory -Path $TelFlow, $TelEng -Force | Out-Null

if (-not $SkipExtract) {
    $py = Get-Command python -ErrorAction SilentlyContinue
    if (-not $py) { throw "python not found" }
    & $py (Join-Path $Repo "scripts\extract-flowcharts-rag.py")
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

Write-Host "=== Sync Flowcharts to Telematry ===" -ForegroundColor Cyan
if ($DryRun) {
    Write-Host "DRY: copy $Flow -> $TelFlow"
    Write-Host "DRY: copy $Extract -> $TelEng"
}
else {
    Copy-Item (Join-Path $Flow "*") $TelFlow -Recurse -Force
    if (Test-Path $Extract) {
        Copy-Item $Extract (Join-Path $TelEng "CCI_Flowcharts_Drawio_Extract.md") -Force
    }
    Write-Host "Synced Flowcharts/ and draw.io extract"
}

$RagSources = @(
    @{ Sid = "ccli-flowcharts-params-equations"; Rel = "projects/ccli/flowcharts/PARAMETERS_AND_EQUATIONS.md" },
    @{ Sid = "ccli-flowcharts-audit"; Rel = "projects/ccli/flowcharts/FLOWCHARTS_CROSSCHECK_AUDIT.md" },
    @{ Sid = "ccli-flowcharts-readme"; Rel = "projects/ccli/flowcharts/README.md" },
    @{ Sid = "ccli-flowcharts-drawio-extract"; Rel = "projects/ccli/engineering/CCI_Flowcharts_Drawio_Extract.md" }
)

if ($SkipRag) {
    Write-Host "SkipRag - corpus synced only. Re-run when RAG is up." -ForegroundColor Yellow
    exit 0
}

if (-not $DryRun) {
    try {
        $health = Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 8
        Write-Host "RAG status: $($health.status)" -ForegroundColor Green
    }
    catch {
        Write-Warning "RAG not reachable at $RagBase. Corpus synced; start Telematry RAG and re-run without -SkipRag."
        exit 0
    }
}

function Submit-IngestJob {
    param([string] $SourceId, [string] $Relative)
    $local = Join-Path $TelematryRoot "documents" ($Relative -replace '/', '\')
    if (-not (Test-Path $local)) {
        Write-Warning "Missing $SourceId : $local"
        return
    }
    $container = "/documents/$($Relative -replace '\\', '/')"
    if ($DryRun) {
        Write-Host "[dry-run] $SourceId -> $container"
        return
    }
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $SourceId
        document_id      = $SourceId
        document_version = "1"
        file_path        = $container
        idempotency_key  = "$SourceId-v1-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = @{ project_id = "ccli"; doc_type = "flowcharts"; cluster = "regulation-equations" }
    } | ConvertTo-Json -Depth 4
    Write-Host "Enqueue: $SourceId"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json"
}

Write-Host "=== RAG ingest ($RagBase) ===" -ForegroundColor Cyan
foreach ($s in $RagSources) {
    Submit-IngestJob -SourceId $s.Sid -Relative $s.Rel
}
Write-Host "Done." -ForegroundColor Green
