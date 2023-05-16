﻿/*
 * PROJECT:   NanaGet
 * FILE:      NanaGetCore.cpp
 * PURPOSE:   Implementation for NanaGet Core Implementation
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "NanaGetCore.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <ShlObj.h>
#include <objbase.h>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")

#undef GetObject

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

namespace Mile
{
    /**
     * @brief Disables C++ class copy construction.
    */
    class DisableCopyConstruction
    {
    protected:
        DisableCopyConstruction() = default;
        ~DisableCopyConstruction() = default;

    private:
        DisableCopyConstruction(
            const DisableCopyConstruction&) = delete;
        DisableCopyConstruction& operator=(
            const DisableCopyConstruction&) = delete;
    };

    /**
     * @brief Disables C++ class move construction.
    */
    class DisableMoveConstruction
    {
    protected:
        DisableMoveConstruction() = default;
        ~DisableMoveConstruction() = default;

    private:
        DisableMoveConstruction(
            const DisableMoveConstruction&&) = delete;
        DisableMoveConstruction& operator=(
            const DisableMoveConstruction&&) = delete;
    };

    /**
     * @brief The template for defining the task when exit the scope.
     * @tparam TaskHandlerType The type of the task handler.
     * @remark For more information, see ScopeGuard.
    */
    template<typename TaskHandlerType>
    class ScopeExitTaskHandler :
        DisableCopyConstruction,
        DisableMoveConstruction
    {
    private:
        bool m_Canceled;
        TaskHandlerType m_TaskHandler;

    public:

        /**
         * @brief Creates the instance for the task when exit the scope.
         * @param TaskHandler The instance of the task handler.
        */
        explicit ScopeExitTaskHandler(TaskHandlerType&& EventHandler) :
            m_Canceled(false),
            m_TaskHandler(std::forward<TaskHandlerType>(EventHandler))
        {

        }

        /**
         * @brief Executes and uninitializes the instance for the task when
         *        exit the scope.
        */
        ~ScopeExitTaskHandler()
        {
            if (!this->m_Canceled)
            {
                this->m_TaskHandler();
            }
        }

        /**
         * @brief Cancels the task when exit the scope.
        */
        void Cancel()
        {
            this->m_Canceled = true;
        }
    };
}

