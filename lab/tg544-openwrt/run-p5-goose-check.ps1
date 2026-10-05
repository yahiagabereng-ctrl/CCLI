# P5-G02 — GOOSE / MMS readiness check (repo + DUT + PC ports)
param(
    [string]$DutAddr = "192.168.10.1",
    [string]$RepoRoot = (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent)
)

$ErrorActionPreference = "Continue"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmm"
$EvidenceDir = Join-Path $RepoRoot "lab\evidence\phase5"
$LogFile = Join-Path $EvidenceDir "P5_GOOSE_CHECK_$Stamp.txt"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

Log "# P5-G02 GOOSE check - DUT $DutAddr"

# --- Repo ---
$Cid = Join-Path $RepoRoot "apps\ccli\config\icd\lab_tg544_eth_a.cid"
$Cfg = Join-Path $RepoRoot "apps\ccli\config\icd\lab_tg544_eth_a.cfg"
if ((Select-String -Path $Cid -Pattern 'GSEControl name="gcb_PdC_Mis4sec"' -Quiet)) {
    Log "R1 CID GoCB: PASS"
} else { Log "R1 CID GoCB: FAIL" }
if (Test-Path $Cfg) {
    $gc = Select-String -Path $Cfg -Pattern 'GC\(gcb_PdC_Mis4sec' -Quiet
    if ($gc) { Log "R2 cfg GC(): PASS" } else { Log "R2 cfg GC(): FAIL - run scripts/gen-mms-model-cfg.ps1" }
} else { Log "R2 cfg GC(): FAIL - missing $Cfg" }

# --- Network ---
$ping = (Test-NetConnection -ComputerName $DutAddr -WarningAction SilentlyContinue).PingSucceeded
Log "N1 Ping ${DutAddr}: $(if ($ping) { 'PASS' } else { 'FAIL' })"
$p102 = (Test-NetConnection -ComputerName $DutAddr -Port 102 -WarningAction SilentlyContinue).TcpTestSucceeded
if ($p102) { Log "N2 TCP :102: PASS" } else { Log "N2 TCP :102: FAIL - ccli not listening" }
$p3782 = (Test-NetConnection -ComputerName $DutAddr -Port 3782 -WarningAction SilentlyContinue).TcpTestSucceeded
if ($p3782) { Log "N3 TCP :3782: PASS" } else { Log "N3 TCP :3782: FAIL (ok if cleartext lab only)" }

# --- DUT SSH ---
if (-not $env:CCLI_TG544_PW) {
    Log "DUT SSH: SKIP - set env:CCLI_TG544_PW"
} elseif (-not (Test-Path $Plink)) {
    Log "DUT SSH: SKIP - PuTTY plink not found"
} else {
    $cmd = @'
/usr/sbin/ccli --version 2>/dev/null || ccli --version
ps w | grep '[c]cli'
ss -tlnp 2>/dev/null | grep -E ':102|:3782'
grep -E 'GC\(gcb_' /etc/ccli/icd/lab_tg544_eth_a.cfg 2>/dev/null | head -3
grep -E 'publish_enabled|goose:' /etc/ccli/lab.yaml 2>/dev/null | head -6
grep -E 'GOOSE publishing|listening on' /tmp/ccli.log 2>/dev/null | tail -5
'@
    Log "--- DUT plink ---"
    & $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $cmd 2>&1 | ForEach-Object { Log $_ }
}

# --- PC Npcap hint ---
$npcap = Test-Path "C:\Windows\System32\Npcap\wpcap.dll"
if ($npcap) { Log "PC Npcap: PASS (dll present)" } else { Log "PC Npcap: WARN - install Npcap for IED Explorer GooseSender" }

Log "Log: $LogFile"
Write-Host "`nDone. See $LogFile"
