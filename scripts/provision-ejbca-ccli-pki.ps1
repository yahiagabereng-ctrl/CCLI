# Provision Annex-aligned CCLI PKI: generate CA -> import to EJBCA -> issue Cert A/B.
param(
    [string]$Container = "ccli-ejbca-ce",
    [string]$CaName = "CCLI-Lab-CA",
    [string]$P12Password = "ccli-lab-p12",
    [string]$OutDir = "",
    [string]$TspDir = "C:\CCLI_product_tls",
    [switch]$ResetVolume,
    [switch]$SkipDockerCheck,
    [switch]$SkipEjbcaImport
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

Write-Host "Issuing Annex-aligned end-entity certificates..." -ForegroundColor Cyan
python $Py issue `
    --ca-p12 $localP12 `
    --ca-pem $localPem `
    --p12-password $P12Password `
    --out $EjbcaTls
if ($LASTEXITCODE -ne 0) { throw "issue failed" }

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

Write-Host "`n=== Annex 10.4 verification ===" -ForegroundColor Cyan
$root = Join-Path $EjbcaTls "root_CA.pem"
$serverTls = Join-Path $EjbcaTls "server_tls.pem"
$serverE2e = Join-Path $EjbcaTls "server.pem"
$serverKey = Join-Path $EjbcaTls "server.key"
$acseKey = Join-Path $EjbcaTls "server_acse.key"
$client = Join-Path $EjbcaTls "client.pem"
$fail = 0

function Assert-Match([string]$Label, [string]$Haystack, [string]$Needle) {
    if ($Haystack -match [regex]::Escape($Needle)) {
        Write-Host "  PASS $Label" -ForegroundColor Green
    }
    else {
        Write-Warning "  FAIL $Label (missing $Needle)"
        $script:fail++
    }
}

# Gate 1 - issuer CN
$issuer = openssl x509 -in $serverTls -noout -issuer 2>&1 | Out-String
Assert-Match "Gate1 issuer CCLI Lab CA" $issuer "CCLI Lab CA"

# Gate 2 - dual cert / dual keys
$serA = openssl x509 -in $serverTls -noout -serial 2>&1 | Out-String
$serB = openssl x509 -in $serverE2e -noout -serial 2>&1 | Out-String
if ($serA -ne $serB) {
    Write-Host "  PASS Gate2 Cert A serial != Cert B serial" -ForegroundColor Green
}
else {
    Write-Warning "  FAIL Gate2 same serial for Cert A and Cert B"
    $fail++
}
if ((Test-Path $serverKey) -and (Test-Path $acseKey)) {
    $h1 = (Get-FileHash $serverKey -Algorithm SHA256).Hash
    $h2 = (Get-FileHash $acseKey -Algorithm SHA256).Hash
    if ($h1 -ne $h2) {
        Write-Host "  PASS Gate2 server.key != server_acse.key" -ForegroundColor Green
    }
    else {
        Write-Warning "  FAIL Gate2 TLS and ACSE keys identical"
        $fail++
    }
}
else {
    Write-Warning "  FAIL Gate2 missing server.key / server_acse.key"
    $fail++
}

# Gate 3 - Cert A EKU
$tlsTxt = openssl x509 -in $serverTls -noout -text 2>&1 | Out-String
Assert-Match "Gate3 Cert A serverAuth" $tlsTxt "TLS Web Server Authentication"
Assert-Match "Gate3 Cert A clientAuth" $tlsTxt "TLS Web Client Authentication"
Assert-Match "Gate3 Cert A SAN 192.168.10.1" $tlsTxt "192.168.10.1"
Assert-Match "Gate3 Cert A CN CCI016_01" $tlsTxt "CCI016_01"

# Gate 4 - Cert B KU (no TLS EKU)
$e2eTxt = openssl x509 -in $serverE2e -noout -text 2>&1 | Out-String
Assert-Match "Gate4 Cert B digitalSignature" $e2eTxt "Digital Signature"
Assert-Match "Gate4 Cert B keyAgreement" $e2eTxt "Key Agreement"
if ($e2eTxt -match "TLS Web Server Authentication") {
    Write-Warning "  FAIL Gate4 Cert B must not carry TLS EKU"
    $fail++
}
else {
    Write-Host "  PASS Gate4 Cert B has no TLS EKU" -ForegroundColor Green
}

# Gate 5 - G.6.2 OID
Assert-Match "Gate5 Cert B subject OID 1.1.1.999.1.12" $e2eTxt "1.1.1.999.1.12"

# Gate 6 - chain verify
$ver = openssl verify -CAfile $root $serverTls $serverE2e $client 2>&1 | Out-String
if ($ver -match "error") {
    Write-Warning "  FAIL Gate6 openssl verify: $ver"
    $fail++
}
else {
    Write-Host "  PASS Gate6 openssl verify chain" -ForegroundColor Green
}

# Size gate T.3.3.4.2
$tmpDer = Join-Path $env:TEMP "ccli-server-e2e.der"
openssl x509 -in $serverE2e -outform DER -out $tmpDer 2>$null
$sz = (Get-Item $tmpDer).Length
if ($sz -lt 8192) {
    Write-Host ("  PASS Gate T.3.3.4.2 Cert B DER size {0} under 8192" -f $sz) -ForegroundColor Green
}
else {
    Write-Warning ("  FAIL Gate T.3.3.4.2 Cert B DER size {0} >= 8192" -f $sz)
    $fail++
}

Write-Host ""
if ($fail -gt 0) {
    throw ("Annex verification failed ({0} checks). See lab/EJBCA_LAB_SETUP.md section 10.4" -f $fail)
}

Write-Host "OK - all Annex 10.4 lab gates passed" -ForegroundColor Green
Write-Host "  DUT/TSP trust + EE certs: $EjbcaTls"
Write-Host "  TSP staged:               $TspDir"
Write-Host '  Next: deploy to TG544 + TSP Connect :3782 - see lab/EJBCA_LAB_SETUP.md section 9'
