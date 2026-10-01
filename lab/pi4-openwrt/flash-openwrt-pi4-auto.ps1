# Non-interactive OpenWrt flash — self-elevates to Administrator
$ErrorActionPreference = 'Stop'
$Log = 'C:\Yahia\projects\CCLI\lab\pi4-openwrt\flash-result.txt'

if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Start-Process powershell.exe -Verb RunAs -Wait -ArgumentList "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`""
    exit $LASTEXITCODE
}
$Image = Join-Path $PSScriptRoot 'openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img'
$DiskNumber = 1

function Log($msg) { $msg | Tee-Object -FilePath $Log -Append }

Log "=== OpenWrt reflash $(Get-Date -Format o) ==="

if (-not (Test-Path $Image)) {
    Log "Decompressing .img.gz ..."
    wsl gzip -dkf "/mnt/c/Yahia/projects/CCLI/lab/pi4-openwrt/openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img.gz"
}

$disk = Get-Disk -Number $DiskNumber
if ($disk.FriendlyName -notmatch 'Mass Storage|USB|SD|Flash') {
    Log "ERROR: Disk $DiskNumber is '$($disk.FriendlyName)' — expected SD card. ABORT."
    exit 1
}

Log "Target: Disk $DiskNumber — $($disk.FriendlyName) ($([math]::Round($disk.Size/1GB,2)) GB)"
Log "Image : $Image ($([math]::Round((Get-Item $Image).Length/1MB,1)) MB)"

Get-Partition -DiskNumber $DiskNumber -ErrorAction SilentlyContinue | Where-Object DriveLetter | ForEach-Object {
    Log "Dismount $($_.DriveLetter):"
    mountvol "$($_.DriveLetter):" /D 2>$null
}

Set-Disk -Number $DiskNumber -IsReadOnly $false -ErrorAction SilentlyContinue

Log "Writing raw image ..."
$phys = "\\.\PHYSICALDRIVE$DiskNumber"
$imgBytes = [System.IO.File]::ReadAllBytes($Image)
$fs = [System.IO.File]::Open($phys, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Write)
try {
    $fs.Write($imgBytes, 0, $imgBytes.Length)
    $fs.Flush()
} finally {
    $fs.Close()
}

# Verify write
$sd = [byte[]]::new(512)
$imgB = [byte[]]::new(512)
$fsr = [IO.File]::OpenRead($phys)
$fsr.Read($sd, 0, 512) | Out-Null; $fsr.Close()
$imgBytes[0..511].CopyTo($imgB, 0)
$match = ($sd[0..511] -join ',') -eq ($imgB[0..511] -join ',')
Log "MBR verify match image: $match"

Get-Partition -DiskNumber $DiskNumber -PartitionNumber 1 | Set-Partition -NewDriveLetter D -ErrorAction SilentlyContinue
Start-Sleep 1
if (Test-Path D:\cmdline.txt) {
    Log "cmdline.txt: $((Get-Content D:\cmdline.txt -Raw).Trim())"
}

Log "FLASH OK — eject SD, insert in Pi 4, power on, wait 3 min"
Log "Then: ssh root@192.168.1.1  (or root@192.168.10.20 if DHCP)"
