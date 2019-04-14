#include "stdafx.h"
#include "UDSDef.h"



UDSService  UDSSevArray[SIZE_UDSSERVICE_NUM] =
{
	{ 0x10, _T("DiagnosticSessionControl") },
	{ 0x11, _T("ECUReset") },
	{ 0x22, _T("ReadDataByIdentifier") },
	{ 0x27, _T("SecurityAccess") },
	{ 0x28, _T("CommunicationControl") },
	{ 0x2e, _T("WriteDataByIdentifier") },
	{ 0x31, _T("RoutineControl") },
	{ 0x34, _T("RequestDownload") },
	{ 0x36, _T("TransferData") },
	{ 0x37, _T("RequestTransferExit") },
	{ 0x85, _T("ControlDTCSetting") },
	{ 0x3e, _T("TesterPresent")}

};

Negative_Resp NegativeResponse[SIZE_NEG_RESP_NUM] =
{
	{ 0x10, _T("Neg Response - General Reject") },
	{ 0x11, _T("Neg Response - Service Not Supported") },
	{ 0x12, _T("Neg Response - SubFunction Not Supported") },
	{ 0x13, _T("Neg Response - Incorrect Message Length Or Invalid Format") },
	{ 0x21, _T("Neg Response - Busy Repeat Request") },
	{ 0x22, _T("Neg Response - Conditions Not Correct") },
	{ 0x24, _T("Neg Response - Request Sequence Error") },
	{ 0x31, _T("Neg Response - Request Out Of Range") },
	{ 0x33, _T("Neg Response - Security Access Denied") },
	{ 0x35, _T("Neg Response - Invalid Key") },
	{ 0x36, _T("Neg Response - Exceed Number Of Attempts") },
	{ 0x37, _T("Neg Response - Required Time Delay Not Expired") },
	{ 0x70, _T("Neg Response - Upload Download NotAccepted") },
	{ 0x71, _T("Neg Response - Transfer Data Suspended") },
	{ 0x72, _T("Neg Response - General Programming Failure") },
	{ 0x73, _T("Neg Response - Wrong Block Sequence Counter") },
	{ 0x78, _T("Neg Response - Response Pending") },
	{ 0x7E, _T("Neg Response - SubFunction Not Supported In Active Session") },
	{ 0x7F, _T("Neg Response - Service Not Supported In Active Session") },
	{ 10, _T("Positive Response") },		//It has to be always declared here 
	{ 4, _T("Negative Response") },
};



CString VerifyService(BYTE sid)
{
	int i = 0;
	CString str;
	if (sid >= 0 && sid <= 9)
	{
		str.Format(_T(" Service # %d "), sid);
		return str;
	}
	for (; i < SIZE_UDSSERVICE_NUM; i++)
	{
		if (UDSSevArray[i].ServiceID == sid)
			return UDSSevArray[i].SIDString;
	}

	if (i == SIZE_UDSSERVICE_NUM)
		return _T("undefined service !");
}

CString VerifyNegResponse(BYTE value)
{

	for (int i_counter = 0; i_counter < SIZE_NEG_RESP_NUM; i_counter++)
	{
		if (NegativeResponse[i_counter].MsgValue == value){
			return NegativeResponse[i_counter].StrTypeResp;
		}
	}
	return NegativeResponse[SIZE_NEG_RESP_NUM - 1].StrTypeResp;
}