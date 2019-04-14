#pragma once

#define  MAXSRECBYTES 32




typedef enum tagFlashBlockType
{
	flashdriver = 0,
	appProgram,
	appData
}FlashBlockType;

typedef struct tagSRecDataRec   // ����s-record ����֧��S3��¼��ÿ��32���ֽ�
{
	DWORD  LoadAddr;
	BYTE   Data[MAXSRECBYTES];
	BYTE   NumBytes;

} SRecDataRec;

typedef struct tagMemBlock
{
	SRecDataRec *pMemBlock;            //�ڴ��
	DWORD        memSize;              //�ڴ��С���ֽ���
	DWORD        itemRec;              //s-record ��¼����
	DWORD        itermBlockAdress[16]; //�ڴ����ʼ��ַ
	WORD         blocksize;             //�ڴ����

}MemBlock;



class MemObject
{
public:
	DWORD         startAdress;
	DWORD         memsize;
	CString       binfile;

	MemObject()
	{
		startAdress = 0;
		memsize = 0;
		binfile = _T("");
	}
	MemObject(DWORD sa, DWORD ea, CString bin) :startAdress(sa), memsize(ea), binfile(bin)
	{}

	MemObject(const MemObject& objRef)
	{
		startAdress = objRef.startAdress;
		memsize = objRef.memsize;
		binfile = objRef.binfile;

	}
	MemObject& operator=(MemObject& objRef)
	{
		startAdress = objRef.startAdress;
		memsize = objRef.memsize;
		binfile = objRef.binfile;

		return *this;
	}
};

class CS19RecStream
{
private:
	BOOL m_bFileOpened;
public:
	CS19RecStream();
	~CS19RecStream();

	MemBlock      m_memblock;
	SRecDataRec  *m_pSRecData;
	MemObject     m_memFlashDrvObj;
	MemObject     m_memAppObj;
	MemObject     m_memDataObj;

	CStdioFile    m_RdFile;
	DWORD         m_nblocks;
	DWORD         m_rlAdress;
	CString       m_strOpenFile;
	CString       m_strLastLoadedFile;
	CString       m_strCurrDirectory;
	CString       m_strExeDirectory;
	bool          m_bUseLastCfg;

	static  int CS19REC_MAJOR_VERSION ;
	static  int CS19REC_MINOR_VERSION ;
	static  int CS19REC_PATCH_VERSION ;

	int   CovertS19Record2Binary();
	BOOL  OpenFile(const CString& filename);
	void  unicode2ansi(char *dest,const CString &src,DWORD size);
	BYTE  char2Hex(const char& ch);
	bool  SavetoBin(FlashBlockType fbt);
	void  ReleaseBuffer();
	void  RelocateCalMemoryAdress();
	void  GetExePath(const CString& path);
	void  GetLastLoadedFile();
	void  SaveLastLoadFlie();
	bool  SaveAllBin();
	static void  UTF8ToUnicode(CString& strUtf8);
	static int   GetVersion();
};

