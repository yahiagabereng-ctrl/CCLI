# P5-G02 - GOOSE publish evidence: Modbus + ccli restart + DUT tcpdump + optional GoEna client.
param(
    [string]$DutAddr = "192.168.10.1",
    [string]$ModbusPort = "COM5",
    [int]$ModbusPowerKw = 450,
    [int]$CaptureSec = 25,
    [switch]$SkipModbus,
    [switch]$SkipGoEna,
    [switch]$SkipBuild
)

$ErrorActionPreference = "Continue"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$EvidenceDir = Join-Path $Repo "lab\evidence\phase5"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$LogFile = Join-Path $EvidenceDir "P5_GOOSE_PUBLISH_$Stamp.txt"
$PcapFile = Join-Path $EvidenceDir "P5_GOOSE_PUBLISH_$Stamp.pcap"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$TtyS1Yaml = Join-Path $Repo "apps\ccli\config\lab_tr400_cleartext_tsp_ttyS1.yaml"
$GoEnaClient = Join-Path $Repo "lab\tg544-openwrt\mms_goena_client"
$LabClient = Join-Path $Repo "lab\tg544-openwrt\mms_lab_client"
$GooseCheck = Join-Path $PSScriptRoot "run-p5-goose-check.ps1"

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set `$env:CCLI_TG544_PW then re-run."
}

Log "# P5-G02 GOOSE publish evidence - DUT $DutAddr"
Log 'Wireshark filter (plant / DUT lan3 192.168.30.x): goose && eth.dst == 01:0c:cd:01:00:01'
Log 'IED Explorer: IED View 192.168.10.1:102 TLS OFF - LLN0 GO FC - gcb_PdC_Mis4sec (GoEna set by ccli at boot)'

# --- Push lab yaml with goose.publish_enabled ---
Log "--- Upload lab yaml (ttyS1 + GOOSE publish) ---"
& $Pscp -scp -pw $env:CCLI_TG544_PW -hostkey $HostKey $TtyS1Yaml "root@${DutAddr}:/tmp/ccli.lab.yaml.new" 2>&1 | ForEach-Object { Log $_ }
& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "cp -f /tmp/ccli.lab.yaml.new /etc/ccli/lab.yaml && grep -E 'device|publish_enabled|interface' /etc/ccli/lab.yaml" 2>&1 |
    ForEach-Object { Log $_ }

Log "--- Restart ccli ---"
& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "killall ccli 2>/dev/null; sleep 2; : > /tmp/ccli.log; /usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 & sleep 8; pgrep -a ccli || echo FAIL_no_ccli" 2>&1 |
    ForEach-Object { Log $_ }
& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "grep -E 'GOOSE|listening on|modbus|TotW|GoCB' /tmp/ccli.log | tail -20" 2>&1 |
    ForEach-Object { Log $_ }

$p102 = (Test-NetConnection -ComputerName $DutAddr -Port 102 -WarningAction SilentlyContinue).TcpTestSucceeded
Log "TCP :102 after restart: $(if ($p102) { 'PASS' } else { 'FAIL' })"

# --- Modbus slave on PC COM port ---
if (-not $SkipModbus) {
    Log "--- Modbus RTU slave on $ModbusPort (power-kw $ModbusPowerKw) ---"
    $slaveProc = Get-CimInstance Win32_Process -Filter "Name='python.exe'" -ErrorAction SilentlyContinue |
        Where-Object { $_.CommandLine -match 'modbus_rtu_slave' -and $_.CommandLine -match [regex]::Escape($ModbusPort) }
    if ($slaveProc) {
        Log "Modbus slave already running PID $($slaveProc.ProcessId)"
    } else {
        Log "Starting: python lab\modbus_rtu_slave.py --port $ModbusPort --power-kw $ModbusPowerKw --trace"
        Start-Process -FilePath "python" -ArgumentList "-u", "lab\modbus_rtu_slave.py", "--port", $ModbusPort, "--power-kw", "$ModbusPowerKw", "--trace" `
            -WorkingDirectory $Repo -WindowStyle Minimized
        Start-Sleep -Seconds 2
    }
}

