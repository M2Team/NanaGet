#include <Windows.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Web.Http.h>

#include <cstdint>
#include <cwchar>

#include <string>
#include <vector>
#include <utility>

#include "NanaGetCore.h"

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
};

Aria2Client::Aria2Client()
    : m_JsonRpcServerUri(
        winrt::Uri(
            L"http://localhost:"
            + winrt::to_hstring(NanaGet::PickUnusedTcpPort())
            + L"/jsonrpc"))
    , m_JsonRpcTokenString(
        NanaGet::CreateGuidString())
    , m_JsonRpcTokenJsonValue(
        winrt::JsonValue::CreateStringValue(
            L"token:" + this->m_JsonRpcTokenString))
{
}

Aria2Client::~Aria2Client()
{
}

winrt::JsonValue Aria2Client::ExecuteJsonRpcCall(
    winrt::Uri const& ServerUri,
    winrt::hstring const& MethodName,
    winrt::IJsonValue const& Parameters)
{
    winrt::hstring Identifier = NanaGet::CreateGuidString();

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

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Networking.Sockets.h>

#include <set>
#include <string>
#include <vector>

namespace winrt
{
    using Windows::Data::Json::JsonArray;
}

std::wstring FromConsoleString(
    std::string const& Utf8String)
{
    std::wstring Utf16String;

    UINT CurrentCodePage = ::GetConsoleOutputCP();

    int Utf16StringLength = ::MultiByteToWideChar(
        CurrentCodePage,
        0,
        Utf8String.c_str(),
        static_cast<int>(Utf8String.size()),
        nullptr,
        0);
    if (Utf16StringLength > 0)
    {
        Utf16String.resize(Utf16StringLength);
        Utf16StringLength = ::MultiByteToWideChar(
            CurrentCodePage,
            0,
            Utf8String.c_str(),
            static_cast<int>(Utf8String.size()),
            &Utf16String[0],
            Utf16StringLength);
        Utf16String.resize(Utf16StringLength);
    }

    return Utf16String;
}




int SimpleDemoEntry()
{
    std::uint16_t ServerPort = 0;
    winrt::hstring ServerToken;
    winrt::handle ProcessHandle;
    winrt::file_handle OutputPipeHandle;

    NanaGet::StartLocalAria2Instance(
        ServerPort,
        ServerToken,
        ProcessHandle,
        OutputPipeHandle);

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
            L"token:" + ServerToken);

        winrt::JsonArray Parameters;
        Parameters.Append(TokenValue);

        //winrt::JsonObject ResponseJson = ::ExecuteJsonRpcCall(
        //    winrt::Uri(L"http://localhost:6800/jsonrpc"),
        //    L"aria2.getVersion", //L"aria2.tellActive",
        //    Parameters).GetObject();

        winrt::JsonArray ResponseJson = Aria2Client().ExecuteJsonRpcCall(
            winrt::Uri(L"http://localhost:" + winrt::to_hstring(ServerPort) + L"/jsonrpc"),
            L"system.listMethods",
            Parameters).GetArray();

        ::MessageBoxW(nullptr, ResponseJson.Stringify().data(), L"Sucks", 0);
    }
    catch (winrt::hresult_error const& ex)
    {
        ::MessageBoxW(nullptr, ex.message().data(), L"Fucking Error Window", 0);
    }

    ::TerminateProcess(ProcessHandle.get(), 0);

    DWORD TotalBytesAvailable = 0;
    if (::PeekNamedPipe(
        OutputPipeHandle.get(),
        nullptr,
        0,
        nullptr,
        &TotalBytesAvailable,
        nullptr))
    {
        std::string Buffer;
        Buffer.resize(TotalBytesAvailable);
        DWORD NumberOfBytesRead = 0;
        if (::ReadFile(
            OutputPipeHandle.get(),
            &Buffer[0],
            TotalBytesAvailable,
            &NumberOfBytesRead,
            nullptr))
        {
            ::MessageBoxW(
                nullptr,
                ::FromConsoleString(Buffer).c_str(),
                L"NanaGet",
                MB_ICONINFORMATION);
        }
    }

    return 0;
}

