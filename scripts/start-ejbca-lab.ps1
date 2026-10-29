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
    Write-Host "Admin UI:  https://localhost:9443/ejbca/adminweb/"
    Write-Host "Public UI: https://localhost:9443/ejbca/"
    Write-Host "Runbook:   lab/EJBCA_LAB_SETUP.md"
}

function Disable-EjbcaOptionalClientAuth {
    # Edge ERR_EMPTY_RESPONSE: want-client-auth=true makes Chromium abort TLS.
    $staging = Join-Path $Repo "lab\ejbca-staging"
    New-Item -ItemType Directory -Force -Path $staging | Out-Null
    $xmlLocal = Join-Path $staging "standalone.xml"
    docker cp "ccli-ejbca-ce:/opt/keyfactor/appserver/standalone/configuration/standalone.xml" $xmlLocal
    $text = Get-Content -Raw -Path $xmlLocal
    $old = 'want-client-auth="true" need-client-auth="false" authentication-optional="true"'
    $new = 'want-client-auth="false" need-client-auth="false" authentication-optional="false"'
    if ($text -notlike "*$old*") {
        if ($text -like "*$new*") {
            Write-Host "HTTPS already has client-auth disabled (Edge-safe)." -ForegroundColor DarkGray
            return
        }
        Write-Warning "Could not find want-client-auth pattern in standalone.xml"
        return
    }
    Set-Content -Path $xmlLocal -Value ($text.Replace($old, $new)) -NoNewline -Encoding utf8
    docker cp $xmlLocal "ccli-ejbca-ce:/opt/keyfactor/appserver/standalone/configuration/standalone.xml"
    docker exec ccli-ejbca-ce /opt/keyfactor/appserver/bin/jboss-cli.sh --connect --command=':reload' | Out-Null
    for ($i = 0; $i -lt 40; $i++) {
        Start-Sleep -Seconds 3
        $code = curl.exe -k -s -o NUL -w "%{http_code}" "https://localhost:9443/ejbca/publicweb/healthcheck/ejbcahealth" 2>$null
        if ($code -eq "200") {
            Write-Host "Disabled optional client-auth (fixes Edge ERR_EMPTY_RESPONSE)." -ForegroundColor Green
            return
        }
    }
    Write-Warning "WildFly reload after client-auth patch did not become healthy in time."
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
    $code = curl.exe -k -s -o NUL -w "%{http_code}" "https://localhost:9443/ejbca/adminweb/" 2>$null
    if ($code -match "^(200|302)$") {
        $ready = $true
        break
    }
    Write-Host "." -NoNewline
}
Write-Host ""

if ($ready) {
    Write-Host "EJBCA is responding on https://localhost:9443" -ForegroundColor Green
    # Edge/Chrome often return ERR_EMPTY_RESPONSE when WildFly asks for optional
    # client certificates (want-client-auth=true). Lab simple mode does not need that.
    try {
        Disable-EjbcaOptionalClientAuth
    }
    catch {
        Write-Warning "Could not disable optional client-auth (Admin Web may fail in Edge): $($_.Exception.Message)"
    }
}
else {
    Write-Warning "EJBCA not yet HTTP-ready. Check: scripts/start-ejbca-lab.ps1 -Logs"
}

Show-Status
