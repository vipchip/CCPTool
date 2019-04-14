
// TEToolsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TETools.h"
#include "TEToolsDlg.h"
#include "afxdialogex.h"
#include "utility.h"
#include "Reg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma comment(lib,"Utility")


static  BYTE buf[2048];





class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTEToolsDlg dialog



CTEToolsDlg::CTEToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTEToolsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	bStart = false;
	bStopThread = false;
	bHWValid = false;
	pThread = NULL;
	m_bFileloaded = false;
	bHWOpened = false;
	m_h10SecEvent = NULL;
	m_hThread = NULL;
	m_bFileChanged = false;
	m_hRegDLL = NULL;
	

	memset(buf, 0, 2048);
	m_15765MSG.ID = 0;
	m_15765MSG.DataLen = 0;
	m_15765MSG.pData = buf;


	m_bWriteProgram = true;
	m_bWriteData = false;
	m_strLoadedFile = _T("");
	m_bRegisted = false;
//	m_b10SecTimeOutFlag = false;
}

void CTEToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list); 
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_CHECK_PROG, m_progCheckCtrl);
	DDX_Control(pDX, IDC_CHECK_DATA, m_dataCheckCtrl);
}

BEGIN_MESSAGE_MAP(CTEToolsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_UPDATESTATUS, &CTEToolsDlg::OnStatusMsg)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTEToolsDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SERCHDEVICE, &CTEToolsDlg::OnSerchDevice)
	ON_BN_CLICKED(IDC_BUTTON_OPENFILE, &CTEToolsDlg::OnBnClickedButtonOpenfile)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CTEToolsDlg::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON1, &CTEToolsDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTEToolsDlg message handlers

BOOL CTEToolsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	Chks_Init();

	pThread = new CNetCom();
	if (pThread->LoadDriver())
		InsertInfo(_T("驱动加载成功..."));
	else
		InsertInfo(_T("驱动加载失败..."));

	//pThread->SetRecieveID(0x18daf13d);
	StartNetLayerThread();

	////验证注册
	//RegisterVerify();
	////
	m_progress.SetRange(0, 100);
	m_progress.ShowWindow(SW_HIDE);
	m_progCheckCtrl.SetCheck(BST_CHECKED);
	m_dataCheckCtrl.SetCheck(BST_CHECKED);
	
	GetDlgItem(IDC_STATIC_ADRINFO)->ShowWindow(SW_HIDE);
	
	CString strExePath;
	GetModuleFileName(NULL, strExePath.GetBuffer(MAX_PATH), MAX_PATH);
	strExePath.ReleaseBuffer();
	int pos = strExePath.ReverseFind(_T('\\'));
	strExePath = strExePath.Left(pos);
    m_s19DataBlock.GetExePath(strExePath);
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTEToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTEToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTEToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTEToolsDlg::InsertInfo(const CString &str)
{
	m_list.InsertString(m_list.GetCount(), str);
	m_list.SetCurSel(m_list.GetCount() - 1);
}



void CTEToolsDlg::OnBnClickedButtonOpen()
{
	bool bresult;
	if (bHWValid)
	{

		if (!bHWOpened)
		{
			bresult = pThread->OpenPort();
			if (bresult)
			{
				InsertInfo(_T("打开端口成功..."));
				SetDlgItemText(IDC_BUTTON_OPEN, _T("关闭设备"));
				bHWOpened = true;
			}
			else
				InsertInfo(_T("打开端口失败..."));
		}
		else
		{
			bresult = pThread->ClosePort();
			//SetEvent(pThread->m_hEventKill);
			//WaitForSingleObject(CNetCom::m_hThreadDead, INFINITE);
			//Sleep(200);
			if (bresult)
			{
				InsertInfo(_T("关闭端口成功..."));
				SetDlgItemText(IDC_BUTTON_OPEN, _T("打开设备"));
				bHWOpened = false;
			}
			else
				InsertInfo(_T("关闭端口失败..."));

		}

		
	}
	else
		InsertInfo(_T("没有可用硬件..."));
		


}


