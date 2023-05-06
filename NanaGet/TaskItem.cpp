#include "pch.h"
#include "TaskItem.h"
#include "TaskItem.g.cpp"
#include "TaskItemConverter.g.cpp"

#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt::NanaGet::implementation
{
    using Windows::UI::Xaml::Visibility;

    TaskItem::TaskItem(
        Aria2TaskInformation const& Information)
    {
        this->Update(Information);
    }

    void TaskItem::Notify()
    {
        this->RaisePropertyChanged(L"Name");
        this->RaisePropertyChanged(L"Status");
        this->RaisePropertyChanged(L"BytesReceived");
        this->RaisePropertyChanged(L"TotalBytesToReceive");
        this->RaisePropertyChanged(L"StatusText");
    }

    void TaskItem::Update(
        Aria2TaskInformation const& Information)
    {
        this->Gid.Value = winrt::to_hstring(Information.Gid);

        this->Name.Value = winrt::to_hstring(Information.FriendlyName);

        if (!Information.InfoHash.empty())
        {
            this->Source.Value = NanaGet::FormatWindowsRuntimeString(
                L"magnet:?xt=urn:btih:%s",
                winrt::to_hstring(Information.InfoHash).c_str());
        }
        else
        {
            hstring Result;

            std::set<hstring> Uris;
            for (Aria2FileInformation const& File : Information.Files)
            {
                for (Aria2UriInformation const& Item : File.Uris)
                {
                    Uris.emplace(winrt::to_hstring(Item.Uri));
                }
            }

            for (hstring const& Item : Uris)
            {
                Result = Result + Item + L"\r\n";
            }

            this->Source.Value = Result;
        }

        this->Path.Value = hstring();

        switch (Information.Status)
        {
        case Aria2TaskStatus::Active:
            this->Status.Value = TaskStatus::Active;
            break;
        case Aria2TaskStatus::Waiting:
            this->Status.Value = TaskStatus::Waiting;
            break;
        case Aria2TaskStatus::Paused:
            this->Status.Value = TaskStatus::Paused;
            break;
        case Aria2TaskStatus::Error:
            this->Status.Value = TaskStatus::Error;
            break;
        case Aria2TaskStatus::Complete:
            this->Status.Value = TaskStatus::Complete;
            break;
        case Aria2TaskStatus::Removed:
            this->Status.Value = TaskStatus::Removed;
            break;
        default:
            throw winrt::hresult_out_of_bounds();
        }

        this->DownloadSpeed.Value = Information.DownloadSpeed;

        this->UploadSpeed.Value = Information.UploadSpeed;

        this->BytesReceived.Value = Information.CompletedLength;

        this->TotalBytesToReceive.Value = Information.TotalLength;

        if (0 == this->DownloadSpeed.Value ||
            0 == this->TotalBytesToReceive.Value)
        {
            this->RemainTime.Value = static_cast<uint64_t>(-1);
        }
        else
        {
            std::uint64_t RemainBytesToReceive =
                this->TotalBytesToReceive.Value - this->BytesReceived.Value;
            this->RemainTime.Value =
                RemainBytesToReceive / this->DownloadSpeed.Value;
        }

        if (TaskStatus::Active == this->Status.Value ||
            TaskStatus::Paused == this->Status.Value)
        {
            this->StatusText.Value = NanaGet::FormatWindowsRuntimeString(
                L"%s %s / %s %s/s \x2193 %s/s \x2191",
                NanaGet::ConvertSecondsToTimeString(
                    this->RemainTime.Value).data(),
                NanaGet::ConvertByteSizeToString(
                    this->BytesReceived.Value).data(),
                NanaGet::ConvertByteSizeToString(
                    this->TotalBytesToReceive.Value).data(),
                NanaGet::ConvertByteSizeToString(
                    this->DownloadSpeed.Value).data(),
                NanaGet::ConvertByteSizeToString(
                    this->UploadSpeed.Value).data());
        }
        else
        {
            this->StatusText.Value = NanaGet::ConvertByteSizeToString(
                this->TotalBytesToReceive.Value);
        }
    }

    void TaskItem::RaisePropertyChanged(
        std::wstring_view const& PropertyName)
    {
        this->PropertyChanged(*this, PropertyChangedEventArgs(PropertyName));
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

    IInspectable ConvertTaskStatusToCancelButtonVisible(
        IInspectable const& value)
    {
        Visibility ButtonVisible = Visibility::Collapsed;

        switch (unbox_value_or<TaskStatus>(
            value, TaskStatus::Error))
        {
        case TaskStatus::Active:
        case TaskStatus::Waiting:
        case TaskStatus::Paused:
            ButtonVisible = Visibility::Visible;
            break;
        default:
            break;
        }

        return box_value(ButtonVisible);
    }

    IInspectable ConvertTaskStatusToRemoveButtonVisible(
        IInspectable const& value)
    {
        Visibility ButtonVisible = Visibility::Collapsed;

        switch (unbox_value_or<TaskStatus>(
            value, TaskStatus::Error))
        {
        case TaskStatus::Error:
        case TaskStatus::Complete:
        case TaskStatus::Removed:
            ButtonVisible = Visibility::Visible;
            break;
        default:
            break;
        }

        return box_value(ButtonVisible);
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

        if (0 == std::wcscmp(
            Parameter,
            L"TaskStatusToProgressBarForegroundColor"))
        {
            return ConvertTaskStatusToProgressBarForegroundColor(value);
        }

        if (0 == std::wcscmp(
            Parameter,
            L"TaskStatusToRetryButtonVisible"))
        {
            return ConvertTaskStatusToRetryButtonVisible(value);
        }

        if (0 == std::wcscmp(
            Parameter,
            L"TaskStatusToResumeButtonVisible"))
        {
            return ConvertTaskStatusToResumeButtonVisible(value);
        }

        if (0 == std::wcscmp(
            Parameter,
            L"TaskStatusToPauseButtonVisible"))
        {
            return ConvertTaskStatusToPauseButtonVisible(value);
        }

        if (0 == std::wcscmp(
            Parameter,
            L"TaskStatusToCancelButtonVisible"))
        {
            return ConvertTaskStatusToCancelButtonVisible(value);
        }

        if (0 == std::wcscmp(
            Parameter,
            L"TaskStatusToRemoveButtonVisible"))
        {
            return ConvertTaskStatusToRemoveButtonVisible(value);
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
