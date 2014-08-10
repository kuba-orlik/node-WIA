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
#include "wia_property_access.h"
#include "structures.h"
#include "scanners.h"

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