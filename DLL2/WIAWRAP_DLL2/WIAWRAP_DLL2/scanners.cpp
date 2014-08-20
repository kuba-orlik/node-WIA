#include "scanners.h"

#include <comdef.h>

IWiaDevMgr* Manager;

HINSTANCE g_hInstance = 0;

void init_wia_manager(){
	Manager = NULL;
	CoCreateInstance( CLSID_WiaDevMgr, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr, (void**)&Manager ); 
}

class Formats{
public:
	static PROPSPEC getProp(int id){
		PROPSPEC p;
		p.ulKind = 1;
		p.propid = id;
		return p;
	}

	static PROPVARIANT getVariant(VARTYPE type){
		PROPVARIANT v;
		v.vt = type;
		return v;
	}

};


std::string bstr_to_str(BSTR source){
	//source = L"lol2inside";
	_bstr_t wrapped_bstr = _bstr_t(source);
	int length = wrapped_bstr.length();
	char* char_array = new char[length];
	strcpy_s(char_array, length+1, wrapped_bstr);
	return char_array;
}

const char* getDeviceName(IWiaItem* p_wia_item){
	IEnumWIA_DEV_INFO *pWiaEnumDevInfo = NULL;
	Manager->EnumDeviceInfo( WIA_DEVINFO_ENUM_LOCAL, &pWiaEnumDevInfo );
	IWiaPropertyStorage* property_storage;
	pWiaEnumDevInfo->Next( 1, &property_storage, NULL );	
	PROPSPEC propspec_array[3];
	PROPVARIANT propvar_array[3];
	propspec_array[0]=Formats::getProp(WIA_DIP_DEV_ID);
	propvar_array[0] =Formats::getVariant(VT_BSTR);
	propspec_array[1]=Formats::getProp(WIA_DIP_VEND_DESC);
	propvar_array[1] =Formats::getVariant(VT_BSTR);
	propspec_array[2]=Formats::getProp(WIA_DIP_DEV_DESC);
	propvar_array[2] =Formats::getVariant(VT_BSTR);
	property_storage->ReadMultiple(3, propspec_array, propvar_array);
	BSTR id = propvar_array[0].bstrVal;
	BSTR vendor = propvar_array[1].bstrVal;
	BSTR model = propvar_array[2].bstrVal;
	std::string s_vendor,s_model;
	s_vendor = bstr_to_str(vendor);
	s_model = bstr_to_str(model);
	std::string ret = s_model + " (" + s_vendor + ")";
	printf("long name inside: %s\n", ret.c_str());
	//printf("model_long: %s", ret.GetBuffer());
	//return ret.GetBuffer();
	//return "testowy string";
	char* ret_cp = new char[ret.length()];
	strcpy_s(ret_cp, ret.length()+1, ret.c_str());
	return ret_cp;
	//return "abc";
}



device_select_result select_device(bool force_display_dialog){
	CoInitialize(nullptr);
	init_wia_manager();
	HWND current_handle= FindMyTopMostWindow();
	//HWND current_handle= NULL;  // use when finding windows would be trublesome
	BSTR device_id;
	IWiaItem* item;
	long flags = 0;
	if(force_display_dialog){
		flags = WIA_SELECT_DEVICE_NODEFAULT;
	}
	Manager->SelectDeviceDlg(current_handle, StiDeviceTypeScanner, flags, &device_id,  &item);
	bool found = (device_id!=NULL);
	if(!found){
		device_select_result ret_fail = {false};
		return ret_fail;
	}
	const char* device_name = getDeviceName(item);
	printf("outside_device_name: %s\n", device_name);
	int device_id_length = _bstr_t(device_id).length();
	char* ch_device_id = new char[device_id_length];
	strcpy_s(ch_device_id, device_id_length+1, _bstr_t(device_id));
	printf("bstr=%ws\n", device_id);
	//BstrToStdString(device_id, s_device_id);
	printf("%s\n", device_name);
	device_select_result ret={found, ch_device_id, item, device_name};
	return ret;
}

bool item_has_feeder(IWiaItem* p_wia_item){
	printf("all_pages\n");
	PROPSPEC specDocumentHandlingSelect;

    specDocumentHandlingSelect.ulKind = PRSPEC_PROPID;
    specDocumentHandlingSelect.propid = WIA_DPS_DOCUMENT_HANDLING_SELECT;

    LONG nDocumentHandlingSelect;
	CComQIPtr<IWiaPropertyStorage> pWiaRootPropertyStorage(p_wia_item);
    HRESULT hr = ReadPropertyLong(
        pWiaRootPropertyStorage, 
        &specDocumentHandlingSelect, 
        &nDocumentHandlingSelect
    );
    if (SUCCEEDED(hr) && (nDocumentHandlingSelect & FEEDER)){
		return true;
	}else{
		return false;
	}
};


