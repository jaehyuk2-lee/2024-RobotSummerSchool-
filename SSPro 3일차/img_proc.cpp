#include "stdafx.h"
#include "SSPro.h"
#include "SSProDlg.h"
UINT img_thread(LPVOID arg)
{
	CSSProDlg* pDlg = (CSSProDlg*)arg;
	while (1)
	{
		pDlg->img_proc();
		WaitForSingleObject(pDlg->m_p_img_thread->m_hThread, 1);
	}
}
bool CSSProDlg::grab_image()
{
	bool result_grab = vcap.read(Origin_img);
	isGrabbed = result_grab;
	return result_grab;
}
void CSSProDlg::img_proc()
{
	static int grab_fail_cnt = 0;
	if (vcap.isOpened())
	{
		if (grab_image())
		{
			grab_fail_cnt = 0;
			int tmp_img_width = Origin_img.cols;
			int tmp_img_height = Origin_img.rows;
			if (tmp_img_width == img_w && tmp_img_height == img_h)
			{
				Show_img = Origin_img.clone();
			}
			else
			{
				resize(Origin_img, Origin_img, cv::Size(img_w, img_h));
				Show_img = Origin_img.clone();
			}
		}
		else
		{
			grab_fail_cnt++;
			if (grab_fail_cnt > 3000)// 1ms 간격 반복이라면 3 초
			{
				grab_fail_cnt = 0;
				vcap.release();
				vcap = VideoCapture(0);
				return;
			}
		}
	}
	else
	{
		vcap.release();
		vcap = VideoCapture(0);
		return;
	}
	/*Mat tmp_extracted_img;
	Mat tmp_mask_img;
	cvtColor(Origin_img, tmp_extracted_img, COLOR_BGR2HSV);
	Scalar lower, upper;
	if (H_max >= H_min)
	{
		Mat tmp_bin_img;
		lower = Scalar(H_min, S_min, V_min);
		upper = Scalar(H_max, S_max, V_max);
		inRange(tmp_extracted_img, lower, upper, tmp_bin_img);
		tmp_mask_img = tmp_bin_img;
	}
	else
	{
		Mat tmp_bin_img, tmp_bin_img2;
		lower = Scalar(0, S_min, V_min);
		upper = Scalar(H_max, S_max, V_max);
		inRange(tmp_extracted_img, lower, upper, tmp_bin_img);
		lower = Scalar(H_min, S_min, V_min);
		upper = Scalar(180, S_max, V_max);
		inRange(tmp_extracted_img, lower, upper, tmp_bin_img2);
		tmp_bin_img |= tmp_bin_img2;
		tmp_mask_img = tmp_bin_img;
	}
	Mat labels, stats, centroids;
	int label_num = connectedComponentsWithStats(tmp_mask_img, labels,
		stats, centroids, 8, CV_32S, CCL_DEFAULT);
	int*p_stats = (int*)stats.data;
	double*p_cents = (double*)centroids.data;
	for (int i = 1; i < label_num; i++)
	{
		int x = *(p_stats + i * 5 + CC_STAT_LEFT);
		int y = *(p_stats + i * 5 + CC_STAT_TOP);
		int w = *(p_stats + i * 5 + CC_STAT_WIDTH);
		int h = *(p_stats + i * 5 + CC_STAT_HEIGHT);
		int area = *(p_stats + i * 5 + CC_STAT_AREA);
		double ct_x = *(p_cents + i * 2 + 0);
		double ct_y = *(p_cents + i * 2 + 1);
		if (area > 500)
		{
			Rect label_rect = Rect(x, y, w, h);
			rectangle(Show_img, label_rect, Scalar(0, 0, 200), 2, LINE_8);
			circle(Show_img, Point(ct_x, ct_y), 7, Scalar(255, 255, 0), -1, LINE_8);
		}
	}*/
	Mat tmp_mask_img = Origin_img.clone();
	GetLabelData(&tmp_mask_img);
	DrawObject();

	img_view(&tmp_mask_img, &m_st_img2);

	img_view(&Show_img, &m_st_img);


}
void CSSProDlg::img_view(Mat * img, CStatic * picCtrl)
{
	int bpp = 8 * (int)(*img).elemSize();
	assert((bpp == 8 || bpp == 24 || bpp == 32));
	int padding = 0;
	if (bpp < 32)
		padding = 4 - ((*img).cols % 4);
	if (padding == 4)
		padding = 0;
	int border = 0;
	if (bpp < 32)
	{
		border = 4 - ((*img).cols % 4);
	}
	Mat mat_temp;
	if (border > 0 || (*img).isContinuous() == false)
	{
		cv::copyMakeBorder((*img), mat_temp, 0, 0, 0, border,
			cv::BORDER_CONSTANT, 0);
	}
	else
	{
		mat_temp = (*img);
	}
	RECT r;
	(*picCtrl).GetClientRect(&r);
	cv::Size winSize(r.right, r.bottom);
	cimage_mfc.Create(winSize.width, winSize.height, 24);
	bitInfo->bmiHeader.biBitCount = bpp;
	bitInfo->bmiHeader.biWidth = mat_temp.cols;
	bitInfo->bmiHeader.biHeight = -mat_temp.rows;
	HDC dcc = cimage_mfc.GetDC();
	if (mat_temp.cols == winSize.width && mat_temp.rows == winSize.height)
	{
		SetDIBitsToDevice(dcc,
			0, 0, winSize.width, winSize.height,
			0, 0, 0, mat_temp.rows,
			mat_temp.data, bitInfo, DIB_RGB_COLORS);
	}
	else
	{
		int destx = 0, desty = 0;
		int destw = winSize.width;
		int desth = winSize.height;
		int imgx = 0, imgy = 0;
		int imgWidth = mat_temp.cols - border;
		int imgHeight = mat_temp.rows;
		SetStretchBltMode(dcc, COLORONCOLOR);
		StretchDIBits(dcc,
			destx, desty, destw, desth,
			imgx, imgy, imgWidth, imgHeight,
			mat_temp.data, bitInfo, DIB_RGB_COLORS, SRCCOPY);
	}
	HDC dc = ::GetDC((*picCtrl).m_hWnd);
	cimage_mfc.BitBlt(dc, 0, 0);
	::ReleaseDC((*picCtrl).m_hWnd, dc);
	cimage_mfc.ReleaseDC();
	cimage_mfc.Destroy();
}

