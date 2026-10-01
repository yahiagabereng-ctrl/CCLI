@echo off
REM CCLI Phase 3 — set PC Ethernet for TesPro LAN1 (DSO Eth_A)
REM Right-click this file → Run as administrator
netsh interface ip set address name="Ethernet" static 192.168.10.10 255.255.255.0 192.168.10.1
netsh interface ip set dns name="Ethernet" static 192.168.10.1
netsh interface ip add dns name="Ethernet" 8.8.8.8 index=2
echo.
echo --- Config ---
ipconfig
echo.
echo --- Ping TG LAN1 ---
ping -n 4 192.168.10.1
echo.
pause
