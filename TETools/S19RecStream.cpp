#include "stdafx.h"
#include "S19RecStream.h"
#include "tinyxml2.h"

#include <iostream>  
using namespace std;


#pragma comment(lib,"tinyxml2")
#define ERROR_BADDATA       -1
#define ERROR_FILENOTOPEN   0
#define ERROR_OK            1

static TCHAR szSubFolder[] = _T("\\datas");
static TCHAR szAppFileName[] = _T("\\Application.bin");
static TCHAR szCalFileName[] = _T("\\Caldata.bin");


int CS19RecStream::CS19REC_MAJOR_VERSION = 1;
int CS19RecStream::CS19REC_MINOR_VERSION = 0;
int CS19RecStream::CS19REC_PATCH_VERSION = 0;

CS19RecStream::CS19RecStream()
{
	memset((BYTE *)&m_memblock, 0, sizeof(MemBlock));
	m_bFileOpened = false;
	m_pSRecData   = NULL;
	m_memblock.pMemBlock = NULL;
	m_pSRecData = NULL;
	m_strOpenFile = _T("");

	//m_memFlashDrvObj.startAdress = 0x40005000;
	//m_memFlashDrvObj.memsize = 0x01f8;
	//m_memFlashDrvObj.binfile = _T("FlashDriver.bin");

	//m_memAppObj.startAdress = 0x00018000;
	//m_memAppObj.memsize = 0x0a8000;
	//m_memAppObj.binfile = _T("Application.bin");

	//m_memDataObj.startAdress = 0x8000;
	//m_memDataObj.memsize = 0x10000;
	//m_memDataObj.binfile = _T("Caldata.bin");
	m_bUseLastCfg = false;
}


CS19RecStream::~CS19RecStream()
{
	ReleaseBuffer();
}

void  CS19RecStream::GetExePath(const CString& path)
{
	m_strExeDirectory = path;

	//if (OpenFile(strFilePath))
	//{
	//	CovertS19Record2Binary();

	//	DWORD dwAttr = GetFileAttributes(m_strCurrDirectory + szSubFolder);
	//	if (dwAttr == 0xFFFFFFFF)
	//		CreateDirectory(m_strCurrDirectory + szSubFolder, NULL);
	//	SavetoBin(m_strCurrDirectory + szSubFolder + szAppFileName, appProgram);
	//	SavetoBin(m_strCurrDirectory + szSubFolder + szCalFileName, appData);
	//}
}

void  CS19RecStream::GetLastLoadedFile()
{
	GetCurrentDirectory(MAX_PATH, m_strCurrDirectory.GetBuffer(MAX_PATH));
	//assert(!m_strCurrDirectory.IsEmpty());
	//调整当前目录位为exe目录
	SetCurrentDirectory(m_strExeDirectory);
	CString  str;
	const char cfgname[] = "DefaultCfg.xml";

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(cfgname)!= 0 )
		return;

	tinyxml2::XMLElement  *subElem = doc.FirstChildElement()->FirstChildElement("LoadedFile");
	tinyxml2::XMLElement  *sub01 = subElem->FirstChildElement("FilePath");
	const tinyxml2::XMLAttribute *pAttr = sub01->FirstAttribute();
	m_strLastLoadedFile = pAttr->Value();
	//utf-8 to unicode
	//int dwUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, m_strLastLoadedFile, -1, NULL, 0);
	UTF8ToUnicode(m_strLastLoadedFile);
	subElem = sub01;
	sub01 = subElem->FirstChildElement()->FirstChildElement("appArea");
	str = sub01->Attribute("app");
	m_memAppObj.binfile = m_strExeDirectory+_T("\\") + str;
	m_memAppObj.startAdress = sub01->UnsignedAttribute("startadress");
	m_memAppObj.memsize = sub01->UnsignedAttribute("size");

	sub01 = subElem->FirstChildElement()->FirstChildElement("dataArea");
	str= sub01->Attribute("data");
	m_memDataObj.binfile = m_strExeDirectory + _T("\\") + str;
	m_memDataObj.startAdress = sub01->UnsignedAttribute("startadress");
	m_memDataObj.memsize = sub01->UnsignedAttribute("size");
	m_rlAdress = sub01->UnsignedAttribute("relocatedAdress");


	subElem = doc.FirstChildElement()->FirstChildElement("FlashDriver");
	sub01 = subElem->FirstChildElement("Driver");
	str= sub01->Attribute("path");
	m_memFlashDrvObj.binfile = m_strExeDirectory + _T("\\") + str;
	m_memFlashDrvObj.startAdress = sub01->UnsignedAttribute("startadress");
	m_memFlashDrvObj.memsize = sub01->UnsignedAttribute("size");
	
	doc.SaveFile(cfgname);

	SetCurrentDirectory(m_strCurrDirectory);
}

