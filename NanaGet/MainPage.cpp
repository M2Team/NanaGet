#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "TaskItem.h"

#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>

namespace winrt::NanaGet::implementation
{
    using Windows::UI::Xaml::Visibility;

    MainPage::MainPage()
    {
        this->InitializeComponent();

        this->m_RefreshTimer = ThreadPoolTimer::CreatePeriodicTimer(
            { this, &MainPage::RefreshTimerHandler },
            std::chrono::milliseconds(500));
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

        /*using Windows::Foundation::Collections::IObservableVector;

        std::vector<NanaGet::TaskItem> contacts;
        contacts.push_back(winrt::make<NanaGet::implementation::TaskItem>());
        IObservableVector<NanaGet::TaskItem> suck =
            winrt::single_threaded_observable_vector(std::move(contacts));
        this->TaskManagerGridTaskList().ItemsSource(suck);*/
    }
}
