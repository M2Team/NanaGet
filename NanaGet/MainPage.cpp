#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "TaskItem.h"

#include <Mile.Helpers.Base.h>

#include <ShlObj.h>

#include <atlres.h>

#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>

namespace NanaGet
{
    extern HWND MainWindowHandle;
    extern LocalAria2Instance* LocalInstance;
}

namespace winrt::NanaGet::implementation
{
    using Windows::ApplicationModel::DataTransfer::Clipboard;
    using Windows::ApplicationModel::DataTransfer::DataPackage;
    using Windows::System::DispatcherQueuePriority;
    using Windows::UI::Xaml::Visibility;

    void MainPage::InitializeComponent()
    {
        MainPageT::InitializeComponent();

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

    void MainPage::NewTaskButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            NanaGet::MainWindowHandle,
            WM_COMMAND,
            MAKEWPARAM(ID_FILE_NEW, BN_CLICKED),
            0);
    }

    void MainPage::StartAllButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::LocalInstance->ResumeAll();
        }
        catch (...)
        {

        }
    }

    void MainPage::PauseAllButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::LocalInstance->PauseAll();
        }
        catch (...)
        {

        }
    }

    void MainPage::ClearListButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::LocalInstance->ClearList();
        }
        catch (...)
        {

        }
    }

    void MainPage::OpenDownloadsFolderButtonClick(
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

    void MainPage::SettingsButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            NanaGet::MainWindowHandle,
            WM_COMMAND,
            MAKEWPARAM(ID_FILE_UPDATE, BN_CLICKED),
            0);
    }

    void MainPage::AboutButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            NanaGet::MainWindowHandle,
            WM_COMMAND,
            MAKEWPARAM(ID_APP_ABOUT, BN_CLICKED),
            0);
    }

    void MainPage::TaskListContainerContentChanging(
        ListViewBase const& sender,
        ContainerContentChangingEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->TaskListNoItemsTextBlock().Visibility(
            !sender.ItemsSource()
            ? Visibility::Visible
            : Visibility::Collapsed);
    }

    void MainPage::TaskItemRetryButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            NanaGet::LocalInstance->Remove(winrt::to_string(Current.Gid()));
            NanaGet::LocalInstance->AddTask(winrt::to_string(Current.Source()));
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskItemResumeButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            NanaGet::LocalInstance->Resume(winrt::to_string(Current.Gid()));
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskItemPauseButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            NanaGet::LocalInstance->Pause(winrt::to_string(Current.Gid()));
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskItemCopyLinkButtonClick(
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

    /*void MainPage::TaskItemOpenFolderButtonClick(
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

    void MainPage::TaskItemCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            NanaGet::LocalInstance->Cancel(winrt::to_string(Current.Gid()));
        }
        catch (...)
        {

        }
    }

    void MainPage::TaskItemRemoveButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        try
        {
            NanaGet::TaskItem Current =
                this->GetTaskItemFromEventSender(sender);
            NanaGet::LocalInstance->Remove(winrt::to_string(Current.Gid()));
        }
        catch (...)
        {

        }
    }

    void MainPage::RefreshThreadEntryPoint()
    {
        winrt::init_apartment();

        ULONGLONG PreviousTick = ::MileGetTickCount();

        while (!this->m_StopRefreshThread)
        {
            //::OutputDebugStringW(L"MainPage::RefreshThreadHandler\r\n");
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

        /*winrt::slim_lock_guard LockGuard(
            NanaGet::LocalInstance->InstanceLock());*/

        NanaGet::LocalInstance->RefreshInformation();

        winrt::hstring GlobalStatusText = winrt::hstring(Mile::FormatWideString(
            L"\x2193 %s/s \x2191 %s/s",
            NanaGet::ConvertByteSizeToString(
                NanaGet::LocalInstance->TotalDownloadSpeed()).data(),
            NanaGet::ConvertByteSizeToString(
                NanaGet::LocalInstance->TotalUploadSpeed()).data()));

        winrt::hstring CurrentSearchFilter = this->m_SearchFilter;

        std::vector<Aria2TaskInformation> Tasks;

        for (std::string const& Gid : NanaGet::LocalInstance->GetTaskList())
        {
            Tasks.emplace_back(NanaGet::LocalInstance->GetTaskInformation(Gid));
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
            this->GlobalStatusTextBlock().Text(GlobalStatusText);

            if (NeedFullRefresh)
            {
                this->TaskList().ItemsSource(this->m_Tasks);
            }
            else if(this->m_Tasks)
            {
                try
                {
                    for (NanaGet::TaskItem const& Task : this->m_Tasks)
                    {
                        Task.as<NanaGet::implementation::TaskItem>()->Notify();
                    }
                }
                catch (...)
                {

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
                "token:" + NanaGet::LocalInstance->ServerToken());

            //std::string ResponseJson = NanaGet::LocalInstance->SimpleJsonRpcCall(
            //    "aria2.getVersion", //"aria2.tellActive",
            //    Parameters);

            //std::string ResponseJson = NanaGet::LocalInstance->SimpleJsonRpcCall(
            //    L"system.listMethods",
            //    Parameters);

            std::string ResponseJson = NanaGet::LocalInstance->SimpleJsonRpcCall(
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
            NanaGet::LocalInstance->ConsoleOutput().c_str(),
            L"NanaGet",
            MB_ICONINFORMATION);*/

        return 0;
    }
}
