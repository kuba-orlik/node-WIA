#define _WIN32_WINNT 0x0600

#include <vector>
#include "stdio.h"
#include "windows.h"
#include "winerror.h"
#include "Wia.h"
#include "comdef.h"

#pragma comment(lib, "wiaguid")

#include "Device.h"

IWiaDevMgr2 * Manager = NULL;

class Devices{
private:
	//static IWiaDevMgr2 *manager;

	static void CreateWiaDeviceManager( IWiaDevMgr2 **ppWiaDevMgr ){
		*ppWiaDevMgr = NULL;
		CoCreateInstance( CLSID_WiaDevMgr2, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr2, (void**)ppWiaDevMgr ); 
	}
public: 
	/*static void initialize(){
		CreateWiaDeviceManager(&manager);
	}*/
	static std::vector< Device > getDevices(){
		std::vector< Device> ret;
		IEnumWIA_DEV_INFO *pWiaEnumDevInfo = NULL;
		HRESULT hr = Manager->EnumDeviceInfo( WIA_DEVINFO_ENUM_LOCAL, &pWiaEnumDevInfo );
		if (SUCCEEDED(hr))
		{
			while (S_OK == hr)
			{
				IWiaPropertyStorage *pWiaPropertyStorage = NULL;
				hr = pWiaEnumDevInfo->Next( 1, &pWiaPropertyStorage, NULL );
				if (hr == S_OK)
				{
					Device t(pWiaPropertyStorage);
					ret.push_back(t);
					pWiaPropertyStorage->Release();
					pWiaPropertyStorage = NULL;
				}
			}
			pWiaEnumDevInfo->Release();
			pWiaEnumDevInfo = NULL;
		}
		return ret;
	}
};