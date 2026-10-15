# DEPRECATED: git ccli-bin.r51 is mislabeled (DUT reports r30). Use deploy-p3-07-connect-dut.ps1
# Deploy Oct-08 TestPro Connect profile: ccli r51 + p3_07_connect PKI + lab_tg544_eth_a.cfg
# Target: TG544 LAN1 192.168.10.1 (logger on LAN1 after lab\set_pc_lan1_logger.cmd)
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\deploy-p3-07-oct08-connect.ps1
#   powershell -File scripts\deploy-p3-07-oct08-connect.ps1 -HostAddr 192.168.1.130

param(
    [string]$HostAddr = "192.168.10.1"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$BinR51 = Join-Path $Repo "lab\tg544-openwrt\ccli-bin.r51"
$BinLive = Join-Path $Repo "lab\tg544-openwrt\ccli-bin"
$BinBak = Join-Path $Repo "lab\tg544-openwrt\ccli-bin.deploy.bak"
$VersionFile = Join-Path $Repo "apps\ccli\VERSION"
$VersionBak = Join-Path $Repo "apps\ccli\VERSION.deploy.bak"
$DeployPs1 = Join-Path $Repo "lab\tg544-openwrt\deploy-ccli-session-fix.ps1"
$LabYaml = Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml"
$TlsDir = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"

if (-not (Test-Path $BinR51)) {
    Write-Host "Missing $BinR51 - extract: git checkout 4c73add -- lab/tg544-openwrt/ccli-bin; copy to ccli-bin.r51" -ForegroundColor Red
    exit 1
}
if (-not (Test-Path $LabYaml)) { throw "Missing $LabYaml" }
if (-not (Test-Path (Join-Path $TlsDir "client_tls.pem"))) { throw "Missing TLS in $TlsDir" }

Write-Host "=== P3-07 Oct-08 deploy -> $HostAddr (binary r51 from ccli-bin.r51) ===" -ForegroundColor Cyan
Write-Host "Ensure PC can reach DUT: ping $HostAddr (LAN1: run lab\set_pc_lan1_logger.cmd as Admin)" -ForegroundColor Yellow

Copy-Item -Force $BinLive $BinBak -ErrorAction SilentlyContinue
Copy-Item -Force $VersionFile $VersionBak
Copy-Item -Force $BinR51 $BinLive
@"
0.1.0
51
P3-07-AARE-SIGN-RAW-GT
"@ | Set-Content -Path $VersionFile -Encoding ASCII -NoNewline
Add-Content -Path $VersionFile -Value "" -Encoding ASCII

try {
    & $DeployPs1 -HostAddr $HostAddr -LabYaml $LabYaml -TlsDir $TlsDir -SkipVersionCheck -Changes @(
        "Oct-08 TSP Connect repro",
        "ccli 0.1.0-r51 P3-07-AARE-SIGN-RAW-GT",
        "p3_07_connect PKI crl=off",
        "lab_tg544_eth_a.cfg"
    )
    $ec = $LASTEXITCODE
    if ($ec -ne 0) { exit $ec }
    Write-Host "=== Post-deploy: preflight + TLS pin ===" -ForegroundColor Cyan
    & (Join-Path $Repo "scripts\dut-mms-tsp-preflight.ps1") -HostAddr $HostAddr
    & (Join-Path $Repo "scripts\verify-dut-p3-07-connect-alignment.ps1") -HostAddr $HostAddr
} finally {
    Copy-Item -Force $BinBak $BinLive -ErrorAction SilentlyContinue
    Copy-Item -Force $VersionBak $VersionFile -ErrorAction SilentlyContinue
    Remove-Item $BinBak, $VersionBak -ErrorAction SilentlyContinue
    Write-Host "Restored repo ccli-bin + VERSION (r55 working tree)." -ForegroundColor DarkGray
}
