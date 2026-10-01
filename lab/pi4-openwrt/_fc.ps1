$log = "C:\Yahia\projects\CCLI\lab\pi4-openwrt\flash-check.txt"
"=== $(Get-Date -Format o) ===" | Out-File $log
Get-Partition -DiskNumber 1 -ErrorAction SilentlyContinue | Out-File $log -Append
$p = Get-Partition -DiskNumber 1 -PartitionNumber 1 -ErrorAction SilentlyContinue
if ($p -and -not $p.DriveLetter) { Set-Partition -InputObject $p -NewDriveLetter D -ErrorAction SilentlyContinue }
Start-Sleep 1
if (Test-Path D:\cmdline.txt) { "cmdline: $((Get-Content D:\cmdline.txt -Raw).Trim())" | Out-File $log -Append }
if (Test-Path D:\issue.txt) { "issue: $((Get-Content D:\issue.txt -Raw).Trim())" | Out-File $log -Append }
