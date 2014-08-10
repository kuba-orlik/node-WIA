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