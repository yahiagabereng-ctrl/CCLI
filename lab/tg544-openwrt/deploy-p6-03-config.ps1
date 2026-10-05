# P6-03 — push lab.yaml with annex_m_trip_monitor and restart ccli (no binary rebuild).
param(
    [string]$DutAddr = "192.168.10.1",
    [string]$LabYaml = ""
)

$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent

if (-not $LabYaml) {
    $LabYaml = Join-Path $Repo "apps\ccli\config\lab_tr400_cleartext_tsp_ttyS1.yaml"
}
if (-not $env:CCLI_TG544_PW) { Write-Error "Set env:CCLI_TG544_PW" }
if (-not (Test-Path $LabYaml)) { Write-Error "Missing $LabYaml" }

$body = (Get-Content $LabYaml -Raw) -replace "`r`n", "`n"
$b64 = [Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($body))

Write-Host "=== P6-03 deploy lab.yaml ($LabYaml) -> $DutAddr ==="
& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "echo $b64 | openssl base64 -d -A > /etc/ccli/lab.yaml.new && mv /etc/ccli/lab.yaml.new /etc/ccli/lab.yaml && killall ccli 2>/dev/null; sleep 1; /usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 & sleep 3; grep annex_m /tmp/ccli.log | tail -5; tail -8 /tmp/ccli.log" 2>&1 |
    ForEach-Object { Write-Host $_ }

Write-Host "Done. Run: .\lab\tg544-openwrt\run-p6-annex-m-inhibit.ps1"
