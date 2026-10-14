# Pull full TLS mirror from deployed DUT, verify key pairing, ready for USB to Masomeh.
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\export-pc1-tls-for-masomeh.ps1
#   powershell -File scripts\export-pc1-tls-for-masomeh.ps1 -Dest D:\handoff\PC1_tls_full

param(
    [string]$Dest = "",
    [string]$HostAddr = "192.168.1.130"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot

& (Join-Path $RepoRoot "scripts\pull-dut-tls-full-mirror.ps1") -Dest $Dest -HostAddr $HostAddr
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (-not $Dest) {
    $Dest = Get-ChildItem (Join-Path $RepoRoot "lab\testpro\staged") -Filter "pc1_tls_from_dut_*" |
        Sort-Object LastWriteTime -Descending | Select-Object -First 1 -ExpandProperty FullName
}

& (Join-Path $RepoRoot "scripts\verify-tsp-tls-key-match.ps1") -TlsDir $Dest
if ($LASTEXITCODE -ne 0) {
    Write-Host "Key pairing failed on pulled bundle" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "READY for Masomeh: $Dest (full DUT TLS mirror)" -ForegroundColor Green
