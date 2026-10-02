# Start a lab MQTT broker on PC Eth_A for TesPro northbound (P5).
# Usage:
#   .\lab\tg544-openwrt\start-mqtt-broker.ps1
#   .\lab\tg544-openwrt\start-mqtt-broker.ps1 -Background

param(
    [int]$Port = 1883,
    [string]$BrokerId = "lab-broker",
    [switch]$Background,
    [switch]$AllowFirewall
)

$LabConf = Join-Path $PSScriptRoot "..\mosquitto-lab.conf"

$ErrorActionPreference = "Stop"

function Find-Mosquitto {
    $cmd = Get-Command mosquitto -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    $paths = @(
        "${env:ProgramFiles}\mosquitto\mosquitto.exe",
        "${env:ProgramFiles(x86)}\mosquitto\mosquitto.exe"
    )
    foreach ($p in $paths) {
        if (Test-Path $p) { return $p }
    }
    return $null
}

$mosq = Find-Mosquitto
if (-not $mosq) {
    Write-Host "Mosquitto not found. Install:"
    Write-Host "  winget install EclipseFoundation.Mosquitto"
    Write-Host "Then run as Admin (one-time LAN bind):"
    Write-Host "  .\lab\tg544-openwrt\install-mqtt-lab-broker.ps1"
    exit 1
}

$svc = Get-Service mosquitto -ErrorAction SilentlyContinue
if ($svc -and $svc.Status -eq "Running") {
    $lan = Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue |
        Where-Object { $_.LocalAddress -eq "0.0.0.0" }
    if ($lan) {
        Write-Host "Mosquitto service already listening on 0.0.0.0:$Port - OK"
        exit 0
    }
    Write-Host "Mosquitto service is localhost-only. Run as Administrator:"
    Write-Host "  .\lab\tg544-openwrt\install-mqtt-lab-broker.ps1"
    exit 1
}

$listen = Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue
$labListen = $listen | Where-Object { $_.LocalAddress -notin @("127.0.0.1", "::1") }
if ($labListen) {
    Write-Host "Port $Port already listening on LAN [pid $($labListen.OwningProcess)] - broker likely up."
    exit 0
}
if ($listen -and -not $labListen) {
    Write-Host "WARN: mosquitto on localhost only - stopping PID $($listen.OwningProcess | Select-Object -First 1) for lab rebind."
    $listen | Select-Object -ExpandProperty OwningProcess -Unique | ForEach-Object { Stop-Process -Id $_ -Force -ErrorAction SilentlyContinue }
    Start-Sleep -Seconds 1
}

if (-not (Test-Path $LabConf)) {
    Write-Error "Missing lab config: $LabConf"
}

if ($AllowFirewall) {
    $ruleName = "CCLI P5 MQTT 1883"
    if (-not (Get-NetFirewallRule -DisplayName $ruleName -ErrorAction SilentlyContinue)) {
        Write-Host "Adding firewall rule: $ruleName (requires admin)"
        New-NetFirewallRule -DisplayName $ruleName -Direction Inbound -Protocol TCP -LocalPort $Port `
            -RemoteAddress 192.168.10.0/24 -Action Allow -ErrorAction SilentlyContinue | Out-Null
    }
}

$args = @("-c", $LabConf, "-v", "-i", $BrokerId)

Write-Host "Starting MQTT broker: $mosq $($args -join ' ')"

if ($Background) {
    Start-Process -FilePath $mosq -ArgumentList $args -WindowStyle Minimized
    Start-Sleep -Seconds 2
    $listen = Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue
    if ($listen) {
        Write-Host "PASS: broker listening on :$Port [pid $($listen.OwningProcess)]"
    } else {
        Write-Error "Broker did not bind to port $Port"
    }
} else {
    Write-Host "Foreground mode - Ctrl+C to stop."
    & $mosq @args
}
