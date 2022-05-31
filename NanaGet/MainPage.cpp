#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "TaskItem.h"

#include <ShlObj.h>

#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>

namespace winrt::NanaGet::implementation
{
    using Windows::ApplicationModel::DataTransfer::Clipboard;
    using Windows::ApplicationModel::DataTransfer::DataPackage;
    using Windows::UI::Core::CoreDispatcher;
    using Windows::UI::Core::CoreDispatcherPriority;
    using Windows::UI::Xaml::Visibility;

    MainPage::MainPage()
    {
        this->InitializeComponent();
        //this->SimpleDemoEntry();
        this->m_RefreshTimer = ThreadPoolTimer::CreatePeriodicTimer(
            { this, &MainPage::RefreshTimerHandler },
            std::chrono::milliseconds(200));
    }

    winrt::hstring MainPage::SearchFilter()
    {
        return this->m_SearchFilter;
    }

    void MainPage::SearchFilter(
        winrt::hstring const& Value)
    {
        this->m_SearchFilter = Value;
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

        try
        {
            this->m_Instance.ResumeAll();
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskManagerGridPauseAllButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        try
        {
            this->m_Instance.PauseAll();
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskManagerGridClearListButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        try
        {
            this->m_Instance.ClearList();
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskManagerGridOpenDownloadsFolderButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        SHELLEXECUTEINFOW ExecInfo = { 0 };
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile = NanaGet::GetDownloadsFolderPath().c_str();
        ExecInfo.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&ExecInfo);
    }

    /*void MainPage::TaskManagerGridSettingsButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->SettingsGrid().Visibility(Visibility::Visible);
    }*/

    void MainPage::TaskManagerGridAboutButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->AboutGrid().Visibility(Visibility::Visible);
    }

    void MainPage::TaskManagerGridTaskListContainerContentChanging(
        ListViewBase const& sender,
        ContainerContentChangingEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

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

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            this->m_Instance.Remove(Current.Gid());
            this->m_Instance.AddTask(Uri(Current.Source()));
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskManagerGridTaskItemResumeButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            this->m_Instance.Resume(Current.Gid());
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskManagerGridTaskItemPauseButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            this->m_Instance.Pause(Current.Gid());  
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskManagerGridTaskItemCopyLinkButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            DataPackage Data = DataPackage();
            Data.SetText(Current.Source());
            Clipboard::SetContent(Data);
        }
        catch (...)
        {

        }
    }

    /*void MainPage::TaskManagerGridTaskItemOpenFolderButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::SHOpenFolderAndSelectItems(
            ::ILCreateFromPathW([TODO]),
            0,
            nullptr,
            0);
    }*/

    void MainPage::TaskManagerGridTaskItemCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            this->m_Instance.Cancel(Current.Gid());
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskManagerGridTaskItemRemoveButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            this->m_Instance.Remove(Current.Gid());
        }
        catch (...)
        {

        }
    }

    /*void MainPage::NewTaskGridDownloadSourceBrowseButtonClick(
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
    }*/

    void MainPage::NewTaskGridDownloadButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        try
        {
            this->m_Instance.AddTask(Uri(
                this->NewTaskGridDownloadSourceTextBox().Text()));
        }
        catch (...)
        {

        }

        this->NewTaskGrid().Visibility(Visibility::Collapsed);

        this->NewTaskGridDownloadSourceTextBox().Text(L"");
    }

    void MainPage::NewTaskGridCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->NewTaskGrid().Visibility(Visibility::Collapsed);

        this->NewTaskGridDownloadSourceTextBox().Text(L"");
    }

    /*void MainPage::SettingsGridCustomDownloadFolderBrowseButtonClick(
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
    }*/

    void MainPage::AboutGridGitHubButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        SHELLEXECUTEINFOW ExecInfo = { 0 };
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile = L"https://github.com/M2Team/NanaGet";
        ExecInfo.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&ExecInfo);
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

        winrt::slim_lock_guard LockGuard(this->m_Instance.InstanceLock());

        this->m_Instance.RefreshInformation();

        winrt::hstring GlobalStatusText = NanaGet::FormatWindowsRuntimeString(
            L"\x2193 %s/s \x2191 %s/s",
            NanaGet::ConvertByteSizeToString(
                this->m_Instance.TotalDownloadSpeed()).data(),
            NanaGet::ConvertByteSizeToString(
                this->m_Instance.TotalUploadSpeed()).data());   

        winrt::hstring CurrentSearchFilter = this->m_SearchFilter;

        std::set<winrt::hstring> Gids;
        for (Aria2TaskInformation const& Task : this->m_Instance.Tasks())
        {
            if (!NanaGet::FindSubString(
                Task.FriendlyName,
                CurrentSearchFilter,
                true))
            {
                continue;
            }

            Gids.emplace(Task.Gid);
        }

        bool NeedFullRefresh = false;

        if (Gids.empty())
        {
            this->m_Gids.clear();
            this->m_Tasks = nullptr;
            NeedFullRefresh = true;
        }
        else if (this->m_Gids != Gids)
        {
            std::vector<NanaGet::TaskItem> RawTasks;
            for (Aria2TaskInformation const& Task : this->m_Instance.Tasks())
            {
                if (!NanaGet::FindSubString(
                    Task.FriendlyName,
                    CurrentSearchFilter,
                    true))
                {
                    continue;
                }

                RawTasks.push_back(
                    winrt::make<NanaGet::implementation::TaskItem>(Task));
            }

            this->m_Tasks = winrt::single_threaded_observable_vector(
                std::move(RawTasks));

            this->m_Gids = Gids;

            NeedFullRefresh = true;
        }

        CoreDispatcher Dispatcher = this->Dispatcher();
        if (!Dispatcher)
        {
            co_return;
        }
        co_await winrt::resume_foreground(Dispatcher);

        this->TaskManagerGridGlobalStatusTextBlock().Text(GlobalStatusText);

        if (NeedFullRefresh)
        {
            this->TaskManagerGridTaskList().ItemsSource(this->m_Tasks);
        }
        else
        {
            if (this->m_Tasks)
            {
                std::map<winrt::hstring, Aria2TaskInformation> RawTasks;
                for (Aria2TaskInformation const& Task : this->m_Instance.Tasks())
                {
                    RawTasks.emplace(std::pair(Task.Gid, Task));
                }

                for (NanaGet::TaskItem const& Task : this->m_Tasks)
                {
                    Task.as<NanaGet::implementation::TaskItem>()->Update(
                        RawTasks[Task.Gid()]);
                }
            }
        }
    }

    NanaGet::TaskItem MainPage::GetTaskItemFromEventSender(
        IInspectable const& sender)
    {
        return sender.try_as<Windows::UI::Xaml::FrameworkElement>(
            ).DataContext().try_as<NanaGet::TaskItem>();
    }



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

            //JsonObject ResponseJson = this->m_Instance.ExecuteJsonRpcCall(
            //    L"aria2.getVersion", //L"aria2.tellActive",
            //    Parameters).GetObject();

            //JsonArray ResponseJson = this->m_Instance.ExecuteJsonRpcCall(
            //    L"system.listMethods",
            //    Parameters).GetArray();

            JsonObject ResponseJson = this->m_Instance.ExecuteJsonRpcCall(
                L"aria2.getGlobalOption",
                Parameters).GetObject();

            winrt::hstring ResponseJsonString = ResponseJson.Stringify();

            ::MessageBoxW(nullptr, ResponseJsonString.data(), L"NanaGet", 0);
        }
        catch (winrt::hresult_error const& ex)
        {
            ::MessageBoxW(nullptr, ex.message().data(), L"NanaGet", 0);
        }

        /*::MessageBoxW(
            nullptr,
            this->m_Instance.ConsoleOutput().c_str(),
            L"NanaGet",
            MB_ICONINFORMATION);*/

        return 0;
    }
}
