/*
#ifdef MATHFUNCSDLL_EXPORTS
#define MATHFUNCSDLL_API __declspec(dllexport) 
#else
#define MATHFUNCSDLL_API __declspec(dllimport) 
#endif
*/

#include <string>

extern "C"{
	__declspec(dllexport) int lol();
}

