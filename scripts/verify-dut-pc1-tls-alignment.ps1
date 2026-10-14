# Fail if repo apps/ccli/config/tls MD5s != DUT /etc/ccli/tls/ (PC1 cert bench set).
# Run BEFORE zipping/USB to Masomeh or staging field pack tls\.
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\verify-dut-pc1-tls-alignment.ps1
#   powershell -File scripts\verify-dut-pc1-tls-alignment.ps1 -HostAddr 192.168.1.130

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$RepoTls = "",
    [string]$OutJson = ""
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $RepoTls) {
    $RepoTls = Join-Path $RepoRoot "apps\ccli\config\tls"
}
$Plink = "C:\Program Files\PuTTY\plink.exe"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$pw = $env:CCLI_TG544_PW
if (-not $pw) { throw "Set CCLI_TG544_PW (source lab-env.ps1)" }

$files = @(
    "root_CA.pem", "root_CA.key",
    "server.pem", "server_tls.pem", "server.key", "server_acse.key",
    "server_combined.pem", "server_combined.key",
    "client.pem", "client_tls.pem", "client.key", "client_acse.key", "client_tsp.key",
    "viewer.pem", "viewer_tls.pem", "viewer.key",
    "revoked.pem", "revoked_tls.pem", "revoked.key", "revoked_acse.key",
    "lab_crl.pem"
)

Write-Host "Verify repo TLS vs DUT $HostAddr ..." -ForegroundColor Cyan

$remoteList = ($files | ForEach-Object { "/etc/ccli/tls/$_" }) -join " "
$dutRaw = & $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch "ccli --version | head -1; md5sum $remoteList 2>/dev/null"
$dutVersion = ($dutRaw | Select-Object -First 1).Trim()
$dutMd5 = @{}
foreach ($line in ($dutRaw | Select-Object -Skip 1)) {
    if ($line -match '^([0-9a-f]{32})\s+/etc/ccli/tls/(.+)$') {
        $dutMd5[$Matches[2]] = $Matches[1].ToLower()
    }
}

$fail = 0
$rows = @()
foreach ($f in $files) {
    $repoPath = Join-Path $RepoTls $f
    if (-not (Test-Path $repoPath)) {
        Write-Host "FAIL repo missing $f" -ForegroundColor Red
        $fail++
        continue
    }
    $repoMd5 = (Get-FileHash $repoPath -Algorithm MD5).Hash.ToLower()
    $dut = $dutMd5[$f]
    if (-not $dut) {
        Write-Host "FAIL DUT missing $f" -ForegroundColor Red
        $fail++
        continue
    }
    if ($repoMd5 -eq $dut) {
        Write-Host "OK  $f" -ForegroundColor Green
        $rows += [ordered]@{ file = $f; md5 = $repoMd5; match = $true }
    } else {
        Write-Host "FAIL $f  repo=$repoMd5  dut=$dut" -ForegroundColor Red
        $rows += [ordered]@{ file = $f; md5_repo = $repoMd5; md5_dut = $dut; match = $false }
        $fail++
    }
}

if ($fail -gt 0) {
    Write-Host "`nResult: FAIL ($fail) - redeploy DUT TLS from repo or sync repo from DUT before sending to PC1." -ForegroundColor Red
    exit 1
}

Write-Host "`nResult: PASS - safe to stage PC1 bundle (DUT: $dutVersion)" -ForegroundColor Green
$doc = [ordered]@{
    verified_utc = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    dut_host = $HostAddr
    dut_ccli = $dutVersion
    repo_tls = $RepoTls
    files = $rows
}
$json = $doc | ConvertTo-Json -Depth 4
if ($OutJson) {
    $json | Set-Content -Path $OutJson -Encoding UTF8
    Write-Host "Wrote $OutJson"
}
exit 0
