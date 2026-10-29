@echo off
call "%~dp0run.cmd" --profile aggregator %*
exit /b %errorlevel%
