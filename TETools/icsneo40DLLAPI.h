//FILE: icsneo40DLLAPI.H
#pragma once

#include <windows.h>
#include "icsnVC40.h"



bool LoadDLLAPI(HINSTANCE &hAPIDLL);
void UnloadDLLAPI(HINSTANCE &hAPIDLL);

//Basic Functions
typedef int  (__stdcall *FINDNEODEVICES)(unsigned long DeviceTypes, NeoDevice *pNeoDevice, int *pNumDevices);
typedef int  (__stdcall *OPENNEODEVICE)(NeoDevice *pNeoDevice, int *hObject, unsigned char *bNetworkIDs, int bConfigRead, int bSyncToPC);
typedef int  (__stdcall *CLOSEPORT)(int hObject, int *pNumberOfErrors); 
typedef void (__stdcall *FREEOBJECT)(int hObject);
typedef int  (__stdcall *OPENPORTEX)(int lPortNumber, int lPortType, int lDriverType, int lIPAddressMSB, int lIPAddressLSBOrBaudRate, 
							         int bConfigRead, unsigned char *bNetworkID, int * hObject);

//Message Functions
typedef int  (__stdcall *GETMESSAGES)(int hObject, icsSpyMessage *pMsg, int * pNumberOfMessages, int * pNumberOfErrors); 
typedef int  (__stdcall *TXMESSAGES)(int hObject, icsSpyMessage *pMsg, int lNetworkID, int lNumMessages); 
typedef int  (__stdcall *WAITFORRXMSGS)(int hObject, unsigned int iTimeOut);
typedef int  (__stdcall *ENABLERXQUEUE)(int hObject, int iEnable);
typedef int  (__stdcall *GETTSFORMSG)(int hObject, icsSpyMessage *pMsg, double *pTimeStamp);
typedef void (__stdcall *GETISO15765STATUS)(int hObject, int lNetwork, int lClearTxStatus, 
											int lClearRxStatus, int *lTxStatus, int *lRxStatus);
typedef void (__stdcall *SETISO15765RXPARMS)(int hObject, int lNetwork, int lEnable, 
			                                spyFilterLong *pFF_CFMsgFilter, icsSpyMessage *pTxMsg, 
											int lCFTimeOutMs, int lFlowCBlockSize,
											int lUsesExtendedAddressing, int lUseHardwareIfPresent);
typedef int (__stdcall *DOWNLOADISO15765_2_TXSCRIPT)(int hObject, unsigned int NetworkID);
typedef int (__stdcall *CLEARISO15765_2_TXSCRIPT)(int hObject);
typedef int (__stdcall *TXISO15765_2_MESSAGE)(int hObject, stCM_ISO157652_TxMessage *pTxMessage);

//Device Functions
typedef int (__stdcall *GETCONFIG)(int hObject, unsigned char * pData, int * lNumBytes);
typedef int (__stdcall *SENDCONFIG)(int hObject, unsigned char * pData, int lNumBytes); 
typedef int (__stdcall *GETFIRESETTINGS)(int hObject, SFireSettings *pSettings, int iNumBytes);
typedef int (__stdcall *SETFIRESETTINGS)(int hObject, SFireSettings *pSettings, int iNumBytes, int bSaveToEEPROM);
typedef int (__stdcall *GETVCAN3SETTINGS)(int hObject, SVCAN3Settings *pSettings, int iNumBytes);
typedef int (__stdcall *SETVCAN3SETTINGS)(int hObject, SVCAN3Settings *pSettings, int iNumBytes, int bSaveToEEPROM);
typedef int (__stdcall *SETBITRATE)(int hObject, int BitRate, int NetworkID);
typedef int (__stdcall *GETDEVICEPARMS)(int hObject, char *pParameter, char *pValues, short ValuesLength);
typedef int (__stdcall *SETDEVICEPARMS)(int hObject, char *pParmValue, int *pErrorIndex, int bSaveToEEPROM);

