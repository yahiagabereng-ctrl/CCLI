# Listen for TesPro IEC61850 northbound MQTT reports (P5 lab).
# Broker setup: .\lab\tg544-openwrt\install-mqtt-lab-broker.ps1 (Admin, once)
# Full verify:   .\lab\tg544-openwrt\verify-tespro-northbound.ps1
param(
    [string]$HostAddr = "192.168.10.10",
    [int]$Port = 1883,
    [string]$GatewayCode = "tg544-lab-gw",
    [string]$DeviceId = "cci016-lab-01"
)

$topic = "device/$GatewayCode/$DeviceId/property/report"
Write-Host "P5 northbound listen — topic: $topic"
Write-Host "Broker: ${HostAddr}:${Port}  (Ctrl+C to stop)"
Write-Host ""

if (-not (Get-Command mosquitto_sub -ErrorAction SilentlyContinue)) {
    Write-Error "mosquitto_sub not found. Install Mosquitto client tools and retry."
    exit 1
}

& mosquitto_sub -h $HostAddr -p $Port -t $topic -v
