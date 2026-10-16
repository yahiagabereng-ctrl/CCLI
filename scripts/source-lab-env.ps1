# Load frozen lab credentials (logger or TSP PC). Idempotent.
# Usage: powershell -File scripts\source-lab-env.ps1

$ErrorActionPreference = "Stop"

function Get-CcliRepoRoot {
    if ($env:CCLI_REPO_ROOT -and (Test-Path (Join-Path $env:CCLI_REPO_ROOT "apps\ccli\VERSION"))) {
        return $env:CCLI_REPO_ROOT
    }
    $here = $PSScriptRoot
    for ($i = 0; $i -lt 6; $i++) {
        $candidate = $here
        for ($j = 0; $j -lt $i; $j++) { $candidate = Split-Path $candidate -Parent }
        if (Test-Path (Join-Path $candidate "apps\ccli\VERSION")) { return $candidate }
    }
    return $null
}

$sourced = $false

$repo = Get-CcliRepoRoot
if ($repo) {
    $local = Join-Path $repo "lab\tg544-openwrt\lab-env.ps1"
    $example = Join-Path $repo "lab\tg544-openwrt\lab-env.example.ps1"
    if (Test-Path $local) {
        . $local
        $sourced = $true
        Write-Verbose "Sourced $local"
    } elseif (Test-Path $example) {
        . $example
        $sourced = $true
        Write-Host "Using frozen lab-env.example.ps1 (run ensure-lab-env.ps1 to create lab-env.ps1)" -ForegroundColor Yellow
    }
}

if (-not $sourced -and $env:CCLI_TSP_FIELD_PACK) {
    $packEnv = Join-Path $env:CCLI_TSP_FIELD_PACK "bench_scripts\lab-env.ps1"
    if (Test-Path $packEnv) {
        . $packEnv
        $sourced = $true
        Write-Verbose "Sourced $packEnv"
    }
}

if (-not $sourced) {
    throw "Could not source lab env. Clone CCLI repo or set CCLI_TSP_FIELD_PACK to a staged field pack with bench_scripts\lab-env.ps1. See lab/LAB_ACCESS_FREEZE.md"
}

if (-not $env:CCLI_TG544_PW) {
    throw "CCLI_TG544_PW not set after sourcing lab env"
}
