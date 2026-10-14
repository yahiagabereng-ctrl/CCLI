# Sync CCLI knowledge-base engineering + protocol docs to Telematry documents tree.
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$KbEng = Join-Path $Repo "knowledge-base\08-engineering"
$KbApp = Join-Path $Repo "knowledge-base\06-application"
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
    "CCI_OpenWrt_Freeze.md",
    "CCI_Product_SW_Freeze.md"
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

foreach ($name in @("CCI_Application_Structure.md", "README.md")) {
    $src = Join-Path $KbApp $name
    if (Test-Path $src) {
        Copy-Item $src (Join-Path $TelEng $name) -Force
        $count++
    }
}

$kbReadme = Join-Path $Repo "knowledge-base\README.md"
if (Test-Path $kbReadme) {
    Copy-Item $kbReadme (Join-Path $TelEng "Knowledge_Base_Brief.md") -Force
    $count++
}

# Lab procedures (TSP / MMS) — product verification
$TelLabRoot = Join-Path $TelematryRoot "documents\projects\ccli\lab"
foreach ($labDoc in @(
        "CCI_TestSuitePro_Verification_Layer.md",
        "CCI_MMS_MultiTool_Verification.md"
    )) {
    $src = Join-Path $Repo "lab\$labDoc"
    if (Test-Path $src) {
        New-Item -ItemType Directory -Path $TelLabRoot -Force | Out-Null
        Copy-Item $src (Join-Path $TelLabRoot $labDoc) -Force
        $count++
    }
}

# Product release readme
$TelRel = Join-Path $TelematryRoot "documents\projects\ccli\releases\product-cyber"
$relReadme = Join-Path $Repo "releases\product-cyber\README.md"
if (Test-Path $relReadme) {
    New-Item -ItemType Directory -Path $TelRel -Force | Out-Null
    Copy-Item $relReadme (Join-Path $TelRel "README.md") -Force
    $count++
}

# TSP Compare waivers (product/TSP RAG)
$TelTspInbox = Join-Path $TelematryRoot "documents\projects\ccli\lab\evidence\testsuite-pro\inbox"
$waivers = Join-Path $Repo "lab\evidence\testsuite-pro\inbox\TSP_P5_COMPARE_WAIVERS.md"
if (Test-Path $waivers) {
    New-Item -ItemType Directory -Path $TelTspInbox -Force | Out-Null
    Copy-Item $waivers (Join-Path $TelTspInbox "TSP_P5_COMPARE_WAIVERS.md") -Force
    $count++
}

# DNV product readiness
$TelDnv = Join-Path $TelematryRoot "documents\projects\ccli\lab\conformance\dnv"
$dnvDoc = Join-Path $Repo "lab\conformance\dnv\DNV_UCA_PRODUCT_READINESS.md"
if (Test-Path $dnvDoc) {
    New-Item -ItemType Directory -Path $TelDnv -Force | Out-Null
    Copy-Item $dnvDoc (Join-Path $TelDnv "DNV_UCA_PRODUCT_READINESS.md") -Force
    $count++
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

# Lab meeting docs
$TelLab = Join-Path $TelematryRoot "documents\projects\ccli\lab\meetings"
$LabMeet = Join-Path $Repo "lab\meetings"
if (Test-Path $LabMeet) {
    New-Item -ItemType Directory -Path $TelLab -Force | Out-Null
    Get-ChildItem $LabMeet -Filter "*.md" -ErrorAction SilentlyContinue | ForEach-Object {
        Copy-Item $_.FullName (Join-Path $TelLab $_.Name) -Force
        $count++
    }
}

# Lab conformance + TSP test identification (RAG field checklists)
$TelConf = Join-Path $TelematryRoot "documents\projects\ccli\lab\conformance"
$LabConf = Join-Path $Repo "lab\conformance"
if (Test-Path $LabConf) {
    New-Item -ItemType Directory -Path $TelConf -Force | Out-Null
    Get-ChildItem $LabConf -Filter "*.md" -ErrorAction SilentlyContinue | ForEach-Object {
        Copy-Item $_.FullName (Join-Path $TelConf $_.Name) -Force
        $count++
    }
}
$TspId = Join-Path $Repo "lab\evidence\testsuite-pro\TSP_TEST_IDENTIFICATION_POST_CONNECT.md"
if (Test-Path $TspId) {
    $TelTspDoc = Join-Path $TelematryRoot "documents\projects\ccli\lab\evidence\testsuite-pro"
    New-Item -ItemType Directory -Path $TelTspDoc -Force | Out-Null
    Copy-Item $TspId (Join-Path $TelTspDoc "TSP_TEST_IDENTIFICATION_POST_CONNECT.md") -Force
    $count++
}

# Handoff docs referenced by unified matrix RAG (U-P0-10)
foreach ($handoffSub in @("masomeh95", "alirezavzand")) {
    $HandoffSrc = Join-Path $Repo "handoff\$handoffSub"
    if (-not (Test-Path $HandoffSrc)) { continue }
    $TelHandoff = Join-Path $TelematryRoot "documents\projects\ccli\lab\handoff\$handoffSub"
    New-Item -ItemType Directory -Path $TelHandoff -Force | Out-Null
    Get-ChildItem $HandoffSrc -Include "*.md", "*.txt" -Recurse -ErrorAction SilentlyContinue | ForEach-Object {
        Copy-Item $_.FullName (Join-Path $TelHandoff $_.Name) -Force
        $count++
    }
}

Write-Host "Synced $count files to Telematry under documents/projects/ccli/" -ForegroundColor Green
