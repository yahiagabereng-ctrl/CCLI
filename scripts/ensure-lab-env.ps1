# Create lab/tg544-openwrt/lab-env.ps1 from frozen example if missing.
# Usage: powershell -File scripts\ensure-lab-env.ps1 [-Force]

param([switch]$Force)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Example = Join-Path $Repo "lab\tg544-openwrt\lab-env.example.ps1"
$Local = Join-Path $Repo "lab\tg544-openwrt\lab-env.ps1"

if (-not (Test-Path $Example)) { throw "Missing $Example" }

if ((Test-Path $Local) -and -not $Force) {
    Write-Host "OK: $Local exists (use -Force to overwrite from example freeze)" -ForegroundColor Green
    exit 0
}

Copy-Item -Force $Example $Local
Write-Host "Created $Local from lab-env.example.ps1 (LAB_ACCESS_FREEZE)" -ForegroundColor Green
