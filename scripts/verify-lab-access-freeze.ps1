# Verify frozen SSH env + optional p3_07 TLS MD5 on a folder.
# Usage:
#   powershell -File scripts\source-lab-env.ps1
#   powershell -File scripts\verify-lab-access-freeze.ps1
#   powershell -File scripts\verify-lab-access-freeze.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls

param(
    [string]$TlsDir = ""
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$FreezeJson = Join-Path $Repo "lab\tg544-openwrt\lab-access.freeze.json"
$Example = Join-Path $Repo "lab\tg544-openwrt\lab-env.example.ps1"
if (-not (Test-Path $FreezeJson)) { throw "Missing $FreezeJson" }
$freeze = Get-Content -Raw $FreezeJson | ConvertFrom-Json

$fail = 0

if (-not $env:CCLI_TG544_PW) {
    Write-Host "FAIL: CCLI_TG544_PW not set - run source-lab-env.ps1" -ForegroundColor Red
    $fail++
} elseif (Test-Path $Example) {
    $exampleRaw = Get-Content $Example -Raw
    if ($exampleRaw -match "CCLI_TG544_PW = '(.+?)'") {
        $frozenPw = $Matches[1]
        if ($env:CCLI_TG544_PW -ne $frozenPw) {
            Write-Host "FAIL: CCLI_TG544_PW differs from lab-env.example.ps1 freeze" -ForegroundColor Red
            $fail++
        } else {
            Write-Host "OK: CCLI_TG544_PW matches access freeze" -ForegroundColor Green
        }
    }
}

if (-not $TlsDir) {
    if ($env:CCLI_TSP_FIELD_PACK) {
        $TlsDir = Join-Path $env:CCLI_TSP_FIELD_PACK "tls"
    } else {
        $TlsDir = Join-Path $Repo "apps\ccli\config\tls\p3_07_connect"
    }
}

if (Test-Path $TlsDir) {
    Write-Host "TLS MD5 check: $TlsDir" -ForegroundColor Cyan
    foreach ($prop in $freeze.tsp_p3_07_connect.md5.PSObject.Properties) {
        $name = $prop.Name
        $want = $prop.Value.ToLower()
        $path = Join-Path $TlsDir $name
        if (-not (Test-Path $path)) {
            Write-Host "  SKIP missing $name" -ForegroundColor Yellow
            continue
        }
        $got = (Get-FileHash -Algorithm MD5 -Path $path).Hash.ToLower()
        if ($got -eq $want) {
            Write-Host "  OK  $name" -ForegroundColor Green
        } else {
            Write-Host "  FAIL $name (got $got want $want)" -ForegroundColor Red
            $fail++
        }
    }
} else {
    Write-Host "SKIP TLS dir not found: $TlsDir" -ForegroundColor Yellow
}

if ($fail -gt 0) { exit 1 }
Write-Host "Lab access freeze verification PASS" -ForegroundColor Green
