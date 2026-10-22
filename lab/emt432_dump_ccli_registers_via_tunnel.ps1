# PC review: read all CCLI-used EMT432 holding regs via Modbus (tunnel through TG544 LAN3).
# Not an interactive DUT SSH session — tunnel + python only.
param(
    [string]$DutHost = "192.168.1.130",
    [string]$MeterHost = "192.168.178.10",
    [int]$LocalPort = 15002,
    [switch]$FullRms
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path $PSScriptRoot -Parent
. (Join-Path $RepoRoot "lab\tg544-openwrt\lab-env.ps1")

$plink = "C:\Program Files\PuTTY\plink.exe"
$hostkey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$py = Join-Path $RepoRoot "lab\emt432_rogowski_config.py"
$out = Join-Path $RepoRoot "lab\evidence\phase5\EMT432_CCLI_REGISTER_SNAPSHOT_LATEST.md"

$tunnel = Start-Process -FilePath $plink -PassThru -WindowStyle Hidden -ArgumentList @(
    "-batch", "-hostkey", $hostkey, "-pw", $env:CCLI_TG544_PW,
    "-L", "${LocalPort}:${MeterHost}:502", "-N", "root@${DutHost}"
)
Start-Sleep -Seconds 3
try {
    $args = @("--host", "127.0.0.1", "--port", "$LocalPort", "--dump-ccli-registers", "--out", $out)
    if ($FullRms) { $args += "--full-rms" }
    & python $py @args
    exit $LASTEXITCODE
} finally {
    if ($tunnel -and -not $tunnel.HasExited) {
        Stop-Process -Id $tunnel.Id -Force -ErrorAction SilentlyContinue
    }
}
