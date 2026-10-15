# Push product_tr400_eth_a_5ln.cfg to DUT and verify SHA256 vs repo.
# Usage: . .\lab\tg544-openwrt\lab-env.ps1; powershell -File scripts\push-5ln-product-cfg-to-dut.ps1

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$CfgLocal = Join-Path $Repo "apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cfg"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
if (-not $env:CCLI_TG544_PW) { throw "Source lab-env.ps1" }
if (-not (Test-Path $CfgLocal)) { throw "Missing $CfgLocal" }

$expected = (Get-FileHash $CfgLocal -Algorithm SHA256).Hash.ToLower()
Write-Host "Push 5-LN cfg (SHA256 $expected) -> ${HostAddr}:/etc/ccli/icd/product/" -ForegroundColor Cyan

& $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch "mkdir -p /etc/ccli/icd/product"
& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $CfgLocal "root@${HostAddr}:/etc/ccli/icd/product/product_tr400_eth_a_5ln.cfg"

$remote = (& $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch "sha256sum /etc/ccli/icd/product/product_tr400_eth_a_5ln.cfg").Trim()
Write-Host $remote
if ($remote -notmatch [regex]::Escape($expected)) {
    Write-Host "FAIL: DUT cfg SHA mismatch" -ForegroundColor Red
    exit 1
}
Write-Host "PASS: DUT cfg matches repo" -ForegroundColor Green
Write-Host "Restart ccli to reload model: /etc/init.d/ccli restart" -ForegroundColor Yellow
