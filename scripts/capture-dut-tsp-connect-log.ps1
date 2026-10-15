# Live DUT log capture while PC1 (Test Suite Pro) attempts Connect on LAN1.
# Run on logger PC (PC-A) with SSH to DUT on LAN2; Masomeh Connects on 192.168.10.1:3782.
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\capture-dut-tsp-connect-log.ps1
#   powershell -File scripts\capture-dut-tsp-connect-log.ps1 -DurationSec 120 -OutFile lab\evidence\testsuite-pro\inbox\MASOMEH_CONNECT_DUT_PULL_2026-10-15.txt
#
param(
    [string]$HostAddr = "192.168.1.130",
    [string]$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE",
    [int]$DurationSec = 90,
    [string]$OutFile = "",
    [switch]$NoPrompt
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Plink = "C:\Program Files\PuTTY\plink.exe"
if (-not (Test-Path $Plink)) { throw "Install PuTTY plink.exe" }
if (-not $env:CCLI_TG544_PW) { throw "Source lab\tg544-openwrt\lab-env.ps1 first" }

$date = Get-Date -Format "yyyy-MM-dd"
if (-not $OutFile) {
    $OutFile = Join-Path $Repo "lab\evidence\testsuite-pro\inbox\TSP_CONNECT_DUT_LOG_${date}.txt"
}
$OutFile = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutFile)
New-Item -ItemType Directory -Force -Path (Split-Path $OutFile) | Out-Null

Write-Host ""
Write-Host "=== DUT log capture (TestPro Connect window) ===" -ForegroundColor Cyan
Write-Host "SSH DUT   : root@${HostAddr} (LAN2)"
Write-Host "MMS target: 192.168.10.1:3782 (PC1 LAN1 -> Masomeh Connect)"
Write-Host "Duration  : ${DurationSec}s after you press Enter"
Write-Host "Output    : $OutFile"
Write-Host ""
Write-Host "1. Masomeh: Test Suite Pro open, certs wired, ready on Connect"
Write-Host "2. Press ENTER here, then she clicks Connect within ${DurationSec}s"
Write-Host ""

if (-not $NoPrompt) {
    Read-Host "Press Enter to start DUT log follow"
}

$header = @(
    "# DUT log - TestPro Connect attempt",
    "generated_local: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')",
    "ssh_host: $HostAddr",
    "mms: 192.168.10.1:3782",
    "duration_sec: $DurationSec",
    ""
) -join "`r`n"
Set-Content -Path $OutFile -Value $header -Encoding UTF8

Write-Host "CAPTURE RUNNING - Masomeh Connect NOW" -ForegroundColor Yellow
Write-Host ""

$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$remoteSh = Join-Path $Repo "lab\evidence\testsuite-pro\capture-dut-tsp-connect-remote.sh"
if (-not (Test-Path $remoteSh)) { throw "Missing $remoteSh" }
if (-not (Test-Path $Pscp)) { throw "Install PuTTY pscp.exe" }

& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $remoteSh "root@${HostAddr}:/tmp/capture-dut-tsp-connect-remote.sh" | Out-Null
$remoteCmd = "sed -i 's/\r$//' /tmp/capture-dut-tsp-connect-remote.sh; chmod +x /tmp/capture-dut-tsp-connect-remote.sh; sh /tmp/capture-dut-tsp-connect-remote.sh $DurationSec"

$prevEap = $ErrorActionPreference
$ErrorActionPreference = "Continue"
$out = & $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $remoteCmd 2>&1 | Out-String
$ErrorActionPreference = $prevEap

Add-Content -Path $OutFile -Value $out -Encoding UTF8

Write-Host "Saved: $OutFile" -ForegroundColor Green
Write-Host ""
Write-Host "Quick grep hints in output:" -ForegroundColor Cyan
Write-Host "  PASS: ACSE auth ACCEPT, AARE responder auth [10]EXTERNAL, Initiate (no signature error)"
Write-Host "  FAIL: handshake failed, ACSE auth REJECT, Transport connection closed, bind FAILED"
Write-Host ""

if ($out -match 'ACSE auth ACCEPT') {
    Write-Host "Seen: ACSE auth ACCEPT" -ForegroundColor Green
}
if ($out -match 'handshake failed|ACSE auth REJECT|Transport connection closed') {
    Write-Host "Seen: likely Connect failure signature in DUT log" -ForegroundColor Red
}

exit 0
