@echo off
REM Plant LAN3 — meter @ 192.168.178.250 (Chronos factory-style subnet)
REM TG544 lan3 UCI: network.lan2_sec.ipaddr=192.168.178.1
REM Run as Administrator. Use the PC port cabled to DUT LAN3 (or direct to meter).

netsh interface ip set address name="Ethernet 2" static 192.168.178.10 255.255.255.0
echo Plant NIC set to 192.168.178.10/24 — ping 192.168.178.250
ping -n 3 192.168.178.250
echo.
echo Next: pip install pymodbus
echo   python lab\modbus_emt432_tcp_probe.py --host 192.168.178.250
pause
