# Verify GNSS fix + ccli time_quality on TG544 (REQ-TIM-002 / V-TIM-02).
param(
    [string]$HostAddr = "192.168.10.1",
    [string]$EvidenceDir = ""
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
if (-not $EvidenceDir) {
    $EvidenceDir = Join-Path $Repo "lab\evidence\phase5"
}
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$LogFile = Join-Path $EvidenceDir "P5_GNSS_FIX_VERIFY_$Stamp.txt"

if (-not $env:CCLI_TG544_PW) {
    Write-Error "Set `$env:CCLI_TG544_PW before running."
}

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null

$cmd = "ubus call sim-manager gnss_get_position; echo '---'; ubus call sim-manager gnss_check; echo '---'; chronyc tracking; echo '---'; chronyc sources; echo '---'; grep time_quality /tmp/ccli.log | tail -5"

$out = & $Plink -ssh "root@${HostAddr}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $cmd 2>&1
$text = ($out | Out-String)

$verdict = "FAIL"
if ($text -match 'gnss_fix=1' -and $text -match 'clockNotSynchronized=0') {
    $verdict = "PASS"
} elseif ($text -match '"fix":\s*true' -and $text -match 'within_pm100=yes') {
    $verdict = "PART - fix true; wait for next ccli gnss poll (15s) and re-run"
}

$header = "# P5 GNSS + time quality verify`nDate: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')`nHost: $HostAddr`nVERDICT: $verdict`n`n"

Set-Content -Path $LogFile -Value ($header + $text) -Encoding UTF8
Write-Host $header
Write-Host $text
Write-Host "Saved: $LogFile"
Write-Host "VERDICT: $verdict"

if ($verdict -eq "PASS") { exit 0 }
exit 1
