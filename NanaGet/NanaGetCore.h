/*
 * PROJECT:   NanaGet
 * FILE:      NanaGetCore.h
 * PURPOSE:   Definition for NanaGet Core Implementation
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#ifndef NANA_GET_CORE
#define NANA_GET_CORE

#if (defined(__cplusplus) && __cplusplus >= 201703L)
#elif (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#else
#error "[Mile] You should use a C++ compiler with the C++17 standard."
#endif

#include <winrt/Windows.Foundation.h>

namespace NanaGet
{
    winrt::hstring GetApplicationFolderPath();

    winrt::hstring GetSettingsFolderPath();

    winrt::hstring CreateGuidString();

    std::uint16_t PickUnusedTcpPort();
}

#endif // !NANA_GET_CORE
