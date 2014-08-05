#define _WIN32_WINNT 0x0600

#include "stdio.h"
#include <vector>
#include "windows.h"
#include "winerror.h"
#include "Wia.h"
#include "comdef.h"
#pragma comment(lib, "wiaguid")

#include "Devices.h"

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

	std::vector< Device > devices = Devices::getDevices();



	Sleep(20000);
}