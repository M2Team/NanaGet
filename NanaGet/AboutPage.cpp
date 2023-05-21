#include "pch.h"
#include "AboutPage.h"
#if __has_include("AboutPage.g.cpp")
#include "AboutPage.g.cpp"
#endif

#include <Windows.h>

#include <Mile.Project.Version.h>

#include <winrt/Windows.UI.Xaml.Documents.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaGet::implementation
{
    void AboutPage::InitializeComponent()
    {
        AboutPageT::InitializeComponent();
        this->Version().Text(winrt::hstring(
            L"NanaGet " MILE_PROJECT_VERSION_STRING
            L" (" MILE_PROJECT_DOT_VERSION_STRING L")"));
    }

    void AboutPage::GitHubButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        SHELLEXECUTEINFOW ExecInfo = { 0 };
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile = L"https://github.com/M2Team/NanaGet";
        ExecInfo.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&ExecInfo);
    }

    void AboutPage::CancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }
}