void tell_scanner_to_scan_all_pages(IWiaItem* p_wia_item){
	PROPSPEC specPages;
    specPages.ulKind = PRSPEC_PROPID;
    specPages.propid = WIA_DPS_PAGES;

    PROPVARIANT varPages;
                    
    varPages.vt = VT_I4;
    varPages.lVal = ALL_PAGES;

	CComQIPtr<IWiaPropertyStorage> pWiaRootPropertyStorage(p_wia_item);
    pWiaRootPropertyStorage->WriteMultiple(
        1,
        &specPages,
        &varPages,
        WIA_DPS_FIRST
    );
                
    PropVariantClear(&varPages);
}


struct scan_settings_result display_scan_settings_dialog(struct device_select_result sel_res, bool single_image, int intent){
	scan_settings_result ret;
	ret.error = false;
	if(!sel_res.found){
		ret.item_count=0;
		ret.error = true;
	}
	HWND current_window = FindMyTopMostWindow();
	LONG item_count;
	IWiaItem** ppIWiaItem;
	LONG flags = WIA_DEVICE_DIALOG_USE_COMMON_UI; //intent flag - see http://msdn.microsoft.com/en-us/library/windows/desktop/ms630190(v=vs.85).aspx
	LONG l_intent = intent;//text intent
	printf("intent:%i\n", l_intent);
	if(l_intent & WIA_INTENT_IMAGE_TYPE_TEXT){
		printf("text indeed\n");
	}
	if(single_image){
		flags|=WIA_DEVICE_DIALOG_SINGLE_IMAGE;
	}
	HRESULT hr = sel_res.p_wia_item->DeviceDlg(
        current_window,
        flags,
        l_intent,
		&item_count,
        &ppIWiaItem
    );
	tell_scanner_to_scan_all_pages(sel_res.p_wia_item);
	ret.item_count = item_count;
	ret.wia_item_array = ppIWiaItem;
	ret.wia_root_item = sel_res.p_wia_item;
	return ret;	
}


