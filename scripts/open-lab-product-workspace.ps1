# Open CCLI lab + Official Product multi-root workspace in Cursor.
$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Ws = Join-Path $Repo "CCLI-Lab-And-Product.code-workspace"

if (-not (Test-Path $Ws)) {
    throw "Missing workspace file: $Ws"
}

$cursorCmd = Get-Command cursor -ErrorAction SilentlyContinue
if ($cursorCmd) {
    & cursor $Ws
    exit 0
}

$cursorExe = Join-Path $env:LOCALAPPDATA "Programs\cursor\Cursor.exe"
if (Test-Path $cursorExe) {
    Start-Process $cursorExe -ArgumentList "`"$Ws`""
    exit 0
}

Write-Host "Open manually: $Ws" -ForegroundColor Yellow
Invoke-Item $Ws
