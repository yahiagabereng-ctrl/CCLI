# ONE handoff: full DUT TLS folder + optional single ZIP for USB/WhatsApp.
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\export-pc1-tls-for-masomeh.ps1
#   powershell -File scripts\export-pc1-tls-for-masomeh.ps1 -Dest D:\handoff\CCLI_TSP_TLS_COMPLETE -ZipPath D:\handoff\CCLI_TSP_TLS_COMPLETE.zip

param(
    [string]$Dest = "C:\Yahia\CCLI_TSP_TLS_COMPLETE",
    [string]$ZipPath = "",
    [switch]$NoZip,
    [string]$HostAddr = "192.168.1.130"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot

if (-not $ZipPath -and -not $NoZip) {
    $ZipPath = if ($Dest -match '[\\/]$') { "${Dest}CCLI_TSP_TLS_COMPLETE.zip" }
               else { "$Dest.zip" }
}

& (Join-Path $RepoRoot "scripts\pull-dut-tls-full-mirror.ps1") -Dest $Dest -HostAddr $HostAddr
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& (Join-Path $RepoRoot "scripts\verify-tsp-tls-key-match.ps1") -TlsDir $Dest
if ($LASTEXITCODE -ne 0) {
    Write-Host "Key pairing failed on pulled bundle" -ForegroundColor Red
    exit 1
}

if (-not $NoZip -and $ZipPath) {
    if (Test-Path $ZipPath) { Remove-Item -Force $ZipPath }
    Compress-Archive -Path $Dest -DestinationPath $ZipPath -CompressionLevel Optimal
    Write-Host ""
    Write-Host "ONE FILE for handoff: $ZipPath" -ForegroundColor Green
    Write-Host "Masomeh: extract zip -> one folder with ALL certs (Connect + revoked + CRL + viewer + server ref)." -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "Folder ready (no zip): $Dest" -ForegroundColor Green
}

Write-Host "See TSP_TLS_COMPLETE_README.txt inside the folder." -ForegroundColor Cyan