BOOL  CS19RecStream::OpenFile(const CString& filename)
{

	 GetLastLoadedFile();

	 if (filename == m_strLastLoadedFile)
	 {
		 m_bUseLastCfg = true;
		 m_strOpenFile = m_strLastLoadedFile;
		 m_bFileOpened = true;
		 return m_bFileOpened;
	 }


	ReleaseBuffer();                               //加载新文件前，要先释放旧文件申请的内存空间

	memset((BYTE *)&m_memblock, 0, sizeof(MemBlock));
	m_bFileOpened = m_RdFile.Open(filename, CFile::modeRead | CFile::typeText | CFile::shareDenyWrite);
	
	if (m_bFileOpened)
	{
		m_strOpenFile = filename;
	}

	return m_bFileOpened;
}


//此函数只保证转换的字符串为英文字符，超出范围的字符转换可能不正确
void  CS19RecStream::unicode2ansi(char *dest, const CString &src, DWORD size)
{
	DWORD i;
	for ( i = 0; i < size; i++)
	{
		*(dest + i) = src.GetAt(i);
	}
}

BYTE  CS19RecStream::char2Hex(const char& ch)
{
	BYTE hexbyte = 0;
	if (ch >= '0' && ch <= '9')
		hexbyte = ch - '0';
	if (ch >= 'A' && ch <= 'F')
		hexbyte = ch - 'A'+10;
	return hexbyte;
}