//Error Functions
typedef int (__stdcall *GETLASTAPIERROR)(int hObject, unsigned long *pErrorNumber);
typedef int (__stdcall *GETERRMSGS)(int hObject, int * pErrorMsgs, int * pNumberOfErrors);
typedef int (__stdcall *GETERRORINFO)(int lErrorNumber, TCHAR *szErrorDescriptionShort, 
										TCHAR *szErrorDescriptionLong, int * lMaxLengthShort,
                                        int * lMaxLengthLong,int * lErrorSeverity,int * lRestartNeeded);

//General Utility Functions
typedef int (__stdcall *VALIDATEHOBJECT)(int hObject);
typedef int (__stdcall *GETDLLVERSION)(void);
typedef int (__stdcall *GETSERIALNUMBER)(int hObject, unsigned int *iSerialNumber);
typedef int (__stdcall *STARTSOCKSERVER)(int hObject, int iPort);
typedef int (__stdcall *STOPSOCKSERVER)(int hObject);

//CoreMini Script functions
typedef int  (__stdcall *SCRIPTSTART)(int hObject, int iLocation);  
typedef int  (__stdcall *SCRIPTSTOP)(int hObject);  
typedef int  (__stdcall *SCRIPTLOAD)(int hObject, const unsigned char * bin, unsigned long len_bytes, int iLocation);
typedef int  (__stdcall *SCRIPTCLEAR)(int hObject, int iLocation);
typedef int  (__stdcall *SCRIPTSTARTFBLOCK)(int hObject,unsigned int fb_index);
typedef int  (__stdcall *SCRIPTGETFBLOCKSTATUS)(int hObject, unsigned int fb_index, int *piRunStatus);
typedef int  (__stdcall *SCRIPTSTOPFBLOCK)(int hObject,unsigned int fb_index);
typedef int  (__stdcall *SCRIPTGETSCRIPTSTATUS)(int hObject, int *piStatus);
typedef int  (__stdcall *SCRIPTREADAPPSIGNAL)(int hObject, unsigned int iIndex, double *dValue);
typedef int  (__stdcall *SCRIPTWRITEAPPSIGNAL)(int hObject, unsigned int iIndex, double dValue);
typedef int  (__stdcall *SCRIPTREADRXMESSAGE)(int hObject, unsigned int iIndex,
											  icsSpyMessage *pRxMessageMask, icsSpyMessage *pRxMessageValue);
typedef int  (__stdcall *SCRIPTREADTXMESSAGE)(int hObject, unsigned int iIndex, icsSpyMessage *pTxMessage);
typedef int  (__stdcall *SCRIPTWRITERXMESSAGE)(int hObject, unsigned int iIndex,
											   const icsSpyMessage *pRxMessageMask, const icsSpyMessage *pRxMessageValue);
typedef int  (__stdcall *SCRIPTWRITETXMESSAGE)(int hObject, unsigned int iIndex, const icsSpyMessage *pTxMessage);
typedef int  (__stdcall *SCRIPTREADISO15765TXMESSAGE)(int hObject, unsigned int iIndex, stCM_ISO157652_TxMessage *pTxMessage);
typedef int  (__stdcall *SCRIPTWRITEISO15765TXMESSAGE)(int hObject, unsigned int iIndex, const stCM_ISO157652_TxMessage *pTxMessage);


//Deprecated (but still suppored in the DLL)
typedef int  (__stdcall *OPENPORTEX)(int lPortSerialNumber, int lPortType, int lDriverType, 
					                 int lIPAddressMSB, int lIPAddressLSBOrBaudRate,int bConfigRead, 
				                     unsigned char * bNetworkID, int * hObject);
typedef int  (__stdcall *OPENPORT)(int lPortNumber, int lPortType, int lDriverType, 
					               unsigned char *bNetworkID, unsigned char *bSCPIDs,  int * hObject);
