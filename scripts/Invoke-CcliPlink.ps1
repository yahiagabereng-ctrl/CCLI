# Shared PuTTY plink/pscp args: SSH key (preferred) or lab password.
# Usage:
#   . .\scripts\Invoke-CcliPlink.ps1
#   Invoke-CcliPlink -HostAddr 192.168.1.130 -Command "ccli --version"

function Get-CcliSshHostKey {
    if ($env:CCLI_TG544_HOSTKEY) { return $env:CCLI_TG544_HOSTKEY }
    return "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
}

function Get-CcliPlinkExe {
    $p = "C:\Program Files\PuTTY\plink.exe"
    if (-not (Test-Path $p)) { throw "PuTTY plink not found at $p" }
    return $p
}

function Get-CcliPscpExe {
    $p = "C:\Program Files\PuTTY\pscp.exe"
    if (-not (Test-Path $p)) { throw "PuTTY pscp not found at $p" }
    return $p
}

function Add-CcliPlinkAuth {
    param([string[]]$BaseArgs)

    if ($env:CCLI_SSH_KEY -and (Test-Path $env:CCLI_SSH_KEY)) {
        return $BaseArgs + @("-i", $env:CCLI_SSH_KEY)
    }
    if ($env:CCLI_TG544_PW) {
        return $BaseArgs + @("-pw", $env:CCLI_TG544_PW)
    }
    throw "Set CCLI_SSH_KEY (OpenSSH or .ppk) or CCLI_TG544_PW in lab-env.ps1 (see lab/LAB_ACCESS_FREEZE.md)"
}

function Invoke-CcliPlink {
    param(
        [string]$HostAddr,
        [string]$Command,
        [string]$User = "root"
    )
    $plink = Get-CcliPlinkExe
    $args = Add-CcliPlinkAuth @("-batch", "-hostkey", (Get-CcliSshHostKey))
    $prev = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $out = (& $plink @args -ssh "${User}@${HostAddr}" $Command 2>&1 | Out-String).Trim()
    $ErrorActionPreference = $prev
    return $out
}

function Invoke-CcliPscp {
    param(
        [string]$LocalPath,
        [string]$RemotePath,
        [string]$HostAddr,
        [string]$User = "root"
    )
    $pscp = Get-CcliPscpExe
    $local = (Resolve-Path -LiteralPath $LocalPath).Path
    $remote = "${User}@${HostAddr}:${RemotePath}"
    $args = Add-CcliPlinkAuth @("-batch", "-hostkey", (Get-CcliSshHostKey))
    & $pscp @args -scp $local $remote
    if ($LASTEXITCODE -ne 0) { throw "pscp failed ($LASTEXITCODE)" }
}
