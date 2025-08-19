#
# This script patches AutomationTool so that we can run scripts post-stage from a plugin.
#
param([Parameter(Mandatory=$false)][string] $ProjectDir)

$ErrorActionPreference = "Stop"

# Unreal Engine 5 messes up the PATH environment variable. Ensure we fix up PATH from the
# current system's settings.
$UserPath = ([System.Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::User))
$ComputerPath = ([System.Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::Machine))
if ($UserPath.Trim() -ne "") {
    $env:PATH = "$($UserPath);$($ComputerPath)"
} else {
    $env:PATH = $ComputerPath
}

# Check to make sure we have a project directory.
if ($ProjectDir -eq $null -or $ProjectDir -eq "") {
    exit 0
}

function Set-RetryableContent([string] $Path, [string] $Value) {
    while ($true) {
        try {
            Set-Content -Force -Path $Path -Value $Value
            break
        } catch {
            if ($_.ToString().Contains("Stream was not readable.")) {
                continue
            }
        }
    }
}

# Check to see if the project's post-stage hook is set up correctly.
function Copy-IfNotChanged([string] $SourcePath, [string] $TargetPath) {
    $SourceContent = Get-Content -Raw $SourcePath
    if (Test-Path $TargetPath) {
        if ((Get-Item $TargetPath) -is [system.IO.directoryinfo]) {
            Remove-Item -Path $TargetPath
        } else {
            $TargetContent = Get-Content -Raw $TargetPath
            if ($SourceContent -eq $TargetContent) {
                return;
            }
        }
    }
    if (!(Test-Path ([System.IO.Path]::GetDirectoryName("$TargetPath")))) {
        New-Item -ItemType Directory ([System.IO.Path]::GetDirectoryName("$TargetPath"))
    }
    Write-Host "Setting up $TargetPath..."
    Set-RetryableContent -Path $TargetPath -Value $SourceContent
}
if ($ProjectDir -ne $null -and $ProjectDir -ne "") {
    Copy-IfNotChanged "$PSScriptRoot\PostStageHook.bat" "$ProjectDir\Build\NoRedist\PostStageHook.bat"
    Copy-IfNotChanged "$PSScriptRoot\PostStageHook.ps1" "$ProjectDir\Build\NoRedist\PostStageHook.ps1"
}

