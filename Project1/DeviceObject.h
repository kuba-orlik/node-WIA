#include <vector>

class CWiaTransferCallback : public IWiaTransferCallback{
public: // Constructors, destructor
    CWiaTransferCallback () : m_cRef(1) {
		printf("construct");
	};

    ~ CWiaTransferCallback () {
	};

    HRESULT __stdcall TransferCallback(LONG lFlags, WiaTransferParams *pWiaTransferParams){
        HRESULT hr = S_OK;
		printf("callback");
        switch (pWiaTransferParams->lMessage){
            case WIA_TRANSFER_MSG_STATUS:				
                break;
            case WIA_TRANSFER_MSG_END_OF_STREAM:
                break;
            case WIA_TRANSFER_MSG_END_OF_TRANSFER:
                break;
            default:
                break;
        }
        return hr;
    }

    HRESULT __stdcall GetNextStream(LONG lFlags, BSTR bstrItemName, BSTR bstrFullItemName, IStream **ppDestination){
		printf("getNextStream");
        HRESULT hr = S_OK;

        //  Return a new stream for this item's data.
        //
        //hr = CreateDestinationStream(bstrItemName, ppDestination);
        return hr;
    }

public: // IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject){
		printf("query");
		return NULL;
	};

	ULONG STDMETHODCALLTYPE AddRef( void){
		printf("addref");
		return NULL;
	};

	ULONG STDMETHODCALLTYPE Release( void){
		printf("release");
		return NULL;
	};

private:
    /// For ref counting implementation
    LONG                m_cRef;
};


class DeviceObject{
private:
	IWiaItem2* wia_item;
	IWiaTransfer* wia_transfer;
	std::vector <IWiaItem2*> files;
	std::vector <IWiaItem2*> transferable;


	void obtainTransferInterface(){
		//IWiaTransfer *pWiaTransfer = NULL;
		wia_item->QueryInterface(IID_IWiaTransfer,(void**)&wia_transfer);
	}

	HRESULT enumerate_elements(IWiaItem2* wia_item_t){
        LONG lItemType = 0;
        if(wia_item_t==NULL){
            wia_item_t = wia_item;//root;
        }
        HRESULT hr = wia_item_t->GetItemType( &lItemType );
        if (SUCCEEDED(hr))
        {
            if (lItemType & WiaItemTypeFolder || lItemType & WiaItemTypeHasAttachments)
            {
                //
                // Get the child item enumerator for this item.
                //
                IEnumWiaItem2 *pEnumWiaItem = NULL;
                
                hr = wia_item_t->EnumChildItems( NULL, &pEnumWiaItem );
                if (SUCCEEDED(hr))
                {
                    //
                    // Loop until you get an error or pEnumWiaItem->Next returns
                    // S_FALSE to signal the end of the list.
                    //
                    while (S_OK == hr)
                    {
                        //
                        // Get the next child item.
                        //
                        IWiaItem2 *pChildWiaItem = NULL; //Vista or later

                        hr = pEnumWiaItem->Next( 1, &pChildWiaItem, NULL );                        

                        //
                        // pEnumWiaItem->Next will return S_FALSE when the list is
                        // exhausted, so check for S_OK before using the returned
                        // value.
                        //
                        if (S_OK == hr)
                        {
							long type;
							pChildWiaItem->GetItemType(&type);

							printf("%i:\n", type);
							if((type & WiaItemTypeFile)!=0){
								printf("    file\n");
								files.push_back(pChildWiaItem);
							}

							if((type & WiaItemTypeTransfer )!=0){
								printf("    transfer\n");
								transferable.push_back(pChildWiaItem);
								//printf("transferable\n");
							}

                            //
                            // Recurse into this item.
                            //
							hr = enumerate_elements( pChildWiaItem );

                            //
                            // Release this item.
                            //
                            pChildWiaItem->Release();
                            pChildWiaItem = NULL;
                        }
                    }

                    //
                    // If the result of the enumeration is S_FALSE (which
                    // is normal), change it to S_OK.
                    //
                    if (S_FALSE == hr)
                    {
                        hr = S_OK;
                    }

                    //
                    // Release the enumerator.
                    //
                    pEnumWiaItem->Release();
                    pEnumWiaItem = NULL;
                }
            }
        }
		return hr;
	};
public:
	DeviceObject(IWiaItem2* wia_itemL):wia_item(wia_itemL){
		enumerate_elements(NULL);
		obtainTransferInterface();
	}

	DeviceObject(){
		//for fuck's sake
	}

	void transfer(){
		IWiaItem2* image;
		//transferable;
		HRESULT hr = transferable[1]->DeviceCommand(0, &WIA_CMD_TAKE_PICTURE, &image);	
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		printf("%s\n", errMsg);
	}
};