﻿#include "pch.h"
#include "TaskItem.h"
#include "TaskItem.g.cpp"
#include "TaskItemConverter.g.cpp"

#include <winrt/Windows.UI.Xaml.Media.h>

namespace
{
    winrt::hstring VFormatWindowsRuntimeString(
        _In_z_ _Printf_format_string_ wchar_t const* const Format,
        _In_z_ _Printf_format_string_ va_list ArgList)
    {
        // Check the argument list.
        if (Format)
        {
            // Get the length of the format result.
            size_t nLength =
                static_cast<size_t>(::_vscwprintf(Format, ArgList)) + 1;

            // Allocate for the format result.
            std::wstring Buffer(nLength + 1, L'\0');

            // Format the string.
            int nWritten = ::_vsnwprintf_s(
                &Buffer[0],
                Buffer.size(),
                nLength,
                Format,
                ArgList);

            if (nWritten > 0)
            {
                // If succeed, resize to fit and return result.
                Buffer.resize(nWritten);
                return winrt::hstring(Buffer);
            }
        }

        // If failed, return an empty string.
        return winrt::hstring();
    }

    winrt::hstring FormatWindowsRuntimeString(
        _In_z_ _Printf_format_string_ wchar_t const* const Format,
        ...)
    {
        va_list ArgList;
        va_start(ArgList, Format);
        winrt::hstring Result = ::VFormatWindowsRuntimeString(Format, ArgList);
        va_end(ArgList);
        return Result;
    }

    static winrt::hstring ConvertByteSizeToString(
        std::uint64_t ByteSize)
    {
        const wchar_t* Systems[] =
        {
            L"Byte",
            L"Bytes",
            L"KiB",
            L"MiB",
            L"GiB",
            L"TiB",
            L"PiB",
            L"EiB"
        };

        size_t nSystem = 0;
        double result = static_cast<double>(ByteSize);

        if (ByteSize > 1)
        {
            for (
                nSystem = 1;
                nSystem < sizeof(Systems) / sizeof(*Systems);
                ++nSystem)
            {
                if (1024.0 > result)
                    break;

                result /= 1024.0;
            }

            result = static_cast<uint64_t>(result * 100) / 100.0;
        }

        return ::FormatWindowsRuntimeString(
            L"%.2f %s",
            result,
            Systems[nSystem]);
    }

    static winrt::hstring ConvertSecondsToTimeString(
        std::uint64_t Seconds)
    {
        int Hour = static_cast<int>(Seconds / 3600);
        int Minute = static_cast<int>(Seconds / 60 % 60);
        int Second = static_cast<int>(Seconds % 60);

        return ::FormatWindowsRuntimeString(
            L"%d:%02d:%02d",
            Hour,
            Minute,
            Second);
    }
}

namespace winrt::NanaGet::implementation
{
    using Windows::UI::Xaml::Visibility;

    hstring TaskItem::Gid()
    {
        return hstring();
    }

    hstring TaskItem::Name()
    {
        return L"QQ9.0.9.exe";
    }

    Uri TaskItem::Source()
    {
        return nullptr;
    }

    hstring TaskItem::Path()
    {
        return hstring();
    }

    TaskStatus TaskItem::Status()
    {
        return TaskStatus::Active;
    }

    std::uint64_t TaskItem::DownloadSpeed()
    {
        return 20480;
    }

    std::uint64_t TaskItem::UploadSpeed()
    {
        return 10240;
    }

    std::uint64_t TaskItem::BytesReceived()
    {
        return 768000;
    }

    std::uint64_t TaskItem::TotalBytesToReceive()
    {
        return 1048576;
    }

    std::uint64_t TaskItem::RemainTime()
    {
        return 3000;
    }

    hstring TaskItem::StatusText()
    {
        if (TaskStatus::Active == this->Status() ||
            TaskStatus::Paused == this->Status())
        {
            return ::FormatWindowsRuntimeString(
                L"%s/s \x2193 %s/s \x2191 %s / %s %s",
                ::ConvertByteSizeToString(this->DownloadSpeed()).data(),
                ::ConvertByteSizeToString(this->UploadSpeed()).data(),
                ::ConvertByteSizeToString(this->BytesReceived()).data(),
                ::ConvertByteSizeToString(this->TotalBytesToReceive()).data(),
                ::ConvertSecondsToTimeString(this->RemainTime()).data());
        }

        return ::ConvertByteSizeToString(this->TotalBytesToReceive());
    }

