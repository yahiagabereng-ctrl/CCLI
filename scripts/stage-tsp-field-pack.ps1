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
    @{ Rel = "lab/conformance/LAB_UNIFIED_CERT_TEST_MATRIX.md"; Out = "00_UNIFIED_CERT_TEST_MATRIX.md" },
    @{ Rel = "lab/conformance/LAB_UNIFIED_CERT_TEST_ROADMAP.md"; Out = "00b_UNIFIED_CERT_ROADMAP.md" },
    @{ Rel = "lab/conformance/PRODUCT_SPECIMEN_5LN.md"; Out = "00c_PRODUCT_SPECIMEN_5LN.md" },
    @{ Rel = "lab/conformance/PRODUCT_SPECIMEN_5LN_VALIDATION.md"; Out = "00d_PRODUCT_SPECIMEN_5LN_VALIDATION.md" },
    @{ Rel = "lab/conformance/P5_FULLCIRCLE_5LN_SCOPE.md"; Out = "00e_P5_FULLCIRCLE_5LN_SCOPE.md" },
    @{ Rel = "handoff/masomeh95/TSP_PC1_START_MASOMEH.md"; Out = "00_START_PC1_MASOMEH.md" },
    @{ Rel = "lab/conformance/LAB_UNIFIED_CERT_TEST_MATRIX.csv"; Out = "00_UNIFIED_CERT_TEST_MATRIX.csv" },
    @{ Rel = "lab/conformance/LAB_REQUIREMENT_TEST_MATRIX.md"; Out = "01_CHECKLIST_LAB_REQUIREMENT_TEST_MATRIX.md" },
    @{ Rel = "lab/conformance/LAB_REQUIREMENT_TEST_MATRIX_CYBER.md"; Out = "01b_CHECKLIST_CYBER_REQUIREMENT_TEST_MATRIX.md" },
    @{ Rel = "lab/evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md"; Out = "02_TSP_TEST_IDENTIFICATION.md" },
    @{ Rel = "lab/CCI_TestSuitePro_Sequencer_Draft.md"; Out = "03_TSP_SEQUENCER_DRAFT.md" },
    @{ Rel = "lab/CCI_TestSuitePro_Verification_Layer.md"; Out = "04_TSP_VERIFICATION_LAYER.md" },
    @{ Rel = "lab/evidence/testsuite-pro/README.md"; Out = "05_EVIDENCE_README.md" },
    @{ Rel = "lab/evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md"; Out = "05b_EVIDENCE_PER_TEST_PROTOCOL.md" },
    @{ Rel = "lab/conformance/TSP_PC_FIELD_PACK.md"; Out = "00_README_FIELD_PACK.md" },
    @{ Rel = "lab/conformance/2026-10-08_UCA_Document_Phase_Checklist.md"; Out = "06_LAB_UCA_DOCUMENT_CHECKLIST.md" },
    @{ Rel = "lab/conformance/LAB_TEMPLATES_VS_TEST_SCOPE.md"; Out = "07_LAB_TEMPLATES_VS_TEST_SCOPE.md" },
    @{ Rel = "lab/conformance/TSP_PC_FULL_BENCH_HW.md"; Out = "08_TSP_PC_FULL_BENCH_HW.md" },
    @{ Rel = "lab/conformance/TSP_TWO_PC_BENCH_RUNBOOK.md"; Out = "00_TWO_PC_BENCH_RUNBOOK.md" },
    @{ Rel = "lab/conformance/PICS_FILL.md"; Out = "09_PICS_FILL.md" },
    @{ Rel = "lab/conformance/PICS_FILLED_WORKBOOK.md"; Out = "09b_PICS_FILLED_WORKBOOK.md" },
    @{ Rel = "lab/conformance/drafts/PICS_CCI016_01_filled_rev3p0_AutoRecovered.xlsx"; Out = "09c_PICS_CCI016_01_filled.xlsx" },
    @{ Rel = "lab/conformance/PIXIT_DRAFT.md"; Out = "10_PIXIT_DRAFT.md" },
    @{ Rel = "lab/conformance/MICS_DRAFT.md"; Out = "11_MICS_DRAFT.md" },
    @{ Rel = "lab/conformance/TICS_DRAFT.md"; Out = "12_TICS_DRAFT.md" },
    @{ Rel = "lab/conformance/D7_62351-3_PID_DRAFT.md"; Out = "13_D7_62351-3_PID.md" },
    @{ Rel = "lab/conformance/CID_PICS_ALIGNMENT.md"; Out = "14_CID_PICS_ALIGNMENT.md" },
    @{ Rel = "lab/conformance/LAB_CONFIG_GUIDE.md"; Out = "15_LAB_CONFIG_GUIDE.md" },
    @{ Rel = "lab/conformance/LAB_SPECIMEN_VERSION.md"; Out = "16_LAB_SPECIMEN_VERSION.md" },
    @{ Rel = "handoff/masomeh95/PC1_HW_CERT_PREFLIGHT.md"; Out = "00f_PC1_HW_CERT_PREFLIGHT.md" },
    @{ Rel = "handoff/masomeh95/PC1_LOG_CAPTURE_REQUIREMENTS.md"; Out = "00h_PC1_LOG_CAPTURE_REQUIREMENTS.md" },
    @{ Rel = "lab/conformance/LAB_UNIFIED_TSP_5LN_EXECUTION.md"; Out = "00g_TSP_5LN_EXECUTION_AND_LOGS.md" },
    @{ Rel = "lab/evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md"; Out = "05c_EVIDENCE_PER_TEST_PROTOCOL.md" }
)

