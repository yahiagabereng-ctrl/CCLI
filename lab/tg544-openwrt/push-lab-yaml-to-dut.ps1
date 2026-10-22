# Push lab.yaml only — no ccli binary upgrade (keep DUT firmware version).
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File lab\tg544-openwrt\push-lab-yaml-to-dut.ps1 `
#     -LabYaml apps\ccli\config\lab_tr400_phase4_emt432_lan3_tcp.yaml

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$User = "root",
    [string]$LabYaml = "",
    [switch]$Push5LnCfg
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"

if (-not $LabYaml) {
    # Product bench + unchanged TestPro (P3-07 TLS)
    $LabYaml = Join-Path $RepoRoot "apps\ccli\config\product_tr400_p3_07_connect.yaml"
} elseif (-not [System.IO.Path]::IsPathRooted($LabYaml)) {
    $LabYaml = Join-Path $RepoRoot $LabYaml
}

if (-not $env:CCLI_TG544_PW) { Write-Error "Source lab-env.ps1 first." }
if (-not (Test-Path $LabYaml)) { Write-Error "Missing $LabYaml" }
if (-not (Test-Path $Plink)) { Write-Error "Install PuTTY plink." }

Write-Host "=== Push yaml only -> $HostAddr ===" -ForegroundColor Cyan
Write-Host "Source: $LabYaml"

& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $LabYaml "${User}@${HostAddr}:/etc/ccli/lab.yaml"
if ($LASTEXITCODE -ne 0) {
    Write-Error "pscp failed (exit $LASTEXITCODE). Is DUT reachable at ${User}@${HostAddr}?"
}

if ($Push5LnCfg -or (Select-String -Path $LabYaml -Pattern "product_tr400_eth_a_5ln\.cfg" -Quiet)) {
    $pushCfg = Join-Path $RepoRoot "scripts\push-5ln-product-cfg-to-dut.ps1"
    if (Test-Path $pushCfg) {
        Write-Host "=== Ensure 5-LN .cfg on DUT ===" -ForegroundColor Cyan
        & powershell -NoProfile -File $pushCfg -HostAddr $HostAddr
    }
}

& $Plink -ssh "${User}@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "/etc/init.d/ccli stop; killall ccli 2>/dev/null; sleep 2; /etc/init.d/ccli start; sleep 4; /usr/sbin/ccli --version; grep backend /etc/ccli/lab.yaml; grep host /etc/ccli/lab.yaml; logread -e ccli | grep modbus | tail -4"
if ($LASTEXITCODE -ne 0) {
    Write-Error "plink failed (exit $LASTEXITCODE) after yaml upload."
}

Write-Host "Done. Binary unchanged; config is /etc/ccli/lab.yaml" -ForegroundColor Green
