# Pull ALL /etc/ccli/tls/*.pem and *.key from DUT (authoritative PC1 / TSP bundle).
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\pull-dut-tls-full-mirror.ps1 -Dest C:\Yahia\CCLI_TSP_TLS_COMPLETE

param(
    [string]$Dest = "",
    [string]$HostAddr = "192.168.1.130"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$pw = $env:CCLI_TG544_PW
if (-not $pw) { throw "Set CCLI_TG544_PW (source lab-env.ps1)" }

if (-not $Dest) {
    $Dest = Join-Path $RepoRoot "lab\testpro\staged\pc1_tls_from_dut_$(Get-Date -Format 'yyyy-MM-dd_HHmmss')"
}
New-Item -ItemType Directory -Force -Path $Dest | Out-Null

Write-Host "Pull DUT TLS (all PEM/key) from ${HostAddr} -> $Dest" -ForegroundColor Cyan

$dutVer = (& $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch "ccli --version | head -1").Trim()

$remoteListRaw = & $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch `
    "ls -1 /etc/ccli/tls/*.pem /etc/ccli/tls/*.key 2>/dev/null | xargs -n1 basename 2>/dev/null || true"
$files = @($remoteListRaw -split "`n" | ForEach-Object { $_.Trim() } | Where-Object { $_ -match '\.(pem|key)$' } | Sort-Object -Unique)

if ($files.Count -eq 0) {
    throw "No PEM/key files listed on DUT /etc/ccli/tls - check SSH and deploy"
}

$md5Cmd = "cd /etc/ccli/tls; md5sum " + ($files -join " ")
$md5Lines = & $Plink -ssh "root@$HostAddr" -pw $pw -hostkey $hk -batch $md5Cmd

$pulled = @()
foreach ($f in $files) {
    $remote = "root@${HostAddr}:/etc/ccli/tls/$f"
    $local = Join-Path $Dest $f
    & $Pscp -scp -batch -pw $pw -hostkey $hk $remote $local 2>$null
    if (Test-Path $local) {
        $pulled += $f
        Write-Host "  got $f"
    } else {
        Write-Warning "  skip (pull failed): $f"
    }
}

# TSP OpenSSL filename aliases (same bytes as on DUT)
function Ensure-Alias([string]$From, [string]$To) {
    $src = Join-Path $Dest $From
    $dst = Join-Path $Dest $To
    if ((Test-Path $src) -and -not (Test-Path $dst)) {
        Copy-Item -Force $src $dst
        Write-Host "  alias $To (copy of $From)"
    }
}
Ensure-Alias "client.key" "client_tsp.key"
Ensure-Alias "revoked.key" "revoked_tsp.key"
Ensure-Alias "revoked_acse.key" "revoked_acse_tsp.key"
if (-not (Test-Path (Join-Path $Dest "client_acse_tsp.key")) -and (Test-Path (Join-Path $Dest "client_acse.key"))) {
    Ensure-Alias "client_acse.key" "client_acse_tsp.key"
}

$entries = @()
foreach ($line in $md5Lines) {
    if ($line -match '^([0-9a-f]{32})\s+(.+)$') {
        $entries += [ordered]@{ file = $Matches[2].Trim(); md5 = $Matches[1].ToLower() }
    }
}
foreach ($f in (Get-ChildItem $Dest -File | Sort-Object Name)) {
    if ($entries.file -contains $f.Name) { continue }
    $entries += [ordered]@{
        file = $f.Name
        md5  = (Get-FileHash $f.FullName -Algorithm MD5).Hash.ToLower()
    }
}

$readme = @(
    "# CCLI Test Suite Pro - complete TLS from DUT"
    ""
    "DUT: $dutVer"
    "Source: /etc/ccli/tls/ on $HostAddr (every *.pem and *.key on device)"
    ""
    "## T1-01 Connect (browse these five files in TSP)"
    "  Trusted CA              -> root_CA.pem"
    "  Transport certificate   -> client_tls.pem"
    "  Transport private key   -> client_tsp.key"
    "  MMS Security certificate -> client.pem"
    "  MMS Security private key -> client_acse.key"
    ""
    "Do NOT put client_tsp.key on the MMS Security tab."
    ""
    "IED: CCI016_01 @ 192.168.10.1:3782  TLS ON  prefer TLS 1.2"
    "Re-browse every cert path in TSP after copy (cached paths break Connect)."
    ""
    "## Tier 3 / negatives (same folder)"
    "  Viewer: viewer.pem + viewer.key (+ viewer_tls.pem)"
    "  Revoked T3-04: revoked_tls.pem + revoked.pem + revoked_tsp.key + revoked_acse.key + lab_crl.pem"
    "  DUT only (not TSP Connect): server*.pem, server*.key, root_CA.key"
    ""
    "Preflight: scripts/dut-mms-tsp-preflight.ps1"
    "Verify: scripts/verify-tsp-tls-key-match.ps1 -TlsDir this folder"
) -join "`r`n"
Set-Content -Path (Join-Path $Dest "TSP_TLS_COMPLETE_README.txt") -Value $readme -Encoding UTF8

$manifest = [ordered]@{
    source = "DUT /etc/ccli/tls/ (all PEM + KEY on device)"
    dut_host = $HostAddr
    dut_ccli = $dutVer
    scope = "Complete TSP + lab PKI - USB/WhatsApp to PC1 only; never git commit"
    tsp_connect_uses = "root_CA.pem, client_tls.pem+client_tsp.key, client.pem+client_acse.key"
    generated_utc = (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss") + " UTC"
    files = ($entries | Sort-Object { $_.file })
}
$manifest | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $Dest "DUT_TLS_MANIFEST.json") -Encoding UTF8

Copy-Item -Force (Join-Path $RepoRoot "lab\testpro\MASOMEH_CONNECT_FIX_2026-10-14.md") (Join-Path $Dest "READ_ME_CONNECT_FIX.md") -ErrorAction SilentlyContinue

Write-Host ""
Write-Host "Pulled $($pulled.Count) files from DUT with TSP aliases. Folder: $Dest" -ForegroundColor Green