void CTEToolsDlg::OnSerchDevice()
{
	// TODO: Add your control notification handler code here
	CString str;
	bool hasDevice;
	hasDevice = pThread->OnSerchDevice();
	if (hasDevice)
	{
		bHWValid = true;
		str.Format(_T(",SN: %d ..."), pThread->GetSerialNumber());
		str = _T("搜索到硬件设备") + str;
		InsertInfo(str);
	}	
	else
	{
		bHWValid = false;
		InsertInfo(_T("没有发现硬件设备..."));
	}
		
	

}


void CTEToolsDlg::StartNetLayerThread()
{
	pThread->m_pThreadParams = NULL;
	pThread->CreateThread(CREATE_SUSPENDED);
	pThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	pThread->ResumeThread();

}


void CTEToolsDlg::OnBnClickedButtonOpenfile()
{
	// TODO: Add your control notification handler code here
	
	TCHAR szFilter[] = _T("S19文件(*.s19)|*.s19|所有文件(*.*)|*.*||");

	CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);
	CString strFilePath;

	if (IDOK == fileDlg.DoModal())
	{
		strFilePath = fileDlg.GetPathName();
	
		if (m_strLoadedFile == strFilePath)
		{
			//m_bFileChanged = false;
			return;
		}
			
		m_strLoadedFile = strFilePath;
		if (m_s19DataBlock.OpenFile(m_strLoadedFile))
		{
			m_bFileChanged = true;
			InsertInfo(_T("文件打开成功..."));
			SetDlgItemText(IDC_EDIT_FILENAME, strFilePath);
		}
	}

}



void CTEToolsDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_hRegDLL != NULL)
		UnloadRegDLL(m_hRegDLL);

	if (pThread != NULL)
	{
		delete pThread;
		pThread = NULL;
	}

	CDialogEx::PostNcDestroy();
}


LRESULT CTEToolsDlg::OnStatusMsg(WPARAM wParam, LPARAM lParam)
{
	int pos = (int)lParam;
    m_progress.SetPos(pos);
	return 0;

}




bool CTEToolsDlg::SendServiceCMD(BYTE *param, WORD len)
{
	return (pThread->TxISO15765Msg(param, len));

}



