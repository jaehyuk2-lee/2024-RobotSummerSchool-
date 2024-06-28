#pragma once

#include "stdafx.h"
#include "SSPro.h"
#include "SSProDlg.h"

void CSSProDlg::send_data_to_dsp()
{
	//static int init_cnt = 0;
	static int send_cnt_10ms = 0;
	static int Send_wait_cnt = 0;
	int send_max_cnt = 4;
	if (comm_1 && comm_1->m_bIsOpenned)
	{
		CString str = _T(";");
		if (SendNReturn)
		{
			SendNReturn = false;
			Send_wait_cnt = 0;


			if (send_cnt_10ms == 0)
			{
				if (data_from_typing)
				{
					str = m_str_edit_senddata;
					data_from_typing = false;
					m_str_edit_senddata.Empty();
					SetDlgItemText(IDC_EDIT_SENDDATA, m_str_edit_senddata);
				}
				else
				{
					str = L"Q?;";
					// Q?-> Q?0000;  Q?0001;
				}
				;/* 통신으로 DSP에 보낼 명령어를 만드는 부분*/
			}
			else if (send_cnt_10ms == 1)	// UD Angle
			{
				if (neck_angle_chg_UD)
				{
					int tmp_angle = 20000
						+ cvRound((double)UD_NeckAngle * 55.55555555);
					str.Format(L"PS00024,%d;", tmp_angle);
				}
				/* 통신으로 DSP에 보낼 명령어를 만드는 부분*/
			}
			else if (send_cnt_10ms == 2)	// RL Angle
			{
				if (neck_angle_chg_RL)
				{
					int tmp_angle = 20000
						+ cvRound((double)RL_NeckAngle * 55.55555555);
					str.Format(L"PS00023,%d;", tmp_angle);
				}
				/* 통신으로 DSP에 보낼 명령어를 만드는 부분*/
			}
			else if (send_cnt_10ms == 3)
			{
				if (Q_state && MotionCmd != M_NONE)
				{
					switch (MotionCmd)
					{
						;
					}
					MotionCmd = M_NONE;
				}
			}

			if (++send_cnt_10ms >= send_max_cnt)
				send_cnt_10ms = 0;

			comm_1->Send(str, str.GetLength());	// send data to comm1
			str += _T("\r\n");
			str = L"S>>" + str;
			m_edit_comlog.ReplaceSel(str); // view state in edit box
			m_edit_comlog.LineScroll(m_edit_comlog.GetLineCount());
			str.ReleaseBuffer();
		}
		else
		{
			if (Send_wait_cnt++ > 100)
			{
				SendNReturn = true;
				Send_wait_cnt = 0;
			}
		}
	}
}

