// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>

namespace Microsoft { namespace Windows { namespace Azure { namespace DeviceManagement { namespace Utils {

    class Tpm
    {
    public:
        static std::string GetEndorsementKey();
        static std::string GetSRootKey();
        static void DestroyServiceUrl(int logicalId);
        static void StoreServiceUrl(int logicalId, const std::string& url);
        static void ImportSymetricIdentity(int logicalId, const std::string& hostageFile);
        static void EvictHmacKey(int logicalId);
        static std::string GetServiceUrl(int logicalId);
        static std::string GetSASToken(int logicalId, unsigned int durationInSeconds);
        static void ClearTPM();
    private:
        static std::string RunLimpet(const std::wstring& params);
    };

}}}}}
