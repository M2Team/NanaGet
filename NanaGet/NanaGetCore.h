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

#ifndef NANAGET_CORE
#define NANAGET_CORE

#if (defined(__cplusplus) && __cplusplus >= 201703L)
#elif (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#else
#error "[NanaGetCore] You should use a C++ compiler with the C++17 standard."
#endif

#include <Mile.Helpers.CppWinRT.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>

#include <filesystem>
#include <set>

#include <json.hpp>

namespace winrt
{
    using Windows::Foundation::Uri;
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
        std::string Uri;
        Aria2UriStatus Status;
    };

    struct Aria2FileInformation
    {
        std::uint64_t Index;
        std::string Path;
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
        std::string Gid;
        Aria2TaskStatus Status;
        std::uint64_t TotalLength;
        std::uint64_t CompletedLength;
        std::uint64_t DownloadSpeed;
        std::uint64_t UploadSpeed;
        std::string InfoHash;
        std::string Dir;
        std::vector<Aria2FileInformation> Files;
        std::string FriendlyName;
    };

    class Aria2Instance
    {
    public:
        Aria2Instance(
            winrt::Uri const& ServerUri,
            std::string const& ServerToken);

        ~Aria2Instance();

        winrt::Uri ServerUri();

        std::string ServerToken();

        void Shutdown(
            bool Force = false);

        void PauseAll(
            bool Force = false);

        void ResumeAll();

        void ClearList();

        void Pause(
            std::string const& Gid,
            bool Force = false);

        void Resume(
            std::string const& Gid);

        void Cancel(
            std::string const& Gid,
            bool Force = false);

        void Remove(
            std::string const& Gid);

        std::string AddTask(
            std::string const& Source);

        winrt::slim_mutex& InstanceLock();

        std::uint64_t TotalDownloadSpeed();

        std::uint64_t TotalUploadSpeed();

        void RefreshInformation();

        Aria2TaskInformation GetTaskInformation(
            std::string const& Gid);

        std::vector<std::string> GetTaskList();

        std::string SimplePost(
            std::string const& Content);

        std::string SimpleJsonRpcCall(
            std::string const& MethodName,
            std::string const& Parameters);

    protected:

        Aria2Instance();

        void UpdateInstance(
            winrt::Uri const& ServerUri,
            std::string const& ServerToken);

    private:

        winrt::Uri m_ServerUri = nullptr;
        std::string m_ServerToken;

        winrt::HttpClient m_HttpClient;

        winrt::slim_mutex m_InstanceLock;
        std::uint64_t m_TotalDownloadSpeed = 0;
        std::uint64_t m_TotalUploadSpeed = 0;

        Aria2UriInformation ParseUriInformation(
            nlohmann::json const& Value);

        Aria2FileInformation ParseFileInformation(
            nlohmann::json const& Value);

        Aria2TaskInformation ParseTaskInformation(
            nlohmann::json const& Value);
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

namespace winrt::Mile
{
    using namespace ::Mile;
}

#endif // !NANAGET_CORE
