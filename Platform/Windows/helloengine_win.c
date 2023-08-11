#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM WParam,
                            LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
    // window handler
    HWND hWnd;
    // window class struct
    WNDCLASSEX wc;
    
    //clear out window class
    ZeroMemory(&wc,sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClass1");
    RegisterClassEx(&wc);
    //create window
    hWnd = CreateWindowEx(0,_T("WindowClass1"),_T("Hello Engine!"),WS_OVERLAPPEDWINDOW,300,300,500,400,NULL,NULL,hInstance,NULL);

    //display 
    ShowWindow(hWnd,nCmdShow);

    // windows event message
    MSG msg;

    while(GetMessage(&msg,NULL,0,0)){
        //translate key msg into right formats
        TranslateMessage(&msg);

        //send msg to windowproc function
        DispatchMessage(&msg);

    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam){
    switch (message)
    {
        case WM_DESTROY:{
            //close application
            PostQuitMessage(0);            
            return 0;        
        } break;
    }
    return DefWindowProc(hWnd,message,wParam,lParam);
}