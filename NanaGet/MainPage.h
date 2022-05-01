#pragma once

#include "MainPage.g.h"

namespace winrt::NanaGet::implementation
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;

    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        void NewTaskButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void SettingsButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void AboutButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

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
