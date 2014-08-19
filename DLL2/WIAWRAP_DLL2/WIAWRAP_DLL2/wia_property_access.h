#include "StdAfx.h"
#ifndef prop_accs
#define prop_accs

#define MAX_GUID_STRING_LEN 39

HRESULT ReadPropertyLong(IWiaPropertyStorage *pWiaPropertyStorage, const PROPSPEC *pPropSpec, LONG *plResult);

HRESULT ReadPropertyGuid(IWiaPropertyStorage *pWiaPropertyStorage, const PROPSPEC *pPropSpec, GUID *pguidResult);

#endif

