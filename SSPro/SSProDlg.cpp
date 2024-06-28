
// SSProDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SSPro.h"
#include "SSProDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinThread*CSSProDlg::m_p_img_thread = NULL;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected: 
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSSProDlg dialog



CSSProDlg::CSSProDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SSPRO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSSProDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMG, m_st_img);
	DDX_Control(pDX, IDC_STATIC_IMG2, m_st_img2);
	DDX_Control(pDX, IDC_SLIDER_H_MAX, m_slider_h_max);
	DDX_Control(pDX, IDC_SLIDER_H_MIN, m_slider_h_min);
	DDX_Control(pDX, IDC_SLIDER_S_MIN, m_slider_s_min);
	DDX_Control(pDX, IDC_SLIDER_S_MAX, m_slider_s_max);
	DDX_Control(pDX, IDC_SLIDER_V_MIN, m_slider_v_min);
	DDX_Control(pDX, IDC_SLIDER_V_MAX, m_slider_v_max);
}

BEGIN_MESSAGE_MAP(CSSProDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_H_MAX, &CSSProDlg::OnNMCustomdrawSliderHMax)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_S_MIN, &CSSProDlg::OnNMCustomdrawSliderSMin)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CSSProDlg message handlers

BOOL CSSProDlg:: OnInitDialog()
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
	init();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSSProDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSSProDlg::OnPaint()
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
HCURSOR CSSProDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSSProDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (vcap.isOpened())
	{
		vcap.release();
	}
	DWORD nExitCode = NULL;
	GetExitCodeThread(m_p_img_thread->m_hThread, &nExitCode);
	TerminateThread(m_p_img_thread->m_hThread, nExitCode);
	m_p_img_thread->Delete();
	m_p_img_thread = NULL;
}


void CSSProDlg::OnNMCustomdrawSliderHMax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CSSProDlg::OnNMCustomdrawSliderSMin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CSSProDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	H_min = m_slider_h_min.GetPos();
	H_max = m_slider_h_max.GetPos();

	S_min = m_slider_s_min.GetPos();
	S_max = m_slider_s_max.GetPos();

	V_min = m_slider_v_min.GetPos();
	V_max = m_slider_v_max.GetPos();

	SetDlgItemInt(IDC_EDIT_H_MIN, H_min);
	SetDlgItemInt(IDC_EDIT_H_MAX, H_max);
	SetDlgItemInt(IDC_EDIT_S_MIN, S_min);
	SetDlgItemInt(IDC_EDIT_S_MAX, S_max);
	SetDlgItemInt(IDC_EDIT_V_MIN, V_min);
	SetDlgItemInt(IDC_EDIT_V_MAX, V_max);
}
