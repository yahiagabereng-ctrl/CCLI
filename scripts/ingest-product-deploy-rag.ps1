# Enqueue product-deployment P0 RAG sources from rag-product-deploy-sources.json
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string] $RagBase = "http://localhost:8001",
    [switch] $SyncOnly,
    [switch] $SkipSync,
    [switch] $DryRun
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$RegistryPath = Join-Path $Repo "scripts\rag-product-deploy-sources.json"

if (-not (Test-Path $RegistryPath)) {
    throw "Missing registry: $RegistryPath"
}

if (-not $SkipSync) {
    Write-Host "=== Sync CCLI corpus (incl. product-deploy paths) ===" -ForegroundColor Cyan
    & (Join-Path $Repo "scripts\sync-ccli-corpus-to-telematry.ps1") -TelematryRoot $TelematryRoot
}

if ($SyncOnly) {
    Write-Host "SyncOnly complete (product-deploy registry not enqueued)." -ForegroundColor Yellow
    exit 0
}

$registry = Get-Content $RegistryPath -Raw | ConvertFrom-Json
$docsRoot = Join-Path $TelematryRoot "documents"
$enqueued = 0
$skipped = 0

if (-not $DryRun) {
    try {
        $health = Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 8
        if ($health.status -ne "ok") { throw "RAG health not ok" }
    }
    catch {
        Write-Warning "RAG not reachable at $RagBase - run with -SyncOnly or start Telematry docker."
        exit 2
    }
}

foreach ($s in $registry.sources) {
    $sourceId = $s.source_id
    $relative = $s.relative
    $local = Join-Path $docsRoot ($relative -replace '/', '\')
    if (-not (Test-Path $local)) {
        Write-Warning "Skip $sourceId - missing $local"
        $skipped++
        continue
    }
    $container = "/documents/$($relative -replace '\\', '/')"
    if ($DryRun) {
        Write-Host "[dry-run] $sourceId -> $container ($($s.cluster))"
        $enqueued++
        continue
    }
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $sourceId
        document_id      = $sourceId
        document_version = "1"
        file_path        = $container
        idempotency_key  = "$sourceId-prod-deploy-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = @{
            project_id = "ccli"
            cluster    = $s.cluster
            phase      = "product-deploy"
        }
    } | ConvertTo-Json -Depth 5
    Write-Host "Enqueue: $sourceId"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json" | Out-Null
    $enqueued++
}

Write-Host ("Done. Enqueued {0}, skipped {1} (missing mirror)." -f $enqueued, $skipped) -ForegroundColor Green
