@echo off
REM Flash OpenWrt 24.10.5 to Pi 4 SD card — requires Administrator
echo.
echo OpenWrt Pi 4 flash — will ERASE the SD card in the reader.
echo Image: lab\pi4-openwrt\openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img
echo.
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "Start-Process powershell.exe -Verb RunAs -ArgumentList '-NoProfile -ExecutionPolicy Bypass -File ""%~dp0flash-openwrt-pi4.ps1""'"