bool CTEToolsDlg::ProcessService(BYTE sid)
{
	CString str,srv;
	//BYTE resCode;
	BYTE NegResCode;  //否定应答代码

	SYSTEMTIME st = { 0 };
	CString strlog;


   #ifdef _DEBUG
	srv = VerifyService(sid);
	str = _T("Start Process: ") + srv;
	InsertInfo(str);
   #endif

#ifdef _LOG
	GetLocalTime(&st);
	strlog.Format(_T("%02d:%02d:%03d,"), st.wMinute, st.wSecond, st.wMilliseconds);
	strlog += str;
	OutputDebugString(strlog + _T("\n"));
#endif

	if (SendServiceCMD(m_15765MSG.pData, m_15765MSG.DataLen) == false)
	{      
		      
		InsertInfo(_T("发送数据失败..."));

#ifdef _LOG
	GetLocalTime(&st);
	strlog.Format(_T("%02d:%02d:%03d,"), st.wMinute, st.wSecond, st.wMilliseconds);
	strlog += _T("send 15765 message failed...");
	OutputDebugString(strlog + _T("\n"));
#endif
		return false;
	}

	if (WaitForSingleObject(pThread->m_hNetLayerEvent, DEFAULTSERVERTIMEOUT) != WAIT_OBJECT_0)
	{
        #ifdef _DEBUG
		       InsertInfo(_T("Receive message timeout..."));
        #endif
#ifdef _LOG
	GetLocalTime(&st);
	strlog.Format(_T("%02d:%02d:%03d,"), st.wMinute, st.wSecond, st.wMilliseconds);
	strlog += _T("Receive message timeout...");
	OutputDebugString(strlog+_T("\n"));
#endif
		return false;
	}

	pThread->RxISO15765Msg(&m_15765MSG);

	if ((*(m_15765MSG.pData) != sid + 0x40) && (*(m_15765MSG.pData + 1) != sid)) // 意外的数据
	{

  
		InsertInfo(_T("通讯序列错误 ..."));
   
		return false;
	}


    NegResCode = *(m_15765MSG.pData + 2);

	while ((*(m_15765MSG.pData) == 0x7f) && (*(m_15765MSG.pData + 1) == sid) )
	{
		if (NegResCode==0x78)
		{ 
			//InsertInfo(_T("ecu busy ,wait.."));
            #ifdef _LOG
			GetLocalTime(&st);
			strlog.Format(_T("%02d:%02d:%03d,"), st.wMinute, st.wSecond, st.wMilliseconds);
			strlog += _T("ecu busy ,wait..");
			OutputDebugString(strlog + _T("\n"));
            #endif
		   if (WaitForSingleObject(pThread->m_hNetLayerEvent, DEFAULTSERVERTIMEOUT) != WAIT_OBJECT_0)
		   {
				 #ifdef _DEBUG
						InsertInfo(_T("Receive message timeout..."));
				 #endif
				 #ifdef _LOG
					 GetLocalTime(&st);
					 strlog.Format(_T("%02d:%02d:%03d,"), st.wMinute, st.wSecond, st.wMilliseconds);
					 strlog += _T("Receive positive message timeout..");
					 OutputDebugString(strlog + _T("\n"));
				 #endif
			 return false;
		   }

		   pThread->RxISO15765Msg(&m_15765MSG);
		   NegResCode = *(m_15765MSG.pData + 2);
		}
		else
		{
            #ifdef _DEBUG
			   str = VerifyNegResponse(NegResCode);
			   InsertInfo(srv+_T(",")+str);
            #endif
			#ifdef _LOG
				GetLocalTime(&st);
				strlog.Format(_T("%02d:%02d:%03d,"), st.wMinute, st.wSecond, st.wMilliseconds);
				strlog += srv+_T(",")+str;
				OutputDebugString(strlog + _T("\n"));
			#endif
			return false;
		}
	}

	if (*(m_15765MSG.pData) == sid + 0x40)
	{
        #ifdef _DEBUG
		    InsertInfo(srv + _T(" process done..."));
        #endif
		#ifdef _LOG
			GetLocalTime(&st);
			strlog.Format(_T("%02d:%02d:%03d,"), st.wMinute, st.wSecond, st.wMilliseconds);
			strlog += srv + _T(" process done...");
			OutputDebugString(strlog + _T("\n"));
		#endif
		return true;
	}
	else
	{

		#ifdef _LOG
			GetLocalTime(&st);
			strlog.Format(_T("%02d:%02d:%03d,15765msg,%08x,%02x,%02x,%02x,%02x---"), st.wMinute, st.wSecond, st.wMilliseconds,
				m_15765MSG.ID, *(m_15765MSG.pData), *(m_15765MSG.pData + 1), *(m_15765MSG.pData+2),
				*(m_15765MSG.pData+3));
			strlog += _T(" some error,ProcessService return false ...");
			OutputDebugString(strlog + _T("\n"));
		#endif
	    InsertInfo(_T("通讯序列错误 ..."));
		return false;
	}



}

void CTEToolsDlg::OnBnClickedButtonWrite()
{
	// TODO: Add your control notification handler code here

	if (!bHWOpened)
	{
		InsertInfo(_T("没有连接硬件，请先连接硬件"));
		return;
	}

	RegisterVerify();

	m_bWriteProgram = m_progCheckCtrl.GetCheck() ? true : false;
	m_bWriteData = m_dataCheckCtrl.GetCheck() ? true : false;

	if (m_bWriteProgram || m_bWriteData)
	    m_hThread = CreateThread(NULL, 0, CTEToolsDlg::WorkThread, this, 0, 0);
	m_progress.ShowWindow(SW_SHOW);
}

