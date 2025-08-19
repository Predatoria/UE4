@echo off
echo Extracting Developer Authentication Tool from the SDK...
echo Located in: %*
cd /D %*
powershell -ExecutionPolicy Bypass "%~dp0\ExtractDevAuthTool.ps1"