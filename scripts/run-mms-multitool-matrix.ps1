# Run CCLI MMS multi-tool verification helpers (libiec61850 + capture + offline analyse).
param(
    [string]$DutHost = "192.168.10.1",
    [int]$MmsPort = 3782,
    [string]$TlsDir = "",
    [string]$CertForLocate = "server_combined.pem",
    [int]$CaptureSec = 90,
    [switch]$SkipCapture,
    [switch]$SkipLibClient,
    [switch]$SkipLocate
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Evidence = Join-Path $Repo "lab\evidence\testsuite-pro\inbox"
$PcapDir = Join-Path $Repo "lab\evidence\testsuite-pro\pcap"
$Stamp = Get-Date -Format "yyyy-MM-dd"
$Log = Join-Path $Evidence "MMS_MULTITOOL_RUN_$Stamp.log"

if (-not $TlsDir) {
    if (Test-Path "C:\CCLI_tls\root_CA.pem") { $TlsDir = "C:\CCLI_tls" }
    else { $TlsDir = Join-Path $Repo "apps\ccli\config\tls" }
}

function Write-Log($msg) {
    $line = "[$(Get-Date -Format 'HH:mm:ss')] $msg"
    Write-Host $line
    Add-Content -Path $Log -Value $line
}

New-Item -ItemType Directory -Force -Path $Evidence, $PcapDir | Out-Null
Write-Log "=== MMS multi-tool matrix run ==="
Write-Log "DUT=$DutHost`:$MmsPort TLS dir=$TlsDir"
Write-Log "Log file: $Log"

# --- Phase 0 hint ---
Write-Log ""
Write-Log "Phase 0: confirm DUT — ssh root@$DutHost 'netstat -tlnp | grep $MmsPort; grep auth /tmp/ccli.log | tail -3'"

# --- Phase A: libiec61850 clients ---
if (-not $SkipLibClient) {
    Write-Log ""
    Write-Log "Phase A: libIEC61850 lab clients"
    $ClientDir = Join-Path $Repo "lab\tg544-openwrt\clients"
    $WslClientDir = "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/clients"
    $WslTls = "/mnt/c/Yahia/projects/CCLI/apps/ccli/config/tls"
    if ($TlsDir -like "C:\CCLI_tls*") { $WslTls = "/mnt/c/CCLI_tls" }

    $mmsTls = Join-Path $ClientDir "mms_tls_client"
    if (Test-Path $mmsTls) {
        Write-Log "Running $mmsTls (native)"
        & $mmsTls $DutHost $MmsPort $TlsDir 2>&1 | Tee-Object -FilePath (Join-Path $Evidence "TSP_P3_06_LIBIEC61850_CLIENT_$Stamp.txt")
    }
    elseif (Get-Command wsl -ErrorAction SilentlyContinue) {
        Write-Log "Running mms_tls_client via WSL"
        wsl bash -lc "test -x '$WslClientDir/mms_tls_client' || (cd /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt && ./build-mms-lab-client.sh); '$WslClientDir/mms_tls_client' '$DutHost' '$MmsPort' '$WslTls'" 2>&1 |
            Tee-Object -FilePath (Join-Path $Evidence "TSP_P3_06_LIBIEC61850_CLIENT_$Stamp.txt")
    }
    else {
        Write-Log "SKIP lib client — build with: lab/tg544-openwrt/build-mms-lab-client.sh"
    }

    $wlim = Join-Path $ClientDir "mms_tls_wlim_client"
    if (Test-Path $wlim) {
        Write-Log "Running mms_tls_wlim_client dso write"
        & $wlim $DutHost $MmsPort $TlsDir dso 12 write 2>&1 |
            Add-Content (Join-Path $Evidence "TSP_P3_04_LIBIEC61850_WLIM_$Stamp.txt")
    }
}

# --- Phase B/E: Wireshark capture window ---
if (-not $SkipCapture) {
    Write-Log ""
    Write-Log "Phase B/E: starting Wireshark capture ${CaptureSec}s — CONNECT WITH TSP NOW"
    Write-Log "  .\scripts\capture-mms-tsp.ps1 -DutHost $DutHost -DurationSec $CaptureSec"
    & (Join-Path $Repo "scripts\capture-mms-tsp.ps1") -DutHost $DutHost -MmsPort $MmsPort -DurationSec $CaptureSec -OutDir $PcapDir
}

# --- Offline locate (needs hex on disk) ---
if (-not $SkipLocate) {
    Write-Log ""
    Write-Log "Phase B offline: locate_p3_07_failure.py"
    $HexCandidates = @(
        (Join-Path $Repo "lab\tmp_tx_solution_c_20260929.hex"),
        (Join-Path $Repo "lab\tmp_tx_latest.hex"),
        (Join-Path $Repo "lab\tmp_tx_dut_20260929.hex")
    )
    $CertPath = Join-Path $Repo "apps\ccli\config\tls\$CertForLocate"
    foreach ($hex in $HexCandidates) {
        if (-not (Test-Path $hex)) { continue }
        Write-Log "Analysing $hex"
        python (Join-Path $Repo "lab\locate_p3_07_failure.py") $hex --cert $CertPath 2>&1 |
            Tee-Object -FilePath (Join-Path $Evidence "TSP_P3_07_LOCATE_$Stamp.txt")
        break
    }
}

Write-Log ""
Write-Log "Manual steps remaining:"
Write-Log "  Phase B — Test Suite Pro Connect (see lab/CCI_MMS_MultiTool_Verification.md)"
Write-Log "  Phase C — TMW IED Simulator + Use TMW Sample Security"
Write-Log "  Phase D — IEDScout (if licensed)"
Write-Log "  Fill checklist: lab/evidence/testsuite-pro/inbox/MMS_MULTITOOL_MATRIX_$Stamp.md"
Write-Log "Done."
