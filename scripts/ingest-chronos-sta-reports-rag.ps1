# Sync Chronos S.T.A. corpus + enqueue STA / vendor map RAG jobs; optional chunk score check.
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string] $RagBase = "http://localhost:8001",
    [switch] $SyncOnly,
    [switch] $SkipSync,
    [switch] $CheckScoresOnly
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot

$jobs = @(
    @{ id = "ccli-chronos-sta-test-reports"; rel = "projects/ccli/engineering/CCI_Chronos_STA_Test_Reports_Extract.md" },
    @{ id = "ccli-chronos-sta-annex-map"; rel = "projects/ccli/lab/conformance/CHRONOS_STA_REPORTS_ANNEX_MAP.md" },
    @{ id = "ccli-chronos-emt432-vendor-requirements"; rel = "projects/ccli/engineering/CCI_Chronos_EMT432_Vendor_Requirements.md" },
    @{ id = "ccli-61557-12-tespro-vendor-map"; rel = "projects/ccli/lab/conformance/61557-12_TESPRO_VENDOR_MAP.md" }
)

if (-not $CheckScoresOnly) {
    if (-not $SkipSync) {
        & (Join-Path $Repo "scripts\sync-ccli-corpus-to-telematry.ps1") -TelematryRoot $TelematryRoot
    }
    if ($SyncOnly) {
        Write-Host "SyncOnly - RAG enqueue skipped." -ForegroundColor Yellow
        exit 0
    }
    try {
        $h = Invoke-RestMethod "$RagBase/health" -TimeoutSec 8
        if ($h.status -ne "ok") { throw "health not ok" }
    }
    catch {
        Write-Warning "RAG offline at $RagBase. Sync done; re-run without -SyncOnly."
        exit 2
    }
    $docsRoot = Join-Path $TelematryRoot "documents"
    foreach ($j in $jobs) {
        $local = Join-Path $docsRoot ($j.rel -replace '/', '\')
        if (-not (Test-Path $local)) {
            Write-Warning "Missing mirror: $($j.id) -> $local"
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
            idempotency_key  = "$($j.id)-sta-$(Get-Date -Format 'yyyyMMddHHmmss')"
            metadata         = @{ project_id = "ccli"; cluster = "lab-specimen" }
        } | ConvertTo-Json -Depth 5
        Write-Host "Enqueue $($j.id)" -ForegroundColor Cyan
        $maxTry = 4
        for ($t = 1; $t -le $maxTry; $t++) {
            try {
                Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json" -TimeoutSec 90 | Out-Null
                break
            }
            catch {
                $msg = $_.Exception.Message
                if ($t -lt $maxTry -and ($msg -match '500|locked|Internal Server')) {
                    Write-Host "  retry $t/$maxTry (SQLite lock or 500)..." -ForegroundColor Yellow
                    Start-Sleep -Seconds (2 * $t)
                    continue
                }
                throw
            }
        }
    }
    Write-Host "Jobs enqueued. Wait 1-3 min then re-run with -CheckScoresOnly" -ForegroundColor Green
}

$check = Join-Path $Repo "lab\tmp\check_rag_chunk_scores.py"
if (Test-Path $check) {
    $ids = @($jobs | ForEach-Object { $_.id })
    if ($CheckScoresOnly) {
        python $check @ids
    }
}
elseif ($CheckScoresOnly) {
    Write-Warning "Missing $check"
}
