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
        : m_Information(Information)
    {

    }

    void TaskItem::Update(
        Aria2TaskInformation const& Information)
    {
        this->m_Information = Information;
        this->RaisePropertyChanged(L"Name");
        this->RaisePropertyChanged(L"Status");
        this->RaisePropertyChanged(L"BytesReceived");
        this->RaisePropertyChanged(L"TotalBytesToReceive");
        this->RaisePropertyChanged(L"StatusText");
    }

    hstring TaskItem::Gid()
    {
        return this->m_Information.Gid;
    }

    hstring TaskItem::Name()
    {
        if (!this->m_Information.BittorrentName.empty())
        {
            return this->m_Information.BittorrentName;
        }
        else if (!this->m_Information.Files[0].Path.empty())
        {
            return hstring(&std::wcsrchr(
                this->m_Information.Files[0].Path.c_str(), L'/')[1]);
        }
        else
        {
            return this->Gid();
        }
    }

    hstring TaskItem::Source()
    {
        if (!this->m_Information.InfoHash.empty())
        {
            return L"magnet:?xt=urn:btih:" + this->m_Information.InfoHash;
        }
        else
        {
            hstring Result;

            std::set<hstring> Uris;
            for (Aria2FileInformation const& File : this->m_Information.Files)
            {
                for (Aria2UriInformation const& Item : File.Uris)
                {
                    Uris.emplace(Item.Uri);
                }
            }

            for (hstring const& Item : Uris)
            {
                Result = Result + Item + L"\r\n";
            }

            return Result;
        }
    }

    hstring TaskItem::Path()
    {
        return hstring();
    }

    TaskStatus TaskItem::Status()
    {
        TaskStatus Result;

        switch (this->m_Information.Status)
        {
        case Aria2TaskStatus::Active:
            Result = TaskStatus::Active;
            break;
        case Aria2TaskStatus::Waiting:
            Result = TaskStatus::Waiting;
            break;
        case Aria2TaskStatus::Paused:
            Result = TaskStatus::Paused;
            break;
        case Aria2TaskStatus::Error:
            Result = TaskStatus::Error;
            break;
        case Aria2TaskStatus::Complete:
            Result = TaskStatus::Complete;
            break;
        case Aria2TaskStatus::Removed:
            Result = TaskStatus::Removed;
            break;
        default:
            throw winrt::hresult_out_of_bounds();
        }

        return Result;
    }

    std::uint64_t TaskItem::DownloadSpeed()
    {
        return this->m_Information.DownloadSpeed;
    }

    std::uint64_t TaskItem::UploadSpeed()
    {
        return this->m_Information.UploadSpeed;
    }

    std::uint64_t TaskItem::BytesReceived()
    {
        return this->m_Information.CompletedLength;
    }

    std::uint64_t TaskItem::TotalBytesToReceive()
    {
        return this->m_Information.TotalLength;
    }

    std::uint64_t TaskItem::RemainTime()
    {
        if (0 == this->DownloadSpeed() ||
            0 == this->TotalBytesToReceive())
        {
            return static_cast<uint64_t>(-1);
        }
        else
        {
            std::uint64_t RemainBytesToReceive =
                this->TotalBytesToReceive() - this->BytesReceived();
            return (RemainBytesToReceive / this->DownloadSpeed());
        }
    }

    hstring TaskItem::StatusText()
    {
        if (TaskStatus::Active == this->Status() ||
            TaskStatus::Paused == this->Status())
        {
            return NanaGet::FormatWindowsRuntimeString(
                L"%s/s \x2193 %s/s \x2191 %s / %s %s",
                NanaGet::ConvertByteSizeToString(
                    this->DownloadSpeed()).data(),
                NanaGet::ConvertByteSizeToString(
                    this->UploadSpeed()).data(),
                NanaGet::ConvertByteSizeToString(
                    this->BytesReceived()).data(),
                NanaGet::ConvertByteSizeToString(
                    this->TotalBytesToReceive()).data(),
                NanaGet::ConvertSecondsToTimeString(
                    this->RemainTime()).data());
        }

        return NanaGet::ConvertByteSizeToString(
            this->TotalBytesToReceive());
    }

    event_token TaskItem::PropertyChanged(
        PropertyChangedEventHandler const& value)
    {
        return this->m_PropertyChanged.add(value);
    }

    void TaskItem::PropertyChanged(
        event_token const& token)
    {
        this->m_PropertyChanged.remove(token);
    }

    void TaskItem::RaisePropertyChanged(
        std::wstring_view const& PropertyName)
    {
        this->m_PropertyChanged(
            *this, PropertyChangedEventArgs(PropertyName));
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
