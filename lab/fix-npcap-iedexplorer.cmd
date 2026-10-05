@echo off
REM Run as Administrator: fixes IED Explorer 0.80d looking for wpcap in Program Files\Npcap
title Fix Npcap paths for IED Explorer GooseSender
net session >nul 2>&1
if %errorlevel% neq 0 (
  echo ERROR: Run this script as Administrator.
  pause
  exit /b 1
)
set DEST=C:\Program Files\Npcap
if not exist "%DEST%" mkdir "%DEST%"
copy /Y "C:\Windows\System32\Npcap\wpcap.dll" "%DEST%\"
copy /Y "C:\Windows\System32\Npcap\Packet.dll" "%DEST%\"
copy /Y "C:\Windows\System32\Npcap\NpcapHelper.exe" "%DEST%\"
echo.
echo Installed:
dir "%DEST%\wpcap.dll" "%DEST%\Packet.dll" 2>nul
echo.
echo Restart IED Explorer as Administrator, load lab_tg544_eth_a.cid, open GooseSender on Ethernet.
pause
