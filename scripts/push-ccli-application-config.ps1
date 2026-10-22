# Push canonical apps/ccli/config/ccli_application.yaml to DUT and point UCI procd at it.
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\push-ccli-application-config.ps1
#   powershell -File scripts\push-ccli-application-config.ps1 -RestartCcli

param(
    [string]$HostAddr = "192.168.1.130",
    [switch]$RestartCcli
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
. (Join-Path $Repo "scripts\Invoke-CcliPlink.ps1")

$Src = Join-Path $Repo "apps\ccli\config\ccli_application.yaml"
if (-not (Test-Path $Src)) { throw "Missing $Src" }

$Remote = "/etc/ccli/ccli_application.yaml"
Write-Host "=== Push application config -> $Remote ===" -ForegroundColor Cyan
Invoke-CcliPscp -LocalPath $Src -RemotePath $Remote -HostAddr $HostAddr

$uci = @"
uci set ccli.main.config_file='$Remote'
uci commit ccli
ln -sf ccli_application.yaml /etc/ccli/lab.yaml 2>/dev/null || cp -f '$Remote' /etc/ccli/lab.yaml
grep -E '^  model_cfg:|bind_address|tcp_port:' '$Remote' | head -8
uci -q get ccli.main.config_file
"@

if ($RestartCcli) {
    $uci += @"

/etc/init.d/ccli restart 2>/dev/null || (/usr/sbin/ccli --config '$Remote' >>/tmp/ccli.log 2>&1 &)
sleep 3
/usr/sbin/ccli --version | head -1
netstat -tlnp 2>/dev/null | grep -E '3782|2404' || true
"@
}

$out = Invoke-CcliPlink -HostAddr $HostAddr -Command $uci
Write-Host $out
Write-Host "OK: single application config at $Remote" -ForegroundColor Green
