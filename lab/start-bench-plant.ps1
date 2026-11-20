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
$SunSim = Join-Path $PSScriptRoot "third_party\Sun2000-100KTL-M2-Inverter-Simulator\sun2000_100ktl_m2_rtu_real_register_mock.py"
if (-not (Test-Path $Probe)) {
    $Repo = Split-Path -Parent $PSScriptRoot
    $Probe = Join-Path $Repo "lab\modbus_emt432_tcp_probe.py"
}
if (-not (Test-Path $SunSim)) {
    $Repo = Split-Path -Parent $PSScriptRoot
    $SunSim = Join-Path $Repo "lab\third_party\Sun2000-100KTL-M2-Inverter-Simulator\sun2000_100ktl_m2_rtu_real_register_mock.py"
}

if ($SunOnly) {
    if (-not (Test-Path $SunSim)) { Write-Error "Missing Sun2000 simulator: $SunSim" }
    Write-Host "=== SUN2000 inverter mock on $Port ===" -ForegroundColor Cyan
    python $SunSim --port $Port --baudrate 9600
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
Write-Host "Control check yaml: apps/ccli/config/lab_tr400_inverter_control.yaml"
Write-Host "FC06 40125 caps P. FC06 40201 takes the inverter out of service."
Write-Host ""
python $SunSim --port $Port --baudrate 9600
