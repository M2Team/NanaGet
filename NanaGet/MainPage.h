#pragma once

#include "MainPage.g.h"

#include <winrt\Windows.UI.Xaml.Controls.h>

namespace winrt::NanaGet::implementation
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;
    using Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs;
    using Windows::UI::Xaml::Controls::ListViewBase;

    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        void TaskManagerGridNewTaskButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridSettingsButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridAboutButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void TaskManagerGridTaskListContainerContentChanging(
            ListViewBase const& sender,
            ContainerContentChangingEventArgs const& e);

        void NewTaskGridDownloadButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void NewTaskGridCancelButtonClick(
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
    };
}

namespace winrt::NanaGet::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
