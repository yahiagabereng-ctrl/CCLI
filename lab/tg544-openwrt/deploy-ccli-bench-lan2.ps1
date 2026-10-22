# Two-PC bench deploy from logger PC (LAN2) - MMS :3782 + 104 :2404 + Modbus RTU ttyS1
# For EMT432 TCP + DIO (full HW), yaml-only: push-lab-yaml-to-dut.ps1 (no binary bump).
# Usage (from repo root, after lab\set_pc_lan2_oa.cmd):
#   . .\lab\tg544-openwrt\lab-env.ps1
#   .\lab\tg544-openwrt\deploy-ccli-bench-lan2.ps1
#   .\lab\tg544-openwrt\deploy-ccli-bench-lan2.ps1 -Profile Emt432Tcp
#
# Uses ccli-bin matching apps/ccli/VERSION (see apps/ccli/VERSION). APK files stop at r30;
# for r52+ use this script (binary deploy), not old APK.

param(
    [string]$HostAddr = "192.168.1.130",
    [ValidateSet("RtuTtyS1", "Emt432Tcp")]
    [string]$Profile = "RtuTtyS1",
    [switch]$UseLegacyApk
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$SessionFix = Join-Path $PSScriptRoot "deploy-ccli-session-fix.ps1"
$Yaml = if ($Profile -eq "Emt432Tcp") {
    Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_emt432_lan3_tcp.yaml"
} else {
    Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1.yaml"
}

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Source lab-env.ps1 first: . .\lab\tg544-openwrt\lab-env.ps1"
}

if ($UseLegacyApk) {
    $apks = Get-ChildItem (Join-Path $PSScriptRoot "ccli-0.1.0-r*.apk") | Sort-Object { [int]($_.BaseName -replace '.*-r', '') }
    Write-Warning "Legacy APK only ($($apks[-1].Name)) - prefer binary deploy for VERSION r52."
    & (Join-Path $PSScriptRoot "deploy-phase1.ps1") -HostAddr $HostAddr
    exit $LASTEXITCODE
}

$changes = if ($Profile -eq "Emt432Tcp") {
    @(
        "Two-PC bench lab_tr400_phase4_emt432_lan3_tcp.yaml",
        "MMS TLS 3782 + 104 2404 + EMT432 Modbus TCP LAN3 + DIO",
        "Yaml profile Emt432Tcp"
    )
} else {
    @(
        "Two-PC bench lab_tr400_phase4_eth_b_ttyS1.yaml",
        "MMS TLS 3782 LAN1 + IEC104 2404 LAN2 + Modbus RTU /dev/ttyS1",
        "Unified cert test session deploy"
    )
}

& $SessionFix -HostAddr $HostAddr -LabYaml $Yaml -Changes $changes
