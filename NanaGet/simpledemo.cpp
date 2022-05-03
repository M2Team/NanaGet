#include <Mile.Windows.h>

#include <objbase.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Web.Http.h>

#include <string>
#include <vector>

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

winrt::hstring GetBaseSettingsFolderPath()
{
    static winrt::hstring CachedResult = ([]() -> winrt::hstring
    {
        using winrt::Windows::Storage::ApplicationData;
        return ApplicationData::Current().LocalFolder().Path();
    }());

    return CachedResult;
}

winrt::guid CreateGuid()
{
    GUID Result;
    winrt::check_hresult(::CoCreateGuid(&Result));
    return Result;
}

winrt::JsonValue ExecuteJsonRpcCall(
    winrt::Uri const& ServerUri,
    winrt::hstring const& MethodName,
    winrt::IJsonValue const& Parameters)
{
    winrt::hstring Identifier = winrt::to_hstring(::CreateGuid());

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
    std::vector<std::wstring> LaunchParameters;
    LaunchParameters.emplace_back(L"--enable-rpc=true");
    LaunchParameters.emplace_back(L"--rpc-secret={" + winrt::to_hstring(::CreateGuid()) + L"}");
    LaunchParameters.emplace_back(L"--check-certificate=false");

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
        winrt::JsonArray Parameters;
        Parameters.Append(
            winrt::JsonValue::CreateStringValue(
                L"token:{E6628220-5201-424D-8E64-C23C99528851}"));

        winrt::JsonObject ResponseJson = ::ExecuteJsonRpcCall(
            winrt::Uri(L"http://localhost:6800/jsonrpc"),
            L"aria2.getVersion", //L"aria2.tellActive",
            Parameters).GetObject();

        ::MessageBoxW(nullptr, ResponseJson.Stringify().data(), L"Sucks", 0);
    }
    catch (winrt::hresult_error const& ex)
    {
        ::MessageBoxW(nullptr, ex.message().data(), L"Fucking Error Window", 0);
    }

    

    return 0;
}
