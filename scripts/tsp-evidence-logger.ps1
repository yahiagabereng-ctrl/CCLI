# Per-test evidence: Wireshark capture + inbox log stub for Test Suite Pro Phase 1.
# Run on PC-B (LAN1) before each matrix step; stop after TSP action and paste Output.
#
#   powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate 2026-10-09
#   powershell -File scripts\tsp-evidence-logger.ps1 -Action Start -TestId T1-07
#   ... run test in TSP ...
#   powershell -File scripts\tsp-evidence-logger.ps1 -Action Stop -PasteClipboard
#
param(
    [ValidateSet("Init", "Start", "Stop", "Status")]
    [string]$Action = "Start",
    [ValidateSet("T1-01", "T1-02", "T1-03", "T1-04", "T1-05", "T1-06", "T1-07", "T1-08", "T1-09", "T1-10")]
    [string]$TestId = "",
    [string]$SessionDate = "",  # YYYY-MM-DD; default today
    [string]$Operator = "",
    [string]$CcliPkg = "0.1.0-r52",
    [string]$TspVersion = "",
    [string]$PcIp = "192.168.10.10",
    [string]$DutIp = "192.168.10.1",
    [int]$MmsPort = 3782,
    [string]$InterfaceAlias = "Ethernet",
    [switch]$PasteClipboard,
    [switch]$NoCapture,
    [switch]$NoSsh,
    [switch]$NoWireSummary,
    [string]$SshUser = "root",
    [string]$SshHost = ""  # default: same as DutIp
)

$ErrorActionPreference = "Stop"
$Repo = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Inbox = Join-Path $Repo "lab\evidence\testsuite-pro\inbox"
$PcapDir = Join-Path $Repo "lab\evidence\testsuite-pro\pcap"
$StatePath = Join-Path $Inbox ".tsp_evidence_active.json"
$IndexName = "TSP_PHASE1_SESSION_INDEX_{0}.md"

$tshark = "C:\Program Files\Wireshark\tshark.exe"

$TestCatalog = [ordered]@{
    "T1-01" = @{ Gate = "P3_01"; Tool = "CONNECT"; Base = "TSP_P3_01_CONNECT"; Hint = "Connect CCI016_01 TLS; save System Status screenshot." }
    "T1-02" = @{ Gate = "P3_COMPARE"; Tool = "COMPARE"; Base = "TSP_P3_COMPARE"; Hint = "Export Compare xlsx + scope note (M objects waiver OK)." }
    "T1-03" = @{ Gate = "P3_03"; Tool = "BROWSE"; Base = "TSP_P3_03_BROWSE"; Hint = "Browse LD_Plant / ~31 LNs; paste sequencer or Output." }
    "T1-04" = @{ Gate = "P3_03"; Tool = "READ"; Base = "TSP_P3_03_READ_PdC"; Hint = "Read PdCMMXU1 TotW, TotVAr, PPV." }
    "T1-05" = @{ Gate = "P3_03"; Tool = "REPORT"; Base = "TSP_P3_03_REPORT_URCB"; Hint = "Enable urcb_PdC_Mis4sec01 IntgPd=4000; note dataset DS_R_PdC_Mis4sec." }
    "T1-06" = @{ Gate = "P3_03"; Tool = "REPORT"; Base = "TSP_P3_03_REPORT_TotW"; Hint = "Report Viewer: SqNum, Integrity ~4s, dataRef TotW/PPV." }
    "T1-07" = @{ Gate = "P3_04"; Tool = "OPERATE"; Base = "TSP_P3_04_OPERATE_Wlim"; Hint = "Operate Mod (on) + WMaxSptPct=10 or 70; no Write Mod.stVal." }
    "T1-08" = @{ Gate = "P3_04"; Tool = "OPERATE"; Base = "TSP_P3_04_OPERATE_WSd"; Hint = "Operate Mod + WSptPct=20." }
    "T1-09" = @{ Gate = "P3_05"; Tool = "FALLBACK"; Base = "TSP_P3_05_FALLBACK"; Hint = "Release association; wait 15 s; note fallback behaviour." }
    "T1-10" = @{ Gate = "P3_01"; Tool = "RECONNECT"; Base = "TSP_P3_01_RECONNECT"; Hint = "Disconnect then Connect again." }
}

