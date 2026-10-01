@echo off
title Flash OpenWrt with Raspberry Pi Imager
echo.
echo  SD card detected. Opening Pi Imager + image folder.
echo.
echo  IN IMAGER:
echo    1. Device  = Raspberry Pi 4
echo    2. OS      = Use custom image
echo    3. Browse  = openwrt-24.10.5-pi4-factory.img.gz  (14 MB in Downloads)
echo    4. Storage = Mass Storage Device ~64 GB  (NOT C:)
echo    5. NEXT - Write
echo.
start "" "C:\Users\yahia\Downloads"
start "" "C:\Program Files\Raspberry Pi Ltd\Imager\rpi-imager.exe"
pause
