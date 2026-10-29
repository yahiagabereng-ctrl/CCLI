@echo off
py -B "%~dp0control.py" %*
exit /b %errorlevel%
