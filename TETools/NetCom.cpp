// NetCom.cpp : implementation file
//

#include "stdafx.h"
#include "TETools.h"
#include "NetCom.h"
#include "TEToolsDlg.h"


byte DataBuf[MAXMESSAGELEN];


#define WAITTIME_NEOVI  5    //ValueCAN 3 设备等待超时



HANDLE CNetCom::m_hThreadDead = CreateEvent(NULL, FALSE, FALSE, NULL);
// CNetCom

IMPLEMENT_DYNCREATE(CNetCom, CWinThread)

CNetCom::CNetCom()
{
	m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	//m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);


	m_hTxTimer = NULL;
	m_hRxTimer = NULL;
	m_hFCEvent = NULL;
	m_bOpened = false;
	//m_RxLargeBuff.SetSize(100 * sizeof(CAN_Msg));

	m_RxMsg.pData = DataBuf;
	m_TxMsg.pData = DataBuf;

	InitializeCriticalSection(&m_cs);
	m_hTxTimer=CreateWaitableTimer(NULL, TRUE, NULL);
	m_hNetLayerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hFCEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_Device.SerialNumber = 0;
	SetRecieveID();
	
}

CNetCom::~CNetCom()
{
	CloseHandle(m_hEventKill);
	CloseHandle(m_hThreadDead);
	DeleteCriticalSection(&m_cs);
}

BOOL CNetCom::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	int size;

	while (WaitForSingleObject(m_hEventKill, 0) == WAIT_TIMEOUT)
	{
		if (!m_bOpened)
			continue;

		if (CheckRxBuf())
		{
			size = m_RxLargeBuff.GetCount();

			while (size)
			{
				ISO15765RxRoutine();
				size--;
			}

		}
		else
		{
			continue;
		}

	}

	return FALSE;
}


BEGIN_MESSAGE_MAP(CNetCom, CWinThread)
END_MESSAGE_MAP()


// CNetCom message handlers




void CNetCom::KillThread()
{
	VERIFY(SetEvent(m_hEventKill));
	SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	Sleep(200);
	WaitForSingleObject(m_hThread, INFINITE);
	delete this;
}

void CNetCom::Delete()
{
	CWinThread::Delete();
//	SetEvent(m_hThreadDead);
}


int  CNetCom::TxCANFrame(const CAN_Msg& txmsg)
{

	DWORD ErrorNumber;
	if (txmsg.ID <= 0x7FF)
	{
		m_icsMsg.StatusBitField = 0;
	}
	else if (txmsg.ID <= 0x1FFFFFFF)
	{
		m_icsMsg.StatusBitField = SPY_STATUS_XTD_FRAME;
	}
	else
	{
		return -1;
	}

	m_icsMsg.ArbIDOrHeader = txmsg.ID;
	m_icsMsg.NumberBytesData = txmsg.Len;
	memcpy(m_icsMsg.Data, txmsg.Data, txmsg.Len);

	m_icsMsg.StatusBitField2 = 0;

	int b;
	b = icsneoTxMessages(m_hObject, &m_icsMsg, NETID_HSCAN, 1);

#ifdef _LOG
	CString strlog;
	SYSTEMTIME st;
	GetLocalTime(&st);
	strlog.Format(_T("%02d:%02d:%03d,Tx,%08x %02x %02x %02x %02x %02x %02x %02x %02x,"), st.wMinute, st.wSecond, st.wMilliseconds,
		txmsg.ID, txmsg.Data[0], txmsg.Data[1], txmsg.Data[2], txmsg.Data[3], txmsg.Data[4], txmsg.Data[5],
		txmsg.Data[6], txmsg.Data[7]);	
#endif
	if (!b)
	{
		icsneoGetLastAPIError(m_hObject, &ErrorNumber);
#ifdef _LOG
		strlog += _T(" failed...");
#endif
	}
#ifdef _LOG
	OutputDebugString(strlog + _T("\n"));
#endif

	return b;
}

void  CNetCom::RxCANFrame()
{

	EnterCriticalSection(&m_cs);

	m_RxCANBuf = m_RxLargeBuff.GetAt(0);
	m_RxLargeBuff.RemoveAt(0);
#ifdef _LOG
	CString strlog;
	SYSTEMTIME st;
	GetLocalTime(&st);
	strlog.Format(_T("%02d:%02d:%03d,Rx,%08x %02x %02x %02x %02x %02x %02x %02x %02x,"), st.wMinute, st.wSecond, st.wMilliseconds,
		m_RxCANBuf.ID, m_RxCANBuf.Data[0], m_RxCANBuf.Data[1], m_RxCANBuf.Data[2], m_RxCANBuf.Data[3], m_RxCANBuf.Data[4], m_RxCANBuf.Data[5],
		m_RxCANBuf.Data[6], m_RxCANBuf.Data[7]);
	OutputDebugString(strlog + _T("\n"));
#endif

	LeaveCriticalSection(&m_cs);
	

}


