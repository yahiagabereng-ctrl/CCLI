# Mirror CCLI PDFs into knowledge-base/ layout and Telematry documents/.
# Run from repo root: .\scripts\sync-ccli-pdfs.ps1

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Kb = Join-Path $Root "knowledge-base"
$Tel = "C:\Yahia\projects\Telematry System\documents"

@(
  "$Kb\07-protocols\cei-0-16",
  "$Kb\07-protocols\github-refs",
  "$Kb\08-engineering\reference",
  "$Tel\projects\ccli\protocols",
  "$Tel\projects\ccli\engineering",
  "$Tel\projects\ccli\reference"
) | ForEach-Object { New-Item -ItemType Directory -Path $_ -Force | Out-Null }

function Sync-File($Src, $Dest) {
  if (-not (Test-Path $Src)) {
    Write-Warning "Missing: $Src"
    return
  }
  if ((Resolve-Path $Src).Path -eq (Resolve-Path $Dest -ErrorAction SilentlyContinue).Path) {
    Write-Host "  (skip same path) $Dest"
    return
  }
  Copy-Item $Src $Dest -Force
  Write-Host "  $Dest"
}

Write-Host "CEI 0-16 -> KB mirror" -ForegroundColor Cyan
Sync-File "$Tel\regulations\cei-0-16\allegato-o\EstrattoAllegatoO.pdf" "$Kb\07-protocols\cei-0-16\EstrattoAllegatoO.pdf"
Sync-File "$Tel\regulations\cei-0-16\allegato-t\EstrattoAllegatoT.pdf" "$Kb\07-protocols\cei-0-16\EstrattoAllegatoT.pdf"
Sync-File "$Tel\regulations\cei-0-16\manuale-cci\Manuale_CCI_I_24_R6_240610.pdf" "$Kb\07-protocols\cei-0-16\Manuale_CCI_I_24_R6_240610.pdf"

Write-Host "Protocol library docs" -ForegroundColor Cyan
Sync-File "$Kb\07-protocols\CCI_GitHub_Protocol_Libraries.md" "$Tel\projects\ccli\protocols\CCI_GitHub_Protocol_Libraries.md"
Get-ChildItem "$Kb\07-protocols\github-refs\*.md" -ErrorAction SilentlyContinue | ForEach-Object {
  Sync-File $_.FullName "$Tel\projects\ccli\protocols\$($_.Name)"
}

Write-Host "Reference PDFs" -ForegroundColor Cyan
foreach ($f in @("iGate-850_V10_UM_EN.pdf", "moxa-mgate-5119-series-datasheet-v1.2.pdf", "STM32L151RCTx.pdf", "MT7986A_Datasheet_1.15.pdf")) {
  $src = if (Test-Path "$Root\$f") { "$Root\$f" } elseif (Test-Path "$Kb\08-engineering\reference\$f") { "$Kb\08-engineering\reference\$f" } else { "$Tel\projects\ccli\reference\$f" }
  Sync-File $src "$Kb\08-engineering\reference\$f"
  Sync-File $src "$Tel\projects\ccli\reference\$f"
}

Write-Host "Programme PDFs -> Telematry" -ForegroundColor Cyan
foreach ($f in @("CCI_Clone_RE_Option.pdf", "CCI_Project_Roadmap.pdf")) {
  Sync-File "$Root\$f" "$Tel\projects\ccli\$f"
}