namespace NanaGet
{
    NLOHMANN_JSON_SERIALIZE_ENUM(NanaGet::Aria2UriStatus, {
        { NanaGet::Aria2UriStatus::Used, "used" },
        { NanaGet::Aria2UriStatus::Waiting, "waiting" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(NanaGet::Aria2TaskStatus, {
        { NanaGet::Aria2TaskStatus::Active, "active" },
        { NanaGet::Aria2TaskStatus::Waiting, "waiting" },
        { NanaGet::Aria2TaskStatus::Paused, "paused" },
        { NanaGet::Aria2TaskStatus::Error, "error" },
        { NanaGet::Aria2TaskStatus::Complete, "complete" },
        { NanaGet::Aria2TaskStatus::Removed, "removed" }
    })
}

namespace winrt
{
    using Windows::ApplicationModel::Package;
    using Windows::Storage::ApplicationData;
    using Windows::Storage::Streams::IBuffer;
    using Windows::Web::Http::HttpResponseMessage;
    using Windows::Web::Http::HttpStringContent;
}

std::filesystem::path NanaGet::GetApplicationFolderPath()
{
    static std::filesystem::path CachedResult = ([]() -> std::filesystem::path
    {
        // 32767 is the maximum path length without the terminating null
        // character.
        const DWORD BufferSize = 32767;
        wchar_t Buffer[BufferSize];
        ::GetModuleFileNameW(nullptr, Buffer, BufferSize);
        std::wcsrchr(Buffer, L'\\')[0] = L'\0';
        return std::filesystem::path(Buffer);
    }());

    return CachedResult;
}

bool NanaGet::IsPackagedMode()
{
    static bool CachedResult = ([]() -> bool
    {
        try
        {
            const auto CurrentPackage = winrt::Package::Current();
            return true;
        }
        catch (...)
        {
            return false;
        }
    }());

    return CachedResult;
}

std::filesystem::path NanaGet::GetSettingsFolderPath()
{
    static std::filesystem::path CachedResult = ([]() -> std::filesystem::path
    {
        std::filesystem::path FolderPath;
        {
            LPWSTR RawFolderPath = nullptr;
            // KF_FLAG_FORCE_APP_DATA_REDIRECTION, when engaged, causes
            // SHGetKnownFolderPath to return the new AppModel paths
            // (Packages/xxx/RoamingState, etc.) for standard path requests.
            // Using this flag allows us to avoid
            // Windows.Storage.ApplicationData completely.
            winrt::check_hresult(::SHGetKnownFolderPath(
                FOLDERID_LocalAppData,
                KF_FLAG_FORCE_APP_DATA_REDIRECTION,
                nullptr,
                &RawFolderPath));
            FolderPath = std::filesystem::path(RawFolderPath);
            if (!NanaGet::IsPackagedMode())
            {
                FolderPath /= L"M2-Team\\NanaGet";
            }
            ::CoTaskMemFree(RawFolderPath);
        }

        // Create the directory if it doesn't exist.
        std::filesystem::create_directories(FolderPath);
        
        return FolderPath;
    }());

    return CachedResult;
}

std::filesystem::path NanaGet::GetDownloadsFolderPath()
{
    static std::filesystem::path CachedResult = ([]() -> std::filesystem::path
    {
        std::filesystem::path FolderPath;
        {
            LPWSTR RawFolderPath = nullptr;
            winrt::check_hresult(::SHGetKnownFolderPath(
                FOLDERID_Downloads,
                KF_FLAG_DEFAULT,
                nullptr,
                &RawFolderPath));
            FolderPath = std::filesystem::path(RawFolderPath);
            ::CoTaskMemFree(RawFolderPath);
        }

        // Create the directory if it doesn't exist.
        std::filesystem::create_directories(FolderPath);

        return FolderPath;
    }());

    return CachedResult;
}

winrt::hstring NanaGet::CreateGuidString()
{
    GUID Result;
    winrt::check_hresult(::CoCreateGuid(&Result));
    return winrt::to_hstring(Result);
}

winrt::hstring NanaGet::VFormatWindowsRuntimeString(
    _In_z_ _Printf_format_string_ wchar_t const* const Format,
    _In_z_ _Printf_format_string_ va_list ArgList)
{
    // Check the argument list.
    if (Format)
    {
        // Get the length of the format result.
        size_t nLength =
            static_cast<size_t>(::_vscwprintf(Format, ArgList)) + 1;

        // Allocate for the format result.
        std::wstring Buffer(nLength + 1, L'\0');

        // Format the string.
        int nWritten = ::_vsnwprintf_s(
            &Buffer[0],
            Buffer.size(),
            nLength,
            Format,
            ArgList);

        if (nWritten > 0)
        {
            // If succeed, resize to fit and return result.
            Buffer.resize(nWritten);
            return winrt::hstring(Buffer);
        }
    }

    // If failed, return an empty string.
    return winrt::hstring();
}

winrt::hstring NanaGet::FormatWindowsRuntimeString(
    _In_z_ _Printf_format_string_ wchar_t const* const Format,
    ...)
{
    va_list ArgList;
    va_start(ArgList, Format);
    winrt::hstring Result = NanaGet::VFormatWindowsRuntimeString(
        Format,
        ArgList);
    va_end(ArgList);
    return Result;
}

winrt::hstring NanaGet::ConvertByteSizeToString(
    std::uint64_t ByteSize)
{
    const wchar_t* Systems[] =
    {
        L"Byte",
        L"Bytes",
        L"KiB",
        L"MiB",
        L"GiB",
        L"TiB",
        L"PiB",
        L"EiB"
    };

    size_t nSystem = 0;
    double result = static_cast<double>(ByteSize);

    if (ByteSize > 1)
    {
        for (
            nSystem = 1;
            nSystem < sizeof(Systems) / sizeof(*Systems);
            ++nSystem)
        {
            if (1024.0 > result)
                break;

            result /= 1024.0;
        }

        result = static_cast<uint64_t>(result * 100) / 100.0;
    }

    if (true)
    {

    }

    return NanaGet::FormatWindowsRuntimeString(
        nSystem ? L"%.2f %s" : L"%.0f %s",
        result,
        Systems[nSystem]);
}

winrt::hstring NanaGet::ConvertSecondsToTimeString(
    std::uint64_t Seconds)
{
    if (static_cast<uint64_t>(-1) == Seconds)
    {
        return L"N/A";
    }

    int Hour = static_cast<int>(Seconds / 3600);
    int Minute = static_cast<int>(Seconds / 60 % 60);
    int Second = static_cast<int>(Seconds % 60);

    return NanaGet::FormatWindowsRuntimeString(
        L"%d:%02d:%02d",
        Hour,
        Minute,
        Second);
}

bool NanaGet::FindSubString(
    winrt::hstring const& SourceString,
    winrt::hstring const& SubString,
    bool IgnoreCase)
{
    return (::FindNLSStringEx(
        nullptr,
        (IgnoreCase ? NORM_IGNORECASE : 0) | FIND_FROMSTART,
        SourceString.c_str(),
        SourceString.size(),
        SubString.c_str(),
        SubString.size(),
        nullptr,
        nullptr,
        nullptr,
        0) >= 0);
}

NanaGet::Aria2Instance::Aria2Instance(
    winrt::Uri const& ServerUri,
    std::string const& ServerToken)
    : m_HttpClient(winrt::HttpClient())
{
    this->UpdateInstance(ServerUri, ServerToken);
}

NanaGet::Aria2Instance::~Aria2Instance()
{
    this->m_HttpClient.Close();
}

winrt::Uri NanaGet::Aria2Instance::ServerUri()
{
    return this->m_ServerUri;
}

std::string NanaGet::Aria2Instance::ServerToken()
{
    return this->m_ServerToken;
}

void NanaGet::Aria2Instance::Shutdown(
    bool Force)
{
    nlohmann::json Parameters;
    Parameters.push_back("token:" + this->m_ServerToken);

    if ("\"OK\"" != this->SimpleJsonRpcCall(
        Force ? "aria2.forceShutdown" : "aria2.shutdown",
        Parameters.dump(2)))
    {
        throw winrt::hresult_error();
    }
}

void NanaGet::Aria2Instance::PauseAll(
    bool Force)
{
    nlohmann::json Parameters;
    Parameters.push_back("token:" + this->m_ServerToken);

    if ("\"OK\"" != this->SimpleJsonRpcCall(
        Force ? "aria2.forcePauseAll" : "aria2.pauseAll",
        Parameters.dump(2)))
    {
        throw winrt::hresult_error();
    }
}

void NanaGet::Aria2Instance::ResumeAll()
{
    nlohmann::json Parameters;
    Parameters.push_back("token:" + this->m_ServerToken);

    if ("\"OK\"" != this->SimpleJsonRpcCall(
        "aria2.unpauseAll",
        Parameters.dump(2)))
    {
        throw winrt::hresult_error();
    }
}

void NanaGet::Aria2Instance::ClearList()
{
    nlohmann::json Parameters;
    Parameters.push_back("token:" + this->m_ServerToken);

    if ("\"OK\"" != this->SimpleJsonRpcCall(
        "aria2.purgeDownloadResult",
        Parameters.dump(2)))
    {
        throw winrt::hresult_error();
    }
}

void NanaGet::Aria2Instance::Pause(
    std::string const& Gid,
    bool Force)
{
    nlohmann::json Parameters;
    Parameters.push_back("token:" + this->m_ServerToken);
    Parameters.push_back(Gid);

    this->SimpleJsonRpcCall(
        Force ? "aria2.forcePause" : "aria2.pause",
        Parameters.dump(2));
}

void NanaGet::Aria2Instance::Resume(
    std::string const& Gid)
{
    nlohmann::json Parameters;
    Parameters.push_back("token:" + this->m_ServerToken);
    Parameters.push_back(Gid);

    this->SimpleJsonRpcCall(
        "aria2.unpause",
        Parameters.dump(2));
}

void NanaGet::Aria2Instance::Cancel(
    std::string const& Gid,
    bool Force)
{
    nlohmann::json Parameters;
    Parameters.push_back("token:" + this->m_ServerToken);
    Parameters.push_back(Gid);

    if (std::string("\"") + Gid + "\"" != this->SimpleJsonRpcCall(
        Force ? "aria2.forceRemove" : "aria2.remove",
        Parameters.dump(2)))
    {
        throw winrt::hresult_error();
    }
}

void NanaGet::Aria2Instance::Remove(
    std::string const& Gid)
{
    nlohmann::json Parameters;
    Parameters.push_back("token:" + this->m_ServerToken);
    Parameters.push_back(Gid);

    if ("\"OK\"" != this->SimpleJsonRpcCall(
        "aria2.removeDownloadResult",
        Parameters.dump(2)))
    {
        throw winrt::hresult_error();
    }
}

std::string NanaGet::Aria2Instance::AddTask(
    std::string const& Source)
{
    nlohmann::json Parameters;
    Parameters.push_back("token:" + this->m_ServerToken);

    nlohmann::json Uris;
    Uris.push_back(Source);
    Parameters.push_back(Uris);

    return this->SimpleJsonRpcCall(
        "aria2.addUri",
        Parameters.dump(2));
}

winrt::slim_mutex& NanaGet::Aria2Instance::InstanceLock()
{
    return this->m_InstanceLock;
}

std::uint64_t NanaGet::Aria2Instance::TotalDownloadSpeed()
{
    return this->m_TotalDownloadSpeed;
}

std::uint64_t NanaGet::Aria2Instance::TotalUploadSpeed()
{
    return this->m_TotalUploadSpeed;
}

void NanaGet::Aria2Instance::RefreshInformation()
{
    this->m_TotalDownloadSpeed = 0;
    this->m_TotalUploadSpeed = 0;

    {
        nlohmann::json Parameters;
        Parameters.push_back("token:" + this->m_ServerToken);

        nlohmann::json ResponseJson = nlohmann::json::parse(
            this->SimpleJsonRpcCall("aria2.getGlobalStat", Parameters.dump(2)));

        this->m_TotalDownloadSpeed = std::strtoull(
            ResponseJson["downloadSpeed"].get<std::string>().c_str(),
            nullptr,
            10);

        this->m_TotalUploadSpeed = std::strtoull(
            ResponseJson["uploadSpeed"].get<std::string>().c_str(),
            nullptr,
            10);
    }
}

NanaGet::Aria2TaskInformation NanaGet::Aria2Instance::GetTaskInformation(
    std::string const& Gid)
{
    nlohmann::json Parameters;
    Parameters.emplace_back("token:" + this->m_ServerToken);
    Parameters.emplace_back(Gid);

    nlohmann::json ResponseJson = nlohmann::json::parse(
        this->SimpleJsonRpcCall("aria2.tellStatus", Parameters.dump(2)));

    return this->ParseTaskInformation(ResponseJson);
}

std::vector<std::string> NanaGet::Aria2Instance::GetTaskList()
{
    std::vector<std::string> Result;

    std::uint64_t NumWaiting = 0;
    std::uint64_t NumStopped = 0;

    {
        nlohmann::json Parameters;
        Parameters.push_back("token:" + this->m_ServerToken);

        nlohmann::json ResponseJson = nlohmann::json::parse(
            this->SimpleJsonRpcCall("aria2.getGlobalStat", Parameters.dump(2)));

        NumWaiting = std::strtoull(
            ResponseJson["numWaiting"].get<std::string>().c_str(),
            nullptr,
            10);

        NumStopped = std::strtoull(
            ResponseJson["numStopped"].get<std::string>().c_str(),
            nullptr,
            10);
    }

    {
        nlohmann::json Parameters;
        Parameters.emplace_back("token:" + this->m_ServerToken);
        nlohmann::json Keys;
        Keys.emplace_back("gid");
        Parameters.emplace_back(Keys);

        nlohmann::json ResponseJson = nlohmann::json::parse(
            this->SimpleJsonRpcCall("aria2.tellActive", Parameters.dump(2)));

        for (nlohmann::json const& Task : ResponseJson)
        {
            Result.emplace_back(Task["gid"].get<std::string>());
        }
    }

    {
        nlohmann::json Parameters;
        Parameters.emplace_back("token:" + this->m_ServerToken);
        Parameters.emplace_back(0);
        Parameters.emplace_back(static_cast<double>(NumWaiting));
        nlohmann::json Keys;
        Keys.emplace_back("gid");
        Parameters.emplace_back(Keys);

        nlohmann::json ResponseJson = nlohmann::json::parse(
            this->SimpleJsonRpcCall("aria2.tellWaiting", Parameters.dump(2)));

        for (nlohmann::json const& Task : ResponseJson)
        {
            Result.emplace_back(Task["gid"].get<std::string>());
        }
    }

    {
        nlohmann::json Parameters;
        Parameters.emplace_back("token:" + this->m_ServerToken);
        Parameters.emplace_back(0);
        Parameters.emplace_back(static_cast<double>(NumStopped));
        nlohmann::json Keys;
        Keys.emplace_back("gid");
        Parameters.emplace_back(Keys);

        nlohmann::json ResponseJson = nlohmann::json::parse(
            this->SimpleJsonRpcCall("aria2.tellStopped", Parameters.dump(2)));

        for (nlohmann::json const& Task : ResponseJson)
        {
            Result.emplace_back(Task["gid"].get<std::string>());
        }
    }

    return Result;
}

std::string NanaGet::Aria2Instance::SimplePost(
    std::string const& Content)
{
    winrt::HttpResponseMessage ResponseMessage = this->m_HttpClient.PostAsync(
        this->m_ServerUri,
        winrt::HttpStringContent(winrt::to_hstring(Content))).get();

    winrt::IBuffer ResponseBuffer =
        ResponseMessage.Content().ReadAsBufferAsync().get();

    if (!ResponseMessage.IsSuccessStatusCode() && !ResponseBuffer.Length())
    {
        ResponseMessage.EnsureSuccessStatusCode();
    }

    return std::string(
        reinterpret_cast<char*>(ResponseBuffer.data()),
        ResponseBuffer.Length());
}

std::string NanaGet::Aria2Instance::SimpleJsonRpcCall(
    std::string const& MethodName,
    std::string const& Parameters)
{
    std::string Identifier = winrt::to_string(NanaGet::CreateGuidString());

    nlohmann::json RequestJson;
    try
    {
        RequestJson["jsonrpc"] = "2.0";
        RequestJson["method"] = MethodName;
        RequestJson["params"] = nlohmann::json::parse(Parameters);
        RequestJson["id"] = Identifier;
    }
    catch (std::exception const& ex)
    {
        throw winrt::hresult_invalid_argument(winrt::to_hstring(ex.what()));
    }
    
    std::string ResponseString = this->SimplePost(RequestJson.dump(2));

    nlohmann::json ResponseJson;
    try
    {
        ResponseJson = nlohmann::json::parse(ResponseString);
    }
    catch (std::exception const& ex)
    {
        throw winrt::hresult_illegal_method_call(winrt::to_hstring(ex.what()));
    }

    if ("2.0" != ResponseJson["jsonrpc"].get<std::string>() ||
        Identifier != ResponseJson["id"].get<std::string>())
    {
        throw winrt::hresult_illegal_method_call();
    }

    if (ResponseJson.end() != ResponseJson.find("error"))
    {
        throw winrt::hresult_illegal_method_call(
            winrt::to_hstring(ResponseJson["error"].get<std::string>()));
    }

    return ResponseJson["result"].dump(2);
}

NanaGet::Aria2Instance::Aria2Instance()
    : m_HttpClient(winrt::HttpClient())
{
}

void NanaGet::Aria2Instance::UpdateInstance(
    winrt::Uri const& ServerUri,
    std::string const& ServerToken)
{
    this->m_ServerUri = ServerUri;
    this->m_ServerToken = ServerToken;
}

NanaGet::Aria2UriInformation NanaGet::Aria2Instance::ParseUriInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2UriInformation Result;

    Result.Uri = Value["uri"].get<std::string>();
    Result.Status = Value["status"].get<NanaGet::Aria2UriStatus>();

    return Result;
}

NanaGet::Aria2FileInformation NanaGet::Aria2Instance::ParseFileInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2FileInformation Result;

    Result.Index = std::strtoull(
        Value["index"].get<std::string>().c_str(),
        nullptr,
        10);

    Result.Path = Value["path"].get<std::string>();

    Result.Length = std::strtoull(
        Value["length"].get<std::string>().c_str(),
        nullptr,
        10);

    Result.CompletedLength = std::strtoull(
        Value["completedLength"].get<std::string>().c_str(),
        nullptr,
        10);

    std::string Selected = Value["selected"].get<std::string>();
    if (0 == std::strcmp(Selected.c_str(), "true"))
    {
        Result.Selected = true;
    }
    else if (0 == std::strcmp(Selected.c_str(), "false"))
    {
        Result.Selected = false;
    }
    else
    {
        throw winrt::hresult_out_of_bounds();
    }

    nlohmann::json Uris = Value["uris"];
    for (nlohmann::json const& Uri : Uris)
    {
        Result.Uris.emplace_back(this->ParseUriInformation(Uri));
    }

    return Result;
}

NanaGet::Aria2TaskInformation NanaGet::Aria2Instance::ParseTaskInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2TaskInformation Result;

