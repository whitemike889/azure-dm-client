:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.
:: This script will build all the  components in the repo needed for Limpet.
call build.azure-c-sdk.cmd %1
call build.azure-dm.cmd %1