# Restart ccli on DUT for A2/B2 Modbus bench + verify TotW via MMS.
# Usage:
#   .\lab\tg544-openwrt\restart-dut-modbus-check.ps1
# Credentials: lab\tg544-openwrt\lab-env.ps1 (gitignored)

param(
    [string]$DutAddr = "192.168.10.1",
    [string]$User = "root",
    [switch]$UseA1B1 = $true,
    [switch]$UseRs485_2,
    [switch]$SkipVerify
)

$Plink = "C:\Program Files\PuTTY\plink.exe"
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$A1Yaml = Join-Path $Repo "apps\ccli\config\lab_tr400_cleartext_tsp_ttyS1.yaml"
$Rs485_2Yaml = Join-Path $Repo "apps\ccli\config\lab_tr400_cleartext_tsp.yaml"
$VerifyScript = Join-Path $PSScriptRoot "verify-tespro-points-live.ps1"
$LabEnv = Join-Path $PSScriptRoot "lab-env.ps1"

function Log($msg) { Write-Host "[$(Get-Date -Format 'HH:mm:ss')] $msg" }

if (Test-Path $LabEnv) { . $LabEnv }

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Missing CCLI_TG544_PW. Copy lab-env.example.ps1 to lab-env.ps1 or set `$env:CCLI_TG544_PW."
}

if (-not (Test-Path $Plink)) { Write-Error "Install PuTTY plink." }

function Invoke-Dut($cmd) {
    & $Plink -ssh "${User}@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $cmd 2>&1 |
        ForEach-Object { Log "  $_" }
}

Log "=== Restart DUT ccli (Modbus bench check) ==="

$benchYaml = if ($UseRs485_2) { $Rs485_2Yaml } else { $A1Yaml }
$benchLabel = if ($UseRs485_2) { "rs485_2_uart (LuCI RS485-2)" } else { "ttyS1 (A1/B1 screws)" }
if (-not (Test-Path $benchYaml)) { Write-Error "Missing $benchYaml" }
Log "Upload lab yaml ($benchLabel) ..."
& $Pscp -scp -pw $env:CCLI_TG544_PW -hostkey $HostKey $benchYaml "${User}@${DutAddr}:/tmp/ccli.lab.yaml.new" 2>&1 |
    ForEach-Object { Log "  $_" }
Invoke-Dut "cp -f /tmp/ccli.lab.yaml.new /etc/ccli/lab.yaml && grep device /etc/ccli/lab.yaml | head -1"

Log "DUT devices:"
Invoke-Dut "ls -l /dev/ttyS2 /dev/rs485_2_uart 2>&1; head -1 /etc/ccli/lab.yaml"

Log "Restart ccli ..."
Invoke-Dut "killall ccli 2>/dev/null; sleep 2; /usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 & sleep 6; pgrep -a ccli || echo FAIL_no_ccli"
Invoke-Dut "grep modbus /tmp/ccli.log | tail -8"
Invoke-Dut "ss -tlnp 2>/dev/null | grep ':102' || netstat -tln 2>/dev/null | grep ':102' || echo no_102"

if (-not $SkipVerify) {
    Log ""
    if (Test-Path $LabEnv) { . $LabEnv }
    & $VerifyScript -DutAddr $DutAddr
}
