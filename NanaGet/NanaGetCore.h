﻿/*
 * PROJECT:   NanaGet
 * FILE:      NanaGetCore.h
 * PURPOSE:   Definition for NanaGet Core Implementation
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#ifndef NANAGET_CORE
#define NANAGET_CORE

#if (defined(__cplusplus) && __cplusplus >= 201703L)
#elif (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#else
#error "[NanaGetCore] You should use a C++ compiler with the C++17 standard."
#endif

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Web.Http.h>

#include <filesystem>
#include <set>

namespace winrt
{
    using Windows::Foundation::Uri;
    using Windows::Data::Json::IJsonValue;
    using Windows::Data::Json::JsonObject;
    using Windows::Data::Json::JsonValue;
    using Windows::Web::Http::HttpClient;
}

namespace NanaGet
{
    std::filesystem::path GetApplicationFolderPath();

    bool IsPackagedMode();

    std::filesystem::path GetSettingsFolderPath();

    std::filesystem::path GetDownloadsFolderPath();

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

    bool FindSubString(
        winrt::hstring const& SourceString,
        winrt::hstring const& SubString,
        bool IgnoreCase);

    struct Aria2GlobalStatus
    {
        std::uint64_t DownloadSpeed;
        std::uint64_t UploadSpeed;
        std::uint64_t NumActive;
        std::uint64_t NumWaiting;
        std::uint64_t NumStopped;
        std::uint64_t NumStoppedTotal;
    };

    enum class Aria2UriStatus : std::int32_t
    {
        Used = 0,
        Waiting = 1,
    };

    struct Aria2UriInformation
    {
        winrt::hstring Uri;
        Aria2UriStatus Status;
    };

    struct Aria2FileInformation
    {
        std::uint64_t Index;
        winrt::hstring Path;
        std::uint64_t Length;
        std::uint64_t CompletedLength;
        bool Selected;
        std::vector<Aria2UriInformation> Uris;
    };

    enum class Aria2TaskStatus : std::int32_t
    {
        Active = 0,
        Waiting = 1,
        Paused = 2,
        Error = 3,
        Complete = 4,
        Removed = 5,
    };

    struct Aria2TaskInformation
    {
        winrt::hstring Gid;
        Aria2TaskStatus Status;
        std::uint64_t TotalLength;
        std::uint64_t CompletedLength;
        std::uint64_t DownloadSpeed;
        std::uint64_t UploadSpeed;
        winrt::hstring InfoHash;
        winrt::hstring Dir;
        std::vector<Aria2FileInformation> Files;
        winrt::hstring FriendlyName;
    };

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

        void Cancel(
            winrt::hstring Gid,
            bool Force = false);

        void Remove(
            winrt::hstring Gid);

        winrt::hstring AddTask(
            winrt::Uri const& Source);

        winrt::slim_mutex& InstanceLock();

        std::uint64_t TotalDownloadSpeed();

        std::uint64_t TotalUploadSpeed();

        std::vector<Aria2TaskInformation> Tasks();

        void RefreshInformation();

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

        winrt::slim_mutex m_InstanceLock;
        std::uint64_t m_TotalDownloadSpeed = 0;
        std::uint64_t m_TotalUploadSpeed = 0;
        std::vector<Aria2TaskInformation> m_Tasks;

        Aria2GlobalStatus ParseGlobalStatus(
            winrt::JsonObject Value);

        Aria2UriInformation ParseUriInformation(
            winrt::JsonObject Value);

        Aria2FileInformation ParseFileInformation(
            winrt::JsonObject Value);

        Aria2TaskInformation ParseTaskInformation(
            winrt::JsonObject Value);
    };

    class LocalAria2Instance : public Aria2Instance
    {
    public:

        LocalAria2Instance();

        ~LocalAria2Instance();

        void Restart();

        bool Available();

    private:

        std::uint16_t PickUnusedTcpPort();

        void Startup();

        void ForceTerminate();

        void Terminate();

    private:

        bool m_Available = false;
        winrt::handle m_ProcessHandle;
        winrt::handle m_JobObjectHandle;
    };
}

namespace winrt::NanaGet
{
    using namespace ::NanaGet;
}

#endif // !NANAGET_CORE
