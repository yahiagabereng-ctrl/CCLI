# One-shot: application config + admin harden (LuCI off, SSH pubkey).
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\apply-dut-secure-profile.ps1 -PubKeyPath $env:CCLI_SSH_PUBKEY

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$PubKeyPath = "",
    [switch]$DisablePasswordAuth
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot

& powershell -NoProfile -File (Join-Path $Repo "scripts\push-ccli-application-config.ps1") -HostAddr $HostAddr -RestartCcli
$hArgs = @{ HostAddr = $HostAddr }
if ($PubKeyPath) { $hArgs["PubKeyPath"] = $PubKeyPath }
if ($DisablePasswordAuth) { $hArgs["DisablePasswordAuth"] = $true }
& powershell -NoProfile -File (Join-Path $Repo "scripts\harden-dut-admin.ps1") @hArgs

Write-Host "Done: secure profile (see lab/CCLI_DUT_ADMIN_HARDENING.md)" -ForegroundColor Green
