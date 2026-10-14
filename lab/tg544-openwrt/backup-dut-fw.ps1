# Backup TG544 DUT firmware/config snapshot (LAN2 management / Eth_B path).
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   .\lab\tg544-openwrt\backup-dut-fw.ps1
# Optional: -DutAddr 192.168.1.130

param(
    [string]$DutAddr = "192.168.1.130",
    [string]$User = "root"
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$BackupRoot = Join-Path $RepoRoot "lab\evidence\dut-fw-backup"
$OutDir = Join-Path $BackupRoot $Stamp

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set `$env:CCLI_TG544_PW (see lab\tg544-openwrt\lab-env.ps1)."
}
if (-not (Test-Path $Plink)) {
    Write-Error "PuTTY plink not found at $Plink"
}
if (-not (Test-Path $Pscp)) {
    Write-Error "PuTTY pscp not found at $Pscp"
}

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$Manifest = Join-Path $OutDir "BACKUP_MANIFEST.md"

function Write-Manifest([string]$Line) {
    Add-Content -Path $Manifest -Value $Line -Encoding UTF8
}

Write-Manifest "# DUT firmware backup - $Stamp"
Write-Manifest ""
Write-Manifest '| Field | Value |'
Write-Manifest '|-------|--------|'
Write-Manifest "| DUT | ${User}@${DutAddr} (LAN2 / lab mgmt) |"
Write-Manifest "| Host key | $HostKey |"
Write-Manifest ""

$remoteInfo = @'
echo "=== ccli version ==="
/usr/sbin/ccli --version 2>/dev/null || /usr/bin/ccli --version 2>/dev/null || echo MISSING
echo "=== /etc/ccli/VERSION ==="
cat /etc/ccli/VERSION 2>/dev/null || echo MISSING
echo "=== binary sha256 ==="
sha256sum /usr/sbin/ccli 2>/dev/null || sha256sum /usr/bin/ccli 2>/dev/null || echo MISSING
echo "=== binary size ==="
ls -la /usr/sbin/ccli /usr/bin/ccli 2>/dev/null
echo "=== process ==="
ps w | grep -v grep | grep ccli || echo not_running
echo "=== listen ==="
netstat -tlnp 2>/dev/null | grep -E '3782|:102|:2404' || ss -tlnp 2>/dev/null | grep -E '3782|2404|102' || true
echo "=== opkg ccli ==="
opkg info ccli 2>/dev/null | head -20 || true
'@

$InfoFile = Join-Path $OutDir "dut_identity.txt"
Write-Host "Collecting DUT identity via plink..."
& $Plink -ssh "${User}@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $remoteInfo |
    Tee-Object -FilePath $InfoFile

$files = @(
    @{ Remote = "/usr/sbin/ccli"; Local = "ccli-bin-from-dut" },
    @{ Remote = "/etc/ccli/VERSION"; Local = "VERSION" },
    @{ Remote = "/etc/ccli/lab.yaml"; Local = "lab.yaml" },
    @{ Remote = "/etc/ccli/icd/lab_tg544_eth_a.cfg"; Local = "lab_tg544_eth_a.cfg" }
)

foreach ($f in $files) {
    $dest = Join-Path $OutDir $f.Local
    Write-Host "Fetching $($f.Remote) ..."
    & $Pscp -scp -pw $env:CCLI_TG544_PW -hostkey $HostKey "${User}@${DutAddr}:$($f.Remote)" $dest 2>&1 | Out-Host
    if (Test-Path $dest) {
        $hash = (Get-FileHash $dest -Algorithm SHA256).Hash.ToLowerInvariant()
        $size = (Get-Item $dest).Length
        Write-Manifest "| $($f.Local) | SHA256 $hash ; $size bytes |"
    } else {
        Write-Manifest "| $($f.Local) | FETCH FAILED |"
    }
}

# Local deploy artifact for diff
$LocalBin = Join-Path $RepoRoot "lab\tg544-openwrt\ccli-bin"
if (Test-Path $LocalBin) {
    $lbDest = Join-Path $OutDir "ccli-bin-from-repo-lab-tg544"
    Copy-Item $LocalBin $lbDest -Force
    $lh = (Get-FileHash $lbDest -Algorithm SHA256).Hash.ToLowerInvariant()
    Write-Manifest "| ccli-bin-from-repo-lab-tg544 | SHA256 $lh (PC copy at backup time) |"
}

$ExpectedR52 = "1df82c26243021a86c533a179781c749fc852c2f724efa3b0aa4018b2969eca1"
Write-Manifest ""
Write-Manifest "## Reference (lab pin)"
Write-Manifest "- Specimen: lab/conformance/LAB_SPECIMEN_VERSION.md -> 0.1.0-r52"
Write-Manifest "- Deploy manifest SHA (r52): $ExpectedR52"
Write-Manifest ""

# Pack-up mirror (lab/evidence/pacb-backup)
$PackUp = Join-Path $RepoRoot "lab\evidence\pacb-backup"
New-Item -ItemType Directory -Force -Path $PackUp | Out-Null
$stampName = Split-Path $OutDir -Leaf
Set-Content -Path (Join-Path $PackUp "LATEST.txt") -Encoding UTF8 -Value @(
    $stampName
    "Full path: lab/evidence/dut-fw-backup/$stampName/"
    "Manifest: lab/evidence/dut-fw-backup/$stampName/BACKUP_MANIFEST.md"
)
foreach ($name in @("ccli-bin-from-dut", "VERSION", "lab.yaml", "lab_tg544_eth_a.cfg", "BACKUP_MANIFEST.md", "dut_identity.txt")) {
    $p = Join-Path $OutDir $name
    if (Test-Path $p) {
        Copy-Item $p (Join-Path $PackUp $name) -Force
    }
}

Write-Host "`nBackup complete: $OutDir"
Write-Host "Manifest: $Manifest"
Write-Host "Pack-up mirror: $PackUp"
