// NB6SlotDlg.h : CNB6SlotDlg �̐錾

#pragma once

#include "resource.h"       // ���C�� �V���{��
#include <atlhost.h>
#include "SaveDataStruct.h"
#include "data.h"

// CNB6SlotDlg

class CNB6SlotDlg : 
	public CAxDialogImpl<CNB6SlotDlg>
{
public:
	int m_Current;
	CNB6SlotDlg()
	{
		m_Current = -1;
	}

	~CNB6SlotDlg()
	{
	}

	enum { IDD = IDD_NB6SLOT };

BEGIN_MSG_MAP(CNB6SlotDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDC_SLOT1, BN_CLICKED, OnClicked)
	COMMAND_HANDLER(IDC_SLOT2, BN_CLICKED, OnClicked)
	COMMAND_HANDLER(IDC_SLOT3, BN_CLICKED, OnClicked)
	COMMAND_HANDLER(IDC_SLOT4, BN_CLICKED, OnClicked)
	COMMAND_HANDLER(IDC_SLOT5, BN_CLICKED, OnClicked)
	COMMAND_HANDLER(IDC_SLOT6, BN_CLICKED, OnClicked)
	COMMAND_HANDLER(IDC_SLOT7, BN_CLICKED, OnClicked)
	COMMAND_HANDLER(IDC_SLOT8, BN_CLICKED, OnClicked)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	CHAIN_MSG_MAP(CAxDialogImpl<CNB6SlotDlg>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);(& )

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CNB6SlotDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);

		const char* scenario[] = 
		{
			"�M���̒a��",
			"�����Ԃ̍���",
			"�M����͖�",
			"�{�\���̕�",
			"�M������",
			"�փ����O��",
		};
		const char* difficult[] = {"�y","��","��","��","��",};
		const char* season[] = {"�t","��","�H","�~",};

		BYTE *lpByte = (BYTE*)lParam;
		WORD *used = (WORD*)(lpByte + 0x0b);
		NB6HEAD *pHead;
		ATLASSERT(sizeof(NB6HEAD) == 18);

		for(int i = 0;i < 8;i++)
		{
			char text[256];
            pHead = (NB6HEAD*)(lpByte + i * 158 + 0x1f);
			if(used[i])
			{
				wsprintf(text,"%d:%d�N %s %13s %13s %s��",i + 1,1454 + pHead->year,
					season[pHead->season],pHead->name,scenario[pHead->scenario],difficult[pHead->difficulty]);
				SetDlgItemText(IDC_SLOT1 + i,text);
			}else
				SetDlgItemText(IDC_SLOT1 + i,"���g�p");
		}

		return 1;  // �V�X�e���Ńt�H�[�J�X��ݒ肵�܂��B
	}

	LRESULT OnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		if(m_Current == (wID - IDC_SLOT1))
			EndDialog(wID);
		m_Current = (wID - IDC_SLOT1);
		return 0;
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		if(m_Current != -1)
            EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
};

