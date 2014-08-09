#define _WIN32_WINNT 0x0600

#include "stdio.h"
#include "comdef.h"
#pragma comment(lib, "wiaguid")

#include "windows.h"
#include "winerror.h"
#include <Wia.h>
#include <wtypes.h>
#include "formats.h"
#include <vector>;

IWiaDevMgr2 * Manager = NULL;


class DeviceListEntry{
public:
	BSTR id;
	BSTR vendor;
	BSTR model;
	DeviceListEntry(IWiaPropertyStorage* property_storage){
		PROPSPEC propspec_array[3];
		PROPVARIANT propvar_array[3];
		propspec_array[0]=Formats::getProp(WIA_DIP_DEV_ID);
		propvar_array[0] =Formats::getVariant(VT_BSTR);
		propspec_array[1]=Formats::getProp(WIA_DIP_VEND_DESC);
		propvar_array[1] =Formats::getVariant(VT_BSTR);
		propspec_array[2]=Formats::getProp(WIA_DIP_DEV_DESC);
		propvar_array[2] =Formats::getVariant(VT_BSTR);
		property_storage->ReadMultiple(3, propspec_array, propvar_array);
		id = propvar_array[0].bstrVal;
		vendor = propvar_array[1].bstrVal;
		model = propvar_array[2].bstrVal;
	}
	
	BSTR getLongName(){
		_bstr_t modelL = model;
		_bstr_t vendorL = vendor;
		return modelL + " (" + vendorL + ")";
	}
};

typedef std::vector <DeviceListEntry> device_list;

class DeviceList{
public:
	static device_list listAll(){
		device_list ret;
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
					ret.push_back(DeviceListEntry(pWiaPropertyStorage));
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