# Start ccli MMS on TG544 Eth_A via SSH (non-interactive plink).
# Usage:
#   $env:CCLI_TG544_PW = "root-password"
#   .\lab\tg544-openwrt\start-mms-remote.ps1
# Optional: -HostAddr 192.168.1.130 when on operator LAN

param(
    [string]$HostAddr = "192.168.10.1",
    [string]$User = "root"
)

$ErrorActionPreference = "Stop"
# Eth_A 192.168.10.1 (ED25519). Operator LAN 192.168.1.130 uses a different fingerprint.
$HostKey = if ($HostAddr -eq "192.168.10.1") {
    "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
} else {
    "SHA256:2hiPouwqC1oxP//Q0BpvgIcqD6IG+pqLkzNih1EpNRE"
}
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$Script = Join-Path $PSScriptRoot "start-mms-eth-a.sh"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set `$env:CCLI_TG544_PW then re-run."
}
if (-not (Test-Path $Plink)) {
    Write-Error "Install PuTTY plink at: $Plink"
}
if (-not (Test-Path $Script)) {
    Write-Error "Missing script: $Script"
}

Write-Host "=== Ping $HostAddr ===" -ForegroundColor Cyan
ping -n 1 $HostAddr | Out-Null
if ($LASTEXITCODE -ne 0) {
    Write-Error "Host $HostAddr unreachable"
}

$pw = $env:CCLI_TG544_PW
$remoteSh = "/tmp/start-mms-eth-a.sh"

Write-Host "=== Upload start script ===" -ForegroundColor Cyan
& $Pscp -scp -pw $pw -hostkey $HostKey $Script "${User}@${HostAddr}:${remoteSh}"

Write-Host "=== Run on DUT ===" -ForegroundColor Cyan
& $Plink -ssh "${User}@${HostAddr}" -pw $pw -hostkey $HostKey -batch "sed 's/\r$//' $remoteSh > /tmp/smm.sh && chmod +x /tmp/smm.sh && sh /tmp/smm.sh"

Write-Host "=== PC port check ===" -ForegroundColor Cyan
Test-NetConnection -ComputerName $HostAddr -Port 3782 -WarningAction SilentlyContinue |
    Select-Object ComputerName, RemotePort, TcpTestSucceeded
