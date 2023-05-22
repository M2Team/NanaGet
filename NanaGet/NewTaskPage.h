#pragma once

#include "NewTaskPage.g.h"

namespace winrt::NanaGet::implementation
{
    using Windows::UI::Xaml::RoutedEventArgs;

    struct NewTaskPage : NewTaskPageT<NewTaskPage>
    {
        NewTaskPage() = default;

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
    };
}

namespace winrt::NanaGet::factory_implementation
{
    struct NewTaskPage : NewTaskPageT<NewTaskPage, implementation::NewTaskPage>
    {
    };
}
