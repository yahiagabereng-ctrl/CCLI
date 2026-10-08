# Ingest Test Suite Pro corpus + lab logs into Telematry mirror + optional RAG HTTP.
param(
    [string] $RagBase = "http://localhost:8001",
    [switch] $DryRun,
    [switch] $SkipRag
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Kb = Join-Path $Repo "knowledge-base\08-engineering"
$RawRoot = Join-Path $Kb "testsuite-pro"
$Raw = Join-Path $RawRoot "raw"
$Ev = Join-Path $Repo "lab\evidence\testsuite-pro"
$Proc = Join-Path $Repo "lab\CCI_TestSuitePro_Verification_Layer.md"
$TelEng = "C:\Yahia\projects\Telematry System\documents\projects\ccli\engineering"
$TelLab = "C:\Yahia\projects\Telematry System\documents\projects\ccli\lab"

New-Item -ItemType Directory -Path $TelEng, $TelLab -Force | Out-Null

$Conf = Join-Path $Repo "lab\conformance"
$Files = @(
    @{ Path = (Join-Path $Kb "CCI_TestSuitePro_Capture_Plan.md"); Sid = "ccli-testsuite-pro-capture-plan" },
    @{ Path = (Join-Path $Kb "CCI_TestSuitePro_Extract.md"); Sid = "ccli-testsuite-pro-extract" },
    @{ Path = $Proc; Sid = "ccli-testsuite-pro-procedure" },
    @{ Path = (Join-Path $Conf "LAB_REQUIREMENT_TEST_MATRIX.md"); Sid = "ccli-lab-requirement-test-matrix" },
    @{ Path = (Join-Path $Conf "TSP_PC_FIELD_PACK.md"); Sid = "ccli-tsp-field-pack" },
    @{ Path = (Join-Path $Repo "lab\evidence\testsuite-pro\TSP_TEST_IDENTIFICATION_POST_CONNECT.md"); Sid = "ccli-tsp-test-identification" }
)

Write-Host "=== Sync markdown to Telematry ==="
foreach ($f in $Files) {
    if (-not (Test-Path $f.Path)) { Write-Warning ("Missing " + $f.Path); continue }
    if ($f.Path -like "*\lab\*") {
        $dest = Join-Path $TelLab (Split-Path $f.Path -Leaf)
    } else {
        $dest = Join-Path $TelEng (Split-Path $f.Path -Leaf)
    }
    if ($DryRun) { Write-Host ("DRY copy " + $f.Path + " -> " + $dest); continue }
    Copy-Item $f.Path $dest -Force
    Write-Host ("Synced " + $f.Sid + " -> " + $dest)
}

$TelTsp = Join-Path $TelLab "testsuite-pro"
if (-not $DryRun) {
    New-Item -ItemType Directory -Path $TelTsp -Force | Out-Null
    if (Test-Path $RawRoot) {
        Copy-Item (Join-Path $RawRoot "*") $TelTsp -Recurse -Force -ErrorAction SilentlyContinue
    }
    if (Test-Path $Ev) {
        $TelEv = Join-Path $TelLab "evidence-testsuite-pro"
        if (Test-Path $TelEv) { Remove-Item $TelEv -Recurse -Force -ErrorAction SilentlyContinue }
        Copy-Item $Ev $TelEv -Recurse -Force -ErrorAction SilentlyContinue
    }
}

if ($SkipRag) {
    Write-Host "SkipRag - corpus files synced only"
    exit 0
}

function Post-RagFile {
    param([string]$Path, [string]$SourceId)
    if (-not (Test-Path $Path)) { return }
    $body = @{
        path      = $Path
        source_id = $SourceId
        project   = "ccli"
    } | ConvertTo-Json
    try {
        Invoke-RestMethod -Uri ($RagBase + "/ingest") -Method Post -Body $body -ContentType "application/json" -TimeoutSec 30 | Out-Null
        Write-Host ("RAG OK " + $SourceId)
    }
    catch {
        Write-Warning ("RAG skip " + $SourceId + " : " + $_ + " (is " + $RagBase + " up?)")
    }
}

Write-Host ("=== RAG ingest (" + $RagBase + ") ===")
if ($DryRun) {
    Write-Host "DRY RAG would post capture-plan, extract, procedure, raw/*.txt, evidence logs"
    exit 0
}

foreach ($f in $Files) { Post-RagFile -Path $f.Path -SourceId $f.Sid }

if (Test-Path $Raw) {
    Get-ChildItem $Raw -Filter *.txt -ErrorAction SilentlyContinue | ForEach-Object {
        Post-RagFile -Path $_.FullName -SourceId "ccli-testsuite-pro-raw"
    }
}

if (Test-Path $Ev) {
    Get-ChildItem $Ev -Recurse -Include *.txt,*.log,*.csv -ErrorAction SilentlyContinue | ForEach-Object {
        Post-RagFile -Path $_.FullName -SourceId "ccli-testsuite-pro-lab-logs"
    }
}

Write-Host "Done."
