#pragma once

#include "icsneo40DLLAPI.h"

//扩展帧J1939 ID
#define ExFuncReqID       0x18DB33F1
#define ExPyhReqID        0x18DA00F1 
#define ExPhyResID        0x18DAF100

//11位ID
#define StdFuncReqID      0x7DF
#define StdECMPhyReqID    0x7E0
#define StdECMPhyResID    0x7E8
#define StdTCMPhyReqID    0x7E1
#define StdTCMPhyResID    0x7E9

#define SCRDCUAddress   0x3D
#define RxMaxByte       256
#define MAXMESSAGELEN   4096

#define DIAGCANBAUDRATE 500000


/*网络默认发送、接收超时，协议规定了多种超时，但各种超时均为1000ms，
  协议允许网络层超时检测最大到+50%，故合并各种超时并统一设置为1500ms
  如需特别需求，增加或修改此定时参数*/
#define DEFAULTTIMEOUT  1500    

typedef enum eMapingAdrMode
{
	stdMapingAdrMode =0,
	ExdMapingAdrMode 
}MapingAdrMode;


typedef struct tagCAN_Msg
{
	DWORD  ID;
	BYTE   Data[8];
	BYTE   Len;
}CAN_Msg;


extern byte DataBuf[MAXMESSAGELEN];

typedef struct tagISO15765Msg
{
	DWORD ID;
	WORD  DataLen;   //15765接受数据长度
	BYTE  *pData;
}ISO16765Msg;

typedef struct tagISO15765MsgState
{
	DWORD ID;
	BYTE  Type;      //0 - SF;1-FF; 2- CF ; 3 - FC ,0xff - 无效
	BYTE  DelayTime;
	BYTE  IsRxTx;    //0- rx;1-tx
	BYTE  St_ok;     //0 - transmiting;1 - trans finished
	BYTE  bWait;     //0 - 立即发送 ，1-延时发送
	BYTE  CurFrameNo;//当前帧编号
	WORD  RecBytes;  //已接收字节数

}ISO15765MsgState;

typedef struct tagISO15765NETFLOWCONTROL
{
	BYTE Direct;                //0 - read; 1 -write ; 0xff - invalide
	BYTE FlowStatus;
	BYTE BlockSize;
	WORD STmin;                //ms
}ISO15765NetFC;



// CNetCom

class CNetCom : public CWinThread
{
	DECLARE_DYNCREATE(CNetCom)

protected:
	//CNetCom();           // protected constructor used by dynamic creation
	

public:
	CNetCom();
	virtual ~CNetCom();
	
	virtual BOOL InitInstance();
	virtual void Delete();

protected:
	DECLARE_MESSAGE_MAP()
public:
	HANDLE m_hTxTimer ;
	HANDLE m_hRxTimer;
	HANDLE m_hEventKill;
	HANDLE m_hNetLayerEvent;   //网络层消息事件
	HANDLE m_hFCEvent;

	static HANDLE m_hThreadDead;
	CAN_Msg  m_RxCANBuf;
	//CAN_Msg  m_TxCANBuf;

	NeoDevice     m_Device;
	icsSpyMessage m_icsMsg;
	HINSTANCE     hDLL;
	int           m_hObject;

	ISO16765Msg      m_RxMsg;
	ISO16765Msg      m_TxMsg;
	ISO15765MsgState m_TxRxStatus;
	ISO15765NetFC    m_NetFC;

	CArray <CAN_Msg, CAN_Msg&>       m_RxLargeBuff;
	DWORD            m_DestID;
	CRITICAL_SECTION m_cs;
	
	bool   m_bOpened;

	int  TxCANFrame(const CAN_Msg& txmsg);
	void  RxCANFrame();

    //通过15765发送一个消息，data，应用层的消息，size，data所指定的消息长度
	bool TxISO15765Msg(BYTE *data, WORD size, MapingAdrMode mode = ExdMapingAdrMode, DWORD destAdress = SCRDCUAddress);
	void RxISO15765Msg(ISO16765Msg *msg);

	void FlowControl( BYTE direct, BYTE flowstatus,BYTE blocksize,WORD STmin);

	void ISO15765RxRoutine();
	void ISO15765TxRoutine();

	bool LoadDriver();
	bool OnSerchDevice();
	bool OpenPort();
	bool ClosePort();
	int  GetSerialNumber();
	bool CheckRxBuf();
	void SetRecieveID(DWORD id = SCRDCUAddress);

	void KillThread();

};


