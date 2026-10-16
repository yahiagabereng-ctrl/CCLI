# Refresh handoff/masomeh95/CCLI_TSP_CONNECT_MIN from repo p3_07_connect (MD5-pinned pack for PC1).
param(
    [string]$Repo = ""
)

$ErrorActionPreference = "Stop"
if (-not $Repo) { $Repo = Split-Path -Parent $PSScriptRoot }
$SrcTls = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"
$Pack = Join-Path $Repo "handoff\masomeh95\CCLI_TSP_CONNECT_MIN"
$cidSrc = Join-Path $Repo "apps\ccli\config\icd\lab_tg544_eth_a.cid"

foreach ($p in @($SrcTls, $cidSrc)) {
    if (-not (Test-Path $p)) { throw "Missing $p - run publish-p3-07-connect-profile.ps1" }
}

$files = @("root_CA.pem", "client_tls.pem", "client.pem", "client_tsp.key")
foreach ($f in $files) {
    Copy-Item -Force (Join-Path $SrcTls $f) (Join-Path $Pack "tls\$f")
}
Copy-Item -Force $cidSrc (Join-Path $Pack "cid\lab_tg544_eth_a.cid")

Write-Host "CONNECT_MIN refreshed from p3_07_connect" -ForegroundColor Green
Get-ChildItem (Join-Path $Pack "tls") | ForEach-Object {
    $h = (Get-FileHash $_.FullName -Algorithm MD5).Hash.ToLower()
    Write-Host "  $($_.Name)  $h"
}
