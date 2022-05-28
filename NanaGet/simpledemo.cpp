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
    using Windows::Data::Json::JsonArray;
    using Windows::Data::Json::JsonObject;
    using Windows::Data::Json::JsonValue;
    using Windows::Web::Http::HttpClient;
    using Windows::Web::Http::HttpResponseMessage;
    using Windows::Web::Http::HttpStringContent;
}

struct Aria2Task
{
    winrt::hstring Gid;
    winrt::hstring Name;
    winrt::hstring Uri;
    winrt::hstring Path;
    winrt::hstring Status;

    std::uint64_t DownloadSpeed;
    std::uint64_t UploadSpeed;

    std::uint64_t BytesReceived;
    std::uint64_t TotalBytesToReceive;

    std::uint64_t RemainTime; 
};
// AddTask
// GetTasks

class Aria2Client
{
public:
    Aria2Client(
        winrt::Uri const& ServerUri,
        winrt::hstring const& ServerToken);
    ~Aria2Client();

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

void Aria2Client::Shutdown(
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

void Aria2Client::PauseAll(
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

void Aria2Client::ResumeAll()
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

void Aria2Client::ClearList()
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

void Aria2Client::Pause(
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

void Aria2Client::Resume(
    winrt::hstring Gid)
{
    winrt::JsonArray Parameters;
    Parameters.Append(this->m_ServerTokenJsonValue);
    Parameters.Append(winrt::JsonValue::CreateStringValue(Gid));

    this->ExecuteJsonRpcCall(
        L"aria2.unpause",
        Parameters);
}

void Aria2Client::Remove(
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

#include <winrt\Windows.Storage.Streams.h>

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

#include <set>
#include <string>
#include <vector>

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
    /*winrt::Uri fuck = winrt::Uri(L"nanaget-attachment://D:\\file.svg");
    ::MessageBoxW(nullptr, fuck.SchemeName().data(), L"NanaGet", 0);*/

    NanaGet::LocalAria2Instance Instance;

    //Sleep(500);

    /*std::uint16_t ServerPort = 6800;
    winrt::hstring ServerToken;*/

    winrt::Uri ServerUri = winrt::Uri(
        L"http://localhost:" + winrt::to_hstring(Instance.ServerPort()) + L"/jsonrpc");

    Aria2Client Client(ServerUri, Instance.ServerToken());

    try
    {
        winrt::JsonValue TokenValue = winrt::JsonValue::CreateStringValue(
            L"token:" + Instance.ServerToken());

        winrt::JsonArray Parameters;
        Parameters.Append(TokenValue);

        winrt::JsonObject ResponseJson = Client.ExecuteJsonRpcCall(
            L"aria2.getVersion", //L"aria2.tellActive",
            Parameters).GetObject();

        /*winrt::JsonArray ResponseJson = Client.ExecuteJsonRpcCall(
            L"system.listMethods",
            Parameters).GetArray();*/

        /*winrt::JsonObject ResponseJson = Client.ExecuteJsonRpcCall(
            L"aria2.getGlobalOption",
            Parameters).GetObject();*/

        /*winrt::JsonObject ResponseJson = Client.ExecuteJsonRpcCall(
            L"aria2.getGlobalStat",
            Parameters).GetObject();*/

        //Parameters.Append(winrt::JsonValue::CreateNumberValue(0));
        //Parameters.Append(winrt::JsonValue::CreateNumberValue(1000));

        //winrt::JsonArray ResponseJson = Client.ExecuteJsonRpcCall(
        //    /*L"aria2.tellStopped",*/ L"aria2.tellActive",// L"aria2.tellWaiting",
        //    Parameters).GetArray();

        winrt::hstring ResponseJsonString = ResponseJson.Stringify();

        ::MessageBoxW(nullptr, ResponseJsonString.data(), L"NanaGet", 0);

        Client.Shutdown();
    }
    catch (winrt::hresult_error const& ex)
    {
        ::MessageBoxW(nullptr, ex.message().data(), L"NanaGet", 0);
    }  

    ::MessageBoxW(
        nullptr,
        Instance.ConsoleOutput().c_str(),
        L"NanaGet",
        MB_ICONINFORMATION);

    return 0;
}
