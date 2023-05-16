#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "TaskItem.h"

#include <Mile.Helpers.Base.h>

#include <ShlObj.h>

#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>

namespace winrt::NanaGet::implementation
{
    using Windows::ApplicationModel::DataTransfer::Clipboard;
    using Windows::ApplicationModel::DataTransfer::DataPackage;
    using Windows::System::DispatcherQueuePriority;
    using Windows::UI::Xaml::Visibility;

    MainPage::MainPage()
    {
        this->InitializeComponent();
        this->m_DispatcherQueue = DispatcherQueue::GetForCurrentThread();
        //this->SimpleDemoEntry();
        this->m_RefreshThread = std::thread(
            &MainPage::RefreshThreadEntryPoint,
            this);
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
        this->m_StopRefreshThread = true;
        if (this->m_RefreshThread.joinable())
        {
            this->m_RefreshThread.join();
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

        std::filesystem::path downloadsFolderPath = NanaGet::GetDownloadsFolderPath();

        SHELLEXECUTEINFOW ExecInfo = { 0 };
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile = downloadsFolderPath.c_str();
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
            this->m_Instance.Remove(winrt::to_string(Current.Gid()));
            this->m_Instance.AddTask(winrt::to_string(Current.Source()));
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
            this->m_Instance.Resume(winrt::to_string(Current.Gid()));
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
            this->m_Instance.Pause(winrt::to_string(Current.Gid()));
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
            this->m_Instance.Cancel(winrt::to_string(Current.Gid()));
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
            this->m_Instance.Remove(winrt::to_string(Current.Gid()));
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
            this->m_Instance.AddTask(winrt::to_string(
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

    void MainPage::RefreshThreadEntryPoint()
    {
        winrt::init_apartment();

        ULONGLONG PreviousTick = ::MileGetTickCount();

        while (!this->m_StopRefreshThread)
        {
            ::OutputDebugStringW(L"MainPage::RefreshThreadHandler\r\n");
            this->RefreshThreadHandler();

            ULONGLONG ElapsedTick = ::MileGetTickCount() - PreviousTick;
            if (ElapsedTick < 1000)
            {
                ::SleepEx(static_cast<DWORD>(1000 - ElapsedTick), FALSE);
            }
            PreviousTick += ElapsedTick;
        }
    }

    void MainPage::RefreshThreadHandler()
    {
        winrt::handle EventHandle;
        EventHandle.attach(::CreateEventExW(
            nullptr, nullptr,
            0,
            EVENT_ALL_ACCESS));
        winrt::check_bool(static_cast<bool>(EventHandle));

        //winrt::slim_lock_guard LockGuard(this->m_Instance.InstanceLock());

        this->m_Instance.RefreshInformation();

        winrt::hstring GlobalStatusText = NanaGet::FormatWindowsRuntimeString(
            L"\x2193 %s/s \x2191 %s/s",
            NanaGet::ConvertByteSizeToString(
                this->m_Instance.TotalDownloadSpeed()).data(),
            NanaGet::ConvertByteSizeToString(
                this->m_Instance.TotalUploadSpeed()).data());

        winrt::hstring CurrentSearchFilter = this->m_SearchFilter;

        std::vector<Aria2TaskInformation> Tasks;

        for (std::string const& Gid : this->m_Instance.GetTaskList())
        {
            Tasks.emplace_back(this->m_Instance.GetTaskInformation(Gid));
        }

        std::set<winrt::hstring> Gids;
        for (Aria2TaskInformation const& Task : Tasks)
        {
            if (!NanaGet::FindSubString(
                winrt::to_hstring(Task.FriendlyName),
                CurrentSearchFilter,
                true))
            {
                continue;
            }

            Gids.emplace(winrt::to_hstring(Task.Gid));
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
            for (Aria2TaskInformation const& Task : Tasks)
            {
                if (!NanaGet::FindSubString(
                    winrt::to_hstring(Task.FriendlyName),
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

        if (!NeedFullRefresh && this->m_Tasks)
        {
            std::map<winrt::hstring, Aria2TaskInformation> RawTasks;
            for (Aria2TaskInformation const& Task : Tasks)
            {
                RawTasks.emplace(std::pair(
                    winrt::to_hstring(Task.Gid),
                    Task));
            }

            for (NanaGet::TaskItem const& Task : this->m_Tasks)
            {
                Task.as<NanaGet::implementation::TaskItem>()->Update(
                    RawTasks[Task.Gid()]);
            }
        }

        if (!this->m_DispatcherQueue)
        {
            return;
        }
        this->m_DispatcherQueue.TryEnqueue(
            DispatcherQueuePriority::Normal,
            [&]()
        {
            this->TaskManagerGridGlobalStatusTextBlock().Text(GlobalStatusText);

            if (NeedFullRefresh)
            {
                this->TaskManagerGridTaskList().ItemsSource(this->m_Tasks);
            }
            else
            {
                if (this->m_Tasks)
                {
                    for (NanaGet::TaskItem const& Task : this->m_Tasks)
                    {
                        Task.as<NanaGet::implementation::TaskItem>()->Notify();
                    }
                }
            }

            ::SetEvent(EventHandle.get());
        });

        ::WaitForSingleObjectEx(
            EventHandle.get(),
            INFINITE,
            FALSE);
    }

    NanaGet::TaskItem MainPage::GetTaskItemFromEventSender(
        IInspectable const& sender)
    {
        return sender.try_as<Windows::UI::Xaml::FrameworkElement>(
            ).DataContext().try_as<NanaGet::TaskItem>();
    }




    int MainPage::SimpleDemoEntry()
    {
        /*winrt::Uri fuck = winrt::Uri(L"nanaget-attachment://D:\\file.svg");
    ::MessageBoxW(nullptr, fuck.SchemeName().data(), L"NanaGet", 0);*/

    //Sleep(500);

    /*std::uint16_t ServerPort = 6800;
    winrt::hstring ServerToken;*/

        try
        {
            nlohmann::json Parameters;
            Parameters.push_back(
                "token:" + this->m_Instance.ServerToken());

            //std::string ResponseJson = this->m_Instance.SimpleJsonRpcCall(
            //    "aria2.getVersion", //"aria2.tellActive",
            //    Parameters);

            //std::string ResponseJson = this->m_Instance.SimpleJsonRpcCall(
            //    L"system.listMethods",
            //    Parameters);

            std::string ResponseJson = this->m_Instance.SimpleJsonRpcCall(
                "aria2.getGlobalOption",
                Parameters);

            winrt::hstring ResponseJsonString = winrt::to_hstring(ResponseJson);

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
