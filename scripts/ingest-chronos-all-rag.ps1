# Sync all Chronos corpus (PDF + MD) to Telematry and enqueue RAG jobs; validate chunk scores.
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string] $RagBase = "http://localhost:8001",
    [switch] $SyncOnly,
    [switch] $SkipSync,
    [switch] $CheckScoresOnly,
    [int] $WaitSeconds = 120
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot

# source_id -> path under Telematry documents/
$ChronosJobs = @(
    # Engineering extracts
    @{ id = "ccli-chronos-emt432"; rel = "projects/ccli/engineering/CCI_Chronos_EMT432_Extract.md"; kind = "md" },
    @{ id = "ccli-chronos-rogowski-bench"; rel = "projects/ccli/engineering/CCI_Chronos_Rogowski_Bench_Extract.md"; kind = "md" },
    @{ id = "ccli-chronos-emt432-vendor-requirements"; rel = "projects/ccli/engineering/CCI_Chronos_EMT432_Vendor_Requirements.md"; kind = "md" },
    @{ id = "ccli-chronos-sta-test-reports"; rel = "projects/ccli/engineering/CCI_Chronos_STA_Test_Reports_Extract.md"; kind = "md" },
    @{ id = "ccli-chronos-emt432-ui"; rel = "projects/ccli/engineering/CCI_Chronos_EMT432_Web_UI_Capabilities.md"; kind = "md" },
    @{ id = "ccli-chronos-sta-annex-map"; rel = "projects/ccli/lab/conformance/CHRONOS_STA_REPORTS_ANNEX_MAP.md"; kind = "md" },
    @{ id = "ccli-meter-specimen-emt432-v-full"; rel = "projects/ccli/lab/conformance/METER_SPECIMEN_EMT432_V_FULL.md"; kind = "md" },
    @{ id = "ccli-emt432-cert-register-pin"; rel = "projects/ccli/lab/conformance/EMT432_CERT_REGISTER_PIN.md"; kind = "md" },
    @{ id = "ccli-emt432-commissioning-map"; rel = "projects/ccli/lab/conformance/EMT432_CCLI_COMMISSIONING_MAP.md"; kind = "md" },
    @{ id = "ccli-emt432-meter-code-freeze"; rel = "projects/ccli/lab/conformance/EMT432_METER_CODE_FREEZE.md"; kind = "md" },
    @{ id = "ccli-emt432-mr-register-validation"; rel = "projects/ccli/lab/conformance/EMT432_MR_REGISTER_VALIDATION.md"; kind = "md" },
    @{ id = "ccli-emt432-portal-step-by-step"; rel = "projects/ccli/lab/evidence/phase5/P5_EMT432_PORTAL_STEP_BY_STEP.md"; kind = "md" },
    @{ id = "ccli-lab-p5-rogowski-frc350-bench"; rel = "projects/ccli/lab/evidence/phase5/P5_ROGOWSKI_FRC350_BENCH.md"; kind = "md" },
    # Vendor PDFs
    @{ id = "ccli-chronos-pdf-emt432-mi-eng"; rel = "projects/ccli/reference/vendor/chronos/EMT432_MI-ENG.pdf"; kind = "pdf" },
    @{ id = "ccli-chronos-pdf-emt432-mi-ita"; rel = "projects/ccli/reference/vendor/chronos/EMT432_MI-ITA.pdf"; kind = "pdf" },
    @{ id = "ccli-chronos-pdf-emt432-fl-ita"; rel = "projects/ccli/reference/vendor/chronos/EMT432_FL-ITA.pdf"; kind = "pdf" },
    @{ id = "ccli-chronos-pdf-emt430-mr"; rel = "projects/ccli/reference/vendor/chronos/EMT430_MR.pdf"; kind = "pdf" },
    @{ id = "ccli-chronos-pdf-ta-fl-ita"; rel = "projects/ccli/reference/vendor/chronos/TA_FL_ITA.pdf"; kind = "pdf" },
    @{ id = "ccli-chronos-pdf-sta-emc-emt130"; rel = "projects/ccli/reference/vendor/chronos/2026-sta-reports/C25305-ACR_01_EMT130_EMC_61326-1.pdf"; kind = "pdf" },
    @{ id = "ccli-chronos-pdf-sta-emc-emt430"; rel = "projects/ccli/reference/vendor/chronos/2026-sta-reports/C25305-ACR_02_EMT430_EMC_61326-1.pdf"; kind = "pdf" },
    @{ id = "ccli-chronos-pdf-sta-61010-emt130"; rel = "projects/ccli/reference/vendor/chronos/2026-sta-reports/C25306_01_EMT130_61010-2-030.pdf"; kind = "pdf" },
    @{ id = "ccli-chronos-pdf-sta-61010-emt430"; rel = "projects/ccli/reference/vendor/chronos/2026-sta-reports/C25306_01_EMT430_61010-2-030.pdf"; kind = "pdf" }
)

