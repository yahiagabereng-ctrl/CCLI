@echo off
REM PuTTY with SSH key (no password prompt if key is authorized on Pi)
"C:\Program Files\PuTTY\putty.exe" -ssh yahia@192.168.10.20 -P 22 -i "%USERPROFILE%\.ssh\id_ed25519_testbench.ppk" 2>nul
if errorlevel 1 (
  REM Fallback without key — password login
  "C:\Program Files\PuTTY\putty.exe" -ssh yahia@192.168.10.20 -P 22
)
