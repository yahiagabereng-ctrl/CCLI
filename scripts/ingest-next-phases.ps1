# Priority RAG ingest for CCLI Phase 6 (Defence I/O / Annex M) and Phase 7 (evidence pack).
# Also refreshes P5 carry-over: 61850-7-3/7-4, TLS/62351, metrology, 104, flowcharts.
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string] $RagBase = "http://localhost:8001",
    [switch] $SkipDocker,
    [switch] $SyncOnly,
    [switch] $DryRun
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$KbEng = Join-Path $Repo "knowledge-base\08-engineering"
$TelEng = Join-Path $TelematryRoot "documents\projects\ccli\engineering"
$TelProto = Join-Path $TelematryRoot "documents\projects\ccli\protocols"
$TelCcli = Join-Path $TelematryRoot "documents\projects\ccli"

New-Item -ItemType Directory -Path $TelEng, $TelProto, $TelCcli -Force | Out-Null

Write-Host "=== [1/4] Sync CCLI corpus to Telematry ===" -ForegroundColor Cyan
& (Join-Path $Repo "scripts\sync-ccli-corpus-to-telematry.ps1") -TelematryRoot $TelematryRoot

# Extra engineering docs not always picked up or missing from ingest-ccli registry
$extraEng = @(
    "CCI_Phase_Regulation_Checklists.md",
    "CCI_TG500_Knowledge_Tree.md",
    "CCI_Application_Structure.md",
    "CCI_OpenWrt_Freeze.md",
    "CCI_CCLI_Product_Spec_and_Roadmap.md",
    "CCI_TestSuitePro_Extract.md",
    "CCI_TestSuitePro_Capture_Plan.md",
    "CCI_ARERA_540_2021_Extract.md"
)
foreach ($name in $extraEng | Select-Object -Unique) {
    $src = Join-Path $KbEng $name
    if (Test-Path $src) {
        Copy-Item $src (Join-Path $TelEng $name) -Force
        Write-Host "  synced $name"
    }
}

# Sync flowcharts (regulation equations O.9.x — P6 PFSP/VArV/PFW)
& (Join-Path $Repo "scripts\ingest-flowcharts.ps1") -TelematryRoot $TelematryRoot -SkipRag

# TesPro 61850 manual (P5 northbound / LuCI)
$tespro = Join-Path $Repo "scripts\ingest-tespro-61850-manual.ps1"
if (Test-Path $tespro) {
    & $tespro -SyncOnly -SkipRag -ErrorAction SilentlyContinue
}

# 62351-9 PDF + extract (product TLS / key mgmt — not in standards drop)
$ing62351 = Join-Path $Repo "scripts\ingest-62351-9.ps1"
if (Test-Path $ing62351) {
    & $ing62351 -SyncOnly -SkipRag -ErrorAction SilentlyContinue
}

if ($SyncOnly) {
    Write-Host "SyncOnly complete." -ForegroundColor Yellow
    exit 0
}

Write-Host "`n=== [2/4] Start Telematry RAG (docker) ===" -ForegroundColor Cyan
if (-not $SkipDocker) {
    Push-Location $TelematryRoot
    try {
        cmd /c "docker compose up -d --force-recreate 2>&1"
        $deadline = (Get-Date).AddMinutes(3)
        do {
            Start-Sleep -Seconds 4
            try {
                $h = Invoke-RestMethod -Uri "$RagBase/health" -TimeoutSec 8
                if ($h.status -eq "ok") { break }
            } catch { }
        } while ((Get-Date) -lt $deadline)
    }
    finally {
        Pop-Location
    }
}

try {
    $health = Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 15
    Write-Host "RAG status: $($health.status)" -ForegroundColor Green
}
catch {
    throw "RAG not reachable at $RagBase. Start: docker compose up -d in $TelematryRoot"
}

Write-Host "`n=== [3/4] Bulk ingest (ingest-ccli -ProjectOnly) ===" -ForegroundColor Cyan
$ingest = Join-Path $TelematryRoot "scripts\ingest-ccli.ps1"
if (-not (Test-Path $ingest)) { throw "Missing $ingest" }
if ($DryRun) {
    & $ingest -ProjectOnly -DryRun
}
else {
    & $ingest -ProjectOnly
}

Write-Host "`n=== [4/4] Priority supplemental jobs (P6/P7 + 61850-7-3/7-4) ===" -ForegroundColor Cyan

