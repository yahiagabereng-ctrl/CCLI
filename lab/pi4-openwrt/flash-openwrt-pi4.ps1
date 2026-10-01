# Flash OpenWrt 24.10.5 factory image to Pi 4 SD card (Disk 1).
# RUN AS ADMINISTRATOR. ERASES THE ENTIRE SD CARD.
# Image: openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img

$ErrorActionPreference = 'Stop'
$Image = Join-Path $PSScriptRoot 'openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img'
$DiskNumber = 1

if (-not (Test-Path $Image)) {
    Write-Host "Missing image. Decompressing .img.gz ..."
    wsl gzip -dkf "/mnt/c/Yahia/projects/CCLI/lab/pi4-openwrt/openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img.gz"
}

$disk = Get-Disk -Number $DiskNumber
Write-Host ""
Write-Host "=== OpenWrt Pi 4 flash ===" -ForegroundColor Cyan
Write-Host "Target disk : $($disk.Number) - $($disk.FriendlyName) ($([math]::Round($disk.Size/1GB,2)) GB)"
Write-Host "Image       : $Image ($([math]::Round((Get-Item $Image).Length/1MB,1)) MB)"
Write-Host ""
Write-Host "WARNING: ALL DATA ON THIS SD CARD WILL BE DESTROYED." -ForegroundColor Red
Write-Host "Expected: Mass Storage Device ~57-64 GB (Raspberry Pi SD)"
Write-Host ""
$confirm = Read-Host "Type YES to flash Disk $DiskNumber"
if ($confirm -ne 'YES') { Write-Host "Aborted."; exit 1 }

Write-Host "Clearing read-only ..."
Set-Disk -Number $DiskNumber -IsReadOnly $false -ErrorAction SilentlyContinue

Write-Host "Writing image (1-3 minutes) ..."
$phys = "\\.\PHYSICALDRIVE$DiskNumber"
$imgBytes = [System.IO.File]::ReadAllBytes($Image)
$fs = [System.IO.File]::Open($phys, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Write)
try {
    $fs.Write($imgBytes, 0, $imgBytes.Length)
    $fs.Flush()
} finally {
    $fs.Close()
}

Write-Host ""
Write-Host "Flash complete." -ForegroundColor Green
Write-Host "Next:"
Write-Host "  1. Eject SD, insert in Pi 4, Ethernet to lab switch, power on"
Write-Host "  2. Wait 2 min, then: ssh root@192.168.1.1"
Write-Host "  3. Run: lab\pi4-openwrt\openwrt-first-boot.sh (copy to Pi or paste commands)"
Write-Host ""
pause
