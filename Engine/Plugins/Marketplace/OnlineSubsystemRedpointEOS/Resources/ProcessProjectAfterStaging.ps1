#
# This script is used to wire up the EOS bootstrapper and sign projects for Anti-Cheat on Windows.
#
param([string] $EngineDir, [string] $ProjectDir, [string] $PluginDir, [string] $StageDir)

$ErrorActionPreference = "Stop"

$TargetPlatform = $env:TargetPlatform
$TargetConfiguration = $env:TargetConfiguration

# We don't do post-stage processing for anything other than Win64 at the moment.
if ($TargetPlatform -ne "Win64") {
    exit 0
}

# We don't do post-stage processing for dedicated servers.
foreach ($Exe in (Get-ChildItem -Path $StageDir -Filter *.exe)) {
    if ($Exe.Name -ne "start_protected_game.exe") {
        $ExecutableName = $Exe.Name.Substring(0, $Exe.Name.Length - 4)
        if ($ExecutableName.EndsWith("Server")) {
            exit 0
        }
    }
}

# Read the configuration and see what kind of post-stage processing we need to do.
$AntiCheatEnabled = $false
$EpicGamesCrossPlatformAccounts = $false
$ProductId = $null
$SandboxId = $null
$DeploymentId = $null
$CopyIconToBootstrapper = $false
$ConfigFile = "$ProjectDir\Config\DefaultEngine.ini"
if (Test-Path $ConfigFile) {
    $ConfigValues = Get-Content $ConfigFile
    $ConfigSection = ""
    foreach ($ConfigLine in $ConfigValues) {
        if ($ConfigLine.StartsWith("[")) {
            $ConfigSection = $ConfigLine.Substring(1, $ConfigLine.Length - 2).Trim()
        }
        if ($ConfigSection -eq "EpicOnlineServices") {
            if ($ConfigLine -eq "EnableAntiCheat=True") {
                $AntiCheatEnabled = $true
            }
            if ($ConfigLine -eq "CrossPlatformAccountProvider=EpicGames") {
                $EpicGamesCrossPlatformAccounts = $true
            }
            if ($ConfigLine.StartsWith("ProductId=")) {
                $ProductId = $ConfigLine.Substring("ProductId=".Length).Trim()
            }
            if ($ConfigLine.StartsWith("SandboxId=")) {
                $SandboxId = $ConfigLine.Substring("SandboxId=".Length).Trim()
            }
            if ($ConfigLine.StartsWith("DeploymentId=")) {
                $DeploymentId = $ConfigLine.Substring("DeploymentId=".Length).Trim()
            }
            if ($ConfigLine -eq "CopyIconForBootstrapper=True") {
                $CopyIconToBootstrapper = $true
            }
        }
    }
}

# If the project isn't configured for EOS, we don't have any post-stage processing to do.
if ($null -eq $ProductId -or $null -eq $SandboxId -or $null -eq $DeploymentId) {
    exit 0
}

# If we don't have any work to do, exit early.
if (!$AntiCheatEnabled -and !$EpicGamesCrossPlatformAccounts) {
    exit 0
}

# Locate the SDK tools.
$SdkLocation = $null
$SdkVersions = @(
    # Supported SDKs.
    "20662730-1.15.1",
    "20906061-v1.14.2-hf-2",

    # De-prioritized due to known P2P networking issues.
    "21924193-v1.15.3"
)
foreach ($SdkVersion in $SdkVersions) {
    $SdkLocations = @(
        "$PluginDir\Source\ThirdParty",
        "$ProjectDir\Plugins\EOS-SDK-$SdkVersion\SDK",
        "$env:PROGRAMDATA\EOS-SDK-$SdkVersion\SDK",
        "$env:USERPROFILE\Downloads\EOS-SDK-$SdkVersion\SDK",
        "$env:SYSTEMDRIVE\EOS-SDK-$SdkVersion\SDK"
    )
    foreach ($CandidateLocation in $SdkLocations) {
        if (Test-Path $CandidateLocation) {
            $SdkLocation = $CandidateLocation
            break
        }
    }
    if ($null -ne $SdkLocation) {
        break
    }
}

# If we don't have an SDK, error out since we won't be able to do anything.
if ($null -eq $SdkLocation) {
    Write-Host "error: Could not find EOS SDK in any installed location for post-stage processing."
    exit 1
}

