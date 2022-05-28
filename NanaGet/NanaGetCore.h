/*
 * PROJECT:   NanaGet
 * FILE:      NanaGetCore.h
 * PURPOSE:   Definition for NanaGet Core Implementation
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#ifndef NANA_GET_CORE
#define NANA_GET_CORE

#if (defined(__cplusplus) && __cplusplus >= 201703L)
#elif (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#else
#error "[NanaGetCore] You should use a C++ compiler with the C++17 standard."
#endif

#include <winrt/Windows.Foundation.h>

#include <filesystem>

namespace NanaGet
{
    std::filesystem::path GetApplicationFolderPath();

    bool IsPackagedMode();

    std::filesystem::path GetSettingsFolderPath();

    winrt::hstring CreateGuidString();

    winrt::hstring VFormatWindowsRuntimeString(
        _In_z_ _Printf_format_string_ wchar_t const* const Format,
        _In_z_ _Printf_format_string_ va_list ArgList);

    winrt::hstring FormatWindowsRuntimeString(
        _In_z_ _Printf_format_string_ wchar_t const* const Format,
        ...);

    winrt::hstring ConvertByteSizeToString(
        std::uint64_t ByteSize);

    winrt::hstring ConvertSecondsToTimeString(
        std::uint64_t Seconds);

    class LocalAria2Instance
    {
    public:

        LocalAria2Instance();

        ~LocalAria2Instance();

        void Startup();

        void ForceShutdown();

        void Shutdown();

        bool Available();

        std::uint16_t ServerPort();

        winrt::hstring ServerToken();

        winrt::hstring ConsoleOutput();

    private:

        std::uint16_t PickUnusedTcpPort();

    private:

        bool m_Available = false;
        std::uint16_t m_ServerPort;
        winrt::hstring m_ServerToken;
        winrt::handle m_ProcessHandle;
        winrt::file_handle m_OutputPipeHandle;
    };
}

namespace winrt::NanaGet
{
    using namespace ::NanaGet;
}

#endif // !NANA_GET_CORE
