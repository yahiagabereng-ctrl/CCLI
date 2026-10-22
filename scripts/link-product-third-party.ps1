# Create junction product/apps/ccli/third_party -> apps/ccli/third_party (single vendor tree).
$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$LabTp = Join-Path $Repo "apps\ccli\third_party"
$ProdTp = Join-Path $Repo "product\apps\ccli\third_party"

if (-not (Test-Path $LabTp)) {
    throw "Missing lab third_party: $LabTp"
}
if (-not (Test-Path (Join-Path $Repo "product\apps\ccli"))) {
    throw "Missing product tree: product\apps\ccli — see product\README.md"
}

if (Test-Path $ProdTp) {
    $item = Get-Item $ProdTp -Force
    if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint) {
        Write-Host "OK: junction already exists -> $LabTp" -ForegroundColor Green
        exit 0
    }
    throw "product/apps/ccli/third_party exists and is not a junction — move aside manually"
}

New-Item -ItemType Directory -Force -Path (Split-Path $ProdTp) | Out-Null
cmd /c mklink /J "$ProdTp" "$LabTp"
Write-Host "Created junction: $ProdTp -> $LabTp" -ForegroundColor Green
