﻿#include "pch.h"

#include "App.h"

#include <Mile.Xaml.h>

namespace winrt::NanaGet::implementation
{
    App::App()
    {
        ::MileXamlGlobalInitialize();

        using Windows::Foundation::IInspectable;
        using Windows::UI::Xaml::UnhandledExceptionEventArgs;

        this->UnhandledException([](
            IInspectable const& sender,
            UnhandledExceptionEventArgs const& e)
        {
            UNREFERENCED_PARAMETER(sender);

            e.Handled(true);       
        });
    }

    void App::Close()
    {
        Exit();
        ::MileXamlGlobalUninitialize();
    }
}
