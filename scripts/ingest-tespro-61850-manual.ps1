# Ingest TesPro IEC61850-User-Manual.docx into knowledge-base corpus.
param(
    [string]$Source = "$env:USERPROFILE\OneDrive\Documents\xwechat_files\wxid_9o7d32f54k1922_1b1e\msg\file\2026-09\IEC61850-User-Manual.docx"
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$DestDir = Join-Path $Repo "knowledge-base\08-engineering\reference\vendor\tespro"
$Dest = Join-Path $DestDir "IEC61850-User-Manual.docx"

if (-not (Test-Path $Source)) {
    Write-Error "Source not found: $Source"
}

New-Item -ItemType Directory -Force -Path $DestDir | Out-Null
Copy-Item -Force $Source $Dest
Write-Host "Copied -> $Dest"

python (Join-Path $Repo "scripts\extract-tespro-61850-manual.py")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "Ingest complete. Extract: knowledge-base/08-engineering/CCI_TesPro_61850_Manual_Extract.md"
