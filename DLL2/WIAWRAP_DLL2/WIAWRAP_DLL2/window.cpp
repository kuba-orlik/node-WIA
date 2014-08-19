#include "window.h"

HWND FindMyTopMostWindow(){
    DWORD dwProcID = GetCurrentProcessId();
    HWND hWnd = GetTopWindow(GetDesktopWindow());
    while(hWnd)
    {
        DWORD dwWndProcID = 0;
        GetWindowThreadProcessId(hWnd, &dwWndProcID);
        if(dwWndProcID == dwProcID)
            return hWnd;            
        hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
    }
    return NULL;
 }