    Result.Gid = Value["gid"].get<std::string>();

    Result.Status = Value["status"].get<NanaGet::Aria2TaskStatus>();

    Result.TotalLength = std::strtoull(
        Value["totalLength"].get<std::string>().c_str(),
        nullptr,
        10);

    Result.CompletedLength = std::strtoull(
        Value["completedLength"].get<std::string>().c_str(),
        nullptr,
        10);

    Result.DownloadSpeed = std::strtoull(
        Value["downloadSpeed"].get<std::string>().c_str(),
        nullptr,
        10);

    Result.UploadSpeed = std::strtoull(
        Value["uploadSpeed"].get<std::string>().c_str(),
        nullptr,
        10);

    if (Value.contains("infoHash"))
    {
        Result.InfoHash = Value["infoHash"].get<std::string>();
    }

    Result.Dir = Value["dir"].get<std::string>();

    nlohmann::json Files = Value["files"];
    for (nlohmann::json const& File : Files)
    {
        Result.Files.emplace_back(this->ParseFileInformation(File));
    }

    if (Value.contains("bittorrent") &&
        Value["bittorrent"].contains("info") &&
        Value["bittorrent"]["info"].contains("name"))
    {
        Result.FriendlyName =
            Value["bittorrent"]["info"]["name"].get<std::string>();
    }