Write-Host "Authored engineering docs -> Telematry" -ForegroundColor Cyan
foreach ($f in @(
  "CCI_Prototype_BOM.md",
  "CCI_Vendor_Decision_Scorecard.md",
  "CCI_Validation_and_Mocking_Strategy.md",
  "CCI_Mocking_Bench_BOM.md",
  "CCI_Mocking_Bench_BOM.json",
  "CCI_RAG_Knowledge_Base.md",
  "CCI_SOC_Freeze.md",
  "CCI_TG500_Lab_Platform.md",
  "CCI_3Month_Italy_NoFab_Plan.md",
  "CCI_Architecture_Framework.md"
)) {
  Sync-File "$Kb\08-engineering\$f" "$Tel\projects\ccli\engineering\$f"
}
Sync-File "$Kb\08-engineering\CCI_TG500_Knowledge_Tree.md" "$Tel\projects\ccli\engineering\CCI_TG500_Knowledge_Tree.md"
Sync-File "$Kb\06-application\CCI_Application_Structure.md" "$Tel\projects\ccli\engineering\CCI_Application_Structure.md"
Sync-File "$Kb\08-engineering\CCI_OpenWrt_Freeze.md" "$Tel\projects\ccli\engineering\CCI_OpenWrt_Freeze.md"
Sync-File "$Kb\08-engineering\CCI_TesPro_Supplier_Correspondence.md" "$Tel\projects\ccli\engineering\CCI_TesPro_Supplier_Correspondence.md"
Sync-File "$Kb\08-engineering\CCI_61557-12_Extract.md" "$Tel\projects\ccli\engineering\CCI_61557-12_Extract.md"
Sync-File "$Kb\08-engineering\CCI_MT7986A_Datasheet_Extract.md" "$Tel\projects\ccli\engineering\CCI_MT7986A_Datasheet_Extract.md"
Sync-File "$Kb\08-engineering\CCI_62443_Zones.md" "$Tel\projects\ccli\engineering\CCI_62443_Zones.md"
Sync-File "$Kb\08-engineering\CCI_62443-3-3_Capture_Plan.md" "$Tel\projects\ccli\engineering\CCI_62443-3-3_Capture_Plan.md"
Sync-File "$Kb\08-engineering\CCI_62443-3-3_Extract.md" "$Tel\projects\ccli\engineering\CCI_62443-3-3_Extract.md"
Sync-File "$Kb\08-engineering\CCI_62443-4-1_Capture_Plan.md" "$Tel\projects\ccli\engineering\CCI_62443-4-1_Capture_Plan.md"
Sync-File "$Kb\08-engineering\CCI_62443-4-1_Extract.md" "$Tel\projects\ccli\engineering\CCI_62443-4-1_Extract.md"
Sync-File "$Kb\08-engineering\CCI_62443-4-2_Capture_Plan.md" "$Tel\projects\ccli\engineering\CCI_62443-4-2_Capture_Plan.md"
Sync-File "$Kb\08-engineering\CCI_62443-4-2_Extract.md" "$Tel\projects\ccli\engineering\CCI_62443-4-2_Extract.md"
Sync-File "$Kb\08-engineering\CCI_62351-1_Capture_Plan.md" "$Tel\projects\ccli\engineering\CCI_62351-1_Capture_Plan.md"
Sync-File "$Kb\08-engineering\CCI_62351-1_Extract.md" "$Tel\projects\ccli\engineering\CCI_62351-1_Extract.md"
Sync-File "$Kb\08-engineering\CCI_62351-3_Capture_Plan.md" "$Tel\projects\ccli\engineering\CCI_62351-3_Capture_Plan.md"
Sync-File "$Kb\08-engineering\CCI_62351-3_Extract.md" "$Tel\projects\ccli\engineering\CCI_62351-3_Extract.md"
Sync-File "$Kb\08-engineering\CCI_62351-4_Capture_Plan.md" "$Tel\projects\ccli\engineering\CCI_62351-4_Capture_Plan.md"
Sync-File "$Kb\08-engineering\CCI_62351-4_Extract.md" "$Tel\projects\ccli\engineering\CCI_62351-4_Extract.md"
Sync-File "$Kb\08-engineering\CCI_62351-9_Capture_Plan.md" "$Tel\projects\ccli\engineering\CCI_62351-9_Capture_Plan.md"
Sync-File "$Kb\08-engineering\CCI_62351-9_Extract.md" "$Tel\projects\ccli\engineering\CCI_62351-9_Extract.md"
Sync-File "$Kb\08-engineering\CCI_K6.3_Key_Ceremony_SOP.md" "$Tel\projects\ccli\engineering\CCI_K6.3_Key_Ceremony_SOP.md"
Sync-File "$Kb\08-engineering\CCI_62351-14_Capture_Plan.md" "$Tel\projects\ccli\engineering\CCI_62351-14_Capture_Plan.md"
Sync-File "$Kb\08-engineering\CCI_62351-14_Extract.md" "$Tel\projects\ccli\engineering\CCI_62351-14_Extract.md"

Write-Host "TG-500 / TR-400 PDFs -> Telematry" -ForegroundColor Cyan
foreach ($f in @("TG-500_Series_Datasheet.pdf", "TesPro_Gateway_Specifications_Response_Form.pdf")) {
  Sync-File "$Kb\08-engineering\$f" "$Tel\projects\ccli\engineering\$f"
}
Sync-File "$Kb\08-engineering\reference\TR400_User_Manual_EN_v1.0.pdf" "$Tel\projects\ccli\reference\TR400_User_Manual_EN_v1.0.pdf"
Sync-File "$Kb\08-engineering\CCI_TR400_Capture_Plan.md" "$Tel\projects\ccli\engineering\CCI_TR400_Capture_Plan.md"
Sync-File "$Kb\08-engineering\CCI_TR400_Extract.md" "$Tel\projects\ccli\engineering\CCI_TR400_Extract.md"
Sync-File "$Kb\08-engineering\CCI_TR500_IO_Peripheral.md" "$Tel\projects\ccli\engineering\CCI_TR500_IO_Peripheral.md"
Sync-File "$Kb\08-engineering\CCI_CCLI_Product_Spec_and_Roadmap.md" "$Tel\projects\ccli\engineering\CCI_CCLI_Product_Spec_and_Roadmap.md"

Write-Host "KB brief + README -> Telematry" -ForegroundColor Cyan
Sync-File "$Kb\README.md" "$Tel\projects\ccli\CCI_Knowledge_Base_Brief.md"

Write-Host "EU regulations -> KB copies" -ForegroundColor Cyan
Sync-File "$Tel\regulations\eu\directive-2018-2001-red\CELEX_32018L2001_EN_TXT.pdf" "$Kb\07-protocols\CELEX_32018L2001_EN_TXT.pdf"
Sync-File "$Tel\regulations\eu\regulation-2024-2847-cra\OJ_L_202402847_EN_TXT.pdf" "$Kb\07-protocols\OJ_L_202402847_EN_TXT.pdf"

Write-Host "`nDone. Ingest: cd `"$Tel\..`"; .\scripts\ingest-ccli.ps1" -ForegroundColor Green
