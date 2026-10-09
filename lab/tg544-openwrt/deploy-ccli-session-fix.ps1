# Deploy ccli-bin + lab TLS to TG544 (versioned manifest required).
# Usage:
#   $env:CCLI_TG544_PW = "root-password"
#   .\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 -Changes @(
#     "CS104 on 192.168.1.130:2404",
#     "lab_tr400_phase4_eth_b.yaml"
#   )
# LAN2 Eth_B deploy:
#   .\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 -HostAddr 192.168.1.130 -Changes @("...")

param(
    [Parameter(Mandatory = $true)]
    [string[]]$Changes,
    [string]$HostAddr = "192.168.10.1",
    [string]$User = "root",
    [string]$LabYaml = "",
    [string]$TlsDir = "",
    [switch]$SkipManifest
)

$ErrorActionPreference = "Stop"
# TG544 same host key on all role interfaces (LAN1/LAN2/LAN3).
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$Bin = Join-Path $PSScriptRoot "ccli-bin"
$DeploySh = Join-Path $PSScriptRoot "deploy-ccli-dut.sh"
$ManifestScript = Join-Path $PSScriptRoot "write-deploy-manifest.ps1"
$RepoRoot = Split-Path $PSScriptRoot -Parent | Split-Path -Parent
$VersionFile = Join-Path $RepoRoot "apps\ccli\VERSION"
if (-not $TlsDir) {
    $TlsDir = Join-Path $RepoRoot "apps\ccli\config\tls"
}

if (-not $LabYaml) {
    $LabYaml = Join-Path $RepoRoot "apps\ccli\config\lab_tr400_phase4_eth_b.yaml"
}

if (-not $env:CCLI_TG544_PW) { Write-Error "Set `$env:CCLI_TG544_PW then re-run." }
if (-not (Test-Path $Bin)) { Write-Error "Missing $Bin - run wsl-build-ccli.sh first." }
if (-not (Test-Path $Plink)) { Write-Error "Install PuTTY plink." }
if (-not (Test-Path (Join-Path $TlsDir "server.pem"))) {
    Write-Error "Missing TLS in $TlsDir - run: powershell -File scripts/provision-ejbca-ccli-pki.ps1  (or gen_lab_pki.py)"
}
if (-not (Test-Path $VersionFile)) { Write-Error "Missing $VersionFile" }

$vlines = Get-Content $VersionFile | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne "" }
$expectedFull = "$($vlines[0])-r$($vlines[1])"

Write-Host "=== Deploy target version: $expectedFull ===" -ForegroundColor Cyan

Write-Host "=== Verify binary version (TG544 aarch64, no local exec on x86) ===" -ForegroundColor Cyan
$wslBin = ($Bin -replace '\\', '/')
if ($wslBin -match '^([A-Za-z]):(.*)$') {
    $wslBin = ('/mnt/{0}{1}' -f $Matches[1].ToLower(), $Matches[2])
}
$verOk = $false
$prevEap = $ErrorActionPreference
$ErrorActionPreference = "SilentlyContinue"
$verOut = wsl bash -lc "'$wslBin' --version" 2>&1 | Out-String
if ($verOut -match [regex]::Escape($expectedFull)) {
    Write-Host $verOut.Trim()
    $verOk = $true
} else {
    $grepPat = "CCLI_VERSION_FULL|$expectedFull"
    $embedded = wsl bash -lc "strings '$wslBin' | grep -E '$grepPat' | head -5" 2>&1 | Out-String
    Write-Host "strings:" $embedded.Trim()
    if ($embedded -match [regex]::Escape($expectedFull)) {
        $verOk = $true
    }
}
$ErrorActionPreference = $prevEap
if (-not $verOk) {
    Write-Error "Binary version mismatch. Expected '$expectedFull' embedded in ccli-bin. Rebuild after editing apps/ccli/VERSION."
}

$manifestPath = $null
if (-not $SkipManifest) {
    $manifestPath = & $ManifestScript -Changes $Changes -Binary $Bin -LabYaml $LabYaml -HostAddr $HostAddr
}

$pw = $env:CCLI_TG544_PW

Write-Host "=== PC network ($HostAddr) ===" -ForegroundColor Cyan
Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
    Where-Object { $_.IPAddress -like "192.168.*" } |
    Select-Object IPAddress, InterfaceAlias
