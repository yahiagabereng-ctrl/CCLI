# Verify TesPro northbound prerequisites: Modbus + MMS TotW + optional DUT log.
# Usage:
#   python -u lab\modbus_rtu_slave.py --port COM5 --power-kw 450 --trace
#   $env:CCLI_TG544_PW = "root-password"   # optional SSH tail
#   .\lab\tg544-openwrt\verify-tespro-points-live.ps1

param(
    [string]$DutAddr = "192.168.10.1",
    [int]$MmsPort = 102,
    [string]$ModbusPort = "COM5"
)

$LabClient = Join-Path $PSScriptRoot "mms_lab_client"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$LabEnv = Join-Path $PSScriptRoot "lab-env.ps1"

function Log($msg) { Write-Host "[$(Get-Date -Format 'HH:mm:ss')] $msg" }

if (Test-Path $LabEnv) { . $LabEnv }

Log "=== P5 TesPro points live verify ==="

$ping = Test-Connection -ComputerName $DutAddr -Count 1 -Quiet -ErrorAction SilentlyContinue
Log "Ping $DutAddr : $(if ($ping) { 'OK' } else { 'FAIL' })"

$tcp = Test-NetConnection -ComputerName $DutAddr -Port $MmsPort -WarningAction SilentlyContinue -ErrorAction SilentlyContinue
Log "MMS ${DutAddr}:${MmsPort} : $(if ($tcp.TcpTestSucceeded) { 'OPEN' } else { 'CLOSED' })"

$mb = Get-CimInstance Win32_Process -Filter "Name='python.exe'" -ErrorAction SilentlyContinue |
    Where-Object { $_.CommandLine -match 'modbus_rtu_slave' -and $_.CommandLine -match [regex]::Escape($ModbusPort) }
if ($mb) {
    Log "Modbus slave on $ModbusPort : RUNNING (PID $($mb.ProcessId))"
} else {
    Log "Modbus slave on $ModbusPort : NOT RUNNING"
    Log "  Start: python -u lab\modbus_rtu_slave.py --port $ModbusPort --power-kw 450 --trace"
}

if (-not (Test-Path $LabClient)) {
    Log "mms_lab_client missing - run lab\tg544-openwrt\build-mms-lab-client.sh in WSL"
} else {
    Log '--- MMS read 8 sec ---'
    $out = wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_lab_client $DutAddr $MmsPort 8 2>&1" 2>&1
    $totw = ($out | Select-String -Pattern 'TotW\.mag\.f = ([0-9.+-]+)' | Select-Object -Last 1)
    $totvar = ($out | Select-String -Pattern 'TotVAr\.mag\.f = ([0-9.+-]+)' | Select-Object -Last 1)
    $ppv = ($out | Select-String -Pattern '20\.000000' | Select-Object -First 1)
    if ($totw) { Log "TotW.mag.f = $($totw.Matches.Groups[1].Value)" } else { Log "TotW read: not found" }
    if ($totvar) { Log "TotVAr.mag.f = $($totvar.Matches.Groups[1].Value)" } else { Log "TotVAr read: not found" }
    if ($ppv) { Log "PPV in report: ~20 kV (yaml path OK)" }
    if ($totw -and [double]$totw.Matches.Groups[1].Value -ge 400) {
        Log "PASS: TotW live - TesPro LuCI should show ~450 within 8 s"
    } else {
        Log 'FAIL: TotW=0 - check RS485 COM5 to rs485_2_uart; DUT log ERR=read_fail'
    }
}

if ($env:CCLI_TG544_PW -and (Test-Path $Plink)) {
    Log "--- DUT modbus log (plink) ---"
    $cmd = "grep modbus /tmp/ccli.log | tail -8; ls -l /dev/rs485_2_uart 2>&1"
    & $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $cmd 2>&1 |
        ForEach-Object { Log "  $_" }
} else {
    Log "SSH skip - set `$env:CCLI_TG544_PW for DUT log tail"
}

Log "=== end ==="
