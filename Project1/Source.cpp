#define _WIN32_WINNT 0x0600

#include "stdio.h"
#include <vector>
#include "windows.h"
#include "winerror.h"
#include "Wia.h"
#include "comdef.h"
#pragma comment(lib, "wiaguid")
#include <iostream>

#include "DeviceList.h"

int main(){
	//IWiaDevMgr2 * wia_manager = NULL;
	//CreateWiaDeviceManager(&wia_manager);
	//HRESULT list ;
	//list = EnumerateWiaDevices(wia_manager);
	CoInitialize(nullptr);
	HRESULT hr2 = CoCreateInstance( CLSID_WiaDevMgr2, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr2, (void**)&Manager ); 
	_com_error err(hr2);
	LPCTSTR errMsg = err.ErrorMessage();
	printf("%s\n", errMsg);

	device_list devices = DeviceList::listAll();
	printf("found %i device(s):\n", devices.size());
	printf("%ws\n", devices[0].getLongName());


	printf("about to initiate transfer:\n");
	devices[0].transfer();
	printf("after transfer\n");

	getchar();
	//Sleep(20000);
}