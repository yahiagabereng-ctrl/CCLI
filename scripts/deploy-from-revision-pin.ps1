# Deploy TG544 using a lab revision pin (yaml + TLS + ICD + version checks).
#
# Usage:
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\deploy-from-revision-pin.ps1 -Profile P3-07-TSP-CONNECT -WhatIf
#   powershell -File scripts\deploy-from-revision-pin.ps1 -Profile UNIFIED-5LN-CERT -HostAddr 192.168.1.130
#   powershell -File scripts\deploy-from-revision-pin.ps1 -Profile R51-P3-07-SOURCE -ApplyVersion -WhatIf

param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("R51-P3-07-SOURCE", "P3-07-TSP-CONNECT", "UNIFIED-5LN-CERT", "PRODUCT-5LN-CERT")]
    [string]$Profile,
    [string]$HostAddr = "",
    [switch]$WhatIf,
    [switch]$ApplyVersion,
    [switch]$AllowVersionMismatch,
    [switch]$SkipDeploy
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $env:CCLI_TG544_PW) {
    & (Join-Path $PSScriptRoot "source-lab-env.ps1") | Out-Null
}
$PinDir = Join-Path $RepoRoot "lab\tg544-openwrt\revision-pins"
$PinPath = Join-Path $PinDir "$Profile.json"
if (-not (Test-Path $PinPath)) { throw "Missing pin: $PinPath" }

$pin = Get-Content -Raw -Path $PinPath | ConvertFrom-Json
$VersionFile = Join-Path $RepoRoot "apps\ccli\VERSION"
$Bin = Join-Path $RepoRoot "lab\tg544-openwrt\ccli-bin"
$DeployScript = Join-Path $RepoRoot "lab\tg544-openwrt\deploy-ccli-session-fix.ps1"

function Resolve-RepoPath {
    param([string]$Rel)
    if ([string]::IsNullOrWhiteSpace($Rel)) { return $null }
    $p = Join-Path $RepoRoot ($Rel -replace '/', '\')
    if (-not (Test-Path $p)) { return $null }
    return $p
}

$labYaml = Resolve-RepoPath $pin.deploy.lab_yaml
$tlsDir = Resolve-RepoPath $pin.deploy.tls_dir
$icdCfg = Resolve-RepoPath $pin.deploy.icd_cfg

if (-not $HostAddr) {
    $HostAddr = $pin.deploy.host_default
}

Write-Host "=== Revision pin: $($pin.profile_id) ===" -ForegroundColor Cyan
Write-Host $pin.title
Write-Host ""

$fail = 0

# VERSION check
$vlines = Get-Content $VersionFile | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne "" }
$curSem = $vlines[0]
$curRel = $vlines[1]
$curCode = if ($vlines.Count -gt 2) { ($vlines[2..($vlines.Count - 1)] -join " ") } else { "" }
$wantRel = [string]$pin.version.release
$wantCode = [string]$pin.version.codename

if ($ApplyVersion -and $Profile -eq "R51-P3-07-SOURCE") {
    $newBody = @(
        $pin.version.semver
        $pin.version.release
        $pin.version.codename
    ) -join "`n"
    if (-not $WhatIf) {
        Set-Content -Path $VersionFile -Value ($newBody + "`n") -Encoding UTF8 -NoNewline
        & (Join-Path $RepoRoot "scripts\sync-ccli-version.ps1")
        Write-Host "Applied VERSION from pin (r$wantRel)." -ForegroundColor Green
        $curRel = $wantRel
        $curCode = $wantCode
    } else {
        Write-Host "[WhatIf] Would write VERSION -> 0.1.0 / $wantRel / $wantCode" -ForegroundColor Yellow
    }
}

if ($curRel -ne $wantRel -and -not $AllowVersionMismatch) {
    Write-Host "FAIL REQ-VER-001: apps/ccli/VERSION release is $curRel; pin expects $wantRel ($wantCode)" -ForegroundColor Red
    Write-Host "  Rebuild after fixing VERSION, or use -ApplyVersion for R51-P3-07-SOURCE, or -AllowVersionMismatch (document why)." -ForegroundColor Yellow
    $fail++
} else {
    Write-Host "OK  VERSION 0.1.0-r$curRel ($curCode)" -ForegroundColor Green
}

