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

	__declspec(dllexport) int select_scanner(){
		select_device(false);
		return 4;
	}
}

/*	 
}
*/