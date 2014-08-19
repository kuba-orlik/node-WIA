/*
#ifdef MATHFUNCSDLL_EXPORTS
#define MATHFUNCSDLL_API __declspec(dllexport) 
#else
#define MATHFUNCSDLL_API __declspec(dllimport) 
#endif
*/

#include <string>

/*namespace WIAWRAP
{
    // This class is exported from the MathFuncsDll.dll
    class WIAWRAP{
    public: 
        // Returns a + b
		static  MATHFUNCSDLL_API int scan(); 

      
		
    };
}

*/
extern "C"{
	__declspec(dllexport) int lol();
}