    if (Result.FriendlyName.empty())
    {
        if (!Result.Files.empty())
        {
            const char* Candidate = nullptr;

            if (!Result.Files[0].Path.empty())
            {
                Candidate = Result.Files[0].Path.c_str();
            }
            else if (!Result.Files[0].Uris.empty())
            {
                Candidate = Result.Files[0].Uris[0].Uri.c_str();
            }

            const char* RawName = std::strrchr(Candidate, L'/');
            Result.FriendlyName = std::string(
                (RawName && RawName != Candidate)
                ? &RawName[1]
                : Candidate);
        }
    }
    if (Result.FriendlyName.empty())
    {
        Result.FriendlyName = Result.Gid;
    }

    return Result;
}

NanaGet::LocalAria2Instance::LocalAria2Instance()
{
    this->m_JobObjectHandle.attach(
        ::CreateJobObjectW(nullptr, nullptr));
    if (!this->m_JobObjectHandle)
    {
        winrt::throw_last_error();
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION ExtendedLimitInformation = { 0 };
    ExtendedLimitInformation.BasicLimitInformation.LimitFlags =
        JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE |
        JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
    winrt::check_bool(::SetInformationJobObject(
        this->m_JobObjectHandle.get(),
        JobObjectExtendedLimitInformation,
        &ExtendedLimitInformation,
        sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION)));

    this->Startup();
}

