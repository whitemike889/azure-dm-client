:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script runs CMAKE to prepare the Azure SDK then builds the necessary SDK components for Limpet.
@echo off

goto START

:Usage
echo Usage: build.azure-c-sdk.cmd [WinSDKVer]
echo    WinSDKVer............... Default is 10.0.17134.0, specify another version if necessary
echo    [/?].................... Displays this usage string.
echo    Example:
echo        build.azure-c-sdk.cmd 10.0.16299.0
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
echo "Using CMAKE to set up Azure projects"
echo .
pushd deps\azure-iot-sdk-c
set NLM=^



set SPLITSTR=x86 arm,ARM x64,Win64 arm64,ARM64
set SPLITSTR=%SPLITSTR: =!NLM!%

for /f "tokens=1,2 delims==," %%i in ("!SPLITSTR!") do (
    mkdir %%i
    pushd %%i
    set BUILDSTR=Visual Studio 15 2017
    set ARCH=
    if [%%j]==[ARM64] (
        set ARCH=-A %%j
    ) else (
        IF NOT [%%j]==[] (
            set BUILDSTR=!BUILDSTR! %%j
        )
    )
    cmake -G "!BUILDSTR!" !ARCH! .. -Duse_prov_client:BOOL=ON -Duse_tpm_simulator:BOOL=OFF -Dbuild_provisioning_service_client=ON ..
    popd
)

echo .
echo "Building Azure SDK libraries"
echo .
for %%Z in (x86 arm x64 arm64) do (
    pushd %%Z
    msbuild c-utility\aziotsharedutil.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER%
    msbuild iothub_client\iothub_client.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER%
    msbuild uhttp\uhttp.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER%
    msbuild uamqp\uamqp.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER%
    msbuild provisioning_client\prov_device_ll_client.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER%
    msbuild provisioning_client\prov_http_transport.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER%
    msbuild provisioning_client\tools\tpm_device_provision\tpm_device_provision.vcxproj /p:TargetPlatformVersion=%TARGETPLATVER%
    popd
)
