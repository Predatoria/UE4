#
# This script launches the editor in a way that allows you to authorize EOS accounts. 
#
param(
    # The value of "FPlatformProcess::ExecutablePath()".
    [Parameter(Mandatory = $true)][string] $EditorBinary,
    # The path to the .uproject file.
    [Parameter(Mandatory = $true)][string] $ProjectPath,
    # The path to the EOS SDK.
    [Parameter(Mandatory = $true)][string] $SdkLocation,
    # The product ID of the game.
    [Parameter(Mandatory = $true)][string] $ProductId,
    # The path to output the log file.
    [Parameter(Mandatory = $true)][string] $LogFilePath
)

$ErrorActionPreference = "Stop"

function Force-Resolve-Path {
    param (
        [string] $FileName
    )
    $FileName = Resolve-Path $FileName -ErrorAction SilentlyContinue `
                                       -ErrorVariable _frperror
    if (-not($FileName)) {
        $FileName = $_frperror[0].TargetObject
    }
    return $FileName
}

$ProjectPath = (Resolve-Path $ProjectPath).Path
$LogFilePath = (Force-Resolve-Path $LogFilePath).ToString()

# Generate a bootstrapper for the current editor binary.
$BootstrapperPath = (Join-Path ([System.IO.Path]::GetDirectoryName($EditorBinary)) ([System.IO.Path]::GetFileNameWithoutExtension($EditorBinary) + "-EOSBootstrapper.exe"))
$BootstrapperConfigPath = (Join-Path ([System.IO.Path]::GetDirectoryName($EditorBinary)) ([System.IO.Path]::GetFileNameWithoutExtension($EditorBinary) + "-EOSBootstrapper.ini"))
if (!(Test-Path $BootstrapperPath)) {
    Write-Host "Wrapping Unreal Engine editor with EOS bootstrapper and emitting to: $BootstrapperPath"
    if (Test-Path $BootstrapperPath) {
        Remove-Item -Force $BootstrapperPath
    }
    if (Test-Path $BootstrapperConfigPath) {
        Remove-Item -Force $BootstrapperConfigPath
    }
    Push-Location "$SdkLocation\Tools"
    try {
        & "$SdkLocation\Tools\EOSBootstrapperTool.exe" `
            -o "$BootstrapperPath" `
            -a ([System.IO.Path]::GetFileNameWithoutExtension($EditorBinary) + "-Cmd.exe") `
            -f
    } finally {
        Pop-Location
    }
}

# Install the EOS bootstrapper services if they haven't been installed for the
# product ID the project is configured for.
if (!(Test-Path "HKLM:\SOFTWARE\WOW6432Node\Epic Games\EOS\RegisteredProducts\$ProductId")) {
    & "$SdkLocation\Tools\EpicOnlineServicesInstaller.exe" /install productId=$ProductId /quiet
    while ($null -ne (Get-Process -ErrorAction SilentlyContinue "EpicOnlineServicesInstaller")) {
        Start-Sleep -Seconds 1
        Write-Host "Waiting for EOS installer to finish..."
    }
}

# Launch the editor wrapped with the bootstrapper in EOS authorizer mode.
Push-Location ([System.IO.Path]::GetDirectoryName($ProjectPath))
if (Test-Path $LogFilePath) {
    Remove-Item -Force $LogFilePath
}
try {
    Write-Host "Launching editor in EOS authorization mode..."
    $Process = Start-Process `
        -FilePath "$BootstrapperPath" `
        -ArgumentList @(
            "`"$ProjectPath`"",
            "/Engine/Maps/Entry?Game=/Script/Engine.GameModeBase",
            "-eosauthorizer",
            "-game",
            "-ini:Engine:[EpicOnlineServices]:AuthenticationGraph=CrossPlatformOnly",
            "-ini:Engine:[EpicOnlineServices]:DisablePersistentLogin=True",
            "-ini:Engine:[EpicOnlineServices]:CrossPlatformAccountProvider=EpicGames",
            "-ini:Engine:[EpicOnlineServices]:RequireCrossPlatformAccount=True",
            "-ini:Engine:[/Script/EngineSettings.GameMapsSettings]:GameInstanceClass=",
            "`"-abslog=$LogFilePath`"",
            "-stdout"
        ) `
        -NoNewWindow `
        -PassThru
    $Process.Handle | Out-Null
    Write-Host
    Write-Host "---"
    Write-Host "The editor is now starting up. It may take a minute before the authorization window appears."
    Write-Host "---"
    Write-Host
    Write-Host "Waiting for editor to close after authorization..."
    while (!$Process.HasExited) {
        Start-Sleep -Seconds 1
    }
} finally {
    Pop-Location
}