# Check to see if CopyBuildToStagingDirectory.Automation.cs is patched in the engine. If it isn't, 
# we need to patch AutomationTool and recompile it so we have a hook that we can use to modify the 
# project when it is staged (not when it's built, which is when we are running right now).
$EngineDir = (Resolve-Path "$((Get-Location).Path)\..\..").Path
$IsUnrealEngine5 = $false
if (Test-Path "$EngineDir\Engine\Binaries\Win64\UnrealEditor-Cmd.exe") {
    $IsUnrealEngine5 = $true
}
$CopyBuildScriptPath = "$EngineDir\Engine\Source\Programs\AutomationTool\Scripts\CopyBuildToStagingDirectory.Automation.cs"
$CopyBuildScriptBackupPath = "$EngineDir\Engine\Source\Programs\AutomationTool\Scripts\CopyBuildToStagingDirectory.Automation.cs.backup"
$CopyBuildScriptContent = Get-Content -Raw $CopyBuildScriptPath
$CopyBuildScriptContentOriginal = $CopyBuildScriptContent
$PatchVersionLevel = "None"
if (Test-Path "$EngineDir\Engine\Binaries\DotNET\UBT_EOS_PatchLevel.txt") {
    $PatchVersionLevel = (Get-Content -Raw -Path "$EngineDir\Engine\Binaries\DotNET\UBT_EOS_PatchLevel.txt" | Out-String).Trim()
}
if (!$CopyBuildScriptContent.Contains("// EOS Online Subsystem Anti-Cheat Hook 1.1") -or $PatchVersionLevel -ne "1.1") {
    Write-Host "Installing staging hooks into AutomationTool..."

    Get-ChildItem -Path "$EngineDir\Engine\Source\Programs\AutomationTool" -Recurse -Filter "*" | % {
        if ($_.IsReadOnly) {
            $_.IsReadOnly = $false
        }
    }
    Get-ChildItem -Path "$EngineDir\Engine\Source\Programs\DotNETCommon" -Recurse -Filter "*" | % {
        if ($_.IsReadOnly) {
            $_.IsReadOnly = $false
        }
    }
    Get-ChildItem -Path "$EngineDir\Engine\Source\Programs\UnrealBuildTool" -Recurse -Filter "*" | % {
        if ($_.IsReadOnly) {
            $_.IsReadOnly = $false
        }
    }

    if (Test-Path $CopyBuildScriptBackupPath) {
        # We previously installed an older hook. Use original to so we can re-patch.
        $CopyBuildScriptContent = Get-Content -Raw $CopyBuildScriptBackupPath
    }

    $Hook = @"
// EOS Online Subsystem Anti-Cheat Hook 1.1
// EOS BEGIN HOOK
private static void ExecuteProjectPostStageHook(ProjectParams Params, DeploymentContext SC)
{
    string StageHookPath = Path.Combine(SC.ProjectRoot.FullName, "Build", "NoRedist", "PostStageHook.bat");
    if (File.Exists(StageHookPath))
    {
        RunAndLog(CmdEnv, StageHookPath, "\"" + SC.StageDirectory + "\"", Options: ERunOptions.Default | ERunOptions.UTF8Output, EnvVars: new Dictionary<string, string>
        {
            { "TargetConfiguration", SC.StageTargetConfigurations.FirstOrDefault().ToString() },
            { "TargetPlatform", SC.StageTargetPlatform.PlatformType.ToString() },
        });
    }
}
// EOS END HOOK
"@
    $CopyBuildScriptContent = $CopyBuildScriptContent.Replace("public static void CopyBuildToStagingDirectory(", "$Hook`r`n`r`npublic static void CopyBuildToStagingDirectory(");
    $CopyBuildScriptContent = $CopyBuildScriptContent.Replace("ApplyStagingManifest(ParamsInstance, SC);", "ApplyStagingManifest(ParamsInstance, SC); ExecuteProjectPostStageHook(ParamsInstance, SC);");
    $CopyBuildScriptContent = $CopyBuildScriptContent.Replace("ApplyStagingManifest(Params, SC);", "ApplyStagingManifest(Params, SC); ExecuteProjectPostStageHook(Params, SC);");
    if (!(Test-Path $CopyBuildScriptBackupPath)) {
        Copy-Item -Force $CopyBuildScriptPath $CopyBuildScriptBackupPath
    }
    Set-RetryableContent -Path $CopyBuildScriptPath -Value $CopyBuildScriptContent

    $Success = $false
    Push-Location "$EngineDir\Engine\Build\BatchFiles"
    try {
        $env:ENGINE_PATH = $EngineDir
        if ($IsUnrealEngine5) {
            $local:CopyRules = @(
                @{
                    Source = "$PSScriptRoot\UE5\fastJSON.dll";
                    Target = "$EngineDir\Engine\Binaries\ThirdParty\fastJSON\netstandard2.0\fastJSON.dll";
                },
                @{
                    Source = "$PSScriptRoot\UE5\fastJSON.deps.json";
                    Target = "$EngineDir\Engine\Binaries\ThirdParty\fastJSON\netstandard2.0\fastJSON.deps.json";
                }
            )
            if (Test-Path "$EngineDir\Engine\Source\Programs\Shared\EpicGames.Perforce.Native") {
                $local:CopyRules += @{
                    Source = "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationUtils\EpicGames.Perforce.Native.dll";
                    Target = "$EngineDir\Engine\Binaries\DotNET\EpicGames.Perforce.Native\win-x64\Release\EpicGames.Perforce.Native.dll";
                }
                $local:CopyRules += @{
                    Source = "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationUtils\EpicGames.Perforce.Native.dylib";
                    Target = "$EngineDir\Engine\Binaries\DotNET\EpicGames.Perforce.Native\mac-x64\Release\EpicGames.Perforce.Native.dylib";
                }
                $local:CopyRules += @{
                    Source = "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationUtils\EpicGames.Perforce.Native.so";
                    Target = "$EngineDir\Engine\Binaries\DotNET\EpicGames.Perforce.Native\linux-x64\Release\EpicGames.Perforce.Native.so";
                }
            }
            foreach ($CopyRule in $local:CopyRules) {
                if ($global:IsMacOS -or $global:IsLinux) {
                    $CopyRule.Source = $CopyRule.Source.Replace("\", "/")
                    $CopyRule.Target = $CopyRule.Target.Replace("\", "/")
                }
                if ((Test-Path -Path $CopyRule.Source) -and !(Test-Path -Path $CopyRule.Target)) {
                    $local:TargetDir = [System.IO.Path]::GetDirectoryName($CopyRule.Target)
                    if (!(Test-Path -Path $local:TargetDir)) {
                        New-Item -ItemType Directory -Path $local:TargetDir | Out-Null
                    }
                    Copy-Item -Force -Path $CopyRule.Source -Destination $CopyRule.Target
                }
            }

            & "$PSScriptRoot\ForceRebuildUAT_UE5.bat"
        } else {
            & "$PSScriptRoot\ForceRebuildUAT.bat"
        }
        if ($LASTEXITCODE -ne 0) {
            if ($LASTEXITCODE -eq 8) {
                # This is a soft failure. We don't want to block people building the game
                # outright when they are missing dependencies to install the hooks.
                exit 0
            }
            exit $LASTEXITCODE
        }
        if ($IsUnrealEngine5) {
            if (Test-Path "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts\AutomationScripts.Automation.dll") {
                Move-Item -Force "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts\AutomationScripts.Automation.dll" "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts\AutomationScripts.Automation.dll.old"
            }
            if (Test-Path "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts\AutomationScripts.Automation.pdb") {
                Move-Item -Force "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts\AutomationScripts.Automation.pdb" "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts\AutomationScripts.Automation.pdb.old"
            }
            if (!(Test-Path "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts")) {
                New-Item -ItemType Directory -Path "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts" -Force
            }
            Copy-Item -Force "$EngineDir\Engine\Binaries\DotNET_EOSPatched\AutomationScripts.Automation.dll" "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts\AutomationScripts.Automation.dll"
            Copy-Item -Force "$EngineDir\Engine\Binaries\DotNET_EOSPatched\AutomationScripts.Automation.pdb" "$EngineDir\Engine\Binaries\DotNET\AutomationTool\AutomationScripts\Scripts\AutomationScripts.Automation.pdb"
        } else {
            if (Test-Path "$EngineDir\Engine\Binaries\DotNET\AutomationScripts\AutomationScripts.Automation.dll") {
                Move-Item -Force "$EngineDir\Engine\Binaries\DotNET\AutomationScripts\AutomationScripts.Automation.dll" "$EngineDir\Engine\Binaries\DotNET\AutomationScripts\AutomationScripts.Automation.dll.old"
            }
            if (Test-Path "$EngineDir\Engine\Binaries\DotNET\AutomationScripts\AutomationScripts.Automation.pdb") {
                Move-Item -Force "$EngineDir\Engine\Binaries\DotNET\AutomationScripts\AutomationScripts.Automation.pdb" "$EngineDir\Engine\Binaries\DotNET\AutomationScripts\AutomationScripts.Automation.pdb.old"
            }
            if (!(Test-Path "$EngineDir\Engine\Binaries\DotNET\AutomationScripts")) {
                New-Item -ItemType Directory -Path "$EngineDir\Engine\Binaries\DotNET\AutomationScripts" -Force
            }
            Copy-Item -Force "$EngineDir\Engine\Binaries\DotNET_EOSPatched\AutomationScripts.Automation.dll" "$EngineDir\Engine\Binaries\DotNET\AutomationScripts\AutomationScripts.Automation.dll"
            Copy-Item -Force "$EngineDir\Engine\Binaries\DotNET_EOSPatched\AutomationScripts.Automation.pdb" "$EngineDir\Engine\Binaries\DotNET\AutomationScripts\AutomationScripts.Automation.pdb"
        }
        Set-Content -Force -Path "$EngineDir\Engine\Binaries\DotNET\UBT_EOS_PatchLevel.txt" -Value "1.1"
        $Success = $true
    } finally {
        if (!$Success) {
            # Restore original script.
            Set-RetryableContent -Path $CopyBuildScriptPath -Value $CopyBuildScriptContentOriginal
        }
        Pop-Location
    }

    Write-Host "warning: EOS Online Subsystem had to patch your engine's CopyBuildToStagingDirectory.Automation.cs file to be compatible with Anti-Cheat and the EOS Bootstrapper Tool. Executables won't be correctly packaged until you next build and package the project."
    exit 0
}

exit 0