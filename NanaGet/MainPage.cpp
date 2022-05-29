#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "TaskItem.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>

namespace winrt::NanaGet::implementation
{
    using Windows::UI::Xaml::Visibility;

    MainPage::MainPage()
    {
        this->InitializeComponent();
        this->SimpleDemoEntry();
        this->m_RefreshTimer = ThreadPoolTimer::CreatePeriodicTimer(
            { this, &MainPage::RefreshTimerHandler },
            std::chrono::milliseconds(500));
    }

    MainPage::~MainPage()
    {
        if (this->m_RefreshTimer)
        {
            this->m_RefreshTimer.Cancel();
        }
    }

    void MainPage::TaskManagerGridNewTaskButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->NewTaskGrid().Visibility(Visibility::Visible);
    }

    void MainPage::TaskManagerGridStartAllButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridPauseAllButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridClearListButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridOpenDownloadsFolderButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridSettingsButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->SettingsGrid().Visibility(Visibility::Visible);
    }

    void MainPage::TaskManagerGridAboutButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->AboutGrid().Visibility(Visibility::Visible);
    }

    void MainPage::TaskManagerGridSearchBoxQuerySubmitted(
        IInspectable const& sender,
        AutoSuggestBoxQuerySubmittedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridTaskListContainerContentChanging(
        ListViewBase const& sender,
        ContainerContentChangingEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->TaskManagerGridTaskListNoItemsTextBlock().Visibility(
            !sender.ItemsSource()
            ? Visibility::Visible
            : Visibility::Collapsed);
    }

    void MainPage::TaskManagerGridTaskItemRetryButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridTaskItemResumeButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridTaskItemPauseButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridTaskItemCopyLinkButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridTaskItemOpenFolderButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridTaskItemCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::TaskManagerGridTaskItemRemoveButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::NewTaskGridDownloadSourceBrowseButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::NewTaskGridSaveFolderBrowseButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::NewTaskGridDownloadButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->NewTaskGrid().Visibility(Visibility::Collapsed);
    }

    void MainPage::NewTaskGridCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->NewTaskGrid().Visibility(Visibility::Collapsed);
    }

    void MainPage::SettingsGridCustomDownloadFolderBrowseButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void MainPage::SettingsGridApplyButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->SettingsGrid().Visibility(Visibility::Collapsed);
    }

    void MainPage::SettingsGridCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->SettingsGrid().Visibility(Visibility::Collapsed);
    }

    void MainPage::AboutGridGitHubButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        using Windows::Foundation::Uri;
        using Windows::System::Launcher;

        Launcher::LaunchUriAsync(Uri(L"https://github.com/M2Team/NanaGet"));
    }

    void MainPage::AboutGridCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->AboutGrid().Visibility(Visibility::Collapsed);
    }

    winrt::fire_and_forget MainPage::RefreshTimerHandler(
        ThreadPoolTimer const& timer)
    {
        UNREFERENCED_PARAMETER(timer);

        co_await winrt::resume_foreground(this->Dispatcher());

        using Windows::Foundation::Collections::IObservableVector;

        std::vector<NanaGet::TaskItem> contacts;
        contacts.push_back(winrt::make<NanaGet::implementation::TaskItem>());
        IObservableVector<NanaGet::TaskItem> suck =
            winrt::single_threaded_observable_vector(std::move(contacts));
        this->TaskManagerGridTaskList().ItemsSource(suck);
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

    using Windows::Data::Json::JsonArray;
    using Windows::Data::Json::JsonObject;

    int MainPage::SimpleDemoEntry()
    {
        /*winrt::Uri fuck = winrt::Uri(L"nanaget-attachment://D:\\file.svg");
    ::MessageBoxW(nullptr, fuck.SchemeName().data(), L"NanaGet", 0);*/

    //Sleep(500);

    /*std::uint16_t ServerPort = 6800;
    winrt::hstring ServerToken;*/

        try
        {
            JsonValue TokenValue = JsonValue::CreateStringValue(
                L"token:" + this->m_Instance.ServerToken());

            JsonArray Parameters;
            Parameters.Append(TokenValue);

            JsonObject ResponseJson = this->m_Instance.ExecuteJsonRpcCall(
                L"aria2.getVersion", //L"aria2.tellActive",
                Parameters).GetObject();

            //JsonArray ResponseJson = this->m_Instance.ExecuteJsonRpcCall(
            //    L"system.listMethods",
            //    Parameters).GetArray();

            //JsonObject ResponseJson = this->m_Instance.ExecuteJsonRpcCall(
            //    L"aria2.getGlobalOption",
            //    Parameters).GetObject();

            //JsonObject ResponseJson = this->m_Instance.ExecuteJsonRpcCall(
            //    L"aria2.getGlobalStat",
            //    Parameters).GetObject();

            //Parameters.Append(JsonValue::CreateNumberValue(0));
            //Parameters.Append(JsonValue::CreateNumberValue(1000));

            //JsonArray ResponseJson = this->m_Instance.ExecuteJsonRpcCall(
            //    /*L"aria2.tellStopped",*/ L"aria2.tellActive",// L"aria2.tellWaiting",
            //    Parameters).GetArray();

            winrt::hstring ResponseJsonString = ResponseJson.Stringify();

            ::MessageBoxW(nullptr, ResponseJsonString.data(), L"NanaGet", 0);
        }
        catch (winrt::hresult_error const& ex)
        {
            ::MessageBoxW(nullptr, ex.message().data(), L"NanaGet", 0);
        }

        ::MessageBoxW(
            nullptr,
            this->m_Instance.ConsoleOutput().c_str(),
            L"NanaGet",
            MB_ICONINFORMATION);

        return 0;
    }
}
