# Deploy regulation mock UI to TesPro TG544 (uhttpd + CGI + ccli bench API)
# Usage:
#   $env:CCLI_TG544_PW = "password"
#   .\lab\tg544-openwrt\deploy-regulation-ui.ps1
# Browser: http://192.168.1.130/ccli/mock_console.html

param(
    [string]$HostAddr = "192.168.1.130",
    [string]$User = "root"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path $PSScriptRoot -Parent
$Www = Join-Path $PSScriptRoot "www\ccli"
$Cgi = Join-Path $PSScriptRoot "cgi-bin\ccli-bench"
$Bin = Join-Path $PSScriptRoot "ccli-bin"
$Yaml = Join-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) "apps\ccli\config\lab_tr400_phase1_regulation.yaml"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$HostKey = "SHA256:2hiPouwqC1oxP//Q0BpvgIcqD6IG+pqLkzNih1EpNRE"

if (-not $env:CCLI_TG544_PW) { Write-Error 'Set $env:CCLI_TG544_PW' }

$pw = $env:CCLI_TG544_PW
$plinkArgs = @("-ssh", "${User}@${HostAddr}", "-pw", $pw, "-hostkey", $HostKey, "-batch")

Write-Host "=== Upload ccli binary (bench-status API) ==="
if (Test-Path $Bin) {
    & $Plink @plinkArgs "/etc/init.d/ccli stop; killall ccli 2>/dev/null; sleep 1"
    & $Pscp -scp -pw $pw -hostkey $HostKey $Bin "${User}@${HostAddr}:/usr/sbin/ccli"
}

Write-Host "=== Upload www + CGI ==="
& $Plink @plinkArgs "mkdir -p /www/ccli /www/cgi-bin"
& $Pscp -scp -pw $pw -hostkey $HostKey "$Www\index.html" "${User}@${HostAddr}:/www/ccli/index.html"
& $Pscp -scp -pw $pw -hostkey $HostKey "$Www\mock_console.html" "${User}@${HostAddr}:/www/ccli/mock_console.html"
& $Pscp -scp -pw $pw -hostkey $HostKey $Cgi "${User}@${HostAddr}:/www/cgi-bin/ccli-bench"
& $Pscp -scp -pw $pw -hostkey $HostKey $Yaml "${User}@${HostAddr}:/etc/ccli/lab.yaml"

& $Plink @plinkArgs "mkdir -p /www/ccli /www/cgi-bin /var/run /etc/ccli"
& $Plink @plinkArgs "chmod +x /usr/sbin/ccli /www/cgi-bin/ccli-bench"
& $Plink @plinkArgs "sed 's/\r$//' /www/cgi-bin/ccli-bench > /tmp/cb && mv /tmp/cb /www/cgi-bin/ccli-bench && chmod +x /www/cgi-bin/ccli-bench"
& $Plink @plinkArgs "/etc/init.d/uhttpd restart 2>/dev/null; /etc/init.d/ccli restart 2>/dev/null; sleep 2; /usr/sbin/ccli --bench-status --json | head -c 200; echo"

Write-Host ""
Write-Host "Open: http://${HostAddr}/ccli/mock_console.html"
Write-Host "API:  http://${HostAddr}/cgi-bin/ccli-bench?action=status"