function Get-SessionDate {
    if ($SessionDate) { return $SessionDate }
    return (Get-Date -Format "yyyy-MM-dd")
}

function Get-EvidenceTxtPath {
    param([string]$Base, [string]$Date)
    Join-Path $Inbox "${Base}_${Date}.txt"
}

function Get-EvidencePcapPath {
    param([string]$Base, [string]$Date)
    Join-Path $PcapDir "${Base}_${Date}.pcapng"
}

function Get-EvidenceDutLogPath {
    param([string]$Base, [string]$Date)
    Join-Path $Inbox "${Base}_${Date}_DUT_LOG.txt"
}

function Get-EvidenceWirePath {
    param([string]$Base, [string]$Date)
    Join-Path $Inbox "${Base}_${Date}_WIRE.txt"
}

function Ensure-LabEnv {
    if ($env:CCLI_TG544_PW) { return }
    $labEnv = Join-Path $Repo "lab\tg544-openwrt\lab-env.ps1"
    if (Test-Path $labEnv) {
        . $labEnv
    }
}

function Get-DutHostKey {
    param([string]$HostAddr)
    if ($HostAddr -eq "192.168.10.1") {
        return "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
    }
    return "SHA256:2hiPouwqC1oxP//Q0BpvgIcqD6IG+pqLkzNih1EpNRE"
}

function Invoke-DutEvidencePull {
    param(
        [string]$TestId,
        [string]$Phase,
        [string]$OutFile,
        [string]$HostAddr
    )
    $Plink = "C:\Program Files\PuTTY\plink.exe"
    if (-not (Test-Path $Plink)) {
        Write-Warning "plink not found; skip DUT SSH ($Phase)."
        return $false
    }
    Ensure-LabEnv
    if (-not $env:CCLI_TG544_PW) {
        Write-Warning "Set `$env:CCLI_TG544_PW or source lab-env.ps1; skip DUT SSH ($Phase)."
        return $false
    }
    $remoteSh = Join-Path $Repo "lab\evidence\testsuite-pro\pull-dut-tsp-test-remote.sh"
    if (-not (Test-Path $remoteSh)) {
        Write-Warning "Missing pull-dut-tsp-test-remote.sh"
        return $false
    }
    $Pscp = "C:\Program Files\PuTTY\pscp.exe"
    $hostKey = Get-DutHostKey -HostAddr $HostAddr
    $remoteDest = "/tmp/pull-dut-tsp-test-remote.sh"
    if (Test-Path $Pscp) {
        & $Pscp -batch -scp -pw $env:CCLI_TG544_PW -hostkey $hostKey $remoteSh "${SshUser}@${HostAddr}:${remoteDest}" 2>&1 | Out-Null
        $remoteCmd = "sed -i 's/\r$//' $remoteDest; chmod +x $remoteDest; sh $remoteDest $TestId $Phase"
    }
    else {
        Write-Warning "pscp not found; using inline remote (reduced)."
        $remoteCmd = "date -Iseconds; pgrep -a ccli; ubus call dido_v2 status 2>/dev/null; logread -e ccli 2>/dev/null | tail -40; tail -40 /tmp/ccli.log 2>/dev/null"
    }
    $block = @(
        "",
        "========== DUT SSH $Phase $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss') UTC-ish local =========="
    ) -join "`r`n"
    try {
        $out = & $Plink -batch -ssh "${SshUser}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $hostKey $remoteCmd 2>&1
        Add-Content -Path $OutFile -Value ($block + "`r`n" + ($out | Out-String)) -Encoding UTF8
        return $true
    }
    catch {
        Add-Content -Path $OutFile -Value ($block + "`r`nSSH_ERROR: $_") -Encoding UTF8
        Write-Warning "DUT SSH failed ($Phase): $_"
        return $false
    }
}

