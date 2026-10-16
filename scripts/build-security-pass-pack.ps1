# Build lab/releases/P3-07-SECURITY-PASS (PASS certs + r51 apk if present).
# Usage: powershell -File scripts\build-security-pass-pack.ps1

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Pack = Join-Path $Repo "lab\releases\P3-07-SECURITY-PASS"

New-Item -ItemType Directory -Force -Path (Join-Path $Pack "tls"), (Join-Path $Pack "cid"), (Join-Path $Pack "bin"), (Join-Path $Pack "config") | Out-Null

$TlsSrc = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"
Copy-Item -Force (Join-Path $TlsSrc "*") (Join-Path $Pack "tls")
Copy-Item -Force (Join-Path $Repo "apps\ccli\config\icd\lab_tg544_eth_a.cid") (Join-Path $Pack "cid")
$Cfg = Join-Path $Repo "apps\ccli\config\icd\lab_tg544_eth_a.cfg"
if (Test-Path $Cfg) { Copy-Item -Force $Cfg (Join-Path $Pack "cid") }
Copy-Item -Force (Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml") (Join-Path $Pack "config")

$r51 = Join-Path $Repo "lab\tg544-openwrt\ccli-0.1.0-r51.apk"
if (Test-Path $r51) {
    Copy-Item -Force $r51 (Join-Path $Pack "bin\ccli-0.1.0-r51.apk")
    $sha = (Get-FileHash $r51 -Algorithm SHA256).Hash.ToLower()
    Write-Host "bin: ccli-0.1.0-r51.apk  SHA256=$sha"
} else {
    Write-Warning "r51 apk missing at $r51 - MANIFEST still lists expected pins"
}

Write-Host "OK pack: $Pack"
Write-Host "Read: $Pack\README.md and MANIFEST.json"
Write-Host "TSP: Transport=client_tls.pem + client_tsp.key"
Write-Host "TSP: MMS=client.pem + client_tsp.key"
Write-Host "CID: lab_tg544_eth_a.cid"
