#pragma once

#include "stdafx.h"



//#define _LOG
typedef struct tagUDSService   
{
	BYTE           ServiceID;
	CString        SIDString;

} UDSService;


typedef struct tagNegative_Resp
{
	BYTE 			MsgValue;
	CString         StrTypeResp;
} Negative_Resp;

#define SIZE_UDSSERVICE_NUM   12
#define SIZE_NEG_RESP_NUM     21

//#define    SID10DIAGNSESSIONCTRL         0x10     
//#define    SID11ECURESET                 0x11     
//#define    SID22READDATABYID             0x22
//#define    SID27SECCURITYACCESS          0x27
//#define    SID28COMMUNICATIONCTRL        0x28
//#define    SID2EWRITEDATEBYID            0x2E
//#define    SID31ROUTINECTRL              0x31
//#define    SID34REQUESTDOWNLOAD          0x34
//#define    SID36TRANSFERDATA             0x36
//#define    SID37REQUESTTANSFEXIT         0x37
//#define    SID85CTRLDTCSETTING           0x85


CString VerifyService(BYTE sid);
CString VerifyNegResponse(BYTE value);