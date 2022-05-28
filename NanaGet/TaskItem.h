﻿#pragma once

#include "TaskItem.g.h"
#include "TaskItemConverter.g.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt::NanaGet::implementation
{
    using NanaGet::TaskStatus;
    using Windows::Foundation::IInspectable;
    using Windows::Foundation::Uri;
    using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;   
    using Windows::UI::Xaml::Interop::TypeName;

    struct TaskItem : TaskItemT<TaskItem>
    {
    public:

        TaskItem() = default;

        hstring Gid();
        hstring Name();
        Uri Source();
        hstring Path();
        TaskStatus Status();
        std::uint64_t DownloadSpeed();
        std::uint64_t UploadSpeed();
        std::uint64_t BytesReceived();
        std::uint64_t TotalBytesToReceive();
        std::uint64_t RemainTime();

        hstring StatusText();

        event_token PropertyChanged(
            PropertyChangedEventHandler const& value);

        void PropertyChanged(
            event_token const& token);

    protected:

        void RaisePropertyChanged(
            std::wstring_view const& PropertyName);

    private:

        event<PropertyChangedEventHandler> m_PropertyChanged; 
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
