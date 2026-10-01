# Phase 4 Step 0 - verify PC + DUT on LAN2 (Operator Eth_B).
# Prerequisite: cable on TG544 LAN2; PC IP on 192.168.1.0/24.
#
# Usage (elevated PC IP first):
#   Right-click lab\set_pc_lan2_oa.cmd → Run as administrator
#   $env:CCLI_TG544_PW = "root-password"
#   .\lab\tg544-openwrt\verify-lan2-eth-b.ps1
#
# Optional: skip SSH if password not set (local checks only)
#   .\lab\tg544-openwrt\verify-lan2-eth-b.ps1 -SkipSsh

param(
    [string]$DutAddr = "192.168.1.130",
    [string]$PcAddr = "192.168.1.183",
    [string]$User = "root",
    [switch]$SkipSsh
)

$ErrorActionPreference = "Continue"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$EvidenceDir = Join-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) "lab\evidence\phase4"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$LogFile = Join-Path $EvidenceDir "P4_01_LAN2_VERIFY_$Stamp.txt"

function Write-Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null
Write-Log "=== P4-01 LAN2 / Eth_B verify ==="
Write-Log "DUT=$DutAddr PC=$PcAddr"

# --- PC side ---
Write-Log "--- PC Ethernet ---"
Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
    Where-Object { $_.InterfaceAlias -eq "Ethernet" } |
    ForEach-Object { Write-Log "  $($_.IPAddress)/$($_.PrefixLength) on $($_.InterfaceAlias)" }

$ethIp = (Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
    Where-Object { $_.InterfaceAlias -eq "Ethernet" }).IPAddress
if ($ethIp -notmatch "^192\.168\.1\.") {
    Write-Log "FAIL: PC Ethernet not on 192.168.1.0/24 (got '$ethIp')."
    Write-Log "      Run lab\set_pc_lan2_oa.cmd as Administrator, or use DHCP on LAN2."
} else {
    Write-Log "PASS: PC on operator subnet ($ethIp)."
}

Write-Log "--- Ping DUT ---"
$ping = Test-Connection -ComputerName $DutAddr -Count 4 -Quiet
if ($ping) {
    Write-Log "PASS: ping $DutAddr"
} else {
    Write-Log "FAIL: no reply from $DutAddr - check cable on LAN2, DUT power, firewall input on lan_sec."
}

Write-Log "--- LuCI (Eth_B) ---"
Write-Log "  http://${DutAddr}/cgi-bin/luci"
Write-Log "  http://${DutAddr}/ccli/mock_console.html"

# --- DUT side ---
if ($SkipSsh) {
    Write-Log "SKIP: SSH ( -SkipSsh )"
    Write-Log "=== end ==="
    Write-Host "`nEvidence: $LogFile"
    exit 0
}

if (-not $env:CCLI_TG544_PW) {
    Write-Log "SKIP: SSH - set `$env:CCLI_TG544_PW for DUT checks."
    Write-Log "=== end ==="
    Write-Host "`nEvidence: $LogFile"
    exit 0
}
if (-not (Test-Path $Plink)) {
    Write-Log "SKIP: plink not found at $Plink"
    exit 1
}

$remote = "echo '=== DUT LAN2 / Eth_B'; date; echo '--- listen 3782/102/2404 ---'; netstat -tlnp 2>/dev/null | grep -E '3782|:102|:2404' || echo '(none)'; echo '--- P4-01 :2404 check ---'; netstat -tlnp 2>/dev/null | grep 2404 || echo '(no :2404)'; echo '--- ccli ---'; pgrep -a ccli || echo '(ccli not running)'; /usr/sbin/ccli --version 2>/dev/null || true; echo '=== end DUT ==='"

Write-Log "--- SSH $User@${DutAddr} ---"
$sshOut = & $Plink -ssh "${User}@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $remote 2>&1
$sshOut | ForEach-Object { Write-Log $_ }

if ($LASTEXITCODE -ne 0) {
    Write-Log "FAIL: SSH exit $LASTEXITCODE"
} else {
    Write-Log "PASS: SSH session OK"
}

Write-Log "=== end ==="
Write-Host "`nEvidence: $LogFile"
