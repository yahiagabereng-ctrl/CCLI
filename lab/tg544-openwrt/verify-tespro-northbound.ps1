# P5 — verify TesPro IEC 61850 MQTT northbound end-to-end.
# Prerequisites:
#   - LuCI channel_1 saved: Enabled, Host 192.168.10.10:1883, Publish topic set
#   - ccli + Modbus slave running (TotW ~450)
# Usage:
#   .\lab\tg544-openwrt\start-mqtt-broker.ps1 -Background
#   .\lab\tg544-openwrt\verify-tespro-northbound.ps1
#   .\lab\tg544-openwrt\verify-tespro-northbound.ps1 -ListenSeconds 30

param(
    [string]$HostAddr = "192.168.10.10",
    [int]$Port = 1883,
    [string]$GatewayCode = "tg544-lab-gw",
    [string]$DeviceId = "cci016-lab-01",
    [int]$ListenSeconds = 20,
    [string]$EvidenceDir = (Join-Path $PSScriptRoot "..\evidence\phase5")
)

$ErrorActionPreference = "Continue"
$topic = "device/$GatewayCode/$DeviceId/property/report"
$stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$evidenceFile = Join-Path $EvidenceDir "P5_TESPRO_NORTHBOUND_$stamp.txt"

function Log($msg) {
    $line = "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $msg"
    Write-Host $line
    Add-Content -Path $evidenceFile -Value $line -Encoding UTF8
}

function Find-MosquittoSub {
    $cmd = Get-Command mosquitto_sub -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    $paths = @(
        "${env:ProgramFiles}\mosquitto\mosquitto_sub.exe",
        "${env:ProgramFiles(x86)}\mosquitto\mosquitto_sub.exe"
    )
    foreach ($p in $paths) {
        if (Test-Path $p) { return $p }
    }
    return $null
}

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null
Log "=== P5 TesPro northbound verify ==="
Log "Topic: $topic"
Log "Broker: ${HostAddr}:${Port}"
Log "Listen: ${ListenSeconds}s"
Log ""

# PC IP on lab subnet
$labIp = Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
    Where-Object { $_.IPAddress -eq $HostAddr }
if ($labIp) {
    Log "PC lab IP $HostAddr on $($labIp.InterfaceAlias): OK"
} else {
    Log "WARN: PC does not have $HostAddr - LuCI Host must match PC Eth_A"
}

# DUT reachability
$ping = Test-Connection -ComputerName 192.168.10.1 -Count 1 -Quiet -ErrorAction SilentlyContinue
Log "Ping 192.168.10.1: $(if ($ping) { 'OK' } else { 'FAIL' })"

$mms = Test-NetConnection -ComputerName 192.168.10.1 -Port 102 -WarningAction SilentlyContinue -ErrorAction SilentlyContinue
Log "MMS :102: $(if ($mms.TcpTestSucceeded) { 'OPEN' } else { 'CLOSED' })"

$brokerListen = Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue
$lanListen = $brokerListen | Where-Object { $_.LocalAddress -eq "0.0.0.0" }
if ($lanListen) {
    Log "MQTT broker 0.0.0.0:$Port LISTENING (pid $($lanListen.OwningProcess))"
} elseif ($brokerListen) {
    Log "WARN: broker on localhost only - DUT cannot connect. Run install-mqtt-lab-broker.ps1 as Admin."
} else {
    Log "FAIL: No broker on :$Port - run install-mqtt-lab-broker.ps1 (Admin) or start-mqtt-broker.ps1"
    Log "=== end (blocked) ==="
    exit 2
}

$sub = Find-MosquittoSub
if (-not $sub) {
    Log "FAIL: mosquitto_sub not found - winget install EclipseFoundation.Mosquitto"
    Log "=== end (blocked) ==="
    exit 3
}

Log ('--- MQTT capture (' + $ListenSeconds + ' sec) ---')
$tmpOut = Join-Path $env:TEMP "p5_mqtt_capture_$stamp.txt"
$proc = Start-Process -FilePath $sub -ArgumentList @("-h", $HostAddr, "-p", "$Port", "-t", $topic, "-v", "-W", "$ListenSeconds") `
    -RedirectStandardOutput $tmpOut -RedirectStandardError "$tmpOut.err" -PassThru -NoNewWindow -Wait

$capture = @()
if (Test-Path $tmpOut) { $capture = Get-Content $tmpOut -ErrorAction SilentlyContinue }
foreach ($line in $capture) { Log "  $line" }
if (Test-Path "$tmpOut.err") {
    Get-Content "$tmpOut.err" -ErrorAction SilentlyContinue | ForEach-Object { Log "  [stderr] $_" }
}

$jsonLines = $capture | Where-Object { $_ -match '\{' }
$passTotw = $false
$passJson = $false
$lastJson = $null

foreach ($line in $jsonLines) {
    if ($line -match '\{.*\}') {
        $lastJson = $Matches[0]
        if ($lastJson -match 'PdC_TotW' -and ($lastJson -match '"value"\s*:\s*450|"PdC_TotW"[^}]*450|450\.0')) {
            $passTotw = $true
        }
        if ($lastJson -match 'REPORT_PROPERTY') { $passJson = $true }
    }
}

# Fallback: any line mentioning TotW and 450
if (-not $passTotw) {
    $passTotw = ($capture | Where-Object { $_ -match 'PdC_TotW' -and $_ -match '450' }).Count -gt 0
}
if (-not $passJson) {
    $passJson = ($capture | Where-Object { $_ -match 'REPORT_PROPERTY' }).Count -gt 0
}

Log ""
Log '--- Results ---'
Log "MQTT messages received: $($jsonLines.Count)"
Log "REPORT_PROPERTY JSON: $(if ($passJson) { 'PASS' } else { 'FAIL' })"
Log "PdC_TotW ~450: $(if ($passTotw) { 'PASS' } else { 'FAIL / no message yet' })"

if (-not $passJson -and $jsonLines.Count -eq 0) {
    Log ""
    Log "Troubleshooting:"
    Log "  1. LuCI Northbound: Enabled ON, Save Channel, Status Connected"
    Log "  2. Publish Topic typed explicitly (not empty placeholder)"
    Log "  3. Clear Subscribe/Reply topics for lab"
    Log "  4. Windows firewall: allow inbound TCP $Port from 192.168.10.1"
    Log "  5. Restart IEC 61850 service on DUT after save"
}

$exitCode = if ($passJson -and $passTotw) { 0 } elseif ($jsonLines.Count -gt 0) { 1 } else { 2 }
Log "Evidence: $evidenceFile"
Log "=== end (exit $exitCode) ==="
exit $exitCode