int  CS19RecStream::CovertS19Record2Binary()
{
	char chtemp[128];
	DWORD lines;
	CString str;
	BYTE hex;
	BYTE recLen;
	BYTE chkSum;
	BYTE recChkSum;
	int  i;
	BYTE adressLen =4;
	BYTE tmbyte;

	if (m_bUseLastCfg)
		return ERROR_OK;

	bool bRecord = false;
	if (!m_bFileOpened)
		return ERROR_FILENOTOPEN;

	lines = 0;


	//测试文件中所含SRecord的数量，以确定内存分配大小
	while (m_RdFile.ReadString(str)) //直到文件结束
	{
		unicode2ansi(chtemp, str, str.GetLength());

		if (chtemp[0] == 'S'&& chtemp[1] == '0')
		{
			continue;
		}
		if (chtemp[0] == 'S'&&chtemp[1] == '5')
		{
			continue;
		}
		if (chtemp[0] == 'S'&&chtemp[1] == '7')
		{
			continue;
		}

		lines++;
	}


	//分配内存，此处应该捕获一个异常，在程序内存分配失败时做相应处理
	//暂时不做处理
	m_pSRecData = new SRecDataRec[lines];   
	m_memblock.pMemBlock = m_pSRecData;
	m_memblock.itemRec = lines;
	lines = 0;

    //开始数据转换
	m_memblock.blocksize = 0;

	m_RdFile.SeekToBegin();
	while (m_RdFile.ReadString(str))
	{
		unicode2ansi(chtemp, str, str.GetLength());

		if (chtemp[0] == 'S'&& chtemp[1] == '0')
		{
			bRecord = true;
			continue;                               //s0跳过
		}
		if (chtemp[0] == 'S'&&chtemp[1] == '5')
		{
			bRecord = true;
			continue;                               //s5跳过
		}
		if (chtemp[0] == 'S'&&chtemp[1] == '7')
		{
			continue;                                //s7跳过
		}


	    //只处理S3记录

		hex = char2Hex(chtemp[2]);
		recLen = hex << 4;
		hex = char2Hex(chtemp[3]);
		recLen += hex;
		chkSum = recLen;
		(m_memblock.pMemBlock + lines)->NumBytes = recLen - 5;  //四字节地址和一字节的校验和长度

		hex = (char2Hex(chtemp[4]));
		tmbyte = hex<<4;
		hex = (char2Hex(chtemp[5]));
		tmbyte += hex;
        chkSum += tmbyte;
        (m_memblock.pMemBlock + lines)->LoadAddr = tmbyte << 24;

		hex = (char2Hex(chtemp[6]));
		tmbyte = hex << 4;
		hex = (char2Hex(chtemp[7]));
		tmbyte += hex;
		chkSum += tmbyte;
		(m_memblock.pMemBlock + lines)->LoadAddr += tmbyte << 16;

		hex = (char2Hex(chtemp[8]));
		tmbyte = hex << 4;
		hex = (char2Hex(chtemp[9]));
		tmbyte += hex;
		chkSum += tmbyte;
		(m_memblock.pMemBlock + lines)->LoadAddr += tmbyte << 8;

		hex = (char2Hex(chtemp[10]));
		tmbyte = hex << 4;
		hex = (char2Hex(chtemp[11]));
		tmbyte += hex;
		chkSum += tmbyte;
		(m_memblock.pMemBlock + lines)->LoadAddr += tmbyte ;
		

		if (bRecord)
		{

			if(m_memblock.blocksize>0)
			{
			   m_memblock.itermBlockAdress[2*m_memblock.blocksize-1] = (m_memblock.pMemBlock + lines-1)->LoadAddr;
			   //TRACE1("block end adress %08x \n", m_memblock.itermBlockAdress[2*m_memblock.blocksize-1]);
			}
			m_memblock.itermBlockAdress[2*m_memblock.blocksize] = (m_memblock.pMemBlock + lines)->LoadAddr;
			//TRACE1("block start adress %08x \n", m_memblock.itermBlockAdress[2 * m_memblock.blocksize]);
			m_memblock.blocksize++;
			bRecord = false;
			
		}

	

		for ( i = 0; i < recLen-5; i++)
		{
			hex = (char2Hex(chtemp[12 + 2 * i]));
			tmbyte = hex << 4;
			hex = char2Hex(chtemp[13 + 2 * i]);
			tmbyte += hex;

			(m_memblock.pMemBlock + lines)->Data[i] = tmbyte;
			chkSum += tmbyte;
		}


		
		hex = (char2Hex(chtemp[12 + 2 * i]));
		recChkSum = hex << 4;
		hex = char2Hex(chtemp[13 + 2 * i]);
		recChkSum += hex;

		chkSum = 0xff - chkSum;

		byte n;
		n = (m_memblock.pMemBlock + lines)->NumBytes - 1;
		//TRACE3("adr %08xh, ldata %02x, chcsum %02x\n", (m_memblock.pMemBlock + lines)->LoadAddr,
		//	(m_memblock.pMemBlock + lines)->Data[n], chkSum);


		//数据块中数据存在错误，释放申请的内存并退出
		if (chkSum != recChkSum)
		{
			//TRACE("S19 files have error data lines");

			delete[]m_pSRecData;  
			m_pSRecData = NULL;
			return ERROR_BADDATA;
		}
		lines++;
	}

	m_memblock.itermBlockAdress[2 * m_memblock.blocksize - 1] = (m_memblock.pMemBlock + lines-1)->LoadAddr;
	//TRACE1("block end adress %08x \n", m_memblock.itermBlockAdress[2 * m_memblock.blocksize - 1]);
	return ERROR_OK;
}

