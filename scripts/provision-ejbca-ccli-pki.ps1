# Provision Annex-aligned CCLI PKI: generate CA -> import to EJBCA -> issue Cert A/B.
# Default issue path is hybrid Python. Use -UseRest for EJBCA REST pkcs10enroll (mTLS admin P12).
param(
    [string]$Container = "ccli-ejbca-ce",
    [string]$CaName = "CCLI-Lab-CA",
    [string]$P12Password = "ccli-lab-p12",
    [string]$OutDir = "",
    [string]$TspDir = "C:\CCLI_product_tls",
    [switch]$ResetVolume,
    [switch]$SkipDockerCheck,
    [switch]$SkipEjbcaImport,
    [switch]$StrictG62,
    [switch]$UseRest,
    [switch]$UseEjbcaCli,
    [string]$AdminP12 = "",
    [string]$AdminP12Password = "foo123",
    [string]$RestBaseUrl = "https://localhost:8443/ejbca/ejbca-rest-api"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Staging = Join-Path $Repo "lab\ejbca-staging"
$EjbcaTls = if ($OutDir) { $OutDir } else { Join-Path $Repo "apps\ccli\config\tls\ejbca" }
$Py = Join-Path $Repo "scripts\provision_ejbca_ccli_pki.py"
$EjbcaSh = "/opt/keyfactor/bin/ejbca.sh"
$ComposeDir = Join-Path $Repo "knowledge-base\08-engineering\reference\vendor\ejbca\docker"

function Invoke-Ejbca {
    param([Parameter(Mandatory = $true)][string[]]$EjbcaArgs)
    $cmd = @("exec", $Container, $EjbcaSh) + $EjbcaArgs
    Write-Host "+ docker $($cmd -join ' ')"
    $out = & docker @cmd 2>&1
    $out | Write-Host
    if ($LASTEXITCODE -ne 0) { throw "EJBCA command failed: $($EjbcaArgs -join ' ')" }
    if ($out -match "The following categories are available") {
        throw "EJBCA CLI returned help instead of running: $($EjbcaArgs -join ' ')"
    }
    return $out
}

if ($ResetVolume) {
    Write-Host "Resetting EJBCA lab volume (lab data only)..." -ForegroundColor Yellow
    Push-Location $ComposeDir
    docker compose down -v
    Pop-Location
    & (Join-Path $Repo "scripts\start-ejbca-lab.ps1")
}

if (-not $SkipDockerCheck) {
    $running = docker ps --filter "name=$Container" --format "{{.Names}}"
    if (-not $running) {
        Write-Host "Starting EJBCA..." -ForegroundColor Yellow
        & (Join-Path $Repo "scripts\start-ejbca-lab.ps1")
    }
}

New-Item -ItemType Directory -Force -Path $Staging, $EjbcaTls | Out-Null

Write-Host "Generating Annex CA (RSA-2048 / SHA-256 / 3650d)..." -ForegroundColor Cyan
python $Py generate-ca --staging $Staging --p12-password $P12Password
if ($LASTEXITCODE -ne 0) { throw "generate-ca failed" }

$localP12 = Join-Path $Staging "ca-export.p12"
$localPem = Join-Path $Staging "root_CA.pem"
if (-not (Test-Path $localP12)) { throw "Missing $localP12" }

if (-not $SkipEjbcaImport) {
    $cas = docker exec $Container $EjbcaSh ca listcas 2>&1 | Out-String
    if ($cas -match [regex]::Escape($CaName)) {
        Write-Host "CA $CaName already in EJBCA - skipping import (use -ResetVolume to recreate)" -ForegroundColor Yellow
    }
    else {
        Write-Host "Importing CA into EJBCA as $CaName..." -ForegroundColor Cyan
        $p12In = "/tmp/ccli-ca-import.p12"
        docker cp $localP12 "${Container}:${p12In}"
        Invoke-Ejbca @(
            "ca", "importca",
            "--caname", $CaName,
            "--p12", $p12In,
            "-kspassword", $P12Password,
            "--signalias", "signKey",
            "--autoactivate", "true"
        )
    }
}

if ($UseEjbcaCli -and $UseRest) {
    throw "Use only one of -UseEjbcaCli or -UseRest"
}

if (-not $SkipEjbcaImport) {
    $SetupPy = Join-Path $Repo "scripts\setup_ejbca_annex_profiles.py"
    Write-Host "Ensuring EJBCA Annex cert/EE profiles..." -ForegroundColor Cyan
    python $SetupPy
    if ($LASTEXITCODE -ne 0) { throw "EJBCA profile setup failed" }
}

if ($UseRest) {
    Write-Host "Issuing end-entity certificates via EJBCA REST (pkcs10enroll)..." -ForegroundColor Cyan
    $admin = if ($AdminP12) { $AdminP12 } else { Join-Path $Staging "superadmin.p12" }
    if (-not (Test-Path $admin)) {
        Write-Host "Admin P12 missing - trying export from container..." -ForegroundColor Yellow
        & (Join-Path $Repo "scripts\export-ejbca-admin-p12.ps1") -Container $Container -OutDir $Staging -Password $AdminP12Password
        if (-not (Test-Path $admin)) {
            throw "Need SuperAdmin P12 at $admin for REST mTLS. See lab/EJBCA_LAB_SETUP.md section 12."
        }
    }
    $RestPy = Join-Path $Repo "scripts\issue_ejbca_rest_pki.py"
    $restArgs = @(
        $RestPy,
        "--base-url", $RestBaseUrl,
        "--ca-name", $CaName,
        "--out", $EjbcaTls,
        "--admin-p12", $admin,
        "--admin-p12-password", $AdminP12Password,
        "--insecure",
        "--root-ca-pem", $localPem
    )
    python @restArgs
    if ($LASTEXITCODE -ne 0) { throw "EJBCA REST issue failed" }
}
elseif ($UseEjbcaCli) {
    Write-Host "Issuing end-entity certificates via EJBCA CLI (createcert)..." -ForegroundColor Cyan
    $CliPy = Join-Path $Repo "scripts\issue_ejbca_cli_pki.py"
    python $CliPy --ca-name $CaName --out $EjbcaTls
    if ($LASTEXITCODE -ne 0) { throw "EJBCA CLI issue failed" }
}
else {
    Write-Host "Issuing Annex-aligned end-entity certificates (hybrid Python)..." -ForegroundColor Cyan
    python $Py issue `
        --ca-p12 $localP12 `
        --ca-pem $localPem `
        --p12-password $P12Password `
        --out $EjbcaTls
    if ($LASTEXITCODE -ne 0) { throw "issue failed" }
}

# Prefer EJBCA-published CRL if CA is present; otherwise keep Python CRL.
if (-not $SkipEjbcaImport) {
    $cas2 = docker exec $Container $EjbcaSh ca listcas 2>&1 | Out-String
    if ($cas2 -match [regex]::Escape($CaName)) {
        try {
            Write-Host "Publishing CRL in EJBCA..." -ForegroundColor Cyan
            Invoke-Ejbca @("ca", "createcrl", "--caname", $CaName)
            $crlIn = "/tmp/ccli-lab.crl"
            Invoke-Ejbca @("ca", "getcrl", "--caname", $CaName, "-f", $crlIn)
            docker cp "${Container}:${crlIn}" (Join-Path $EjbcaTls "lab_crl.ejbca.der")
            Write-Host '  (Annex lab CRL remains lab_crl.pem from Python; EJBCA DER saved as lab_crl.ejbca.der)' -ForegroundColor DarkGray
        }
        catch {
            Write-Warning "EJBCA CRL publish skipped: $($_.Exception.Message)"
        }
    }
}

New-Item -ItemType Directory -Force -Path $TspDir | Out-Null
$copy = @(
    "root_CA.pem", "client.pem", "client.key", "client_tls.pem", "client_tsp.key",
    "viewer.pem", "viewer.key", "revoked.pem", "revoked.key", "lab_crl.pem"
)
foreach ($f in $copy) {
    $src = Join-Path $EjbcaTls $f
    if (Test-Path $src) { Copy-Item -Force $src (Join-Path $TspDir $f) }
}

Write-Host "`n=== Annex 10.4 verification (verify_annex_pki.py) ===" -ForegroundColor Cyan
$VerifyPy = Join-Path $Repo "scripts\verify_annex_pki.py"
# Default accepts lab UTF8String G.6.2 (gen_lab_pki / TSP). Use -StrictG62 for Annex
# OBJECT IDENTIFIER tag 0x06 (required once EJBCA Cert B DN is fixed).
$verifyArgs = @($VerifyPy, "--dir", $EjbcaTls)
if ($StrictG62) { $verifyArgs += "--strict-g62" }
python @verifyArgs
if ($LASTEXITCODE -ne 0) {
    throw "Annex verification failed. Fix EJBCA profiles / re-issue. See lab/EJBCA_LAB_SETUP.md §10."
}

Write-Host "OK - all Annex 10.4 lab gates passed" -ForegroundColor Green
Write-Host "  DUT/TSP trust + EE certs: $EjbcaTls"
Write-Host "  TSP staged:               $TspDir"
Write-Host '  Next: deploy to TG544 + TSP Connect :3782 - see lab/EJBCA_LAB_SETUP.md section 9'
