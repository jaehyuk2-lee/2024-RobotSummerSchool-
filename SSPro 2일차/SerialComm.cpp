// SerialComm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SerialComm.h"


// CSerialComm

IMPLEMENT_DYNAMIC(CSerialComm, CCmdTarget)


CSerialComm::CSerialComm()
{
}

CSerialComm::~CSerialComm()
{
	if (m_bIsOpenned)
		Close();
	delete m_pEvent;
}

CSerialComm::CSerialComm(CString port,CString baudrate,CString parity,CString databit,CString stopbit)
{
	// 생성자
	m_sComPort = port;
	m_sBaudRate = baudrate;
	m_sParity = parity;
	m_sDataBit = databit;
	m_sStopBit = stopbit;
	m_bFlowChk = 1;
	m_bIsOpenned = FALSE;
	m_nLength = 0;
	memset(m_sInBuf,0,MAXBUF*2);
	m_pEvent = new CEvent(FALSE,TRUE);
	//
	m_send_return_flg = false;
}

BEGIN_MESSAGE_MAP(CSerialComm, CCmdTarget)
END_MESSAGE_MAP()

void CSerialComm::ResetSerial()
{
	DCB		dcb;
	DWORD	DErr;
	COMMTIMEOUTS	CommTimeOuts;

	if (!m_bIsOpenned)
		return;

	ClearCommError(m_hComDev,&DErr,NULL);
	SetupComm(m_hComDev,InBufSize,OutBufSize);
	PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	// set up for overlapped I/O
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 0 ;

	// CBR_9600 is approximately 1byte/ms. For our purposes, allow
	// double the expected time per character for a fudge factor.

	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 1000;
	SetCommTimeouts(m_hComDev, &CommTimeOuts);                

	memset(&dcb,0,sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hComDev, &dcb);

	dcb.fBinary = TRUE;
	dcb.fParity = TRUE;

	if (m_sBaudRate == "300")					dcb.BaudRate = CBR_300;
	else if (m_sBaudRate == "600")				dcb.BaudRate = CBR_600;
	else if (m_sBaudRate == "1200")				dcb.BaudRate = CBR_1200;
	else if (m_sBaudRate == "2400")				dcb.BaudRate = CBR_2400;
	else if (m_sBaudRate == "4800")				dcb.BaudRate = CBR_4800;
	else if (m_sBaudRate == "9600")				dcb.BaudRate = CBR_9600;
	else if (m_sBaudRate == "14400")			dcb.BaudRate = CBR_14400;
	else if (m_sBaudRate == "19200")			dcb.BaudRate = CBR_19200;
	else if (m_sBaudRate == "28800")			dcb.BaudRate = CBR_38400;
	else if (m_sBaudRate == "33600")			dcb.BaudRate = CBR_38400;
	else if (m_sBaudRate == "38400")			dcb.BaudRate = CBR_38400;
	else if (m_sBaudRate == "56000")			dcb.BaudRate = CBR_56000;
	else if (m_sBaudRate == "57600")			dcb.BaudRate = CBR_57600;
	else if (m_sBaudRate == "115200")			dcb.BaudRate = CBR_115200;
	else if (m_sBaudRate == "128000")			dcb.BaudRate = CBR_128000;
	else if (m_sBaudRate == "256000")			dcb.BaudRate = CBR_256000;

	else if (m_sBaudRate == "PCI_9600")			dcb.BaudRate = 1075;
	else if (m_sBaudRate == "PCI_19200")		dcb.BaudRate = 2212;
	else if (m_sBaudRate == "PCI_38400")		dcb.BaudRate = 4300;
	else if (m_sBaudRate == "PCI_57600")		dcb.BaudRate = 6450;
	else if (m_sBaudRate == "PCI_500K")			dcb.BaudRate = 56000;

	if (m_sParity == "None")		dcb.Parity = NOPARITY;
	else if (m_sParity == "Even")	dcb.Parity = EVENPARITY;
	else if (m_sParity == "Odd")	dcb.Parity = ODDPARITY;

	if (m_sDataBit == "7 Bit")		dcb.ByteSize = 7;
	else if (m_sDataBit == "8 Bit")	dcb.ByteSize = 8;                             

	if (m_sStopBit == "1 Bit")			dcb.StopBits = ONESTOPBIT;
	else if (m_sStopBit == "1.5 Bit")	dcb.StopBits = ONE5STOPBITS;
	else if (m_sStopBit == "2 Bit")		dcb.StopBits = TWOSTOPBITS;

	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fOutxDsrFlow = FALSE;

	if (m_bFlowChk) 
	{
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		dcb.XonLim = 2048;
		dcb.XoffLim = 1024;
	}
	else 
	{
		dcb.fOutxCtsFlow = TRUE;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	}

	SetCommState(m_hComDev, &dcb);
	SetCommMask(m_hComDev,EV_RXCHAR);
}
void CSerialComm::Close()
{
	if (!m_bIsOpenned)
		return;

	m_bIsOpenned = FALSE;
	SetCommMask(m_hComDev, 0);
	EscapeCommFunction(m_hComDev, CLRDTR);
	PurgeComm(m_hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	Sleep(500);
}
void CSerialComm::HandleClose()
{
	CloseHandle(m_hComDev);
	CloseHandle(m_OLR.hEvent);
	CloseHandle(m_OLW.hEvent);
}
BOOL CSerialComm::Create(HWND hWnd)
{
	m_hWnd = hWnd;
	m_hComDev = CreateFile(m_sComPort, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
		NULL);

	if (m_hComDev!=INVALID_HANDLE_VALUE)
		m_bIsOpenned = TRUE;
	else
		return FALSE;

	ResetSerial();
	m_OLW.Offset = 0;
	m_OLW.OffsetHigh = 0;
	m_OLR.Offset = 0;
	m_OLR.OffsetHigh = 0;

	m_OLR.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_OLR.hEvent == NULL) {
		CloseHandle(m_OLR.hEvent);
		return FALSE;
	}

	m_OLW.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_OLW.hEvent == NULL) {
		CloseHandle(m_OLW.hEvent);
		return FALSE;
	}
	
	EscapeCommFunction(m_hComDev, SETDTR);

	return TRUE;
}

