@echo off
title TG544 DUT SSH (lan_sec 192.168.1.130)
echo ========================================
echo  TG544 shell via LAN2 / lan_sec
echo  Host: 192.168.1.130   user: root
echo  From here you can see BOTH networks.
echo ========================================
echo.
"C:\Program Files\PuTTY\plink.exe" -ssh root@192.168.1.130 -pw 000000 -hostkey SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE
echo.
echo Session ended. Press any key to close.
pause >nul
