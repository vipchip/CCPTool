#pragma once
#include <windows.h>



typedef BOOL(*lpcheckout)(void);
typedef void(*lpcheckin)(void);
typedef int (*lplockcan)(const int& sn);


extern lpcheckout CheckOut;
extern lpcheckin  CheckIn;
extern lplockcan  LockCANDev;

bool LoadRegDLL(HINSTANCE &hAPIDLL);
void UnloadRegDLL(HINSTANCE &hAPIDLL);