# Verify EMT432 meter reads match ccli/MMS map per lab/emt432-meter-code.freeze.json
# Usage:
#   $env:CCLI_TG544_PW = '...'
#   powershell -File scripts/verify-emt432-meter-code-freeze.ps1

param(
    [string]$FreezeJson = "",
    [string]$DutAddr = "192.168.1.130",
    [string]$User = "root"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path $PSScriptRoot -Parent
if (-not $FreezeJson) {
    $FreezeJson = Join-Path $RepoRoot "lab\emt432-meter-code.freeze.json"
}
$Probe = Join-Path $RepoRoot "lab\modbus_emt432_tcp_probe.py"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"

$fr = Get-Content $FreezeJson -Raw | ConvertFrom-Json
Write-Host "=== EMT432 meter code freeze rev $($fr.revision) map_code=$($fr.mms_map_code_revision) ===" -ForegroundColor Cyan

foreach ($rel in $fr.sha256.PSObject.Properties) {
    $name = $rel.Name
    switch -Regex ($name) {
        '^product_tr400_p3_07_connect_5ln\.yaml$' {
            $path = Join-Path $RepoRoot "apps\ccli\config\$name"
        }
        '^product_tr400_eth_a_5ln\.cfg$' {
            $path = Join-Path $RepoRoot "apps\ccli\config\icd\product\$name"
        }
        default {
            $path = Join-Path $RepoRoot $name
        }
    }
    if (Test-Path $path) {
        $h = (Get-FileHash -Algorithm SHA256 $path).Hash.ToLower()
        if ($h -ne $rel.Value.ToLower()) {
            Write-Error "SHA256 mismatch $($rel.Name): got $h expected $($rel.Value)"
        }
        Write-Host "PASS sha256 $($rel.Name)"
    } else {
        Write-Warning "SKIP missing file for hash check: $($rel.Name)"
    }
}

if (-not $env:CCLI_TG544_PW) {
    Write-Warning "SKIP live compare (set CCLI_TG544_PW)"
    exit 0
}
if (-not (Test-Path $Plink)) { Write-Error "Install PuTTY plink" }

$meterHost = $fr.meter.host
$tunnelPort = 15002
$t = Start-Process -FilePath $Plink -PassThru -WindowStyle Hidden -ArgumentList @(
    "-batch", "-hostkey", $HostKey, "-pw", $env:CCLI_TG544_PW,
    "-L", "${tunnelPort}:${meterHost}:$($fr.meter.tcp_port)", "-N", "${User}@${DutAddr}"
)
Start-Sleep -Seconds 2
try {
    $probeOut = python $Probe --host 127.0.0.1 --port $tunnelPort 2>&1 | Out-String
    Write-Host $probeOut
    if ($probeOut -notmatch "VERDICT: PASS") {
        Write-Error "EMT432 probe failed"
    }
    $logLine = & $Plink -batch -hostkey $HostKey -pw $env:CCLI_TG544_PW "${User}@${DutAddr}" `
        "logread | grep 'modbus:' | tail -1" 2>&1 | Out-String
    Write-Host "DUT: $($logLine.Trim())"
    if ($logLine -notmatch 'AngA=') {
        Write-Warning "DUT log missing AngA= (is r64+ running?)"
    }
    Write-Host "PASS: live EMT432 + DUT modbus trace captured (manual compare or see evidence log)" -ForegroundColor Green
} finally {
    Stop-Process -Id $t.Id -Force -ErrorAction SilentlyContinue
}
