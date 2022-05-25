#include "pch.h"
#include "TaskItem.h"
#include "TaskItem.g.cpp"

namespace winrt::NanaGet::implementation
{
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
        return 0;
    }

    std::uint64_t TaskItem::UploadSpeed()
    {
        return 0;
    }

    std::uint64_t TaskItem::BytesReceived()
    {
        return 0;
    }

    std::uint64_t TaskItem::TotalBytesToReceive()
    {
        return 0;
    }

    std::uint64_t TaskItem::RemainTime()
    {
        return 0;
    }
}
