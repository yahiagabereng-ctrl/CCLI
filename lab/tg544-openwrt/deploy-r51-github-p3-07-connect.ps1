# Install GitHub PASS binary (ccli-0.1.0-r51.apk) + P3-07 TLS + lab Connect yaml.
# Use this when TestPro still works with p3_07_connect certs but deploy-product (r55 ccli-bin) broke Connect.
#
# Release: https://github.com/yahiagabereng-ctrl/CCLI/releases/tag/p3-07-security-pass
# APK SHA-256: 0a2d2782d76e60dcbbafb851416ec24a0aa707b592c51cfa4545c08f1504419c
#
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File lab\tg544-openwrt\deploy-r51-github-p3-07-connect.ps1 -HostAddr 192.168.1.130

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$User = "root",
    [switch]$SkipApk,
    [switch]$ConfigOnly
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"

$ApkName = "ccli-0.1.0-r51.apk"
$ApkExpectedSha = "0a2d2782d76e60dcbbafb851416ec24a0aa707b592c51cfa4545c08f1504419c"
$ApkUrl = "https://github.com/yahiagabereng-ctrl/CCLI/releases/download/p3-07-security-pass/$ApkName"
$ApkLocal = Join-Path $PSScriptRoot $ApkName

$Yaml = Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml"
$ModelCfg = Join-Path $Repo "apps\ccli\config\icd\lab_tg544_eth_a.cfg"
$TlsDir = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"

if (-not $env:CCLI_TG544_PW) { throw "Source lab\tg544-openwrt\lab-env.ps1 first." }
if (-not (Test-Path $Plink)) { throw "Install PuTTY plink." }

function Ensure-R51Apk {
    if (Test-Path $ApkLocal) {
        $sha = (Get-FileHash $ApkLocal -Algorithm SHA256).Hash.ToLower()
        if ($sha -eq $ApkExpectedSha) { return }
        Write-Host "Local $ApkName SHA mismatch ($sha) — re-downloading." -ForegroundColor Yellow
    }
    Write-Host "Downloading GitHub r51 APK..." -ForegroundColor Cyan
    Invoke-WebRequest -Uri $ApkUrl -OutFile $ApkLocal -UseBasicParsing
    $sha = (Get-FileHash $ApkLocal -Algorithm SHA256).Hash.ToLower()
    if ($sha -ne $ApkExpectedSha) {
        throw "Downloaded APK SHA $sha != expected $ApkExpectedSha"
    }
}

function Push-Config {
    if (-not (Test-Path $Yaml)) { throw "Missing yaml: $Yaml" }
    Write-Host "=== Yaml + lab model + P3-07 TLS ===" -ForegroundColor Cyan
    & $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $Yaml "${User}@${HostAddr}:/etc/ccli/lab.yaml"
    & $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch "mkdir -p /etc/ccli/icd"
    & $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $ModelCfg "${User}@${HostAddr}:/etc/ccli/icd/lab_tg544_eth_a.cfg"
    & powershell -NoProfile -File (Join-Path $PSScriptRoot "push-p3-07-tls-to-dut.ps1") -HostAddr $HostAddr
}

if (-not $ConfigOnly) {
    Ensure-R51Apk
    Write-Host "=== Install $ApkName on DUT (replaces hand-copied r55 ccli-bin) ===" -ForegroundColor Cyan
    & $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $ApkLocal "${User}@${HostAddr}:/tmp/$ApkName"
    & $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch @"
set -eu
uci set iec61850service.global.enabled='0' 2>/dev/null || true
uci commit iec61850service 2>/dev/null || true
/etc/init.d/iec61850service stop 2>/dev/null || true
/etc/init.d/iec61850-mmsd stop 2>/dev/null || true
killall ccli 2>/dev/null || true
sleep 1
apk add --allow-untrusted --force-overwrite /tmp/$ApkName
echo '=== ccli after apk ==='
/usr/sbin/ccli --version || true
sha256sum /usr/sbin/ccli || true
"@
}

Push-Config

& $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "killall ccli 2>/dev/null || true; sleep 1; /etc/init.d/ccli restart 2>/dev/null || (/usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 &); sleep 2; /usr/sbin/ccli --version; ss -tlnp | grep -E '3782|2404' || true"

Write-Host ""
Write-Host "Expect: ccli 0.1.0-r51 (P3-07-AARE-SIGN-RAW-GT). TestPro unchanged (p3_07_connect + lab_tg544_eth_a.cid)." -ForegroundColor Green
Write-Host "Do NOT run deploy-product-fw-same-cyber.ps1 unless you intend r55 binary e192d3cc..." -ForegroundColor Yellow