bool  CS19RecStream::SavetoBin(FlashBlockType fbt)
{
	bool  result = true;
	CString savefile,str;
	CFile file;
	DWORD index;
	int blankMem;
	BYTE buf[256];
	memset(buf,0xff,256);
	DWORD savedbytes =0;
	DWORD fillbytes = 0;

	DWORD adrStart, adrEnd;
	CString filename;
	/*int index = m_strLoadedFile.GetLength()-1;
	while (m_strLoadedFile.GetAt(index) != _T('.'))
	{
		str = m_strLoadedFile.GetAt(index)+str;
		index--;
	}
	savefile = m_strLoadedFile.TrimRight(str)+_T("bin");*/

	switch (fbt)
	{
	case flashdriver:
		//adrStart = ;
		//adrEnd = ;
		return true;
	case appProgram:
		adrStart = m_memblock.itermBlockAdress[0];
		adrEnd   = m_memblock.itermBlockAdress[1];
		filename = m_strExeDirectory + szSubFolder + szAppFileName;
		break;
	case appData:
		adrStart = m_memblock.itermBlockAdress[2];
		adrEnd = m_memblock.itermBlockAdress[3];
		filename = m_strExeDirectory + szSubFolder + szCalFileName;
		break;
	}

	if (!file.Open(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareExclusive))
	{
		result = false;
		return result;
	}
	
	index = 0;
	while (adrStart != (m_memblock.pMemBlock + index)->LoadAddr)
	{
		index++;
	}


	while ((m_memblock.pMemBlock + index)->LoadAddr <= adrEnd && index <= m_memblock.itemRec)
	{
		if ((m_memblock.pMemBlock + index)->LoadAddr > adrStart)
		{
			blankMem = (m_memblock.pMemBlock + index)->LoadAddr - 
				((m_memblock.pMemBlock + index - 1)->LoadAddr + (m_memblock.pMemBlock + index - 1)->NumBytes);
			if (blankMem > 0)
			{
				file.Write(buf, blankMem);
				savedbytes += blankMem;
			}

		}
		file.Write((m_memblock.pMemBlock + index)->Data, (m_memblock.pMemBlock + index)->NumBytes);
	    //file.Flush();
		savedbytes += (m_memblock.pMemBlock + index)->NumBytes;
		index++;
	} 

	file.Flush();
	switch (fbt)
	{
	case flashdriver:
		//fillbytes = m_memFlashDrvObj.memsize - savedbytes;
		break;
	case appProgram:
		fillbytes = m_memAppObj.memsize - savedbytes;
		break;
	case appData:
		fillbytes = m_memDataObj.memsize - savedbytes;
		break;
	}
	if (fillbytes > 0)
	{
		BYTE byt = 0xff;
		for (DWORD i = 0; i < fillbytes ; i++)
		{
			file.Write(&byt, 1);
		}

	}
	file.Flush();
	file.Close();

	switch (fbt)
	{
	case flashdriver:
		// m_memFlashDrvObj.binfile = filename;
		break;
	case appProgram:
		m_memAppObj.binfile = filename;
		break;
	case appData:
		m_memDataObj.binfile = filename;
		break;
	}

	return result;
}

void  CS19RecStream::ReleaseBuffer()
{
	if (m_pSRecData != NULL)
	{

		delete[]m_pSRecData;
		m_pSRecData = NULL;
	}
}

void  CS19RecStream::RelocateCalMemoryAdress()
{
	m_memDataObj.startAdress = m_rlAdress;
}

void CS19RecStream::UTF8ToUnicode(CString& strUtf8)
{
	DWORD dwUnicodeLen;       
	TCHAR *pwText;            
	CString strUnicode;
	//先转换成 char*
	int len = WideCharToMultiByte(CP_ACP, 0, strUtf8, -1, NULL, 0, NULL, NULL);
	char *sztemp = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, strUtf8, -1, sztemp, len, NULL, NULL);
	//分配内存
	dwUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, sztemp, -1, NULL, 0);
	pwText = new TCHAR[dwUnicodeLen];
	if (!pwText)
	{
		strUtf8 = _T("0");
		return ;
	}
	//转换成unicode编码
	MultiByteToWideChar(CP_UTF8, 0, sztemp, -1, pwText, dwUnicodeLen);
	strUnicode.Format(_T("%s"), pwText);

	delete[] pwText;
	delete[] sztemp;
	strUtf8 = strUnicode;

}

int CS19RecStream::GetVersion()
{
	return 0;
}

void  CS19RecStream::SaveLastLoadFlie()
{

	const char cfgname[] = "DefaultCfg.xml";
	SetCurrentDirectory(m_strExeDirectory);

	//unicode to utf-8
	int len = WideCharToMultiByte(CP_UTF8, 0, m_strOpenFile, -1, NULL, 0, NULL, NULL);
	char *strlastloadfile = new char[len + 1];
	WideCharToMultiByte(CP_UTF8, 0, m_strOpenFile, -1, strlastloadfile, len, NULL, NULL);

	tinyxml2::XMLDocument doc;
	int i = doc.LoadFile(cfgname);
	tinyxml2::XMLElement  *subElem = doc.FirstChildElement()->FirstChildElement("LoadedFile");
	tinyxml2::XMLElement  *sub01 = subElem->FirstChildElement("FilePath");
	sub01->SetAttribute("value", strlastloadfile);
	delete[] strlastloadfile;
	doc.SaveFile(cfgname);

	SetCurrentDirectory(m_strCurrDirectory);
}

bool CS19RecStream::SaveAllBin()
{
	bool bResult;

	if (m_bUseLastCfg)
	{
		m_bUseLastCfg = false;
		return true;
	}

	bResult = SavetoBin(appProgram);
	bResult &= SavetoBin(appData);
	return bResult;
}
