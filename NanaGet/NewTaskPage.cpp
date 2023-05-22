#include "pch.h"
#include "NewTaskPage.h"
#if __has_include("NewTaskPage.g.cpp")
#include "NewTaskPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaGet::implementation
{
    NewTaskPage::NewTaskPage(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle)
    {

    }

    /*void NewTaskPage::DownloadSourceBrowseButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void NewTaskPage::SaveFolderBrowseButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }*/

    void NewTaskPage::DownloadButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        /*try
        {
            this->m_Instance.AddTask(winrt::to_string(
                this->DownloadSourceTextBox().Text()));
        }
        catch (...)
        {

        }*/
    }

    void NewTaskPage::CancelButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::DestroyWindow(this->m_WindowHandle);
    }
}
