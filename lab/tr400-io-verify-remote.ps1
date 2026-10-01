# Run DIO map checks on TG544 via plink (non-interactive).
# Usage:
#   $env:CCLI_TG544_PW = "root-password"
#   .\lab\tr400-io-verify-remote.ps1

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$User = "root"
)

$ErrorActionPreference = "Stop"
$HostKey = "SHA256:2hiPouwqC1oxP//Q0BpvgIcqD6IG+pqLkzNih1EpNRE"
$Plink = "C:\Program Files\PuTTY\plink.exe"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set `$env:CCLI_TG544_PW then re-run."
}
if (-not (Test-Path $Plink)) {
    Write-Error "Install PuTTY plink at: $Plink"
}

$remote = @'
set -e
echo "=== hostname ==="
uci get system.@system[0].hostname 2>/dev/null || hostname
echo "=== dido_v2 status ==="
ubus call dido_v2 status
echo "=== UCI channel modes ==="
echo -n "ch1: "; uci get didoservice_v2.channel_1.mode 2>/dev/null || echo "?"
echo -n "ch2: "; uci get didoservice_v2.channel_2.mode 2>/dev/null || echo "?"
echo "=== /etc/ccli/lab.yaml (io) ==="
grep -E 'permissive_bypass|do_curtail|di_permissive|gpio:|active_' /etc/ccli/lab.yaml 2>/dev/null | head -12 || echo "(missing lab.yaml)"
echo "=== DO channel 1: OFF -> ON -> OFF ==="
ubus call dido_v2 set_relay '{"channel":1,"value":0}'
ubus call dido_v2 set_relay '{"channel":1,"value":1}'
sleep 1
ubus call dido_v2 set_relay '{"channel":1,"value":0}'
echo "=== DI channel 2: gd32.read_di (di[1]) ==="
ubus call dido_v2 gd32.read_di '{}'
'@

Write-Host "=== Ping $HostAddr ===" -ForegroundColor Cyan
ping -n 1 $HostAddr | Out-Null

Write-Host "=== SSH verify ===" -ForegroundColor Cyan
& $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $remote
