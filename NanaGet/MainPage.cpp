#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "TaskItem.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaGet::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();

        using Windows::Foundation::Collections::IObservableVector;

        std::vector<NanaGet::TaskItem> contacts;
        contacts.push_back(winrt::make<NanaGet::implementation::TaskItem>());
        IObservableVector<NanaGet::TaskItem> suck =
            winrt::single_threaded_observable_vector(std::move(contacts));
        this->TaskManagerGridTaskList().ItemsSource(suck);
    }

    void MainPage::TaskManagerGridNewTaskButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        using winrt::Windows::UI::Xaml::Visibility;

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

        using winrt::Windows::UI::Xaml::Visibility;

        this->SettingsGrid().Visibility(Visibility::Visible);
    }

    void MainPage::TaskManagerGridAboutButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        using winrt::Windows::UI::Xaml::Visibility;

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

        using winrt::Windows::UI::Xaml::Visibility;

        this->NewTaskGrid().Visibility(Visibility::Collapsed);
    }

    void MainPage::NewTaskGridCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        using winrt::Windows::UI::Xaml::Visibility;

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

        using winrt::Windows::UI::Xaml::Visibility;

        this->SettingsGrid().Visibility(Visibility::Collapsed);
    }

    void MainPage::SettingsGridCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        using winrt::Windows::UI::Xaml::Visibility;

        this->SettingsGrid().Visibility(Visibility::Collapsed);
    }

    void MainPage::AboutGridGitHubButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        using winrt::Windows::Foundation::Uri;
        using winrt::Windows::System::Launcher;

        Launcher::LaunchUriAsync(Uri(L"https://github.com/M2Team/NanaGet"));
    }

    void MainPage::AboutGridCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        using winrt::Windows::UI::Xaml::Visibility;

        this->AboutGrid().Visibility(Visibility::Collapsed);
    }
}
