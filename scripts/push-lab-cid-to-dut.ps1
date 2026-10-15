# Push lab_tg544_eth_a.cid + .cfg to DUT (MMS model + LuCI ICD path).
# Usage: . .\lab\tg544-openwrt\lab-env.ps1; powershell -File scripts\push-lab-cid-to-dut.ps1 -HostAddr 192.168.1.130

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
if (-not $env:CCLI_TG544_PW) { throw "Source lab-env.ps1" }

$cidLocal = Join-Path $Repo "apps\ccli\config\icd\lab_tg544_eth_a.cid"
$cfgLocal = Join-Path $Repo "apps\ccli\config\icd\lab_tg544_eth_a.cfg"
foreach ($p in @($cidLocal, $cfgLocal)) {
    if (-not (Test-Path $p)) { throw "Missing $p" }
}

$cidHash = (Get-FileHash $cidLocal -Algorithm SHA256).Hash.ToLower()
$cfgHash = (Get-FileHash $cfgLocal -Algorithm SHA256).Hash.ToLower()

Write-Host "Push lab CID/cfg -> ${HostAddr}:/etc/ccli/icd/ + /tmp/iec61850_icd/lab_tg544" -ForegroundColor Cyan
& $Plink -ssh "root@$HostAddr" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch "mkdir -p /etc/ccli/icd /tmp/iec61850_icd"
& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $cidLocal "root@${HostAddr}:/etc/ccli/icd/lab_tg544_eth_a.cid"
& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $cfgLocal "root@${HostAddr}:/etc/ccli/icd/lab_tg544_eth_a.cfg"
& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $cidLocal "root@${HostAddr}:/tmp/iec61850_icd/lab_tg544"

$remote = @"
sha256sum /etc/ccli/icd/lab_tg544_eth_a.cid /etc/ccli/icd/lab_tg544_eth_a.cfg /tmp/iec61850_icd/lab_tg544
grep model_cfg /etc/ccli/lab.yaml | head -1
"@ -replace "`r`n", "; "

& $Plink -ssh "root@$HostAddr" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $remote
if ($remote -notmatch $cidHash) {
    $out = & $Plink -ssh "root@$HostAddr" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch "sha256sum /etc/ccli/icd/lab_tg544_eth_a.cid"
    if ($out -notmatch [regex]::Escape($cidHash)) {
        Write-Host "FAIL: CID SHA mismatch on DUT" -ForegroundColor Red
        exit 1
    }
}
Write-Host "PASS: lab_tg544_eth_a.cid + cfg on DUT (CID SHA256 $cidHash)" -ForegroundColor Green
Write-Host "Restart ccli if model_cfg already points at lab_tg544_eth_a.cfg: /etc/init.d/ccli restart" -ForegroundColor Yellow
