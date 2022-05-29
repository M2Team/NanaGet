#include "pch.h"

#include "App.h"
#include "App.g.cpp"

namespace winrt::NanaGet::implementation
{
    App::App()
    {
        this->Initialize();

        this->AddRef();
        m_inner.as<::IUnknown>()->Release();
    }

    App::~App()
    {
        this->Close();
    }
}
