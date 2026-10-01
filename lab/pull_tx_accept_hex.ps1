# Pull latest TX_ACCEPT_SPDU hex dump from TG544 /tmp/ccli.log into lab/*.hex
param(
    [string]$HostAddr = "192.168.10.1",
    [string]$User = "root",
    [string]$OutHex = "",
    [int]$ExpectedLen = 1269
)

$ErrorActionPreference = "Stop"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$Repo = Split-Path -Parent $PSScriptRoot
if (-not $OutHex) { $OutHex = Join-Path $Repo "lab\tmp_tx_step7_latest.hex" }
if (-not $env:CCLI_TG544_PW) { Write-Error "Set `$env:CCLI_TG544_PW" }

$pw = $env:CCLI_TG544_PW
$lineNo = & $Plink -ssh "${User}@${HostAddr}" -pw $pw -hostkey $HostKey -batch `
    "grep -n 'TX_ACCEPT_SPDU len=$ExpectedLen' /tmp/ccli.log | tail -1 | cut -d: -f1"
$lineNo = $lineNo.Trim()
if (-not $lineNo) { Write-Error "No TX_ACCEPT len=$ExpectedLen in DUT log" }

$start = [int]$lineNo + 1
$hex = & $Plink -ssh "${User}@${HostAddr}" -pw $pw -hostkey $HostKey -batch `
    "sed -n '${start},/^mms: HEX TX_ACCEPT_SPDU end/p' /tmp/ccli.log | grep -v 'mms: HEX' | tr -d '\n'"
$hex = $hex.Trim()
if ($hex.Length -ne ($ExpectedLen * 2)) {
    Write-Warning "Expected $($ExpectedLen * 2) hex chars, got $($hex.Length)"
}
Set-Content -Path $OutHex -Value $hex -NoNewline -Encoding ascii
Write-Host "Wrote $OutHex ($($hex.Length / 2) bytes) from log line $lineNo"
