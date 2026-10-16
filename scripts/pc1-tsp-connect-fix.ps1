# PC1 one-shot: LAN1 check, DUT preflight, TLS verify, firewall for Test Suite Pro, optional capture.
# Run PowerShell as Administrator on Masomeh PC.
param(
    [string]$TlsDir = "C:\CCLI_TSP_FIELD_PACK\tls",
    [string]$DutHost = "192.168.10.1",
    [int]$Port = 3782,
    [string]$PcIp = "192.168.10.10",
    [switch]$CaptureOnly
)

$ErrorActionPreference = "Stop"
$Repo = Resolve-Path (Join-Path $PSScriptRoot "..")
$TspExe = "C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\Bin\TestSuite.exe"

function Step($msg) { Write-Host "`n=== $msg ===" -ForegroundColor Cyan }

Step "LAN1 NIC"
$eth = Get-NetIPAddress -AddressFamily IPv4 -InterfaceAlias Ethernet -ErrorAction SilentlyContinue |
    Where-Object { $_.IPAddress -eq $PcIp }
if (-not $eth) {
    Write-Host "FAIL: Ethernet is not $PcIp/24. Run as Admin: lab\set_pc_lan1_dso.cmd" -ForegroundColor Red
    exit 1
}
if ($eth.PrefixLength -ne 24) {
    Write-Host "WARN: prefix is /$($eth.PrefixLength) - use 255.255.255.0 for LAN1" -ForegroundColor Yellow
}
$tcp = Test-NetConnection -ComputerName $DutHost -Port $Port -WarningAction SilentlyContinue
if (-not $tcp.TcpTestSucceeded) {
    Write-Host "FAIL: TCP ${DutHost}:${Port} from $PcIp" -ForegroundColor Red
    exit 1
}
Write-Host "PASS: $PcIp -> ${DutHost}:${Port} TCP OK"

if (-not $CaptureOnly) {
    Step "DUT preflight (single ccli on 3782)"
    if (-not $env:CCLI_TG544_PW) {
        Write-Host "WARN: CCLI_TG544_PW not set - skip SSH preflight (Yahia must run dut-mms-tsp-preflight.ps1)" -ForegroundColor Yellow
    } else {
        & (Join-Path $Repo "scripts\dut-mms-tsp-preflight.ps1") -HostAddr $DutHost
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }

    Step "TLS key match + TLS 1.2 probe"
    & (Join-Path $Repo "scripts\verify-tsp-tls-key-match.ps1") -TlsDir $TlsDir
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & (Join-Path $Repo "scripts\pc1-test-mms-tls12.ps1") -TlsDir $TlsDir -DutHost $DutHost -Port $Port
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    Step "Windows Firewall (Test Suite Pro -> DUT :3782)"
    if (Test-Path $TspExe) {
        $ruleName = "CCLI TSP MMS TLS 3782"
        netsh advfirewall firewall delete rule name="$ruleName" 2>$null | Out-Null
        netsh advfirewall firewall add rule name="$ruleName" dir=out action=allow `
            program="$TspExe" protocol=TCP remoteip=$DutHost remoteport=$Port | Out-Null
        Write-Host "PASS: outbound allow rule for TestSuite.exe -> ${DutHost}:${Port}"
    } else {
        Write-Host "WARN: TestSuite.exe not found at $TspExe" -ForegroundColor Yellow
    }
}

Step "TSP UI checklist (P3-07 Connect — see handoff/masomeh95/PC1_ENCRYPTED_ALERT_P3_07.md)"
@(
    "1. Close and reopen Test Suite Pro AS ADMINISTRATOR"
    "2. System Status: Ethernet $PcIp must be GREEN (not red / no adapters)"
    "3. Import CID: C:\CCLI_TSP_FIELD_PACK\cid\lab_tg544_eth_a.cid (NOT product 5-LN for Connect)"
    "4. IED Connection: $DutHost port $Port TLS ON local IP $PcIp IED CCI016_01 AP accessPoint1"
    "5. TLS paths ONLY from $TlsDir (re-browse every file):"
    "   CA root_CA.pem | Transport client_tls.pem + client_tsp.key"
    "   MMS client.pem + client_tsp.key (SAME key file — NOT client_acse.key) | CRL/revoked EMPTY"
    "6. Chaining depth 1 both tabs | AP Title 1,1,1,999,1 | Allow TLS v1.3 OFF"
    "7. Output filter: MMS; APP; SSL"
) | ForEach-Object { Write-Host $_ }

Step "Synchronized capture (run this, then Connect within 45s)"
$pcapDir = Join-Path $Repo "lab\evidence\testsuite-pro\pcap"
New-Item -ItemType Directory -Force -Path $pcapDir | Out-Null
$pcap = Join-Path $pcapDir "TSP_CONNECT_$(Get-Date -Format yyyy-MM-dd_HHmmss).pcapng"
Write-Host @"

Write-Host '>>> Click Connect in TSP NOW <<<' -ForegroundColor Yellow
& `"C:\Program Files\Wireshark\tshark.exe`" -i 7 -f `"host $DutHost and port $Port`" -a duration:45 -w `"$pcap`"
`$n = (& `"C:\Program Files\Wireshark\tshark.exe`" -r `"$pcap`" -T fields -e frame.number 2>`$null | Measure-Object).Count
Write-Host `"Frames captured: `$n -> $pcap`"
if (`$n -eq 0) { Write-Host 'No TCP 3782 = TSP did not connect (fix System Status / IED Connection above)' -ForegroundColor Red }

"@
