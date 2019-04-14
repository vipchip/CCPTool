#include "stdafx.h"
#include "Reg.h"


lpcheckout CheckOut;
lpcheckin  CheckIn;
lplockcan  LockCANDev;


bool LoadRegDLL(HINSTANCE &hDLL)
{
	if ((hDLL = LoadLibrary(_T("AuthDIL.dll"))) == NULL)
		return false;
	CheckOut = (lpcheckout)GetProcAddress(hDLL, "CheckOut");
	CheckIn = (lpcheckin)GetProcAddress(hDLL, "CheckIn");
	LockCANDev = (lplockcan)GetProcAddress(hDLL, "LockCAN");

	if (!CheckOut || !CheckIn || !LockCANDev)
	{
		FreeLibrary(hDLL);
		return false;
	}

	return true;

}


void UnloadRegDLL(HINSTANCE &hDLL)
{
	if (hDLL)
		FreeLibrary(hDLL);

}