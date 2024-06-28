#include "stdafx.h"
#include "SSPro.h"
#include "SSProDlg.h"
void CSSProDlg::init()
{
	vcap = VideoCapture(0);
	if (!vcap.isOpened())
	{
		MessageBox(_T("캠을 열수 없습니다."));
	}
	vcap.set(CAP_PROP_FRAME_WIDTH, img_w);//img_w = 320
	vcap.set(CAP_PROP_FRAME_HEIGHT, img_h); //img_h = 240
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

	for (int i = 0; i < Color_info_num; i++)
	{
		COLOR tmp_color = COLOR();
		tmp_color.Name.Format(L"Color %d", i + 1);
		Color_info.push_back(tmp_color); //COLOR());
		m_combo_color.AddString(tmp_color.Name);
	}
	m_combo_color.SetCurSel(0);
	Color_info[0].line_color = Scalar(0, 0, 230);
	Color_info[1].line_color = Scalar(0, 220, 0);
	Color_info[2].line_color = Scalar(210, 0, 0);


	m_slider_h_min.SetRange(0, 180); m_slider_h_min.SetPos(H_min);
	m_slider_h_max.SetRange(0, 180); m_slider_h_max.SetPos(H_max);
	m_slider_s_min.SetRange(0, 255); m_slider_s_min.SetPos(S_min);
	m_slider_s_max.SetRange(0, 255); m_slider_s_max.SetPos(S_max);
	m_slider_v_min.SetRange(0, 255); m_slider_v_min.SetPos(V_min);
	m_slider_v_max.SetRange(0, 255); m_slider_v_max.SetPos(V_max);
	SetDlgItemInt(IDC_EDIT_H_MIN, H_min);
	SetDlgItemInt(IDC_EDIT_H_MAX, H_max);
	SetDlgItemInt(IDC_EDIT_S_MIN, S_min);
	SetDlgItemInt(IDC_EDIT_S_MAX, S_max);
	SetDlgItemInt(IDC_EDIT_V_MIN, V_min);
	SetDlgItemInt(IDC_EDIT_V_MAX, V_max);

	if (m_p_img_thread == NULL)
		m_p_img_thread = AfxBeginThread(img_thread, this);
}

void CSSProDlg::GetHSVParam(int color_index)
{
	Color_info[color_index].H_max = H_max;
	Color_info[color_index].H_min = H_min;
	Color_info[color_index].S_max = S_max;
	Color_info[color_index].S_min = S_min;
	Color_info[color_index].V_max = V_max;
	Color_info[color_index].V_min = V_min;
}
//vector -> UI
void CSSProDlg::SetHSVParam(int color_index)
{
	H_max = Color_info[color_index].H_max;
	H_min = Color_info[color_index].H_min;
	S_max = Color_info[color_index].S_max;
	S_min = Color_info[color_index].S_min;
	V_max = Color_info[color_index].V_max;
	V_min = Color_info[color_index].V_min;
	m_slider_h_min.SetPos(H_min);
	m_slider_h_max.SetPos(H_max);
	m_slider_s_min.SetPos(S_min);
	m_slider_s_max.SetPos(S_max);
	m_slider_v_min.SetPos(V_min);
	m_slider_v_max.SetPos(V_max);
	SetDlgItemInt(IDC_EDIT_H_MIN, H_min);
	SetDlgItemInt(IDC_EDIT_H_MAX, H_max);
	SetDlgItemInt(IDC_EDIT_S_MIN, S_min);
	SetDlgItemInt(IDC_EDIT_S_MAX, S_max);
	SetDlgItemInt(IDC_EDIT_V_MIN, V_min);
	SetDlgItemInt(IDC_EDIT_V_MAX, V_max);
}

void CSSProDlg::SaveHSVParam()
{
	GetHSVParam(Color_Index);
	FILE *fp = fopen("data.txt", "w");
	for (int i = 0; i < Color_info_num; i++)
	{
		fprintf(fp, "[Color%d.H_max] %d\n", i + 1, Color_info[i].H_max);
		fprintf(fp, "[Color%d.H_min] %d\n", i + 1, Color_info[i].H_min);
		fprintf(fp, "[Color%d.S_max] %d\n", i + 1, Color_info[i].S_max);
		fprintf(fp, "[Color%d.S_min] %d\n", i + 1, Color_info[i].S_min);
		fprintf(fp, "[Color%d.V_max] %d\n", i + 1, Color_info[i].V_max);
		fprintf(fp, "[Color%d.V_min] %d\n", i + 1, Color_info[i].V_min);
	}
	fclose(fp);
	fp = NULL;
}

void CSSProDlg::LoadHSVParam() {
	FILE* fp = fopen("data.txt", "r");
	char DataSave[256];

	memset(DataSave, 0, 256);
	for (int i = 0; i < Color_info_num; i++) {
		memset(DataSave, 0, 256); fscanf(fp, "%s", DataSave); fscanf(fp, "%s", DataSave);
		Color_info[i].H_max = atoi(DataSave);
		memset(DataSave, 0, 256); fscanf(fp, "%s", DataSave); fscanf(fp, "%s", DataSave);
		Color_info[i].H_min = atoi(DataSave);

		memset(DataSave, 0, 256); fscanf(fp, "%s", DataSave); fscanf(fp, "%s", DataSave);
		Color_info[i].S_max = atoi(DataSave);
		memset(DataSave, 0, 256); fscanf(fp, "%s", DataSave); fscanf(fp, "%s", DataSave);
		Color_info[i].S_min = atoi(DataSave);

		memset(DataSave, 0, 256); fscanf(fp, "%s", DataSave); fscanf(fp, "%s", DataSave);
		Color_info[i].V_max = atoi(DataSave);
		memset(DataSave, 0, 256); fscanf(fp, "%s", DataSave); fscanf(fp, "%s", DataSave);
		Color_info[i].V_min = atoi(DataSave);
	}
	SetHSVParam(Color_Index);
	fclose(fp);
	fp = NULL;
}