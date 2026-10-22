# One plink SSH session: live read of all CCLI EMT432 registers (DUT LAN3 -> meter).
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File lab\emt432_dump_registers_one_ssh.ps1

param(
    [string]$DutHost = "192.168.1.130",
    [string]$MeterHost = "192.168.178.10",
    [string]$OutFile = "",
    [switch]$FullLogs,
    [switch]$Open
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path $PSScriptRoot -Parent
. (Join-Path $RepoRoot "lab\tg544-openwrt\lab-env.ps1")

$plink = "C:\Program Files\PuTTY\plink.exe"
$pscp = "C:\Program Files\PuTTY\pscp.exe"
$hostkey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$luaScript = Join-Path $PSScriptRoot "emt432_dut_ccli_register_dump.lua"
$remotePath = "/tmp/emt432_ccli_register_dump.lua"

if (-not (Test-Path $luaScript)) { throw "Missing $luaScript" }
if (-not $OutFile) {
    if ($FullLogs) {
        $OutFile = Join-Path $RepoRoot "lab\evidence\phase5\EMT432_DUT_FULL_REGISTER_LOG_LATEST.txt"
    } else {
        $OutFile = Join-Path $RepoRoot "lab\evidence\phase5\EMT432_DUT_SSH_DUMP_LATEST.txt"
    }
}

& $pscp -scp -batch -hostkey $hostkey -pw $env:CCLI_TG544_PW $luaScript "root@${DutHost}:${remotePath}"
if ($LASTEXITCODE -ne 0) { throw "pscp failed" }

# Optional: ring-buffer modbus lines from ccli (commissioning PASS/FAIL, FC3 trace when yaml trace:true).
$logSection = ""
if ($FullLogs) {
    $logSection =
      "echo '=== DUT logread modbus (recent, ccli running) ==='; " +
      "logread 2>/dev/null | grep -E 'modbus:' | tail -n 500; " +
      "echo ''; "
}

$postStartLogs = ""
if ($FullLogs) {
    $postStartLogs =
      "echo '=== DUT logread modbus (after ccli restart / commissioning) ==='; " +
      "sleep 7; logread 2>/dev/null | grep -E 'modbus:' | tail -n 200; " +
      "echo ''; "
}

# Pause ccli Modbus master during snapshot (avoid FC3 overlap on same TCP slave).
$remoteCmd =
  $logSection +
  "/etc/init.d/ccli stop 2>/dev/null; " +
  "EMT432_HOST=$MeterHost EMT432_PORT=502 EMT432_UNIT=1 EMT432_WORD_SWAP=1 lua $remotePath; " +
  "rc=`$?; /etc/init.d/ccli start 2>/dev/null; " +
  $postStartLogs +
  "exit `$rc"

Write-Host "=== SSH $DutHost -> Modbus $MeterHost (all CCLI registers, live) ===" -ForegroundColor Cyan

$prev = $ErrorActionPreference
$ErrorActionPreference = "Continue"
& $plink -batch -hostkey $hostkey -pw $env:CCLI_TG544_PW "root@${DutHost}" $remoteCmd 2>&1 |
    Tee-Object -FilePath $OutFile
$ec = $LASTEXITCODE
$ErrorActionPreference = $prev

Write-Host "Saved: $OutFile" -ForegroundColor Green
if ($Open -and (Test-Path $OutFile)) {
    Start-Process $OutFile
}
if ($ec -ne 0) { exit $ec }
