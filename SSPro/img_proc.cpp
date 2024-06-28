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
	Mat tmp_extracted_img;
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