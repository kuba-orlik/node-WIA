#define _WIN32_WINNT 0x0600

#include "stdio.h"
#include <vector>
#include "windows.h"
#include "winerror.h"
#include "Wia.h"
#include "comdef.h"
#pragma comment(lib, "wiaguid")
#include <iostream>

#include "WiaWrap.h"


IWiaDevMgr* Manager;

#include "temp_window.h"

int main(){
	Manager = NULL;
	CoInitialize(nullptr);
	CoCreateInstance( CLSID_WiaDevMgr, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr, (void**)&Manager ); 
	ULONG number = 0;
	WiaWrap::WiaGetNumDevices(Manager, &number);
	printf("%i\n", number);
	//HWND temp_window = create_temp_window();
	HWND console_window = GetConsoleWindow();
	printf("%i\n", console_window);
	BSTR device_id;
	IWiaItem* item;
	Manager->SelectDeviceDlg(console_window, StiDeviceTypeScanner, WIA_SELECT_DEVICE_NODEFAULT, &device_id,  &item);
	printf("after dialog");
	getchar();
	return 0;
}