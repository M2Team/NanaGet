#pragma once

#include "SettingsPage.g.h"

#include <Windows.h>

namespace winrt::NanaGet::implementation
{
    using Windows::UI::Xaml::RoutedEventArgs;

    struct SettingsPage : SettingsPageT<SettingsPage>
    {
    public:

        SettingsPage(
            _In_ HWND WindowHandle = nullptr);

        void CustomDownloadFolderBrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void ApplyButtonClick(
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
    struct SettingsPage :
        SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
