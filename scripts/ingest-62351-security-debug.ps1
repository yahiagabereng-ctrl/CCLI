# Ingest IEC 62351-3/4/8/9 MMS security debug map + parent extracts into RAG.
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string] $RagBase = "http://localhost:8001",
    [switch] $SyncOnly,
    [switch] $DryRun
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Kb = Join-Path $Repo "knowledge-base\08-engineering"
$TelEng = Join-Path $TelematryRoot "documents\projects\ccli\engineering"
$TelRef = Join-Path $TelematryRoot "documents\projects\ccli\reference"

New-Item -ItemType Directory -Path $TelEng, $TelRef -Force | Out-Null

$files = @(
    "CCI_62351_MMS_Security_Debug_Map.md",
    "CCI_62351-3_Extract.md",
    "CCI_62351-4_Extract.md",
    "CCI_62351-8_Extract.md",
    "CCI_62351-9_Extract.md",
    "CCI_62351-3_Capture_Plan.md",
    "CCI_62351-4_Capture_Plan.md",
    "CCI_62351-8_Capture_Plan.md",
    "CCI_Annex_T_Extract.md",
    "CCI_K6.3_Key_Ceremony_SOP.md"
)

Write-Host "=== Sync 62351 security corpus ===" -ForegroundColor Cyan
foreach ($name in $files) {
    $src = Join-Path $Kb $name
    if (-not (Test-Path $src)) {
        Write-Warning "Missing $name"
        continue
    }
    Copy-Item $src (Join-Path $TelEng $name) -Force
    Write-Host "  synced $name"
}

$pdf = Join-Path $Kb "reference\IEC_62351-9_2023.pdf"
if (Test-Path $pdf) {
    Copy-Item $pdf (Join-Path $TelRef "IEC_62351-9_2023.pdf") -Force
    Write-Host "  synced IEC_62351-9_2023.pdf"
}

$tlsReadme = Join-Path $Repo "apps\ccli\config\tls\README.md"
if (Test-Path $tlsReadme) {
    Copy-Item $tlsReadme (Join-Path $TelEng "TLS_Lab_PKI_README.md") -Force
    Write-Host "  synced TLS_Lab_PKI_README.md"
}

if ($SyncOnly) {
    Write-Host "SyncOnly complete." -ForegroundColor Yellow
    exit 0
}

try {
    Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 10 | Out-Null
}
catch {
    Write-Warning "RAG offline at $RagBase. Corpus synced; start docker compose and re-run."
    exit 0
}

$jobs = [ordered]@{
    "ccli-62351-mms-security-debug-map" = "projects/ccli/engineering/CCI_62351_MMS_Security_Debug_Map.md"
    "ccli-62351-3-extract"              = "projects/ccli/engineering/CCI_62351-3_Extract.md"
    "ccli-62351-4-extract"              = "projects/ccli/engineering/CCI_62351-4_Extract.md"
    "ccli-62351-8-extract"              = "projects/ccli/engineering/CCI_62351-8_Extract.md"
    "ccli-62351-9-extract"              = "projects/ccli/engineering/CCI_62351-9_Extract.md"
    "cei-0-16-allegato-t-extract"       = "projects/ccli/engineering/CCI_Annex_T_Extract.md"
    "ccli-k63-ceremony-sop"             = "projects/ccli/engineering/CCI_K6.3_Key_Ceremony_SOP.md"
    "ccli-tls-lab-pki-readme"           = "projects/ccli/engineering/TLS_Lab_PKI_README.md"
}

$docsRoot = Join-Path $TelematryRoot "documents"
$n = 0
foreach ($entry in $jobs.GetEnumerator()) {
    $local = Join-Path $docsRoot ($entry.Value -replace '/', '\')
    if (-not (Test-Path $local)) {
        Write-Warning "Skip $($entry.Key) - missing"
        continue
    }
    if ($DryRun) {
        Write-Host "[dry-run] $($entry.Key)"
        continue
    }
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $entry.Key
        document_id      = $entry.Key
        document_version = "2"
        file_path        = "/documents/$($entry.Value -replace '\\', '/')"
        idempotency_key  = "$($entry.Key)-62351dbg-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = @{ project_id = "ccli"; cluster = "62351-mms-security"; priority = "P0-debug" }
    } | ConvertTo-Json -Depth 4
    Write-Host "Enqueue: $($entry.Key)"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json" | Out-Null
    $n++
}

Write-Host "Enqueued $n 62351 security ingest jobs." -ForegroundColor Green
