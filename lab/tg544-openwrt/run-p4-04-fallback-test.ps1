# P4-04 — Eth_B comms-loss fallback (O.13.1.2 / iec104.comms_loss_fallback_s).
param(
    [string]$DutAddr = "192.168.1.130",
    [int]$Port = 2404,
    [int]$FallbackS = 60,
    [int]$WaitExtraS = 8
)

$ErrorActionPreference = "Stop"
$prevEap = $ErrorActionPreference
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Evidence = Join-Path $Repo "lab\evidence\phase4"
$LogFile = Join-Path $Evidence "P4_04_104_FALLBACK.txt"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$User = "root"
$Client = "/mnt/c/Yahia/projects/CCLI/lab/104/p4_cs104_client"

New-Item -ItemType Directory -Force -Path $Evidence | Out-Null
Set-Content -Path $LogFile -Value ""

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

function Get-DutLog {
    if (-not $env:CCLI_TG544_PW) { return "(no CCLI_TG544_PW)" }
    if (-not (Test-Path $Plink)) { return "(no plink)" }
    $cmd = "grep -E 'iec104|P4-04|fallback' /tmp/ccli.log 2>/dev/null | tail -25"
    $ErrorActionPreference = "Continue"
    $out = & $Plink -ssh "${User}@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $cmd 2>&1
    $ErrorActionPreference = $prevEap
    return ($out -join "`n")
}

Log "=== P4-04 Eth_B comms-loss fallback ==="
Log "DUT=${DutAddr}:${Port} fallback_s=${FallbackS} (lab yaml iec104.comms_loss_fallback_s)"
Log "DUT log before test:"
Log (Get-DutLog)

Log "Step 1: TLS connect + STARTDT + disconnect (--once --tls)..."
$ErrorActionPreference = "Continue"
$out = wsl bash -c "$Client $DutAddr $Port 1 1001 --tls --once" 2>&1
$connectExit = $LASTEXITCODE
$ErrorActionPreference = $prevEap
$out | ForEach-Object {
    $s = "$_"
    if ($s -notmatch 'RemoteException') { Log $s }
}
if ($connectExit -ne 0) {
    Log "P4-04 VERDICT: FAIL (connect/disconnect exit=$connectExit)"
    exit $connectExit
}

$waitTotal = $FallbackS + $WaitExtraS
Log "Step 2: wait ${waitTotal}s for fallback timer..."
Start-Sleep -Seconds $waitTotal

Log "Step 3: check DUT log for P4-04 fallback..."
$dutLog = Get-DutLog
Log $dutLog

$fallbackHit = $dutLog -match 'P4-04 Eth_B comms-loss fallback'
if (-not $fallbackHit) {
    Log "P4-04 VERDICT: FAIL (no fallback log after ${waitTotal}s)"
    exit 1
}
Log "PASS: fallback log seen"

Log "Step 4: reconnect (--once) to verify restore..."
$ErrorActionPreference = "Continue"
$out2 = wsl bash -c "$Client $DutAddr $Port 1 1001 --tls --once" 2>&1
$restoreExit = $LASTEXITCODE
$ErrorActionPreference = $prevEap
$out2 | ForEach-Object {
    $s = "$_"
    if ($s -notmatch 'RemoteException') { Log $s }
}

if ($restoreExit -eq 0) {
    Log "PASS: restore session after fallback"
    Log "P4-04 VERDICT: PASS"
    exit 0
}

Log "P4-04 VERDICT: PARTIAL (fallback OK, restore exit=$restoreExit)"
exit 2
