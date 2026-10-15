# Deploy P3-07 Connect profile: current ccli-bin (r55+) + p3_07_connect TLS + lab_tg544_eth_a.cfg
# Does NOT use mislabeled git ccli-bin.r51 (reports r30 on DUT).
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\deploy-p3-07-connect-dut.ps1 -HostAddr 192.168.10.1
#   powershell -File scripts\deploy-p3-07-connect-dut.ps1 -HostAddr 192.168.1.130 -InstallApk

param(
    [string]$HostAddr = "192.168.1.130",
    [switch]$InstallApk
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$DeployPs1 = Join-Path $Repo "lab\tg544-openwrt\deploy-ccli-session-fix.ps1"
$LabYaml = Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml"
$TlsDir = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$pw = $env:CCLI_TG544_PW

if (-not $pw) { throw "Source lab-env.ps1" }

if ($InstallApk) {
    $vlines = Get-Content (Join-Path $Repo "apps\ccli\VERSION") | ForEach-Object { $_.Trim() } | Where-Object { $_ }
    $rel = $vlines[1]
    $Apk = Join-Path $Repo "lab\tg544-openwrt\ccli-0.1.0-r$rel.apk"
    if (-not (Test-Path $Apk)) { throw "Missing $Apk - run wsl bash lab/tg544-openwrt/build-ccli-ipk-sdk.sh" }
    Write-Host "=== Install APK r$rel on DUT (then config deploy) ===" -ForegroundColor Cyan
    & $Pscp -scp -batch -pw $pw -hostkey $hk $Apk "root@${HostAddr}:/tmp/ccli-install.apk"
    & $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch `
        "apk add --allow-untrusted --force-overwrite /tmp/ccli-install.apk; /usr/sbin/ccli --version | head -3"
}

& $DeployPs1 -HostAddr $HostAddr -LabYaml $LabYaml -TlsDir $TlsDir -Changes @(
    "P3-07 connect profile p3_07_connect",
    "lab_tg544_eth_a.cfg crl=off",
    "ccli-bin from apps/ccli/VERSION (not legacy r51 git blob)"
)

Write-Host "=== Verify ===" -ForegroundColor Cyan
& (Join-Path $Repo "scripts\verify-dut-p3-07-connect-alignment.ps1") -HostAddr $HostAddr
& (Join-Path $Repo "scripts\verify-dut-ccli-install.ps1") -HostAddr $HostAddr
