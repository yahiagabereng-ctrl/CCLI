# PC-A plant bench: EMT432 probe + SUN2000 RTU inverter mock on USB RS485
# Usage (Admin not required):
#   powershell -File lab\start-bench-plant.ps1
#   powershell -File lab\start-bench-plant.ps1 -Port COM5 -MeterHost 192.168.178.250

param(
    [string]$Port = "COM5",
    [string]$MeterHost = "",
    [switch]$ScanOnly,
    [switch]$SunOnly
)

$ErrorActionPreference = "Stop"
$Probe = Join-Path $PSScriptRoot "modbus_emt432_tcp_probe.py"
$SunSim = Join-Path $PSScriptRoot "sun2000\sun2000_100ktl_m2_rtu_sim.py"
if (-not (Test-Path $Probe)) {
    $Repo = Split-Path -Parent $PSScriptRoot
    $Probe = Join-Path $Repo "lab\modbus_emt432_tcp_probe.py"
    $SunSim = Join-Path $Repo "lab\third_party\Sun2000-100KTL-M2-Inverter-Simulator\sun2000_100ktl_m2_rtu_sim.py"
}

if ($SunOnly) {
    if (-not (Test-Path $SunSim)) { Write-Error "Missing Sun2000 simulator: $SunSim" }
    Write-Host "=== SUN2000 inverter mock on $Port ===" -ForegroundColor Cyan
    python $SunSim --port $Port --baud 9600 --slave 1
    exit $LASTEXITCODE
}

Write-Host "=== EMT432 meter check (LAN3) ===" -ForegroundColor Cyan
if ($MeterHost) {
    python $Probe --host $MeterHost
} else {
    python $Probe --host 192.168.178.250
    if ($LASTEXITCODE -ne 0) {
        python $Probe --host 192.168.30.50
    }
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Scan 192.168.178.0/24 ..." -ForegroundColor Yellow
        python $Probe --scan --subnet 192.168.178
        Write-Host "Scan 192.168.30.0/24 ..." -ForegroundColor Yellow
        python $Probe --scan --subnet 192.168.30
    }
}

if ($ScanOnly) { exit 0 }

if (-not (Test-Path $SunSim)) {
    Write-Error "Missing Sun2000 simulator: $SunSim"
}

Write-Host ""
Write-Host "=== SUN2000 inverter mock on $Port (A1/B1) ===" -ForegroundColor Cyan
Write-Host "DUT yaml: apps/ccli/config/product_tr400.yaml (P/Q/PPV from EMT432 TCP LAN3)"
Write-Host "RS485 Sun mock on COM5 -> A1/B1 — inverter plant path; not ccli Modbus when backend=tcp"
Write-Host ""
python $SunSim --port $Port --baud 9600 --slave 1