# Evidence tree on TSP PC (logger writes here when cwd = pack root)
$evidenceInbox = Join-Path $Dest "lab\evidence\testsuite-pro\inbox"
$evidencePcap = Join-Path $Dest "lab\evidence\testsuite-pro\pcap"
New-Item -ItemType Directory -Force -Path $evidenceInbox, $evidencePcap | Out-Null
$pullDut = Join-Path $Repo "lab\evidence\testsuite-pro\pull-dut-tsp-test-remote.sh"
if (Test-Path $pullDut) {
    Copy-Item -Force $pullDut (Join-Path $Dest "lab\evidence\testsuite-pro\pull-dut-tsp-test-remote.sh")
    $manifest.files += "lab/evidence/testsuite-pro/pull-dut-tsp-test-remote.sh"
}
$manifest.files += "lab/evidence/testsuite-pro/inbox/", "lab/evidence/testsuite-pro/pcap/"

$tspScripts = @(
    "scripts/tsp-evidence-logger.ps1",
    "scripts/capture-mms-tsp.ps1",
    "scripts/capture-mms-tsp-interactive.ps1"
)
New-Item -ItemType Directory -Force -Path (Join-Path $Dest "scripts") | Out-Null
foreach ($rel in $tspScripts) {
    $src = Join-Path $Repo ($rel -replace '/', '\')
    if (Test-Path $src) {
        Copy-Item -Force $src (Join-Path $Dest "scripts\$(Split-Path $src -Leaf)")
        $manifest.files += "scripts/$(Split-Path $src -Leaf)"
    }
}
foreach ($scr in @("validate-5ln-cid.ps1", "validate-dut-unified-matrix.ps1")) {
    $v5 = Join-Path $Repo "scripts\$scr"
    if (Test-Path $v5) {
        Copy-Item -Force $v5 (Join-Path $Dest "scripts\$scr")
        $manifest.files += "scripts/$scr"
    }
}

$benchScripts = @(
    "lab/set_pc_lan1_dso.cmd",
    "lab/set_pc_lan2_oa.cmd",
    "lab/set_pc_lan3_plant.cmd",
    "lab/set_pc_lan3_plant_178.cmd"
)
New-Item -ItemType Directory -Force -Path (Join-Path $Dest "bench_scripts") | Out-Null
foreach ($rel in $benchScripts) {
    $src = Join-Path $Repo ($rel -replace '/', '\')
    if (Test-Path $src) {
        Copy-Item -Force $src (Join-Path $Dest "bench_scripts\$(Split-Path $src -Leaf)")
        $manifest.files += "bench_scripts/$(Split-Path $src -Leaf)"
    }
}
New-Item -ItemType Directory -Force -Path (Join-Path $Dest "tools") | Out-Null
$toolFiles = @(
    "lab\modbus_rtu_slave.py",
    "lab\modbus_emt432_tcp_probe.py",
    "lab\start-bench-plant.ps1"
)
foreach ($rel in $toolFiles) {
    $src = Join-Path $Repo ($rel -replace '/', '\')
    if (-not (Test-Path $src)) { continue }
    Copy-Item -Force $src (Join-Path $Dest "tools\$(Split-Path $src -Leaf)")
    $manifest.files += "tools/$(Split-Path $src -Leaf)"
}
$sunSim = Join-Path $Repo "lab\third_party\Sun2000-100KTL-M2-Inverter-Simulator\sun2000_100ktl_m2_rtu_sim.py"
if (Test-Path $sunSim) {
    New-Item -ItemType Directory -Force -Path (Join-Path $Dest "tools\sun2000") | Out-Null
    Copy-Item -Force $sunSim (Join-Path $Dest "tools\sun2000\sun2000_100ktl_m2_rtu_sim.py")
    $manifest.files += "tools/sun2000/sun2000_100ktl_m2_rtu_sim.py"
}
$emtMap = Join-Path $Repo "apps\ccli\config\modbus\chronos_emt432_map.yaml"
if (Test-Path $emtMap) {
    New-Item -ItemType Directory -Force -Path (Join-Path $Dest "config\modbus") | Out-Null
    Copy-Item -Force $emtMap (Join-Path $Dest "config\modbus\chronos_emt432_map.yaml")
    $manifest.files += "config/modbus/chronos_emt432_map.yaml"
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
$prodCid = "apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid"
if (Test-Path (Join-Path $Repo ($prodCid -replace '/', '\'))) {
    Copy-Rel $prodCid "cid" | Out-Null
    $manifest.files += "cid/product_tr400_eth_a_5ln.cid"
} else {
    Write-Warning "5-LN product CID not in repo yet - TSP must load product CID when ready (see 00c_PRODUCT_SPECIMEN_5LN.md)"
}
Copy-Rel "apps/ccli/config/lab_tr400_phase1_regulation.yaml" "config" | Out-Null
Copy-Rel "apps/ccli/config/lab_tr400_phase4_eth_b_ttyS1.yaml" "config" | Out-Null
Copy-Rel "apps/ccli/config/lab_tr400_phase4_emt432_lan3_tcp.yaml" "config" | Out-Null

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
Write-Host "Copy this folder to the Test Suite Pro PC (USB). Open 00_START_PC1_MASOMEH.md + 00h_PC1_LOG_CAPTURE_REQUIREMENTS.md + 00_UNIFIED_CERT_TEST_MATRIX.md"
Write-Host "RAG: ccli-lab-unified-cert-test-matrix, ccli-lab-unified-cert-test-roadmap, ccli-product-specimen-5ln, ccli-tsp-two-pc-bench-runbook"
