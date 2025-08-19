@echo off
"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -ExecutionPolicy Bypass -File "%~dp0\LaunchAccountAuthorizer.ps1" -EditorBinary %1 -ProjectPath %2 -SdkLocation %3 -ProductId %4 -LogFilePath %5
exit %errorlevel%