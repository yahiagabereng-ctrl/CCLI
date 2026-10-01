@echo off

title OpenWrt Pi4 Reflash - ADMIN REQUIRED

color 0E

echo.

echo  ============================================

echo   OpenWrt 24.10.5 - Pi 4 SD REFLASH

echo  ============================================

echo.

echo  This WILL ERASE the SD card (Disk 1 ~64GB).

echo.

echo  Click YES on the next UAC Admin prompt.

echo.

pause

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0flash-openwrt-pi4-auto.ps1"

echo.

if exist "%~dp0flash-result.txt" (

  echo === RESULT ===

  type "%~dp0flash-result.txt"

) else (

  echo [FAILED] No flash log. Did you click YES on UAC?

)

echo.

pause


