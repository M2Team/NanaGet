#pragma once

#include "MainPage.g.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt::NanaGet::implementation
{
    using Windows::Foundation::IInspectable;
    using Windows::System::Threading::ThreadPoolTimer;
    using Windows::UI::Xaml::RoutedEventArgs;
    using Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs;
    using Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs;
    using Windows::UI::Xaml::Controls::ListViewBase;

    struct MainPage : MainPageT<MainPage>
    {
    public:

        MainPage();

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

        void TaskManagerGridSettingsButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridAboutButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridSearchBoxQuerySubmitted(
            IInspectable const& sender,
            AutoSuggestBoxQuerySubmittedEventArgs const& e);

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

        void TaskManagerGridTaskItemOpenFolderButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridTaskItemCancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridTaskItemRemoveButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void NewTaskGridDownloadSourceBrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void NewTaskGridSaveFolderBrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void NewTaskGridDownloadButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void NewTaskGridCancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void SettingsGridCustomDownloadFolderBrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void SettingsGridApplyButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void SettingsGridCancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void AboutGridGitHubButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void AboutGridCancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

    private:

        ThreadPoolTimer m_RefreshTimer = nullptr;

        winrt::fire_and_forget RefreshTimerHandler(
            ThreadPoolTimer const& timer);

    };
}

namespace winrt::NanaGet::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
