Set shell = CreateObject("Shell.Application")
shell.ShellExecute "powershell.exe", "-NoProfile -ExecutionPolicy Bypass -File ""C:\Yahia\projects\CCLI\lab\pi4-openwrt\flash-openwrt-pi4-auto.ps1""", "", "runas", 1
