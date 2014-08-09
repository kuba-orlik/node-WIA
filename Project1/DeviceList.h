#define _WIN32_WINNT 0x0600

#include "comdef.h"
#include "windows.h"
#include "winerror.h"
#include <wtypes.h>
#include "formats.h"
#include <vector>
#include <Wia.h>
#include "DeviceObject.h"

#pragma comment(lib, "wiaguid")

IWiaDevMgr2* Manager;


class DeviceListEntry{

private:
	void getAttributes(IWiaPropertyStorage* property_storage){
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
	HRESULT getDeviceObject(){
		IWiaItem2* wia_item;
		HRESULT hr = Manager->CreateDevice(0, id, &wia_item);
		device_object = DeviceObject(wia_item);
        return hr;
	}

public:
	BSTR id;
	BSTR vendor;
	BSTR model;
	DeviceObject device_object;
	DeviceListEntry(IWiaPropertyStorage* property_storage){
		getAttributes(property_storage);
		getDeviceObject();
	}
	
	BSTR getLongName(){
		_bstr_t modelL = model;
		_bstr_t vendorL = vendor;
		return modelL + " (" + vendorL + ")";
	}

	void transfer(){
		device_object.transfer();
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