#pragma once

#include "MainPage.g.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

#include "NanaGetCore.h"

namespace winrt::NanaGet::implementation
{
    using Windows::Foundation::Collections::IObservableVector;
    using Windows::System::DispatcherQueue;
    using Windows::UI::Xaml::DispatcherTimer;
    using Windows::UI::Xaml::RoutedEventArgs;
    using Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs;
    using Windows::UI::Xaml::Controls::ListViewBase;

    struct MainPage : MainPageT<MainPage>
    {
    public:

        MainPage();

        winrt::hstring SearchFilter();

        void SearchFilter(
            winrt::hstring const& Value);

        ~MainPage();

        void TaskManagerGridNewTaskButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridStartAllButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridPauseAllButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridClearListButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridOpenDownloadsFolderButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        /*void TaskManagerGridSettingsButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);*/

        void TaskManagerGridAboutButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridTaskListContainerContentChanging(
            ListViewBase const& sender,
            ContainerContentChangingEventArgs const& e);

        void TaskManagerGridTaskItemRetryButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridTaskItemResumeButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridTaskItemPauseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridTaskItemCopyLinkButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        /*void TaskManagerGridTaskItemOpenFolderButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);*/

        void TaskManagerGridTaskItemCancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridTaskItemRemoveButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        /*void NewTaskGridDownloadSourceBrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void NewTaskGridSaveFolderBrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);*/

        void NewTaskGridDownloadButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void NewTaskGridCancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        /*void SettingsGridCustomDownloadFolderBrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void SettingsGridApplyButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void SettingsGridCancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);*/

        void AboutGridGitHubButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void AboutGridCancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

    private:

        NanaGet::LocalAria2Instance m_Instance;

        DispatcherQueue m_DispatcherQueue = nullptr;

        std::thread m_RefreshThread;
        volatile bool m_StopRefreshThread = false;

        std::set<winrt::hstring> m_Gids;
        IObservableVector<NanaGet::TaskItem> m_Tasks = nullptr;
        winrt::hstring m_SearchFilter;

        void RefreshThreadEntryPoint();

        void RefreshThreadHandler();

        int SimpleDemoEntry();

        NanaGet::TaskItem GetTaskItemFromEventSender(
            IInspectable const& sender);

    };
}

namespace winrt::NanaGet::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
