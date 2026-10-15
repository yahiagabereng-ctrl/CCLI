# LuCI IEC 61850: start mmsd, normalize /tmp/iec61850_icd paths (TesPro convention: no .cid suffix).
# Usage: . .\lab\tg544-openwrt\lab-env.ps1; powershell -File scripts\enable-dut-luci-iec61850.ps1

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE",
    [ValidateSet("5ln", "lab31")]
    [string]$Model = "5ln"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$ShLocal = Join-Path $Repo "lab\tg544-openwrt\enable-dut-luci-iec61850.sh"
if (-not $env:CCLI_TG544_PW) { throw "Source lab-env.ps1" }

if ($Model -eq "5ln") {
    $srcCid = Join-Path $Repo "apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cid"
    $icdBase = "product_tr400_eth_a_5ln"
} else {
    $srcCid = Join-Path $Repo "apps\ccli\config\icd\lab_tg544_eth_a.cid"
    $icdBase = "lab_tg544"
}

if (-not (Test-Path $srcCid)) { throw "Missing $srcCid" }

Write-Host "LuCI IEC61850: deploy ICD + start mmsd on $HostAddr (model=$Model)" -ForegroundColor Cyan
& $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch "mkdir -p /tmp/iec61850_icd"
& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $srcCid "root@${HostAddr}:/tmp/iec61850_icd/${icdBase}"
& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $ShLocal "root@${HostAddr}:/tmp/enable-dut-luci-iec61850.sh"
& $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch "sed -i 's/\r$//' /tmp/enable-dut-luci-iec61850.sh; chmod +x /tmp/enable-dut-luci-iec61850.sh; sh /tmp/enable-dut-luci-iec61850.sh $icdBase"

Write-Host ""
Write-Host "LuCI ICD/CID Path (no extension): /tmp/iec61850_icd/$icdBase" -ForegroundColor Green
