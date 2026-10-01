# P5 — cleartext MMS :102 LN browser + Wireshark/tshark evidence (all 31 LNs).
param(
    [string]$DutAddr = "192.168.10.1",
    [int]$Port = 102,
    [string]$Iface = "8",
    [int]$CaptureSeconds = 45
)

$ErrorActionPreference = "Stop"
$prevEap = $ErrorActionPreference
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Evidence = Join-Path $Repo "lab\evidence\phase5"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$Pcap = Join-Path $Evidence "P5_MMS_CLEARTEXT_LN_$Stamp.pcapng"
$TxtLog = Join-Path $Evidence "P5_MMS_CLEARTEXT_LN.txt"
$SessionLog = Join-Path $Evidence "P5_MMS_CLEARTEXT_LN_SESSION.txt"
$Tshark = "C:\Program Files\Wireshark\tshark.exe"
$Browser = Join-Path $Repo "lab\tg544-openwrt\mms_ln_browser"
$LabClient = Join-Path $Repo "lab\tg544-openwrt\mms_lab_client"

New-Item -ItemType Directory -Force -Path $Evidence | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $TxtLog -Value $line
}

Log "=== P5 cleartext MMS LN capture + analysis ==="
Log "DUT=${DutAddr}:${Port} iface=$Iface (LAN1 Ethernet)"
Log "PC=$(Get-NetIPAddress -AddressFamily IPv4 | Where-Object { $_.InterfaceIndex -eq [int]$Iface } | Select-Object -ExpandProperty IPAddress -First 1)"

if (-not (Test-Path $Tshark)) { throw "Install Wireshark (tshark) at $Tshark" }
if (-not (Test-Path $Browser)) {
    throw "Build mms_ln_browser: lab/tg544-openwrt/build-mms-lab-client.sh"
}

$filter = "host $DutAddr and port $Port"
Log "Capture filter: $filter"
Log "Writing: $Pcap"

$pcapW = ($Pcap -replace '\\', '/')
$tsharkArgStr = "-i $Iface -f `"$filter`" -a duration:$CaptureSeconds -w `"$pcapW`""
$cap = Start-Process -FilePath $Tshark -ArgumentList $tsharkArgStr -PassThru -NoNewWindow
Start-Sleep -Seconds 2

Log "Running mms_ln_browser (expect 31 LNs) ..."
$wslBrowser = "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_ln_browser $DutAddr $Port"
$ErrorActionPreference = "Continue"
$browserOut = wsl bash -lc $wslBrowser 2>&1
$browserExit = $LASTEXITCODE
$browserOut | ForEach-Object {
    $s = "$_"
    if ($s -notmatch 'RemoteException') { Log $s }
}

Start-Sleep -Seconds 1
if (Test-Path $LabClient) {
    Log "Running mms_lab_client (TotW/TotVAr/URCB) ..."
    $wslLab = "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_lab_client $DutAddr $Port 8"
    $labOut = wsl bash -lc $wslLab 2>&1
    $labOut | ForEach-Object {
        $s = "$_"
        if ($s -notmatch 'RemoteException') { Log $s }
    }
}

$ErrorActionPreference = $prevEap
Set-Content -Path $SessionLog -Value (($browserOut + $labOut) -join "`n")

Wait-Process -Id $cap.Id -ErrorAction SilentlyContinue
Start-Sleep -Seconds 1

if (-not (Test-Path $Pcap)) { throw "Capture file not created (run as Administrator?)" }
$pcapSize = (Get-Item $Pcap).Length
Log "PCAP size: $pcapSize bytes"

Log "--- io,stat summary ---"
& $Tshark -r $Pcap -q -z io,stat,0 2>&1 | ForEach-Object { Log $_ }

Log "--- MMS InitiateRequest (cleartext TPKT port $Port) ---"
& $Tshark -r $Pcap -Y "mms && mms.confirmedServiceRequest && mms.confirmedServiceRequest == 1" -T fields `
    -e frame.number -e frame.time_relative -e ip.src -e ip.dst -e mms.confirmedServiceRequest 2>&1 |
    Select-Object -First 20 | ForEach-Object { Log $_ }

Log "--- MMS GetNameList (LN discovery) ---"
& $Tshark -r $Pcap -Y "mms && mms.confirmedServiceRequest == 1 && mms.getNameListObjectClass == 2" -T fields `
    -e frame.number -e ip.src -e mms.getNameListObjectClass -e mms.getNameListObjectScope 2>&1 |
    Select-Object -First 30 | ForEach-Object { Log $_ }

Log "--- MMS Read (sample) ---"
& $Tshark -r $Pcap -Y "mms && mms.confirmedServiceRequest == 4" -T fields `
    -e frame.number -e ip.src -e mms.listOfVariable 2>&1 |
    Select-Object -First 25 | ForEach-Object { Log $_ }

Log "--- cleartext BER 0x03 initiate (must be present on :102) ---"
$initiate = & $Tshark -r $Pcap -Y "tcp.port==$Port && tcp.payload[0] == 0x03" -T fields -e frame.number 2>&1
if ($initiate) { Log "PASS initiate frames: $initiate" } else { Log "WARN no 0x03 initiate seen" }

Log "--- TLS on port $Port (must be 0) ---"
$tls = & $Tshark -r $Pcap -Y "tcp.port==$Port && (tcp.payload[0] == 0x16 || tcp.payload[0] == 0x17)" -T fields -e frame.number 2>&1
if ($tls) { Log "WARN TLS-like frames on cleartext port: $tls" } else { Log "PASS no TLS records on :$Port" }

Log "--- frame list (first 50) ---"
& $Tshark -r $Pcap -T fields -e frame.number -e frame.time_relative -e ip.src -e ip.dst `
    -e tcp.len -e _ws.col.Protocol -e mms 2>&1 |
    Select-Object -First 50 | ForEach-Object { Log $_ }

$j = ($browserOut -join "`n")
$okLn = $j -match "ln_count=31" -or $j -match "VERDICT: PASS"
$okConn = $j -match "CONNECT OK"

if ($browserExit -eq 0 -and $okConn -and $okLn) {
    Log "P5 FULL CID LN VERDICT: PASS (31 LNs + capture)"
    $rc = 0
} elseif ($okConn) {
    Log "P5 FULL CID LN VERDICT: PART (connected; LN count check log)"
    $rc = 1
} else {
    Log "P5 FULL CID LN VERDICT: FAIL"
    $rc = 2
}

Copy-Item $Pcap (Join-Path $Evidence "P5_MMS_CLEARTEXT_LN.pcapng") -Force
Copy-Item $TxtLog (Join-Path $Evidence "P5_MMS_CLEARTEXT_LN_ANALYSIS.txt") -Force
Write-Host "`nPCAP: $Pcap" -ForegroundColor Cyan
Write-Host "Analysis: $TxtLog" -ForegroundColor Cyan
exit $rc
