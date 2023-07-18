#pragma once

#include "NewTaskPage.g.h"

#include <Windows.h>

namespace winrt::NanaGet::implementation
{
    using Windows::UI::Xaml::RoutedEventArgs;

    struct NewTaskPage : NewTaskPageT<NewTaskPage>
    {
    public:

        NewTaskPage(
            _In_ HWND WindowHandle = nullptr);

        /*void DownloadSourceBrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);

        void SaveFolderBrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);*/

        void DownloadButtonClick(
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
    struct NewTaskPage : NewTaskPageT<NewTaskPage, implementation::NewTaskPage>
    {
    };
}
