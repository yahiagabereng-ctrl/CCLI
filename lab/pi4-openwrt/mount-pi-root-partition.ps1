# Mount Raspberry Pi SD card ext4 root (partition 2) via WSL2.
# Run as Administrator (right-click -> Run with PowerShell).
# SD card = PHYSICALDRIVE1 (~64 GB); D: = boot (partition 1).

$ErrorActionPreference = 'Stop'
$Disk = '\\.\PHYSICALDRIVE1'

Write-Host "Mounting Pi root (ext4) partition 2 on $Disk ..."
wsl --unmount $Disk 2>$null
wsl --mount $Disk --partition 2 --type ext4
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: wsl --mount failed (exit $LASTEXITCODE). Is WSL2 installed? SD inserted?"
    pause
    exit 1
}

$distro = (wsl -l -q | Select-Object -First 1).Trim()
$winPath = "\\wsl$\$distro\mnt\wsl\PHYSICALDRIVE1p2"
Write-Host ""
Write-Host "Mounted. Open in Explorer:"
Write-Host "  $winPath"
Write-Host ""
Write-Host "Your home folder:"
Write-Host "  $winPath\home\yahia"
Write-Host ""
Write-Host "To unmount later (Admin PowerShell):"
Write-Host "  wsl --unmount $Disk"
Write-Host ""

if (Test-Path $winPath) {
    explorer.exe $winPath
} else {
    Write-Host "Path not visible yet — try in Explorer: $winPath"
}

pause