function Enqueue-RagJob {
    param([hashtable] $Job, [string] $DocsRoot, [string] $RagBase)
    $local = Join-Path $DocsRoot ($Job.rel -replace '/', '\')
    if (-not (Test-Path $local)) {
        Write-Warning "MISSING mirror: $($Job.id) -> $local"
        return $null
    }
    $container = "/documents/$($Job.rel -replace '\\', '/')"
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $Job.id
        document_id      = $Job.id
        document_version = "1"
        file_path        = $container
        idempotency_key  = "$($Job.id)-all-$(Get-Date -Format 'yyyyMMddHHmmssfff')"
        metadata         = @{
            project_id = "ccli"
            cluster    = "lab-specimen"
            vendor     = "chronos"
            kind       = $Job.kind
        }
    } | ConvertTo-Json -Depth 5
    $maxTry = 5
    for ($t = 1; $t -le $maxTry; $t++) {
        try {
            $resp = Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json" -TimeoutSec 120
            return $resp.job_id
        }
        catch {
            if ($t -lt $maxTry -and ($_.Exception.Message -match '500|locked|Internal Server')) {
                Write-Host "  retry $t/$maxTry $($Job.id)..." -ForegroundColor Yellow
                Start-Sleep -Seconds (3 * $t)
                continue
            }
            throw
        }
    }
    return $null
}

if (-not $CheckScoresOnly) {
    if (-not $SkipSync) {
        Write-Host "=== Sync CCLI corpus (incl. Chronos PDFs) ===" -ForegroundColor Cyan
        & (Join-Path $Repo "scripts\sync-ccli-corpus-to-telematry.ps1") -TelematryRoot $TelematryRoot
    }
    if ($SyncOnly) {
        Write-Host "SyncOnly complete." -ForegroundColor Yellow
        exit 0
    }
    $h = Invoke-RestMethod "$RagBase/health" -TimeoutSec 10
    if ($h.status -ne "ok") { throw "RAG not healthy" }
    $docsRoot = Join-Path $TelematryRoot "documents"
    $jobIds = @()
    foreach ($j in $ChronosJobs) {
        Write-Host "Enqueue $($j.id) [$($j.kind)]" -ForegroundColor Cyan
        $jid = Enqueue-RagJob -Job $j -DocsRoot $docsRoot -RagBase $RagBase
        if ($jid) { $jobIds += $jid }
        if ($j.kind -eq "pdf") { Start-Sleep -Seconds 2 }
    }
    Write-Host "Waiting up to ${WaitSeconds}s for $($jobIds.Count) job(s)..." -ForegroundColor DarkGray
    $deadline = (Get-Date).AddSeconds($WaitSeconds)
    foreach ($jid in $jobIds) {
        while ((Get-Date) -lt $deadline) {
            $st = Invoke-RestMethod "$RagBase/v1/ingestion/jobs/$jid" -TimeoutSec 30
            if ($st.state -in @("completed", "failed", "dead_letter")) {
                $color = if ($st.state -eq "completed") { "Green" } else { "Red" }
                Write-Host "  $jid -> $($st.state) $($st.error_message)" -ForegroundColor $color
                break
            }
            Start-Sleep -Seconds 3
        }
    }
}

$checkPy = Join-Path $Repo "lab\tmp\check_rag_chunk_scores.py"
$ids = @($ChronosJobs | ForEach-Object { $_.id })
python $checkPy --chronos-pack @ids
exit $LASTEXITCODE
