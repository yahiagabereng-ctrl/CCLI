# Deploy CCLI Phase 1 to TG544 (Modbus RTU A2/B2 + DIO)
# Runbook: lab/PHASE1.md
# Usage:
#   $env:CCLI_TG544_PW = "your-root-password"
#   .\lab\tg544-openwrt\deploy-phase1.ps1
#   .\lab\tg544-openwrt\deploy-phase1.ps1 -BinaryOnly   # no APK; installs procd init only

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$User = "root",
    [switch]$BinaryOnly
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Apk = Join-Path $PSScriptRoot "ccli-0.1.0-r4.apk"
if (-not (Test-Path $Apk)) {
    $Apk = Join-Path $PSScriptRoot "ccli-0.1.0-r3.apk"
}
$Bin = Join-Path $PSScriptRoot "ccli-bin"
$Yaml = Join-Path $Repo "apps\ccli\config\lab_tr400_phase1_regulation.yaml"
$Init = Join-Path $Repo "package\ccli\files\ccli.init"
$Uci = Join-Path $Repo "package\ccli\files\ccli.config"
$ProcdInstall = Join-Path $PSScriptRoot "install-ccli-procd.sh"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$HostKey = "SHA256:2hiPouwqC1oxP//Q0BpvgIcqD6IG+pqLkzNih1EpNRE"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set password: `$env:CCLI_TG544_PW = 'your-password'"
}

if (-not $BinaryOnly -and -not (Test-Path $Apk)) {
    Write-Error "Missing APK. Run WSL build or use -BinaryOnly with lab/tg544-openwrt/ccli-bin"
}

if (-not (Test-Path $Bin)) {
    Write-Warning "Missing $Bin - skip binary upload unless you rebuilt ccli"
}

$pw = $env:CCLI_TG544_PW
$plinkArgs = @("-ssh", "${User}@${HostAddr}", "-pw", $pw, "-hostkey", $HostKey, "-batch")

Write-Host "=== Ping $HostAddr ==="
ping -n 1 $HostAddr | Out-Null

Write-Host "=== Upload lab.yaml (Phase 1 regulation / Figura 2) ==="
& $Pscp -scp -pw $pw -hostkey $HostKey $Yaml "${User}@${HostAddr}:/etc/ccli/lab.yaml"

if ($BinaryOnly) {
    Write-Host "=== Binary-only: ccli-bin + procd init (LuCI Startup entry) ==="
    if (Test-Path $Bin) {
        & $Pscp -scp -pw $pw -hostkey $HostKey $Bin "${User}@${HostAddr}:/usr/sbin/ccli"
    }
    & $Pscp -scp -pw $pw -hostkey $HostKey $Init "${User}@${HostAddr}:/tmp/ccli.init"
    & $Pscp -scp -pw $pw -hostkey $HostKey $Uci "${User}@${HostAddr}:/tmp/ccli.config"
    & $Pscp -scp -pw $pw -hostkey $HostKey $ProcdInstall "${User}@${HostAddr}:/tmp/install-ccli-procd.sh"
    $remoteBin = @'
chmod +x /usr/sbin/ccli
sed 's/\r$//' /tmp/ccli.init > /etc/init.d/ccli
chmod +x /etc/init.d/ccli
sed 's/\r$//' /tmp/ccli.config > /etc/config/ccli
/etc/init.d/ccli enable
/etc/init.d/ccli restart
uci set didoservice_v2.channel_2.mode=di 2>/dev/null || true
uci commit didoservice_v2 2>/dev/null || true
/etc/init.d/didoservice_v2 restart 2>/dev/null || true
pgrep -a ccli
'@
    & $Plink @plinkArgs $remoteBin
} else {
    Write-Host "=== Upload APK + install (includes /etc/init.d/ccli) ==="
    & $Pscp -scp -pw $pw -hostkey $HostKey $Apk "${User}@${HostAddr}:/tmp/ccli.apk"
    $remoteApk = @'
set -e
apk add --allow-untrusted /tmp/ccli.apk || apk add --allow-untrusted --force-overwrite /tmp/ccli.apk
sed 's/\r//g' /etc/init.d/ccli > /tmp/ccli.init && mv /tmp/ccli.init /etc/init.d/ccli
chmod +x /etc/init.d/ccli
/etc/init.d/ccli enable
uci set didoservice_v2.channel_2.mode=di 2>/dev/null || true
uci commit didoservice_v2 2>/dev/null || true
/etc/init.d/didoservice_v2 restart 2>/dev/null || true
/etc/init.d/ccli restart
sleep 2
pgrep -a ccli || logread | tail -15
'@
    & $Plink @plinkArgs $remoteApk
}

Write-Host ""
Write-Host "LuCI: System -> Startup -> ccli (enabled)"
Write-Host 'Verify on device: /etc/init.d/ccli status; ccli --regulation-check --config /etc/ccli/lab.yaml'
