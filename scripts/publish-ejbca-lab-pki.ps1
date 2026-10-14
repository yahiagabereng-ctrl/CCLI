# End-to-end: EJBCA issue (CLI) -> CRL PEM -> sync to apps/ccli/config/tls -> optional DUT deploy.
#
# Usage:
#   powershell -File scripts\publish-ejbca-lab-pki.ps1
#   powershell -File scripts\publish-ejbca-lab-pki.ps1 -SkipIssue   # sync only
#   powershell -File scripts\publish-ejbca-lab-pki.ps1 -DeployDut   # needs lab-env + LAN2

param(
    [switch]$SkipIssue,
    [switch]$SkipDockerCheck,
    [switch]$DeployDut,
    [string]$HostAddr = "192.168.1.130"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Provision = Join-Path $Repo "scripts\provision-ejbca-ccli-pki.ps1"
$Sync = Join-Path $Repo "scripts\sync-ejbca-tls-to-product.ps1"

if (-not $SkipIssue) {
    Write-Host "=== EJBCA CLI issue + revoke + CRL ===" -ForegroundColor Cyan
    & $Provision -UseEjbcaCli -SkipDockerCheck:$SkipDockerCheck
    if ($LASTEXITCODE -ne 0) { throw "provision-ejbca-ccli-pki failed" }
}

Write-Host "`n=== Sync ejbca -> product tls ===" -ForegroundColor Cyan
& $Sync -Verify
if ($LASTEXITCODE -ne 0) { throw "sync-ejbca-tls-to-product failed" }

Write-Host "`n=== Stage PC1 field pack (local) ===" -ForegroundColor Cyan
& (Join-Path $Repo "scripts\setup-pc1-testpro-from-git.ps1") -Dest "C:\CCLI_TSP_FIELD_PACK"

if ($DeployDut) {
    if (-not $env:CCLI_TG544_PW) {
        Write-Warning "DeployDut skipped: source lab/tg544-openwrt/lab-env.ps1 first"
    } else {
        Write-Host "`n=== Deploy TLS to DUT ===" -ForegroundColor Cyan
        & (Join-Path $Repo "lab\tg544-openwrt\deploy-ccli-bench-lan2.ps1") -HostAddr $HostAddr
    }
}

Write-Host ""
Write-Host "Done. Commit apps/ccli/config/tls if PEMs changed; PC1: git pull + setup-pc1-testpro-from-git.ps1" -ForegroundColor Green
