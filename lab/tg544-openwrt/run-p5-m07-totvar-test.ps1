# P5-M07 - TotVAr on Eth_A MMS (T.3.1.3 / O.8.3)
param(
    [string]$DutMmsAddr = "192.168.10.1",
    [string]$DeployHost = "192.168.1.130",
    [int]$Port = 3782,
    [string]$CertDir = "/mnt/c/Yahia/projects/CCLI/apps/ccli/config/tls",
    [switch]$SkipDeploy,
    [switch]$SkipBuild,
    [switch]$ForceMmsTest
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$DeployScript = Join-Path $PSScriptRoot "deploy-ccli-session-fix.ps1"
$LabYaml = Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_eth_b.yaml"
$EvidenceDir = Join-Path $Repo "lab\evidence\phase5"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$LogFile = Join-Path $EvidenceDir "P5_M07_TOTVAR_$Stamp.txt"
$Canonical = Join-Path $EvidenceDir "P5_M07_TOTVAR.txt"
$TlsClient = Join-Path $Repo "lab\tg544-openwrt\mms_tls_client"

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

Log "# P5-M07 - PdCMMXU1.TotVAr (Eth_A MMS TLS)"
Log "Gate: T.3.1.3 / O.8.3 - Q at POC every 4 s"

if (-not $SkipDeploy) {
    if (-not $env:CCLI_TG544_PW) { Write-Error "Set `$env:CCLI_TG544_PW" }
    Log "Deploying ccli r21 via Eth_B ($DeployHost) ..."
    $prev = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    & $DeployScript -HostAddr $DeployHost -LabYaml $LabYaml -Changes @(
        "P5-M07 TotVAr MMS DO",
        "DS_R_PdC_Mis4sec + TotVAr member",
        "update_totvar_kvar from Modbus q_kvar"
    ) 2>&1 | ForEach-Object { Log $_ }
    $ErrorActionPreference = $prev
}

$ethAReachable = (Test-NetConnection -ComputerName $DutMmsAddr -WarningAction SilentlyContinue -ErrorAction SilentlyContinue).PingSucceeded
Log "Eth_A reachability ($DutMmsAddr): $ethAReachable"
if (-not $ethAReachable -and -not $ForceMmsTest) {
    Log ""
    Log "SKIP MMS client - PC not on LAN1 (need 192.168.10.10/24 cable to DUT LAN1)."
    Log "After cable swap: re-run with -SkipDeploy to verify TotVAr read."
    Log "P5-M07 VERDICT: PART (r21 deployed; MMS read pending LAN1)"
    Copy-Item $LogFile $Canonical -Force
    Write-Host ""
    Write-Host "Evidence: $Canonical" -ForegroundColor Cyan
    exit 2
}

if (-not $SkipBuild) {
    Log "Building MMS TLS lab client ..."
    wsl bash -lc "cd /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt; bash build-mms-lab-client.sh" 2>&1 | ForEach-Object { Log $_ }
}

if (-not (Test-Path $TlsClient)) {
    throw "Missing $TlsClient - run build-mms-lab-client.sh"
}

Log "Running mms_tls_client (TotW+TotVAr read) ..."
$prev2 = $ErrorActionPreference
$ErrorActionPreference = "Continue"
$wslCmd = "cd $CertDir; /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_tls_client $DutMmsAddr $Port ."
$out = wsl bash -lc $wslCmd 2>&1
$exitCode = $LASTEXITCODE
$ErrorActionPreference = $prev2
foreach ($line in $out) { Log $line }

$joined = $out -join "`n"
$hasTotVar = $joined -match "TotVAr"
$hasReadOk = $joined -match "READ OK TotVAr"

Log ""
if ($exitCode -eq 0 -and $hasTotVar -and $hasReadOk) {
    Log "P5-M07 VERDICT: PASS (TotVAr read on MMS TLS)"
} else {
    Log "P5-M07 VERDICT: FAIL exit=$exitCode totvar=$hasTotVar read_ok=$hasReadOk"
}

Copy-Item $LogFile $Canonical -Force
Write-Host ""
Write-Host "Evidence: $Canonical" -ForegroundColor Cyan
if ($exitCode -eq 0 -and $hasTotVar -and $hasReadOk) { exit 0 } else { exit 1 }
