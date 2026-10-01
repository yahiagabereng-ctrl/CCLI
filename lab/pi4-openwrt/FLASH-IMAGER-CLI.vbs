Set shell = CreateObject("Shell.Application")
img = "C:\Yahia\projects\CCLI\lab\pi4-openwrt\openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img.gz"
disk = "\\.\PhysicalDrive1"
imager = "C:\Program Files\Raspberry Pi Ltd\Imager\rpi-imager.exe"
shell.ShellExecute imager, "--cli """ & img & """ """ & disk & """", "", "runas", 1
