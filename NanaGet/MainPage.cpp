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

    void  MainPage::NewTaskGridDownloadButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        using winrt::Windows::UI::Xaml::Visibility;

        this->NewTaskGrid().Visibility(Visibility::Collapsed);
    }

    void  MainPage::NewTaskGridCancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        using winrt::Windows::UI::Xaml::Visibility;

        this->NewTaskGrid().Visibility(Visibility::Collapsed);
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
