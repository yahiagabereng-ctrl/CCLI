@echo off
REM CCLI logger PC — LAN1 (Eth_A) for deploy / capture at 192.168.10.1
REM Use a different host than PC-B TSP (192.168.10.10). Right-click -> Run as administrator.
REM Adapter name: change "Ethernet" if Device Manager shows another label (e.g. USB GbE).

set IP=192.168.10.20
set MASK=255.255.255.0
set GW=192.168.10.1
set ADAPTER=Ethernet

netsh interface ip set address name="%ADAPTER%" static %IP% %MASK% %GW%
netsh interface ip set dns name="%ADAPTER%" static %GW%
echo.
echo Logger LAN1: %IP% /24  gateway %GW%
echo.
ipconfig | findstr /i "IPv4 Ethernet adapter"
echo.
echo --- Ping DUT LAN1 ---
ping -n 4 %GW%
echo.
echo SSH/deploy target: root@%GW%
echo   deploy-ccli-session-fix.ps1 -HostAddr %GW% ...
echo.
pause
