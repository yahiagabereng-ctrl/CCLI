# Capture MMS/TLS traffic between PC (192.168.10.x) and TG544 DUT (192.168.10.1:3782).
# Requires: Wireshark + Npcap (run npcap installer once; enable "WinPcap API-compatible Mode").
param(
    [string]$DutHost = "192.168.10.1",
    [int]$MmsPort = 3782,
    [string]$InterfaceAlias = "Ethernet",
    [int]$DurationSec = 120,
    [string]$OutDir = ""
)

$ErrorActionPreference = "Stop"
$tshark = "C:\Program Files\Wireshark\tshark.exe"
if (-not (Test-Path $tshark)) {
    Write-Error "Wireshark not found. Install: winget install WiresharkFoundation.Wireshark"
}

if (-not (Test-Path "C:\Windows\System32\Npcap\wpcap.dll") -and -not (Test-Path "C:\Program Files\Npcap\wpcap.dll")) {
    Write-Host @"

Npcap is required for live capture (one-time install):
  1. Run: $env:TEMP\npcap-install.exe  (or https://npcap.com/#download)
  2. Check: WinPcap API-compatible Mode
  3. Re-run this script

"@
    if (Test-Path "$env:TEMP\npcap-install.exe") {
        Start-Process "$env:TEMP\npcap-install.exe"
    }
    exit 1
}

if (-not $OutDir) {
    $OutDir = Join-Path (Resolve-Path (Join-Path $PSScriptRoot "..")).Path "lab\evidence\testsuite-pro\pcap"
}
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$pcap = Join-Path $OutDir "TSP_P3_07_mms_${stamp}.pcapng"
$filter = "host $DutHost and port $MmsPort"

Write-Host "=== MMS/TSP capture ==="
Write-Host "Interface : $InterfaceAlias"
Write-Host "Filter    : $filter"
Write-Host "Duration  : ${DurationSec}s"
Write-Host "Output    : $pcap"
Write-Host ""
Write-Host "Start TSP Connect to CCI016_01 @ ${DutHost}:${MmsPort} now"
Write-Host ""

& $tshark -i $InterfaceAlias -f $filter -a "duration:$DurationSec" -w $pcap
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host ""
Write-Host "Saved: $pcap"
Write-Host "Open in Wireshark: wireshark `"$pcap`""
Write-Host "TLS keys: if needed, export SSLKEYLOGFILE from client (TSP may not support)."
Write-Host "Follow TLS: right-click TLS packet -> Follow -> TLS stream (needs RSA key or key log)."
