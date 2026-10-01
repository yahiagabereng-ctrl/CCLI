# Sync standards/protocol extracts to Telematry and enqueue RAG jobs (-ProjectOnly).
param(
    [string] $TelematryRoot = "C:\Yahia\projects\Telematry System",
    [string] $RagBase = "http://localhost:8001",
    [switch] $SyncOnly,
    [switch] $DryRun
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot

& (Join-Path $Repo "scripts\sync-ccli-corpus-to-telematry.ps1") -TelematryRoot $TelematryRoot

if ($SyncOnly) {
    Write-Host "SyncOnly: Telematry documents updated; RAG ingest skipped." -ForegroundColor Yellow
    exit 0
}

$ingest = Join-Path $TelematryRoot "scripts\ingest-ccli.ps1"
if (-not (Test-Path $ingest)) {
    Write-Error "Missing $ingest"
}

try {
    Invoke-RestMethod -Uri "$RagBase/health" -Method Get -TimeoutSec 5 | Out-Null
}
catch {
    Write-Warning "RAG not reachable at $RagBase. Corpus synced only. Start Telematry stack and run:"
    Write-Host "  cd `"$TelematryRoot`"; .\scripts\ingest-ccli.ps1 -ProjectOnly"
    exit 0
}

if ($DryRun) {
    & $ingest -ProjectOnly -DryRun
}
else {
    & $ingest -ProjectOnly
}
