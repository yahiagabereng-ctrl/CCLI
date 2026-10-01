# P5 full circle - cleartext MMS :102 for Test Suite Pro (LAB BYPASS).
# Modbus COM5 -> DUT -> MMS read + Wlim Operate -> DIO evidence.
param(
    [string]$DutAddr = "192.168.10.1",
    [int]$MmsPort = 102,
    [string]$DeployHost = "192.168.10.1",
    [string]$ExpectedPcIp = "192.168.10.10",
    [int]$ModbusPowerKw = 450,
    [switch]$SkipDeploy,
    [switch]$SkipBuild,
    [switch]$SkipModbusStart
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$DeployScript = Join-Path $PSScriptRoot "deploy-ccli-session-fix.ps1"
$LabYaml = Join-Path $Repo "apps\ccli\config\lab_tr400_cleartext_tsp.yaml"
$EvidenceDir = Join-Path $Repo "lab\evidence\phase5"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$LogFile = Join-Path $EvidenceDir "P5_FULLCIRCLE_CLEARTEXT_$Stamp.txt"
$Canonical = Join-Path $EvidenceDir "P5_FULLCIRCLE_CLEARTEXT.txt"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$LabClient = Join-Path $Repo "lab\tg544-openwrt\mms_lab_client"
$WlimClient = Join-Path $Repo "lab\tg544-openwrt\mms_wlim_client"
$VArSdClient = Join-Path $Repo "lab\tg544-openwrt\mms_varsd_client"

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

Log "# P5 full circle - cleartext MMS :102 (TSP lab bypass)"
Log "NOTE: Product TLS path remains :3782 - see P5_MMS_TLS.pcapng"
Log "DUT target: ${DutAddr}:${MmsPort} tls=off"

$pcIps = @(Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
    Where-Object { $_.IPAddress -like "192.168.*" } |
    ForEach-Object { $_.IPAddress })
Log "PC IPv4: $($pcIps -join ', ')"
if ($pcIps -notcontains $ExpectedPcIp) {
    Log "WARN: PC not on ${ExpectedPcIp}/24 - run Admin: lab\set_pc_lan1_dso.cmd"
}

$reachable = (Test-NetConnection -ComputerName $DutAddr -WarningAction SilentlyContinue -ErrorAction SilentlyContinue).PingSucceeded
Log "Ping ${DutAddr}: $reachable"
if (-not $reachable) {
    Log "FAIL: DUT unreachable on LAN1"
    Copy-Item $LogFile $Canonical -Force
    exit 2
}

if (-not $SkipDeploy) {
    if (-not $env:CCLI_TG544_PW) { Write-Error "Set env:CCLI_TG544_PW" }
    Log "Deploying cleartext lab yaml via LAN1 ..."
    $prev = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    & $DeployScript -HostAddr $DeployHost -LabYaml $LabYaml -Changes @(
        "P5 full circle cleartext TSP",
        "mms tcp_port 102 tls_enabled false",
        "permissive_bypass true for actuation"
    ) 2>&1 | ForEach-Object { Log $_ }
    $ErrorActionPreference = $prev
    Start-Sleep -Seconds 3
}

if (-not $SkipModbusStart) {
    Log "Modbus slave: ensure ONE process on COM5 --power-kw $ModbusPowerKw --trace"
    $comBusy = Get-CimInstance Win32_SerialPort -ErrorAction SilentlyContinue |
        Where-Object { $_.DeviceID -eq "COM5" }
    if ($comBusy) { Log "COM5 present: $($comBusy.Name)" }
    Log "If not running: python -u lab\modbus_rtu_slave.py --port COM5 --power-kw $ModbusPowerKw --trace"
}

if (-not $SkipBuild) {
    Log "Building cleartext MMS lab clients ..."
    $prevB = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    wsl bash -lc "sed -i 's/\r$//' /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/build-mms-lab-client.sh; cd /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt; bash build-mms-lab-client.sh" 2>&1 | ForEach-Object { Log $_ }
    $ErrorActionPreference = $prevB
}

if ($env:CCLI_TG544_PW) {
    Log "--- DUT listen + modbus (plink) ---"
    $prevP = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $plinkCmd = '/usr/sbin/ccli --version; ss -tlnp 2>/dev/null | grep 102; grep listening /tmp/ccli.log | tail -3; grep modbus /tmp/ccli.log | tail -5'
    $dutOut = & $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $plinkCmd 2>&1
    $ErrorActionPreference = $prevP
    $dutOut | ForEach-Object { Log $_ }
    $listen102 = ($dutOut -join "`n") -match ':102'
    if (-not $listen102) {
        Log "FAIL: DUT not listening on :102 - check deploy and LuCI 61850 conflict"
    } else {
        Log "PASS: DUT :102 listen seen"
    }
}

$portTest = Test-NetConnection -ComputerName $DutAddr -Port $MmsPort -WarningAction SilentlyContinue -ErrorAction SilentlyContinue
Log "Test-NetConnection ${DutAddr}:${MmsPort} TcpTestSucceeded=$($portTest.TcpTestSucceeded)"

if (-not (Test-Path $LabClient)) {
    throw "Missing $LabClient - run build-mms-lab-client.sh"
}

Log "--- mms_lab_client browse + 4s report (16s) ---"
$prev2 = $ErrorActionPreference
$ErrorActionPreference = "Continue"
$labOut = wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_lab_client $DutAddr $MmsPort 16" 2>&1
$labExit = $LASTEXITCODE
$ErrorActionPreference = $prev2
$labOut | ForEach-Object { Log $_ }

if (Test-Path $WlimClient) {
    Log "--- mms_wlim_client Wlim Operate 10pct ---"
    $prev3 = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $wlimOut = wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_wlim_client $DutAddr $MmsPort 10" 2>&1
    $wlimExit = $LASTEXITCODE
    $ErrorActionPreference = $prev3
    $wlimOut | ForEach-Object { Log $_ }
} else {
    $wlimExit = 1
    Log "SKIP wlim client - not built"
}

$varsdExit = 1
$varsdOut = ""
if (Test-Path $VArSdClient) {
    Log "--- mms_varsd_client VArSd Operate 10pct (P5-R01) ---"
    $prevV = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $varsdOut = wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_varsd_client $DutAddr $MmsPort 10" 2>&1
    $varsdExit = $LASTEXITCODE
    $ErrorActionPreference = $prevV
    $varsdOut | ForEach-Object { Log $_ }
} else {
    Log "SKIP varsd client - not built"
}

if ($env:CCLI_TG544_PW) {
    Log "--- DUT actuation excerpt ---"
    $prev4 = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $actCmd = "grep Wlim /tmp/ccli.log | tail -6; grep VArSd /tmp/ccli.log | tail -6; grep 'plant Q' /tmp/ccli.log | tail -4; grep FC16 /tmp/ccli.log | tail -4; grep curtail /tmp/ccli.log | tail -5; ubus call dido_v2 status 2>/dev/null | head -20"
    & $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $actCmd 2>&1 |
        ForEach-Object { Log $_ }
    $ErrorActionPreference = $prev4
}

Log ""
Log "=== Test Suite Pro (manual) ==="
Log "  IP: $DutAddr  Port: $MmsPort  TLS: OFF"
Log "  CID: apps\ccli\config\icd\lab_tg544_eth_a.cid"
Log "  Matrix: lab\evidence\phase5\P5_LN_VALUE_MATRIX.md"
Log "  LN+Wireshark: lab\tg544-openwrt\run-p5-cleartext-ln-capture.ps1"
Log "  Sequencer: lab\evidence\testsuite-pro\inbox\TSP_P5_FULLCIRCLE_SEQUENCER.md"
Log "  Save log: lab\evidence\testsuite-pro\inbox\TSP_P5_FULLCIRCLE_CLEARTEXT_$Stamp.txt"
Log "  Guide: lab\evidence\phase5\P5_TSP_CLEARTEXT_README.md"

$okLab = $labExit -eq 0 -and ($labOut -join "`n") -match "PASS"
$okWlim = $wlimExit -eq 0 -and ($wlimOut -join "`n") -match "OPERATE OK"
$okVArSd = $varsdExit -eq 0 -and ($varsdOut -join "`n") -match "OPERATE OK"
$okPort = $portTest.TcpTestSucceeded

if ($okPort -and $okLab) {
    Log "P5 FULL CIRCLE (cleartext client): PASS"
    if ($okWlim) { Log "P3-04 Wlim actuation (cleartext): PASS" }
    if ($okVArSd) { Log "P5-R01 VArSd actuation (cleartext): PASS" }
    $rc = 0
} else {
    Log "P5 FULL CIRCLE: FAIL port=$okPort lab=$okLab wlim=$okWlim varsd=$okVArSd"
    $rc = 1
}

Copy-Item $LogFile $Canonical -Force
Write-Host "`nEvidence: $Canonical" -ForegroundColor Cyan
exit $rc