void CSSProDlg::DrawObject()
{
	for (int i = 0; i < Color_info_num; i++)
	{
		int draw_label_num = (int)Color_info[i].Labels.size();
		for (int j = 0; j < draw_label_num; j++)
		{
			rectangle(Show_img, Color_info[i].Labels[j].rect,
				Color_info[i].line_color, 2, 8);
			circle(Show_img, Color_info[i].Labels[j].ct_p, 3,
				Color_info[i].line_color, -1, 8);
		}
	}
}

void CSSProDlg::GetLabelData(Mat* color_img)
{
	Mat tmp_hsv_img;
	cvtColor(Origin_img, tmp_hsv_img, COLOR_BGR2HSV);
	for (int i = 0; i < Color_info_num; i++)
	{
		Scalar lower, upper;
		if (Color_info[i].H_max >= Color_info[i].H_min)
		{
			Mat tmp_bin_img;
			lower = Scalar(Color_info[i].H_min, Color_info[i].S_min, Color_info[i].V_min);
			upper = Scalar(Color_info[i].H_max, Color_info[i].S_max, Color_info[i].V_max);
			inRange(tmp_hsv_img, lower, upper, tmp_bin_img);
			Color_info[i].img = tmp_bin_img;
		}
		else
		{
			Mat tmp_bin_img, tmp_bin_img2;
			lower = Scalar(0, Color_info[i].S_min, Color_info[i].V_min);
			upper = Scalar(Color_info[i].H_max, Color_info[i].S_max, Color_info[i].V_max);
			inRange(tmp_hsv_img, lower, upper, tmp_bin_img);
			lower = Scalar(Color_info[i].H_min, Color_info[i].S_min, Color_info[i].V_min);
			upper = Scalar(180, Color_info[i].S_max, Color_info[i].V_max);
			inRange(tmp_hsv_img, lower, upper, tmp_bin_img2);
			tmp_bin_img |= tmp_bin_img2;
			Color_info[i].img = tmp_bin_img;
		}
		if (i == Color_Index)
			(*color_img) = Color_info[i].img;
		Color_info[i].InitLabels();
		Mat stats, centroids;
		int label_num = connectedComponentsWithStats(Color_info[i].img, Color_info[i].Label, stats, centroids, LINE_8, CV_32S, CCL_DEFAULT);
		int* p_stats = (int*)stats.data;
		double* p_centroids = (double*)centroids.data;
		for (int j = 1; j < label_num; j++)
		{
			LabelData _label = LabelData();
			int x = *(p_stats + j * 5 + CC_STAT_LEFT);
			int y = *(p_stats + j * 5 + CC_STAT_TOP);
			int w = *(p_stats + j * 5 + CC_STAT_WIDTH);
			int h = *(p_stats + j * 5 + CC_STAT_HEIGHT);
			int area = *(p_stats + j * 5 + CC_STAT_AREA);
			double ct_x = *(p_centroids + j * 2 + 0);
			double ct_y = *(p_centroids + j * 2 + 1);
			_label.rect = Rect(x, y, w, h);
			_label.area = area;
			_label.ct_p = Point2d(ct_x, ct_y);
			_label.label_idx = j;
			if (CheckLabelStats(i, _label))
			{
				Color_info[i].Labels.push_back(_label);
			}
		}
	}
}

bool CSSProDlg::CheckLabelStats(int color_index, LabelData labeldata)
{
	bool result = false;
	if (color_index == 0)
	{
		if (labeldata.area > 1000)
			result = true;
	}
	else if (color_index == 1)
	{
		if (labeldata.area > 500)
			result = true;
	}
	else if (color_index == 2)
	{
		if (labeldata.area > 300)
			result = true;
	}
	else
	{
		if (labeldata.area > 500)
			result = true;
	}
	return result;
}