typedef int (__stdcall *ENABLENETWORKCOM)(int hObject, int Enable);
typedef int (__stdcall *FINDCOMDEVICES)(int lDriverType,  int lGetSerialNumbers, int lStopAtFirst, int lUSBCommOnly,
							            int *p_lDeviceTypes, int *p_lComPorts, int *p_lSerialNumbers, int *lNumDevices); 



//Basic Functions
extern FINDNEODEVICES icsneoFindNeoDevices;
extern OPENNEODEVICE icsneoOpenNeoDevice;
extern CLOSEPORT icsneoClosePort;
extern FREEOBJECT icsneoFreeObject;
extern OPENPORTEX icsneoOpenPortEx;
extern ENABLENETWORKCOM icsneoEnableNetworkCom;

//Message Functions
extern GETMESSAGES icsneoGetMessages;
extern TXMESSAGES icsneoTxMessages;
extern WAITFORRXMSGS icsneoWaitForRxMessagesWithTimeOut;
extern GETTSFORMSG icsneoGetTimeStampForMsg;
extern ENABLERXQUEUE icsneoEnableNetworkRXQueue;
extern GETISO15765STATUS icsneoGetISO15765Status;
extern SETISO15765RXPARMS icsneoSetISO15765RxParameters;
extern DOWNLOADISO15765_2_TXSCRIPT icsneoDownloadISO15765_2_TXScript;
extern CLEARISO15765_2_TXSCRIPT icsneoClearISO15765_2_TxScript;
extern TXISO15765_2_MESSAGE icsneoTransmitISO15765_2_Message;


//Device Functions
extern GETCONFIG icsneoGetConfiguration;
extern SENDCONFIG icsneoSendConfiguration;
extern GETFIRESETTINGS icsneoGetFireSettings;
extern SETFIRESETTINGS icsneoSetFireSettings;
extern GETVCAN3SETTINGS icsneoGetVCAN3Settings;
extern SETVCAN3SETTINGS icsneoSetVCAN3Settings;
extern SETBITRATE icsneoSetBitRate;
extern GETDEVICEPARMS icsneoGetDeviceParameters;
extern SETDEVICEPARMS icsneoSetDeviceParameters;

//Error Functions
extern GETLASTAPIERROR icsneoGetLastAPIError;
extern GETERRMSGS icsneoGetErrorMessages;
extern GETERRORINFO icsneoGetErrorInfo;

//General Utility Functions
extern VALIDATEHOBJECT icsneoValidateHObject;
extern GETDLLVERSION icsneoGetDLLVersion;
extern GETSERIALNUMBER icsneoGetSerialNumber;
extern STARTSOCKSERVER icsneoStartSockServer;
extern STOPSOCKSERVER icsneoStopSockServer;

//CoreMini Functions
extern SCRIPTSTART icsneoScriptStart;
extern SCRIPTSTOP icsneoScriptStop;
extern SCRIPTLOAD icsneoScriptLoad;
extern SCRIPTCLEAR icsneoScriptClear;
extern SCRIPTSTARTFBLOCK icsneoScriptStartFBlock;
extern SCRIPTGETFBLOCKSTATUS icsneoScriptGetFBlockStatus;
extern SCRIPTSTOPFBLOCK icsneoScriptStopFBlock;
extern SCRIPTGETSCRIPTSTATUS icsneoScriptGetScriptStatus;
extern SCRIPTREADAPPSIGNAL icsneoScriptReadAppSignal;
extern SCRIPTWRITEAPPSIGNAL icsneoScriptWriteAppSignal;
extern SCRIPTREADRXMESSAGE icsneoScriptReadRxMessage;
extern SCRIPTREADTXMESSAGE icsneoScriptReadTxMessage;
extern SCRIPTWRITERXMESSAGE icsneoScriptWriteRxMessage;
extern SCRIPTWRITETXMESSAGE icsneoScriptWriteTxMessage;
extern SCRIPTREADISO15765TXMESSAGE icsneoScriptReadISO15765_2_TxMessage;
extern SCRIPTWRITEISO15765TXMESSAGE icsneoScriptWriteISO15765_2_TxMessage;