NanaGet::LocalAria2Instance::~LocalAria2Instance()
{
    this->Terminate();
}

void NanaGet::LocalAria2Instance::Restart()
{
    this->Terminate();
    this->Startup();
}

bool NanaGet::LocalAria2Instance::Available()
{
    if (!this->m_Available)
    {
        throw winrt::hresult_illegal_method_call();
    }

    return this->m_Available;
}

std::uint16_t NanaGet::LocalAria2Instance::PickUnusedTcpPort()
{
    std::uint16_t Result = 0;

    WSADATA WSAData;
    int Status = ::WSAStartup(
        MAKEWORD(2, 2),
        &WSAData);
    if (ERROR_SUCCESS == Status)
    {
        SOCKET ListenSocket = ::socket(
            AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP);
        if (INVALID_SOCKET != ListenSocket)
        {
            sockaddr_in Service;
            Service.sin_family = AF_INET;
            Service.sin_addr.s_addr = INADDR_ANY;
            Service.sin_port = ::htons(0);
            Status = ::bind(
                ListenSocket,
                reinterpret_cast<LPSOCKADDR>(&Service),
                sizeof(Service));
            if (ERROR_SUCCESS == Status)
            {
                int NameLength = sizeof(Service);
                Status = ::getsockname(
                    ListenSocket,
                    reinterpret_cast<LPSOCKADDR>(&Service),
                    &NameLength);
                if (ERROR_SUCCESS == Status)
                {
                    Result = ::ntohs(Service.sin_port);
                }
            }
            ::closesocket(ListenSocket);
        }

        ::WSACleanup();
    }

    return Result;
}

