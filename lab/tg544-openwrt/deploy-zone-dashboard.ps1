# Deploy the read-only zone dashboard (DSO / Operator / Plant) to TesPro TG544.
# Uploads www/ccli/* + cgi-bin/ccli-bench only. Does NOT touch /etc/ccli/lab.yaml or /usr/sbin/ccli
# (deploy the r33+ binary through the normal manifest flow for the zone status block).
#
# Usage:
#   python scripts/plant-map-generate.py          # refresh plant_map.json from the CSV
#   $env:CCLI_TG544_PW = "password"
#   .\lab\tg544-openwrt\deploy-zone-dashboard.ps1                    # bench on LAN1
#   .\lab\tg544-openwrt\deploy-zone-dashboard.ps1 -HostAddr 192.168.1.130
# Browser: http://<HostAddr>/ccli/zone_dashboard.html

param(
    [string]$HostAddr = "192.168.10.1",
    [string]$User = "root"
)

$ErrorActionPreference = "Stop"
$Www = Join-Path $PSScriptRoot "www\ccli"
$Cgi = Join-Path $PSScriptRoot "cgi-bin\ccli-bench"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
# Eth_A 192.168.10.1 and operator LAN 192.168.1.130 present different host-key fingerprints.
# TG544 presents the same host key on LAN1/LAN2/LAN3 in current lab images.
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"

if (-not $env:CCLI_TG544_PW) { Write-Error 'Set $env:CCLI_TG544_PW' }
$Files = @("index.html", "zone_dashboard.html", "zone_dashboard.js", "plant_map.json", "mock_console.html")
foreach ($f in $Files) {
    if (-not (Test-Path (Join-Path $Www $f))) { Write-Error "Missing $Www\$f (run scripts/plant-map-generate.py for plant_map.json)" }
}

$pw = $env:CCLI_TG544_PW
$plinkArgs = @("-ssh", "${User}@${HostAddr}", "-pw", $pw, "-hostkey", $HostKey, "-batch")

Write-Host "=== Upload zone dashboard -> $HostAddr ==="
& $Plink @plinkArgs "mkdir -p /www/ccli /www/cgi-bin"
foreach ($f in $Files) {
    & $Pscp -scp -batch -pw $pw -hostkey $HostKey (Join-Path $Www $f) "${User}@${HostAddr}:/www/ccli/$f"
}
& $Pscp -scp -batch -pw $pw -hostkey $HostKey $Cgi "${User}@${HostAddr}:/www/cgi-bin/ccli-bench"
& $Plink @plinkArgs "sed -i 's/\r$//' /www/cgi-bin/ccli-bench && chmod +x /www/cgi-bin/ccli-bench"

Write-Host "=== Smoke check ==="
& $Plink @plinkArgs "/usr/sbin/ccli --bench-status --json | grep -q zones && echo 'zones block OK' || echo 'zones block missing - deploy ccli r33+'; /usr/sbin/ccli --config /etc/ccli/lab.yaml --event-dump --count 3 --json | head -c 300; echo"

Write-Host ""
Write-Host "Open: http://${HostAddr}/ccli/zone_dashboard.html"
Write-Host "API:  http://${HostAddr}/cgi-bin/ccli-bench?action=status | ?action=events"