function Write-WiresharkSummary {
    param(
        [string]$PcapPath,
        [string]$WirePath,
        [string]$DutIp,
        [int]$MmsPort
    )
    if (-not (Test-Path $tshark)) { return $false }
    if (-not (Test-Path $PcapPath)) { return $false }
    $size = (Get-Item $PcapPath).Length
    $frames = @(& $tshark -r $PcapPath -T fields -e frame.number 2>$null)
    $count = if ($frames) { $frames.Count } else { 0 }
    $tls = @(& $tshark -r $PcapPath -Y "tls" -T fields -e frame.number 2>$null).Count
    $mmsHint = @(& $tshark -r $PcapPath -Y "mms" -T fields -e frame.number 2>$null).Count
    $lines = @(
        "# Wireshark summary (auto)",
        "pcap: $PcapPath",
        "pcap_bytes: $size",
        "frame_count: $count",
        "tls_frames: $tls",
        "mms_dissector_frames: $mmsHint",
        "filter_hint: host $DutIp and port $MmsPort",
        "decrypt: lab/evidence/testsuite-pro/pcap/README.md",
        "",
        "--- tshark -q -z io,stat,0 ---"
    )
    $stat = & $tshark -r $PcapPath -q -z io,stat,0 2>&1 | Out-String
    $lines += $stat.TrimEnd()
    Set-Content -Path $WirePath -Value ($lines -join "`r`n") -Encoding UTF8
    return $true
}

function New-MetadataHeader {
    param(
        [hashtable]$Meta,
        [string]$Hint,
        [string]$Result = "PENDING"
    )
    @(
        "# CCLI TSP evidence",
        "tsp_id: $($Meta.TestId)",
        "tsp_version: $($Meta.TspVersion)",
        "pc_ip: $($Meta.PcIp)",
        "dut_ip: $($Meta.DutIp)",
        "dut_port: $($Meta.MmsPort)",
        "tls: true",
        "cid: apps/ccli/config/icd/lab_tg544_eth_a.cid",
        "ccli_pkg: $($Meta.CcliPkg)",
        "gate: $($Meta.Gate)",
        "tool: $($Meta.Tool)",
        "operator: $($Meta.Operator)",
        "date: $($Meta.Date)",
        "result: $Result",
        "hint: $Hint",
        "",
        "--- TSP Output / notes (paste below) ---",
        ""
    ) -join "`r`n"
}

function Ensure-Dirs {
    New-Item -ItemType Directory -Force -Path $Inbox | Out-Null
    New-Item -ItemType Directory -Force -Path $PcapDir | Out-Null
}

function Write-SessionIndex {
    param([string]$Date)
    $indexPath = Join-Path $Inbox ($IndexName -f $Date)
    if (Test-Path $indexPath) { return }
    $lines = @(
        ("# Phase 1 evidence index - " + $Date),
        "",
        "Protocol: [EVIDENCE_PER_TEST_PROTOCOL.md](../EVIDENCE_PER_TEST_PROTOCOL.md)",
        "",
        "| TSP ID | TSP log | DUT SSH | Wire summary | Pcap | Result |",
        "|--------|---------|---------|--------------|------|--------|"
    )
    foreach ($id in $TestCatalog.Keys) {
        $c = $TestCatalog[$id]
        $base = $c.Base
        $lines += "| $id | ``${base}_${Date}.txt`` | ``${base}_${Date}_DUT_LOG.txt`` | ``${base}_${Date}_WIRE.txt`` | ``${base}_${Date}.pcapng`` | PENDING |"
    }
    $lines += @(
        "",
        "Before each test: ``powershell -File scripts\tsp-evidence-logger.ps1 -Action Start -TestId T1-xx``",
        "After each test: ``-Action Stop -PasteClipboard`` (copy TSP Output first).",
        ""
    )
    Set-Content -Path $indexPath -Value ($lines -join "`n") -Encoding UTF8
    Write-Host "Created index: $indexPath"
}

