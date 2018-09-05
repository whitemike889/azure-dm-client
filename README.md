# azure-dm-client

<pre>
git clone --recursive https://github.com/ms-iot/azure-dm-client.git dm

cd dm

@REM for each architecture, build the following Azure SDK projectS:
@REM - azureutils
@REM - iothubclient
@REM - prov_device_ll_client
@REM - uhttp
@REM - prov_http_transport
 
cd <root>\deps\azure-iot-sdk-c
md <root>\deps\azure-iot-sdk-c\x86
cd <root>\deps\azure-iot-sdk-c\x86
cmake .. -G "Visual Studio 15 2017" -Duse_prov_client=ON -Dbuild_provisioning_service_client=ON ..
@REM open <root>\deps\azure-iot-sdk-c\x86\azure_iot_sdks.sln & build for x86

md <root>\deps\azure-iot-sdk-c\x64
cd <root>\deps\azure-iot-sdk-c\x64
cmake .. -G "Visual Studio 15 2017 Win64" -Duse_prov_client=ON -Dbuild_provisioning_service_client=ON ..
@REM open <root>\deps\azure-iot-sdk-c\x64\azure_iot_sdks.sln & build for x64
 
md <root>\deps\azure-iot-sdk-c\arm
cd <root>\deps\azure-iot-sdk-c\arm
cmake .. -G "Visual Studio 15 2017 ARM" -Duse_prov_client=ON -Dbuild_provisioning_service_client=ON ..
@REM open <root>\deps\azure-iot-sdk-c\arm\azure_iot_sdks.sln & build for arm
 
@REM open <root>\code\AzureDeviceManagement.sln & build the solution for the desired architecture.
@REM 
@REM Limpet.exe will be generated:
@REM 
@REM <root>\code\ARM\Debug\Limpet.exe
@REM <root>\code\Debug\Limpet.exe
@REM <root>\code\x64\Debug\Limpet.exe

</pre>