BOOL CSerialComm::Send(LPCTSTR outbuf, int len)
{
	BOOL	bRet=TRUE;
	DWORD	ErrorFlags;
	COMSTAT	ComStat;
	DWORD	BytesWritten;
	DWORD	BytesSent=0;

	// unicode(wide char) : outbuf-> multibyte : strA 변환
	CStringA strA;
	strA = outbuf;

	ClearCommError(m_hComDev, &ErrorFlags, &ComStat);
	if (!WriteFile(m_hComDev, strA, len, &BytesWritten, &m_OLW))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			if (WaitForSingleObject(m_OLW.hEvent,1000) != WAIT_OBJECT_0)
				bRet=FALSE;
			else
				GetOverlappedResult(m_hComDev, &m_OLW, &BytesWritten, FALSE);
		}
		else /* I/O error */
			bRet=FALSE; /* ignore error */
	}

	ClearCommError(m_hComDev, &ErrorFlags, &ComStat);
	return bRet;
}

int CSerialComm::Receive(LPSTR inbuf, int len)
{
	CSingleLock lockObj((CSyncObject*) m_pEvent,FALSE);

	// argument value is not valid
	if (len == 0)
		return -1;
	else if  (len > MAXBUF)
		return -1;
	
	if (m_nLength == 0)
	{
		inbuf[0] = '\0';
		return 0;
	}

	else if (m_nLength <= len)
	{
		lockObj.Lock();
		memcpy(inbuf,m_sInBuf,m_nLength);
		memset(m_sInBuf,0,MAXBUF*2);
		int tmp = m_nLength;
		m_nLength = 0;
		lockObj.Unlock();
		return tmp;
	}
	else
	{
		lockObj.Lock();
		memcpy(inbuf,m_sInBuf,len);
		memmove(m_sInBuf,m_sInBuf+len,MAXBUF*2-len);
		m_nLength -= len;
		lockObj.Unlock();
		return len;
	}
}

void CSerialComm::Clear()
{
	PurgeComm(m_hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	memset(m_sInBuf,0,MAXBUF*2);
	m_nLength = 0;
} 