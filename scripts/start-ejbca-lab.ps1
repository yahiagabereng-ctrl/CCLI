# Start EJBCA Community Edition for CCLI lab PKI.
param(
    [switch]$Stop,
    [switch]$Logs,
    [switch]$Status
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$ComposeDir = Join-Path $Repo "knowledge-base\08-engineering\reference\vendor\ejbca\docker"
$ComposeFile = Join-Path $ComposeDir "docker-compose.yml"

function Show-Status {
    Write-Host "=== EJBCA lab status ===" -ForegroundColor Cyan
    docker ps -a --filter "name=ccli-ejbca-ce" --format "table {{.Names}}\t{{.Status}}\t{{.Ports}}"
    Write-Host ""
    Write-Host "Admin UI:  https://localhost:8443/ejbca/adminweb/"
    Write-Host "Public UI: https://localhost:8443/ejbca/"
    Write-Host "Runbook:   lab/EJBCA_LAB_SETUP.md"
}

if ($Status) {
    Show-Status
    exit 0
}

if ($Stop) {
    Push-Location $ComposeDir
    docker compose down
    Pop-Location
    Write-Host "EJBCA stopped." -ForegroundColor Yellow
    exit 0
}

if ($Logs) {
    docker logs -f ccli-ejbca-ce
    exit 0
}

if (-not (docker images keyfactor/ejbca-ce --format "{{.ID}}" 2>$null)) {
    Write-Host "Pulling keyfactor/ejbca-ce:latest ..."
    docker pull keyfactor/ejbca-ce:latest
}

Push-Location $ComposeDir
docker compose up -d
Pop-Location

Write-Host "Waiting for EJBCA (first boot can take 2-5 min)..." -ForegroundColor Yellow
$ready = $false
for ($i = 0; $i -lt 60; $i++) {
    Start-Sleep -Seconds 5
    $code = curl.exe -k -s -o NUL -w "%{http_code}" "https://localhost:8443/ejbca/publicweb/healthcheck/ejbcahealth" 2>$null
    if ($code -eq "200") {
        $ready = $true
        break
    }
    Write-Host "." -NoNewline
}
Write-Host ""

if ($ready) {
    Write-Host "EJBCA is responding on https://localhost:8443" -ForegroundColor Green
}
else {
    Write-Warning "EJBCA not yet HTTP-ready. Check: scripts/start-ejbca-lab.ps1 -Logs"
}

Show-Status