# Determine executable name.
$ExecutableName = $null
foreach ($Exe in (Get-ChildItem -Path $StageDir -Filter *.exe)) {
    if ($Exe.Name -ne "start_protected_game.exe" -and $Exe.Name -ne "EpicOnlineServicesInstaller.exe") {
        $ExecutableName = $Exe.Name.Substring(0, $Exe.Name.Length - 4)
        break
    }
}

# Determine folder name.
$FolderName = $null
foreach ($Folder in (Get-ChildItem -Path $StageDir -Directory)) {
    if ($Folder.Name -ne "Engine" -and $Folder.Name -ne "EasyAntiCheat") {
        $FolderName = $Folder.Name
        break
    }
}

# Check that we could figure out the executable name and folder name.
if ($null -eq $ExecutableName -or $null -eq $FolderName) {
    Write-Host "error: Post-stage processing could not determine project name from main executable filename."
    exit 1
}

# Determine main executable path.
$MainExePath = $null
foreach ($Exe in (Get-ChildItem -Path "$StageDir\$FolderName\Binaries\Win64" -Filter *.exe)) {
    if ($Exe.Name.StartsWith("$ExecutableName")) {
        $MainExePath = $Exe.FullName
    }
}
if ($null -eq $MainExePath) {
    Write-Host "error: Could not determine main executable for Anti-Cheat to run."
}
$RelativeExePath = $MainExePath.Substring($StageDir.Length+1)

Write-Host "Running post-stage processing for project..."
Write-Host "Engine path: $EngineDir"
Write-Host "Plugin dir: $PluginDir"
Write-Host "Project dir: $ProjectDir"
Write-Host "Stage dir: $StageDir"
Write-Host "Target platform: $TargetPlatform"
Write-Host "Target configuration: $TargetConfiguration"
Write-Host "EOS product ID: $ProductId"
Write-Host "Executable name: $ExecutableName"
Write-Host "Folder name: $FolderName"
Write-Host "Main executable absolute path: $MainExePath"
Write-Host "Main executable relative path: $RelativeExePath"
Write-Host "Wrapping with EOS bootstrapper? $(if ($EpicGamesCrossPlatformAccounts) { "Yes" } else { "No" })"
Write-Host "Signing for Anti-Cheat? $(if ($AntiCheatEnabled) { "Yes" } else { "No" })"
Write-Host "Copying icon to bootstrapper? $(if ($CopyIconToBootstrapper) { "Yes" } else { "No" })"

# Sign for Anti-Cheat first, since the EOS bootstrapper can't be protected with Anti-Cheat.
if ($AntiCheatEnabled) {
    & "$PSScriptRoot\ProcessProjectForAntiCheat.ps1" `
        -EngineDir $EngineDir `
        -ProjectDir $ProjectDir `
        -PluginDir $PluginDir `
        -StageDir $StageDir `
        -TargetPlatform $TargetPlatform `
        -TargetConfiguration $TargetConfiguration `
        -ProductId $ProductId `
        -SandboxId $SandboxId `
        -DeploymentId $DeploymentId `
        -SdkLocation $SdkLocation `
        -ExecutableName $ExecutableName `
        -FolderName $FolderName `
        -MainExePath $MainExePath `
        -RelativeExePath $RelativeExePath
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}

# Finally, wrap with the EOS bootstrapper if we need to do so.
if ($EpicGamesCrossPlatformAccounts) {
    & "$PSScriptRoot\ProcessProjectForEpicGamesAccounts.ps1" `
        -EngineDir $EngineDir `
        -ProjectDir $ProjectDir `
        -PluginDir $PluginDir `
        -StageDir $StageDir `
        -TargetPlatform $TargetPlatform `
        -TargetConfiguration $TargetConfiguration `
        -ProductId $ProductId `
        -SandboxId $SandboxId `
        -DeploymentId $DeploymentId `
        -SdkLocation $SdkLocation `
        -ExecutableName $ExecutableName `
        -FolderName $FolderName `
        -MainExeAbsolutePath $MainExePath `
        -MainExeRelativePath $RelativeExePath `
        -CopyIconToBootstrapper $CopyIconToBootstrapper
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}
