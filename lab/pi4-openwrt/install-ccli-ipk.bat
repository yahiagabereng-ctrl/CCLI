@echo off

setlocal

cd /d "%~dp0..\.."



set "PLINK=C:\Program Files\PuTTY\plink.exe"

set "PSCP=C:\Program Files\PuTTY\pscp.exe"

set "HOSTKEY=SHA256:T+gkCI4E13YkwpfL9aEb8wIQHwszNcoGuZw5yU9D7lo"

set "PI=192.168.1.1"

if not defined CCLI_PI_PW set /p CCLI_PI_PW=OpenWrt root password: 



for %%F in (lab\pi4-openwrt\ccli_*.ipk) do set IPK=%%F

if not defined IPK (

    echo No ccli_*.ipk in lab\pi4-openwrt\ — run build first.

    exit /b 1

)



echo Installing deps + %IPK% on root@%PI% ...



if exist lab\pi4-openwrt\libmodbus_3.1.8-r1_aarch64_cortex-a72.ipk (

    "%PSCP%" -scp -batch -pw %CCLI_PI_PW% -hostkey %HOSTKEY% lab\pi4-openwrt\libmodbus_3.1.8-r1_aarch64_cortex-a72.ipk root@%PI%:/tmp/libmodbus.ipk

)



if exist lab\pi4-openwrt\libgpiod_2.1.3-r1_aarch64_cortex-a72.ipk (

    "%PSCP%" -scp -batch -pw %CCLI_PI_PW% -hostkey %HOSTKEY% lab\pi4-openwrt\libgpiod_2.1.3-r1_aarch64_cortex-a72.ipk root@%PI%:/tmp/libgpiod.ipk

)



if exist lab\pi4-openwrt\libstdcpp6.ipk (

    "%PSCP%" -scp -batch -pw %CCLI_PI_PW% -hostkey %HOSTKEY% lab\pi4-openwrt\libstdcpp6.ipk root@%PI%:/tmp/libstdcpp6.ipk

)



"%PSCP%" -scp -batch -pw %CCLI_PI_PW% -hostkey %HOSTKEY% "%IPK%" root@%PI%:/tmp/ccli.ipk

if errorlevel 1 exit /b 1



"%PLINK%" -batch -pw %CCLI_PI_PW% -hostkey %HOSTKEY% root@%PI% "opkg install /tmp/libstdcpp6.ipk /tmp/libmodbus.ipk /tmp/libgpiod.ipk /tmp/ccli.ipk 2>/dev/null; opkg install --force-reinstall /tmp/ccli.ipk && /etc/init.d/ccli enable && /etc/init.d/ccli restart && sleep 1; pgrep -a ccli; logread | tail -10"

if errorlevel 1 exit /b 1



echo.

echo Done. LuCI: System - Startup - ccli should be enabled.

pause

