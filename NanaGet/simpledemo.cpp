﻿#include <Windows.h>
#include <objbase.h>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Web.Http.h>

#include <cstdint>
#include <cwchar>

#include <string>
#include <vector>
#include <utility>

namespace winrt
{
    using Windows::Foundation::Uri;
    using Windows::Data::Json::IJsonValue;
    using Windows::Data::Json::JsonObject;
    using Windows::Data::Json::JsonValue;
    using Windows::Web::Http::HttpClient;
    using Windows::Web::Http::HttpResponseMessage;
    using Windows::Web::Http::HttpStringContent;
}

class Aria2Client
{
public:
    Aria2Client();
    ~Aria2Client();

    winrt::JsonValue ExecuteJsonRpcCall(
        winrt::Uri const& ServerUri,
        winrt::hstring const& MethodName,
        winrt::IJsonValue const& Parameters);

private:

    winrt::Uri m_JsonRpcServerUri;
    winrt::hstring m_JsonRpcTokenString;
    winrt::JsonValue m_JsonRpcTokenJsonValue;

private:

    winrt::hstring ApplicationFolderPath();

    winrt::hstring SettingsFolderPath();

    winrt::hstring CreateGuidString();

    std::uint16_t PickUnusedTcpPort();
    
    
};

Aria2Client::Aria2Client()
    : m_JsonRpcServerUri(
        winrt::Uri(
            L"http://localhost:"
            + winrt::to_hstring(this->PickUnusedTcpPort())
            + L"/jsonrpc"))
    , m_JsonRpcTokenString(
        this->CreateGuidString())
    , m_JsonRpcTokenJsonValue(
        winrt::JsonValue::CreateStringValue(
            L"token:" + this->m_JsonRpcTokenString))
{
    winrt::hstring ExecutablePath =
        this->ApplicationFolderPath() + L"\\aria2c.exe";

    std::vector<std::pair<std::wstring, std::wstring>> Settings;
    Settings.emplace_back(
        L"enable-rpc",
        L"true");
    Settings.emplace_back(
        L"rpc-listen-port",
        winrt::to_hstring(this->m_JsonRpcServerUri.Port()));
    Settings.emplace_back(
        L"rpc-secret",
        this->m_JsonRpcTokenString);

    std::wstring Parameters;
    for (auto const& Setting : Settings)
    {
        Parameters.append(L"--");
        Parameters.append(Setting.first);

        if (!Setting.second.empty())
        {
            Parameters.append(L"=");
            Parameters.append(Setting.second);
        }

        Parameters.append(L" ");
    }
    Parameters.resize(Parameters.size() - 1);

    //::CreateProcessW()
}

Aria2Client::~Aria2Client()
{
}

winrt::hstring Aria2Client::ApplicationFolderPath()
{
    static winrt::hstring CachedResult = ([]() -> winrt::hstring
    {
        // 32767 is the maximum path length without the terminating null
        // character.
        const DWORD BufferSize = 32767;
        wchar_t Buffer[BufferSize];
        ::GetModuleFileNameW(nullptr, Buffer, BufferSize);
        std::wcsrchr(Buffer, L'\\')[0] = L'\0';
        return Buffer;
    }());

    return CachedResult;
}

winrt::hstring Aria2Client::SettingsFolderPath()
{
    static winrt::hstring CachedResult = ([]() -> winrt::hstring
    {
        using winrt::Windows::Storage::ApplicationData;
        return ApplicationData::Current().LocalFolder().Path();
    }());

    return CachedResult;
}

winrt::hstring Aria2Client::CreateGuidString()
{
    GUID Result;
    winrt::check_hresult(::CoCreateGuid(&Result));
    return winrt::to_hstring(Result);
}

std::uint16_t Aria2Client::PickUnusedTcpPort()
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

