#include <Windows.h>

#include "pch.h"

#include "App.h"
#include "MainPage.h"

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

namespace winrt
{
    using winrt::Windows::UI::Xaml::ElementTheme;
    using winrt::Windows::UI::Xaml::FrameworkElement;
    using winrt::Windows::UI::Xaml::UIElement;
    using winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;
    using winrt::Windows::UI::Xaml::Media::VisualTreeHelper;
}
namespace
{
    static LRESULT CALLBACK XamlIslandWindowCallback(
        _In_ HWND   hWnd,
        _In_ UINT   uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CREATE:
        {
            // Note: Return -1 directly because WM_DESTROY message will be sent
            // when destroy the window automatically. We free the resource when
            // processing the WM_DESTROY message of this window.

            LPCREATESTRUCT CreateStruct =
                reinterpret_cast<LPCREATESTRUCT>(lParam);

            winrt::DesktopWindowXamlSource XamlSource;

            winrt::com_ptr<IDesktopWindowXamlSourceNative> XamlSourceNative =
                XamlSource.as<IDesktopWindowXamlSourceNative>();

            // Parent the DesktopWindowXamlSource object to current window
            if (FAILED(XamlSourceNative->AttachToWindow(hWnd)))
            {
                return -1;
            }

            winrt::UIElement Content = nullptr;
            winrt::copy_from_abi(Content, CreateStruct->lpCreateParams);
            XamlSource.Content(Content);

            HWND XamlWindowHandle = nullptr;
            if (FAILED(XamlSourceNative->get_WindowHandle(&XamlWindowHandle)))
            {
                return -1;
            }

            if (!::SetPropW(
                hWnd,
                L"XamlWindowSource",
                reinterpret_cast<HANDLE>(winrt::detach_abi(XamlSource))))
            {
                return -1;
            }

            if (!::SetPropW(
                hWnd,
                L"XamlWindowHandle",
                reinterpret_cast<HANDLE>(XamlWindowHandle)))
            {
                return -1;
            }

            return 0;
        }
        case WM_SETFOCUS:
        {
            HWND XamlWindowHandle = reinterpret_cast<HWND>(
                ::GetPropW(hWnd, L"XamlWindowHandle"));
            if (XamlWindowHandle)
            {
                ::SetFocus(XamlWindowHandle);
            }

            break;
        }
        case WM_SIZE:
        {
            HWND XamlWindowHandle = reinterpret_cast<HWND>(
                ::GetPropW(hWnd, L"XamlWindowHandle"));
            if (XamlWindowHandle)
            {
                ::SetWindowPos(
                    XamlWindowHandle,
                    nullptr,
                    0,
                    0,
                    LOWORD(lParam),
                    HIWORD(lParam),
                    SWP_SHOWWINDOW);
            }

            break;
        }
        case WM_DPICHANGED:
        {
            LPRECT NewWindowRectangle = reinterpret_cast<LPRECT>(lParam);

            ::SetWindowPos(
                hWnd,
                nullptr,
                NewWindowRectangle->left,
                NewWindowRectangle->top,
                NewWindowRectangle->right - NewWindowRectangle->left,
                NewWindowRectangle->bottom - NewWindowRectangle->top,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        case WM_MENUCHAR:
        {
            // Reference: https://github.com/microsoft/terminal
            //            /blob/756fd444b1d443320cf2ed6887d4b65aa67a9a03
            //            /scratch/ScratchIslandApp
            //            /WindowExe/SampleIslandWindow.cpp#L155
            // Return this LRESULT here to prevent the app from making a bell
            // when alt+key is pressed. A menu is active and the user presses a
            // key that does not correspond to any mnemonic or accelerator key.

            return MAKELRESULT(0, MNC_CLOSE);
        }
        case WM_SETTINGCHANGE:
        {
            winrt::DesktopWindowXamlSource XamlSource = nullptr;
            winrt::copy_from_abi(
                XamlSource,
                ::GetPropW(hWnd, L"XamlWindowSource"));
            if (XamlSource)
            {
                winrt::FrameworkElement Content =
                    XamlSource.Content().try_as<winrt::FrameworkElement>();
                if (Content &&
                    winrt::VisualTreeHelper::GetParent(Content))
                {
                    Content.RequestedTheme(winrt::ElementTheme::Default);
                }
            }
            break;
        }
        case WM_DESTROY:
        {
            winrt::DesktopWindowXamlSource XamlSource = nullptr;
            winrt::copy_from_abi(
                XamlSource,
                ::RemovePropW(hWnd, L"XamlWindowSource"));
            XamlSource.Close();

            ::RemovePropW(hWnd, L"XamlWindowHandle");

            ::PostQuitMessage(0);

            break;
        }
        default:
            return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }
}

int WINAPI StartXamlIslandHost(
    _In_ HINSTANCE InstanceHandle,
    _In_opt_ HICON WindowIconHandle,
    _In_ LPCWSTR WindowTitle,
    _In_ LPVOID XamlWindowContent,
    _In_ DWORD ShowWindowMode)
{
    WNDCLASSEXW WindowClass;
    WindowClass.cbSize = sizeof(WNDCLASSEXW);
    WindowClass.style = 0;
    WindowClass.lpfnWndProc = ::XamlIslandWindowCallback;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = InstanceHandle;
    WindowClass.hIcon = WindowIconHandle;
    WindowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
    WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    WindowClass.lpszMenuName = nullptr;
    WindowClass.lpszClassName = L"Mile.Xaml.IslandWindow";
    WindowClass.hIconSm = WindowIconHandle;
    if (!::RegisterClassExW(&WindowClass))
    {
        return -1;
    }

    HWND WindowHandle = ::CreateWindowExW(
        WS_EX_CLIENTEDGE,
        WindowClass.lpszClassName,
        WindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        CW_USEDEFAULT,
        0,
        nullptr,
        nullptr,
        InstanceHandle,
        XamlWindowContent);
    if (!WindowHandle)
    {
        return -1;
    }

    ::ShowWindow(WindowHandle, ShowWindowMode);
    ::UpdateWindow(WindowHandle);

    MSG Message;
    while (::GetMessageW(&Message, nullptr, 0, 0))
    {
        ::TranslateMessage(&Message);
        ::DispatchMessageW(&Message);
    }

    return static_cast<int>(Message.wParam);
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    winrt::init_apartment(winrt::apartment_type::single_threaded);

    winrt::NanaGet::App app =
        winrt::make<winrt::NanaGet::implementation::App>();

    winrt::NanaGet::MainPage XamlWindowContent =
        winrt::make<winrt::NanaGet::implementation::MainPage>();

    int Result = ::StartXamlIslandHost(
        hInstance,
        nullptr,
        L"NanaGetWindow",
        winrt::get_abi(XamlWindowContent),
        nShowCmd);

    app.Close();

    return Result;
}
