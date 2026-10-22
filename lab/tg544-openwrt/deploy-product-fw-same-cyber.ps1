# Deploy product ccli (r55) + 5-LN bench yaml — DUT TLS = p3_07_connect (same as r51 TestPro, no TSP changes).
# See DEPLOY_PRODUCT_KEEP_TSP.md
#
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File lab\tg544-openwrt\deploy-product-fw-same-cyber.ps1

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$LabYaml = "",
    [switch]$TlsOnly,
    [switch]$ReplaceBinary
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$SessionFix = Join-Path $PSScriptRoot "deploy-ccli-session-fix.ps1"
if ([string]::IsNullOrWhiteSpace($LabYaml)) {
    $Yaml = Join-Path $Repo "apps\ccli\config\product_tr400_p3_07_connect.yaml"
} elseif (Test-Path $LabYaml) {
    $Yaml = (Resolve-Path $LabYaml).Path
} else {
    $Yaml = Join-Path $Repo $LabYaml
}
$TlsDir = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Source lab\tg544-openwrt\lab-env.ps1 first."
}

# Default: keep DUT r51 Connect-PASS ELF. r55 ccli-bin breaks TestPro Initiate.
if ($TlsOnly -or -not $ReplaceBinary) {
    Write-Host "=== Product yaml + P3-07 TLS only (keep DUT binary) ===" -ForegroundColor Cyan
    Write-Host "DUT Connect-PASS ELF pin: 7b05d7cc… r51. Pass -ReplaceBinary to upload ccli-bin (r55)."
    & powershell -NoProfile -File (Join-Path $PSScriptRoot "push-p3-07-tls-to-dut.ps1") -HostAddr $HostAddr
    & powershell -NoProfile -File (Join-Path $PSScriptRoot "push-lab-yaml-to-dut.ps1") -HostAddr $HostAddr -LabYaml $Yaml
    exit 0
}

$changes = @(
    "Product FW 0.1.0-r55 + 5-LN bench",
    "product_tr400_p3_07_connect.yaml",
    "TLS p3_07_connect (same as r51 Oct-08 Connect, crl off)",
    "Single ccli; iec61850service off"
)

Write-Host "=== Product FW + P3-07 cyber ===" -ForegroundColor Cyan
& $SessionFix -HostAddr $HostAddr -LabYaml $Yaml -TlsDir $TlsDir -Changes $changes -DeployProfile "PRODUCT-5LN-CERT"

$validate = Join-Path $Repo "scripts\validate-dut-p3-07-connect-alignment.ps1"
Write-Host "=== TLS pin check (PEMs only; yaml may differ for 5-LN) ===" -ForegroundColor Cyan
$prev = $ErrorActionPreference
$ErrorActionPreference = "Continue"
& powershell -NoProfile -File $validate -HostAddr $HostAddr 2>&1
$ErrorActionPreference = $prev

Write-Host "TestPro: no change if already on P3-07 Connect TLS (LAB_ACCESS_FREEZE.md MD5 pins)." -ForegroundColor Green
Write-Host "DUT must show crl=off and root_CA MD5 7ada0cbe... after deploy."