//#include <iphlpapi.h>
//#pragma comment(lib, "iphlpapi.lib")
//
//void test()
//{
//    std::set<std::uint16_t> UsedLocalTcpPorts;
//
//    {
//        ULONG TcpTableLength = 0;
//        DWORD Error = ::GetExtendedTcpTable(
//            nullptr,
//            &TcpTableLength,
//            FALSE,
//            AF_INET,
//            TCP_TABLE_OWNER_PID_ALL,
//            0);
//        if (Error == ERROR_INSUFFICIENT_BUFFER)
//        {
//            PMIB_TCPTABLE_OWNER_PID TcpTable =
//                reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(
//                    Mile::HeapMemory::Allocate(TcpTableLength));
//            if (TcpTable)
//            {
//                Error = ::GetExtendedTcpTable(
//                    TcpTable,
//                    &TcpTableLength,
//                    FALSE,
//                    AF_INET,
//                    TCP_TABLE_OWNER_PID_ALL,
//                    0);
//                if (Error == NO_ERROR)
//                {
//                    for (DWORD i = 0; i < TcpTable->dwNumEntries; ++i)
//                    {
//                        UsedLocalTcpPorts.insert(::ntohs(static_cast<u_short>(
//                            TcpTable->table[i].dwLocalPort)));
//                    }
//                }
//
//                Mile::HeapMemory::Free(TcpTable);
//            }
//        }
//    }
//
//    {
//        ULONG TcpTableLength = 0;
//        DWORD Error = ::GetExtendedTcpTable(
//            nullptr,
//            &TcpTableLength,
//            FALSE,
//            AF_INET6,
//            TCP_TABLE_OWNER_PID_ALL,
//            0);
//        if (Error == ERROR_INSUFFICIENT_BUFFER)
//        {
//            PMIB_TCP6TABLE_OWNER_PID TcpTable =
//                reinterpret_cast<PMIB_TCP6TABLE_OWNER_PID>(
//                    Mile::HeapMemory::Allocate(TcpTableLength));
//            if (TcpTable)
//            {
//                Error = ::GetExtendedTcpTable(
//                    TcpTable,
//                    &TcpTableLength,
//                    FALSE,
//                    AF_INET6,
//                    TCP_TABLE_OWNER_PID_ALL,
//                    0);
//                if (Error == NO_ERROR)
//                {
//                    for (DWORD i = 0; i < TcpTable->dwNumEntries; ++i)
//                    {
//                        UsedLocalTcpPorts.insert(::ntohs(static_cast<u_short>(
//                            TcpTable->table[i].dwLocalPort)));
//                    }
//                }
//
//                Mile::HeapMemory::Free(TcpTable);
//            }
//        }
//    }
//
//    {
//        ULONG TcpTableLength = 0;
//        DWORD Error = ::GetExtendedUdpTable(
//            nullptr,
//            &TcpTableLength,
//            FALSE,
//            AF_INET,
//            UDP_TABLE_OWNER_PID,
//            0);
//        if (Error == ERROR_INSUFFICIENT_BUFFER)
//        {
//            PMIB_UDPTABLE_OWNER_PID TcpTable =
//                reinterpret_cast<PMIB_UDPTABLE_OWNER_PID>(
//                    Mile::HeapMemory::Allocate(TcpTableLength));
//            if (TcpTable)
//            {
//                Error = ::GetExtendedUdpTable(
//                    TcpTable,
//                    &TcpTableLength,
//                    FALSE,
//                    AF_INET,
//                    UDP_TABLE_OWNER_PID,
//                    0);
//                if (Error == NO_ERROR)
//                {
//                    for (DWORD i = 0; i < TcpTable->dwNumEntries; ++i)
//                    {
//                        UsedLocalTcpPorts.insert(::ntohs(static_cast<u_short>(
//                            TcpTable->table[i].dwLocalPort)));
//                    }
//                }
//
//                Mile::HeapMemory::Free(TcpTable);
//            }
//        }
//    }
//
//    {
//        ULONG TcpTableLength = 0;
//        DWORD Error = ::GetExtendedUdpTable(
//            nullptr,
//            &TcpTableLength,
//            FALSE,
//            AF_INET6,
//            UDP_TABLE_OWNER_PID,
//            0);
//        if (Error == ERROR_INSUFFICIENT_BUFFER)
//        {
//            PMIB_UDP6TABLE_OWNER_PID TcpTable =
//                reinterpret_cast<PMIB_UDP6TABLE_OWNER_PID>(
//                    Mile::HeapMemory::Allocate(TcpTableLength));
//            if (TcpTable)
//            {
//                Error = ::GetExtendedUdpTable(
//                    TcpTable,
//                    &TcpTableLength,
//                    FALSE,
//                    AF_INET6,
//                    UDP_TABLE_OWNER_PID,
//                    0);
//                if (Error == NO_ERROR)
//                {
//                    for (DWORD i = 0; i < TcpTable->dwNumEntries; ++i)
//                    {
//                        UsedLocalTcpPorts.insert(::ntohs(static_cast<u_short>(
//                            TcpTable->table[i].dwLocalPort)));
//                    }
//                }
//
//                Mile::HeapMemory::Free(TcpTable);
//            }
//        }
//    }
//}

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
