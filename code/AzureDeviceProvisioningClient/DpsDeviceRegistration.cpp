// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <filesystem>
#include <fstream>
#include <assert.h>
#include <wrl\client.h>
#include "TpmSupport.h"
#include "..\Utilities\Logger.h"
#include "..\Utilities\Registry.h"
#include "..\Utilities\DMString.h"
#include "..\Utilities\AutoCloseTypes.h"
#include "..\Utilities\ResultMacros.h"

#include "NetConnection.h"

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/xlogging.h"

#include "azure_prov_client/prov_device_ll_client.h"
#include "azure_prov_client/prov_security_factory.h"
#include "azure_prov_client/prov_transport_http_client.h"

#define IOTDPSCLIENT_PARAMETERS_REGPATH             L"SYSTEM\\CurrentControlSet\\Services\\AzureDeviceManagementClient\\Parameters"
#define IOTDPSCLIENT_PARAMETERS_REGNAME_DPSSCOPE    L"IdScope"
#define IOTDPSCLIENT_PARAMETERS_REGNAME_TPMSLOT     L"logicalDeviceId"

using namespace std;
using namespace Microsoft::Windows::Azure::DeviceManagement::Utils;
using namespace Microsoft::Windows::IoT::DeviceManagement::Provisioning;
using namespace Microsoft::WRL;

extern void xlogging_set_log_function(LOGGER_LOG log_function);
void LoggingForDpsSdk(LOG_CATEGORY log_category, const char* file, const char* func, const int line, unsigned int /*options*/, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    string fmt("");
    switch (log_category)
    {
    case AZ_LOG_INFO:
        fmt = "Info: ";
        break;
    case AZ_LOG_ERROR:
    {
        int size = _scprintf("Error: File:%s Func:%s Line:%d ", file, func, line);
        vector<char> message(size + 1, '\0');
        sprintf_s(message.data(), message.size(), "Error: File:%s Func:%s Line:%d ", file, func, line);
        fmt = message.data();
    }
    break;
    default:
        break;
    }
    fmt += format;

    int size = _vscprintf(fmt.data(), args);
    vector<char> message(size + 1, '\0');
    vsprintf_s(message.data(), message.size(), fmt.data(), args);
    TRACE(LoggingLevel::Verbose, message.data());

    va_end(args);
}


#define DPS_SUCCESS 1
#define DPS_FAILURE -1
#define DPS_RUNNING 0

typedef struct CLIENT_SAMPLE_INFO_TAG
{
    unsigned int sleep_time;
    char* iothub_uri;
    char* access_key_name;
    char* device_key;
    char* device_id;
    int registration_complete;
    int slot;
} CLIENT_SAMPLE_INFO;

CLIENT_SAMPLE_INFO dps_info;

DEFINE_ENUM_STRINGS(PROV_DEVICE_RESULT, PROV_DEVICE_RESULT_VALUE);
DEFINE_ENUM_STRINGS(PROV_DEVICE_REG_STATUS, PROV_DEVICE_REG_STATUS_VALUES);


static void registation_status_callback(PROV_DEVICE_REG_STATUS reg_status, void* user_context)
{
    if (user_context == NULL)
    {
        TRACE(LoggingLevel::Verbose, "user_context is NULL");
    }
    else
    {
        CLIENT_SAMPLE_INFO* local_dps_info = (CLIENT_SAMPLE_INFO*)user_context;

        TRACEP(LoggingLevel::Verbose, "Provisioning Status: ", ENUM_TO_STRING(PROV_DEVICE_REG_STATUS, reg_status));
        if (reg_status == PROV_DEVICE_REG_STATUS_CONNECTED)
        {
            local_dps_info->sleep_time = 600;
        }
        else if (reg_status == PROV_DEVICE_REG_STATUS_REGISTERING)
        {
            local_dps_info->sleep_time = 900;
        }
        else if (reg_status == PROV_DEVICE_REG_STATUS_ASSIGNING)
        {
            local_dps_info->sleep_time = 1200;
        }
    }
}