HRESULT CALLBACK DefaultProgressCallback(LONG   lStatus, LONG lPercentComplete, PVOID  pParam){
    WiaWrap::CProgressDlg *pProgressDlg = (WiaWrap::CProgressDlg *) pParam;
    if (pProgressDlg == NULL){
        return E_POINTER;
    }
    if (pProgressDlg->Cancelled()){
        return S_FALSE;
    }
	UINT uID;
    switch (lStatus){
        case IT_STATUS_TRANSFER_FROM_DEVICE:
		{
			uID = IDS_STATUS_TRANSFER_FROM_DEVICE;
            break;
		}
        case IT_STATUS_PROCESSING_DATA:
		{
            uID = IDS_STATUS_PROCESSING_DATA;
            break;
		}
        case IT_STATUS_TRANSFER_TO_CLIENT:
		{
            uID = IDS_STATUS_TRANSFER_TO_CLIENT;
            break;
		}
		default:
		{
			return E_INVALIDARG;
		}
    }		

    TCHAR szFormat[DEFAULT_STRING_SIZE] = _T("%d");
    LoadString(g_hInstance, uID, szFormat, COUNTOF(szFormat));
    TCHAR szStatusText[DEFAULT_STRING_SIZE];
    _sntprintf_s(szStatusText, COUNTOF(szStatusText), COUNTOF(szStatusText) - 1, szFormat, lPercentComplete);
    szStatusText[COUNTOF(szStatusText) - 1] = _T('\0');
    // Update the progress bar values
    pProgressDlg->SetMessage(szStatusText);
    pProgressDlg->SetPercent(lPercentComplete);
    return S_OK;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid){
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j){
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ){
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

custom_image::custom_image(IStream* p_stream):m_stream(p_stream){
	Gdiplus::GdiplusStartupInput gdi_input;
	gdi_input.GdiplusVersion= 1;
	gdi_input.DebugEventCallback=NULL;
	gdi_input.SuppressBackgroundThread=FALSE;
	Gdiplus::Status init_status = Gdiplus::GdiplusStartup(&gdi_pointer, &gdi_input, NULL);
	printf("GDI+ init status: %d\n", init_status);
}
custom_image::~custom_image(){
	Gdiplus::GdiplusShutdown(gdi_pointer);	
}

void custom_image::save_to_file(CString file){
	Gdiplus::Image image(m_stream);
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	Gdiplus::Status stat = image.Save(file, &pngClsid, NULL);
	if(stat == Gdiplus::Ok)
		printf("Bird.png was saved successfully\n");
	else
		printf("Failure: stat = %d\n", stat); 
}


std::vector<saved_image> scan(scan_settings_result settings){
	printf("called scan\n");
	CComPtr<WiaWrap::CProgressDlg> pProgressDlg;
	//HRESULT CALLBACK pfnProgressCallback = DefaultProgressCallback;
	HWND window = FindMyTopMostWindow();
	pProgressDlg = new WiaWrap::CProgressDlg(window);
	//pProgressDlg = new WiaWrap::CProgressDlg(NULL);
	PVOID pProgressCallbackParam = (WiaWrap::CProgressDlg *) pProgressDlg;

    // Create the data callback interface

	LONG item_count=0;
	IStream ** streams = (IStream **) CoTaskMemAlloc(0);//That line fixed many things. It was very hard to come up with!	
	std::vector<LONG> sizes;

    CComPtr<WiaWrap::CDataCallback> pDataCallback = new WiaWrap::CDataCallback(
        DefaultProgressCallback,
        pProgressCallbackParam,
        &item_count, 
        &streams,
		&sizes
    );

	for (int i = 0; i < settings.item_count; ++i){
		CComQIPtr<IWiaPropertyStorage> pWiaPropertyStorage(settings.wia_item_array[i]);
		CComQIPtr<IWiaDataTransfer> pIWiaDataTransfer(settings.wia_item_array[i]);
        PROPSPEC specTymed;

        specTymed.ulKind = PRSPEC_PROPID;
        specTymed.propid = WIA_IPA_TYMED;

        PROPVARIANT varTymed;

        varTymed.vt = VT_I4;
        varTymed.lVal = TYMED_CALLBACK;

        HRESULT hr = pWiaPropertyStorage->WriteMultiple(
            1,
            &specTymed,
            &varTymed,
            WIA_IPA_FIRST
        );

        PropVariantClear(&varTymed);

        if (FAILED(hr)){
            //return hr;
        }
		// If there is no transfer format specified, use the device default

        GUID guidFormat = GUID_NULL;

		GUID* pguidFormat = NULL;//needs to be changed if I want to have a specific image format by default

        if (pguidFormat == NULL){
            pguidFormat = &guidFormat;
        }

        if (*pguidFormat == GUID_NULL){
            PROPSPEC specPreferredFormat;
            specPreferredFormat.ulKind = PRSPEC_PROPID;
            specPreferredFormat.propid = WIA_IPA_PREFERRED_FORMAT;
            hr = ReadPropertyGuid(
                pWiaPropertyStorage,
                &specPreferredFormat,
                pguidFormat
            );

            if (FAILED(hr)){
                //return hr;
            }
        }

        // Set the transfer format

        PROPSPEC specFormat;

        specFormat.ulKind = PRSPEC_PROPID;
        specFormat.propid = WIA_IPA_FORMAT;

        PROPVARIANT varFormat;

        varFormat.vt = VT_CLSID;
        varFormat.puuid = (CLSID *) CoTaskMemAlloc(sizeof(CLSID));

        *varFormat.puuid = *pguidFormat;

        hr = pWiaPropertyStorage->WriteMultiple(
            1,
            &specFormat,
            &varFormat,
            WIA_IPA_FIRST
        );

        PropVariantClear(&varFormat);

        /*if (FAILED(hr)){
            return hr;
        }*/

        // Read the transfer buffer size from the device, default to 64K

        PROPSPEC specBufferSize;

        specBufferSize.ulKind = PRSPEC_PROPID;
        specBufferSize.propid = WIA_IPA_BUFFER_SIZE;

        LONG nBufferSize;

        hr = ReadPropertyLong(
            pWiaPropertyStorage, 
            &specBufferSize, 
            &nBufferSize
        );

        if (FAILED(hr)){
            nBufferSize = 64 * 1024;
		}
        // Choose double buffered transfer for better performance

        WIA_DATA_TRANSFER_INFO WiaDataTransferInfo = { 0 };

        WiaDataTransferInfo.ulSize        = sizeof(WIA_DATA_TRANSFER_INFO);
        WiaDataTransferInfo.ulBufferSize  = 2 * nBufferSize;
        WiaDataTransferInfo.bDoubleBuffer = TRUE;

        // Start the transfer

        hr = pIWiaDataTransfer->idtGetBandedData(
            &WiaDataTransferInfo,
            pDataCallback
        );

		std::vector<saved_image> ret;

		for(int i=0; i<item_count; i++){
			custom_image temp_image(streams[i]);
			CString filename;
			filename.Format(L"scan_temp%i.png", i);
			temp_image.save_to_file(filename);
			saved_image temp_svd;
			temp_svd.filename=filename;
			ret.push_back(temp_svd);
		}
		return ret;
    }
	//return streams;
}