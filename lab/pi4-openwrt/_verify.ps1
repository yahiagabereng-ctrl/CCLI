$log = "C:\Yahia\projects\CCLI\lab\pi4-openwrt\sd-verify.txt"
"=== SD verify $(Get-Date -Format o) ===" | Out-File $log
Get-Partition -DiskNumber 1 | Out-File $log -Append
$p = Get-Partition -DiskNumber 1 -PartitionNumber 1
if (-not $p.DriveLetter) { Set-Partition -DiskNumber 1 -PartitionNumber 1 -NewDriveLetter D -ErrorAction SilentlyContinue }
Start-Sleep 1
if (Test-Path D:\cmdline.txt) {
  "cmdline:" | Out-File $log -Append
  Get-Content D:\cmdline.txt | Out-File $log -Append
}
if (Test-Path D:\issue.txt) { "issue:" | Out-File $log -Append; Get-Content D:\issue.txt | Out-File $log -Append }
Get-ChildItem D:\ -ErrorAction SilentlyContinue | Select-Object Name, Length | Out-File $log -Append
$img = "C:\Yahia\projects\CCLI\lab\pi4-openwrt\openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img"
$sd = [byte[]]::new(512); $imgB = [byte[]]::new(512)
$fs = [IO.File]::OpenRead("\\.\PHYSICALDRIVE1"); $fs.Read($sd,0,512)|Out-Null; $fs.Close()
$ir = [IO.File]::OpenRead($img); $ir.Read($imgB,0,512)|Out-Null; $ir.Close()
"MBR matches OpenWrt image: $(($sd[0..511]-join',') -eq ($imgB[0..511]-join','))" | Out-File $log -Append
