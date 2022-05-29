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
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Web.Http.h>

#include <filesystem>

namespace winrt
{
    using Windows::Foundation::Uri;
    using Windows::Data::Json::IJsonValue;
    using Windows::Data::Json::JsonValue;
    using Windows::Web::Http::HttpClient;
}

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

    class Aria2Instance
    {
    public:
        Aria2Instance(
            winrt::Uri const& ServerUri,
            winrt::hstring const& ServerToken);

        ~Aria2Instance();

        winrt::Uri ServerUri();

        winrt::hstring ServerToken();

        void Shutdown(
            bool Force = false);

        void PauseAll(
            bool Force = false);

        void ResumeAll();

        void ClearList();

        void Pause(
            winrt::hstring Gid,
            bool Force = false);

        void Resume(
            winrt::hstring Gid);

        void Remove(
            winrt::hstring Gid,
            bool Force = false);

        winrt::JsonValue ExecuteJsonRpcCall(
            winrt::hstring const& MethodName,
            winrt::IJsonValue const& Parameters);

    protected:

        Aria2Instance();

        void UpdateInstance(
            winrt::Uri const& ServerUri,
            winrt::hstring const& ServerToken);

    private:

        winrt::Uri m_ServerUri = nullptr;
        winrt::hstring m_ServerToken;
        winrt::JsonValue m_ServerTokenJsonValue = nullptr;

        winrt::HttpClient m_HttpClient;
    };

    class LocalAria2Instance : public Aria2Instance
    {
    public:

        LocalAria2Instance();

        ~LocalAria2Instance();

        void Restart();

        bool Available();

        winrt::hstring ConsoleOutput();

    private:

        std::uint16_t PickUnusedTcpPort();

        void Startup();

        void ForceTerminate();

        void Terminate();

    private:

        bool m_Available = false;
        winrt::handle m_ProcessHandle;
        winrt::file_handle m_OutputPipeHandle;
    };
}

namespace winrt::NanaGet
{
    using namespace ::NanaGet;
}

#endif // !NANA_GET_CORE
