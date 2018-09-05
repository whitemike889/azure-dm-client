// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>

namespace Microsoft { namespace Windows { namespace Azure { namespace DeviceManagement { namespace Utils {

    struct ISO8601DateTime
    {
        unsigned short year;
        unsigned short month;
        unsigned short day;
        unsigned short hour;
        unsigned short minute;
        unsigned short second;
        unsigned short milliseconds;
        short zoneHour;
        unsigned short zoneMinute;
    };

    class DateTime
    {
    public:

        static bool SystemTimeFromISO8601(const std::wstring& dateTimeString, SYSTEMTIME& dateTime);
        static std::wstring ISO8601FromSystemTime(const SYSTEMTIME& dateTime);

        static bool ISO8601DateTimeFromString(const std::wstring& dateTimeString, ISO8601DateTime& dateTime);
        static std::wstring StringFromISO8601DateTime(const ISO8601DateTime& dateTime, bool utc = false);

        static std::wstring CanonicalizeDateTime(const std::wstring& dateTimeString, bool utc = false);

        static std::wstring GetCurrentDateTimeString();
        static std::wstring GetDateTimeString(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);

    private:

        static void ToUTC(const ISO8601DateTime& in, ISO8601DateTime& out);
    };

}}}}}