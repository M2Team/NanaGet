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

        MainPage() = default;

        void InitializeComponent();

        winrt::hstring SearchFilter();

        void SearchFilter(
            winrt::hstring const& Value);

        ~MainPage();

        void NewTaskButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void StartAllButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void PauseAllButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void ClearListButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void OpenDownloadsFolderButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void SettingsButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void AboutButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskListContainerContentChanging(
            ListViewBase const& sender,
            ContainerContentChangingEventArgs const& e);

        void TaskItemRetryButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskItemResumeButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskItemPauseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskItemCopyLinkButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        /*void TaskItemOpenFolderButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);*/

        void TaskItemCancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskItemRemoveButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

    private:

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
