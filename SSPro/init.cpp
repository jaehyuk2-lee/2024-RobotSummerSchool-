#include "stdafx.h"
#include "SSPro.h"
#include "SSProDlg.h"

void CSSProDlg::init()
{
	vcap = VideoCapture(0);
	if (!vcap.isOpened())
	{
		MessageBox(_T("캠을 열 수 없습니다."));
	}
	vcap.set(CAP_PROP_FRAME_WIDTH, img_w);//img_w = 320
	vcap.set(CAP_PROP_FRAME_WIDTH, img_h);//img_h = 240

	bitInfo->bmiHeader.biPlanes = 1;
	bitInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo->bmiHeader.biCompression = BI_RGB;
	bitInfo->bmiHeader.biClrImportant = 0;
	bitInfo->bmiHeader.biClrUsed = 0;
	bitInfo->bmiHeader.biSizeImage = 0;
	bitInfo->bmiHeader.biXPelsPerMeter = 0;
	bitInfo->bmiHeader.biYPelsPerMeter = 0;

	RGBQUAD* palette = bitInfo->bmiColors;
	for (int i = 0; i < 256; i++)
	{
		palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
		palette[i].rgbReserved = 0;
	}

	if (m_p_img_thread == NULL)
		m_p_img_thread = AfxBeginThread(img_thread, this);

	m_slider_h_min.SetRange(0, 180); m_slider_h_min.SetPos(H_min);
	m_slider_h_max.SetRange(0, 180); m_slider_h_max.SetPos(H_max);

	m_slider_s_min.SetRange(0, 255);m_slider_s_min.SetPos(S_min);
	m_slider_s_max.SetRange(0, 255); m_slider_s_max.SetPos(S_max);
	
	m_slider_v_min.SetRange(0, 255); m_slider_v_min.SetPos(V_min);
	m_slider_v_max.SetRange(0, 255); m_slider_v_max.SetPos(V_max);

	SetDlgItemInt(IDC_EDIT_H_MIN, H_min);
	SetDlgItemInt(IDC_EDIT_H_MAX, H_max);
	SetDlgItemInt(IDC_EDIT_S_MIN, S_min);
	SetDlgItemInt(IDC_EDIT_S_MAX, S_max);
	SetDlgItemInt(IDC_EDIT_V_MIN, V_min);
	SetDlgItemInt(IDC_EDIT_V_MAX, V_max);
}