# source_id -> path under Telematry documents/
$priority = [ordered]@{
    # P6 — Annex M defence I/O
    "cei-0-16-allegato-m-extract"       = "projects/ccli/engineering/CCI_Annex_M_Extract.md"
    "ccli-phase-regulation-checklists"  = "projects/ccli/engineering/CCI_Phase_Regulation_Checklists.md"
    # P5 carry / P6 regulation depth
    "cei-0-16-allegato-o-extract"       = "projects/ccli/engineering/CCI_Annex_O_Extract.md"
    "cei-0-16-allegato-t-extract"       = "projects/ccli/engineering/CCI_Annex_T_Extract.md"
    "cei-tr-57-126-extract"             = "projects/ccli/engineering/CCI_TR_57-126_Extract.md"
    # 61850 LN/CDC (missing from ingest-ccli registry)
    "ccli-61850-7-3-extract"            = "projects/ccli/engineering/CCI_61850-7-3_Extract.md"
    "ccli-61850-7-4-extract"            = "projects/ccli/engineering/CCI_61850-7-4_Extract.md"
    "ccli-61850-7-3-ocr-corpus"         = "projects/ccli/engineering/CCI_61850-7-3_OCR_Corpus.md"
    "ccli-61850-7-4-ocr-corpus"         = "projects/ccli/engineering/CCI_61850-7-4_OCR_Corpus.md"
    "ccli-61850-7-3-capture-plan"       = "projects/ccli/engineering/CCI_61850-7-3_Capture_Plan.md"
    "ccli-61850-7-4-capture-plan"       = "projects/ccli/engineering/CCI_61850-7-4_Capture_Plan.md"
    "ccli-61850-10-extract"             = "projects/ccli/engineering/CCI_61850-10_Extract.md"
    "ccli-61850-10-ocr-corpus"          = "projects/ccli/reference/iec61850/IEC_61850-10_2025_full.txt"
    "ccli-62351-100-3-extract"          = "projects/ccli/engineering/CCI_62351-100-3_Extract.md"
    "ccli-62351-100-3-ocr-corpus"       = "projects/ccli/reference/iec62351/IEC_TS_62351-100-3_2020_full.txt"
    "ccli-lab-conformance-questionnaire" = "projects/ccli/lab/meetings/2026-10-08_Lab_Conformance_Questionnaire.md"
    "ccli-lab-uca-document-checklist"   = "projects/ccli/lab/conformance/2026-10-08_UCA_Document_Phase_Checklist.md"
    "ccli-lab-requirement-test-matrix"  = "projects/ccli/lab/conformance/LAB_REQUIREMENT_TEST_MATRIX.md"
    "ccli-tsp-field-pack"               = "projects/ccli/lab/conformance/TSP_PC_FIELD_PACK.md"
    "ccli-tsp-test-identification"      = "projects/ccli/lab/evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md"
    "ccli-lab-templates-test-scope"     = "projects/ccli/lab/conformance/LAB_TEMPLATES_VS_TEST_SCOPE.md"
    "ccli-tsp-full-bench-hw"            = "projects/ccli/lab/conformance/TSP_PC_FULL_BENCH_HW.md"
    "ccli-tsp-two-pc-bench-runbook"     = "projects/ccli/lab/conformance/TSP_TWO_PC_BENCH_RUNBOOK.md"
    "ccli-lab-pics-fill"               = "projects/ccli/lab/conformance/PICS_FILL.md"
    "ccli-lab-pixit-draft"             = "projects/ccli/lab/conformance/PIXIT_DRAFT.md"
    "ccli-lab-mics-draft"              = "projects/ccli/lab/conformance/MICS_DRAFT.md"
    "ccli-lab-tics-draft"              = "projects/ccli/lab/conformance/TICS_DRAFT.md"
    "ccli-lab-d7-62351-3-pid"          = "projects/ccli/lab/conformance/D7_62351-3_PID_DRAFT.md"
    "ccli-lab-cid-pics-alignment"      = "projects/ccli/lab/conformance/CID_PICS_ALIGNMENT.md"
    "ccli-lab-config-guide"            = "projects/ccli/lab/conformance/LAB_CONFIG_GUIDE.md"
    # Product TLS path (revert cleartext lab)
    "ccli-62351-9-pdf"                  = "projects/ccli/reference/IEC_62351-9_2023.pdf"
    "ccli-tespro-61850-manual-extract"  = "projects/ccli/engineering/CCI_TesPro_61850_Manual_Extract.md"
    # P7 evidence / test tooling
    "ccli-testsuitepro-extract"         = "projects/ccli/engineering/CCI_TestSuitePro_Extract.md"
    "arera-540-2021-extract"            = "projects/ccli/engineering/CCI_ARERA_540_2021_Extract.md"
}

$docsRoot = Join-Path $TelematryRoot "documents"
$enqueued = 0
foreach ($entry in $priority.GetEnumerator()) {
    $sourceId = $entry.Key
    $relative = $entry.Value
    $local = Join-Path $docsRoot ($relative -replace '/', '\')
    if (-not (Test-Path $local)) {
        Write-Warning "Skip $sourceId - missing $local"
        continue
    }
    $container = "/documents/$($relative -replace '\\', '/')"
    if ($DryRun) {
        Write-Host "[dry-run] $sourceId -> $container"
        continue
    }
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $sourceId
        document_id      = $sourceId
        document_version = "1"
        file_path        = $container
        idempotency_key  = "$sourceId-p6-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = @{ project_id = "ccli"; cluster = "next-phases"; phase = "P6-P7" }
    } | ConvertTo-Json -Depth 4
    Write-Host "Enqueue: $sourceId"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json" | Out-Null
    $enqueued++
}

Write-Host ("`nDone. Bulk ingest via ingest-ccli + {0} supplemental priority jobs." -f $enqueued) -ForegroundColor Green
