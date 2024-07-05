/*
 * PROJECT:   NanaGet
 * FILE:      NanaGetCore.cpp
 * PURPOSE:   Implementation for NanaGet Core Implementation
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#define _WINSOCKAPI_

#include "NanaGetCore.h"

#include "NanaGet.JsonRpc2.h"

#include <Windows.h>
#include <ShlObj.h>
#include <objbase.h>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")

#undef GetObject

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

namespace winrt
{
    //using Windows::Networking::Sockets::MessageWebSocketReceiveMode;
    //using Windows::Networking::Sockets::SocketMessageType;
    using Windows::Storage::ApplicationData;
    //using Windows::Storage::Streams::Buffer;
    //using Windows::Storage::Streams::DataReader;
    using Windows::Storage::Streams::IBuffer;
    //using Windows::Storage::Streams::IInputStream;
    //using Windows::Storage::Streams::InputStreamOptions;
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
            if (!Mile::WinRT::IsPackagedMode())
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

    return winrt::hstring(Mile::FormatWideString(
        nSystem ? L"%.2f %s" : L"%.0f %s",
        result,
        Systems[nSystem]));
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

    return winrt::hstring(Mile::FormatWideString(
        L"%d:%02d:%02d",
        Hour,
        Minute,
        Second));
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
    return this->m_GlobalStatus.DownloadSpeed;
}

std::uint64_t NanaGet::Aria2Instance::TotalUploadSpeed()
{
    return this->m_GlobalStatus.UploadSpeed;
}

void NanaGet::Aria2Instance::RefreshInformation()
{
    try
    {
        nlohmann::json Parameters;
        Parameters.push_back("token:" + this->m_ServerToken);

        nlohmann::json ResponseJson = nlohmann::json::parse(
            this->SimpleJsonRpcCall("aria2.getGlobalStat", Parameters.dump(2)));

        this->m_GlobalStatus =
            NanaGet::Aria2::ToGlobalStatusInformation(ResponseJson);
    }
    catch (...)
    {

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
        Parameters.emplace_back(this->m_GlobalStatus.NumWaiting);
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
        Parameters.emplace_back(this->m_GlobalStatus.NumStopped);
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
    NanaGet::JsonRpc2::RequestMessage Request;
    Request.Method = MethodName;
    Request.Parameters = Parameters;
    Request.Identifier = winrt::to_string(NanaGet::CreateGuidString());
    std::string RawRequest = NanaGet::JsonRpc2::FromRequestMessage(Request);
    if (RawRequest.empty())
    {
        throw winrt::hresult_invalid_argument(
            L"Invalid JSON-RPC 2.0 request message.");
    }

    NanaGet::JsonRpc2::ResponseMessage Response;
    if (!NanaGet::JsonRpc2::ToResponseMessage(
        this->SimplePost(RawRequest),
        Response) ||
        Response.Identifier != Request.Identifier)
    {
        throw winrt::hresult_illegal_method_call(
            L"Invalid JSON-RPC 2.0 response message.");
    }

    if (!Response.IsSucceeded)
    {
        throw winrt::hresult_illegal_method_call(
            winrt::to_hstring(Response.Message));
    }

    return Response.Message;
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

    /*this->CloseMessageWebSocket();

    if (!this->m_ServerUri)
    {
        return;
    }

    this->m_MessageWebSocket = winrt::MessageWebSocket();

    this->m_MessageWebSocket.Control().MessageType(
        winrt::SocketMessageType::Utf8);
    this->m_MessageWebSocket.Control().ReceiveMode(
        winrt::MessageWebSocketReceiveMode::FullMessage);
    this->m_MessageWebSocket.MessageReceived(
        { this, &NanaGet::Aria2Instance::MessageWebSocketMessageReceived });
    this->m_MessageWebSocket.Closed(
        { this, &NanaGet::Aria2Instance::WebSocketClosed });

    this->m_MessageWebSocket.ConnectAsync(
        winrt::Uri(winrt::hstring(Mile::FormatWideString(
            L"ws://%s:%d/jsonrpc",
            this->ServerUri().Host().c_str(),
            this->ServerUri().Port())))).get();*/
}

