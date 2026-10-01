# P4-03 — TLS 104 test + Wireshark/tshark capture on LAN2.
param(
    [string]$DutAddr = "192.168.1.130",
    [int]$Port = 2404,
    [string]$Iface = "8",
    [int]$CaptureSeconds = 35
)

$ErrorActionPreference = "Stop"
$prevEap = $ErrorActionPreference
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Evidence = Join-Path $Repo "lab\evidence\phase4"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$Pcap = Join-Path $Evidence "P4_03_104_TLS_$Stamp.pcapng"
$TxtLog = Join-Path $Evidence "P4_03_104_TLS.txt"
$SessionLog = Join-Path $Evidence "P4_03_104_TLS_SESSION.txt"
$Tshark = "C:\Program Files\Wireshark\tshark.exe"
$Client = Join-Path $Repo "lab\104\p4_cs104_client"

New-Item -ItemType Directory -Force -Path $Evidence | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $TxtLog -Value $line
}

Log "=== P4-03 TLS capture + test ==="
Log "DUT=${DutAddr}:${Port} iface=$Iface"

if (-not (Test-Path $Tshark)) { throw "Install Wireshark (tshark) at $Tshark" }
if (-not (Test-Path $Client)) { throw "Build client: lab/104/wsl-build-p4-client.sh" }

$filter = "host $DutAddr and port $Port"
Log "Capture filter: $filter"
Log "Writing: $Pcap"

# Start-Process requires one quoted arg string (array form breaks -f vs -w parsing).
$pcapW = ($Pcap -replace '\\', '/')
$tsharkArgStr = "-i $Iface -f `"$filter`" -a duration:$CaptureSeconds -w `"$pcapW`""
$cap = Start-Process -FilePath $Tshark -ArgumentList $tsharkArgStr -PassThru -NoNewWindow
Start-Sleep -Seconds 2

Log "Running TLS client (--tls)..."
$wslClient = "/mnt/c/Yahia/projects/CCLI/lab/104/p4_cs104_client"
$ErrorActionPreference = "Continue"
$out = wsl bash -c "$wslClient $DutAddr $Port 1 1001 --tls" 2>&1
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
Log "--- cleartext 104 start byte 0x68 (must be 0 with TLS) ---"
$cleartext68 = & $Tshark -r $Pcap -Y "tcp.port==$Port and tcp.payload[0] == 0x68" -T fields -e frame.number 2>&1
if ($cleartext68) { Log "WARN cleartext 0x68 frames: $cleartext68" } else { Log "PASS: no cleartext IEC104 0x68 APDU on wire" }
Log "NOTE: Wireshark may label TLS records as IEC60870 heuristically; use payload bytes above."
Log "--- frame list ---"
& $Tshark -r $Pcap -T fields -e frame.number -e frame.time_relative -e ip.src -e ip.dst -e tcp.flags -e tls.record.content_type -e _ws.col.Protocol 2>&1 | Select-Object -First 40 | ForEach-Object { Log $_ }

if ($exitTls -eq 0) {
    Log "P4-03 VERDICT: PASS (TLS client + capture)"
} else {
    Log "P4-03 VERDICT: FAIL client exit=$exitTls"
}

Copy-Item $Pcap (Join-Path $Evidence "P4_03_104_TLS.pcapng") -Force
Copy-Item $TxtLog (Join-Path $Evidence "P4_03_104_TLS_ANALYSIS.txt") -Force
Write-Host "`nPCAP: $Pcap" -ForegroundColor Cyan
Write-Host "Analysis: $TxtLog" -ForegroundColor Cyan
exit $exitTls
