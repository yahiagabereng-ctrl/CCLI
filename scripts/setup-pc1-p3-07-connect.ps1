# PC1 field pack for P3-07 Oct-08 Connect profile (31-LN CID + p3_07_connect TLS).
#
# Usage:
#   git pull
#   powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK

param(
    [string]$Dest = "D:\CCLI_TSP_FIELD_PACK"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$TlsSrc = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"
if (-not (Test-Path (Join-Path $TlsSrc "client.pem"))) {
    Write-Host "Missing PKI - run: powershell -File scripts\publish-p3-07-connect-profile.ps1" -ForegroundColor Red
    exit 1
}

& (Join-Path $Repo "scripts\stage-tsp-field-pack.ps1") -Dest $Dest
$tlsDest = Join-Path $Dest "tls"
if (Test-Path $tlsDest) {
    Remove-Item -Force -Recurse $tlsDest
}
New-Item -ItemType Directory -Force -Path $tlsDest | Out-Null
Copy-Item -Force (Join-Path $TlsSrc "*") $tlsDest

$cidSrc = Join-Path $Repo "apps\ccli\config\icd\lab_tg544_eth_a.cid"
$cidDst = Join-Path $Dest "cid\lab_tg544_eth_a.cid"
New-Item -ItemType Directory -Force -Path (Join-Path $Dest "cid") | Out-Null
Copy-Item -Force $cidSrc $cidDst

$docs = @(
    @{ Rel = "handoff/masomeh95/PC1_FIX_WRONG_TRANSPORT_CERT.md"; Out = "00t_PC1_FIX_WRONG_TRANSPORT_CERT.md" },
    @{ Rel = "handoff/masomeh95/PC1_P3_07_CONNECT_PROFILE_ACTIVE.md"; Out = "00r_PC1_P3_07_CONNECT_PROFILE_ACTIVE.md" },
    @{ Rel = "handoff/masomeh95/PC1_P3_07_FIRST_PASS_TLS_2026-10-08.md"; Out = "00m_PC1_P3_07_FIRST_PASS_TLS_2026-10-08.md" },
    @{ Rel = "lab/evidence/testsuite-pro/inbox/TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md"; Out = "00s_TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md" },
    @{ Rel = "lab/evidence/testsuite-pro/inbox/TSP_TRANSPORT_CLOSED_ROOT_CAUSE_2026-10-15.md"; Out = "00u_TSP_TRANSPORT_CLOSED_ROOT_CAUSE_2026-10-15.md" }
)
foreach ($d in $docs) {
    $from = Join-Path $Repo ($d.Rel -replace "/", "\")
    if (Test-Path $from) {
        Copy-Item -Force $from (Join-Path $Dest $d.Out)
        Write-Host "  $($d.Out)"
    }
}

Write-Host "`n=== TLS verify (Oct-08 profile) ===" -ForegroundColor Cyan
& (Join-Path $Repo "scripts\verify-tsp-tls-key-match.ps1") -TlsDir (Join-Path $Dest "tls")

Write-Host "`nOK: $Dest" -ForegroundColor Green
Write-Host "Read FIRST: 00t_PC1_FIX_WRONG_TRANSPORT_CERT.md"
Write-Host "Then:       00r_PC1_P3_07_CONNECT_PROFILE_ACTIVE.md"
Write-Host 'CID:  cid\lab_tg544_eth_a.cid  (31-LN lab, same as 2026-10-08 PASS)'
Write-Host 'TLS:  tls\  from p3_07_connect (client_tsp.key in BOTH TSP key slots)'
Write-Host 'NO CRL / NO revoked certs in TSP for this session.'
