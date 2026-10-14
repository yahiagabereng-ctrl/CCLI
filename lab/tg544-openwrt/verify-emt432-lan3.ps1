# EMT432 on LAN3 — PC plant NIC + DUT cross-check + Modbus TCP probe.
#
# Bench B (Chronos SN 261001102952):
#   DUT lan3 192.168.178.1/24 · meter 192.168.178.250:502 · PC 192.168.178.10
#   Admin: lab\set_pc_lan3_plant_178.cmd on the NIC cabled to DUT LAN3 (often "Ethernet 2")
#
# Bench A (zone docs):
#   192.168.30.1 / .30.50 / .30.10 — lab\set_pc_lan3_plant.cmd
#
# Usage:
#   $env:CCLI_TG544_PW = "..."   # optional; enables DUT ping + yaml audit via LAN2 SSH
#   .\lab\tg544-openwrt\verify-emt432-lan3.ps1
#   .\lab\tg544-openwrt\verify-emt432-lan3.ps1 -MeterHost 192.168.178.250 -UseSshTunnel
#
# If PC has no LAN3 cable/NIC, -UseSshTunnel probes via DUT (requires LAN2 ping + plink).

param(
    [string]$DutAddr = "192.168.1.130",
    [string]$MeterHost178 = "192.168.178.250",
    [string]$MeterHost30 = "192.168.30.50",
    [string]$User = "root",
    [switch]$UseSshTunnel,
    [switch]$SkipProbe
)

$ErrorActionPreference = "Continue"
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Probe = Join-Path $RepoRoot "lab\modbus_emt432_tcp_probe.py"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$EvidenceDir = Join-Path $RepoRoot "lab\evidence\phase5"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$LogFile = Join-Path $EvidenceDir "P5_EMT432_LAN3_VERIFY_$Stamp.txt"

function Write-Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null
Write-Log "=== EMT432 LAN3 verify ==="

# --- PC plant NIC ---
Write-Log "--- PC IPv4 (plant subnets) ---"
$plantIps = Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
    Where-Object { $_.IPAddress -match "^192\.168\.(178|30)\." }
if ($plantIps) {
    $plantIps | ForEach-Object { Write-Log "  PASS: $($_.IPAddress) on $($_.InterfaceAlias)" }
    $on178 = $plantIps | Where-Object { $_.IPAddress -match "^192\.168\.178\." }
    $on30 = $plantIps | Where-Object { $_.IPAddress -match "^192\.168\.30\." }
} else {
    Write-Log "  WARN: no 192.168.178.x or 192.168.30.x on this PC."
    Write-Log "        Cable PC second NIC to DUT LAN3; run set_pc_lan3_plant_178.cmd (or _plant.cmd)."
    Write-Log "        LAN2-only PC cannot ping/probe the meter directly."
}

foreach ($pair in @(
        @{ Host = $MeterHost178; Label = "Plan B (178.x)" },
        @{ Host = $MeterHost30; Label = "Plan A (30.x)" }
    )) {
    $h = $pair.Host
    if (Test-Connection -ComputerName $h -Count 2 -Quiet -ErrorAction SilentlyContinue) {
        Write-Log "PASS: ping $h ($($pair.Label))"
    } else {
        Write-Log "FAIL: ping $h ($($pair.Label))"
    }
}

# --- DUT (SSH over LAN2) ---
if ($env:CCLI_TG544_PW -and (Test-Path $Plink)) {
    Write-Log "--- DUT LAN3 (SSH via $DutAddr) ---"
    $remote = "uci get network.lan2_sec.ipaddr; ip addr show dev lan3 2>/dev/null | grep inet; ping -c 2 -W 2 $MeterHost178; ping -c 2 -W 2 $MeterHost30; grep -E '^  backend:|^  host:|^  device:' /etc/ccli/lab.yaml 2>/dev/null | head -6"
    $out = & $Plink -batch -hostkey $HostKey -pw $env:CCLI_TG544_PW "${User}@${DutAddr}" $remote 2>&1
    $out | ForEach-Object { Write-Log "  $_" }
} else {
    Write-Log "SKIP: DUT SSH (set CCLI_TG544_PW and install PuTTY plink)."
}

if ($SkipProbe) {
    Write-Log "SKIP: Modbus probe (-SkipProbe)"
    Write-Log "=== end ==="
    Write-Host "`nEvidence: $LogFile"
    exit 0
}

if (-not (Test-Path $Probe)) {
    Write-Log "FAIL: missing $Probe"
    exit 1
}

$probeHost = $MeterHost178
$probePort = 502
$tunnelProc = $null

if ($UseSshTunnel -or -not $plantIps) {
    if (-not ($env:CCLI_TG544_PW -and (Test-Path $Plink))) {
        Write-Log "FAIL: need -UseSshTunnel with CCLI_TG544_PW + plink, or configure PC LAN3 NIC."
        exit 1
    }
    if (-not (Test-Connection -ComputerName $DutAddr -Count 1 -Quiet)) {
        Write-Log "FAIL: ping DUT $DutAddr (run set_pc_lan2_oa.cmd for LAN2)."
        exit 1
    }
    $probePort = 15002
    $probeHost = "127.0.0.1"
    Write-Log "--- Modbus probe via SSH tunnel (127.0.0.1:$probePort -> ${MeterHost178}:502) ---"
    $tunnelProc = Start-Process -FilePath $Plink -PassThru -WindowStyle Hidden -ArgumentList @(
        "-batch", "-hostkey", $HostKey, "-pw", $env:CCLI_TG544_PW,
        "-L", "${probePort}:${MeterHost178}:502", "-N", "${User}@${DutAddr}"
    )
    Start-Sleep -Seconds 2
} else {
    if ($on30 -and -not $on178) { $probeHost = $MeterHost30 }
    Write-Log "--- Modbus probe (direct $probeHost:502) ---"
    Write-Log "  pip install pymodbus  # if needed"
}

$probeOut = python $Probe --host $probeHost --port $probePort 2>&1
$probeOut | ForEach-Object { Write-Log $_ }

if ($tunnelProc -and -not $tunnelProc.HasExited) {
    Stop-Process -Id $tunnelProc.Id -Force -ErrorAction SilentlyContinue
}

$exitCode = $LASTEXITCODE
Write-Log "=== end (probe exit $exitCode) ==="
Write-Host "`nEvidence: $LogFile"
Write-Host "Matrix evidence name: modbus_emt432_${probeHost}_${Stamp}.txt (copy log if needed)"
exit $exitCode
