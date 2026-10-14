# Stage CCLI TSP field pack and copy to PC1 (Test Suite Pro machine).
#
# Usage (from repo root on Yahia/dev PC):
#   powershell -File scripts\push-tsp-field-pack-to-pc1.ps1
#   powershell -File scripts\push-tsp-field-pack-to-pc1.ps1 -RemoteDest "\\PC1\D$\CCLI_TSP_FIELD_PACK"
#   powershell -File scripts\push-tsp-field-pack-to-pc1.ps1 -RemoteDest "\\192.168.10.10\D$\CCLI_TSP_FIELD_PACK"
#
# Requires: admin share or shared folder reachable from this machine; PC1 powered on same LAN.

param(
    [string]$LocalStaging = "C:\CCLI_TSP_FIELD_PACK",
    [string]$RemoteDest = "",
    [switch]$StageOnly,
    [switch]$WhatIf
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot

Write-Host "Staging field pack -> $LocalStaging" -ForegroundColor Cyan
& (Join-Path $Repo "scripts\stage-tsp-field-pack.ps1") -Dest $LocalStaging

# Masomeh quick-start into pack
$startGuide = Join-Path $Repo "handoff\masomeh95\TSP_PC1_START_MASOMEH.md"
if (Test-Path $startGuide) {
    Copy-Item -Force $startGuide (Join-Path $LocalStaging "00_START_PC1_MASOMEH.md")
    Write-Host "  Added 00_START_PC1_MASOMEH.md"
}

$validator = Join-Path $Repo "scripts\validate-5ln-cid.ps1"
if (Test-Path $validator) {
    New-Item -ItemType Directory -Force -Path (Join-Path $LocalStaging "scripts") | Out-Null
    Copy-Item -Force $validator (Join-Path $LocalStaging "scripts\validate-5ln-cid.ps1")
}

if ($StageOnly) {
    Write-Host "StageOnly: pack ready at $LocalStaging" -ForegroundColor Green
    Write-Host "Copy USB or run again with -RemoteDest \\PC1\D$\CCLI_TSP_FIELD_PACK"
    exit 0
}

if (-not $RemoteDest) {
    $candidates = @(
        "\\PC1\D$\CCLI_TSP_FIELD_PACK",
        "\\PC1\C$\CCLI_TSP_FIELD_PACK",
        "\\TSP-PC\D$\CCLI_TSP_FIELD_PACK",
        "\\192.168.10.10\D$\CCLI_TSP_FIELD_PACK"
    )
    foreach ($c in $candidates) {
        $parent = Split-Path $c -Parent
        if (Test-Path $parent -ErrorAction SilentlyContinue) {
            $RemoteDest = $c
            Write-Host "Auto-selected remote: $RemoteDest" -ForegroundColor Yellow
            break
        }
    }
}

if (-not $RemoteDest) {
    Write-Host ""
    Write-Host "No -RemoteDest and no reachable PC1 admin share." -ForegroundColor Yellow
    Write-Host "Manual: copy folder to USB -> PC1 -> D:\CCLI_TSP_FIELD_PACK"
    Write-Host "  Source: $LocalStaging"
    Write-Host "  Guide:  handoff\masomeh95\TSP_PC1_START_MASOMEH.md"
    exit 0
}

$remoteParent = Split-Path $RemoteDest -Parent
if (-not (Test-Path $remoteParent)) {
    throw "Cannot reach remote parent: $remoteParent (check PC name, firewall, credentials)"
}

Write-Host "Robocopy -> $RemoteDest" -ForegroundColor Cyan
$roboArgs = @(
    $LocalStaging, $RemoteDest,
    "/MIR", "/Z", "/FFT", "/R:2", "/W:3",
    "/XD", ".git"
)
if ($WhatIf) { $roboArgs += "/L" }
& robocopy @roboArgs | Out-Host
$rc = $LASTEXITCODE
if ($rc -ge 8) {
    throw "Robocopy failed with exit code $rc"
}

Write-Host ""
Write-Host "OK: field pack on PC1 at $RemoteDest" -ForegroundColor Green
Write-Host "Masomeh: open 00_START_PC1_MASOMEH.md then 00_UNIFIED_CERT_TEST_MATRIX.md"
