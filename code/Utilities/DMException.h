// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>
#include <exception>
#include <sstream> 
#include "Logger.h"
#include "DMString.h"

namespace Microsoft { namespace Windows { namespace Azure { namespace DeviceManagement { namespace Utils {

    class DMException : public std::exception
    {
    public:
        DMException(long code) :
            _code(code)
        {
            TRACELINEP(Utils::LoggingLevel::Error, L"Exception code   : ", code);
        }

        DMException(long code, const std::string& message) :
            _code(code),
            _debugMessage(message)
        {
            TRACELINEP(Utils::LoggingLevel::Error, L"Exception error code   : ", code);
            TRACELINEP(Utils::LoggingLevel::Error, L"Exception error message: ", message.c_str());
        }

        DMException(const std::string subSystem, long code, const std::string& message) :
            _subSystem(subSystem),
            _code(code),
            _debugMessage(message)
        {
            TRACELINEP(Utils::LoggingLevel::Error, L"Exception error code   : ", subSystem.c_str());
            TRACELINEP(Utils::LoggingLevel::Error, L"Exception error code   : ", code);
            // ToDo: how come message is mcbs and string is unicode?!
            TRACELINEP(Utils::LoggingLevel::Error, L"Exception error message: ", message.c_str());
        }

        std::string SubSystem() const
        {
            return _subSystem;
        }

        long Code() const
        {
            return _code;
        }

        std::string Message() const
        {
            return _debugMessage;
        }

        virtual std::string DebugMessage() const
        {
            std::basic_ostringstream<char> displayMessage;
            displayMessage << "Error: [" << _subSystem << "] Code: " << _code << ", Message: " << _debugMessage;
            return displayMessage.str();
        }

    protected:
        long _code;
        std::string _debugMessage;
        std::string _subSystem;
    };

    class DMSectionException : public DMException
    {
    public:
        DMSectionException(const std::string& sectionId, long code, const std::string& message) :
            DMException(code, message),
            _sectionId(sectionId)
        {
            TRACELINEP(Utils::LoggingLevel::Error, L"Exception error section: ", sectionId.c_str());
        }

        std::string GetSectionId() const
        {
            return _sectionId;
        }

        std::string DebugMessage() const
        {
            std::basic_ostringstream<char> displayMessage;
            displayMessage << "Section: " << _sectionId << ", Error: " << _code << ", Message: " << _debugMessage;
            return displayMessage.str();
        }
    protected:
        std::string _sectionId;
    };

}}}}}