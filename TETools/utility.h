
#pragma once

#include "stdafx.h"

#define  USEREXPORT __declspec(dllexport)



#ifdef __cplusplus
  extern "C"{
#endif
	  

	  DWORD  USEREXPORT Get_Key(DWORD seed);
	  void   USEREXPORT Chks_Init(void);
	  void   USEREXPORT Chks_Process(BYTE *buf, DWORD size);
	  DWORD  USEREXPORT Chks_Finish(void);


#ifdef __cplusplus
}
#endif