# --- Build MMS clients ---
if (-not $SkipBuild) {
    Log "--- Build mms_goena_client + mms_lab_client (WSL) ---"
    wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/build-mms-lab-client.sh" 2>&1 | ForEach-Object { Log $_ }
}

# --- lan3 carrier (TesproOS plant port; br-lan is empty on TR544) ---
Log "--- DUT lan3 carrier check ---"
& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "ip link show lan3 2>/dev/null | head -1; grep 'interface:' /etc/ccli/lab.yaml" 2>&1 | ForEach-Object { Log $_ }
Log "GOOSE publish must use lan3 (not empty br-lan). PC plant NIC: 192.168.30.10/24 optional."

# --- DUT tcpdump on lan3 (GOOSE egress) ---
Log "--- DUT tcpdump lan3 ${CaptureSec}s (GOOSE ethertype 0x88b8) ---"
$remotePcap = "/tmp/goose_cap.pcap"
& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "killall tcpdump 2>/dev/null; TX0=\$(cat /sys/class/net/lan3/statistics/tx_packets 2>/dev/null); timeout $CaptureSec tcpdump -i lan3 -s 0 -w $remotePcap 'ether proto 0x88b8' 2>/tmp/tcpdump_goose.log & sleep 2; echo tcpdump_started TX0=\$TX0" 2>&1 |
    ForEach-Object { Log $_ }

Start-Sleep -Seconds 3

if (-not $SkipGoEna -and (Test-Path $GoEnaClient)) {
    Log "--- mms_goena_client (enable GoCB publish) ---"
    wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_goena_client $DutAddr 102" 2>&1 | ForEach-Object { Log $_ }
} else {
    Log 'GoEna client skipped - enable GoEna in IED Explorer during capture window'
}

if (Test-Path $LabClient) {
    Log "--- mms_lab_client browse + 4s report (12s) ---"
    wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_lab_client $DutAddr 102 12" 2>&1 | ForEach-Object { Log $_ }
}

Log "Waiting for tcpdump to finish ..."
Start-Sleep -Seconds ($CaptureSec + 3)

& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "tcpdump -r $remotePcap -nn -c 20 2>/dev/null; wc -c $remotePcap 2>/dev/null; cat /tmp/tcpdump_goose.log 2>/dev/null | tail -5" 2>&1 |
    ForEach-Object { Log $_ }

if (Test-Path $Pscp) {
    & $Pscp -scp -pw $env:CCLI_TG544_PW -hostkey $HostKey "root@${DutAddr}:$remotePcap" $PcapFile 2>&1 | ForEach-Object { Log $_ }
    if (Test-Path $PcapFile) {
        Log "PCAP saved: $PcapFile"
        $tshark = "C:\Program Files\Wireshark\tshark.exe"
        if (Test-Path $tshark) {
            Log "--- tshark GOOSE decode ---"
            & $tshark -r $PcapFile -Y "goose" -T fields -e frame.number -e eth.dst -e goose.appid -e goose.stnum -e goose.timeallowedtolive 2>&1 |
                ForEach-Object { Log $_ }
        }
    }
}

& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "grep -E 'GoCB|GOOSE|goose' /tmp/ccli.log | tail -15" 2>&1 | ForEach-Object { Log $_ }

Log ""
Log "=== IED Explorer / wire checklist ==="
Log "1. IED View 192.168.10.1:102 TLS OFF (not offline IEC View)"
Log '2. CCI016_01LD_Plant / LLN0 / GO FC / gcb_PdC_Mis4sec'
Log "3. GoEna=1 confirmed via mms_goena_client (GetGoCBValues)"
Log '4. lan3 UP + goose.interface lan3 — Wireshark on plant NIC: goose && eth.dst == 01:0c:cd:01:00:01'
Log ""
Log "Evidence log: $LogFile"
Write-Host "`nDone. See $LogFile"
