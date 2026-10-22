@echo off
REM Plant LAN3 — meter @ 192.168.178.10 (UI) · PC plant NIC .11 (no clash with meter)
REM TG544 lan3 UCI: network.lan2_sec.ipaddr=192.168.178.1
REM Run as Administrator. Use the PC port cabled to meter / plant switch.

netsh interface ip set address name="Ethernet 2" static 192.168.178.11 255.255.255.0
echo Plant NIC set to 192.168.178.11/24 — meter 192.168.178.10
ping -n 2 192.168.178.10
ping -n 2 192.168.178.1
echo.
echo Next: pip install pymodbus
echo   python lab\modbus_emt432_tcp_probe.py --host 192.168.178.10
echo   python lab\emt432_rogowski_config.py --host 192.168.178.10 --read
echo Web: https://192.168.178.10  (PW Web on meter label)
pause