winrt::JsonValue Aria2Client::ExecuteJsonRpcCall(
    winrt::Uri const& ServerUri,
    winrt::hstring const& MethodName,
    winrt::IJsonValue const& Parameters)
{
    winrt::hstring Identifier = this->CreateGuidString();

    winrt::HttpClient Client = winrt::HttpClient();

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

    winrt::HttpResponseMessage ResponseMessage = Client.PostAsync(
        ServerUri,
        winrt::HttpStringContent(RequestJson.Stringify())).get();

    winrt::hstring ResponseString =
        ResponseMessage.Content().ReadAsStringAsync().get();

    if (!ResponseMessage.IsSuccessStatusCode() && ResponseString.empty())
    {
        ResponseMessage.EnsureSuccessStatusCode();
    }

    winrt::JsonObject ResponseJson = winrt::JsonObject::Parse(ResponseString);

    {
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
    }

    return ResponseJson.GetNamedValue(L"result");
}






#include <Mile.Windows.h>





#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")






#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Networking.Sockets.h>

#include <set>
#include <string>
#include <vector>





namespace winrt
{
    using Windows::Data::Json::JsonArray;
}








int SimpleDemoEntry()
{
    //::MessageBoxW(nullptr, winrt::to_hstring(::PickUnusedTcpPort()).data(), L"sucks", 0);

    

    /*try
    {
        ::MessageBoxW(nullptr, ::GetBaseSettingsFolderPath().data(), L"sucks", 0);
    }
    catch (...)
    {
        ::MessageBoxW(nullptr, L"unpackaged sucks", L"Fucking Error Window", 0);
    }*/

    try
    {
        winrt::JsonValue TokenValue = winrt::JsonValue::CreateStringValue(
            L"token:{E6628220-5201-424D-8E64-C23C99528851}");

        winrt::JsonArray Parameters;
        Parameters.Append(TokenValue);

        //winrt::JsonObject ResponseJson = ::ExecuteJsonRpcCall(
        //    winrt::Uri(L"http://localhost:6800/jsonrpc"),
        //    L"aria2.getVersion", //L"aria2.tellActive",
        //    Parameters).GetObject();

        winrt::JsonArray ResponseJson = Aria2Client().ExecuteJsonRpcCall(
            winrt::Uri(L"http://localhost:6800/jsonrpc"),
            L"system.listMethods",
            Parameters).GetArray();

        ::MessageBoxW(nullptr, ResponseJson.Stringify().data(), L"Sucks", 0);
    }
    catch (winrt::hresult_error const& ex)
    {
        ::MessageBoxW(nullptr, ex.message().data(), L"Fucking Error Window", 0);
    }

    

    return 0;
}

