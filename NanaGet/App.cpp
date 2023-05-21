#include "pch.h"

#include "App.h"

#include <Mile.Xaml.h>

#include <CommCtrl.h>
#pragma comment(lib,"comctl32.lib")

#include "NanaGetCore.h"

namespace NanaGet
{
    extern HWND MainWindowHandle;
}

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

            ::TaskDialog(
                NanaGet::MainWindowHandle,
                nullptr,
                L"NanaGet",
                e.Message().c_str(),
                nullptr,
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);

            e.Handled(true);       
        });
    }

    void App::Close()
    {
        Exit();
        ::MileXamlGlobalUninitialize();
    }
}
