#include <Wia.h>
#include "window.h"
#include "wia_property_access.h"
#pragma comment(lib, "wiaguid")
#include "structures.h"
#include "DataCallback.h"
#include "ProgressDlg.h"
#include "stdafx.h"
#include <dlgs.h>
#include <sstream>
#include <iostream>  
#include <fstream>   
#include <atlstr.h>

#ifdef WIAWRAP_DLL_EXPORT
#define WIAWRAP_API __declspec(dllexport) 
#else
#define WIAWRAP_API __declspec(dllimport) 
#endif

extern IWiaDevMgr* Manager;

void init_wia_manager();

struct saved_image{
	/*IStream* stream;
	LONG size;
	//char* char_array;
	std::string strng;*/
	CString filename;
};


device_select_result select_device(bool force_display_dialog);

bool item_has_feeder(IWiaItem* p_wia_item);

void tell_scanner_to_scan_all_pages(IWiaItem* p_wia_item);

struct scan_settings_result display_scan_settings_dialog(struct device_select_result sel_res, bool single_image=false, int intent=4);

HRESULT CALLBACK DefaultProgressCallback(LONG   lStatus, LONG lPercentComplete, PVOID  pParam);

struct image;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

class custom_image{
private:
	IStream* m_stream;
	IStream* png_stream;
	ULONG_PTR gdi_pointer;
public:
	custom_image(IStream* p_stream);
	~custom_image();

	void save_to_file(CString file);

};

std::vector<saved_image> scan(scan_settings_result settings);