constexpr std::string_view BitTorrentTrackers =
    "http://1337.abcvg.info:80/announce,"
    "http://milanesitracker.tekcities.com:80/announce,"
    "http://nyaa.tracker.wf:7777/announce,"
    "http://open-v6.demonoid.ch:6969/announce,"
    "http://open.acgnxtracker.com:80/announce,"
    "http://opentracker.xyz:80/announce,"
    "http://share.camoe.cn:8080/announce,"
    "http://t.overflow.biz:6969/announce,"
    "http://tr.cili001.com:8070/announce,"
    "http://tracker.bt4g.com:2095/announce,"
    "http://tracker.files.fm:6969/announce,"
    "http://tracker.gbitt.info:80/announce,"
    "http://tracker.ipv6tracker.ru:80/announce,"
    "http://tracker.mywaifu.best:6969/announce,"
    "http://tracker.noobsubs.net:80/announce,"
    "http://uraniumhexafluori.de:1919/announce,"
    "https://carbon-bonsai-621.appspot.com:443/announce,"
    "https://opentracker.i2p.rocks:443/announce,"
    "https://tr.abiir.top:443/announce,"
    "https://tr.burnabyhighstar.com:443/announce,"
    "https://tr.ready4.icu:443/announce,"
    "https://tracker.babico.name.tr:443/announce,"
    "https://tracker.baka.ink:443/announce,"
    "https://tracker.imgoingto.icu:443/announce,"
    "https://tracker.lilithraws.cf:443/announce,"
    "https://tracker.nanoha.org:443/announce,"
    "https://tracker.tamersunion.org:443/announce,"
    "https://trackme.theom.nz:443/announce,"
    "udp://9.rarbg.com:2810/announce,"
    "udp://960303.xyz:6969/announce,"
    "udp://bt1.archive.org:6969/announce,"
    "udp://exodus.desync.com:6969/announce,"
    "udp://fe.dealclub.de:6969/announce,"
    "udp://ipv4.tracker.harry.lu:80/announce,"
    "udp://mirror.aptus.co.tz:6969/announce,"
    "udp://movies.zsw.ca:6969/announce,"
    "udp://open.demonii.com:1337/announce,"
    "udp://open.dstud.io:6969/announce,"
    "udp://open.stealth.si:80/announce,"
    "udp://open.tracker.ink:6969/announce,"
    "udp://open.xxtor.com:3074/announce,"
    "udp://opentor.org:2710/announce,"
    "udp://opentracker.i2p.rocks:6969/announce,"
    "udp://p4p.arenabg.com:1337/announce,"
    "udp://public.publictracker.xyz:6969/announce,"
    "udp://run.publictracker.xyz:6969/announce,"
    "udp://thetracker.org:80/announce,"
    "udp://torrentclub.space:6969/announce,"
    "udp://tracker.0x.tf:6969/announce,"
    "udp://tracker.altrosky.nl:6969/announce,"
    "udp://tracker.auctor.tv:6969/announce,"
    "udp://tracker.beeimg.com:6969/announce,"
    "udp://tracker.birkenwald.de:6969/announce,"
    "udp://tracker.bitsearch.to:1337/announce,"
    "udp://tracker.cyberia.is:6969/announce,"
    "udp://tracker.dler.org:6969/announce,"
    "udp://tracker.dump.cl:6969/announce,"
    "udp://tracker.jordan.im:6969/announce,"
    "udp://tracker.leech.ie:1337/announce,"
    "udp://tracker.lelux.fi:6969/announce,"
    "udp://tracker.moeking.me:6969/announce,"
    "udp://tracker.monitorit4.me:6969/announce,"
    "udp://tracker.openbittorrent.com:6969/announce,"
    "udp://tracker.opentrackr.org:1337/announce,"
    "udp://tracker.pomf.se:80/announce,"
    "udp://tracker.publictracker.xyz:6969/announce,"
    "udp://tracker.srv00.com:6969/announce,"
    "udp://tracker.theoks.net:6969/announce,"
    "udp://tracker.tiny-vps.com:6969/announce,"
    "udp://tracker.torrent.eu.org:451/announce,"
    "udp://tracker1.bt.moack.co.kr:80/announce,"
    "udp://tracker2.dler.com:80/announce,"
    "udp://tracker4.itzmx.com:2710/announce,"
    "udp://tracker6.lelux.fi:6969/announce,"
    "udp://v2.iperson.xyz:6969/announce,"
    "udp://vibe.sleepyinternetfun.xyz:1738/announce,"
    "udp://www.torrent.eu.org:451/announce,"
    "ws://hub.bugout.link:80/announce,"
    "wss://tracker.openwebtorrent.com:443/announce";

