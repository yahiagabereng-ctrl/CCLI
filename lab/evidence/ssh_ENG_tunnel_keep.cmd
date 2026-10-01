@echo off
title TG544 ENG tunnel KEEP OPEN (do not close)
echo ========================================
echo  Port forwards (keep this window OPEN):
echo    127.0.0.1:18022 -^> SSH  (eng view)
echo    127.0.0.1:18080 -^> LuCI http://127.0.0.1:18080
echo ========================================
echo.
"C:\Program Files\PuTTY\plink.exe" -ssh root@192.168.1.130 -pw 000000 -hostkey SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE -N -L 18022:127.0.0.1:22 -L 18080:127.0.0.1:80
echo.
echo Tunnel closed. Press any key.
pause >nul
