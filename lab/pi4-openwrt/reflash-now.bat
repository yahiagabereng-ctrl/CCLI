@echo off

REM One-click reflash — self-elevates via PowerShell script

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0flash-openwrt-pi4-auto.ps1"

if exist "%~dp0flash-result.txt" (

  echo.

  type "%~dp0flash-result.txt"

) else (

  echo.

  echo [ERROR] Flash did not complete. Did you click Yes on the UAC prompt?

)

echo.

pause


