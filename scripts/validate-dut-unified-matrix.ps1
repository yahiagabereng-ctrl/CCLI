# Validate live DUT against unified cert matrix deploy gates (U-P0-07, U-D7-01, specimen pin).
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\validate-dut-unified-matrix.ps1 -WriteEvidence
#   powershell -File scripts\validate-dut-unified-matrix.ps1 -FixServices -WriteEvidence

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE",
    [switch]$WriteEvidence,
    [switch]$FixServices
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Plink = "C:\Program Files\PuTTY\plink.exe"
if (-not (Test-Path $Plink)) { throw "PuTTY plink not found" }
if (-not $env:CCLI_TG544_PW) { throw "Source lab-env.ps1 first" }

$date = Get-Date -Format "yyyy-MM-dd"
$ts = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$expectedSha = "1df82c26243021a86c533a179781c749fc852c2f724efa3b0aa4018b2969eca1"

function Invoke-Dut {
    param([string]$Cmd)
    $prev = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $out = (& $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $Cmd 2>&1 | Out-String).Trim()
    $ErrorActionPreference = $prev
    return $out
}

if ($FixServices) {
    Write-Host "Normalize DUT: disable iec61850service, single ccli via init.d..." -ForegroundColor Yellow
    Invoke-Dut "/etc/init.d/iec61850service disable; /etc/init.d/iec61850service stop; killall iec61850d; killall ccli; sleep 2; /etc/init.d/ccli start; sleep 4" | Out-Null
}

$version = Invoke-Dut "/usr/sbin/ccli --version"
$sha = Invoke-Dut "sha256sum /usr/sbin/ccli"
$ss = Invoke-Dut "netstat -tlnp 2>/dev/null | grep -E '3782|2404|:102 '"
$ps = Invoke-Dut "ps w | grep -E 'ccli|iec61850' | grep -v grep"
$yaml = Invoke-Dut "grep -E 'model_cfg|tls_enabled|bind_address|tcp_port|backend:|device:' /etc/ccli/lab.yaml | head -20"
$tlsN = Invoke-Dut "ls /etc/ccli/tls/*.pem 2>/dev/null | wc -l"
$pgrepOut = Invoke-Dut "pgrep -f '/usr/sbin/ccli --config'"
$ccliCount = @($pgrepOut -split "\s+" | Where-Object { $_ -match '^\d+$' }).Count
if ($ccliCount -lt 1) {
    $ccliCount = ([regex]::Matches($ps, "/usr/sbin/ccli")).Count
}

$checks = [ordered]@{}
$checks["U-P0-07 listening 3782 (MMS TLS)"] = $ss -match "3782"
$checks["U-P0-07 listening 2404 (IEC 104 TLS)"] = $ss -match "2404"
$checks["U-D7-01 no cleartext :102"] = $ss -notmatch ":102 "
$checks["U-P0-07 single ccli PID"] = $ccliCount -eq 1
$checks["U-P0-07 no iec61850d"] = $ps -notmatch "iec61850"
$checks["U-P0-06 TLS PEMs on DUT"] = ([int]($tlsN -replace '\D', '') -ge 4)
$checks["U-P0-08 version 0.1.0-r52"] = $version -match "0\.1\.0-r52"
$checks["U-P0-08 binary SHA (manifest)"] = $sha -match $expectedSha
$checks["MMS/104 TLS in yaml"] = $yaml -match "tls_enabled: true"
$checks["Interim model lab_tg544_eth_a.cfg"] = $yaml -match "lab_tg544_eth_a\.cfg"
$checks["Modbus RTU ttyS1 (bench)"] = $yaml -match "ttyS1" -or $yaml -match "backend: rtu"

Write-Host ""
Write-Host "=== DUT unified matrix preflight @ $HostAddr ($ts) ===" -ForegroundColor Cyan
Write-Host $version
Write-Host $sha
$fail = 0
foreach ($k in $checks.Keys) {
    $ok = $checks[$k]
    if (-not $ok) { $fail++ }
    Write-Host ("[{0}] {1}" -f $(if ($ok) { "PASS" } else { "FAIL" }), $k) -ForegroundColor $(if ($ok) { "Green" } else { "Red" })
}

$report = @"
# PREFLIGHT_DUT unified matrix — $date
generated: $ts
host: $HostAddr
matrix_rows: U-P0-06 U-P0-07 U-P0-08 U-D7-01
deploy_manifest: lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r52_2026-10-14_134735.md
session_pin: lab/conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md

$version
$sha
ccli_pgrep_count: $ccliCount

## Checks
$(foreach ($k in $checks.Keys) { "- [$($(if ($checks[$k]) {'x'} else {' '}))] $k" })

## netstat
$ss

## ps
$ps

## yaml (excerpt)
$yaml
"@

if ($WriteEvidence) {
    $out = Join-Path $Repo "lab\evidence\testsuite-pro\inbox\PREFLIGHT_DUT_UNIFIED_${date}.txt"
    New-Item -ItemType Directory -Force -Path (Split-Path $out) | Out-Null
    Set-Content -Path $out -Value $report -Encoding UTF8
    Write-Host "Wrote $out" -ForegroundColor Green
}

Write-Host ""
Write-Host "5-LN Compare: needs product_tr400_eth_a_5ln.cid (DUT still on lab_tg544_eth_a.cfg)." -ForegroundColor Yellow
Write-Host "PPV from meter @980: in source tree; redeploy after wsl-build-ccli.sh (r53+)." -ForegroundColor Yellow

if ($fail -gt 0) { exit 1 }
exit 0
