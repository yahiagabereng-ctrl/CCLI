# Copy lab PKI to TSP staging folder (no DUT deploy).
$TlsDir = Join-Path $PSScriptRoot "..\apps\ccli\config\tls" | Resolve-Path
$TspTls = "C:\CCLI_tls"
New-Item -ItemType Directory -Force -Path $TspTls | Out-Null
foreach ($f in @("root_CA.pem", "client.pem", "client.key", "client_tsp.key")) {
    Copy-Item -Force (Join-Path $TlsDir $f) (Join-Path $TspTls $f)
}
Write-Host "Synced to $TspTls"
Get-ChildItem $TspTls | Format-Table Name, Length, LastWriteTime
