/*
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

namespace winrt
{
    using Windows::ApplicationModel::Package;
    using Windows::Data::Json::JsonArray;
    using Windows::Data::Json::JsonObject;
    using Windows::Storage::ApplicationData;
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

    return NanaGet::FormatWindowsRuntimeString(
        L"%.2f %s",
        result,
        Systems[nSystem]);
}

winrt::hstring NanaGet::ConvertSecondsToTimeString(
    std::uint64_t Seconds)
{
    int Hour = static_cast<int>(Seconds / 3600);
    int Minute = static_cast<int>(Seconds / 60 % 60);
    int Second = static_cast<int>(Seconds % 60);

    return NanaGet::FormatWindowsRuntimeString(
        L"%d:%02d:%02d",
        Hour,
        Minute,
        Second);
}

NanaGet::Aria2Instance::Aria2Instance(
    winrt::Uri const& ServerUri,
    winrt::hstring const& ServerToken)
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

winrt::hstring NanaGet::Aria2Instance::ServerToken()
{
    return this->m_ServerToken;
}

void NanaGet::Aria2Instance::Shutdown(
    bool Force)
{
    winrt::JsonArray Parameters;
    Parameters.Append(this->m_ServerTokenJsonValue);

    if (L"OK" != this->ExecuteJsonRpcCall(
        Force ? L"aria2.forceShutdown" : L"aria2.shutdown",
        Parameters).GetString())
    {
        throw winrt::hresult_error();
    }
}

void NanaGet::Aria2Instance::PauseAll(
    bool Force)
{
    winrt::JsonArray Parameters;
    Parameters.Append(this->m_ServerTokenJsonValue);

    if (L"OK" != this->ExecuteJsonRpcCall(
        Force ? L"aria2.forcePauseAll" : L"aria2.pauseAll",
        Parameters).GetString())
    {
        throw winrt::hresult_error();
    }
}

void NanaGet::Aria2Instance::ResumeAll()
{
    winrt::JsonArray Parameters;
    Parameters.Append(this->m_ServerTokenJsonValue);

    if (L"OK" != this->ExecuteJsonRpcCall(
        L"aria2.unpauseAll",
        Parameters).GetString())
    {
        throw winrt::hresult_error();
    }
}

void NanaGet::Aria2Instance::ClearList()
{
    winrt::JsonArray Parameters;
    Parameters.Append(this->m_ServerTokenJsonValue);

    if (L"OK" != this->ExecuteJsonRpcCall(
        L"aria2.purgeDownloadResult",
        Parameters).GetString())
    {
        throw winrt::hresult_error();
    }
}

void NanaGet::Aria2Instance::Pause(
    winrt::hstring Gid,
    bool Force)
{
    winrt::JsonArray Parameters;
    Parameters.Append(this->m_ServerTokenJsonValue);
    Parameters.Append(winrt::JsonValue::CreateStringValue(Gid));

    this->ExecuteJsonRpcCall(
        Force ? L"aria2.forcePause" : L"aria2.pause",
        Parameters);
}

void NanaGet::Aria2Instance::Resume(
    winrt::hstring Gid)
{
    winrt::JsonArray Parameters;
    Parameters.Append(this->m_ServerTokenJsonValue);
    Parameters.Append(winrt::JsonValue::CreateStringValue(Gid));

    this->ExecuteJsonRpcCall(
        L"aria2.unpause",
        Parameters);
}

void NanaGet::Aria2Instance::Remove(
    winrt::hstring Gid,
    bool Force)
{
    winrt::JsonArray Parameters;
    Parameters.Append(this->m_ServerTokenJsonValue);
    Parameters.Append(winrt::JsonValue::CreateStringValue(Gid));

    this->ExecuteJsonRpcCall(
        Force ? L"aria2.forceRemove" : L"aria2.remove",
        Parameters);

    if (L"OK" != this->ExecuteJsonRpcCall(
        L"aria2.removeDownloadResult",
        Parameters).GetString())
    {
        throw winrt::hresult_error();
    }
}

winrt::JsonValue NanaGet::Aria2Instance::ExecuteJsonRpcCall(
    winrt::hstring const& MethodName,
    winrt::IJsonValue const& Parameters)
{
    winrt::hstring Identifier = NanaGet::CreateGuidString();

    winrt::JsonObject RequestJson = winrt::JsonObject();

    RequestJson.Insert(
        L"jsonrpc",
        winrt::JsonValue::CreateStringValue(L"2.0"));
    RequestJson.Insert(
        L"method",
        winrt::JsonValue::CreateStringValue(MethodName));
    RequestJson.Insert(
        L"params",
        Parameters);
    RequestJson.Insert(
        L"id",
        winrt::JsonValue::CreateStringValue(Identifier));

    winrt::HttpResponseMessage ResponseMessage = this->m_HttpClient.PostAsync(
        this->m_ServerUri,
        winrt::HttpStringContent(RequestJson.Stringify())).get();

    auto Buffer = ResponseMessage.Content().ReadAsBufferAsync().get();

    winrt::hstring ResponseString = winrt::to_hstring(std::string_view(
        reinterpret_cast<char*>(Buffer.data()),
        Buffer.Length()));

    if (!ResponseMessage.IsSuccessStatusCode() && ResponseString.empty())
    {
        ResponseMessage.EnsureSuccessStatusCode();
    }

    winrt::JsonObject ResponseJson = winrt::JsonObject::Parse(ResponseString);

    if (L"2.0" != ResponseJson.GetNamedString(L"jsonrpc") ||
        Identifier != ResponseJson.GetNamedString(L"id"))
    {
        throw winrt::hresult_illegal_method_call();
    }

    if (ResponseJson.HasKey(L"error"))
    {
        throw winrt::hresult_illegal_method_call(
            ResponseJson.GetNamedValue(L"error").Stringify());
    }

    return ResponseJson.GetNamedValue(L"result");
}

NanaGet::Aria2Instance::Aria2Instance()
    : m_HttpClient(winrt::HttpClient())
{
}

void NanaGet::Aria2Instance::UpdateInstance(
    winrt::Uri const& ServerUri,
    winrt::hstring const& ServerToken)
{
    this->m_ServerUri = ServerUri;
    this->m_ServerToken = ServerToken;
    this->m_ServerTokenJsonValue = winrt::JsonValue::CreateStringValue(
        L"token:" + this->m_ServerToken);
}

NanaGet::LocalAria2Instance::LocalAria2Instance()
{
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

winrt::hstring NanaGet::LocalAria2Instance::ConsoleOutput()
{
    if (!this->m_Available)
    {
        throw winrt::hresult_illegal_method_call();
    }

    DWORD TotalBytesAvailable = 0;
    if (::PeekNamedPipe(
        this->m_OutputPipeHandle.get(),
        nullptr,
        0,
        nullptr,
        &TotalBytesAvailable,
        nullptr))
    {
        std::string Buffer;
        Buffer.resize(TotalBytesAvailable);
        
        if (::PeekNamedPipe(
            this->m_OutputPipeHandle.get(),
            &Buffer[0],
            TotalBytesAvailable,
            nullptr,
            &TotalBytesAvailable,
            nullptr))
        {
            return winrt::to_hstring(Buffer);
        }
    }

    return winrt::hstring();
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
        NanaGet::GetApplicationFolderPath() / L"aria2c.exe";
    std::filesystem::path SessionFile =
        NanaGet::GetSettingsFolderPath() / L"download.session";
    std::filesystem::path DhtDataFile =
        NanaGet::GetSettingsFolderPath() / L"dht.dat";
    std::filesystem::path Dht6DataFile =
        NanaGet::GetSettingsFolderPath() / L"dht6.dat";

    std::vector<std::pair<std::wstring, std::wstring>> Settings;
    Settings.emplace_back(
        L"enable-rpc",
        L"true");
    Settings.emplace_back(
        L"rpc-listen-port",
        winrt::to_hstring(ServerPort));
    Settings.emplace_back(
        L"rpc-secret",
        ServerToken);
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
        L"dht-file-path",
        DhtDataFile);
    Settings.emplace_back(
        L"dht-file-path6",
        Dht6DataFile);

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

    SECURITY_ATTRIBUTES PipeAttributes;
    PipeAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    PipeAttributes.bInheritHandle = TRUE;
    PipeAttributes.lpSecurityDescriptor = nullptr;
    if (!::CreatePipe(
        this->m_OutputPipeHandle.put(),
        Aria2InstanceOutputPipeHandle.put(),
        &PipeAttributes,
        0))
    {
        winrt::throw_last_error();
    }

    if (!::SetHandleInformation(
        this->m_OutputPipeHandle.get(),
        HANDLE_FLAG_INHERIT,
        FALSE))
    {
        winrt::throw_last_error();
    }

    STARTUPINFOW StartupInfo = { 0 };
    PROCESS_INFORMATION ProcessInformation = { 0 };

    StartupInfo.cb = sizeof(STARTUPINFOW);
    StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
    StartupInfo.hStdInput = INVALID_HANDLE_VALUE;
    StartupInfo.hStdOutput = Aria2InstanceOutputPipeHandle.get();
    StartupInfo.hStdError = Aria2InstanceOutputPipeHandle.get();

    if (!::CreateProcessW(
        nullptr,
        const_cast<LPWSTR>(CommandLine.c_str()),
        nullptr,
        nullptr,
        TRUE,
        CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &StartupInfo,
        &ProcessInformation))
    {
        winrt::throw_last_error();
    }

    this->m_ProcessHandle.attach(ProcessInformation.hProcess);
    ::CloseHandle(ProcessInformation.hThread);

    this->UpdateInstance(
        winrt::Uri(NanaGet::FormatWindowsRuntimeString(
            L"http://localhost:%d/jsonrpc",
            ServerPort)),
        ServerToken);

    this->m_Available = true;
}

void NanaGet::LocalAria2Instance::ForceTerminate()
{
    this->UpdateInstance(nullptr, winrt::hstring());

    ::TerminateProcess(this->m_ProcessHandle.get(), 0);
    this->m_ProcessHandle.close();

    this->m_OutputPipeHandle.close();
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