UINT CommThread(LPVOID arg)
{
	CSSProDlg* pDlg = (CSSProDlg*)arg;

	extern short	g_nRemoteStatus;
	DWORD		ErrorFlags;
	COMSTAT		ComStat1;
	DWORD		EvtMask;
	char		buf[MAXBUF];
	DWORD		Length;
	int	size;
	int	insize = 0;
	static int Comm1_finsh;

	while (1)
	{
		WaitForSingleObject(pDlg->m_receive_thread->m_hThread, 1);
		if (pDlg->comm_1 && pDlg->comm_1->m_bIsOpenned) // Get data from DSP
		{
			Comm1_finsh = 1;

			EvtMask = 0;
			Length = 0;
			insize = 0;
			memset(buf, '\0', MAXBUF);

			WaitCommEvent(pDlg->comm_1->m_hComDev, &EvtMask, NULL);

			ClearCommError(pDlg->comm_1->m_hComDev, &ErrorFlags, &ComStat1);

			if ((EvtMask & EV_RXCHAR) && ComStat1.cbInQue)
			{
				if (ComStat1.cbInQue > MAXBUF)
					size = MAXBUF;
				else
					size = ComStat1.cbInQue;

				do
				{
					ClearCommError(pDlg->comm_1->m_hComDev, &ErrorFlags, &ComStat1);
					if (!ReadFile(pDlg->comm_1->m_hComDev, buf + insize, size, &Length, &(pDlg->comm_1->m_OLR)))
					{
						// 에러
						TRACE("Error in ReadFile\n");
						if (GetLastError() == ERROR_IO_PENDING)
						{
							if (WaitForSingleObject(pDlg->comm_1->m_OLR.hEvent, 1000) != WAIT_OBJECT_0)	Length = 0;
							else	GetOverlappedResult(pDlg->comm_1->m_hComDev, &(pDlg->comm_1->m_OLR), &Length, FALSE);
						}
						else
							Length = 0;
					}
					insize += Length;
				} while ((Length != 0) && (insize < size));

				ClearCommError(pDlg->comm_1->m_hComDev, &ErrorFlags, &ComStat1);

				if (pDlg->comm_1->m_nLength + insize > MAXBUF * 2)
					insize = (pDlg->comm_1->m_nLength + insize) - MAXBUF * 2;

				pDlg->comm_1->m_pEvent->ResetEvent();
				memcpy(pDlg->comm_1->m_sInBuf + pDlg->comm_1->m_nLength, buf, insize);
				pDlg->comm_1->m_nLength += insize;
				pDlg->comm_1->m_pEvent->SetEvent();
				LPARAM temp = (LPARAM)pDlg->comm_1;
				SendMessage(pDlg->comm_1->m_hWnd, WM_DSP_RECEIVE, pDlg->comm_1->m_nLength, temp);
				pDlg->comm_1->m_send_return_flg = false;
			}
		}

		else if (Comm1_finsh == 1)	// close
		{
			WaitForSingleObject(pDlg->m_receive_thread->m_hThread, 1);
			Comm1_finsh = 0;
			PurgeComm(pDlg->comm_1->m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
			LPARAM temp = (LPARAM)pDlg->comm_1;
			SendMessage(pDlg->comm_1->m_hWnd, WM_DSP_CLOSE, 0, temp);
		}
		WaitForSingleObject(pDlg->m_receive_thread->m_hThread, 5);
	}
	return 0;
}

UINT CommSendThread(LPVOID arg)
{
	CSSProDlg* pDlg = (CSSProDlg*)arg;

	while (1)
	{
		WaitForSingleObject(pDlg->m_send_thread->m_hThread, 1);
		if (pDlg->comm_1 && pDlg->comm_1->m_bIsOpenned)
		{
			pDlg->send_data_to_dsp();
		}
	}
}

LRESULT CSSProDlg::OnThreadClosed(WPARAM length, LPARAM lpara)
{
	//overlapped close handle
	((CSerialComm*)lpara)->HandleClose();
	delete ((CSerialComm*)lpara);

	return 0;
}

LRESULT CSSProDlg::OnReceive(WPARAM length, LPARAM lpara)
{

	char data[1000]; // data buf
	memset(data, 0, sizeof(data)); // set

	if (comm_1)
	{
		if (comm_1->m_send_return_flg)
		{
			int werf = 0;
		}

		if (length > 1000) // 중단점을 걸 때, 수신 버퍼에 계속 쌓이는 현상을 막아줌
		{
			comm_1->m_nLength = 0;
			return 0;
		}
		comm_1->Receive(data, (int)length); // get data

		CString str;
		data[length] = _T('\0');

		int idx = 0;
		for (int i = 0; i < (int)length; i++)	str += data[i];

		if (str.Find(L";") != -1 || str.Find(L":") != -1)
		{
			str = remain_data + str;
			str += _T("\r\n");
			remain_data = _T("");
			SendNReturn = true;

			if (str.Find(L"Q?") != -1)
			{
				int Q_check_limit = 10;
				static int Q_check_num = 0;
				if (str.Find(L"Q?0000;") != -1)
				{
					if (++Q_check_num >= Q_check_limit)
					{
						Q_state = true;
						Q_check_num = Q_check_limit;
					}
				}
				else if (str.Find(L"Q?0001;") != -1)
				{
					Q_state = false;
					Q_check_num = 0;
				}
			}
			str = L"R<<" + str;
			m_edit_comlog.ReplaceSel(str); // view state in edit box
			m_edit_comlog.LineScroll(m_edit_comlog.GetLineCount());
			///////	Log Auto Erase	///
			GetDlgItemText(IDC_EDIT_COMLOG, str);
			int TMP_length = str.GetLength();
			str.ReleaseBuffer();
			int TMP_Limit = m_edit_comlog.GetLimitText() - 100;
			if (TMP_length > TMP_Limit)
			{
				str.Empty();
				SetDlgItemText(IDC_EDIT_COMLOG, str);
			}
		}
		else
		{
			remain_data += str;
		}
	}
	return 0;
}
