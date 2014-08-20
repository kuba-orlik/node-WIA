// WIAWRAP_DLL2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "WIAWRAP_DLL2.h"
#include <stdexcept>
#include <string>

#include "scanners.h"

using namespace std;

/*
namespace WIAWRAP{
*/

extern "C"{
	__declspec(dllexport) int lol(){
		return 354;
	}

	__declspec(dllexport) device_select_result select_scanner(){
		return select_device(false);
	}

	__declspec(dllexport) int scan_images(int intent){
		printf("%i", intent);
		device_select_result res = select_device(false);
		if(res.found){
			printf("found");
		}else{
			printf("not found");
		}
		//printf("choice: %ws.\n", res.device_id);
		struct scan_settings_result set_res =  display_scan_settings_dialog(res);
		scan(set_res); 
		return 0;
	}

}

/*	 
}
*/