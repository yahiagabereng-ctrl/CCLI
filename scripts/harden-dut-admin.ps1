# Disable LuCI (uhttpd), TesPro mmsd; install SSH pubkey; optionally disable dropbear password auth.
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\harden-dut-admin.ps1
#   powershell -File scripts\harden-dut-admin.ps1 -DisablePasswordAuth
#   powershell -File scripts\harden-dut-admin.ps1 -PubKeyPath C:\Users\you\.ssh\ccli_lab_ed25519.pub
#
# Requires CCLI_SSH_KEY or CCLI_TG544_PW for upload. Use -DisablePasswordAuth only after key login works.

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$PubKeyPath = "",
    [switch]$DisablePasswordAuth,
    [switch]$WhatIf
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
. (Join-Path $Repo "scripts\Invoke-CcliPlink.ps1")

if (-not $PubKeyPath) {
    if ($env:CCLI_SSH_PUBKEY -and (Test-Path $env:CCLI_SSH_PUBKEY)) {
        $PubKeyPath = $env:CCLI_SSH_PUBKEY
    } elseif ($env:CCLI_SSH_KEY) {
        $key = $env:CCLI_SSH_KEY
        if ($key -match '\.ppk$') {
            throw "Provide OpenSSH .pub via -PubKeyPath or CCLI_SSH_PUBKEY (PuTTY .ppk has no inline pubkey)."
        }
        $guess = [System.IO.Path]::ChangeExtension($key, ".pub")
        if (Test-Path $guess) { $PubKeyPath = $guess }
    }
}

$ShLocal = Join-Path $Repo "lab\tg544-openwrt\harden-dut-admin.sh"
if (-not (Test-Path $ShLocal)) { throw "Missing $ShLocal" }

Write-Host "=== DUT admin harden @ $HostAddr ===" -ForegroundColor Cyan
if ($DisablePasswordAuth -and -not $PubKeyPath) {
    throw "-DisablePasswordAuth requires -PubKeyPath or CCLI_SSH_PUBKEY"
}

if ($WhatIf) {
    Write-Host "[WhatIf] LuCI off, mmsd off, pubkey=$PubKeyPath, password_auth=$(if ($DisablePasswordAuth) { 'off' } else { 'keep' })"
    exit 0
}

Invoke-CcliPscp -LocalPath $ShLocal -RemotePath "/tmp/harden-dut-admin.sh" -HostAddr $HostAddr
if ($PubKeyPath) {
    if (-not (Test-Path $PubKeyPath)) { throw "PubKey not found: $PubKeyPath" }
    Invoke-CcliPscp -LocalPath $PubKeyPath -RemotePath "/tmp/ccli_admin.pub" -HostAddr $HostAddr
}

$pwAuth = if ($DisablePasswordAuth) { "off" } else { "keep" }
$cmd = "sed -i 's/\r$//' /tmp/harden-dut-admin.sh; chmod +x /tmp/harden-dut-admin.sh; CCLI_DROPBEAR_PASSWORD_AUTH=$pwAuth sh /tmp/harden-dut-admin.sh"
$out = Invoke-CcliPlink -HostAddr $HostAddr -Command $cmd
Write-Host $out
if ($out -notmatch 'HARDEN_DUT_ADMIN_DONE') {
    Write-Host "WARN: harden script may have failed — check output" -ForegroundColor Yellow
    exit 1
}
Write-Host "PASS: LuCI disabled; verify SSH key login before -DisablePasswordAuth on production." -ForegroundColor Green
