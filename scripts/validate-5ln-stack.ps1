# End-to-end 5-LN cert stack check: repo CID/cfg + optional live DUT alignment.
# Usage:
#   powershell -File scripts\validate-5ln-stack.ps1
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\validate-5ln-stack.ps1 -CheckDut -Profile Tsp -FixDut
# Profile Tsp: single ccli, no iec61850d/mmsd (Test Suite Pro on :3782).
# Profile Luci: mmsd allowed; still no iec61850d; single ccli.

param(
    [switch]$CheckDut,
    [ValidateSet("Tsp", "Luci", "All")]
    [string]$Profile = "All",
    [switch]$FixDut,
    [string]$HostAddr = "192.168.1.130",
    [string]$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE",
    [switch]$WriteEvidence
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot

$cidRel = "apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cid"
$cfgRel = "apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cfg"
$cidPath = Join-Path $Repo $cidRel
$cfgPath = Join-Path $Repo $cfgRel

# Pins from lab/conformance/PRODUCT_SPECIMEN_5LN.md (update when CID/cfg rev bumps)
$pinCidSha = "68C72E7F87D1C65B04A3E42925E727815EC8B1A421E0DC65296C7C65B3CF2582"
$pinCfgSha = "7E13DE139559ADC68DA0BBAE0C8AB0BD20ABDED6AF3472A9974CD4A5BC6A3146"

Write-Host "=== 5-LN stack validation (repo) ===" -ForegroundColor Cyan

& powershell -NoProfile -File (Join-Path $Repo "scripts\validate-5ln-cid.ps1") -CidPath $cidRel
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$cidSha = (Get-FileHash $cidPath -Algorithm SHA256).Hash.ToUpper()
$cfgSha = (Get-FileHash $cfgPath -Algorithm SHA256).Hash.ToUpper()
$cfgBytes = (Get-Item $cfgPath).Length

$repoChecks = [ordered]@{}
$repoChecks["CID SHA matches PRODUCT_SPECIMEN_5LN pin"] = ($cidSha -eq $pinCidSha)
$repoChecks["cfg SHA matches PRODUCT_SPECIMEN_5LN pin"] = ($cfgSha -eq $pinCfgSha)
$repoChecks["cfg size 6604 bytes"] = ($cfgBytes -eq 6604)

foreach ($k in $repoChecks.Keys) {
    $ok = $repoChecks[$k]
    Write-Host ("[{0}] {1}" -f $(if ($ok) { "PASS" } else { "FAIL" }), $k) -ForegroundColor $(if ($ok) { "Green" } else { "Red" })
}
if ($repoChecks.Values -contains $false) {
    Write-Host "Update pins in PRODUCT_SPECIMEN_5LN.md if CID/cfg intentionally changed." -ForegroundColor Yellow
    exit 1
}

if (-not $CheckDut) {
    Write-Host ""
    Write-Host "Result: PASS (repo only). Add -CheckDut after lab-env.ps1 for live DUT." -ForegroundColor Green
    exit 0
}

if (-not $env:CCLI_TG544_PW) { throw "Source lab\tg544-openwrt\lab-env.ps1 before -CheckDut" }
$Plink = "C:\Program Files\PuTTY\plink.exe"
if (-not (Test-Path $Plink)) { throw "PuTTY plink not found" }

function Invoke-Dut {
    param([string]$Cmd)
    $prev = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $out = (& $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $Cmd 2>&1 | Out-String).Trim()
    $ErrorActionPreference = $prev
    return $out
}

if ($FixDut) {
    & powershell -NoProfile -File (Join-Path $Repo "scripts\push-5ln-product-cfg-to-dut.ps1") -HostAddr $HostAddr | Out-Host
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    if ($Profile -eq "Tsp") {
        Write-Host "FixDut: TSP profile (dut-mms-tsp-preflight)..." -ForegroundColor Yellow
        & powershell -NoProfile -File (Join-Path $Repo "scripts\dut-mms-tsp-preflight.ps1") -HostAddr $HostAddr | Out-Host
    } else {
        Write-Host "FixDut: unified matrix services (single ccli, no iec61850d)..." -ForegroundColor Yellow
        & powershell -NoProfile -File (Join-Path $Repo "scripts\validate-dut-unified-matrix.ps1") -HostAddr $HostAddr -FixServices | Out-Host
        if ($Profile -eq "Luci" -or $Profile -eq "All") {
            & powershell -NoProfile -File (Join-Path $Repo "scripts\enable-dut-luci-iec61850.ps1") -HostAddr $HostAddr | Out-Host
        }
    }
    Start-Sleep -Seconds 2
}

$ts = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$dutCfg = Invoke-Dut "sha256sum /etc/ccli/icd/product/product_tr400_eth_a_5ln.cfg 2>/dev/null"
$dutCid = Invoke-Dut "md5sum /tmp/iec61850_icd/product_tr400_eth_a_5ln.cid 2>/dev/null; ls -la /tmp/iec61850_icd/product_tr400_eth_a_5ln.cid 2>/dev/null"
$repoCidMd5 = (Get-FileHash $cidPath -Algorithm MD5).Hash.ToLower()
$yaml = Invoke-Dut "grep model_cfg /etc/ccli/lab.yaml"
$ps = Invoke-Dut "ps w | grep -E 'ccli|iec61850' | grep -v grep"
$ccliCount = ([regex]::Matches($ps, "/usr/sbin/ccli")).Count
$has61850d = $ps -match "iec61850d"
$hasMmsd = $ps -match "iec61850-mmsd"
$mmsLog = Invoke-Dut "logread 2>/dev/null | grep -E 'full CID model loaded|product_tr400_eth_a_5ln' | tail -3; tail -40 /tmp/ccli.log 2>/dev/null | grep -E 'full CID model|product_tr400_eth_a_5ln|svc_io: ready' | tail -5"

$dutChecks = [ordered]@{}
$dutChecks["yaml model_cfg -> product_tr400_eth_a_5ln.cfg"] = $yaml -match "product_tr400_eth_a_5ln\.cfg"
$dutChecks["DUT cfg SHA = repo cfg SHA"] = $dutCfg -match [regex]::Escape($pinCfgSha.ToLower())
$dutChecks["LuCI ICD md5 = repo CID md5"] = $dutCid -match $repoCidMd5
$dutChecks["single ccli process"] = ($ccliCount -eq 1)
$dutChecks["no iec61850d (TSP / unified)"] = -not $has61850d
if ($Profile -eq "Tsp") {
    $dutChecks["no iec61850-mmsd (TSP bench)"] = -not $hasMmsd
} elseif ($Profile -eq "Luci") {
    $dutChecks["iec61850-mmsd running (LuCI)"] = $hasMmsd
}
$dutChecks["ccli loaded 5-LN cfg (log hint)"] = ($mmsLog -match "product_tr400_eth_a_5ln") -or ($mmsLog -match "full CID model loaded")

Write-Host ""
Write-Host "=== 5-LN stack validation (DUT @ $HostAddr, profile=$Profile) $ts ===" -ForegroundColor Cyan
Write-Host "repo CID md5: $repoCidMd5"
Write-Host $dutCfg
Write-Host $dutCid

$fail = 0
foreach ($k in $dutChecks.Keys) {
    $ok = $dutChecks[$k]
    if (-not $ok) { $fail++ }
    Write-Host ("[{0}] {1}" -f $(if ($ok) { "PASS" } else { "FAIL" }), $k) -ForegroundColor $(if ($ok) { "Green" } else { "Red" })
}

if ($WriteEvidence) {
    $date = Get-Date -Format "yyyy-MM-dd"
    $out = Join-Path $Repo "lab\evidence\testsuite-pro\inbox\PREFLIGHT_5LN_STACK_${date}.txt"
    $body = @"
# PREFLIGHT_5LN_STACK - $date
generated: $ts
profile: $Profile
host: $HostAddr

## Repo
CID SHA256: $cidSha
cfg SHA256: $cfgSha

## DUT
$dutCfg
$dutCid
$yaml

## ps
$ps

## log excerpt
$mmsLog

## Checks
$(foreach ($k in $dutChecks.Keys) { "- [$($(if ($dutChecks[$k]) {'x'} else {' '}))] $k" })
"@
    New-Item -ItemType Directory -Force -Path (Split-Path $out) | Out-Null
    Set-Content -Path $out -Value $body -Encoding UTF8
    Write-Host "Wrote $out" -ForegroundColor Green
}

Write-Host ""
if ($fail -gt 0) {
    Write-Host "Result: FAIL ($fail DUT check(s)). Try: -FixDut -Profile Tsp (Connect) or Luci (logger ICD UI)." -ForegroundColor Red
    exit 1
}
Write-Host "Result: PASS (repo + DUT, profile=$Profile)" -ForegroundColor Green
exit 0
