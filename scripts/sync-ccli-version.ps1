# Sync OpenWrt PKG_VERSION/PKG_RELEASE from apps/ccli/VERSION
# Usage: .\scripts\sync-ccli-version.ps1

$ErrorActionPreference = "Stop"
$Root = Split-Path $PSScriptRoot -Parent
$VersionFile = Join-Path $Root "apps\ccli\VERSION"
$Makefile = Join-Path $Root "package\ccli\Makefile"

if (-not (Test-Path $VersionFile)) { throw "Missing $VersionFile" }
if (-not (Test-Path $Makefile)) { throw "Missing $Makefile" }

$lines = Get-Content $VersionFile | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne "" }
if ($lines.Count -lt 2) { throw "VERSION needs semver + release lines" }

$semver = $lines[0]
$release = $lines[1]
if ($release -notmatch '^\d+$') { throw "Release must be integer: $release" }

$content = Get-Content $Makefile -Raw
$content = [regex]::Replace($content, '(?m)^PKG_VERSION:=.*$', "PKG_VERSION:=$semver")
$content = [regex]::Replace($content, '(?m)^PKG_RELEASE:=.*$', "PKG_RELEASE:=$release")
Set-Content -Path $Makefile -Value $content -NoNewline

Write-Host "Synced package/ccli/Makefile -> ccli-${semver}-r${release}"
Write-Host "Update apps/ccli/CHANGELOG.md before deploy."
