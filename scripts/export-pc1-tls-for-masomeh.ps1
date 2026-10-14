# Verify DUT==repo TLS, then stage PC1 cert bundle for Masomeh (USB / shared folder).
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\export-pc1-tls-for-masomeh.ps1
#   powershell -File scripts\export-pc1-tls-for-masomeh.ps1 -Dest D:\handoff\PC1_tls_verified

param(
    [string]$Dest = "",
    [string]$HostAddr = "192.168.1.130"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $Dest) {
    $stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
    $Dest = Join-Path $RepoRoot "lab\testpro\staged\pc1_tls_verified_$stamp"
}

New-Item -ItemType Directory -Force -Path $Dest | Out-Null
$pinPath = Join-Path $Dest "DUT_PC1_TLS_VERIFIED.json"

& (Join-Path $RepoRoot "scripts\verify-dut-pc1-tls-alignment.ps1") -HostAddr $HostAddr -OutJson $pinPath
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$pin = Get-Content $pinPath -Raw | ConvertFrom-Json
$dutRel = "0.1.0-r55"
if ($pin.dut_ccli -match '0\.1\.0-r(\d+)') { $dutRel = "0.1.0-r$($Matches[1])" }

& (Join-Path $RepoRoot "scripts\stage-pc1-tls-dut-bundle.ps1") -Dest $Dest -DutRelease $dutRel

Copy-Item -Force (Join-Path $RepoRoot "lab\testpro\MASOMEH_CONNECT_FIX_2026-10-14.md") (Join-Path $Dest "READ_ME_CONNECT_FIX.md")
Copy-Item -Force (Join-Path $RepoRoot "lab\evidence\testsuite-pro\inbox\DUT_PC1_TLS_PIN_2026-10-14.json") (Join-Path $Dest "DUT_PC1_TLS_PIN.json")

Write-Host ""
Write-Host "Verified PC1 TLS bundle -> $Dest" -ForegroundColor Green
Write-Host "USB this folder to Masomeh. She uses these PEMs in TSP (see READ_ME_CONNECT_FIX.md)."
