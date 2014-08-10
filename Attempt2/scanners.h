#include <Wia.h>
#include "wia_property_access.h"
#pragma comment(lib, "wiaguid")
#include "structures.h"
#include "DataCallback.h"

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

void scan(scan_settings_result settings){
	 CComPtr<CProgressDlg> pProgressDlg;
    if (pfnProgressCallback == NULL)
    {
        pfnProgressCallback = DefaultProgressCallback;

        pProgressDlg = new CProgressDlg(hWndParent);

        pProgressCallbackParam = (CProgressDlg *) pProgressDlg;
    }

    // Create the data callback interface

    CComPtr<CDataCallback> pDataCallback = new CDataCallback(
        pfnProgressCallback,
        pProgressCallbackParam,
        plCount, 
        pppStream
    );

    if (pDataCallback == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Start the transfer of the selected items

    for (int i = 0; i < ppIWiaItem.Count(); ++i)
    {
        // Get the interface pointers

        CComQIPtr<IWiaPropertyStorage> pWiaPropertyStorage(ppIWiaItem[i]);

        if (pWiaPropertyStorage == NULL)
        {
            return E_NOINTERFACE;
        }

        CComQIPtr<IWiaDataTransfer> pIWiaDataTransfer(ppIWiaItem[i]);

        if (pIWiaDataTransfer == NULL)
        {
            return E_NOINTERFACE;
        }

        // Set the transfer type

        PROPSPEC specTymed;

        specTymed.ulKind = PRSPEC_PROPID;
        specTymed.propid = WIA_IPA_TYMED;

        PROPVARIANT varTymed;

        varTymed.vt = VT_I4;
        varTymed.lVal = TYMED_CALLBACK;

        hr = pWiaPropertyStorage->WriteMultiple(
            1,
            &specTymed,
            &varTymed,
            WIA_IPA_FIRST
        );

        PropVariantClear(&varTymed);

        if (FAILED(hr))
        {
            return hr;
        }

        // If there is no transfer format specified, use the device default

        GUID guidFormat = GUID_NULL;

        if (pguidFormat == NULL)
        {
            pguidFormat = &guidFormat;
        }

        if (*pguidFormat == GUID_NULL)
        {
            PROPSPEC specPreferredFormat;

            specPreferredFormat.ulKind = PRSPEC_PROPID;
            specPreferredFormat.propid = WIA_IPA_PREFERRED_FORMAT;

            hr = ReadPropertyGuid(
                pWiaPropertyStorage,
                &specPreferredFormat,
                pguidFormat
            );

            if (FAILED(hr))
            {
                return hr;
            }
        }

        // Set the transfer format

        PROPSPEC specFormat;

        specFormat.ulKind = PRSPEC_PROPID;
        specFormat.propid = WIA_IPA_FORMAT;

        PROPVARIANT varFormat;

        varFormat.vt = VT_CLSID;
        varFormat.puuid = (CLSID *) CoTaskMemAlloc(sizeof(CLSID));

        if (varFormat.puuid == NULL)
        {
            return E_OUTOFMEMORY;
        }

        *varFormat.puuid = *pguidFormat;

        hr = pWiaPropertyStorage->WriteMultiple(
            1,
            &specFormat,
            &varFormat,
            WIA_IPA_FIRST
        );

        PropVariantClear(&varFormat);

        if (FAILED(hr))
        {
            return hr;
        }

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

        if (FAILED(hr))
        {
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

        if (FAILED(hr) || hr == S_FALSE)
        {
            return hr;
        }
    }
}