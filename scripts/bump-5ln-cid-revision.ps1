# Append SCL History + bump Header revision + LLN0.NamPlt.configRev after editing 5-LN product CID.
# Usage:
#   powershell -File scripts\bump-5ln-cid-revision.ps1 -What "Describe model change" [-Why "Compare/TSP reason"] [-RegenCfg] [-SyncProduct]
#
# Then: validate-5ln-cid.ps1, update PRODUCT_SPECIMEN_5LN.md SHAs, validate-5ln-stack.ps1 pin block.

param(
    [Parameter(Mandatory = $true)]
    [string]$What,
    [string]$Why = "PRODUCT_SPECIMEN_5LN maintenance",
    [string]$Who = "CCLI",
    [string]$CidPath = "",
    [string]$ConfigRev = "",
    [switch]$RegenCfg,
    [switch]$SyncProduct,
    [switch]$DryRun
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
if (-not $CidPath) {
    $CidPath = Join-Path $Repo "apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cid"
} elseif (-not [System.IO.Path]::IsPathRooted($CidPath)) {
    $CidPath = Join-Path $Repo ($CidPath -replace '/', '\')
}
if (-not (Test-Path $CidPath)) { throw "CID not found: $CidPath" }

function Escape-SclAttr {
    param([string]$s)
    return ($s -replace '&', '&amp;' -replace '"', '&quot;' -replace '<', '&lt;')
}

$raw = Get-Content -Raw -Encoding UTF8 $CidPath
$when = Get-Date -Format "yyyy-MM-dd"
if (-not $ConfigRev) {
    $ConfigRev = Get-Date -Format "yyyyMMdd"
}

if ($raw -notmatch '<Header[^>]*\sversion="(\d+)"\s+revision="(\d+)"') {
    throw "Could not parse Header version/revision in $CidPath"
}
$hdrVersion = $Matches[1]
$hdrRevision = [int]$Matches[2]
$newRevision = $hdrRevision + 1

$oldConfigRev = $null
if ($raw -match '<DOI name="NamPlt"[\s\S]*?<DAI name="configRev">\s*<Val>(\d+)</Val>') {
    $oldConfigRev = $Matches[1]
}

Write-Host "5-LN CID revision bump: $CidPath" -ForegroundColor Cyan
Write-Host "  Header revision: $hdrRevision -> $newRevision"
Write-Host "  configRev:       $(if ($oldConfigRev) { $oldConfigRev } else { '?' }) -> $ConfigRev"
Write-Host "  when:            $when"

$whatEsc = Escape-SclAttr $What
$whyEsc = Escape-SclAttr $Why
$whoEsc = Escape-SclAttr $Who

$newHitem = "<Hitem version=`"$hdrVersion`" revision=`"$newRevision`" when=`"$when`" what=`"$whatEsc`" who=`"$whoEsc`" why=`"$whyEsc`" />"

$updated = $raw
$updated = [regex]::Replace(
    $updated,
    '(<Header\s+id="[^"]*"\s+version="\d+"\s+)revision="\d+"',
    "`${1}revision=`"$newRevision`"",
    1
)
if ($updated -notmatch '</History>') { throw "Missing </History> in CID" }
$updated = $updated -replace '</History>', "$newHitem`r`n</History>"

$updated = [regex]::Replace(
    $updated,
    '(<DOI name="NamPlt"[\s\S]*?<DAI name="configRev">\s*<Val>)\d+(</Val>)',
    "`${1}$ConfigRev`${2}",
    1
)

if ($DryRun) {
    Write-Host "DryRun: no file written." -ForegroundColor Yellow
    Write-Host "New Hitem: $newHitem"
    exit 0
}

Set-Content -Path $CidPath -Value $updated -Encoding UTF8 -NoNewline
Write-Host "Updated: $CidPath" -ForegroundColor Green

$targets = @($CidPath)
if ($SyncProduct) {
    $prodCid = Join-Path $Repo "product\apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cid"
    if (Test-Path (Split-Path $prodCid -Parent)) {
        Copy-Item -Force $CidPath $prodCid
        $targets += $prodCid
        Write-Host "Synced:  $prodCid" -ForegroundColor Green
    }
}

if ($RegenCfg) {
    $cfgOut = Join-Path $Repo "apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cfg"
    & powershell -NoProfile -File (Join-Path $Repo "scripts\gen-mms-model-cfg.ps1") `
        -Cid $CidPath -Out $cfgOut -Ied "CCI016_01" -Ap "accessPoint1"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    if ($SyncProduct) {
        $prodCfg = Join-Path $Repo "product\apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cfg"
        if (Test-Path (Split-Path $prodCfg -Parent)) {
            Copy-Item -Force $cfgOut $prodCfg
        }
    }
}

& powershell -NoProfile -File (Join-Path $Repo "scripts\validate-5ln-cid.ps1") -CidPath $CidPath
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$cidSha = (Get-FileHash $CidPath -Algorithm SHA256).Hash.ToUpper()
Write-Host ""
Write-Host "=== Post-bump checklist ===" -ForegroundColor Yellow
Write-Host "1. PRODUCT_SPECIMEN_5LN.md: History rev $newRevision, configRev $ConfigRev, CID SHA-256 $cidSha"
if ($RegenCfg) {
    $cfgPath = Join-Path $Repo "apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cfg"
    $cfgSha = (Get-FileHash $cfgPath -Algorithm SHA256).Hash.ToUpper()
    $cfgBytes = (Get-Item $cfgPath).Length
    Write-Host "2. PRODUCT_SPECIMEN_5LN.md: cfg SHA-256 $cfgSha ($cfgBytes bytes)"
    Write-Host "3. scripts/validate-5ln-stack.ps1: update pinCidSha / pinCfgSha"
    Write-Host "4. DUT: push-5ln-product-cfg-to-dut.ps1; enable-dut-luci-iec61850.ps1; TSP re-import CID"
} else {
    Write-Host "2. Run: powershell -File scripts\gen-mms-model-cfg.ps1 -Cid ... -Out ...\product_tr400_eth_a_5ln.cfg"
}
Write-Host "Targets touched: $($targets -join ', ')"
