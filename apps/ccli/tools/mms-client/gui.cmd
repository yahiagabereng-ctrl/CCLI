@echo off
setlocal
set "PYTHON_EXE=%USERPROFILE%\AppData\Local\Python\pythoncore-3.14-64\python.exe"
if exist "%PYTHON_EXE%" (
    "%PYTHON_EXE%" "%~dp0gui.py" %*
    exit /b %errorlevel%
)
where py >nul 2>nul
if not errorlevel 1 (
    py -3 "%~dp0gui.py" %*
    exit /b %errorlevel%
)
echo Python 3 with Tkinter was not found. Install it or edit gui.cmd to point to python.exe. 1>&2
exit /b 9009
