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
            Aria2TaskInformation const& Information);

        void Update(
            Aria2TaskInformation const& Information);

        hstring Gid();
        hstring Name();
        hstring Source();
        hstring Path();
        TaskStatus Status();
        std::uint64_t DownloadSpeed();
        std::uint64_t UploadSpeed();
        std::uint64_t BytesReceived();
        std::uint64_t TotalBytesToReceive();
        std::uint64_t RemainTime();

        hstring StatusText();

        Mile::WinRT::Event<PropertyChangedEventHandler> PropertyChanged;

    protected:

        void RaisePropertyChanged(
            std::wstring_view const& PropertyName);

    private:

        Aria2TaskInformation m_Information;
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
