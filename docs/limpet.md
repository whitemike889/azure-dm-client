# Limpet

####    Overview

Limpet is a console application developed using Urchin TPM library & windows API's and it uses local processes to use TPM. It simplifies various operations with TPM and sample code to use Azure DPS client functionality.

* Install TPM simulator (or) dTPM's for development/testing purpose.
* Read TPM general information and read endorsement key public and storage root key public.
* It helps to create/evict symmetric identities and sign data with created key.
* It supports importing PFX file into TPMKSP and machine "My" store.
* It has sample code to extract information needed for Azure DPS registration.
* It has sample code to register the device with IotHub via DPS process.


#### TPM Support

| Board | Version | Meets Azure TPM Requirements | Notes |
|-------|---------|---------|---------|
| DragonBoard | 2.0 | Yes | Firmware TPM
| RP2/3 | - | No |TPM simulator can be installed for development purpose. Keep in mind that a TPM simulator will not provide any security improvement to the platform. 
| MBM   | 2.0 | No | Firmware TPM does not support as it did not have support for HMAC commands. TPM simulator can be installed for development purpose.
| HummingBoard | NA | NA | TPM simulator can be installed for development purpose. Keep in mind that a TPM simulator will not provide any security improvement to the platform.


####    Command-line syntax
<pre>
Limpet.exe [Command]

 -VER => Show Limpet.exe version
 -LAT => List supported add-on TPMs
 -IAT [IDx] => Install specified add-on TPM

Global TPM Present Commands:
 -GTI => Get TPM Info
 -FCT => Force Clear TPM
 -PFX [PFXFile] [password] => Import PFX file into TPMKSP and machine "My" Store
 -LLD => List logical devices in use
 -ERK {File} => Get TPM Endorsement RSA Key and optionally store

Logical Device Commands in the form:
Limpet.exe [LogicalDeviceNo 0..9] [Command] [Parameter] {Optional}
 -RID => Read Device Id
 -SUR [URI] => Store Service URI
 -RUR => Read Service URI
 -DUR => Destroy Service URI

HMAC Symmetric Identities:
 -CHK [HmacKey] => Create persisted HMAC Key
 -AST {Validity} => Generate the SAS token-based connection string (default validity 3600s)
 -SHK [File] => Sign data in file with persisted HMAC Key
 -EHK => Evict persisted HMAC Key

Azure device provisioning cloud service(dps) commands in the form:
Limpet.exe [-azuredps [<dps connection string>] [command] [parameters]

 -azuredps -register <tpm slot number> <global dps uri end point> <scope id> => register the current running device in Iothub using dps service.
     <tpm slot number>: tpm slot number to store the key.
     <global dps service uri>: it is generally fixed global.azure-devices-provisioning.net
     <scope id>: customer dps scope id. you can get this information from azure portal dps service ->overview ->id scope.
 -azuredps -enrollmentinfo <csv/json/txt> => read device tpm information needed for enrolling the device later in dps and output in given format.

</pre>

#### User Priviliges

 Requires administrator prviliges to run Limpet.exe.

#### Setup Azure cloud resources

Setup cloud resources by following steps mentioned in https://docs.microsoft.com/en-us/azure/iot-dps/tutorial-set-up-cloud and gather the information below.

    --ID Scope - You can get from Azure portal -> Device Provisioning Services -> Overview -> ID Scope.
    --Global device end point - You can get from Azure portal -> Device Provisioning Services -> Overview -> Global device endpoint.  

#### Enroll device with Azure device provisioning service -  offline manufacturing process

Use this process if the device can not be or should not be registered with an IoT Hub at manufacturing. The DPS enrollment information can be collected through this process. 

##### Pre-requisites:
* Set up Windows IoT device with TPM by using the below link if you have not already.
    https://developer.microsoft.com/en-us/windows/iot/getstarted

* Connect to device using PowerShell by using device administrator credentials from your development machine.

* Copy the limpet tool (limpet.exe built in the previous step) to Windows IoT device.

##### Steps for collecting information from the device:
* Run limpet tool from remote powershell connection.
    limpet.exe -azuredps -enrollmentinfo
    Tool prints endorsement key and registration id, please note down.

Note that this step is for development purpose, feel free to use this tool integrate with your system integrator solution to collect information from the device and upload it to Azure using the tools provided by them.
  
  Here is the screenshot for reference,
  
  <img src="limpetazuredpsenrollinfo.PNG"/>

* Enroll the device in DPS by following TPM based devices steps in the below link,
    https://docs.microsoft.com/en-us/azure/iot-dps/tutorial-provision-device-to-hub#enrolldevice

#### Register a device with IoT Hub via DPS - first boot time
Use this registration process if you like to register the device with an IoT Hub during first boot. This is a sample test code written in limpet to use the dps client api's. Feel free to pick up the code and use it in your customized solution.

* Run the below command in remote PowerShell connection to register the current device in IotHub using DPS service.

        limpet.exe -azuredps -register <tpm slot number> <global dps uri end point> <scope id>
          <tpm slot number>: tpm slot number to store the key.
          <global dps service uri>: it is generally fixed global.azure-devices-provisioning.net
          <scope id>: customer dps scope id. you can get this information from azure portal dps service ->overview ->id scope.


##### Verification

* Option 1: From the command line,

    	run "limpet <TPM_SLOT> -rur"
        It should display the service uri of the device, which confirms the device registered successfully in Azure IoT Hub.
	
    	run "limpet <TPM_SLOT> -ast"
        It should display the IotHub connection string of the device, which confirms the device registered successfully in Azure IoT Hub.

* Option 2: You can also use the Azure portal -> iothub device explorer and can find the device.

* Option 3: Use the [Device Management(DM) hello world application](<https://github.com/ms-iot/iot-core-azure-dm-client/blob/master/docs/dm-hello-world-overview.md>) for managing the device.

Here is the screen shot for reference(morphed PII info),

<img src="limpetazuredpsregister.png"/>


#### Setting up remote PowerShell connection
	Start the PowerShell by running as administrator in your development machine.
	$ip = "<Ip Address>"
	$password = "<administrator password>"
	$username = "administrator"
	$secstr = New-Object -TypeName System.Security.SecureString
	$password.ToCharArray() | ForEach-Object {$secstr.AppendChar($_)}
	$cred = new-object -typename System.Management.Automation.PSCredential -argumentlist $username, $secstr
	Set-Item -Path WSMan:\localhost\Client\TrustedHosts -Value "$ip" -Force
	$session = New-PSSession -ComputerName $ip -Credential $cred
	Enter-Pssession $session

    If you are having trouble using remote PowerShell connection see https://developer.microsoft.com/en-us/windows/iot/docs/powershell

#### Learn more Azure device provisioning service
* [Blog](<https://azure.microsoft.com/de-de/blog/azure-iot-hub-device-provisioning-service-preview-automates-device-connection-configuration/>)
* [Documentation](<https://docs.microsoft.com/en-us/azure/iot-dps/>)
* [Service SDK (preview release)](<https://www.nuget.org/packages/Microsoft.Azure.Devices/>)
* [Device SDK](<https://github.com/Azure/azure-iot-sdk-c>)

