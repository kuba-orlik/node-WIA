#include "windows.h"

class Formats{
public:
	static PROPSPEC getProp(int id){
		PROPSPEC p;
		p.ulKind = 1;
		p.propid = id;
		return p;
	}

	static PROPVARIANT getVariant(VARTYPE type){
		PROPVARIANT v;
		v.vt = type;
		return v;
	}

};