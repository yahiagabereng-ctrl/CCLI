# P6-03 — deploy ccli r26 + lab.yaml (pscp -scp for binary).
param(
    [string]$DutAddr = "192.168.10.1"
)

$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Bin = Join-Path $PSScriptRoot "ccli-bin"
$LabYaml = Join-Path $Repo "apps\ccli\config\lab_tr400_cleartext_tsp_ttyS1.yaml"
$DeploySh = Join-Path $PSScriptRoot "deploy-ccli-dut.sh"

if (-not $env:CCLI_TG544_PW) { Write-Error "Set env:CCLI_TG544_PW" }
if (-not (Test-Path $Bin)) { Write-Error "Missing $Bin - run wsl-build-ccli.sh" }

Write-Host "=== P6-03 full deploy r26 -> $DutAddr ==="
& $Pscp -scp -batch -pw $env:CCLI_TG544_PW -hostkey $HostKey $Bin "root@${DutAddr}:/tmp/ccli.new"

$yamlBody = (Get-Content $LabYaml -Raw) -replace "`r`n", "`n"
$yamlB64 = [Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($yamlBody))
& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "echo $yamlB64 | openssl base64 -d -A > /etc/ccli/lab.yaml" | Out-Null

$shBody = (Get-Content $DeploySh -Raw) -replace "`r`n", "`n"
$shB64 = [Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($shBody))
& $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch `
    "echo $shB64 | openssl base64 -d -A > /tmp/deploy-ccli-dut.sh; sed -i 's/\r$//' /tmp/deploy-ccli-dut.sh; chmod +x /tmp/deploy-ccli-dut.sh; sh /tmp/deploy-ccli-dut.sh" 2>&1 |
    ForEach-Object { Write-Host $_ }

Write-Host "Done."
