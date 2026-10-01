@echo off
title TG544 ENG SSH via tunnel (localhost:18022)
echo ========================================
echo  Engineering-side SSH (tunneled)
echo  Target: 127.0.0.1:18022 -^> TG 192.168.0.1:22
echo  Start ssh_ENG_tunnel_keep.cmd FIRST if this fails.
echo ========================================
echo.
"C:\Program Files\PuTTY\plink.exe" -ssh root@127.0.0.1 -P 18022 -pw 000000 -hostkey SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE
echo.
echo Session ended. Press any key to close.
pause >nul
