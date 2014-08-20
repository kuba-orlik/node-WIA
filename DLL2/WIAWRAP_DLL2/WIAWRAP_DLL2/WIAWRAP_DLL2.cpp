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

	__declspec(dllexport) char* scan_images(int intent){
		printf("%i", intent);
		device_select_result res = select_device(false);
		if(res.found){
			printf("found");
		}else{
			printf("not found");
		}
		//printf("choice: %ws.\n", res.device_id);
		struct scan_settings_result set_res =  display_scan_settings_dialog(res);
		vector<saved_image> images = scan(set_res);
		CStringA return_json = "[";
		for(unsigned int i=0; i<images.size(); i++){
			CString temp;
			wchar_t* tmplt;
			if(i==images.size()-1){
				tmplt = L"\"%ws\"]";
			}else{
				tmplt = L"\"%ws\",";
			}
			temp.Format(tmplt, images[i].filename);
			return_json+=temp;
		}
		printf("%ws\n", return_json);
		int length = return_json.GetLength();
		char* char_array = new char[length+1];
		strcpy_s(char_array, length+1, return_json);
		return char_array;
		//getchar();
		//return 0;
	}

	__declspec(dllexport) int get_char(){
		getchar();
		return 0;
	}

}

/*	 
}
*/