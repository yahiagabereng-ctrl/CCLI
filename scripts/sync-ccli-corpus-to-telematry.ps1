# Sync CCLI knowledge-base engineering + protocol docs to Telematry documents tree.
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$KbEng = Join-Path $Repo "knowledge-base\08-engineering"
$KbProto = Join-Path $Repo "knowledge-base\07-protocols"
$TelEng = Join-Path $TelematryRoot "documents\projects\ccli\engineering"
$TelProto = Join-Path $TelematryRoot "documents\projects\ccli\protocols"

New-Item -ItemType Directory -Path $TelEng, $TelProto -Force | Out-Null

$engPatterns = @(
    "CCI_*Extract.md",
    "CCI_*OCR*.md",
    "CCI_*Capture*.md",
    "CCI_62443_Zones.md",
    "CCI_K6.3_Key_Ceremony_SOP.md",
    "CCI_Architecture_Framework.md",
    "CCI_Validation_and_Mocking_Strategy.md",
    "CCI_SOC_Freeze.md",
    "CCI_TG500_Lab_Platform.md",
    "CCI_OpenWrt_Freeze.md"
)

$count = 0
$dl = Join-Path $Repo "knowledge-base\DOWNLOADS.md"
if (Test-Path $dl) {
    Copy-Item $dl (Join-Path $TelEng "DOWNLOADS.md") -Force
    $count++
}
foreach ($pat in $engPatterns) {
    Get-ChildItem $KbEng -Filter $pat -ErrorAction SilentlyContinue | ForEach-Object {
        Copy-Item $_.FullName (Join-Path $TelEng $_.Name) -Force
        $count++
    }
}

$protoFiles = @(
    "CCI_Module_Regulations_Classification.md",
    "CCI_Standards_PDF_Inventory.md",
    "CCI_GitHub_Protocol_Libraries.md",
    "regulations\README.md"
)
foreach ($rel in $protoFiles) {
    $src = Join-Path $KbProto $rel
    if (-not (Test-Path $src)) { continue }
    $dest = Join-Path $TelProto ($rel -replace '/', '\')
    New-Item -ItemType Directory -Path (Split-Path $dest) -Force | Out-Null
    Copy-Item $src $dest -Force
    $count++
}

# Repo-root programme docs (if mirrored under projects/ccli)
$TelCcli = Join-Path $TelematryRoot "documents\projects\ccli"
foreach ($name in @("CCI_Project_Roadmap.md", "CCI_3Month_Italy_NoFab_Plan.md")) {
    $src = Join-Path $Repo $name
    if (Test-Path $src) {
        Copy-Item $src (Join-Path $TelCcli $name) -Force
        $count++
    }
}

Write-Host "Synced $count files to Telematry under documents/projects/ccli/" -ForegroundColor Green
