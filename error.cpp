// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

// precompiled headers
#include "pch.h"

#include <format>
#include <string>
#include <Windows.h>

#include "error.h"

std::string GetLastErrorString(const std::string& errorContext) {
    using std::string;
    using std::erase_if;
    using std::unique_ptr;
    using std::remove_pointer;
    using namespace std::string_view_literals;

    const DWORD lastError{ GetLastError() };
    HANDLE lastErrorMsgBuffer{ nullptr };
    const DWORD ret{ FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        lastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // deprecated, TODO: change to GetLocaleInfoEx
        reinterpret_cast<LPSTR>(&lastErrorMsgBuffer),
        0,
        nullptr
    ) };

    constexpr auto returnFormat{ "{0} failed with error {1}: {2}"sv };
    if (ret == 0 || lastErrorMsgBuffer == nullptr) {
        return format(returnFormat, errorContext, lastError, "no additional details available (cannot retrive error message)");
    }

    // RAII for HANDLE lastErrorMsgBuffer (source: https://stackoverflow.com/questions/24611215/one-liner-for-raii-on-non-pointer)
    unique_ptr<remove_pointer<HANDLE>::type, decltype(&LocalFree)> lastErrorMsgBufferPtr{ lastErrorMsgBuffer, &LocalFree };
    // another style of RAII with custom deleter (saved as an example, source: https://stackoverflow.com/questions/65873835/custom-stl-deleter-for-any-type-of-resource) 
    // auto CleanUp = [](LPWSTR a) { LocalFree(a); }; unique_ptr<WCHAR, decltype(CleanUp)> lastErrorMsgBufferPtr(lastErrorMsgBuffer, CleanUp);

    // remove trailing new line from error message and return the result
    string msg{ reinterpret_cast<char*>(lastErrorMsgBuffer) };
    erase_if(msg, [](char x) { return iswcntrl(x); }); //shortcut for msg.erase(remove_if(msg.begin(), msg.end(), [](char x) { return iswcntrl(x); }), msg.end());
    return format(returnFormat, errorContext, lastError, msg);
}
