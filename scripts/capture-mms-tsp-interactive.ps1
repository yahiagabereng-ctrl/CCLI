# Interactive MMS capture — press Enter, then Connect in TSP within the duration window.
param(
    [string]$DutHost = "192.168.10.1",
    [int]$MmsPort = 3782,
    [string]$InterfaceAlias = "Ethernet",
    [int]$DurationSec = 90
)

$ErrorActionPreference = "Stop"
$tshark = "C:\Program Files\Wireshark\tshark.exe"
if (-not (Test-Path $tshark)) {
    Write-Error "Wireshark not found. Install: winget install WiresharkFoundation.Wireshark"
}

$Repo = Resolve-Path (Join-Path $PSScriptRoot "..")
$OutDir = Join-Path $Repo "lab\evidence\testsuite-pro\pcap"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$pcap = Join-Path $OutDir "TSP_P3_07_mms_${stamp}.pcapng"
$filter = "host $DutHost and port $MmsPort"

Write-Host ""
Write-Host "=== P3-07 interactive capture ==="
Write-Host "DUT       : ${DutHost}:${MmsPort}"
Write-Host "Interface : $InterfaceAlias (PC should be 192.168.10.x on this NIC)"
Write-Host "Duration  : ${DurationSec}s after you press Enter"
Write-Host ""
Write-Host "1. Open Test Suite Pro -> System Status"
Write-Host "2. Press ENTER here to start capture"
Write-Host "3. Immediately click Connect (within ${DurationSec}s)"
Write-Host ""
Read-Host "Press Enter to start capture"

Write-Host ""
Write-Host 'CAPTURE RUNNING - Connect NOW'
Write-Host ""

& $tshark -i $InterfaceAlias -f $filter -a "duration:$DurationSec" -w $pcap
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host ""
Write-Host "Saved: $pcap"

$frames = @(& $tshark -r $pcap -T fields -e frame.number 2>$null)
if ($frames.Count -eq 0) {
    Write-Host ""
    Write-Host "WARN: 0 packets — Connect was not during capture."
    Write-Host "Re-run: .\scripts\capture-mms-tsp-interactive.ps1"
    exit 2
}

Write-Host "OK: $($frames.Count) frames captured."
Write-Host ""
Write-Host "First TLS/TCP frames:"
& $tshark -r $pcap -T fields -e frame.number -e frame.time_relative -e ip.src -e tcp.len -e tls.record.content_type 2>$null |
    Select-Object -First 25 | ForEach-Object { Write-Host $_ }
Write-Host ""
Write-Host "Decrypt: Wireshark -> TLS RSA keys -> ${DutHost}:${MmsPort} -> C:\CCLI_tls\server.key"
exit 0