    IInspectable ConvertUInt64ToDouble(
        IInspectable const& value)
    {
        return box_value(
            static_cast<double>(unbox_value_or<uint64_t>(value, 0)));
    }

    IInspectable ConvertTaskStatusToProgressBarForegroundColor(
        IInspectable const& value)
    {
        using Windows::UI::Colors;
        using Windows::UI::Xaml::Media::SolidColorBrush;

        SolidColorBrush ProgressBarForegroundColorBrush;

        ProgressBarForegroundColorBrush.Opacity(0.8);

        switch (unbox_value_or<TaskStatus>(
            value, TaskStatus::Error))
        {
        case TaskStatus::Active:
            ProgressBarForegroundColorBrush.Color(Colors::Green());
            break;
        case TaskStatus::Paused:
            ProgressBarForegroundColorBrush.Color(Colors::Orange());
            break;
        case TaskStatus::Error:
            ProgressBarForegroundColorBrush.Color(Colors::Red());
            break;
        default:
            ProgressBarForegroundColorBrush.Color(Colors::Transparent());
            break;
        }

        return ProgressBarForegroundColorBrush;
    }

    IInspectable ConvertTaskStatusToRetryButtonVisible(
        IInspectable const& value)
    {
        Visibility RetryButtonVisible = Visibility::Collapsed;

        switch (unbox_value_or<TaskStatus>(
            value, TaskStatus::Error))
        {
        case TaskStatus::Removed:
        case TaskStatus::Error:
            RetryButtonVisible = Visibility::Visible;
            break;
        default:
            break;
        }

        return box_value(RetryButtonVisible);
    }

    IInspectable ConvertTaskStatusToResumeButtonVisible(
        IInspectable const& value)
    {
        TaskStatus Status = unbox_value_or<TaskStatus>(
            value,
            TaskStatus::Error);

        return box_value(
            (TaskStatus::Paused == Status)
            ? Visibility::Visible
            : Visibility::Collapsed);
    }

    IInspectable ConvertTaskStatusToPauseButtonVisible(
        IInspectable const& value)
    {
        TaskStatus Status = unbox_value_or<TaskStatus>(
            value,
            TaskStatus::Error);

        return box_value(
            (TaskStatus::Active == Status)
            ? Visibility::Visible
            : Visibility::Collapsed);
    }

    IInspectable TaskItemConverter::Convert(
        IInspectable const& value,
        TypeName const& targetType,
        IInspectable const& parameter,
        hstring const& language) const
    {
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(language);

        hstring ParameterObject =
            unbox_value_or<hstring>(parameter, L"");
        const wchar_t* Parameter = ParameterObject.data();

        if (0 == std::wcscmp(Parameter, L"UInt64ToDouble"))
        {
            return ConvertUInt64ToDouble(value);
        }

        if (0 == wcscmp(
            Parameter,
            L"TaskStatusToProgressBarForegroundColor"))
        {
            return ConvertTaskStatusToProgressBarForegroundColor(value);
        }

        if (0 == wcscmp(
            Parameter,
            L"TaskStatusToRetryButtonVisible"))
        {
            return ConvertTaskStatusToRetryButtonVisible(value);
        }

        if (0 == wcscmp(
            Parameter,
            L"TaskStatusToResumeButtonVisible"))
        {
            return ConvertTaskStatusToResumeButtonVisible(value);
        }

        if (0 == wcscmp(
            Parameter,
            L"TaskStatusToPauseButtonVisible"))
        {
            return ConvertTaskStatusToPauseButtonVisible(value);
        }

        throw_hresult(E_NOTIMPL);
    }

    IInspectable TaskItemConverter::ConvertBack(
        IInspectable const& value,
        TypeName const& targetType,
        IInspectable const& parameter,
        hstring const& language) const
    {
        UNREFERENCED_PARAMETER(value);
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(language);

        throw_hresult(E_NOTIMPL);
    }
}
