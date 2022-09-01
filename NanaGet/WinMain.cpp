#include <Windows.h>

#include "pch.h"

#include "App.h"
#include "MainPage.h"

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>

#define _WTL_NO_AUTOMATIC_NAMESPACE
#include <atlapp.h>
#include <atlcrack.h>

#include "NanaGetResources.h"

#include <Mile.Windows.DwmHelpers.h>

namespace winrt
{
    using Windows::UI::Xaml::ElementTheme;
    using Windows::UI::Xaml::FocusState;
    using Windows::UI::Xaml::FrameworkElement;
    using Windows::UI::Xaml::UIElement;
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSourceTakeFocusRequestedEventArgs;
    using Windows::UI::Xaml::Media::VisualTreeHelper;
}

//namespace
//{
//    // {CCD5AA13-D01B-494F-A5E1-3E373234857B}
//    const GUID NotifyIconGuid =
//    {
//        0xccd5aa13,
//        0xd01b,
//        0x494f,
//        { 0xa5, 0xe1, 0x3e, 0x37, 0x32, 0x34, 0x85, 0x7b }
//    };
//
//    const UINT NotifyIconCallbackMessage = WM_APP + 0x100;
//}

namespace NanaGet
{
    class MainWindow : public ATL::CWindowImpl<MainWindow>
    {
    public:

        DECLARE_WND_CLASS(L"NanaGetMainWindow")

        BEGIN_MSG_MAP(MainWindow)
            MSG_WM_CREATE(OnCreate)
            //MESSAGE_HANDLER_EX(NotifyIconCallbackMessage, OnNotifyIcon)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_SIZE(OnSize)
            MSG_WM_DPICHANGED(OnDpiChanged)
            MSG_WM_MENUCHAR(OnMenuChar)
            MSG_WM_SETTINGCHANGE(OnSettingChange)
            MSG_WM_DESTROY(OnDestroy)           
        END_MSG_MAP()

        int OnCreate(
            LPCREATESTRUCT lpCreateStruct);

        /*LRESULT OnNotifyIcon(
            UINT uMsg,
            WPARAM wParam,
            LPARAM lParam);*/

        void OnSetFocus(
            ATL::CWindow wndOld);

        void OnSize(
            UINT nType,
            CSize size);

        void OnDpiChanged(
            UINT nDpiX,
            UINT nDpiY,
            PRECT pRect);

        LRESULT OnMenuChar(
            UINT nChar,
            UINT nFlags,
            WTL::CMenuHandle menu);

        void OnSettingChange(
            UINT uFlags,
            LPCTSTR lpszSection);

        void OnDestroy();

    private:

        WTL::CIcon m_ApplicationIcon;
        winrt::DesktopWindowXamlSource m_XamlSource;
        winrt::NanaGet::MainPage m_MainPage;
    };

    class XamlIslandHostMessageFilter : public WTL::CMessageFilter
    {
    public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
    };
}

BOOL NanaGet::XamlIslandHostMessageFilter::PreTranslateMessage(MSG* pMsg) {

    // Prevent XAML islands from capturing Alt-F4
    // https://github.com/microsoft/microsoft-ui-xaml/issues/2408
    if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
    {
        HWND hwndMain = ::GetAncestor(pMsg->hwnd, GA_ROOT);
        ::SendMessageW(hwndMain, pMsg->message, pMsg->wParam, pMsg->lParam);
        return TRUE;
    }
    return FALSE;
}

