# Create CCLI Annex certificate/end-entity profiles in EJBCA via CLI (one-time lab setup).
param(
    [string]$Container = "ccli-ejbca-ce",
    [string]$CaName = "CCLI-Lab-CA"
)

$ErrorActionPreference = "Stop"
$EjbcaSh = "/opt/keyfactor/bin/ejbca.sh"

function Invoke-Ejbca {
    param([Parameter(Mandatory = $true)][string[]]$EjbcaArgs)
    $cmd = @("exec", $Container, $EjbcaSh) + $EjbcaArgs
    Write-Host "+ docker $($cmd -join ' ')"
    $out = docker @cmd 2>&1 | Out-String
    if ($out.Trim()) { Write-Host $out.TrimEnd() }
    if ($LASTEXITCODE -ne 0) { throw "EJBCA CLI failed: $($EjbcaArgs -join ' ')" }
    return $out
}

Write-Host "=== EJBCA CCLI profile setup ===" -ForegroundColor Cyan
Invoke-Ejbca @("ca", "listcas")

# Certificate profile Cert A — TLS (62351-3 / 62351-9)
try {
    Invoke-Ejbca @(
        "ca", "addcertprofile",
        "--profile", "CCLI-Cert-A-TLS",
        "--type", "ENDUSER",
        "--keyalg", "RSA",
        "--keyspec", "2048",
        "--sigalg", "SHA256WithRSA",
        "--ku", "digitalSignature,keyEncipherment",
        "--eku", "serverAuth,clientAuth"
    )
}
catch {
    if ($_.Exception.Message -match "already exists") {
        Write-Host "  CCLI-Cert-A-TLS exists — skip" -ForegroundColor Yellow
    }
    else { throw }
}

# Certificate profile Cert B — E2E MMS (62351-4 G.5.2)
try {
    Invoke-Ejbca @(
        "ca", "addcertprofile",
        "--profile", "CCLI-Cert-B-E2E-MMS",
        "--type", "ENDUSER",
        "--keyalg", "RSA",
        "--keyspec", "2048",
        "--sigalg", "SHA256WithRSA",
        "--ku", "digitalSignature,keyAgreement"
    )
}
catch {
    if ($_.Exception.Message -match "already exists") {
        Write-Host "  CCLI-Cert-B-E2E-MMS exists — skip" -ForegroundColor Yellow
    }
    else { throw }
}

Write-Host "OK — profiles ready (verify in Admin UI if CLI addcertprofile unsupported on this build)." -ForegroundColor Green
Write-Host "If addcertprofile is unavailable, create profiles manually per lab/EJBCA_LAB_SETUP.md sections 3-5."
