#include "WindowsApplication.hpp"
#include <tchar.h>

using namespace My;



int My::WindowsApplication::Initialize()
{
    int result;
    result = BaseApplication::Initialize();
    if(result != 0)
        exit(result);
    
    //get hinstance of console program
    HINSTANCE hInstance = GetModuleHandle(NULL);

    //handle for window, filled by a function
    HWND hWnd;
    //struct holds information for the window class
    WNDCLASSEX wc;

    //clear out the window class for use
    //clear out window class
    ZeroMemory(&wc,sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // wc.hCursor = LoadCursor(nullptr,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("GameEngineFromZero");
    RegisterClassEx(&wc);
    //create window
    hWnd = CreateWindowEx(0,_T("GameEngineFromZero"),
                            m_Config.appName,
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            m_Config.screenWidth,
                            m_Config.screenHeight,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);
    
    //display the window on the screen
    ShowWindow(hWnd,SW_SHOW);

    m_hWnd = hWnd;

    return result;

}

void My::WindowsApplication::Finalize()
{
}

void My::WindowsApplication::Tick()
{
    MSG msg;

    //use peekmessage instead of getMessae here bcz we should not block the thread at anywhere except the engine execution driver module
    if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT My::WindowsApplication::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // sort through and find what code to run for the message given
    switch(message)
    {
	case WM_PAINT:
        // we will replace this part with Rendering Module
	    {
	    } break;

        // this message is read when the window is closed
    case WM_DESTROY:
        {
            // close the application entirely
            PostQuitMessage(0);
            BaseApplication::m_bQuit = true;
            return 0;
        }
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}
