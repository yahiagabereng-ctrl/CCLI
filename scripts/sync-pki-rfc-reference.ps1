# Download IETF RFC text files for PKI / EST corpus.
param(
    [string]$RefPki = ""
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
if (-not $RefPki) {
    $RefPki = Join-Path $Repo "knowledge-base\08-engineering\reference\pki"
}
New-Item -ItemType Directory -Force -Path $RefPki | Out-Null

$rfcs = [ordered]@{
    "RFC7030_EST.txt"    = "https://www.rfc-editor.org/rfc/rfc7030.txt"
    "RFC8894_SCEP.txt"   = "https://www.rfc-editor.org/rfc/rfc8894.txt"
    "RFC5280_X509.txt"   = "https://www.rfc-editor.org/rfc/rfc5280.txt"
    "RFC6960_OCSP.txt"   = "https://www.rfc-editor.org/rfc/rfc6960.txt"
}

foreach ($entry in $rfcs.GetEnumerator()) {
    $dest = Join-Path $RefPki $entry.Key
    Write-Host "Fetching $($entry.Key)..."
    Invoke-WebRequest -Uri $entry.Value -OutFile $dest -UseBasicParsing
    Write-Host "  OK $($entry.Key) ($((Get-Item $dest).Length) bytes)"
}

Write-Host "RFC sync complete -> $RefPki" -ForegroundColor Green