Test-NetConnection -ComputerName $HostAddr -WarningAction SilentlyContinue -ErrorAction SilentlyContinue |
    Select-Object PingSucceeded

Write-Host "=== Upload ccli-bin + TLS + deploy script ===" -ForegroundColor Cyan
& $Pscp -scp -batch -pw $pw -hostkey $HostKey $Bin "${User}@${HostAddr}:/tmp/ccli.new"
& $Pscp -scp -batch -pw $pw -hostkey $HostKey $DeploySh "${User}@${HostAddr}:/tmp/deploy-ccli-dut.sh"
if ($manifestPath -and (Test-Path $manifestPath)) {
    & $Pscp -scp -batch -pw $pw -hostkey $HostKey $manifestPath "${User}@${HostAddr}:/tmp/ccli.deploy.manifest"
}
& $Plink -ssh "${User}@${HostAddr}" -pw $pw -hostkey $HostKey -batch "mkdir -p /tmp/ccli.tls"
foreach ($f in @(
        "root_CA.pem", "server.pem", "server_tls.pem", "server.key", "server_acse.key",
        "server_combined.pem", "server_combined.key",
        "client.pem", "client_tls.pem", "client.key",
        "viewer.pem", "viewer_tls.pem", "viewer.key",
        "revoked.pem", "revoked_tls.pem", "lab_crl.pem"
    )) {
    $src = Join-Path $TlsDir $f
    if (Test-Path $src) {
        & $Pscp -scp -batch -pw $pw -hostkey $HostKey $src "${User}@${HostAddr}:/tmp/ccli.tls/$f"
    }
}

if (Test-Path $LabYaml) {
    Write-Host "Lab config: $LabYaml" -ForegroundColor Cyan
    & $Pscp -scp -batch -pw $pw -hostkey $HostKey $LabYaml "${User}@${HostAddr}:/etc/ccli/lab.yaml"
} else {
    Write-Error "Lab yaml not found: $LabYaml"
}

$ModelCfg = Join-Path $RepoRoot "apps\ccli\config\icd\lab_tg544_eth_a.cfg"
if (Test-Path $ModelCfg) {
    Write-Host "MMS model cfg: $ModelCfg" -ForegroundColor Cyan
    & $Plink -ssh "${User}@${HostAddr}" -pw $pw -hostkey $HostKey -batch "mkdir -p /etc/ccli/icd"
    & $Pscp -scp -batch -pw $pw -hostkey $HostKey $ModelCfg "${User}@${HostAddr}:/etc/ccli/icd/lab_tg544_eth_a.cfg"
}

Write-Host "=== Install + start on DUT ===" -ForegroundColor Cyan
& $Plink -ssh "${User}@${HostAddr}" -pw $pw -hostkey $HostKey -batch `
    "sed -i 's/\r$//' /tmp/deploy-ccli-dut.sh; chmod +x /tmp/deploy-ccli-dut.sh; sh /tmp/deploy-ccli-dut.sh"

if ($HostAddr -eq "192.168.10.1") {
    Write-Host "=== Sync PC TSP staging (C:\CCLI_tls + C:\CCLI_product_tls) ===" -ForegroundColor Cyan
    foreach ($TspTls in @("C:\CCLI_tls", "C:\CCLI_product_tls")) {
        New-Item -ItemType Directory -Force -Path $TspTls | Out-Null
        foreach ($f in @(
                "root_CA.pem", "client.pem", "client.key", "client_tls.pem", "client_tsp.key",
                "viewer.pem", "viewer.key", "revoked.pem", "revoked.key", "lab_crl.pem"
            )) {
            $src = Join-Path $TlsDir $f
            if (Test-Path $src) {
                Copy-Item -Force $src (Join-Path $TspTls $f)
            }
        }
        if (-not (Test-Path (Join-Path $TspTls "client_tsp.key"))) {
            Copy-Item -Force (Join-Path $TlsDir "client.key") (Join-Path $TspTls "client_tsp.key")
        }
        Write-Host "  staged -> $TspTls"
    }
}

Write-Host "=== Done ===" -ForegroundColor Green
Write-Host "DUT version: ssh $User@${HostAddr} '/usr/sbin/ccli --version'"
if ($manifestPath) {
    Write-Host "Manifest:    $manifestPath"
}
