@echo off
REM Plant LAN3 — TG544 lan2_sec 192.168.30.0/24 (GOOSE capture NIC)
REM Run as Administrator. Use the PC port cabled to DUT LAN3 (not LAN1 DSO).

netsh interface ip set address name="Ethernet 2" static 192.168.30.10 255.255.255.0
echo Plant NIC set to 192.168.30.10/24 — ping 192.168.30.1
ping -n 2 192.168.30.1
pause