function Action-Init {
    param([string]$Date)
    Ensure-Dirs
    Write-SessionIndex -Date $Date
    foreach ($id in $TestCatalog.Keys) {
        $c = $TestCatalog[$id]
        $txt = Get-EvidenceTxtPath -Base $c.Base -Date $Date
        if (Test-Path $txt) { continue }
        $meta = @{
            TestId     = $id
            TspVersion = $TspVersion
            PcIp       = $PcIp
            DutIp      = $DutIp
            MmsPort    = $MmsPort
            CcliPkg    = $CcliPkg
            Gate       = $c.Gate
            Tool       = $c.Tool
            Operator   = $Operator
            Date       = $Date
        }
        New-MetadataHeader -Meta $meta -Hint $c.Hint | Set-Content -Path $txt -Encoding UTF8
        Write-Host "  stub: $(Split-Path $txt -Leaf)"
    }
}

function Read-ActiveState {
    if (-not (Test-Path $StatePath)) { return $null }
    Get-Content $StatePath -Raw | ConvertFrom-Json
}

function Write-ActiveState {
    param($Obj)
    ($Obj | ConvertTo-Json -Depth 5) | Set-Content -Path $StatePath -Encoding UTF8
}

function Action-Start {
    if (-not $TestId) { throw "Start requires -TestId T1-xx" }
    $Date = Get-SessionDate
    Ensure-Dirs
    Action-Init -Date $Date | Out-Null

    $existing = Read-ActiveState
    if ($existing) {
        throw "Evidence session active for $($existing.test_id). Run -Action Stop first."
    }

    $sshTarget = if ($SshHost) { $SshHost } else { $DutIp }
    $c = $TestCatalog[$TestId]
    $txt = Get-EvidenceTxtPath -Base $c.Base -Date $Date
    $pcap = Get-EvidencePcapPath -Base $c.Base -Date $Date
    $dutLog = Get-EvidenceDutLogPath -Base $c.Base -Date $Date
    $wireTxt = Get-EvidenceWirePath -Base $c.Base -Date $Date
    $meta = @{
        TestId     = $TestId
        TspVersion = $TspVersion
        PcIp       = $PcIp
        DutIp      = $DutIp
        MmsPort    = $MmsPort
        CcliPkg    = $CcliPkg
        Gate       = $c.Gate
        Tool       = $c.Tool
        Operator   = $Operator
        Date       = $Date
    }

    $header = New-MetadataHeader -Meta $meta -Hint $c.Hint -Result "IN_PROGRESS"
    $started = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    $header += "`r`ncapture_started_utc: $started`r`npcap: $pcap`r`ndut_log: $dutLog`r`nwire_summary: $wireTxt`r`n`r`n--- TSP Output / notes (paste below) ---`r`n`r`n"
    Set-Content -Path $txt -Encoding UTF8 -Value $header

    if (-not $NoSsh) {
        $dutHeader = @(
            "# CCLI DUT SSH evidence",
            "tsp_id: $TestId",
            "dut_ip: $sshTarget",
            "date: $Date",
            ""
        ) -join "`r`n"
        Set-Content -Path $dutLog -Encoding UTF8 -Value $dutHeader
        Invoke-DutEvidencePull -TestId $TestId -Phase "PRE" -OutFile $dutLog -HostAddr $sshTarget | Out-Null
    }

    $capturePid = $null
    if (-not $NoCapture) {
        if (-not (Test-Path $tshark)) {
            Write-Warning "Wireshark not found; continuing with log stub only (-NoCapture)."
        }
        else {
            $filter = "host $DutIp and port $MmsPort"
            $proc = Start-Process -FilePath $tshark -ArgumentList @(
                "-i", $InterfaceAlias,
                "-f", $filter,
                "-w", $pcap
            ) -PassThru -WindowStyle Hidden
            $capturePid = $proc.Id
        }
    }

    $state = [ordered]@{
        test_id      = $TestId
        date         = $Date
        txt_path     = $txt
        pcap_path    = $pcap
        dut_log_path = $dutLog
        wire_path    = $wireTxt
        ssh_host     = $sshTarget
        capture_pid  = $capturePid
        started_utc  = $started
    }
    Write-ActiveState $state

    Write-Host ""
    Write-Host "=== Evidence logger START - $TestId ===" -ForegroundColor Cyan
    Write-Host "Log  : $txt"
    if (-not $NoSsh) { Write-Host "DUT  : $dutLog (PRE pulled)" }
    if ($capturePid) { Write-Host "Pcap : $pcap (tshark PID $capturePid)" }
    Write-Host "Hint : $($c.Hint)"
    Write-Host ""
    Write-Host "Run the TSP step now. Then copy Output and run:"
    Write-Host "  powershell -File scripts\tsp-evidence-logger.ps1 -Action Stop -PasteClipboard"
    Write-Host ""
}

