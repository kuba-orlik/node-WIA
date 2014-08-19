#define IDS_WAIT                        4
#define IDS_STATUS_TRANSFER_FROM_DEVICE 5
#define IDS_STATUS_PROCESSING_DATA      6
#define IDS_STATUS_TRANSFER_TO_CLIENT   7
#define IDS_ERROR_GET_IMAGE_DLG         8


#ifndef structures
#define structures


struct device_select_result{
	bool found;
	BSTR device_id;
	IWiaItem* p_wia_item;
};


struct scan_settings_result{
	LONG item_count;
	IWiaItem** wia_item_array;
	bool error;
	bool document_feader;
	IWiaItem* wia_root_item;
};

#endif