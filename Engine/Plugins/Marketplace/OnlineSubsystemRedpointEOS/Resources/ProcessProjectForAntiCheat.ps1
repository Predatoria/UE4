#
# This script is called by ProcessProjectAfterStaging.ps1 for signing the project with Anti-Cheat.
#
param(
    [string] $EngineDir, 
    [string] $ProjectDir, 
    [string] $PluginDir,
    [string] $StageDir,
    [string] $TargetPlatform,
    [string] $TargetConfiguration,
    [string] $ProductId,
    [string] $SandboxId,
    [string] $DeploymentId,
    [string] $SdkLocation,
    [string] $ExecutableName,
    [string] $FolderName,
    [string] $MainExePath,
    [string] $RelativeExePath
)

$ErrorActionPreference = "Stop"

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

# Make sure the Anti-Cheat signing keys are set up.
if (!(Test-Path "$ProjectDir\Build\NoRedist\base_private.key")) {
    Write-Host "error: Missing Build\NoRedist\base_private.key for Anti-Cheat signing. Either download the file and place it there, or turn off Anti-Cheat in Project Settings."
    exit 1
}
if (!(Test-Path "$ProjectDir\Build\NoRedist\base_public.cer")) {
    Write-Host "error: Missing Build\NoRedist\base_public.cer for Anti-Cheat signing. Either download the file and place it there, or turn off Anti-Cheat in Project Settings."
    exit 1
}

# Locate the Anti-Cheat tools and extract them so we can use them.
$AntiCheatVersions = @(
    "EOS_AntiCheatTools-win32-x64-1.3.0",
    "EOS_AntiCheatTools-win32-x64-1.2.3",
    "EOS_AntiCheatTools-win32-x64-1.2.2",
    "EOS_AntiCheatTools-win32-x64-1.2.1",
    "EOS_AntiCheatTools-win32-x64-1.2.0",
    "EOS_AntiCheatTools-win32-x64-1.1.2"
)
$AntiCheatVersion = ""
$AntiCheatNeedsGameDirectory = $true
foreach ($CandidateVersion in $AntiCheatVersions) {
    if (!(Test-Path "$SdkLocation\Tools\$CandidateVersion")) {
        if (!(Test-Path "$SdkLocation\Tools\$CandidateVersion.zip")) {
            continue
        }

        Write-Host "Extracting tools for Anti-Cheat signing..."
        Expand-Archive "$SdkLocation\Tools\$CandidateVersion.zip" "$SdkLocation\Tools\$CandidateVersion"
    }
    $AntiCheatVersion = $CandidateVersion
    if ($AntiCheatVersion -eq "EOS_AntiCheatTools-win32-x64-1.1.2") {
        $AntiCheatNeedsGameDirectory = $false
    }
    Write-Host "Using Anti-Cheat Tools version: $AntiCheatVersion"
    break
}
if ($AntiCheatVersion -eq "") {
    Write-Host "error: Could not find Anti-Cheat tools for Anti-Cheat signing."
    exit 1
}

# Workaround for bug in signing tools 1.1.2.
$WorkingDirectoryEntry = ""
if (!$AntiCheatNeedsGameDirectory) {
    $WorkingDirectoryEntry = "working_directory = `"$StageDir`";"
}

$LaunchPath = $RelativeExePath

# Generate the configuration file for signing.
$AntiCheatConfig = @"
config_info:
{
    version = 2;
};
search_options:
{
    $WorkingDirectoryEntry
    exclude_size_threshold = 0;
    exclude_paths = [
        "EasyAntiCheat\\EasyAntiCheat_EOS_Setup.exe",
        "EasyAntiCheat\\Settings.json",
        "EasyAntiCheat\\Certificates",
        "EasyAntiCheat\\Licenses",
        "EpicOnlineServicesInstaller.exe"
    ];
    exclude_extensions = [
        ".bak",
        ".bat",
        ".bmp",
        ".cfg",
        ".db",
        ".ico",
        ".ini",
        ".inf",
        ".jpg",
        ".last",
        ".log",
        ".manifest",
        ".mdb",
        ".pdb",
        ".png",
        ".pubkey",
        ".txt",
        ".vdf",
        ".xml"
    ];
    ignore_files_without_extension = false;
};
"@
Set-Content -Path "$ProjectDir\Saved\anticheat_integritytool.cfg" -Value $AntiCheatConfig

# Execute Anti-Cheat signing process.
if ($AntiCheatNeedsGameDirectory) {
    & "$SdkLocation\Tools\$AntiCheatVersion\devtools\anticheat_integritytool64.exe" -productid $ProductId -inkey "$ProjectDir\Build\NoRedist\base_private.key" -incert "$ProjectDir\Build\NoRedist\base_public.cer" -target_game_dir "$StageDir" "$ProjectDir\Saved\anticheat_integritytool.cfg"
} else {
    & "$SdkLocation\Tools\$AntiCheatVersion\devtools\anticheat_integritytool64.exe" -productid $ProductId -inkey "$ProjectDir\Build\NoRedist\base_private.key" -incert "$ProjectDir\Build\NoRedist\base_public.cer" "$ProjectDir\Saved\anticheat_integritytool.cfg"
}
if ($LASTEXITCODE -ne 0) {
    Write-Host "error: Anti-Cheat integrity tool failed to run successfully."
    exit $LASTEXITCODE
}

# Escape the path for the Anti-Cheat configuration.
$LaunchPathEscaped = $LaunchPath.Replace("\", "\\")

# Now copy runtime dependencies.
Copy-Item -Force -Recurse "$SdkLocation\Tools\$AntiCheatVersion\dist\*" "$StageDir\"
$SplashScreenOption = ""
if (Test-Path "$ProjectDir\Build\NoRedist\AntiCheatSplashScreen.png") {
    Copy-Item -Force "$ProjectDir\Build\NoRedist\AntiCheatSplashScreen.png" "$StageDir\EasyAntiCheat\SplashScreen.png"
    $SplashScreenOption = "	`"requested_splash`"								: `"EasyAntiCheat/SplashScreen.png`","
}
Set-RetryableContent -Path "$StageDir\EasyAntiCheat\Settings.json" -Value @"
{
	"productid"										: "$ProductId",
	"sandboxid"										: "$SandboxId",
	"deploymentid"									: "$DeploymentId",
	"title"											: "$ExecutableName",
	"executable"									: "$LaunchPathEscaped",
	"logo_position"									: "bottom-left",
$SplashScreenOption
	"parameters"									: "",
	"use_cmdline_parameters"						: "1",
	"working_directory"								: "",
	"wait_for_game_process_exit"					: "0",
	"hide_splash_screen"							: "0",
	"hide_ui_controls"								: "0"
}
"@

# Remove launch executable that can't be used.
Remove-Item -Force "$StageDir\$ExecutableName.exe"

# Rename start_protected_game.exe so it has the name of the original game. This keeps games compatible with
# any launch scripts the developer might have written before enabling Anti-Cheat.
Move-Item -Force -Path "$StageDir\start_protected_game.exe" -Destination "$StageDir\$ExecutableName.exe"

# Create batch scripts to install/uninstall Anti-Cheat.
Set-RetryableContent -Path "$StageDir\InstallAntiCheat.bat" -Value @"
@echo off
cd %~dp0
EasyAntiCheat\EasyAntiCheat_EOS_Setup.exe install $ProductId
"@
Set-RetryableContent -Path "$StageDir\UninstallAntiCheat.bat" -Value @"
@echo off
cd %~dp0
EasyAntiCheat\EasyAntiCheat_EOS_Setup.exe uninstall $ProductId
"@

exit 0