void test()
{
    /*std::set<std::uint16_t> UsedLocalTcpPorts;

    {
        ULONG TcpTableLength = 0;
        DWORD Error = ::GetExtendedTcpTable(
            nullptr,
            &TcpTableLength,
            FALSE,
            AF_INET,
            TCP_TABLE_OWNER_PID_ALL,
            0);
        if (Error == ERROR_INSUFFICIENT_BUFFER)
        {
            PMIB_TCPTABLE_OWNER_PID TcpTable =
                reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(
                    Mile::HeapMemory::Allocate(TcpTableLength));
            if (TcpTable)
            {
                Error = ::GetExtendedTcpTable(
                    TcpTable,
                    &TcpTableLength,
                    FALSE,
                    AF_INET,
                    TCP_TABLE_OWNER_PID_ALL,
                    0);
                if (Error == NO_ERROR)
                {
                    for (DWORD i = 0; i < TcpTable->dwNumEntries; ++i)
                    {
                        UsedLocalTcpPorts.insert(::ntohs(static_cast<u_short>(
                            TcpTable->table[i].dwLocalPort)));
                    }
                }

                Mile::HeapMemory::Free(TcpTable);
            }
        }
    }

    {
        ULONG TcpTableLength = 0;
        DWORD Error = ::GetExtendedTcpTable(
            nullptr,
            &TcpTableLength,
            FALSE,
            AF_INET6,
            TCP_TABLE_OWNER_PID_ALL,
            0);
        if (Error == ERROR_INSUFFICIENT_BUFFER)
        {
            PMIB_TCP6TABLE_OWNER_PID TcpTable =
                reinterpret_cast<PMIB_TCP6TABLE_OWNER_PID>(
                    Mile::HeapMemory::Allocate(TcpTableLength));
            if (TcpTable)
            {
                Error = ::GetExtendedTcpTable(
                    TcpTable,
                    &TcpTableLength,
                    FALSE,
                    AF_INET6,
                    TCP_TABLE_OWNER_PID_ALL,
                    0);
                if (Error == NO_ERROR)
                {
                    for (DWORD i = 0; i < TcpTable->dwNumEntries; ++i)
                    {
                        UsedLocalTcpPorts.insert(::ntohs(static_cast<u_short>(
                            TcpTable->table[i].dwLocalPort)));
                    }
                }

                Mile::HeapMemory::Free(TcpTable);
            }
        }
    }

    {
        ULONG TcpTableLength = 0;
        DWORD Error = ::GetExtendedUdpTable(
            nullptr,
            &TcpTableLength,
            FALSE,
            AF_INET,
            UDP_TABLE_OWNER_PID,
            0);
        if (Error == ERROR_INSUFFICIENT_BUFFER)
        {
            PMIB_UDPTABLE_OWNER_PID TcpTable =
                reinterpret_cast<PMIB_UDPTABLE_OWNER_PID>(
                    Mile::HeapMemory::Allocate(TcpTableLength));
            if (TcpTable)
            {
                Error = ::GetExtendedUdpTable(
                    TcpTable,
                    &TcpTableLength,
                    FALSE,
                    AF_INET,
                    UDP_TABLE_OWNER_PID,
                    0);
                if (Error == NO_ERROR)
                {
                    for (DWORD i = 0; i < TcpTable->dwNumEntries; ++i)
                    {
                        UsedLocalTcpPorts.insert(::ntohs(static_cast<u_short>(
                            TcpTable->table[i].dwLocalPort)));
                    }
                }

                Mile::HeapMemory::Free(TcpTable);
            }
        }
    }

    {
        ULONG TcpTableLength = 0;
        DWORD Error = ::GetExtendedUdpTable(
            nullptr,
            &TcpTableLength,
            FALSE,
            AF_INET6,
            UDP_TABLE_OWNER_PID,
            0);
        if (Error == ERROR_INSUFFICIENT_BUFFER)
        {
            PMIB_UDP6TABLE_OWNER_PID TcpTable =
                reinterpret_cast<PMIB_UDP6TABLE_OWNER_PID>(
                    Mile::HeapMemory::Allocate(TcpTableLength));
            if (TcpTable)
            {
                Error = ::GetExtendedUdpTable(
                    TcpTable,
                    &TcpTableLength,
                    FALSE,
                    AF_INET6,
                    UDP_TABLE_OWNER_PID,
                    0);
                if (Error == NO_ERROR)
                {
                    for (DWORD i = 0; i < TcpTable->dwNumEntries; ++i)
                    {
                        UsedLocalTcpPorts.insert(::ntohs(static_cast<u_short>(
                            TcpTable->table[i].dwLocalPort)));
                    }
                }

                Mile::HeapMemory::Free(TcpTable);
            }
        }
    }*/
}

// Get Download List
// 
// Reference: https://aria2.github.io/manual/en
//            /html/aria2c.html#cmdoption-max-download-result
//            https://github.com/pawamoy/aria2p
//            /blob/930f709e1d78f133e8cbd717ba4c7b9f761244ee
//            /src/aria2p/api.py#L284
// aria2.tellActive(secret)
// aria2.tellWaiting(secret, 0, 1000)
// aria2.tellStopped(secret, 0, 1000)
