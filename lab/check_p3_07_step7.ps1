# P3-07 Step 7 only: TSP AARE signature verification vs DUT wire + offline crypto.
param(
    [string]$DutHost = "192.168.10.1",
    [string]$CertPem = "server_combined.pem",
    [switch]$SkipDutPull
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$TlsDir = Join-Path $Repo "apps\ccli\config\tls"
$CertPath = Join-Path $TlsDir $CertPem
$HexOut = Join-Path $Repo "lab\tmp_tx_step7_latest.hex"
$Report = Join-Path $Repo "lab\evidence\testsuite-pro\inbox\TSP_P3_07_STEP7_CHECK_$(Get-Date -Format yyyy-MM-dd_HHmmss).txt"

function Add-Section($lines, [string]$Title) {
    $lines.Add("")
    $lines.Add("========== $Title ==========")
    $lines.Add("")
}

$out = [System.Collections.Generic.List[string]]::new()
Add-Section $out "P3-07 STEP 7 - TSP AARE signature verify"
$out.Add("Time: $(Get-Date -Format o)")
$out.Add("DUT: ${DutHost}:3782")
$out.Add("Verify cert: $CertPem")
$out.Add("")
$out.Add("Step 7 = Test Suite Pro validates responding-authentication-value in AARE.")
$out.Add("Steps 1-6 must already be OK: TLS, Session, AARQ, AARE emission.")

if (-not $SkipDutPull) {
    Add-Section $out "DUT live pull"
    if (Test-Connection -ComputerName $DutHost -Count 1 -Quiet) {
        $out.Add("Ping ${DutHost}: OK")
        $pullScript = Join-Path $Repo "lab\pull_tx_accept_hex.ps1"
        if ((Test-Path $pullScript) -and $env:CCLI_TG544_PW) {
            & $pullScript -HostAddr $DutHost -OutHex $HexOut 2>&1 | ForEach-Object { $out.Add($_) }
        }
        else {
            $out.Add("SKIP pull - set CCLI_TG544_PW and ensure pull_tx_accept_hex.ps1 exists")
        }
    }
    else {
        $out.Add("Ping ${DutHost}: FAIL. Using last saved hex on disk.")
    }
}

$hexFile = $null
if (Test-Path $HexOut) { $hexFile = $HexOut }
else {
    foreach ($c in @(
            (Join-Path $Repo "lab\tmp_tx_solution_c_20260929.hex"),
            (Join-Path $Repo "lab\tmp_tx_latest.hex"),
            (Join-Path $Repo "lab\tmp_tx_dut_20260929.hex")
        )) {
        if (Test-Path $c) { $hexFile = $c; break }
    }
}

if (-not $hexFile) {
    $out.Add("ERROR: no hex file. Capture from DUT after TSP Connect attempt.")
    $text = $out -join [Environment]::NewLine
    Write-Host $text
    New-Item -ItemType Directory -Force -Path (Split-Path $Report) | Out-Null
    Set-Content -Path $Report -Value $text -Encoding utf8
    exit 1
}

Add-Section $out "Offline Step 7 crypto independent of TSP"
$out.Add("Hex file: $hexFile")
$locate = & python (Join-Path $Repo "lab\locate_p3_07_failure.py") $hexFile --cert $CertPath 2>&1 | Out-String
$out.Add($locate.Trim())

Add-Section $out "Live Step 7 confirmation with TSP plus Wireshark"
$out.Add("1. Terminal A: .\scripts\capture-mms-tsp.ps1 -DurationSec 60")
$out.Add("2. Terminal B: TSP System Status -> Connect")
$out.Add("3. Wireshark: TLS decrypt with server.key for ${DutHost}:3782")
$out.Add("4. Look for AARE app data then Encrypted Alert 2-5 ms later = Step 7 reject")
$out.Add('5. TSP log must show: Unable to verify signature (+)')

Add-Section $out "Step 7 verdict"
$out.Add("Offline PASS + TSP FAIL sig = Step 7 is inside Test Suite Pro")
$out.Add("Offline FAIL + TSP FAIL sig = fix DUT mms_aare_auth.cpp")

$text = $out -join [Environment]::NewLine
Write-Host $text
New-Item -ItemType Directory -Force -Path (Split-Path $Report) | Out-Null
Set-Content -Path $Report -Value $text -Encoding utf8
Write-Host ""
Write-Host "Report: $Report"

if ($locate -match "DUT encoding \+ RSA signature are valid") { exit 0 }
if ($locate -match "Failure is on the DUT") { exit 2 }
exit 1