int NanaGet::MainWindow::OnCreate(
    LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);

    this->m_ApplicationIcon.LoadIconW(
        MAKEINTRESOURCE(IDI_NANAGET),
        256,
        256,
        LR_SHARED);

    this->SetIcon(this->m_ApplicationIcon, TRUE);
    this->SetIcon(this->m_ApplicationIcon, FALSE);

    this->m_MainPage = winrt::make<winrt::NanaGet::implementation::MainPage>();
    winrt::com_ptr<IDesktopWindowXamlSourceNative> XamlSourceNative =
        this->m_XamlSource.as<IDesktopWindowXamlSourceNative>();
    winrt::check_hresult(
        XamlSourceNative->AttachToWindow(this->m_hWnd));
    this->m_XamlSource.Content(this->m_MainPage);

    HWND XamlWindowHandle = nullptr;
    winrt::check_hresult(
        XamlSourceNative->get_WindowHandle(&XamlWindowHandle));

    // When focus is moving out from XAML island, move it back in again.
    this->m_XamlSource.TakeFocusRequested(
        [this](winrt::DesktopWindowXamlSource sender,
            winrt::DesktopWindowXamlSourceTakeFocusRequestedEventArgs args) {
            this->m_MainPage.Focus(winrt::FocusState::Programmatic);
        });

    // Focus on XAML Island host window for Acrylic brush support.
    ::SetFocus(XamlWindowHandle);

    ::MileDisableSystemBackdrop(this->m_hWnd);

    winrt::FrameworkElement Content =
        this->m_XamlSource.Content().try_as<winrt::FrameworkElement>();

    ::MileSetUseImmersiveDarkModeAttribute(
        this->m_hWnd,
        (Content.ActualTheme() == winrt::ElementTheme::Dark
            ? TRUE
            : FALSE));

    ::MileSetCaptionColorAttribute(
        this->m_hWnd,
        (Content.ActualTheme() == winrt::ElementTheme::Dark
            ? RGB(0, 0, 0)
            : RGB(255, 255, 255)));

    UINT DpiValue = ::GetDpiForWindow(this->m_hWnd);

    this->SetWindowPos(
        nullptr,
        0,
        0,
        ::MulDiv(720, DpiValue, USER_DEFAULT_SCREEN_DPI),
        ::MulDiv(540, DpiValue, USER_DEFAULT_SCREEN_DPI),
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    /*NOTIFYICONDATAW NotifyIconData = { 0 };
    NotifyIconData.cbSize = sizeof(NOTIFYICONDATAW);
    NotifyIconData.hWnd = this->m_hWnd;
    NotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    NotifyIconData.uFlags |= NIF_GUID | NIF_SHOWTIP;
    NotifyIconData.uCallbackMessage = NotifyIconCallbackMessage;
    NotifyIconData.hIcon = this->m_ApplicationIcon.m_hIcon;
    ::wcscpy_s(NotifyIconData.szTip, L"NanaGet");
    NotifyIconData.guidItem = NotifyIconGuid;
    winrt::check_bool(::Shell_NotifyIconW(NIM_DELETE, &NotifyIconData));
    winrt::check_bool(::Shell_NotifyIconW(NIM_ADD, &NotifyIconData));

    NotifyIconData.uVersion = NOTIFYICON_VERSION_4;
    winrt::check_bool(::Shell_NotifyIconW(NIM_SETVERSION, &NotifyIconData));*/
    
    return 0;
}

//LRESULT NanaGet::MainWindow::OnNotifyIcon(
//    UINT uMsg,
//    WPARAM wParam,
//    LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(uMsg);
//    UNREFERENCED_PARAMETER(wParam);
//
//    switch (LOWORD(lParam))
//    {
//    case NIN_SELECT:
//    {
//        this->ShowWindow(
//            ::IsWindowVisible(this->m_hWnd)
//            ? SW_HIDE
//            : SW_SHOW);
//
//        break;
//    }
//    case WM_CONTEXTMENU:
//    {
//        ::SetForegroundWindow(this->m_hWnd);
//
//        break;
//    }
//    default:
//        break;
//    }
//
//    return 0;
//}

void NanaGet::MainWindow::OnSetFocus(
    ATL::CWindow wndOld)
{
    UNREFERENCED_PARAMETER(wndOld);

    winrt::com_ptr<IDesktopWindowXamlSourceNative> XamlSourceNative =
        this->m_XamlSource.as<IDesktopWindowXamlSourceNative>();

    HWND XamlWindowHandle = nullptr;
    winrt::check_hresult(
        XamlSourceNative->get_WindowHandle(&XamlWindowHandle));

    ::SetFocus(XamlWindowHandle);
}

