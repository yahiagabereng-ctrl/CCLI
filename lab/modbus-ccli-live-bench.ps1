# PC Modbus slave + TG544 CCLI regulation finalize (live)
# Usage:
#   $env:CCLI_TG544_PW = "your-password"
#   .\lab\modbus-ccli-live-bench.ps1 -ComPort COM5 -PowerKw 45

param(
    [string]$ComPort = "COM5",
    [double]$PowerKw = 45.0,
    [int]$LiveWatchSec = 8,
    [string]$HostAddr = "192.168.1.130",
    [string]$User = "root",
    [switch]$NoSlave
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path $PSScriptRoot -Parent
$OutJson = Join-Path $Repo "regulation_bench\last-reg.json"
$SlaveScript = Join-Path $Repo "modbus_rtu_slave.py"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:2hiPouwqC1oxP//Q0BpvgIcqD6IG+pqLkzNih1EpNRE"

if (-not $env:CCLI_TG544_PW) {
    Write-Error 'Set $env:CCLI_TG544_PW'
}

if (-not $NoSlave) {
    Write-Host "=== Start PC Modbus slave (trace) on $ComPort P=$PowerKw kW ==="
    Start-Process -FilePath "python" -ArgumentList @(
        $SlaveScript, "--port", $ComPort, "--power-kw", $PowerKw, "--trace"
    ) -WorkingDirectory $Repo
    Start-Sleep -Seconds 3
}

$pw = $env:CCLI_TG544_PW
$plinkArgs = @("-ssh", "${User}@${HostAddr}", "-pw", $pw, "-hostkey", $HostKey, "-batch")

Write-Host "=== TG544: stop ccli (free ttyS2), live regulation JSON (${LiveWatchSec}s) ==="
$remote = @'
/etc/init.d/ccli stop 2>/dev/null || true
sleep 1
/usr/sbin/ccli --regulation-check --live --live-watch WATCH_SEC --json --config /etc/ccli/lab.yaml
/etc/init.d/ccli start 2>/dev/null || true
'@ -replace 'WATCH_SEC', [string]$LiveWatchSec

$json = & $Plink @plinkArgs $remote
$json | Set-Content -Path $OutJson -Encoding utf8

Write-Host "=== Saved $OutJson ==="
Write-Host "Open lab\regulation_bench\index.html and Import JSON (last-reg.json)"
Write-Host "PC slave window: watch SLAVE RX lines; compare to modbus_log in JSON"

# Quick PASS/FAIL count
try {
    $j = $json | ConvertFrom-Json
    $pass = ($j.rows | Where-Object { $_.status -eq "PASS" }).Count
    $fail = ($j.rows | Where-Object { $_.status -eq "FAIL" }).Count
    Write-Host "Finalize: PASS=$pass FAIL=$fail rows=$($j.rows.Count)"
} catch {
    Write-Warning "Could not parse JSON — check plink output / redeploy ccli binary"
}
