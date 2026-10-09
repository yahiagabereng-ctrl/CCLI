# Stage portable Test Suite Pro field pack for a second PC (checklist + CID + TLS + procedures).
# Usage:
#   powershell -File scripts\stage-tsp-field-pack.ps1
#   powershell -File scripts\stage-tsp-field-pack.ps1 -Dest D:\USB\CCLI_TSP_FIELD_PACK

param(
    [string]$Dest = "C:\CCLI_TSP_FIELD_PACK"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Dest = $Dest.TrimEnd('\')

function Copy-Rel {
    param([string]$Rel, [string]$SubDir = "")
    $src = Join-Path $Repo ($Rel -replace '/', '\')
    if (-not (Test-Path $src)) {
        Write-Warning "Missing: $Rel"
        return $null
    }
    $leaf = Split-Path $src -Leaf
    $targetDir = if ($SubDir) { Join-Path $Dest $SubDir } else { $Dest }
    New-Item -ItemType Directory -Force -Path $targetDir | Out-Null
    $out = Join-Path $targetDir $leaf
    Copy-Item -Force $src $out
    return $out
}

Write-Host "Staging TSP field pack -> $Dest" -ForegroundColor Cyan
if (Test-Path $Dest) {
    Get-ChildItem $Dest -Recurse -ErrorAction SilentlyContinue | Remove-Item -Force -Recurse -ErrorAction SilentlyContinue
}
New-Item -ItemType Directory -Force -Path $Dest | Out-Null

# TLS first (via existing stage script)
& (Join-Path $Repo "scripts\stage-tsp-tls.ps1") -ProductDest (Join-Path $Dest "tls") | Out-Null

$manifest = [ordered]@{
    generated_utc = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    repo          = $Repo
    files         = @()
}

$docs = @(
    @{ Rel = "lab/conformance/LAB_REQUIREMENT_TEST_MATRIX.md"; Out = "01_CHECKLIST_LAB_REQUIREMENT_TEST_MATRIX.md" },
    @{ Rel = "lab/evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md"; Out = "02_TSP_TEST_IDENTIFICATION.md" },
    @{ Rel = "lab/CCI_TestSuitePro_Sequencer_Draft.md"; Out = "03_TSP_SEQUENCER_DRAFT.md" },
    @{ Rel = "lab/CCI_TestSuitePro_Verification_Layer.md"; Out = "04_TSP_VERIFICATION_LAYER.md" },
    @{ Rel = "lab/evidence/testsuite-pro/README.md"; Out = "05_EVIDENCE_README.md" },
    @{ Rel = "lab/conformance/TSP_PC_FIELD_PACK.md"; Out = "00_README_FIELD_PACK.md" },
    @{ Rel = "lab/conformance/2026-10-08_UCA_Document_Phase_Checklist.md"; Out = "06_LAB_UCA_DOCUMENT_CHECKLIST.md" },
    @{ Rel = "lab/conformance/LAB_TEMPLATES_VS_TEST_SCOPE.md"; Out = "07_LAB_TEMPLATES_VS_TEST_SCOPE.md" },
    @{ Rel = "lab/conformance/TSP_PC_FULL_BENCH_HW.md"; Out = "08_TSP_PC_FULL_BENCH_HW.md" },
    @{ Rel = "lab/conformance/TSP_TWO_PC_BENCH_RUNBOOK.md"; Out = "00_TWO_PC_BENCH_RUNBOOK.md" },
    @{ Rel = "lab/conformance/PICS_FILL.md"; Out = "09_PICS_FILL.md" },
    @{ Rel = "lab/conformance/PIXIT_DRAFT.md"; Out = "10_PIXIT_DRAFT.md" },
    @{ Rel = "lab/conformance/MICS_DRAFT.md"; Out = "11_MICS_DRAFT.md" },
    @{ Rel = "lab/conformance/TICS_DRAFT.md"; Out = "12_TICS_DRAFT.md" },
    @{ Rel = "lab/conformance/D7_62351-3_PID_DRAFT.md"; Out = "13_D7_62351-3_PID.md" },
    @{ Rel = "lab/conformance/CID_PICS_ALIGNMENT.md"; Out = "14_CID_PICS_ALIGNMENT.md" },
    @{ Rel = "lab/conformance/LAB_CONFIG_GUIDE.md"; Out = "15_LAB_CONFIG_GUIDE.md" }
)

$benchScripts = @(
    "lab/set_pc_lan1_dso.cmd",
    "lab/set_pc_lan2_oa.cmd",
    "lab/set_pc_lan3_plant.cmd"
)
New-Item -ItemType Directory -Force -Path (Join-Path $Dest "bench_scripts") | Out-Null
foreach ($rel in $benchScripts) {
    $src = Join-Path $Repo ($rel -replace '/', '\')
    if (Test-Path $src) {
        Copy-Item -Force $src (Join-Path $Dest "bench_scripts\$(Split-Path $src -Leaf)")
        $manifest.files += "bench_scripts/$(Split-Path $src -Leaf)"
    }
}
$modbus = Join-Path $Repo "lab\modbus_rtu_slave.py"
if (Test-Path $modbus) {
    New-Item -ItemType Directory -Force -Path (Join-Path $Dest "tools") | Out-Null
    Copy-Item -Force $modbus (Join-Path $Dest "tools\modbus_rtu_slave.py")
    $manifest.files += "tools/modbus_rtu_slave.py"
}

foreach ($d in $docs) {
    $src = Join-Path $Repo ($d.Rel -replace '/', '\')
    if (-not (Test-Path $src)) { Write-Warning "Skip $($d.Rel)"; continue }
    $outPath = Join-Path $Dest $d.Out
    Copy-Item -Force $src $outPath
    $manifest.files += $d.Out
    Write-Host "  $($d.Out)"
}

Copy-Rel "apps/ccli/config/icd/lab_tg544_eth_a.cid" "cid" | Out-Null
Copy-Rel "apps/ccli/config/lab_tr400_phase1_regulation.yaml" "config" | Out-Null

# PICS template optional (large xlsx)
$pics = Join-Path $Repo "lab\TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx"
if (Test-Path $pics) {
    New-Item -ItemType Directory -Force -Path (Join-Path $Dest "templates") | Out-Null
    Copy-Item -Force $pics (Join-Path $Dest "templates\TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx")
    $manifest.files += "templates/TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx"
}

$manifestPath = Join-Path $Dest "PACK_MANIFEST.json"
$manifest.files += "tls/*", "cid/lab_tg544_eth_a.cid", "config/lab_tr400_phase1_regulation.yaml"
($manifest | ConvertTo-Json -Depth 4) | Set-Content -Path $manifestPath -Encoding UTF8

Write-Host ""
Write-Host "OK: $Dest" -ForegroundColor Green
Write-Host "Copy this folder to the Test Suite Pro PC (USB). Open 01_CHECKLIST_LAB_REQUIREMENT_TEST_MATRIX.md"
Write-Host "RAG source_ids: ccli-lab-requirement-test-matrix, ccli-tsp-field-pack, ccli-tsp-test-identification"
