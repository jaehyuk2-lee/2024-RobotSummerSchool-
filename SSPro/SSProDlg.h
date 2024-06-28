
// SSProDlg.h : header file
//

#pragma once
#include "afxwin.h"
#ifdef _DEBUG
#pragma comment(lib,"opencv_world430d.lib")
#else
#pragma comment(lib,"opencv_world430.lib")
#endif

#include "opencv2/opencv.hpp"
#include "afxcmn.h"
using namespace cv;
using namespace std;
UINT img_thread(LPVOID arg);

// CSSProDlg dialog
class CSSProDlg : public CDialogEx
{
// Construction
public:
	CSSProDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SSPRO_DIALOG };
#endif

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
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_st_img;
	afx_msg void OnDestroy();

	bool isGrabbed = false;
	int img_w = 320, img_h = 240;
	VideoCapture vcap;
	Mat Origin_img = Mat::zeros(Size(img_w, img_h), CV_8UC3);
	Mat Show_img = Mat::zeros(Size(img_w, img_h), CV_8UC3);
	CImage cimage_mfc;
	BITMAPINFO*bitInfo = (BITMAPINFO*)
		malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	static CWinThread* m_p_img_thread;
	bool grab_image();
	void img_proc();
	void img_view(Mat* img, CStatic * picCtrl);
	void init();
	CStatic m_st_img2;
	CSliderCtrl m_slider_h_max;
	CSliderCtrl m_slider_h_min;
	afx_msg void OnNMCustomdrawSliderHMax(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_slider_s_min;
	CSliderCtrl m_slider_s_max;
	CSliderCtrl m_slider_v_min;
	CSliderCtrl m_slider_v_max;
	afx_msg void OnNMCustomdrawSliderSMin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	int H_min = 0, H_max = 180;
	int S_min = 0, S_max = 255;
	int V_min = 0, V_max = 255;
};
