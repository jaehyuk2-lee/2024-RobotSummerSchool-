
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

#include "SerialComm.h"
#include "CmdDefine.h"

using namespace cv;
using namespace std;
UINT img_thread(LPVOID arg);
UINT CommThread(LPVOID arg);
UINT CommSendThread(LPVOID arg);

typedef struct LabelData
{
	int label_idx = 0;
	int area = 0;
	Point2d ct_p = Point2d();
	Rect rect = Rect();
}LabelData;
typedef struct COLOR
{
	int H_min = 0, H_max = 180; //변수 선언 및 초기화
	int S_min = 0, S_max = 255;
	int V_min = 0, V_max = 255;
	Scalar line_color = Scalar();
	CString Name = _T("");
	Mat img;
	Mat Label;
	vector<LabelData>Labels = vector<LabelData>();
	void InitLabels()
	{
		vector<LabelData>().swap(Labels);
	}
}COLOR;

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
	int H_min = 0, H_max = 180;
	int S_min = 0, S_max = 255;
	int V_min = 0, V_max = 255;
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
	CSliderCtrl m_slider_h_min;
	CSliderCtrl m_slider_h_max;
	CSliderCtrl m_slider_s_min;
	CSliderCtrl m_slider_s_max;
	CSliderCtrl m_slider_v_min;
	CSliderCtrl m_slider_v_max;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CComboBox m_combo_color;
	afx_msg void OnCbnSelchangeComboColor();

	int Color_info_num = 3;
	int Color_Index = 0;
	vector<COLOR>Color_info = vector<COLOR>();
	void GetHSVParam(int color_index);
	void SetHSVParam(int color_index);
	void GetLabelData(Mat* color_img);
	void DrawObject();
	bool CheckLabelStats(int color_index, LabelData labeldata);

	void CSSProDlg::SaveHSVParam();
	void CSSProDlg::LoadHSVParam();

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	CComboBox m_combo_port;
	CEdit m_edit_comlog;

	LRESULT OnThreadClosed(WPARAM length, LPARAM lpara);
	LRESULT OnReceive(WPARAM length, LPARAM lpara);

	CComboBox m_combo_port;
	CEdit m_edit_comlog;

	CSerialComm* comm_1 = NULL;
	bool comm_1state = false;
	bool SendNReturn = true;
	void send_data_to_dsp();

	static CWinThread* m_receive_thread;
	static CWinThread* m_send_thread;

	int UD_NeckAngle = 180;
	int RL_NeckAngle = 180;
	int MotionCmd = 0;

	bool Q_state = false;
	bool neck_angle_chg_UD = false;
	bool neck_angle_chg_RL = false;
	CString remain_data;

	bool data_from_typing = false;
	bool ready_state = false;
	bool start_state = false;

	CString m_str_edit_senddata;
	CString m_str_combo_port;
};