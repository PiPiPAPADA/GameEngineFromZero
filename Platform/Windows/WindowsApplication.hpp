#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "BaseApplication.hpp"

namespace My {
    class WindowsApplication : public BaseApplication{
    public:
        WindowsApplication(GfxConfiguration& config)
            :BaseApplication(config) {};
        
        virtual int Initialize();
        virtual void Finalize();
        virtual void Tick();
        HWND GetMainWindow() const {return m_hWnd;};

    private:
        // windowproc function
        static LRESULT CALLBACK WindowProc(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam);

      

    protected:
        HWND m_hWnd;
    };
}