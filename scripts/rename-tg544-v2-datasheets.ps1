# Rename user-downloaded datasheet files to canonical MPN names (safe: skip if target exists).
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Ds = Join-Path $Root "knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/datasheets"

$Map = @{
    "114992991.pdf" = "WM1302_Seeed_LoRaWAN_Module.pdf"
    "b74a8e8492ae61cd194d688b7ffc668e.pdf" = "PCAQ7060MW-301_Prodtech_Spec.pdf"
    "infineon-slb9673-tpm20-i2c-fw26xx-ds-rev1-4-2024-11-13-datasheet-en.pdf" = "SLB9673_Infineon_DataSheet_Rev1.4.pdf"
    "TOSC-S-A0009571501-1.pdf" = "TLP291-4_Toshiba.pdf"
    "MCCC-S-A0018719587-1.pdf" = "SK56L_MCC_Schottky.pdf"
    "LFSI-S-A0010723737-1.pdf" = "SMBJ_Series_Littelfuse.pdf"
    "SMBJ SERIES N0193 REV.D.pdf" = "SMBJ_Series_DataSheet_N0193_RevD.pdf"
}

foreach ($kv in $Map.GetEnumerator()) {
    $src = Join-Path $Ds $kv.Key
    $dst = Join-Path $Ds $kv.Value
    if (-not (Test-Path $src)) { continue }
    if (Test-Path $dst) {
        Write-Host "SKIP (target exists): $($kv.Value)"
        continue
    }
    Rename-Item -LiteralPath $src -NewName $kv.Value
    Write-Host "RENAMED $($kv.Key) -> $($kv.Value)"
}
