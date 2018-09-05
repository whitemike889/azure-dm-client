// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

HRESULT InitializeDpsClient();

HRESULT DeinitializeDpsClient();

HRESULT GetEndorsementKeyPub(
    _Out_writes_bytes_all_(*pcbekPub) PBYTE ekPub,
    _Inout_ UINT32 *pcbekPub
);

HRESULT GetRegistrationId(
    __out_ecount_z(*pcchregistrationId) PWSTR registrationId,
    _Inout_ size_t *pcchRegistrationId
);

HRESULT IsDeviceProvisionedInAzure(
    UINT32 tpmSlotNumber,
    _Out_ bool *provisioned
);

HRESULT EmptyTpmSlot(UINT32 tpmSlotNumber);

HRESULT GetAzureConnectionString(
    __in UINT32 tpmSlotNumber,
    __in DWORD expiryDurationInSeconds,
    _In_opt_ PCWSTR moduleId,
    __out_ecount_z(*pcchConnectionString) PWSTR connectionString,
    _Inout_ size_t *pcchConnectionString
);

HRESULT AzureDpsRegisterDevice(
    UINT32 tpmSlotNumber,
    const std::wstring dpsUri,
    const std::wstring dpsScopeId
);
