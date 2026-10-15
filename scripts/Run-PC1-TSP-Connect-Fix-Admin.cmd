@echo off
REM Right-click -> Run as administrator (firewall + optional LAN1)
cd /d "%~dp0.."
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0pc1-tsp-connect-fix.ps1"
pause
