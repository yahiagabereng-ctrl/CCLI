$log = "C:\Yahia\projects\CCLI\lab\pi4-openwrt\sd-boot-check.txt"
"=== SD boot check $(Get-Date -Format o) ===" | Out-File $log
Get-Disk -Number 1 | Out-File $log -Append
Get-Partition -DiskNumber 1 | Out-File $log -Append
Get-Partition -DiskNumber 1 | Where-Object DriveLetter | ForEach-Object { mountvol "$($_.DriveLetter):" /D 2>$null }
Set-Partition -DiskNumber 1 -PartitionNumber 1 -NewDriveLetter D -ErrorAction SilentlyContinue
Start-Sleep 2
if (Test-Path D:\) {
  "Boot files:" | Out-File $log -Append
  Get-ChildItem D:\ -Force | Out-File $log -Append
  foreach ($f in @('cmdline.txt','config.txt','issue.txt','platform.txt')) {
    if (Test-Path "D:\$f") { "--- $f ---"; Get-Content "D:\$f" | Out-File $log -Append }
  }
}
