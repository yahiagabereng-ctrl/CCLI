$log = "C:\Yahia\projects\CCLI\lab\pi4-openwrt\sd-check.txt"
"" | Out-File $log
"=== SD check $(Get-Date -Format o) ===" | Out-File $log -Append
Get-Partition -DiskNumber 1 | Out-File $log -Append
# Assign D: to boot if missing
$p = Get-Partition -DiskNumber 1 -PartitionNumber 1
if (-not $p.DriveLetter) {
  Set-Partition -DiskNumber 1 -PartitionNumber 1 -NewDriveLetter D
  "Assigned D:" | Out-File $log -Append
}
Start-Sleep 2
if (Test-Path D:\) {
  "Boot files:" | Out-File $log -Append
  Get-ChildItem D:\ -Force | Out-File $log -Append
  if (Test-Path D:\config.txt) { "config.txt:" | Out-File $log -Append; Get-Content D:\config.txt | Out-File $log -Append }
  if (Test-Path D:\cmdline.txt) { "cmdline.txt:" | Out-File $log -Append; Get-Content D:\cmdline.txt | Out-File $log -Append }
}
# Compare first 512 bytes SD vs image
$img = "C:\Yahia\projects\CCLI\lab\pi4-openwrt\openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img"
$sd = [byte[]]::new(512)
$imgB = [byte[]]::new(512)
$fs = [IO.File]::OpenRead("\\.\PHYSICALDRIVE1")
$fs.Read($sd,0,512) | Out-Null; $fs.Close()
$ir = [IO.File]::OpenRead($img)
$ir.Read($imgB,0,512) | Out-Null; $ir.Close()
$match = ($sd[0..511] -join ',') -eq ($imgB[0..511] -join ',')
"MBR first 512 bytes match image: $match" | Out-File $log -Append
