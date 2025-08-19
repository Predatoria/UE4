$global:ErrorActionPreference = "Stop"

if (!(Test-Path "DevAuthTool")) {
    New-Item -ItemType Directory -Name "DevAuthTool"
}
if (Test-Path EOS_DevAuthTool-win32-x64-1.0.1.zip) {
    powershell -ExecutionPolicy Bypass -Command Expand-Archive -Verbose EOS_DevAuthTool-win32-x64-1.0.1.zip DevAuthTool/
}
if (Test-Path EOS_DevAuthTool-win32-x64-1.1.0.zip) {
    powershell -ExecutionPolicy Bypass -Command Expand-Archive -Verbose EOS_DevAuthTool-win32-x64-1.1.0.zip DevAuthTool/
}