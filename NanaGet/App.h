#pragma once
#include "App.xaml.g.h"

namespace winrt::NanaGet::implementation
{
    class App : public AppT<App>
    {
    public:
        App();
        void Close();
    };
}