void NanaGet::MainWindow::OnSize(
    UINT nType,
    CSize size)
{
    UNREFERENCED_PARAMETER(nType);

    winrt::com_ptr<IDesktopWindowXamlSourceNative> XamlSourceNative =
        this->m_XamlSource.as<IDesktopWindowXamlSourceNative>();

    HWND XamlWindowHandle = nullptr;
    winrt::check_hresult(
        XamlSourceNative->get_WindowHandle(&XamlWindowHandle));
    ::SetWindowPos(
        XamlWindowHandle,
        nullptr,
        0,
        0,
        size.cx,
        size.cy,
        SWP_SHOWWINDOW);
}

void NanaGet::MainWindow::OnDpiChanged(
    UINT nDpiX,
    UINT nDpiY,
    PRECT pRect)
{
    UNREFERENCED_PARAMETER(nDpiX);
    UNREFERENCED_PARAMETER(nDpiY);

    this->SetWindowPos(
        nullptr,
        pRect,
        SWP_NOZORDER | SWP_NOACTIVATE);
}

LRESULT NanaGet::MainWindow::OnMenuChar(
    UINT nChar,
    UINT nFlags,
    WTL::CMenuHandle menu)
{
    UNREFERENCED_PARAMETER(nChar);
    UNREFERENCED_PARAMETER(nFlags);
    UNREFERENCED_PARAMETER(menu);

    // Reference: https://github.com/microsoft/terminal
    //            /blob/756fd444b1d443320cf2ed6887d4b65aa67a9a03
    //            /scratch/ScratchIslandApp
    //            /WindowExe/SampleIslandWindow.cpp#L155
    // Return this LRESULT here to prevent the app from making a bell
    // when alt+key is pressed. A menu is active and the user presses a
    // key that does not correspond to any mnemonic or accelerator key.

    return MAKELRESULT(0, MNC_CLOSE);
}

void NanaGet::MainWindow::OnSettingChange(
    UINT uFlags,
    LPCTSTR lpszSection)
{
    UNREFERENCED_PARAMETER(uFlags);

    if (lpszSection && 0 == std::wcscmp(
        lpszSection,
        L"ImmersiveColorSet"))
    {
        winrt::FrameworkElement Content =
            this->m_XamlSource.Content().try_as<winrt::FrameworkElement>();
        if (Content &&
            winrt::VisualTreeHelper::GetParent(Content))
        {
            Content.RequestedTheme(winrt::ElementTheme::Default);

            ::MileSetUseImmersiveDarkModeAttribute(
                this->m_hWnd,
                (Content.ActualTheme() == winrt::ElementTheme::Dark
                    ? TRUE
                    : FALSE));

            ::MileSetCaptionColorAttribute(
                this->m_hWnd,
                (Content.ActualTheme() == winrt::ElementTheme::Dark
                    ? RGB(0, 0, 0)
                    : RGB(255, 255, 255)));
        }
    }
}

void NanaGet::MainWindow::OnDestroy()
{
    /*NOTIFYICONDATAW NotifyIconData = { 0 };
    NotifyIconData.cbSize = sizeof(NOTIFYICONDATAW);
    NotifyIconData.uFlags = NIF_GUID;
    NotifyIconData.guidItem = NotifyIconGuid;
    ::Shell_NotifyIconW(NIM_DELETE, &NotifyIconData);*/

    this->m_XamlSource.Close();

    ::PostQuitMessage(0);
}

namespace
{
    WTL::CAppModule g_Module;
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

    WTL::CMessageLoop MessageLoop;
    NanaGet::XamlIslandHostMessageFilter MessageFilter;
    MessageLoop.AddMessageFilter(&MessageFilter);

    g_Module.Init(nullptr, hInstance);
    g_Module.AddMessageLoop(&MessageLoop);

    NanaGet::MainWindow Window;
    if (!Window.Create(
        nullptr,
        Window.rcDefault,
        L"NanaBox",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        WS_EX_CLIENTEDGE))
    {
        return -1;
    }
    Window.ShowWindow(nShowCmd);
    Window.UpdateWindow();

    int Result = MessageLoop.Run();

    g_Module.RemoveMessageLoop();
    g_Module.Term();

    app.Close();

    return Result;
}
