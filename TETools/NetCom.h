#pragma once

#include "icsneo40DLLAPI.h"

//��չ֡J1939 ID
#define ExFuncReqID       0x18DB33F1
#define ExPyhReqID        0x18DA00F1 
#define ExPhyResID        0x18DAF100

//11λID
#define StdFuncReqID      0x7DF
#define StdECMPhyReqID    0x7E0
#define StdECMPhyResID    0x7E8
#define StdTCMPhyReqID    0x7E1
#define StdTCMPhyResID    0x7E9

#define SCRDCUAddress   0x3D
#define RxMaxByte       256
#define MAXMESSAGELEN   4096

#define DIAGCANBAUDRATE 500000


/*����Ĭ�Ϸ��͡����ճ�ʱ��Э��涨�˶��ֳ�ʱ�������ֳ�ʱ��Ϊ1000ms��
  Э����������㳬ʱ������+50%���ʺϲ����ֳ�ʱ��ͳһ����Ϊ1500ms
  �����ر��������ӻ��޸Ĵ˶�ʱ����*/
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
	WORD  DataLen;   //15765�������ݳ���
	BYTE  *pData;
}ISO16765Msg;

typedef struct tagISO15765MsgState
{
	DWORD ID;
	BYTE  Type;      //0 - SF;1-FF; 2- CF ; 3 - FC ,0xff - ��Ч
	BYTE  DelayTime;
	BYTE  IsRxTx;    //0- rx;1-tx
	BYTE  St_ok;     //0 - transmiting;1 - trans finished
	BYTE  bWait;     //0 - �������� ��1-��ʱ����
	BYTE  CurFrameNo;//��ǰ֡���
	WORD  RecBytes;  //�ѽ����ֽ���

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
	HANDLE m_hNetLayerEvent;   //�������Ϣ�¼�
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

    //ͨ��15765����һ����Ϣ��data��Ӧ�ò����Ϣ��size��data��ָ������Ϣ����
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


