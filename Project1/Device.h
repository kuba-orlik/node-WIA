#include "formats.h"


class Device{
private:
	int id;

public:
	Device(IWiaPropertyStorage* property_storage){
		PROPSPEC propspec_array[16];
		PROPVARIANT propvar_array[16];
		propspec_array[0]=Formats::getProp(WIA_DIP_DEV_ID);
		propvar_array[0] =Formats::getVariant(VT_BSTR);

		propspec_array[1]=Formats::getProp(WIA_DIP_VEND_DESC);
		propvar_array[1] =Formats::getVariant(VT_BSTR);

		propspec_array[2]=Formats::getProp(WIA_DIP_DEV_DESC);
		propvar_array[2] =Formats::getVariant(VT_BSTR);

		propspec_array[3]=Formats::getProp(WIA_DIP_DEV_TYPE);
		propvar_array[3] =Formats::getVariant(VT_I4);

		propspec_array[4]=Formats::getProp(WIA_DIP_PORT_NAME);
		propvar_array[4] =Formats::getVariant(VT_BSTR);

		propspec_array[5]=Formats::getProp(WIA_DIP_DEV_NAME);
		propvar_array[5] =Formats::getVariant(VT_BSTR);

		propspec_array[6]=Formats::getProp(WIA_DIP_SERVER_NAME);
		propvar_array[6] =Formats::getVariant(VT_BSTR);

		propspec_array[7]=Formats::getProp(WIA_DIP_REMOTE_DEV_ID);
		propvar_array[7] =Formats::getVariant(VT_BSTR);

		propspec_array[8]=Formats::getProp(WIA_DIP_UI_CLSID);
		propvar_array[8] =Formats::getVariant(VT_BSTR);

		propspec_array[9]=Formats::getProp(WIA_DIP_HW_CONFIG);
		propvar_array[9] =Formats::getVariant(VT_I4);

		propspec_array[10]=Formats::getProp(WIA_DIP_BAUDRATE);
		propvar_array[10] =Formats::getVariant(VT_BSTR);

		propspec_array[11]=Formats::getProp(WIA_DIP_STI_GEN_CAPABILITIES);
		propvar_array[11] =Formats::getVariant(VT_I4);

		propspec_array[12]=Formats::getProp(WIA_DIP_WIA_VERSION);
		propvar_array[12] =Formats::getVariant(VT_BSTR);

		propspec_array[13]=Formats::getProp(WIA_DIP_DRIVER_VERSION);
		propvar_array[13] =Formats::getVariant(VT_BSTR);

		propspec_array[14]=Formats::getProp(WIA_DIP_PNP_ID);
		propvar_array[14] =Formats::getVariant(VT_BSTR);

		propspec_array[15]=Formats::getProp(WIA_DIP_STI_DRIVER_VERSION);
		propvar_array[15] =Formats::getVariant(VT_BSTR);

		property_storage->ReadMultiple(16, propspec_array, propvar_array);
		int i=2;
		for(i=0; i<=15; i++){
			if(propvar_array[i].vt==VT_BSTR){
				printf("%ws\n", propvar_array[i].bstrVal);
			};
		}
				
	}

};
