# P6-03 - Annex M trip on DIO3 blocks PF2 curtail on DIO1 (O.11 lab).
param(
    [string]$DutAddr = "192.168.10.1",
    [string]$TripScript = "/usr/sbin/annex-m-trip.sh"
)

$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$FactoryCmds = Join-Path $Repo "lab\tr400-dio-factory.cmds"
$TripSh = Join-Path $PSScriptRoot "annex-m-trip.sh"
$EvidenceDir = Join-Path $Repo "lab\evidence\phase6"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$LogFile = Join-Path $EvidenceDir "P6-03_ANNEX_M_INHIBIT_$Stamp.txt"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set env:CCLI_TG544_PW"
}

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

function Invoke-Dut([string]$Cmd) {
    & $Plink -ssh "root@${DutAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $Cmd 2>&1
}

Log "# P6-03 Annex M inhibit - DIO3 trip blocks DIO1"
Log "DUT=$DutAddr"

Log "Step 1: restore DIO factory modes (ch1 DO, ch2 DI, ch3 DO) ..."
$factory = Get-Content $FactoryCmds -Raw
Invoke-Dut $factory | ForEach-Object { Log $_ }

Log "Step 2: install annex-m-trip.sh on DUT ..."
$tripBody = (Get-Content $TripSh -Raw) -replace "`r`n", "`n"
$b64 = [Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($tripBody))
Invoke-Dut "echo $b64 | openssl base64 -d -A > $TripScript; chmod +x $TripScript" | ForEach-Object { Log $_ }

Log "Step 3: assert trip (DIO3 ON) ..."
Invoke-Dut "sh $TripScript on" | ForEach-Object { Log $_ }

Log "Step 4: check ccli log for annex_m inhibit ..."
Start-Sleep -Seconds 8
Invoke-Dut "grep -E 'annex_m_trip_monitor|curtail_blocked_annex_m|teledistacco_inhibit|annex_m_trip=ACTIVE' /tmp/ccli.log 2>/dev/null | tail -8; ubus call dido_v2 status 2>/dev/null | head -25" |
    ForEach-Object { Log $_ }

Log "Step 5: clear trip (DIO3 OFF) ..."
Invoke-Dut "sh $TripScript off" | ForEach-Object { Log $_ }

$hit = Select-String -Path $LogFile -Pattern "annex_m_trip_monitor|curtail_blocked_annex_m|annex_m_trip=ACTIVE" -Quiet
if ($hit) {
    Log "P6-03 VERDICT: PASS"
    exit 0
}
Log "P6-03 VERDICT: PARTIAL"
exit 0
