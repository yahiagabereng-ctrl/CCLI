@echo off
py -B -u "%~dp0bridge.py" --transport rtu --device COM8 --accept-cci-pq-writes %*
exit /b %errorlevel%
