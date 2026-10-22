# Apply EMT432 Rogowski 85 mV/kA on meter (40235=0.000085) + push lab yaml to DUT.
# (Filename deploy-emt432-80mv.ps1 kept for existing runbooks.)
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1
#   powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1 -DutHost 192.168.178.1   # SSH via plant LAN3
#   powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1              # yaml + ccli restart only
#   powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1 -ApplyCertPin  # also write meter 40232/40235
#   powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1 -ApplyCertPin -SensitivityMvKa 100

param(
    [string]$MeterHost = "192.168.178.10",
    [string]$DutHost = "192.168.1.130",
    [switch]$ApplyCertPin,
    [switch]$SkipMeterWrite,
    [switch]$UseSshTunnel,
    [double]$SensitivityMvKa = 85
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$LabYaml = Join-Path $RepoRoot "apps\ccli\config\lab_tr400_emt432_lan3_tcp_102.yaml"
$RogowskiScript = Join-Path $RepoRoot "lab\emt432_rogowski_config.py"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"

if (-not (Test-Path $LabYaml)) { throw "Missing $LabYaml" }

function Test-PingHost([string]$Target) {
    try {
        $p = New-Object System.Net.NetworkInformation.Ping
        return ($p.Send($Target, 1500).Status -eq "Success")
    } catch {
        return $false
    }
}

function Invoke-MeterCertPin {
    param([string]$ProbeHost, [int]$ProbePort)
    python $RogowskiScript --host $ProbeHost --port $ProbePort --apply-cert-pin `
        --set-sensitivity-mvka $SensitivityMvKa
}

# Back-compat: -SkipMeterWrite still disables meter; default is yaml-only unless -ApplyCertPin.
$doMeterWrite = $ApplyCertPin -and -not $SkipMeterWrite

if ($doMeterWrite) {
    Write-Host "=== EMT432 cert register pin (EMT432_CERT_REGISTER_PIN.md) ===" -ForegroundColor Cyan
    $meterOk = $false
    if (Test-PingHost $MeterHost) {
        Invoke-MeterCertPin -ProbeHost $MeterHost -ProbePort 502
        $meterOk = ($LASTEXITCODE -eq 0)
    }
    if (-not $meterOk -and ($UseSshTunnel -or -not (Test-PingHost $MeterHost))) {
        if ($env:CCLI_TG544_PW -and (Test-Path $Plink) -and (Test-PingHost $DutHost)) {
            Write-Host "Direct meter unreachable; SSH tunnel via $DutHost to ${MeterHost}:502 on 127.0.0.1:15002" -ForegroundColor Yellow
            $tunnel = Start-Process -FilePath $Plink -PassThru -WindowStyle Hidden -ArgumentList @(
                "-batch", "-hostkey", $HostKey, "-pw", $env:CCLI_TG544_PW,
                "-L", "15002:${MeterHost}:502", "-N", "root@${DutHost}"
            )
            Start-Sleep -Seconds 2
            try {
                Invoke-MeterCertPin -ProbeHost "127.0.0.1" -ProbePort 15002
                $meterOk = ($LASTEXITCODE -eq 0)
            } finally {
                if ($tunnel -and -not $tunnel.HasExited) {
                    Stop-Process -Id $tunnel.Id -Force -ErrorAction SilentlyContinue
                }
            }
        }
    }
    if (-not $meterOk) {
        Write-Warning "Meter cert pin failed. Use LAN3 cable, or lab-env + this script (auto-tunnel from LAN2)."
        throw "EMT432 cert register pin not applied; fix before cert session."
    }
} else {
    Write-Host "=== Skip meter Modbus writes (yaml + ccli only) ===" -ForegroundColor Yellow
    Write-Host "  Use -ApplyCertPin when you must write 40232/40235. Rewriting 40235 can zero P/I until load/UI settle."
}

Write-Host "=== Push lab.yaml to DUT ===" -ForegroundColor Cyan
& powershell -NoProfile -File (Join-Path $PSScriptRoot "push-lab-yaml-to-dut.ps1") `
    -HostAddr $DutHost `
    -LabYaml $LabYaml
if ($LASTEXITCODE -ne 0) {
    Write-Error "DUT yaml push failed (exit $LASTEXITCODE). Check SSH to $DutHost or use -DutHost 192.168.178.1 on LAN3."
}
