// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

HRESULT DpsRegisterDevice(
    UINT32 tpmSlotNumber,
    const std::wstring wdpsUri,
    const std::wstring wdpsScopeId
);
