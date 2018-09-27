:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script runs CMAKE to prepare the Azure SDK then builds the necessary SDK components and DPS app
@echo off

goto START

:Usage
echo Usage: build.azure-dm.cmd [WinSDKVer]
echo    WinSDKVer............... Default is 10.0.17134.0, specify another version if necessary
echo    [/?].................... Displays this usage string.
echo    Example:
echo        build.azure-dm.cmd 10.0.16299.0
endlocal
exit /b 1

:START
setlocal ENABLEDELAYEDEXPANSION

if [%1] == [/?] goto Usage
if [%1] == [-?] goto Usage

if [%1] == [] ( 
    set TARGETPLATVER=10.0.17134.0
) else (
    set TARGETPLATVER=%1
)

pushd %~dp0

echo .
echo "Building DM binaries"
echo .
for %%Z in (x86 ARM x64) do (
    msbuild code\Utilities\Utilities.vcxproj /p:Configuration=Debug /p:Platform=%%Z /p:TargetPlatformVersion=%TARGETPLATVER%
    msbuild code\LimpetApi\LimpetApi.vcxproj /p:Configuration=Debug /p:Platform=%%Z /p:TargetPlatformVersion=%TARGETPLATVER%
    msbuild code\AzureDeviceProvisioningClient\AzureDeviceProvisioningClient.vcxproj /p:Configuration=Debug /p:Platform=%%Z /p:TargetPlatformVersion=%TARGETPLATVER%
    msbuild code\Limpet\Limpet.vcxproj /p:Configuration=Debug /p:Platform=%%Z /p:TargetPlatformVersion=%TARGETPLATVER%
)
