#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <Wia.h>
#pragma comment(lib, "wiaguid")
#include "stdafx.h"
#include "resource.h"


long __stdcall WindowProcedure( HWND window, unsigned int msg, WPARAM wp, LPARAM lp )
{
    switch(msg)
    {
        case WM_DESTROY:
            std::cout << "\ndestroying window\n" ;
            PostQuitMessage(0) ;
            return 0L ;
        case WM_LBUTTONDOWN:
			//BSTR device_id;
			//IWiaItem* item;
			//Manager->SelectDeviceDlg(window, StiDeviceTypeScanner, WIA_SELECT_DEVICE_NODEFAULT, &device_id,  &item);
			//printf("%ws", device_id);
            std::cout << "\nmouse left button down at (" << LOWORD(lp)
                      << ',' << HIWORD(lp) << ")\n" ;
            // fall thru
        default:
            //std::cout << '.' ;
            return DefWindowProc( window, msg, wp, lp ) ;
    }
}

HWND create_temp_window(){
	const char* const myclass = "myclass" ;
    WNDCLASSEX wndclass = { sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure,
                            0, 0, GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
                            LoadCursor(0,IDC_ARROW), HBRUSH(COLOR_WINDOW+1),
                            0, myclass, LoadIcon(0,IDI_APPLICATION) } ;
    if( RegisterClassEx(&wndclass) )
    {
        HWND window = CreateWindowEx( 0, myclass, "title",
                   0, CW_USEDEFAULT, CW_USEDEFAULT,
                   CW_USEDEFAULT, 10, 0, 0, GetModuleHandle(0), 0 ) ;
        if(window)
        {
            ShowWindow( window, SW_SHOWDEFAULT ) ;
            MSG msg ;
            while( GetMessage( &msg, 0, 0, 0 ) ){
				DispatchMessage(&msg);
			}
        }
		//ShowWindow(window, SW_HIDE);

		return window;

    }
}