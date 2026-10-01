@echo off
REM CCLI Phase 4 — set PC Ethernet for TesPro LAN2 (Operator Eth_B)
REM Right-click this file → Run as administrator
REM DUT 192.168.1.130 · PC lab static 192.168.1.183 · zone lan_sec
REM Verify: lab\tg544-openwrt\verify-lan2-eth-b.ps1
netsh interface ip set address name="Ethernet" static 192.168.1.183 255.255.255.0 192.168.1.130
netsh interface ip set dns name="Ethernet" static 192.168.1.130
echo.
echo --- Config ---
ipconfig
echo.
echo --- Ping TG LAN2 (Eth_B) ---
ping -n 4 192.168.1.130
echo.
echo Next: $env:CCLI_TG544_PW = "..." ; .\lab\tg544-openwrt\verify-lan2-eth-b.ps1
pause
