# Apply EMT432 Rogowski 80 mV/kA on meter (40235=0.00008) + push lab yaml to DUT.
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1
#   powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1 -DutHost 192.168.178.1   # SSH via plant LAN3
#   powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1 -SkipMeterWrite

param(
    [string]$MeterHost = "192.168.178.102",
    [string]$DutHost = "192.168.1.130",
    [switch]$SkipMeterWrite
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$LabYaml = Join-Path $RepoRoot "apps\ccli\config\lab_tr400_emt432_lan3_tcp_102.yaml"
$RogowskiScript = Join-Path $RepoRoot "lab\emt432_rogowski_config.py"

if (-not (Test-Path $LabYaml)) { throw "Missing $LabYaml" }

if (-not $SkipMeterWrite) {
    Write-Host "=== Meter 40235: 80 mV/kA (0.00008 V/A) + 1P integrator ===" -ForegroundColor Cyan
    python $RogowskiScript --host $MeterHost --apply-1p-rogowski --set-sensitivity-mvka 80
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Meter write failed (PC on LAN3? ping $MeterHost). Continuing with DUT yaml push."
    }
}

Write-Host "=== Push lab.yaml to DUT ===" -ForegroundColor Cyan
& powershell -NoProfile -File (Join-Path $PSScriptRoot "push-lab-yaml-to-dut.ps1") `
    -HostAddr $DutHost `
    -LabYaml $LabYaml
if ($LASTEXITCODE -ne 0) {
    Write-Error "DUT yaml push failed (exit $LASTEXITCODE). Check SSH to $DutHost or use -DutHost 192.168.178.1 on LAN3."
}
