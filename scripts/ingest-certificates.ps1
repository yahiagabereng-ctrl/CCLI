# Sync Certificate/ competitor samples + extracts to Telematry; enqueue RAG jobs.
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string] $RagBase = "http://localhost:8001",
    [switch] $DryRun,
    [switch] $SkipRag,
    [switch] $SyncOnly
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$CertDir = Join-Path $Repo "Certificate"
$ExtractScript = Join-Path $Repo "scripts\extract-certificates.py"
$TelCert = Join-Path $TelematryRoot "documents\projects\ccli\certificate"
$TelEng = Join-Path $TelematryRoot "documents\projects\ccli\engineering"

New-Item -ItemType Directory -Path $TelCert, (Join-Path $TelCert "_extracts") -Force | Out-Null

if (Test-Path $ExtractScript) {
    python $ExtractScript
}

# Markdown handoff + RAG extract
$mdFiles = @(
    "README.md",
    "CCI_Certificate_Samples_Index.md"
)
$engExtract = Join-Path $Repo "knowledge-base\08-engineering\CCI_Certificate_Samples_Extract.md"
if (Test-Path $engExtract) {
    New-Item -ItemType Directory -Path $TelEng -Force | Out-Null
    Copy-Item $engExtract (Join-Path $TelEng "CCI_Certificate_Samples_Extract.md") -Force
}

foreach ($name in $mdFiles) {
    $src = Join-Path $CertDir $name
    if (Test-Path $src) {
        Copy-Item $src (Join-Path $TelCert $name) -Force
        Write-Host "Synced $name"
    }
}

Get-ChildItem $CertDir -Filter "*.pdf" -ErrorAction SilentlyContinue | ForEach-Object {
    Copy-Item $_.FullName (Join-Path $TelCert $_.Name) -Force
    Write-Host "Synced PDF: $($_.Name)"
}

$extractDir = Join-Path $CertDir "_extracts"
if (Test-Path $extractDir) {
    Get-ChildItem $extractDir -File | Where-Object { $_.Extension -in '.txt', '.md' } | ForEach-Object {
        Copy-Item $_.FullName (Join-Path $TelCert "_extracts\$($_.Name)") -Force
    }
}

# Competitor cert narrative (FAQ + brochure extracts already in corpus)
$compFaq = Join-Path $Repo "competitors\competitors\Higeco\CCI_Compliance_FAQ.html"
if (Test-Path $compFaq) {
    Copy-Item $compFaq (Join-Path $TelCert "Higeco_CCI_Compliance_FAQ.html") -Force
}

if ($SyncOnly) {
    Write-Host "SyncOnly: certificate corpus copied to Telematry." -ForegroundColor Yellow
    exit 0
}

if ($SkipRag) {
    Write-Host "SkipRag: corpus synced only." -ForegroundColor Yellow
    exit 0
}

try {
    $health = Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 8
    Write-Host "RAG status: $($health.status)" -ForegroundColor Green
}
catch {
    Write-Warning "RAG not reachable at $RagBase. Corpus synced only. Start Telematry then re-run:"
    Write-Host "  powershell -File scripts\ingest-certificates.ps1"
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
    $meta = @{ project_id = "ccli"; doc_type = $DocType; cluster = "certificate" }
    foreach ($k in $ExtraMeta.Keys) { $meta[$k] = $ExtraMeta[$k] }
    $body = @{
        tenant_id        = "telematry"
        bot_id           = "default"
        source_id        = $SourceId
        document_id      = $SourceId
        document_version = "1"
        file_path        = $ContainerPath
        idempotency_key  = "$SourceId-cert-$(Get-Date -Format 'yyyyMMddHHmmss')"
        metadata         = $meta
    } | ConvertTo-Json -Depth 5
    Write-Host "Enqueue: $SourceId"
    Invoke-RestMethod -Uri "$RagBase/v1/ingestion/jobs" -Method POST -Body $body -ContentType "application/json"
}

# Priority: samples extract first, then handoff docs, then PDFs
Submit-IngestJob "ccli-certificate-samples-extract" "/documents/projects/ccli/engineering/CCI_Certificate_Samples_Extract.md" "engineering_extract"
Submit-IngestJob "ccli-certificate-readme" "/documents/projects/ccli/certificate/README.md" "certificate_handoff"
Submit-IngestJob "ccli-certificate-samples-index" "/documents/projects/ccli/certificate/CCI_Certificate_Samples_Index.md" "certificate_index"

$pdfJobs = [ordered]@{
    "ccli-cert-ref-uca-61850-higeco"     = "IEC 61850 UCA Standard Group Certificate.pdf"
    "ccli-cert-ref-62443-csa-higeco"     = "IEC 62443-4-2 - Isa Secure CSA-E01.pdf"
    "ccli-cert-ref-62351-3-higeco"       = "IEC-62351-3 Rev1.pdf"
    "ccli-cert-ref-dico-quadro-misura"    = "DI.CO. QUADRO_ACQUISIZIONE E MISURA.pdf"
    "ccli-cert-ref-cei-0-16"              = "CEI 0-16.pdf"
}

foreach ($entry in $pdfJobs.GetEnumerator()) {
    $local = Join-Path $TelCert $entry.Value
    if (-not (Test-Path $local)) {
        Write-Warning "Skip $($entry.Key) - missing $($entry.Value)"
        continue
    }
    $container = "/documents/projects/ccli/certificate/$($entry.Value)"
    Submit-IngestJob $entry.Key $container "competitor_certificate_pdf" @{ vendor = "reference_sample" }
}

foreach ($txt in Get-ChildItem (Join-Path $TelCert "_extracts") -Filter "*.txt" -ErrorAction SilentlyContinue) {
    $sid = "ccli-cert-extract-" + ($txt.BaseName -replace '[^\w\-]', '-').ToLowerInvariant()
    Submit-IngestJob $sid "/documents/projects/ccli/certificate/_extracts/$($txt.Name)" "certificate_text_extract"
}

foreach ($md in Get-ChildItem (Join-Path $TelCert "_extracts") -Filter "*_ocr.md" -ErrorAction SilentlyContinue) {
    $sid = "ccli-cert-ocr-" + ($md.BaseName -replace '[^\w\-]', '-').ToLowerInvariant()
    Submit-IngestJob $sid "/documents/projects/ccli/certificate/_extracts/$($md.Name)" "certificate_ocr_extract"
}

$ocrReadme = Join-Path $TelCert "_extracts\OCR_README.md"
if (Test-Path $ocrReadme) {
    Submit-IngestJob "ccli-cert-ocr-readme" "/documents/projects/ccli/certificate/_extracts/OCR_README.md" "certificate_ocr_index"
}

Submit-IngestJob "ccli-higeco-compliance-faq" "/documents/projects/ccli/certificate/Higeco_CCI_Compliance_FAQ.html" "competitor_faq"

Write-Host "`nDone. Query RAG with source_id ccli-certificate-samples-extract or ccli-certificate-readme." -ForegroundColor Green
