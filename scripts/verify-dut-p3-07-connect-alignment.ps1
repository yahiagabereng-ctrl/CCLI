# Strict DUT vs repo pin for Oct-08 / 43e8e85 P3-07 connect session (Masomeh PC1 pack = same bytes).
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\verify-dut-p3-07-connect-alignment.ps1
#   powershell -File scripts\verify-dut-p3-07-connect-alignment.ps1 -OutJson lab\evidence\testsuite-pro\inbox\DUT_PC1_TLS_PIN_p3_07.json

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$OutJson = ""
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$RepoTls = Join-Path $RepoRoot "apps\ccli\config\tls\p3_07_connect"
$RepoYaml = Join-Path $RepoRoot "apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml"
$RepoCfg = Join-Path $RepoRoot "apps\ccli\config\icd\lab_tg544_eth_a.cfg"

# Same PEM set as setup-pc1-p3-07-connect.ps1 (PC1 tls\ after wipe+copy).
$tlsFiles = @(
    "root_CA.pem",
    "server.pem", "server_tls.pem", "server.key", "server_acse.key",
    "client.pem", "client_tls.pem", "client_tsp.key",
    "viewer.pem", "viewer_tls.pem", "viewer.key"
)

$Plink = "C:\Program Files\PuTTY\plink.exe"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$pw = $env:CCLI_TG544_PW
if (-not $pw) { throw "Set CCLI_TG544_PW (source lab-env.ps1)" }

Write-Host "P3-07 connect alignment: DUT $HostAddr vs p3_07_connect + yaml + lab_tg544_eth_a.cfg" -ForegroundColor Cyan

$remoteList = ($tlsFiles | ForEach-Object { "/etc/ccli/tls/$_" }) -join " "
$dutRaw = & $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch @"
ccli --version | head -1
md5sum $remoteList 2>/dev/null
md5sum /etc/ccli/lab.yaml /etc/ccli/icd/lab_tg544_eth_a.cfg 2>/dev/null
grep -E 'model_cfg|tls_crl|tls_revoked' /etc/ccli/lab.yaml || true
netstat -tlnp 2>/dev/null | grep 3782 || true
"@

$dutVersion = ($dutRaw | Where-Object { $_ -match '^ccli ' } | Select-Object -First 1)
if (-not $dutVersion) { $dutVersion = "unknown" } else { $dutVersion = $dutVersion.Trim() }
$dutMd5 = @{}
foreach ($line in $dutRaw) {
    if ($line -match '^([0-9a-f]{32})\s+/etc/ccli/tls/(.+)$') {
        $dutMd5[$Matches[2]] = $Matches[1].ToLower()
    }
}

$fail = 0
$rows = @()
foreach ($f in $tlsFiles) {
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
        Write-Host "OK  tls/$f" -ForegroundColor Green
        $rows += [ordered]@{ file = $f; md5 = $repoMd5; match = $true }
    } else {
        Write-Host "FAIL tls/$f repo=$repoMd5 dut=$dut" -ForegroundColor Red
        $fail++
    }
}

$yamlRepo = (Get-FileHash $RepoYaml -Algorithm MD5).Hash.ToLower()
$cfgRepo = (Get-FileHash $RepoCfg -Algorithm MD5).Hash.ToLower()
$dutYaml = $null
$dutCfg = $null
foreach ($line in $dutRaw) {
    if ($line -match '^([0-9a-f]{32})\s+/etc/ccli/lab\.yaml$') { $dutYaml = $Matches[1].ToLower() }
    if ($line -match '^([0-9a-f]{32})\s+/etc/ccli/icd/lab_tg544_eth_a\.cfg$') { $dutCfg = $Matches[1].ToLower() }
}
if ($dutYaml -eq $yamlRepo) { Write-Host "OK  lab.yaml" -ForegroundColor Green } else {
    Write-Host "FAIL lab.yaml repo=$yamlRepo dut=$dutYaml" -ForegroundColor Red; $fail++
}
if ($dutCfg -eq $cfgRepo) { Write-Host "OK  lab_tg544_eth_a.cfg" -ForegroundColor Green } else {
    Write-Host "FAIL lab_tg544_eth_a.cfg repo=$cfgRepo dut=$dutCfg" -ForegroundColor Red; $fail++
}

if ($dutRaw -match 'tls_crl|tls_revoked') {
    Write-Host "FAIL lab.yaml must not set tls_crl / tls_revoked (Oct-08 crl=off)" -ForegroundColor Red
    $fail++
} else {
    Write-Host "OK  yaml has no tls_crl / tls_revoked" -ForegroundColor Green
}
if ($dutRaw -match '/ccli' -and $dutRaw -match '3782') {
    Write-Host "OK  3782 listener is ccli" -ForegroundColor Green
} else {
    Write-Host "WARN 3782 not ccli - run dut-mms-tsp-preflight.ps1" -ForegroundColor Yellow
}

if ($fail -gt 0) {
    Write-Host "`nFAIL ($fail) - redeploy:" -ForegroundColor Red
    Write-Host "  deploy-ccli-session-fix.ps1 -HostAddr $HostAddr -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml -TlsDir apps\ccli\config\tls\p3_07_connect ..."
    exit 1
}

Write-Host "`nPASS - DUT matches p3_07_connect (same as Masomeh after setup-pc1-p3-07-connect.ps1)" -ForegroundColor Green
$doc = [ordered]@{
    profile = "p3_07_connect"
    git_reference = "43e8e85"
    verified_utc = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    dut_host = $HostAddr
    dut_ccli = $dutVersion
    lab_yaml_md5 = $yamlRepo
    model_cfg_md5 = $cfgRepo
    tls = $rows
}
if ($OutJson) {
    New-Item -ItemType Directory -Force -Path (Split-Path $OutJson) | Out-Null
    $outPath = Join-Path $RepoRoot $OutJson
    $doc | ConvertTo-Json -Depth 4 | Set-Content -Path $outPath -Encoding UTF8
    Write-Host "Wrote $outPath"
}
exit 0
