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
    Aria2Client(
        winrt::Uri const& ServerUri,
        winrt::hstring const& ServerToken);
    ~Aria2Client();

    winrt::JsonValue ExecuteJsonRpcCall(
        winrt::hstring const& MethodName,
        winrt::IJsonValue const& Parameters);

private:

    winrt::HttpClient m_HttpClient;

    winrt::Uri m_ServerUri;
    winrt::hstring m_ServerToken;
    winrt::JsonValue m_ServerTokenJsonValue;
};

Aria2Client::Aria2Client(
    winrt::Uri const& ServerUri,
    winrt::hstring const& ServerToken)
    : m_HttpClient(winrt::HttpClient())
    , m_ServerUri(ServerUri)
    , m_ServerToken(ServerToken)
    , m_ServerTokenJsonValue(
        winrt::JsonValue::CreateStringValue(
            L"token:" + this->m_ServerToken))
{
}

Aria2Client::~Aria2Client()
{
    this->m_HttpClient.Close();
}

winrt::JsonValue Aria2Client::ExecuteJsonRpcCall(
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

    winrt::Uri ServerUri = winrt::Uri(
        L"http://localhost:" + winrt::to_hstring(ServerPort) + L"/jsonrpc");

    Aria2Client Client(ServerUri, ServerToken);

    try
    {
        winrt::JsonValue TokenValue = winrt::JsonValue::CreateStringValue(
            L"token:" + ServerToken);

        winrt::JsonArray Parameters;
        Parameters.Append(TokenValue);

        //winrt::JsonObject ResponseJson = Client.ExecuteJsonRpcCall(
        //    L"aria2.getVersion", //L"aria2.tellActive",
        //    Parameters).GetObject();

        winrt::JsonArray ResponseJson = Client.ExecuteJsonRpcCall(
            L"system.listMethods",
            Parameters).GetArray();

        winrt::hstring ResponseJsonString = ResponseJson.Stringify();

        ::MessageBoxW(nullptr, ResponseJsonString.data(), L"NanaGet", 0);
    }
    catch (winrt::hresult_error const& ex)
    {
        ::MessageBoxW(nullptr, ex.message().data(), L"NanaGet", 0);
    }

    ::TerminateProcess(ProcessHandle.get(), 0);

    /*DWORD TotalBytesAvailable = 0;
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
    }*/

    return 0;
}