bool CNetCom::TxISO15765Msg(BYTE *data, WORD size, MapingAdrMode mode, DWORD destAdress)
{
	CAN_Msg txframe;
	BYTE    ncf;

	if (mode == ExdMapingAdrMode)
		txframe.ID = ExPyhReqID | destAdress << 8;
	else
		txframe.ID = destAdress;
	SetRecieveID(destAdress);


	if (size <= 7)
	{
		//modified to sent 8 bytes each time 
		//txframe.Len = size+1;      
		txframe.Len = 8;
		memset(txframe.Data,0xff,8);
		txframe.Data[0] = 0x00 + size;
		memcpy(txframe.Data+1, data, size);
		if (TxCANFrame(txframe))
			return true;
		else
			return false;
	}

	else     //多帧处理
	{
		txframe.Len = 8;
		txframe.Data[0] = 0x10 | ((size>>8)&0x0f);
		txframe.Data[1] = size & 0xff;
		memcpy(txframe.Data + 2, data, 6);
		if (TxCANFrame(txframe)!=1)
			return false;



		//等待接收流控制
		if (WaitForSingleObject(m_hFCEvent, DEFAULTTIMEOUT) != WAIT_OBJECT_0)
			return false;
       
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = -10000*m_NetFC.STmin;

		int npacket;
		int icount;
		ncf = 1;
		npacket = ((size - 6) % 7) ? ((size - 6) / 7 + 1) : ((size - 6) / 7);
		for (icount = 0; icount < npacket;icount++)
		{
			SetWaitableTimer(m_hTxTimer, &liDueTime, 0, NULL, NULL, 0);
			if (WaitForSingleObject(m_hTxTimer, INFINITE) == WAIT_OBJECT_0)
			{
				txframe.Data[0] = 0x20 + ncf;
				memcpy(txframe.Data + 1, data + 6 + 7 * icount, 7);
				
				if (TxCANFrame(txframe)!=1)
				{
					return false;
				}
			}
			ncf++;
			if (ncf >= 16)
				ncf = 0;

		}

	}

	
	return true;

}
void CNetCom::RxISO15765Msg(ISO16765Msg *msg)
{

	//msg = &m_RxMsg;
	msg->ID = m_RxMsg.ID;
	msg->DataLen = m_RxMsg.DataLen;
	memcpy(msg->pData, m_RxMsg.pData, msg->DataLen);


	m_TxRxStatus.ID = 0;
	m_TxRxStatus.St_ok = 0;
	m_TxRxStatus.CurFrameNo = 0;
	m_TxRxStatus.RecBytes = 0;
	m_TxRxStatus.Type = 0xff;
	ResetEvent(m_hNetLayerEvent);
}


void CNetCom::FlowControl(BYTE direct, BYTE flowstatus, BYTE blocksize, WORD STmin)
{

	m_NetFC.Direct = direct;
	m_NetFC.FlowStatus = flowstatus;
	m_NetFC.BlockSize = blocksize;
	m_NetFC.STmin = STmin;


	if (m_NetFC.Direct == 0)                    //接收多帧数据需要流控制
	{
		CAN_Msg fc_msg;
		fc_msg.ID = 0x18da3df1;
		fc_msg.Len = 8;
		memset(fc_msg.Data,0xff,8);
		fc_msg.Data[0] = 0x30 + m_NetFC.FlowStatus;
		fc_msg.Data[1] = m_NetFC.BlockSize;
		fc_msg.Data[2] = m_NetFC.STmin;
        TxCANFrame(fc_msg);
		//TRACE("rec flow control... \n");
	}

	if (m_NetFC.Direct == 1)                    //发送多帧数据需要来自ECU的流控制
	{
		SetEvent(m_hFCEvent);                   //收到来自ecu的流控制
		//TRACE("send flow control... \n");
	}



}

