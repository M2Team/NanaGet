#pragma once

#include "TaskItem.g.h"

#include <winrt\Windows.Foundation.h>

namespace winrt::NanaGet::implementation
{
    using NanaGet::TaskStatus;
    using Windows::Foundation::Uri;

    struct TaskItem : TaskItemT<TaskItem>
    {
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
    };
}