static void register_device_callback(PROV_DEVICE_RESULT register_result, const char* iothub_uri, const char* device_id, void* user_context)
{
    TRACE(LoggingLevel::Verbose, __FUNCTION__);

    if (user_context == NULL)
    {
        printf("user_context is NULL\r\n");
    }
    else
    {
        CLIENT_SAMPLE_INFO* reg_dps_info = (CLIENT_SAMPLE_INFO*)user_context;
        if (register_result == PROV_DEVICE_RESULT_OK)
        {
            string fullUri = iothub_uri;
            fullUri += "/";
            fullUri += device_id;

            // Store connection string in TPM:
            //   limpet <slot> -SUR <uri>/<deviceId>
            TRACEP(LoggingLevel::Verbose, "call limpet <slot> -SUR: ", fullUri.c_str());
            wstring wfullUri{ MultibyteToWide(fullUri.c_str()) };
            TpmSupport::StoreIothubUri(reg_dps_info->slot, &wfullUri[0], nullptr);
            reg_dps_info->registration_complete = DPS_SUCCESS;
        }
        else
        {
            TRACEP(LoggingLevel::Verbose, "Failure encountered on registration: %s\r\n", ENUM_TO_STRING(PROV_DEVICE_RESULT, register_result));
            reg_dps_info->registration_complete = DPS_FAILURE;
        }
    }
}

HRESULT DpsRegisterDevice(
    UINT32 tpmSlotNumber,
    const std::wstring wdpsUri,
    const std::wstring wdpsScopeId
)
{
    TRACE(LoggingLevel::Verbose, __FUNCTION__);
    TRACEP(LoggingLevel::Verbose, L"tpm slot number: ", tpmSlotNumber);
    TRACEP(LoggingLevel::Verbose, L"dpsUri: ", wdpsUri.data());
    TRACEP(LoggingLevel::Verbose, L"id scope: ", wdpsScopeId.c_str());

    xlogging_set_log_function(&LoggingForDpsSdk);
    HRESULT hr = S_OK;
    bool provisioned = false;
    hr = IsDeviceProvisionedInAzure(tpmSlotNumber, &provisioned);

    if((FAILED(hr)) || (!provisioned))
    {
        memset(&dps_info, 0, sizeof(CLIENT_SAMPLE_INFO));
        dps_info.slot = tpmSlotNumber;
        string dpsScopeId = WideToMultibyte(wdpsScopeId.c_str());
        string dpsUri = WideToMultibyte(wdpsUri.c_str());

        TpmSupport::EmptyTpmSlot(tpmSlotNumber);

        do
        {
            TRACE(LoggingLevel::Verbose, "Start registration process");
            dps_info.registration_complete = DPS_RUNNING;
            dps_info.sleep_time = 10;

            AutoCloseCom com;
            RETURN_HR_IF_FAILED(com.GetInitializationStatus());

            ComPtr<CNetConnectionState> net;
            hr = MakeAndInitialize<CNetConnectionState>(&net);
            FAIL_FAST_IF_FAILED(hr, L"error occurred in initializing  Network List Manager interfaces.");
            RETURN_HR_IF_FAILED(net->WaitForConnection());

            SECURE_DEVICE_TYPE hsm_type = SECURE_DEVICE_TYPE_TPM;

            hr = platform_init();
            if (FAILED(hr))
            {
                TRACE(LoggingLevel::Error, "Failed calling platform_init");
            }

            hr = prov_dev_security_init(hsm_type);

            if (FAILED(hr))
            {
                TRACE(LoggingLevel::Error, "Failed calling prov_dev_security_init");
            }

            PROV_DEVICE_LL_HANDLE handle;
            if ((handle = Prov_Device_LL_Create(dpsUri.data(), dpsScopeId.data(), Prov_Device_HTTP_Protocol)) == NULL)
            {
                TRACE(LoggingLevel::Error, "failed calling DPS_LL_Create");
                hr = E_FAIL;
                return hr;
            }

            if (Prov_Device_LL_Register_Device(handle, register_device_callback, &dps_info, registation_status_callback, &dps_info) != PROV_DEVICE_RESULT_OK)
            {
                TRACE(LoggingLevel::Error, "failed calling Prov_Device_LL_Register_Device");
                hr = E_FAIL;
                return hr;
            }

            do
            {
                Prov_Device_LL_DoWork(handle);
                ThreadAPI_Sleep(dps_info.sleep_time);
            } while (DPS_RUNNING == dps_info.registration_complete);

            Prov_Device_LL_Destroy(handle);

            if (DPS_SUCCESS == dps_info.registration_complete)
            {
                break;
            }
            else
            {
                ThreadAPI_Sleep(5000);
            }

            TRACE(LoggingLevel::Error, "Registration failed, retry");

        } while (true);
    }

    TRACE(LoggingLevel::Verbose, "Exiting DoDpsWork");

    return hr;
}
