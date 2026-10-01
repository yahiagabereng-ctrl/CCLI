# Open Wireshark GUI with MMS capture filter on lab Ethernet.
$wireshark = "C:\Program Files\Wireshark\Wireshark.exe"
if (-not (Test-Path $wireshark)) {
    Write-Error "Install Wireshark: winget install WiresharkFoundation.Wireshark"
}
if (-not (Test-Path "C:\Windows\System32\Npcap\wpcap.dll") -and -not (Test-Path "C:\Program Files\Npcap\wpcap.dll")) {
    Write-Host "Npcap required. Launching installer..."
    if (Test-Path "$env:TEMP\npcap-install.exe") { Start-Process "$env:TEMP\npcap-install.exe" }
    exit 1
}

$filter = "host 192.168.10.1 and port 3782"
Write-Host "Opening Wireshark on Ethernet, capture filter: $filter"
Write-Host "1. Double-click Ethernet when interface list appears"
Write-Host "2. TSP Connect to 192.168.10.1:3782"
Write-Host "3. Stop capture (red square), save .pcapng to lab/evidence/testsuite-pro/pcap/"
Start-Process $wireshark -ArgumentList @("-i", "Ethernet", "-f", $filter)