//void NanaGet::Aria2Instance::CloseMessageWebSocket()
//{
//    if (this->m_MessageWebSocket)
//    {
//        this->m_MessageWebSocket.Close();
//        this->m_MessageWebSocket = nullptr;
//    }
//}
//
//std::string ReadFromInputStream(
//    winrt::IInputStream const& Stream)
//{
//    std::string Result;
//
//    winrt::Buffer ResultBuffer(4096);
//    do
//    {
//        Stream.ReadAsync(ResultBuffer,
//            ResultBuffer.Capacity(),
//            winrt::InputStreamOptions::Partial).get();
//        Result += std::string(
//            reinterpret_cast<char*>(ResultBuffer.data()),
//            ResultBuffer.Length());
//    } while (ResultBuffer.Length() > 0);
//
//    return Result;
//}
//
//void NanaGet::Aria2Instance::MessageWebSocketMessageReceived(
//    winrt::MessageWebSocket const& sender,
//    winrt::MessageWebSocketMessageReceivedEventArgs const& e)
//{
//    UNREFERENCED_PARAMETER(sender);
//
//    std::string Message;
//    try
//    {
//        Message = ::ReadFromInputStream(e.GetDataStream());
//    }
//    catch (...)
//    {
//        this->CloseMessageWebSocket();
//        return;
//    }
//
//    try
//    {
//
//    }
//    catch (const std::exception&)
//    {
//
//    }
//
//
//    /*nlohmann::json ResponseJson;
//    try
//    {
//        ResponseJson = nlohmann::json::parse(ResponseString);
//    }
//    catch (std::exception const& ex)
//    {
//        throw winrt::hresult_illegal_method_call(winrt::to_hstring(ex.what()));
//    }
//
//    if ("2.0" != ResponseJson["jsonrpc"].get<std::string>() ||
//        Identifier != ResponseJson["id"].get<std::string>())
//    {
//        throw winrt::hresult_illegal_method_call();
//    }
//
//    if (ResponseJson.end() != ResponseJson.find("error"))
//    {
//        throw winrt::hresult_illegal_method_call(
//            winrt::to_hstring(ResponseJson["error"].get<std::string>()));
//    }
//ResponseJson["result"].dump(2);*/
//    
//
//    ::OutputDebugStringW(Mile::FormatWideString(
//        L"\r\n\r\n%s\r\n\r\n",
//        winrt::to_hstring(Message).c_str()).c_str());
//}
//
//void NanaGet::Aria2Instance::WebSocketClosed(
//    winrt::IWebSocket const& sender,
//    winrt::WebSocketClosedEventArgs const& e)
//{
//    UNREFERENCED_PARAMETER(sender);
//    UNREFERENCED_PARAMETER(e);
//
//    this->CloseMessageWebSocket();
//}

NanaGet::Aria2TaskInformation NanaGet::Aria2Instance::ParseTaskInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::DownloadInformation Information =
        NanaGet::Aria2::ToDownloadInformation(Value);

    NanaGet::Aria2TaskInformation Result;

    Result.Gid = NanaGet::Aria2::FromDownloadGid(Information.Gid);
    switch (Information.Status)
    {
    case NanaGet::Aria2::DownloadStatus::Active:
        Result.Status = NanaGet::Aria2TaskStatus::Active;
        break;
    case NanaGet::Aria2::DownloadStatus::Waiting:
        Result.Status = NanaGet::Aria2TaskStatus::Waiting;
        break;
    case NanaGet::Aria2::DownloadStatus::Paused:
        Result.Status = NanaGet::Aria2TaskStatus::Paused;
        break;
    case NanaGet::Aria2::DownloadStatus::Complete:
        Result.Status = NanaGet::Aria2TaskStatus::Complete;
        break;
    case NanaGet::Aria2::DownloadStatus::Removed:
        Result.Status = NanaGet::Aria2TaskStatus::Removed;
        break;
    default:
        Result.Status = NanaGet::Aria2TaskStatus::Error;
        break;
    }
    Result.TotalLength = Information.TotalLength;
    Result.CompletedLength = Information.CompletedLength;
    Result.DownloadSpeed = Information.DownloadSpeed;
    Result.UploadSpeed = Information.UploadSpeed;
    Result.InfoHash = Information.InfoHash;
    Result.Dir = Information.Dir;
    for (NanaGet::Aria2::FileInformation const& File : Information.Files)
    {
        NanaGet::Aria2FileInformation Current;
        Current.Index = File.Index;
        Current.Path = File.Path;
        Current.Length = File.Length;
        Current.CompletedLength = File.CompletedLength;
        Current.Selected = File.Selected;
        for (NanaGet::Aria2::UriInformation const& Uri : File.Uris)
        {
            NanaGet::Aria2UriInformation InnerCurrent;
            InnerCurrent.Uri = Uri.Uri;
            switch (Uri.Status)
            {
            case NanaGet::Aria2::UriStatus::Waiting:
                InnerCurrent.Status = NanaGet::Aria2UriStatus::Waiting;
                break;
            default:
                InnerCurrent.Status = NanaGet::Aria2UriStatus::Used;
                break;
            }
            
            Current.Uris.emplace_back(InnerCurrent);
        }
        Result.Files.emplace_back(Current);
    }
    Result.FriendlyName = NanaGet::Aria2::ToFriendlyName(Information);

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

    WSADATA WSAData = { 0 };
    if (NO_ERROR == ::WSAStartup(
        MAKEWORD(2, 2),
        &WSAData))
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
            if (SOCKET_ERROR != ::bind(
                ListenSocket,
                reinterpret_cast<LPSOCKADDR>(&Service),
                sizeof(Service)))
            {
                int NameLength = sizeof(Service);
                if (SOCKET_ERROR != ::getsockname(
                    ListenSocket,
                    reinterpret_cast<LPSOCKADDR>(&Service),
                    &NameLength))
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
        winrt::Uri(winrt::hstring(Mile::FormatWideString(
            L"http://localhost:%d/jsonrpc",
            ServerPort))),
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
