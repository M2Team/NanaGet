#pragma once

#include "TaskItem.g.h"
#include "TaskItemConverter.g.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include "NanaGetCore.h"

namespace winrt::NanaGet::implementation
{
    using NanaGet::TaskStatus;
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;   
    using Windows::UI::Xaml::Interop::TypeName;

    struct TaskItem : TaskItemT<TaskItem>
    {
    public:

        TaskItem(
            Aria2::DownloadInformation const& Information);

        void Notify();

        void Update(
            Aria2::DownloadInformation const& Information);

        Mile::WinRT::Property<hstring> Gid;
        Mile::WinRT::Property<hstring> Name;
        Mile::WinRT::Property<hstring> Source;
        Mile::WinRT::Property<hstring> Path;
        Mile::WinRT::Property<TaskStatus> Status;
        Mile::WinRT::Property<std::uint64_t> DownloadSpeed;
        Mile::WinRT::Property<std::uint64_t> UploadSpeed;
        Mile::WinRT::Property<std::uint64_t> BytesReceived;
        Mile::WinRT::Property<std::uint64_t> TotalBytesToReceive;
        Mile::WinRT::Property<std::uint64_t> RemainTime;

        Mile::WinRT::Property<hstring> StatusText;

        Mile::WinRT::Event<PropertyChangedEventHandler> PropertyChanged;

    protected:

        void RaisePropertyChanged(
            std::wstring_view const& PropertyName);
    };

    struct TaskItemConverter :
        TaskItemConverterT<TaskItemConverter>
    {
        TaskItemConverter() = default;

        IInspectable Convert(
            IInspectable const& value,
            TypeName const& targetType,
            IInspectable const& parameter,
            hstring const& language) const;

        IInspectable ConvertBack(
            IInspectable const& value,
            TypeName const& targetType,
            IInspectable const& parameter,
            hstring const& language) const;
    };
}

namespace winrt::NanaGet::factory_implementation
{
    struct TaskItemConverter : TaskItemConverterT<
        TaskItemConverter,
        implementation::TaskItemConverter>
    {
    };
}
