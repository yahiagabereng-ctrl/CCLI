# Write deploy manifest BEFORE uploading ccli to TG544.
# Usage:
#   .\lab\tg544-openwrt\write-deploy-manifest.ps1 -Changes @(
#     "CS104 slave on 192.168.1.130:2404",
#     "lab yaml lab_tr400_phase4_eth_b.yaml"
#   )
# Optional: -LabYaml path, -Binary path (default lab/tg544-openwrt/ccli-bin)

param(
    [Parameter(Mandatory = $true)]
    [string[]]$Changes,
    [string]$Binary = "",
    [string]$LabYaml = "",
    [string]$HostAddr = "192.168.1.130",
    [string]$Operator = $env:USERNAME
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$VersionFile = Join-Path $RepoRoot "apps\ccli\VERSION"
$ManifestDir = Join-Path $PSScriptRoot "deploy-manifests"

if (-not $Binary) {
    $Binary = Join-Path $PSScriptRoot "ccli-bin"
}
if (-not $LabYaml) {
    $LabYaml = Join-Path $RepoRoot "apps\ccli\config\lab_tr400_phase4_eth_b.yaml"
}

if (-not (Test-Path $VersionFile)) { throw "Missing $VersionFile" }
if (-not (Test-Path $Binary)) { throw "Missing binary $Binary - build first (wsl-build-ccli.sh)" }

$vlines = Get-Content $VersionFile | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne "" }
$semver = $vlines[0]
$release = $vlines[1]
$codename = if ($vlines.Count -gt 2) { ($vlines[2..($vlines.Count - 1)] -join " ") } else { "" }
$full = "${semver}-r${release}"

$hash = (Get-FileHash -Algorithm SHA256 -Path $Binary).Hash.ToLower()
$size = (Get-Item $Binary).Length
$stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"

$embedded = ""
$wslBin = $Binary -replace '\\', '/' -replace '^C:', '/mnt/c' -replace '^c:', '/mnt/c'
try {
    $embedded = wsl bash -lc "/usr/bin/file '$wslBin' 2>/dev/null; '$wslBin' --version 2>/dev/null | head -6" 2>$null
} catch {
    $embedded = "(run on WSL/Linux host to capture --version output)"
}

$yamlName = Split-Path $LabYaml -Leaf
$manifestName = "CCLI_DEPLOY_${full}_${stamp}.md"
$manifestPath = Join-Path $ManifestDir $manifestName
New-Item -ItemType Directory -Force -Path $ManifestDir | Out-Null

$changeBullets = ($Changes | ForEach-Object { "- $_" }) -join "`n"

$body = @"
# CCLI deploy manifest — $full

| Field | Value |
|-------|-------|
| **Date (UTC)** | $(Get-Date -Format "yyyy-MM-dd HH:mm:ss") UTC |
| **Operator** | $Operator |
| **Target DUT** | TG544 @ $HostAddr |
| **Version** | **$full** |
| **Codename** | $codename |
| **Binary** | ``$Binary`` |
| **SHA256** | ``$hash`` |
| **Size (bytes)** | $size |
| **Lab yaml** | ``$yamlName`` |

## Changes in this deploy

$changeBullets

## Binary identity (pre-upload)

``````
$embedded
``````

## Post-deploy checks (DUT)

``````bash
/usr/sbin/ccli --version
/usr/sbin/ccli --version-json
cat /etc/ccli/VERSION
ss -tlnp | grep -E '3782|2404'
grep -E '^(mms|iec104):' /etc/ccli/lab.yaml
tail -20 /tmp/ccli.log
``````

## Rollback

Previous manifest + matching ``ccli-bin`` in ``deploy-manifests/`` archive folder.

"@

Set-Content -Path $manifestPath -Value $body -Encoding UTF8
Write-Host "Manifest: $manifestPath" -ForegroundColor Green
Write-Host "Version:  $full ($codename)" -ForegroundColor Cyan
Write-Host "SHA256:   $hash"

# Machine-readable sidecar for deploy script
$jsonPath = $manifestPath -replace '\.md$', '.json'
@{
    version_full = $full
    semver       = $semver
    release      = [int]$release
    codename     = $codename
    sha256       = $hash
    binary       = $Binary
    lab_yaml     = $LabYaml
    host         = $HostAddr
    timestamp_utc = (Get-Date).ToUniversalTime().ToString("o")
    changes      = $Changes
} | ConvertTo-Json -Depth 4 | Set-Content -Path $jsonPath -Encoding UTF8

Write-Output $manifestPath