bool CTEToolsDlg::ProgramBlock(const MemObject& memobj , MemoryType mt)
{
	DWORD  maxSendBytes = 0;
	DWORD  chksum;
	CFile  file;
	DWORD  blockSeqCounter;
	DWORD  nblocks;
	DWORD  remainbytes;
	DWORD  readbytes;
	CString str;

	int    Percentage = 0;

	str.Format(_T("刷写地址 %08xh -  %08xh "), memobj.startAdress,
		memobj.startAdress+memobj.memsize-1);
	GetDlgItem(IDC_STATIC_ADRINFO)->SetWindowText(str);

	if (mt == ROM)              // rom 编程前需要先擦除
	{
		str.Format(_T("擦除 Flash, 基地址 %08xh ,大小 %08xh "), memobj.startAdress,
			memobj.memsize);
        InsertInfo(str);
		*(m_15765MSG.pData)      = 0x31;
		*(m_15765MSG.pData + 1)  = 0x01;
		*(m_15765MSG.pData + 2)  = 0xff;
		*(m_15765MSG.pData + 3)  = 0x00;
		*(m_15765MSG.pData + 4)  = 0x44;
		*(m_15765MSG.pData + 5)  = (memobj.startAdress >> 24) & 0xff;
		*(m_15765MSG.pData + 6)  = (memobj.startAdress >> 16) & 0xff;
		*(m_15765MSG.pData + 7)  = (memobj.startAdress >> 8) & 0xff;
		*(m_15765MSG.pData + 8)  =  memobj.startAdress & 0xff;
		*(m_15765MSG.pData + 9)  = (memobj.memsize >> 24) & 0xff;
		*(m_15765MSG.pData + 10) = (memobj.memsize >> 16) & 0xff;
		*(m_15765MSG.pData + 11) = (memobj.memsize >> 8) & 0xff;
		*(m_15765MSG.pData + 12) =  memobj.memsize & 0xff;
		m_15765MSG.DataLen = 13;
		if (!ProcessService(0x31))
			return false;
	}

	str.Format(_T("编程 Flash, 基地址 %08xh ,大小 %08xh ... "), memobj.startAdress,
		memobj.memsize);

	InsertInfo(str);

	*(m_15765MSG.pData)      = 0x34;
	*(m_15765MSG.pData + 1)  = 0x00;
	*(m_15765MSG.pData + 2)  = 0x44;
	*(m_15765MSG.pData + 3)  = (memobj.startAdress >> 24) & 0xff;
	*(m_15765MSG.pData + 4)  = (memobj.startAdress >> 16) & 0xff;
	*(m_15765MSG.pData + 5)  = (memobj.startAdress >> 8) & 0xff;
	*(m_15765MSG.pData + 6)  =  memobj.startAdress & 0xff;
	*(m_15765MSG.pData + 7)  = (memobj.memsize >> 24) & 0xff;
	*(m_15765MSG.pData + 8)  = (memobj.memsize >> 16) & 0xff;
	*(m_15765MSG.pData + 9)  = (memobj.memsize >> 8) & 0xff;
	*(m_15765MSG.pData + 10) =  memobj.memsize & 0xff;
	m_15765MSG.DataLen = 11;
	if (!ProcessService(0x34))
		return false;

	maxSendBytes  = (*(m_15765MSG.pData + 2) << 8 | *(m_15765MSG.pData + 3));
	if (!file.Open(memobj.binfile, CFile::modeRead | CFile::typeBinary | CFile::shareExclusive))
	{
		InsertInfo(_T("读取文件 ") + memobj.binfile+_T(" 失败..."));
		file.Close();
		return false;
	}

	Chks_Init();
	if (memobj.memsize <= maxSendBytes-2) //send in one block
	{
		*(m_15765MSG.pData) = 0x36;
		*(m_15765MSG.pData + 1) = 0x01;
		file.Read(m_15765MSG.pData + 2, memobj.memsize);
		Chks_Process(m_15765MSG.pData + 2, memobj.memsize);
		chksum = Chks_Finish();
		m_15765MSG.DataLen = memobj.memsize + 2;
		if (!ProcessService(0x36))
			return false;
	}
	else                                  //send in mulit-block 
	{
		blockSeqCounter = 1;
		nblocks = memobj.memsize % (maxSendBytes - 2) ? memobj.memsize / (maxSendBytes-2)+1
			      : memobj.memsize / (maxSendBytes - 2);
		
		for (DWORD i = 0; i < nblocks; i++)
		{
			*(m_15765MSG.pData) = 0x36;
			*(m_15765MSG.pData + 1) = blockSeqCounter;
			remainbytes = memobj.memsize - i*(maxSendBytes - 2);
			readbytes = remainbytes >=(maxSendBytes - 2) ? (maxSendBytes - 2) : remainbytes;
			file.Read(m_15765MSG.pData + 2, readbytes);
			Chks_Process(m_15765MSG.pData + 2, readbytes);
			m_15765MSG.DataLen = readbytes+2;
			
		    Percentage = 100 - 100 * remainbytes / memobj.memsize;
			SendMessage(WM_UPDATESTATUS, 0, (LPARAM)Percentage);

			if (!ProcessService(0x36))
				return false;
			blockSeqCounter++;
			if (blockSeqCounter >= 256)
				blockSeqCounter = 0;
		}
		chksum = Chks_Finish();
	}

	Percentage = 100;
	SendMessage(WM_UPDATESTATUS, 0, (LPARAM)Percentage);

	file.Close();
//transfer eixt
	*(m_15765MSG.pData) = 0x37;
	m_15765MSG.DataLen = 1;
	if (!ProcessService(0x37))
		return false;

	str.Format(_T("比较校验和, 基地址 %08xh ,大小 %08xh "), memobj.startAdress,
		memobj.memsize);
    InsertInfo(str);

	InsertInfo(_T("验证中 ..."));
//checksum compare
	*(m_15765MSG.pData) = 0x31;
	*(m_15765MSG.pData + 1) = 0x01;
	*(m_15765MSG.pData + 2) = 0xf0;
	*(m_15765MSG.pData + 3) = 0x01;
	*(m_15765MSG.pData + 4) = (chksum >> 24) & 0xff;
	*(m_15765MSG.pData + 5) = (chksum >> 16) & 0xff;
	*(m_15765MSG.pData + 6) = (chksum >> 8) & 0xff;
	*(m_15765MSG.pData + 7) = chksum & 0xff;
	m_15765MSG.DataLen = 8;
	if (!ProcessService(0x31))
		return false;
	InsertInfo(_T("验证校验和通过 ! "));
//requestRoutineResults
	*(m_15765MSG.pData) = 0x31;
	*(m_15765MSG.pData + 1) = 0x03;
	*(m_15765MSG.pData + 2) = 0xf0;
	*(m_15765MSG.pData + 3) = 0x01;
	m_15765MSG.DataLen = 4;
	if (!ProcessService(0x31))
		return false;

	GetDlgItem(IDC_STATIC_ADRINFO)->SetWindowText(_T("刷写完成!"));
	return true;
}
void CTEToolsDlg::ECUFlashWrite()
{
	MemObject memobj;
	int Percentage = 0;
	SendMessage(WM_UPDATESTATUS, 0, (LPARAM)Percentage);
	GetDlgItem(IDC_STATIC_ADRINFO)->SetWindowText(_T(""));

	InsertInfo(_T("开始通讯..."));

#pragma region SessCtrl03 
	InsertInfo(_T("设置ECU扩展模式..."));
	*(m_15765MSG.pData) = 0x10;
	*(m_15765MSG.pData + 1) = 0x03;
	m_15765MSG.DataLen = 2;

	if (!ProcessService(0x10))
	{
		InsertInfo(_T("设置ECU扩展模式失败..."));
		return;
	}

#pragma endregion SessCtrl03

#pragma region CloseDTCService 
	InsertInfo(_T("关闭DTC服务..."));
	*(m_15765MSG.pData) = 0x85;
	*(m_15765MSG.pData + 1) = 0x02;
	m_15765MSG.DataLen = 2;
	if (!ProcessService(0x85))
	{
		InsertInfo(_T("关闭DTC服务失败..."));
		return;
	}
	
#pragma endregion CloseDTCService

#pragma region CommunicationControl
	*(m_15765MSG.pData) = 0x28;
	*(m_15765MSG.pData + 1) = 0x03;
	*(m_15765MSG.pData + 2) = 0x01;
	m_15765MSG.DataLen = 3;
	if (!ProcessService(0x28))
		return;
#pragma endregion CommunicationControl

#pragma region Readfingeprint
	InsertInfo(_T("读取指纹..."));
	*(m_15765MSG.pData) = 0x22;
	*(m_15765MSG.pData + 1) = 0xf1;
	*(m_15765MSG.pData + 2) = 0x84;
	m_15765MSG.DataLen = 3;
	if (!ProcessService(0x22))
	{
		InsertInfo(_T("读取指纹失败..."));
		return;
	}
#pragma endregion Readfingerprint

#pragma region ProgramRequest
	InsertInfo(_T("设置ECU编程模式..."));
	*(m_15765MSG.pData) = 0x10;
	*(m_15765MSG.pData + 1) = 0x02;
	m_15765MSG.DataLen = 2;
	if (!ProcessService(0x10))
	{
		InsertInfo(_T("设置ECU编程模式失败..."));
		return;
	}
		
#pragma endregion ProgramRequest

#pragma region Wait10Sec
	InsertInfo(_T("等待10秒..."));
	int count = 6;
	CString str;
	m_h10SecEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	while (count--)
	{
		WaitForSingleObject(m_h10SecEvent, 2000);
		*(m_15765MSG.pData) = 0x3e;
		*(m_15765MSG.pData + 1) = 0x00;
		m_15765MSG.DataLen = 2;
		if (!ProcessService(0x3e))
		{
			InsertInfo(_T("等待错误..."));
			return;
		}
		str.Format(_T("等待 2 秒, %d%%..."),100-count*20);
		InsertInfo(str);
	}
	
#pragma endregion Wait10Sec

#pragma region Seed&Key
	InsertInfo(_T("安全验证..."));
	DWORD seed, key;
	*(m_15765MSG.pData) = 0x27;
	*(m_15765MSG.pData + 1) = 0x03;
	m_15765MSG.DataLen = 2;
	if (!ProcessService(0x27))
		return;

	seed = (*(m_15765MSG.pData + 2)) << 24 | (*(m_15765MSG.pData + 3)) << 16
		| (*(m_15765MSG.pData + 4)) << 8 | (*(m_15765MSG.pData + 5));
	key = Get_Key(seed);
	*(m_15765MSG.pData) = 0x27;
	*(m_15765MSG.pData + 1) = 0x04;
	*(m_15765MSG.pData + 2) = (key >> 24) & 0xff;
	*(m_15765MSG.pData + 3) = (key >> 16) & 0xff;
	*(m_15765MSG.pData + 4) = (key >> 8) & 0xff;
	*(m_15765MSG.pData + 5) = key & 0xff;
	m_15765MSG.DataLen = 6;
	if (!ProcessService(0x27))
	{
		InsertInfo(_T("安全校验失败，访问ECU被拒绝..."));
		return;
	}

#pragma endregion Seed&Key


#pragma region Writefingerprint
	/*
	BYTE toolID = 0x01;
	BYTE testerSN[6] = { 0 };
	BYTE blocknum = 03;
	BYTE year = 0x15;
	BYTE mon = 0x06;
	BYTE date = 0x01;
	*(m_15765MSG.pData) = 0x2e;
	*(m_15765MSG.pData + 1) = 0xf1;
	*(m_15765MSG.pData + 2) = 0x84;
	*(m_15765MSG.pData + 3) = toolID;
	memcpy(m_15765MSG.pData + 4, testerSN, 6);
	*(m_15765MSG.pData + 10) = blocknum;
	*(m_15765MSG.pData + 11) = year;
	*(m_15765MSG.pData + 12) = mon;
	*(m_15765MSG.pData + 13) = date;
	m_15765MSG.DataLen = 14;
	*/
	InsertInfo(_T("写入指纹..."));
	BYTE fingerprin[11] = { 0x01,0xFF,0x00,0x01,0x00,0x00,0x02,0x03,0x0C,0x07,0x05};
	*(m_15765MSG.pData) = 0x2e;
	*(m_15765MSG.pData + 1) = 0xf1;
	*(m_15765MSG.pData + 2) = 0x84;
	memcpy(m_15765MSG.pData + 3, fingerprin, 11);
	m_15765MSG.DataLen = 14;
	if (!ProcessService(0x2e))
	{
		InsertInfo(_T("写入指纹失败..."));
		return;
	}

#pragma endregion Writefingerprint

	if (m_bWriteProgram)
	{
		GetDlgItem(IDC_STATIC_ADRINFO)->ShowWindow(SW_SHOW);

#pragma region WriteFlashDriver
		InsertInfo(_T("刷写flash驱动..."));
		memobj = m_s19DataBlock.m_memFlashDrvObj;
		if (!ProgramBlock(memobj, RAM))
		{
			InsertInfo(_T("刷写flash驱动失败..."));
			return;
		}

#pragma endregion WriteFlashDriver

#pragma region  AppProgram

		GetDlgItem(IDC_STATIC_ADRINFO)->ShowWindow(SW_SHOW);
		InsertInfo(_T("刷写应用程序..."));
		memobj = m_s19DataBlock.m_memAppObj;
		if (!ProgramBlock(memobj, ROM))
		{
			InsertInfo(_T("刷写应用程序失败..."));
			return;
		}
		//GetDlgItem(IDC_STATIC_ELPTIME)->ShowWindow(SW_HIDE);
#pragma endregion AppProgram
	}

	if (m_bWriteData && m_bRegisted)
	 {
#pragma region WriteFlashDriver
		 memobj = m_s19DataBlock.m_memFlashDrvObj;
		 if (!ProgramBlock(memobj, RAM))
			 return;
#pragma endregion WriteFlashDriver


#pragma region WriteCaliData
		 InsertInfo(_T("刷写标定数据..."));

#ifdef _LOG
		 CString strlog;
		 SYSTEMTIME st;
		 GetLocalTime(&st);
		 strlog.Format(_T("%02d:%02d:%03d,"), st.wMinute, st.wSecond, st.wMilliseconds);
		 strlog += _T(" 刷写标定数据 ...");
		 OutputDebugString(strlog + _T("\n"));
#endif
		 m_s19DataBlock.RelocateCalMemoryAdress();
		 memobj = m_s19DataBlock.m_memDataObj;
		 if (!ProgramBlock(memobj, ROM))
		 {
			 InsertInfo(_T("刷写标定数据失败..."));
#ifdef _LOG
			 GetLocalTime(&st);
			 strlog.Format(_T("%02d:%02d:%03d,"), st.wMinute, st.wSecond, st.wMilliseconds);
			 strlog += _T(" 刷写标定数据失败 ...");
			 OutputDebugString(strlog + _T("\n"));
#endif
			 return;
		 }
#pragma endregion WriteCaliData

	 }
#pragma region CheckDependencies
	 *(m_15765MSG.pData) = 0x31;
	 *(m_15765MSG.pData + 1) = 0x01;
	 *(m_15765MSG.pData + 2) = 0xff;
	 *(m_15765MSG.pData + 3) = 0x01;
	 m_15765MSG.DataLen = 4;

	 if (!ProcessService(0x31))
		 return;
#pragma endregion CheckDependencies

#pragma region ECUReset
	 InsertInfo(_T("复位ECU..."));
	 *(m_15765MSG.pData) = 0x11;
	 *(m_15765MSG.pData+1) = 0x01;
	 m_15765MSG.DataLen = 2;
	 if (!ProcessService(0x11))
	 {
		 InsertInfo(_T("复位ECU失败..."));
		 return;
	 }
#pragma endregion ECUReset

	 InsertInfo(_T("刷写完成！"));

	 

}