void CNetCom::ISO15765RxRoutine()
{

	BYTE PDUType;
	CString str;

	RxCANFrame();

	PDUType = m_RxCANBuf.Data[0] >> 4;
	switch (PDUType)
	{
	case 0:            //单帧
		{
				m_RxMsg.ID = m_RxCANBuf.ID;
				m_RxMsg.DataLen = m_RxCANBuf.Data[0]&0x0f;
				memcpy(m_RxMsg.pData, m_RxCANBuf.Data+1, m_RxMsg.DataLen);

				m_TxRxStatus.ID = m_RxMsg.ID;
				m_TxRxStatus.Type = 0;
				m_TxRxStatus.IsRxTx = 0;
				m_TxRxStatus.St_ok  = 1;
				m_TxRxStatus.RecBytes = m_RxMsg.DataLen;
				//TRACE(" Recieved single frame \n");
		}
		break;
	case 1:             //首帧
		{
			m_RxMsg.ID = m_RxCANBuf.ID;
			m_RxMsg.DataLen = (m_RxCANBuf.Data[0] & 0x0f) * 256 + m_RxCANBuf.Data[1];
			memcpy(m_RxMsg.pData, m_RxCANBuf.Data+2, 6);

			m_TxRxStatus.ID = m_RxMsg.ID;
			m_TxRxStatus.Type = 1;
			m_TxRxStatus.IsRxTx = 0;
			m_TxRxStatus.St_ok = 0;
			m_TxRxStatus.RecBytes = 6;
			m_TxRxStatus.CurFrameNo = 1;
			
			//发送流控制
			FlowControl(0,0,0,0);       // read,cts,all blocks,and no wait


		
		}
	break;
	case 2:            //连续帧
		{
			   
			if ((m_RxCANBuf.Data[0] & 0x0f) == m_TxRxStatus.CurFrameNo)
			{
				memcpy(m_RxMsg.pData + m_TxRxStatus.RecBytes, m_RxCANBuf.Data + 1, 7);
				m_TxRxStatus.Type = 2;
				m_TxRxStatus.IsRxTx = 0;
				m_TxRxStatus.St_ok = 0;
				m_TxRxStatus.RecBytes += 7;
				m_TxRxStatus.CurFrameNo++;
				if (m_TxRxStatus.CurFrameNo==15)
				{
					m_TxRxStatus.CurFrameNo = 0;
				}
				if (m_TxRxStatus.RecBytes >= m_RxMsg.DataLen)
				{
					m_TxRxStatus.St_ok = 1;

   			    }
			}
		}
		break;
	case 3:            //流控制
		{

			m_TxRxStatus.ID = m_RxMsg.ID;
			m_TxRxStatus.Type = 3;
			m_TxRxStatus.IsRxTx = 0;
			m_TxRxStatus.St_ok = 0;
			m_TxRxStatus.RecBytes = 3;
			FlowControl(1, (m_RxCANBuf.Data[0]) & 0x0f, m_RxCANBuf.Data[1], m_RxCANBuf.Data[2]);
			
		}
		break;
	}

	if (m_TxRxStatus.St_ok)
		SetEvent(m_hNetLayerEvent);    //通知应用程序收到CAN网络层消息

}


void CNetCom::ISO15765TxRoutine()
{
   

}

bool CNetCom::OnSerchDevice()
{
	int iResult;
	int iNumberOfDevices;


	iNumberOfDevices = 1;

	iResult = icsneoFindNeoDevices(16, &m_Device, &iNumberOfDevices);
	if (iResult == false)
	{
        return false;
	}

	if (iNumberOfDevices<1)
	{
        return false;
	}


	return true;
}

bool CNetCom::OpenPort()
{
	int iResult;

	iResult = icsneoOpenNeoDevice(&m_Device, &m_hObject, NULL, 1, 0);
	if (iResult == 0)
	{
       return false;
	}

	iResult = icsneoSetBitRate(m_hObject, DIAGCANBAUDRATE, NETID_HSCAN);
	if (iResult == 0)
	{
		return false;
	}

	m_bOpened = true;
	return true;
}

int  CNetCom::GetSerialNumber()
{
	return m_Device.SerialNumber;
}
bool CNetCom::ClosePort()
{

	bool bResult = true;
	int iResult;
	int iNumberOfErrors;

	iResult = icsneoClosePort(m_hObject, &iNumberOfErrors);
	if (iResult == 0)
	{
		bResult = false;
	}
		
	else
	{
		bResult = true;
	}

	m_bOpened = false;
	m_hObject = 0;
	return bResult;
}

bool CNetCom::LoadDriver()
{

	return (LoadDLLAPI(hDLL));
}


void CNetCom::SetRecieveID(DWORD id)
{
	//m_DestID = id;
	switch (id)
	{
		case SCRDCUAddress:
		{
			m_DestID = ExPhyResID | id;
		}
		break;
		case StdECMPhyReqID:
		{
			m_DestID = StdECMPhyResID;
		}
		break;
		case StdTCMPhyReqID:
		{
			m_DestID = StdTCMPhyResID;
		}
		break;

	}
}


bool CNetCom::CheckRxBuf()
{
	bool bReturn;
	CString str ,strtmp;

	int iResult;
	int count=0;

	icsSpyMessage stMessages[200];
	int iNumberOfErrors;
	int iNumberOfMessages = 0;

	DWORD ErrorNumber;

	bReturn = false;
	int ncount=0;


	iResult = icsneoWaitForRxMessagesWithTimeOut(m_hObject, 5);
	if (iResult == 0)
	{
		return bReturn;
	}
	if (iResult == -1)
	{
        icsneoGetLastAPIError(m_hObject, &ErrorNumber);
		return bReturn;
	}


	iResult = icsneoGetMessages(m_hObject, stMessages, &iNumberOfMessages, &iNumberOfErrors);
	if (iResult)
	{
		while (count < iNumberOfMessages)
		{
			if ((stMessages[count].ArbIDOrHeader == m_DestID) && (stMessages[count].NetworkID == NETID_HSCAN))
			{
				EnterCriticalSection(&m_cs);
				m_RxCANBuf.ID = stMessages[count].ArbIDOrHeader;
				m_RxCANBuf.Len = stMessages[count].NumberBytesData;
				memcpy(m_RxCANBuf.Data, stMessages[count].Data, m_RxCANBuf.Len);
				m_RxLargeBuff.Add(m_RxCANBuf);
				LeaveCriticalSection(&m_cs);
				bReturn = true;
				
			}
            
			count++;
		}

	}

	return bReturn;
}



