# Stage CCLI lab TLS PEMs for Triangle MicroWorks Test Suite Pro (PC-side)
# Does NOT Connect to DUT. Copies identity material into ProgramData for TSP UI browse.
#
# Usage:
#   powershell -File scripts\stage-tsp-tls.ps1
#   powershell -File scripts\stage-tsp-tls.ps1 -Dest "D:\lab\tsp-tls"

param(
    [string]$Dest = "C:\ProgramData\Triangle MicroWorks\61850 Test Suite Pro\CCLI_tls",
    [string]$AltDest = "C:\CCLI_tls"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Src = Join-Path $RepoRoot "apps\ccli\config\tls"

if (-not (Test-Path $Src)) {
    throw "TLS source missing: $Src"
}

New-Item -ItemType Directory -Force -Path $Dest | Out-Null

$files = @(
    "root_CA.pem",
    "client.pem", "client.key", "client_tsp.key",
    "viewer.pem", "viewer.key",
    "revoked.pem", "revoked.key",
    "lab_crl.pem",
    "README.md"
)

function Stage-TspTlsFolder {
    param([string]$Target)
    New-Item -ItemType Directory -Force -Path $Target | Out-Null
    foreach ($f in $files) {
        $from = Join-Path $Src $f
        if (-not (Test-Path $from)) { Write-Warning "Missing $f"; continue }
        Copy-Item -Force $from (Join-Path $Target $f)
        Write-Host "staged $f -> $Target"
    }
}

Stage-TspTlsFolder -Target $Dest
if ($AltDest -and ($AltDest -ne $Dest)) {
    Stage-TspTlsFolder -Target $AltDest
}

$readme = @"
# CCLI TSP TLS stage (lab only)

Copied from repo ``apps/ccli/config/tls`` on $(Get-Date -Format 'yyyy-MM-dd HH:mm').

## Wire in Test Suite Pro

| Role | Files |
|------|-------|
| Trust anchor | root_CA.pem |
| DSO Operate (Wlim/WSd) | client.pem + **client_tsp.key** (TSP OpenSSL RSA) |
| Viewer / RBAC negative | viewer.pem + viewer.key |
| CRL negative (P3-09) | revoked.* + lab_crl.pem |

Connection: 192.168.10.1 port **3782** TLS — do not use cleartext 102 for lab product profile.

See: lab/CCI_TestSuitePro_Sequencer_Draft.md
"@
Set-Content -Path (Join-Path $Dest "TSP_STAGE_README.txt") -Value $readme -Encoding UTF8

Write-Host ""
Write-Host "OK -> $Dest"
if ($AltDest -and ($AltDest -ne $Dest)) { Write-Host "OK -> $AltDest" }
Write-Host "Next: TSP System Status / IED Connection Configuration -> TLS paths above."
Write-Host "Use client_tsp.key for MMS Security + Transport TLS private key (TSP OpenSSL)."
Write-Host "Keep Disconnected until DUT is up (Phase B)."
