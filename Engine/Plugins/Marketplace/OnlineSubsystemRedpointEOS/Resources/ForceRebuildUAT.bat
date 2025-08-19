@echo off
echo UAT: Locating MSBuild...
call "GetMSBuildPath.bat"
if [%MSBUILD_EXE%] == [] goto :skip_build
echo UAT: Rebuilding AutomationTool binary...
cd ../../
%MSBUILD_EXE% /nologo /clp:ErrorsOnly /verbosity:quiet Source\Programs\AutomationTool\Scripts\AutomationScripts.Automation.csproj /property:Configuration=Development /property:Platform=AnyCPU "/property:OutputPath=%ENGINE_PATH%\Engine\Binaries\DotNET_EOSPatched" "/property:ReferencePath=%ENGINE_PATH%\Engine\Binaries\DotNET\AutomationScripts"
goto :end
:skip_build
echo warning: Your Visual Studio installation is missing dependencies required to patch UnrealBuildTool so that it can packaged EOS-enabled games. Please ensure the .NET workload is installed in Visual Studio. Refer to https://docs.redpoint.games/eos-online-subsystem/docs/troubleshooting_windows_dev for more information.
exit 8
:end