function Stop-CaptureProcess {
    param([int]$ProcessId)
    if (-not $ProcessId) { return }
    $p = Get-Process -Id $ProcessId -ErrorAction SilentlyContinue
    if ($p) {
        Stop-Process -Id $ProcessId -Force -ErrorAction SilentlyContinue
        Start-Sleep -Milliseconds 400
    }
}

function Action-Stop {
    $state = Read-ActiveState
    if (-not $state) {
        Write-Warning "No active capture. Use -Action Start -TestId T1-xx first."
        return
    }

    Stop-CaptureProcess -ProcessId ([int]$state.capture_pid)

    $txt = $state.txt_path
    $ended = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    $append = "`r`n--- capture_stopped_utc: $ended ---`r`n"

    if ($PasteClipboard) {
        try {
            $clip = Get-Clipboard -Raw -ErrorAction Stop
            if ($clip -and $clip.Trim()) {
                $append += "`r`n--- pasted from clipboard ---`r`n$clip`r`n"
            }
        }
        catch {
            Write-Warning "Could not read clipboard: $_"
        }
    }

    Add-Content -Path $txt -Value $append -Encoding UTF8

    if (-not $NoSsh -and $state.dut_log_path) {
        $hostAddr = if ($state.ssh_host) { $state.ssh_host } else { $DutIp }
        Invoke-DutEvidencePull -TestId $state.test_id -Phase "POST" -OutFile $state.dut_log_path -HostAddr $hostAddr | Out-Null
        Add-Content -Path $txt -Value "dut_log: $($state.dut_log_path)" -Encoding UTF8
    }

    if ($state.pcap_path -and (Test-Path $state.pcap_path)) {
        $size = (Get-Item $state.pcap_path).Length
        Add-Content -Path $txt -Value "pcap_bytes: $size" -Encoding UTF8
        if ($size -lt 128) {
            Write-Warning "Pcap very small ($size bytes) - was TSP traffic on LAN1 during capture?"
        }
        if (-not $NoWireSummary -and $state.wire_path) {
            if (Write-WiresharkSummary -PcapPath $state.pcap_path -WirePath $state.wire_path -DutIp $DutIp -MmsPort $MmsPort) {
                Add-Content -Path $txt -Value "wire_summary: $($state.wire_path)" -Encoding UTF8
            }
        }
    }

    Remove-Item -Force $StatePath -ErrorAction SilentlyContinue

    Write-Host ""
    Write-Host "=== Evidence logger STOP - $($state.test_id) ===" -ForegroundColor Green
    Write-Host "Updated: $txt"
    if ($state.dut_log_path) { Write-Host "DUT:     $($state.dut_log_path)" }
    if ($state.pcap_path) { Write-Host "Pcap:    $($state.pcap_path)" }
    if ($state.wire_path -and (Test-Path $state.wire_path)) { Write-Host "Wire:    $($state.wire_path)" }
    if (-not $PasteClipboard) {
        Write-Host "Tip: paste TSP Output into the .txt file, or re-run with -PasteClipboard after copying."
    }
    Write-Host ""
}

function Action-Status {
    $state = Read-ActiveState
    if ($state) {
        Write-Host "Active: $($state.test_id) since $($state.started_utc)"
        Write-Host "  log: $($state.txt_path)"
        Write-Host "  dut: $($state.dut_log_path)"
        Write-Host "  pcap: $($state.pcap_path)"
    }
    else {
        Write-Host "No active capture."
    }
}

Ensure-Dirs
$date = Get-SessionDate

switch ($Action) {
    "Init"   { Action-Init -Date $date }
    "Start"  { Action-Start }
    "Stop"   { Action-Stop }
    "Status" { Action-Status }
}
