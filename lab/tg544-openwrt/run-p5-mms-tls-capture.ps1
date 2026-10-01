# P5-M06 — MMS TLS test + Wireshark/tshark capture on LAN1 (Eth_A :3782).
param(
    [string]$DutAddr = "192.168.10.1",
    [int]$Port = 3782,
    [string]$Iface = "8",
    [int]$CaptureSeconds = 35,
    [string]$CertDir = "/mnt/c/Yahia/projects/CCLI/apps/ccli/config/tls"
)

$ErrorActionPreference = "Stop"
$prevEap = $ErrorActionPreference
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Evidence = Join-Path $Repo "lab\evidence\phase5"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$Pcap = Join-Path $Evidence "P5_MMS_TLS_$Stamp.pcapng"
$TxtLog = Join-Path $Evidence "P5_MMS_TLS.txt"
$SessionLog = Join-Path $Evidence "P5_MMS_TLS_SESSION.txt"
$Tshark = "C:\Program Files\Wireshark\tshark.exe"
$Client = Join-Path $Repo "lab\tg544-openwrt\mms_tls_client"

New-Item -ItemType Directory -Force -Path $Evidence | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $TxtLog -Value $line
}

Log "=== P5 MMS TLS capture + test ==="
Log "DUT=${DutAddr}:${Port} iface=$Iface (LAN1 Ethernet)"
Log "PC=$(Get-NetIPAddress -AddressFamily IPv4 | Where-Object { $_.InterfaceIndex -eq [int]$Iface } | Select-Object -ExpandProperty IPAddress -First 1)"

if (-not (Test-Path $Tshark)) { throw "Install Wireshark (tshark) at $Tshark" }
if (-not (Test-Path $Client)) { throw "Build client: lab/tg544-openwrt/build-mms-lab-client.sh" }

$filter = "host $DutAddr and port $Port"
Log "Capture filter: $filter"
Log "Writing: $Pcap"

$pcapW = ($Pcap -replace '\\', '/')
$tsharkArgStr = "-i $Iface -f `"$filter`" -a duration:$CaptureSeconds -w `"$pcapW`""
$cap = Start-Process -FilePath $Tshark -ArgumentList $tsharkArgStr -PassThru -NoNewWindow
Start-Sleep -Seconds 2

Log "Running mms_tls_client (TLS MMS read TotW/TotVAr/PPV) ..."
$wslCmd = "cd $CertDir; /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_tls_client $DutAddr $Port ."
$ErrorActionPreference = "Continue"
$out = wsl bash -lc $wslCmd 2>&1
$exitTls = $LASTEXITCODE
$ErrorActionPreference = $prevEap
$out | ForEach-Object {
    $s = "$_"
    if ($s -notmatch 'RemoteException') { Log $s }
}
Set-Content -Path $SessionLog -Value ($out -join "`n")

Wait-Process -Id $cap.Id -ErrorAction SilentlyContinue
Start-Sleep -Seconds 1

if (-not (Test-Path $Pcap)) { throw "Capture file not created (run as Administrator?)" }
$pcapSize = (Get-Item $Pcap).Length
Log "PCAP size: $pcapSize bytes"

Log "--- TLS Client Hello (tcp.payload 0x16 0x03) ---"
& $Tshark -r $Pcap -Y "tcp.port==$Port and tcp.payload[0] == 0x16 and tcp.payload[1] == 0x03" -T fields -e frame.number -e ip.src -e ip.dst 2>&1 | ForEach-Object { Log $_ }
Log "--- TLS Application Data (0x17) sample ---"
& $Tshark -r $Pcap -Y "tcp.port==$Port and tcp.payload[0] == 0x17" -T fields -e frame.number -e ip.src 2>&1 | Select-Object -First 15 | ForEach-Object { Log $_ }
Log "--- cleartext MMS port 102 (must be 0 frames on :102) ---"
$cleartext102 = & $Tshark -r $Pcap -Y "tcp.port==102" -T fields -e frame.number 2>&1
if ($cleartext102) { Log "WARN cleartext :102 frames: $cleartext102" } else { Log "PASS: no cleartext IEC61850 :102 traffic in capture window" }
Log "--- cleartext BER initiate 0x03 on MMS port (must be 0 with TLS) ---"
$cleartextBer = & $Tshark -r $Pcap -Y "tcp.port==$Port and tcp.payload[0] == 0x03" -T fields -e frame.number 2>&1
if ($cleartextBer) { Log "WARN cleartext BER 0x03 frames: $cleartextBer" } else { Log "PASS: no cleartext MMS BER on wire (TLS only)" }
Log "NOTE: Wireshark may label TLS records as MMS heuristically; use payload bytes above."
Log "--- frame list (first 40) ---"
& $Tshark -r $Pcap -T fields -e frame.number -e frame.time_relative -e ip.src -e ip.dst -e tcp.flags -e tls.record.content_type -e _ws.col.Protocol 2>&1 | Select-Object -First 40 | ForEach-Object { Log $_ }

$j = $out -join "`n"
$okTotW = $j -match "READ OK TotW"
$okTotVar = $j -match "READ OK TotVAr"
$okPpv = $j -match "READ OK PPV"
$okTls = $j -match "CONNECT OK \(TLS\)"

if ($exitTls -eq 0 -and $okTls -and $okTotW -and $okTotVar -and $okPpv) {
    Log "P5-M06 VERDICT: PASS (TLS MMS client + capture)"
} else {
    Log "P5-M06 VERDICT: FAIL client exit=$exitTls totW=$okTotW totVar=$okTotVar ppv=$okPpv"
}

Copy-Item $Pcap (Join-Path $Evidence "P5_MMS_TLS.pcapng") -Force
Copy-Item $TxtLog (Join-Path $Evidence "P5_MMS_TLS_ANALYSIS.txt") -Force
Write-Host "`nPCAP: $Pcap" -ForegroundColor Cyan
Write-Host "Analysis: $TxtLog" -ForegroundColor Cyan
exit $exitTls