//void CTEToolsDlg::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: Add your message handler code here and/or call default
//	static int count = 0;
//	*(m_15765MSG.pData) = 0x3e;
//	*(m_15765MSG.pData + 1) = 0x00;
//    m_15765MSG.DataLen = 2;
//	ProcessService(0x3e);
//	count++;
//	if (count >= 5)
//	{
//		count = 0;
//		SetEvent(m_h10SecEvent);
//		KillTimer(m_id10SecTimer);
//	}
//	CDialogEx::OnTimer(nIDEvent);
//}

DWORD WINAPI CTEToolsDlg::WorkThread(LPVOID lpParam)
{
	CTEToolsDlg *pDlg = (CTEToolsDlg*)lpParam;
	pDlg->WorkThreadFunc(NULL);
	return 0;

}
DWORD  WINAPI CTEToolsDlg::WorkThreadFunc(LPVOID lpParam)
{
	CString str;
	bool bResult;

	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_OPENFILE)->EnableWindow(false);
	InsertInfo(_T("==================================================================="));
	if (m_bFileChanged)
	{
		InsertInfo(_T("转换文件: S19 -> bin..."));
		if (!m_s19DataBlock.CovertS19Record2Binary())
		{
			InsertInfo(_T("提取文件失败..."));
			GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_OPENFILE)->EnableWindow(true);
			return 0;
		}

		bResult = m_s19DataBlock.SaveAllBin();
		
		if (!bResult)
		{
			InsertInfo(_T("转换文件失败..."));
			m_s19DataBlock.ReleaseBuffer();
			GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_OPENFILE)->EnableWindow(true);
			return 0;
		}

		m_s19DataBlock.SaveLastLoadFlie();
		InsertInfo(_T("转换完成..."));
		str.Format(_T("模块版本，  V%d.%d.%d "), CS19RecStream::CS19REC_MAJOR_VERSION,
			CS19RecStream::CS19REC_MINOR_VERSION, CS19RecStream::CS19REC_PATCH_VERSION);
		InsertInfo(str);
		m_bFileChanged = false;
	}
	ECUFlashWrite();

	InsertInfo(_T("==================================================================="));
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_OPENFILE)->EnableWindow(true);


	return 0;
}

void CTEToolsDlg::RegisterVerify()
{
	m_bRegisted = true;
	/*
	if (!LoadRegDLL(m_hRegDLL))
	{
		MessageBox(_T("加载 AuthDIL.dll 失败！"));
		m_bRegisted = false;
		//abort();
	}

	int cansn = pThread->GetSerialNumber();

	if (cansn == 0)
	{
		m_bRegisted = false;
	}
	LockCANDev(cansn);

	BOOL bRegd = CheckOut();

	if (bRegd)
	{
		m_bRegisted = true;
	}
	else
	{
		m_bRegisted = false;
		CheckIn();
	}

	if (m_bRegisted==false)
		InsertInfo(_T("软件未注册，只能刷写程序"));
	*/
}

void CTEToolsDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	// 发送消息示例

    //1,填充消息
	*(m_15765MSG.pData) = 0x01;    
	*(m_15765MSG.pData + 1) = 0x00;
	m_15765MSG.DataLen = 2;

	//2，将消息发送出去，直到网络层做出响应
	if (!ProcessService(0x01))
	{
		InsertInfo(_T("发送15031 服务01 失败..."));
		return;
	} 
	//if没有返回，网络层成功过收到服务01的响应
	//3，解析收到的数据
}
