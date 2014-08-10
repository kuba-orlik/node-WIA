#define _WIN32_WINNT 0x0600

#include "stdio.h"
#include <iostream>
#include <vector>
#include "windows.h"
#include "window.h"
#include "winerror.h"
#include "Wia.h"
#include <WiaDef.h>
#include "comdef.h"
#pragma comment(lib, "wiaguid")
#include "stdafx.h"
#include "ProgressDlg.h"
#include "structures.h"

#define IDS_WAIT                        4
#define IDS_STATUS_TRANSFER_FROM_DEVICE 5
#define IDS_STATUS_PROCESSING_DATA      6
#define IDS_STATUS_TRANSFER_TO_CLIENT   7
#define IDS_ERROR_GET_IMAGE_DLG         8


IWiaDevMgr* Manager;

void init_wia_manager(){
	Manager = NULL;
	CoCreateInstance( CLSID_WiaDevMgr, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr, (void**)&Manager ); 
}

struct device_select_result select_device(bool force_display_dialog=false){
	CoInitialize(nullptr);
	init_wia_manager();
	HWND current_handle= FindMyTopMostWindow();
	BSTR device_id;
	IWiaItem* item;
	long flags = 0;
	if(force_display_dialog){
		flags = WIA_SELECT_DEVICE_NODEFAULT;
	}
	Manager->SelectDeviceDlg(current_handle, StiDeviceTypeScanner, flags, &device_id,  &item);
	bool found = (device_id!=NULL);
	device_select_result ret={found, device_id, item};
	return ret;
}

HRESULT ReadPropertyLong(IWiaPropertyStorage *pWiaPropertyStorage, const PROPSPEC      *pPropSpec, LONG *plResult){
    PROPVARIANT PropVariant;
    HRESULT hr = pWiaPropertyStorage->ReadMultiple(1, pPropSpec, &PropVariant);
    if (SUCCEEDED(hr)){
		switch (PropVariant.vt){
            case VT_I1:{
                *plResult = (LONG) PropVariant.cVal;
                hr = S_OK;
                break;
            }case VT_UI1:{
                *plResult = (LONG) PropVariant.bVal;
                hr = S_OK;
                break;
            }case VT_I2:{
                *plResult = (LONG) PropVariant.iVal;
                hr = S_OK;
                break;
            }case VT_UI2:{
                *plResult = (LONG) PropVariant.uiVal;
                hr = S_OK;
                break;
            }case VT_I4:{
                *plResult = (LONG) PropVariant.lVal;
                hr = S_OK;
                break;
            }case VT_UI4:{
                *plResult = (LONG) PropVariant.ulVal;
                hr = S_OK;
                break;
            }case VT_INT:{
                *plResult = (LONG) PropVariant.intVal;
                hr = S_OK;
                break;
            }case VT_UINT:{
                *plResult = (LONG) PropVariant.uintVal;
                hr = S_OK;
                break;
            }case VT_R4:{
                *plResult = (LONG) (PropVariant.fltVal + 0.5);
                hr = S_OK;
                break;
            }case VT_R8:{
                *plResult = (LONG) (PropVariant.dblVal + 0.5);
                hr = S_OK;
                break;
            }default:{
                hr = E_FAIL;
                break;
            }
        }
    }
    PropVariantClear(&PropVariant);
    return hr;
}

bool item_has_feeder(IWiaItem* p_wia_item){
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


struct scan_settings_result display_scan_settings_dialog(struct device_select_result sel_res, bool single_image = false){
	scan_settings_result ret;
	ret.error = false;
	if(!sel_res.found){
		ret.item_count=0;
		ret.error = true;
	}
	HWND current_window = FindMyTopMostWindow();
	LONG item_count;
	IWiaItem** ppIWiaItem;
	LONG flags = 0; //intent flag - see http://msdn.microsoft.com/en-us/library/windows/desktop/ms630190(v=vs.85).aspx
	if(single_image){
		flags|=WIA_DEVICE_DIALOG_SINGLE_IMAGE;
	}
	HRESULT hr = sel_res.p_wia_item->DeviceDlg(
        current_window,
        WIA_DEVICE_DIALOG_USE_COMMON_UI,
        flags,
		&item_count,
        &ppIWiaItem
    );
	ret.item_count = item_count;
	ret.wia_item_array = ppIWiaItem;
	ret.wia_root_item = sel_res.p_wia_item;
	return ret;	
}

int main(){
	device_select_result res = select_device();
	if(res.found){
		printf("found");
	}else{
		printf("not found");
	}
	printf("choice: %ws.\n", res.device_id);
	struct scan_settings_result set_res =  display_scan_settings_dialog(res);
	printf("amount of items: %i\n", set_res.item_count);
	printf("after dialog");
	getchar();
	return 0;
}