void NanaGet::LocalAria2Instance::Startup()
{
    if (this->m_Available)
    {
        throw winrt::hresult_illegal_method_call();
    }

    std::uint16_t ServerPort = this->PickUnusedTcpPort();
    winrt::hstring ServerToken = NanaGet::CreateGuidString();

    winrt::file_handle Aria2InstanceOutputPipeHandle;

    auto ExitHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (!this->m_Available)
        {
            this->ForceTerminate();
        }
    });

    std::filesystem::path Aria2Executable =
        NanaGet::GetApplicationFolderPath() / L"Mile.Aria2.exe";
    std::filesystem::path Aria2LogFile =
        NanaGet::GetSettingsFolderPath() / L"aria2c.log";
    std::filesystem::path SessionFile =
        NanaGet::GetSettingsFolderPath() / L"download.session";
    std::filesystem::path DhtDataFile =
        NanaGet::GetSettingsFolderPath() / L"dht.dat";
    std::filesystem::path Dht6DataFile =
        NanaGet::GetSettingsFolderPath() / L"dht6.dat";

    std::vector<std::pair<std::wstring, std::wstring>> Settings;
    Settings.emplace_back(
        L"log",
        Aria2LogFile.c_str());
    Settings.emplace_back(
        L"log-level",
        L"notice");
    Settings.emplace_back(
        L"enable-rpc",
        L"true");
    Settings.emplace_back(
        L"rpc-listen-port",
        winrt::to_hstring(ServerPort));
    Settings.emplace_back(
        L"rpc-secret",
        ServerToken);
    Settings.emplace_back(
        L"dir",
        NanaGet::GetDownloadsFolderPath());
    Settings.emplace_back(
        L"continue",
        L"true");
    Settings.emplace_back(
        L"auto-save-interval",
        L"1");
    if (std::filesystem::exists(SessionFile))
    {
        Settings.emplace_back(
            L"input-file",
            SessionFile);
    }
    Settings.emplace_back(
        L"save-session",
        SessionFile);
    Settings.emplace_back(
        L"save-session-interval",
        L"1");
    Settings.emplace_back(
        L"dht-file-path",
        DhtDataFile);
    Settings.emplace_back(
        L"dht-file-path6",
        Dht6DataFile);
    Settings.emplace_back(
        L"bt-tracker",
        winrt::to_hstring(BitTorrentTrackers).c_str());

    std::wstring CommandLine = Aria2Executable;
    for (auto const& Setting : Settings)
    {
        CommandLine.append(L" --");
        CommandLine.append(Setting.first);

        if (!Setting.second.empty())
        {
            CommandLine.append(L"=");
            CommandLine.append(Setting.second);
        }
    }

    STARTUPINFOW StartupInfo = { 0 };
    PROCESS_INFORMATION ProcessInformation = { 0 };

    StartupInfo.cb = sizeof(STARTUPINFOW);
    StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
    StartupInfo.hStdInput = INVALID_HANDLE_VALUE;
    StartupInfo.hStdOutput = INVALID_HANDLE_VALUE;
    StartupInfo.hStdError = INVALID_HANDLE_VALUE;

    if (!::CreateProcessW(
        nullptr,
        const_cast<LPWSTR>(CommandLine.c_str()),
        nullptr,
        nullptr,
        TRUE,
        CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &StartupInfo,
        &ProcessInformation))
    {
        winrt::throw_last_error();
    }

    winrt::check_bool(::AssignProcessToJobObject(
        this->m_JobObjectHandle.get(),
        ProcessInformation.hProcess));
    this->m_ProcessHandle.attach(ProcessInformation.hProcess);
    ::ResumeThread(ProcessInformation.hThread);
    ::CloseHandle(ProcessInformation.hThread);

    this->UpdateInstance(
        winrt::Uri(NanaGet::FormatWindowsRuntimeString(
            L"http://localhost:%d/jsonrpc",
            ServerPort)),
        winrt::to_string(ServerToken));

    this->m_Available = true;
}

void NanaGet::LocalAria2Instance::ForceTerminate()
{
    this->UpdateInstance(nullptr, std::string());

    ::TerminateProcess(this->m_ProcessHandle.get(), 0);
    this->m_ProcessHandle.close();
}

void NanaGet::LocalAria2Instance::Terminate()
{
    if (!this->m_Available)
    {
        throw winrt::hresult_illegal_method_call();
    }

    this->Shutdown();
    ::WaitForSingleObjectEx(this->m_ProcessHandle.get(), 30 * 1000, FALSE);
    this->ForceTerminate();
}
