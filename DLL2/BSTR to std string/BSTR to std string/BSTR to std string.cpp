// BSTR to std string.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <wtypes.h>
#include <comutil.h>
#pragma comment(lib,"comsuppw.lib")
#include <string>
#include <string.h>
#include <stdio.h>

using namespace std;

string bstr_to_str(BSTR source){
	//source = L"lol2inside";
	_bstr_t wrapped_bstr = _bstr_t(source);
	int length = wrapped_bstr.length();
	char* char_array = new char[length];
	strcpy_s(char_array, length+1, wrapped_bstr);
	return char_array;
}


int _tmain(int argc, _TCHAR* argv[])
{
	BSTR bstr_var = SysAllocString(L"I am bstr");
	string str = bstr_to_str(bstr_var);
	printf("result: %s\n", str.c_str());//result: I am bstr
	getchar();
	return 0;
}

