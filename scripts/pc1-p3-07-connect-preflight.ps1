# PC1 — one-shot preflight before Test Suite Pro Connect (P3-07 Oct-08 profile).
# Run as Administrator on PC-B (192.168.10.10).
#
# Usage:
#   cd D:\CCLI\CCLI\CCLI
#   git pull origin main
#   powershell -ExecutionPolicy Bypass -File scripts\pc1-p3-07-connect-preflight.ps1

param(
    [string]$Dest = "D:\CCLI_TSP_FIELD_PACK",
    [string]$DutHost = "192.168.10.1",
    [int]$Port = 3782,
    [string]$PcIp = "192.168.10.10"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot

Write-Host "`n=== P3-07 Connect preflight (PC1) ===" -ForegroundColor Cyan

& (Join-Path $Repo "scripts\setup-pc1-p3-07-connect.ps1") -Dest $Dest
& (Join-Path $Repo "scripts\verify-lab-access-freeze.ps1") -TlsDir (Join-Path $Dest "tls")
& (Join-Path $Repo "scripts\verify-tsp-tls-key-match.ps1") -TlsDir (Join-Path $Dest "tls") -Profile P3-07

$eth = Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
    Where-Object { $_.IPAddress -eq $PcIp }
if (-not $eth) {
    Write-Host "FAIL: PC not on $PcIp — run bench_scripts\set_pc_lan1_dso.cmd as Admin" -ForegroundColor Red
    exit 1
}
$tcp = Test-NetConnection -ComputerName $DutHost -Port $Port -WarningAction SilentlyContinue
if (-not $tcp.TcpTestSucceeded) {
    Write-Host "FAIL: TCP ${DutHost}:${Port} from $PcIp" -ForegroundColor Red
    exit 1
}
Write-Host "PASS: network $PcIp -> ${DutHost}:${Port}" -ForegroundColor Green

& (Join-Path $Repo "scripts\pc1-test-mms-tls12.ps1") -TlsDir (Join-Path $Dest "tls") -DutHost $DutHost -Port $Port

Write-Host @"

=== Test Suite Pro (manual — project must be NEW or re-browsed) ===
  CID:     $Dest\cid\lab_tg544_eth_a.cid   (NOT product_tr400_eth_a_5ln.cid)
  Trust:   $Dest\tls\root_CA.pem
  Transport cert/key: client_tls.pem + client_tsp.key
  MMS cert/key:       client.pem + client_tsp.key  (SAME key — NOT client_acse.key)
  CRL/revoked:        EMPTY
  Server: $DutHost`:$Port  IED CCI016_01  AP Title 1,1,1,999,1
  Connect from: System Status | TLS 1.3 OFF | Sisco OFF

If TLS preflight PASS but TSP still Encrypted Alert:
  DUT will show 'client CONNECT' only (no ACSE) — fix MMS key slot above.
  Read: handoff\masomeh95\PC1_PULL_STILL_FAILS_README.md

"@ -ForegroundColor Yellow
