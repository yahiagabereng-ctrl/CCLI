# Sync EJBCA/PKI corpus (RFCs, extracts, EJBCA docs, TSP evidence) to Telematry; enqueue RAG.
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string] $RagBase = "http://localhost:8001",
    [switch] $DryRun,
    [switch] $SkipRag,
    [switch] $SyncOnly
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Kb = Join-Path $Repo "knowledge-base\08-engineering"
$RefPki = Join-Path $Kb "reference\pki"
$RefEjbca = Join-Path $Kb "reference\vendor\ejbca"
$TelEng = Join-Path $TelematryRoot "documents\projects\ccli\engineering"
$TelPki = Join-Path $TelematryRoot "documents\projects\ccli\pki"
$TelRefPki = Join-Path $TelPki "rfc"
$TelRefEjbca = Join-Path $TelPki "ejbca"
$TelTspEvidence = Join-Path $TelPki "tsp-evidence"

New-Item -ItemType Directory -Force -Path $TelEng, $TelPki, $TelRefPki, $TelRefEjbca, $TelTspEvidence | Out-Null

# Ensure RFCs present
& (Join-Path $Repo "scripts\sync-pki-rfc-reference.ps1") -RefPki $RefPki

$engFiles = @(
    "CCI_EJBCA_PKI_Extract.md",
    "CCI_EJBCA_PKI_Capture_Plan.md",
    "CCI_K6.3_Key_Ceremony_SOP.md",
    "CCI_62351-9_Extract.md",
    "CCI_62351-4_Extract.md",
    "CCI_62351-3_Extract.md",
    "CCI_62351_MMS_Security_Debug_Map.md",
    "CCI_Annex_T_Extract.md"
)

Write-Host "=== Sync EJBCA/PKI engineering markdown ===" -ForegroundColor Cyan
foreach ($name in $engFiles) {
    $src = Join-Path $Kb $name
    if (-not (Test-Path $src)) { Write-Warning "Missing $name"; continue }
    Copy-Item $src (Join-Path $TelEng $name) -Force
    Write-Host "  synced $name"
}

Write-Host "=== Sync RFC + EJBCA reference ===" -ForegroundColor Cyan
Get-ChildItem $RefPki -File | ForEach-Object {
    Copy-Item $_.FullName (Join-Path $TelRefPki $_.Name) -Force
    Write-Host "  synced rfc/$($_.Name)"
}
if (Test-Path $RefEjbca) {
    Copy-Item -Recurse -Force $RefEjbca (Join-Path $TelPki "ejbca")
    Write-Host "  synced ejbca/ tree"
}

$tspEvidence = @(
    "TSP_P3_07_ACSE_CERT_2026-09-29.md",
    "TSP_P3_07_62351-9_CROSSCHECK_2026-09-30.md",
    "TSP_P3_07_SIGNATURE_ANALYSIS_2026-09-29.md",
    "TSP_P3_07_FINAL_2026-09-29.md"
)
$tspInbox = Join-Path $Repo "lab\evidence\testsuite-pro\inbox"
foreach ($name in $tspEvidence) {
    $src = Join-Path $tspInbox $name
    if (-not (Test-Path $src)) { Write-Warning "Missing TSP evidence $name"; continue }
    Copy-Item $src (Join-Path $TelTspEvidence $name) -Force
    Write-Host "  synced tsp-evidence/$name"
}

$tlsReadme = Join-Path $Repo "apps\ccli\config\tls\README.md"
if (Test-Path $tlsReadme) {
    Copy-Item $tlsReadme (Join-Path $TelEng "TLS_Lab_PKI_README.md") -Force
}

if ($SyncOnly) {
    Write-Host "SyncOnly: PKI corpus copied to Telematry." -ForegroundColor Yellow
    exit 0
}

if ($SkipRag) {
    Write-Host "SkipRag: corpus synced only." -ForegroundColor Yellow
    exit 0
}

try {
    Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 8 | Out-Null
    Write-Host "RAG online at $RagBase" -ForegroundColor Green
}
catch {
    Write-Warning "RAG not reachable at $RagBase. Corpus synced only. Re-run when Telematry stack is up."
    exit 0
}

function Submit-IngestJob {
    param(
        [string] $SourceId,
        [string] $ContainerPath,
        [string] $DocType,
        [hashtable] $ExtraMeta = @{}
    )
    if ($DryRun) {
        Write-Host "[dry-run] $SourceId -> $ContainerPath"
        return
    }
    $meta = @{ project_id = "ccli"; doc_type = $DocType; cluster = "pki-ejbca" }
    foreach ($k in $ExtraMeta.Keys) { $meta[$k] = $ExtraMeta[$k] }
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $SourceId
        document_id      = $SourceId
        document_version = "1"
        file_path        = $ContainerPath
        idempotency_key  = "$SourceId-pki-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = $meta
    } | ConvertTo-Json -Depth 5
    Write-Host "Enqueue: $SourceId"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json" | Out-Null
}

$jobs = [ordered]@{
    "ccli-ejbca-pki-extract"       = "/documents/projects/ccli/engineering/CCI_EJBCA_PKI_Extract.md"
    "ccli-ejbca-pki-capture-plan"  = "/documents/projects/ccli/engineering/CCI_EJBCA_PKI_Capture_Plan.md"
    "ccli-k63-ceremony-sop"        = "/documents/projects/ccli/engineering/CCI_K6.3_Key_Ceremony_SOP.md"
    "ccli-62351-9-extract"         = "/documents/projects/ccli/engineering/CCI_62351-9_Extract.md"
    "ccli-rfc7030-est"             = "/documents/projects/ccli/pki/rfc/RFC7030_EST.txt"
    "ccli-rfc8894-scep"            = "/documents/projects/ccli/pki/rfc/RFC8894_SCEP.txt"
    "ccli-rfc5280-x509"            = "/documents/projects/ccli/pki/rfc/RFC5280_X509.txt"
    "ccli-rfc6960-ocsp"            = "/documents/projects/ccli/pki/rfc/RFC6960_OCSP.txt"
    "ccli-ejbca-est-overview"      = "/documents/projects/ccli/pki/ejbca/docs/EST_Overview.md"
    "ccli-ejbca-cert-profiles"     = "/documents/projects/ccli/pki/ejbca/docs/Certificate_Profiles_Overview.md"
    "ccli-ejbca-readme"            = "/documents/projects/ccli/pki/ejbca/README.md"
}

foreach ($entry in $jobs.GetEnumerator()) {
    $local = Join-Path $TelematryRoot "documents$($entry.Value -replace '/documents','' -replace '/','\')"
    if (-not (Test-Path $local)) {
        Write-Warning "Skip $($entry.Key) - missing $local"
        continue
    }
    $dtype = if ($entry.Value -match '\.txt$') { "rfc_reference" } else { "engineering_extract" }
    Submit-IngestJob $entry.Key $entry.Value $dtype
}

foreach ($name in $tspEvidence) {
    $local = Join-Path $TelTspEvidence $name
    if (-not (Test-Path $local)) { continue }
    $sid = "ccli-tsp-pki-" + ($name -replace '\.md$','' -replace '[^\w\-]','-').ToLowerInvariant()
    Submit-IngestJob $sid "/documents/projects/ccli/pki/tsp-evidence/$name" "tsp_pki_evidence"
}

Write-Host "`nDone. Query RAG: ccli-ejbca-pki-extract" -ForegroundColor Green
