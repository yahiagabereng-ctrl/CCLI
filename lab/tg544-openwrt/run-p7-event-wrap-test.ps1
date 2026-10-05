# P7-01 — O.14 event ring wrap test on DUT (r27+)
# Usage: .\lab\tg544-openwrt\run-p7-event-wrap-test.ps1

$ErrorActionPreference = "Stop"
$DutIp = if ($env:CCLI_DUT_IP) { $env:CCLI_DUT_IP } else { "192.168.10.1" }
$Pw = if ($env:CCLI_TG544_PW) { $env:CCLI_TG544_PW } else { "CCLI_TG544_PW" }
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$OutDir = Join-Path $PSScriptRoot "..\evidence\phase7"
$OutFile = Join-Path $OutDir "P7-01_EVENT_WRAP_TEST_$Stamp.txt"

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$plink = Get-Command plink -ErrorAction SilentlyContinue
if (-not $plink) {
    Write-Error "plink not found — install PuTTY and add to PATH"
}

$cmds = @(
    "mkdir -p /var/lib/ccli",
    "ccli --event-wrap-test",
    "ccli --config /etc/ccli/lab.yaml --event-dump --count 5",
    "wc -l /var/lib/ccli/events.jsonl 2>/dev/null || echo 'no daemon log yet'"
)

$remote = ($cmds -join "; ")
Write-Host "P7-01 wrap test on $DutIp ..."

$out = & plink -batch -pw $Pw "root@${DutIp}" $remote 2>&1
$out | Tee-Object -FilePath $OutFile

if ($out -match "P7-01 wrap test: PASS") {
    Write-Host "PASS — evidence: $OutFile" -ForegroundColor Green
    exit 0
}

Write-Host "FAIL or DUT not on r27 — evidence: $OutFile" -ForegroundColor Yellow
exit 1
