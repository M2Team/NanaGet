﻿/*
 * PROJECT:   NanaGet
 * FILE:      NanaGetCore.cpp
 * PURPOSE:   Implementation for NanaGet Core Implementation
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "NanaGetCore.h"

#include <Windows.h>
#include <objbase.h>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <winrt/Windows.Storage.h>

namespace winrt
{
    using Windows::Storage::ApplicationData;
}

winrt::hstring NanaGet::GetApplicationFolderPath()
{
    static winrt::hstring CachedResult = ([]() -> winrt::hstring
    {
        // 32767 is the maximum path length without the terminating null
        // character.
        const DWORD BufferSize = 32767;
        wchar_t Buffer[BufferSize];
        ::GetModuleFileNameW(nullptr, Buffer, BufferSize);
        std::wcsrchr(Buffer, L'\\')[0] = L'\0';
        return Buffer;
    }());

    return CachedResult;
}

winrt::hstring NanaGet::GetSettingsFolderPath()
{
    static winrt::hstring CachedResult = ([]() -> winrt::hstring
    {
        return winrt::ApplicationData::Current().LocalFolder().Path();
    }());

    return CachedResult;
}

winrt::hstring NanaGet::CreateGuidString()
{
    GUID Result;
    winrt::check_hresult(::CoCreateGuid(&Result));
    return winrt::to_hstring(Result);
}

std::uint16_t NanaGet::PickUnusedTcpPort()
{
    std::uint16_t Result = 0;

    WSADATA WSAData;
    int Status = ::WSAStartup(
        MAKEWORD(2, 2),
        &WSAData);
    if (ERROR_SUCCESS == Status)
    {
        SOCKET ListenSocket = ::socket(
            AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP);
        if (INVALID_SOCKET != ListenSocket)
        {
            sockaddr_in Service;
            Service.sin_family = AF_INET;
            Service.sin_addr.s_addr = INADDR_ANY;
            Service.sin_port = ::htons(0);
            Status = ::bind(
                ListenSocket,
                reinterpret_cast<LPSOCKADDR>(&Service),
                sizeof(Service));
            if (ERROR_SUCCESS == Status)
            {
                int NameLength = sizeof(Service);
                Status = ::getsockname(
                    ListenSocket,
                    reinterpret_cast<LPSOCKADDR>(&Service),
                    &NameLength);
                if (ERROR_SUCCESS == Status)
                {
                    Result = ::ntohs(Service.sin_port);
                }
            }
            ::closesocket(ListenSocket);
        }

        ::WSACleanup();
    }

    return Result;
}
