# P5 LAN1 gate - deploy r22 via Eth_A + MMS read TotW/TotVAr/PPV (T.3.1.3)
param(
    [string]$DutMmsAddr = "192.168.10.1",
    [string]$DeployHost = "192.168.10.1",
    [string]$ExpectedPcIp = "192.168.10.10",
    [int]$Port = 3782,
    [string]$CertDir = "/mnt/c/Yahia/projects/CCLI/apps/ccli/config/tls",
    [switch]$SkipDeploy,
    [switch]$SkipBuild,
    [switch]$ConfigureLan1
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$DeployScript = Join-Path $PSScriptRoot "deploy-ccli-session-fix.ps1"
$Lan1Script = Join-Path $Repo "lab\set_pc_lan1_dso.cmd"
$LabYaml = Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_eth_b.yaml"
$EvidenceDir = Join-Path $Repo "lab\evidence\phase5"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$LogFile = Join-Path $EvidenceDir "P5_LAN1_VERIFY_$Stamp.txt"
$Canonical = Join-Path $EvidenceDir "P5_LAN1_VERIFY.txt"
$TlsClient = Join-Path $Repo "lab\tg544-openwrt\mms_tls_client"

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

Log "# P5 Phase 5 LAN1 verify (Eth_A MMS TLS)"
Log "Gates: P5-M07 TotVAr + P5-M08 PPV + TotW"

if ($ConfigureLan1) {
    Log "Running set_pc_lan1_dso.cmd (needs Administrator) ..."
    Start-Process -FilePath "cmd.exe" -ArgumentList "/c", $Lan1Script -Verb RunAs -Wait
}

$pcIps = @(Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
    Where-Object { $_.IPAddress -like "192.168.*" } |
    ForEach-Object { $_.IPAddress })
Log "PC IPv4: $($pcIps -join ', ')"
if ($pcIps -notcontains $ExpectedPcIp) {
    Log "WARN: PC not on ${ExpectedPcIp}/24 - run as Admin: lab\set_pc_lan1_dso.cmd"
}

$ethAReachable = (Test-NetConnection -ComputerName $DutMmsAddr -WarningAction SilentlyContinue -ErrorAction SilentlyContinue).PingSucceeded
Log "Ping ${DutMmsAddr}: $ethAReachable"
if (-not $ethAReachable) {
    Log "FAIL: Cannot reach DUT on LAN1. Cable to LAN1 + 192.168.10.10/24 required."
    Copy-Item $LogFile $Canonical -Force
    exit 2
}

if (-not $SkipDeploy) {
    if (-not $env:CCLI_TG544_PW) { Write-Error "Set env:CCLI_TG544_PW" }
    Log "Deploying ccli r22 via LAN1 ($DeployHost) ..."
    $prev = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    & $DeployScript -HostAddr $DeployHost -LabYaml $LabYaml -Changes @(
        "P5-PPV r22",
        "PdCMMXU1 PPV DEL phsAB",
        "TotVAr + ENC stub fix",
        "urcb DS_R_PdC_Mis4sec"
    ) 2>&1 | ForEach-Object { Log $_ }
    $ErrorActionPreference = $prev
}

if (-not $SkipBuild) {
    Log "Building MMS TLS lab client ..."
    $prevB = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    wsl bash -lc "sed -i 's/\r$//' /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/build-mms-lab-client.sh; cd /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt; bash build-mms-lab-client.sh" 2>&1 | ForEach-Object { Log $_ }
    $ErrorActionPreference = $prevB
}

if (-not (Test-Path $TlsClient)) {
    throw "Missing $TlsClient"
}

Log "mms_tls_client -> ${DutMmsAddr}:${Port} ..."
$prev2 = $ErrorActionPreference
$ErrorActionPreference = "Continue"
$wslCmd = "cd $CertDir; /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_tls_client $DutMmsAddr $Port ."
$out = wsl bash -lc $wslCmd 2>&1
$exitCode = $LASTEXITCODE
$ErrorActionPreference = $prev2
foreach ($line in $out) { Log $line }

$j = $out -join "`n"
$okTotW = $j -match "READ OK TotW"
$okTotVar = $j -match "READ OK TotVAr"
$okPpv = $j -match "READ OK PPV"

Log ""
if ($okTotVar) { Log "P5-M07 TotVAr: PASS" } else { Log "P5-M07 TotVAr: FAIL" }
if ($okPpv) { Log "P5-M08 PPV: PASS" } else { Log "P5-M08 PPV: FAIL" }
if ($okTotW) { Log "P5-02 TotW: PASS" } else { Log "P5-02 TotW: FAIL" }

if ($exitCode -eq 0 -and $okTotW -and $okTotVar -and $okPpv) {
    Log "P5 LAN1 VERDICT: PASS"
    $rc = 0
} else {
    Log "P5 LAN1 VERDICT: FAIL exit=$exitCode"
    $rc = 1
}

Copy-Item $LogFile $Canonical -Force
Write-Host ""
Write-Host "Evidence: $Canonical" -ForegroundColor Cyan
exit $rc
