# One-time setup: Mosquitto Windows service listens on 0.0.0.0:1883 for TG544 northbound.
# Run PowerShell as Administrator:
#   .\lab\tg544-openwrt\install-mqtt-lab-broker.ps1

#Requires -RunAsAdministrator

$ErrorActionPreference = "Stop"

$MosqDir = "${env:ProgramFiles}\mosquitto"
$ServiceConf = Join-Path $MosqDir "mosquitto.conf"
$LabConf = Join-Path $PSScriptRoot "..\mosquitto-lab.conf"
$Backup = Join-Path $MosqDir "mosquitto.conf.bak-p5"

if (-not (Test-Path $LabConf)) {
    Write-Error "Missing $LabConf"
}

Write-Host "Stopping Mosquitto service..."
Stop-Service mosquitto -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 2

if (Test-Path $ServiceConf) {
    if (-not (Test-Path $Backup)) {
        Copy-Item $ServiceConf $Backup
        Write-Host "Backed up: $Backup"
    }
}

Copy-Item $LabConf $ServiceConf -Force
Write-Host "Installed lab config to $ServiceConf"

Write-Host "Starting Mosquitto service..."
Start-Service mosquitto
Start-Sleep -Seconds 2

$listen = Get-NetTCPConnection -LocalPort 1883 -State Listen -ErrorAction SilentlyContinue
$lan = $listen | Where-Object { $_.LocalAddress -eq "0.0.0.0" -or $_.LocalAddress -eq "192.168.10.10" }
if ($lan) {
    Write-Host "PASS: broker listening on LAN (0.0.0.0:1883)"
} else {
    Write-Host "WARN: check bindings:"
    $listen | Format-Table LocalAddress, LocalPort, OwningProcess
}

$ruleName = "CCLI P5 MQTT 1883"
if (-not (Get-NetFirewallRule -DisplayName $ruleName -ErrorAction SilentlyContinue)) {
    New-NetFirewallRule -DisplayName $ruleName -Direction Inbound -Protocol TCP -LocalPort 1883 `
        -RemoteAddress 192.168.10.0/24 -Action Allow | Out-Null
    Write-Host "Firewall rule added: $ruleName"
}

Write-Host ""
Write-Host "Next: LuCI Save Channel, then:"
Write-Host "  .\lab\tg544-openwrt\verify-tespro-northbound.ps1"
