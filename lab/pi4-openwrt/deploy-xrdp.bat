@echo off
setlocal
REM Deploy xRDP setup to Pi when on lab Ethernet (192.168.10.x)
set PI=192.168.10.20
set USER=yahia
set KEY=%USERPROFILE%\.ssh\id_ed25519_testbench
set SCRIPT=%~dp0setup-xrdp-testbench.sh

echo Checking Pi at %PI%...
ping -n 1 -w 1000 %PI% | findstr /i "Reply" >nul
if errorlevel 1 (
  echo.
  echo [ERROR] Pi not reachable at %PI%.
  echo   - Connect PC Ethernet to same switch as Pi
  echo   - PC should be 192.168.10.x ^(lab^)
  echo   - Pi powered on
  echo.
  echo Or run in PuTTY manually:
  echo   bash %SCRIPT%
  pause
  exit /b 1
)

echo Installing xRDP via SSH (key auth)...
"C:\Program Files\PuTTY\plink.exe" -ssh %USER%@%PI% -i "%KEY%" -batch -m "%SCRIPT%"
if errorlevel 1 (
  echo.
  echo Key auth failed — trying with PuTTY password in manual mode.
  echo Open PuTTY, login as yahia, then run:
  echo   bash %SCRIPT%
  echo.
  start "" "C:\Program Files\PuTTY\putty.exe" -ssh %USER%@%PI% -P 22
  pause
  exit /b 1
)

echo.
echo Opening Remote Desktop...
start mstsc.exe /v:%PI%
echo Done. Login as %USER% with your Pi password.
pause
