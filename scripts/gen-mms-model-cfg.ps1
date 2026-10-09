# Generate libiec61850 model.cfg from lab CID (genconfig.jar).
param(
    [string]$Cid = (Join-Path $PSScriptRoot "..\apps\ccli\config\icd\lab_tg544_eth_a.cid"),
    [string]$Out = (Join-Path $PSScriptRoot "..\apps\ccli\config\icd\lab_tg544_eth_a.cfg"),
    [string]$Ied = "CCI016_01",
    [string]$Ap = "accessPoint1"
)

$ErrorActionPreference = "Stop"
$mg = Join-Path $PSScriptRoot "..\apps\ccli\third_party\libiec61850\tools\model_generator"
$jar = Join-Path $mg "genconfig.jar"

if (-not (Test-Path $jar)) {
    $javac = Get-Command javac -ErrorAction SilentlyContinue
    if (-not $javac) {
        throw "Install JDK (javac) then re-run, or commit genconfig.jar"
    }
    Push-Location $mg
    New-Item -ItemType Directory -Force -Path build | Out-Null
    Get-ChildItem -Recurse src -Filter *.java | ForEach-Object { $_.FullName } | Set-Content listFile.tmp
    & javac -target 1.8 -source 1.8 -d build "@listFile.tmp"
    & jar cfm genconfig.jar manifest-dynamic.mf -C build/ com/
    Remove-Item listFile.tmp, build -Recurse -Force -ErrorAction SilentlyContinue
    Pop-Location
}

if (-not (Test-Path $Cid)) { throw "CID not found: $Cid" }

& java -jar $jar $Cid -ied $Ied -ap $Ap $Out
if ($LASTEXITCODE -ne 0) { throw "genconfig failed exit=$LASTEXITCODE" }

# genconfig sets RCB options=191/255 with BUFFER_OVERFLOW on URCB; TSP Compare expects
# CID OptFields (URCB 159 = confRev, no entryID/bufOvfl; BRCB 191 = bufOvfl+confRev, no entryID).
$cfgText = Get-Content -Path $Out -Raw
$cfgText = $cfgText -replace '(urcb_[^\r\n]+?\s+\d+\s+\d+\s+)191(\s)', '${1}159${2}'
$cfgText = $cfgText -replace '(brcb_[^\r\n]+?\s+\d+\s+\d+\s+)255(\s)', '${1}191${2}'
Set-Content -Path $Out -Value $cfgText -NoNewline

$lnCount = (Select-String -Path $Out -Pattern '^\s*LN\(' -AllMatches).Count
Write-Host "Generated $Out ($lnCount logical nodes)"
Write-Host "Deploy to DUT: /etc/ccli/icd/lab_tg544_eth_a.cfg"
