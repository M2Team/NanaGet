#pragma once

#include "AboutPage.g.h"

#include <Windows.h>

namespace winrt::NanaGet::implementation
{
    using Windows::UI::Xaml::RoutedEventArgs;

    struct AboutPage :
        AboutPageT<AboutPage>
    {
    public:

        AboutPage(
            _In_ HWND WindowHandle = nullptr);

        void InitializeComponent();

        void GitHubButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void CancelButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

    private:

        HWND m_WindowHandle;
    };
}

namespace winrt::NanaGet::factory_implementation
{
    struct AboutPage :
        AboutPageT<AboutPage, implementation::AboutPage>
    {
    };
}
