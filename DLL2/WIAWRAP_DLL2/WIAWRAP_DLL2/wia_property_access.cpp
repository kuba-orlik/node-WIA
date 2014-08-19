#include "wia_property_access.h"

HRESULT ReadPropertyLong(IWiaPropertyStorage *pWiaPropertyStorage, const PROPSPEC *pPropSpec, LONG *plResult){
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

HRESULT ReadPropertyGuid(IWiaPropertyStorage *pWiaPropertyStorage, const PROPSPEC *pPropSpec, GUID *pguidResult){
    PROPVARIANT PropVariant;

    HRESULT hr = pWiaPropertyStorage->ReadMultiple(
        1, 
        pPropSpec, 
        &PropVariant
    );
    if (SUCCEEDED(hr)){
        switch (PropVariant.vt){
            case VT_CLSID:{
                *pguidResult = *PropVariant.puuid; 
                hr = S_OK;
                break;
            }case VT_BSTR:{
                hr = CLSIDFromString(PropVariant.bstrVal, pguidResult);
                break;
            }case VT_LPWSTR:{
                hr = CLSIDFromString(PropVariant.pwszVal, pguidResult);
                break;
            }case VT_LPSTR:{
                WCHAR wszGuid[MAX_GUID_STRING_LEN];
				mbstowcs_s(NULL, wszGuid, MAX_GUID_STRING_LEN, PropVariant.pszVal, MAX_GUID_STRING_LEN);

                wszGuid[MAX_GUID_STRING_LEN - 1] = L'\0';

                hr = CLSIDFromString(wszGuid, pguidResult);

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