#pragma once

#include "AboutPage.g.h"

namespace winrt::NanaGet::implementation
{
    using Windows::UI::Xaml::RoutedEventArgs;

    struct AboutPage :
        AboutPageT<AboutPage>
    {
        AboutPage() = default;

        void InitializeComponent();

        void GitHubButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void CancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);
    };
}

namespace winrt::NanaGet::factory_implementation
{
    struct AboutPage :
        AboutPageT<AboutPage, implementation::AboutPage>
    {
    };
}
