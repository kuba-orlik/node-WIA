#include <stdio.h>
#include "windows.h"
#include "winerror.h"
#include "wia.h"
#pragma comment(lib, "wiaguid")


HRESULT CreateWiaDeviceManager( IWiaDevMgr2 **ppWiaDevMgr ){
	if (NULL == ppWiaDevMgr){
		return E_INVALIDARG;
	}
	*ppWiaDevMgr = NULL;
	HRESULT hr = CoCreateInstance( CLSID_WiaDevMgr2, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr2, (void**)ppWiaDevMgr ); 
    return hr;
}

int main(){
	printf("lol");
	Sleep(2000);
	return 0;
}