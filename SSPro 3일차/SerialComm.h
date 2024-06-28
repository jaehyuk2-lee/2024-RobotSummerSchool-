
#pragma once

#include <afxmt.h>

#define MAXBUF         50000 
#define InBufSize      50000
#define OutBufSize     50000
#define ASCII_XON      0x11
#define ASCII_XOFF     0x13
#define WM_DSP_RECEIVE	(WM_USER+1) // dsp 데이터 수신 메시지
#define WM_DSP_CLOSE	(WM_USER+2) // dsp 종료 메시지
#define WM_SEN_RECEIVE	(WM_USER+3) // sen 
#define WM_SEN_CLOSE	(WM_USER+4)


// CSerialComm 명령 대상입니다.

class CSerialComm : public CCmdTarget
{
	DECLARE_DYNAMIC(CSerialComm)

public:
	CSerialComm();
	virtual ~CSerialComm();

public:

	HANDLE  m_hComDev;
	HWND    m_hWnd;

	BOOL    m_bIsOpenned;
	CString m_sComPort;
	CString m_sBaudRate;
	CString m_sParity;
	CString m_sDataBit;
	CString m_sStopBit;
	BOOL    m_bFlowChk;

	OVERLAPPED m_OLR,m_OLW;
	char	m_sInBuf[MAXBUF*2];
	int		m_nLength;
	CEvent* m_pEvent;

	bool m_send_return_flg;
	// Operations

public:

	void Clear();
	int Receive(LPSTR inbuf, int len);
	BOOL Send(LPCTSTR outbuf, int len);
	BOOL Create(HWND hWnd);
	void HandleClose();
	void Close();
	void ResetSerial();

	CSerialComm(CString port,CString baudrate,CString parity,
		CString databit,CString stopbit);

protected:
	DECLARE_MESSAGE_MAP()
};