# Git advisory
$head = (git -C $RepoRoot rev-parse --short HEAD 2>$null)
if ($pin.source_git_commit_short -and $head -and $head -ne $pin.source_git_commit_short) {
    Write-Host "WARN git HEAD $head != pin $($pin.source_git_commit_short) - source may differ from reference binary" -ForegroundColor Yellow
}

# Paths
foreach ($label in @(
        @{ N = "lab_yaml"; P = $labYaml; R = $pin.deploy.lab_yaml }
        @{ N = "tls_dir"; P = $tlsDir; R = $pin.deploy.tls_dir }
        @{ N = "icd_cfg"; P = $icdCfg; R = $pin.deploy.icd_cfg }
    )) {
    if (-not $label.P) {
        Write-Host "FAIL missing $($label.N): $($label.R)" -ForegroundColor Red
        $fail++
    } else {
        Write-Host "OK  $($label.N): $($label.R)" -ForegroundColor Green
    }
}

if (-not (Test-Path $Bin)) {
    Write-Host "FAIL missing ccli-bin - build first" -ForegroundColor Red
    $fail++
} elseif ($pin.reference_binary.sha256) {
    $hash = (Get-FileHash -Algorithm SHA256 -Path $Bin).Hash.ToLower()
    $ref = [string]$pin.reference_binary.sha256
    if ($hash -eq $ref) {
        Write-Host "OK  binary SHA256 matches pin reference" -ForegroundColor Green
    } else {
        Write-Host "WARN binary SHA256 $hash" -ForegroundColor Yellow
        Write-Host "     pin reference $ref (rebuild expected unless archiving exact bytes)" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "--- Pin requirements ---" -ForegroundColor Cyan
$pin.requirements | ForEach-Object { Write-Host "  $_" }

$changes = @()
if ($pin.deploy_script.example_changes) {
    $changes = @($pin.deploy_script.example_changes)
} else {
    $changes = @("$Profile deploy")
}

$deployCmd = @"
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr $HostAddr `
  -LabYaml $($pin.deploy.lab_yaml -replace '/', '\') `
  -TlsDir $($pin.deploy.tls_dir -replace '/', '\') `
  -Changes @('$($changes -join "', '")')
"@

Write-Host ""
Write-Host "--- Deploy command ---" -ForegroundColor Cyan
Write-Host $deployCmd

if ($WhatIf -or $SkipDeploy) {
    if ($fail -gt 0) {
        Write-Host "[WhatIf] Preflight issues: $fail (fix before real deploy)." -ForegroundColor Yellow
    }
    Write-Host "[WhatIf] No upload." -ForegroundColor Yellow
    exit $(if ($fail -gt 0) { 2 } else { 0 })
}

if ($fail -gt 0) {
    throw "Pin preflight failed ($fail). Fix above before deploy."
}

if (-not $env:CCLI_TG544_PW) {
    throw "Set `$env:CCLI_TG544_PW (source lab-env.ps1)"
}

& $DeployScript -HostAddr $HostAddr `
    -LabYaml (Join-Path $RepoRoot ($pin.deploy.lab_yaml -replace '/', '\')) `
    -TlsDir (Join-Path $RepoRoot ($pin.deploy.tls_dir -replace '/', '\')) `
    -DeployProfile $Profile `
    -Changes $changes

$verify = $pin.verify_scripts | Select-Object -First 1
if ($verify) {
    $vPath = Join-Path $RepoRoot ($verify -replace '/', '\')
    if (Test-Path $vPath) {
        Write-Host "=== Suggested verify: $verify ===" -ForegroundColor Cyan
        if ($verify -match "verify-dut-p3-07") {
            Write-Host "powershell -File $verify -HostAddr $HostAddr"
        } elseif ($verify -match "validate-dut-unified") {
            Write-Host "powershell -File $verify -FixServices -WriteEvidence"
        } else {
            Write-Host "powershell -File $verify -HostAddr $HostAddr"
        }
    }
}
