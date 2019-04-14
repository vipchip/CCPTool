
// TEToolsDlg.h : header file
//
#pragma once


#include "afxwin.h"
#include "TextProgressCtrl.h"
#include "NetCom.h"
#include "afxcmn.h"
#include "S19RecStream.h"
#include "UDSDef.h"

#define   DEFAULTSERVERTIMEOUT   5500


#define   REQUESTTIMEOUT                0x00
#define   OK                            0x01
#define   INVALIDERESPOND               0x02


//服务ID
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

//否定应答代码定义
//#define    GENERALREJECT                                0x10
//#define    SERVICENOTSUPPORTED                          0x11 
//#define    SUBFUNCTIONNOTSUPPORTED_INVALIDFORMAT        0x12
//#define    BUS_REPEATREQUEST                            0x21
//#define    CONDITIONSNOTCORRECTORREQUESTSEQUENCEERROR   0x22
//#define    REQUESTCORRECTLYRECEIVED_RESPONSEPENDING     0x78




#define    WM_UPDATESTATUS    (WM_USER+100)


typedef enum tagMemoryType{RAM =0,ROM}MemoryType;
extern CStdioFile glog;

// CTEToolsDlg dialog
class CTEToolsDlg : public CDialogEx
{
// Construction
public:
	CTEToolsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TETOOLS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnStatusMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:

	
	HANDLE        m_hThread;
	HANDLE        m_hEvent;
	HANDLE        m_h10SecEvent;
	HINSTANCE     m_hRegDLL;

	bool          bStopThread;
	bool          bStart;
	bool          bHWValid;
	bool          bHWOpened;
	bool          m_bRegisted;
	
	CNetCom*      pThread;
    CListBox      m_list;
	CTextProgressCtrl m_progress;
	
	bool          m_bFileloaded;
	CS19RecStream m_s19DataBlock;
	bool          m_bFileChanged;

	ISO16765Msg   m_15765MSG;
	int           m_id10SecTimer;
	CString       m_strLoadedFile;
	//HANDLE        m_hThread;

	CButton      m_progCheckCtrl;
	CButton      m_dataCheckCtrl;
	bool         m_bWriteProgram;
	bool         m_bWriteData;


	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnSerchDevice();

	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedButtonOpenfile();
	virtual void PostNcDestroy();


	void InsertInfo(const CString &str);
	void StartNetLayerThread();
	bool SendServiceCMD(BYTE *param, WORD len);
	bool ProcessService(BYTE sid);
	void ECUFlashWrite();
	bool ProgramBlock(const MemObject& memobj, MemoryType mt);
	
	static DWORD WINAPI WorkThread(LPVOID lpParam);
	DWORD  WINAPI WorkThreadFunc(LPVOID lpParam);
	void   RegisterVerify();


	afx_msg void OnBnClickedButton1();
};
