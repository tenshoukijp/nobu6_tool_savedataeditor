#pragma once

#include "SaveDataStruct.h"
#include "NB6SlotDlg.h"
#include "CustomDef.h"

#define numof(array) (sizeof(array)/sizeof(array[0]))

#pragma warning(push)
#pragma warning(disable:4996)

/******************************************************************************************
	AttachDlg
******************************************************************************************/

enum ATTACHTYPE {NB6BUSHOU = 1};
class AttachDlg : 
	public CAxDialogImpl<AttachDlg>
{
public:
	AttachDlg(ATTACHTYPE Type){
		m_Mode = Type;
	}
	~AttachDlg(){}

	enum { IDD = IDD_ATTACHDLG };

BEGIN_MSG_MAP_EX(AttachDlg)
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	MSG_WM_INITDIALOG(OnInitDialog)
	COMMAND_HANDLER_EX(IDC_LIST1, LBN_SELCHANGE, OnListSelChange)
	COMMAND_HANDLER_EX(IDC_COMBO2, CBN_SELCHANGE, OnComboSelChange)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
END_MSG_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void ComboUpdate();
	//NB6��r�p
	struct NB6CompareAttach : std::binary_function<int,int,bool>
	{
		NB6CompareAttach(BYTE *base,NB6GENERAL* old)
		{
            m_old = old;
			m_lpByte = base;
		}
		bool operator()(int a, int b)const
		{
			NB6GENERAL* lpNge = (NB6GENERAL*)(m_lpByte + 2786);
			NB6DAIMYO* lpNd = (NB6DAIMYO*)(m_lpByte + 0xc5ca);

			NB6GENERAL* aa = &lpNge[lpNd[a].attach - 1];
			NB6GENERAL* bb = &lpNge[lpNd[b].attach - 1];
			if(m_old->blood != 0xFF)
			{
				if(aa->blood == m_old->blood)
					return true;
				if(bb->blood == m_old->blood)
					return false;
			}
			int x = abs(aa->affinity - m_old->affinity);
			int aaa = x > 8 ? 16 - x:x;
			int y = abs(bb->affinity - m_old->affinity);
			int bbb = y > 8 ? 16 - y:y;
			return (aaa < bbb);
		}
		BYTE* m_lpByte;
		NB6GENERAL* m_old;
	};

	void OnListSelChange(UINT uNotifyCode, int nID, HWND hWndCtl)
	{
		ComboUpdate();
	}

	void OnComboSelChange(UINT uNotifyCode, int nID, HWND hWndCtl)
	{
		int index = m_combor.GetCurSel();
		if(index == -1)return;
		int rItem = m_combor.GetItemData(index);

		int count = m_list.GetCount();
		if(count == LB_ERR || count == 0)return;

		// �d���喼�����Z�b�g���邩�ǂ������O�Ƀ��[�U�[�ɖ₤
		int ret ;
		ret = ::MessageBox( NULL , "�������ړ�����ɂ�����A\n�d���N�������Z�b�g���܂����H" , "�d���N�����Z�b�g�H" , MB_YESNO | MB_APPLMODAL ) ;
		if( ret == IDYES ) {
			isWorkResetOnAttachDaimyo = true; // �喼�ύX�Ŏd���N�������Z�b�g
		} else {
			isWorkResetOnAttachDaimyo = false; // �d���N���͂��̂܂�
		}
		
		for(int i = 0;i != count;i++)
		{
			if(m_list.GetSel(i) > 0)
			{
				int lItem =	m_list.GetItemData(i);
				switch(m_Mode)
				{
				case NB6BUSHOU:
					NB6AttachDaimyo(lItem,rItem);
					break;
				}
			}
		}
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		EndDialog(0);
		return 0;
	}
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

private:

	bool isWorkResetOnAttachDaimyo;

	void NB6AttachDaimyo(int bushou,int daimyo)
	{
		NB6GENERAL* lpNge = (NB6GENERAL*)(m_lpByte + 2786);
		NB6DAIMYO* lpNd = (NB6DAIMYO*)(m_lpByte + 0xc5ca);
		NB6GENERALNAME* lpNgen = (NB6GENERALNAME*)(m_lpByte + 27790);
		NB6CASTLE* lpNc = (NB6CASTLE*)(m_lpByte + 37366);
		NB6GENERAL* lpLeft = &lpNge[bushou];
		NB6DAIMYO* lpRight = &lpNd[daimyo];
		lpLeft->attach = daimyo + 1;
		lpLeft->gundan = lpNge[lpRight->attach - 1].gundan;
		lpLeft->castle = lpNge[lpRight->attach - 1].castle;

		// �d���N�����Z�b�g�₤���H ��Yes��������c
		if( isWorkResetOnAttachDaimyo ) {
			lpLeft->work = 0; // �d���N�������Z�b�g
		}

		lpNgen[bushou].State = 2;
		//������
		lpLeft->rise = 0xFFFF;
		lpLeft->bRise = 0;

		//���Q�l�B�����X�g�X�V
		for(int i = 0;i != 214;i++)
		{
			if(lpNc[i].master - 1 == bushou)
				lpNc[i].master = lpLeft->next;
			if(lpNc[i].ronin - 1 == bushou)
				lpNc[i].ronin = lpLeft->next;
		}
		for(int i = 0;i != 72;i++)
		{
			if(lpNd[i].retire - 1 == bushou)
				lpNd[i].retire = lpLeft->next;
		}

		//�ړ�������
		if(lpLeft->prev != 0xFFFF && lpNge[lpLeft->prev - 1].next == bushou + 1)
			lpNge[lpLeft->prev - 1].next = lpLeft->next;
		if(lpLeft->next != 0xFFFF)
			lpNge[lpLeft->next - 1].prev = lpLeft->prev;

		//�ړ��揈��
		WORD next = lpNge[lpRight->attach - 1].next;
		lpNge[lpRight->attach - 1].next = bushou + 1;
		if(next != 0xFFFF)
			lpNge[next - 1].prev = bushou + 1;
		lpLeft->prev = lpRight->attach;
		lpLeft->next = next;
	}

	ATTACHTYPE m_Mode;
	BYTE *m_lpByte;
	CComboBox m_combor;
	CListBox m_list;
};

/******************************************************************************************
	CData to nakamatati
******************************************************************************************/

class CData
{
public:
	CData(void)
	{
		m_Slot = 0;
		m_lp = NULL;
		m_fs = 0;
	}

	virtual ~CData(void)
	{
		Clear();
	}

	void Clear()
	{
		if(m_lp)
			GlobalFree(m_lp);
		m_lp = NULL;
		m_fs = 0;
	}

	void SetParam(LPVOID lp,DWORD dwSize,FILETIME ft,LPCSTR fn,int slot)
	{
		m_lp = lp;
		m_fs = dwSize;
		m_ft = ft;
		m_Filename = fn;
		m_Slot = slot;
	}

	BOOL SaveFile(LPCSTR fname,LPCVOID lp,DWORD dwSize) const
	{
		HANDLE hf;
		BOOL bResult;
		DWORD dwResult;

		hf = ::CreateFile(fname,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hf == INVALID_HANDLE_VALUE)
			return FALSE;

		bResult = ::WriteFile(hf,lp,dwSize,&dwResult,NULL);
            
		::CloseHandle(hf);
		if(!bResult)
			return FALSE;

        return TRUE;
	}

	BOOL Save(LPSTR szFile = NULL)
	{
		CString fn = szFile;
		if(szFile == NULL)
			fn = m_Filename;
		if(m_lp == NULL || m_fs == 0)
			return FALSE;

		BOOL res = SaveFile(fn,m_lp,m_fs);
		HANDLE hf = ::CreateFile(m_Filename,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hf != INVALID_HANDLE_VALUE)
		{
			GetFileTime(hf,NULL,NULL,&m_ft);
			::CloseHandle(hf);
		}

		return res;
	}

	virtual int GetTabCount() const = 0;
	virtual void GetTabText(int nTab,LPSTR pszText,int nMax) const = 0;

	virtual void SetColumn(CGridCtrl* pGrid,int nTab) = 0;
	virtual void SetGrid(CGridCtrl* pGrid,int nTab) = 0;
	virtual void GetGrid(CGridCtrl* pGrid,int nTab) = 0;

	// Return changed tab
	// -1 fail
	virtual int Command(WORD wID,HWND hwnd) = 0;
	
	LPVOID m_lp;
	DWORD m_fs;
	FILETIME m_ft;
	CString m_Filename;
	int m_Slot;
};

class CNB6Data :public CData
{
	enum NB6TAB {TDAIMYO = 0,TGUNDAN = 1,TSIRO = 2,TBUSHO = 3,TKAHO = 4,TKANI = 5};
public:
/*	CNB6Data(void)
	{
	}

	~CNB6Data(void)
	{
		Clear();
	}*/

	int GetTabCount() const
	{
		return 6;
	}

	void GetTabText(int nTab,LPSTR pszText,int nMax) const
	{
		ATLASSERT(m_lp && m_fs);
		ATLASSERT(nTab < GetTabCount());
		const char* pszNB6TabString[] = {"�喼","�R�c","��","����","�ƕ�","����"};

		if(nTab < numof(pszNB6TabString))
            strncpy(pszText,pszNB6TabString[nTab],nMax);
		else *pszText = '\0';
		return;
	}

	int Command(WORD wID, HWND hwnd)
	{
        BYTE *lpByte = (BYTE*)m_lp + (105416 * m_Slot) + 0x50F;
		NB6GENERAL* lpNge = (NB6GENERAL*)(lpByte + 2786);
		NB6GENERALNAME* lpNgen = (NB6GENERALNAME*)(lpByte + 27790);
		NB6CASTLE* lpNc = (NB6CASTLE*)(lpByte + 37366);
		NB6CASTLEMAX* lpNcm = (NB6CASTLEMAX*)(lpByte + 44428);
		NB6DAIMYO* lpNd = (NB6DAIMYO*)(lpByte + 0xc5ca);
		switch(wID)
		{
		case ID_MAX_GRAIN:
			for(int i = 0;i != 214;i++)
				lpNc[i].grain = lpNcm[i].maxgrain * 10;
			return TSIRO;
		case ID_MAX_TOWN:
			for(int i = 0;i != 214;i++)
				lpNc[i].town = lpNcm[i].maxtown;
			return TSIRO;
		case ID_MAX_BUILD:
			for(int i = 0;i != 214;i++)
				lpNc[i].build = (lpNc[i].bLarge) ? 250:200;
			return TSIRO;
		case ID_MAX_LOYAL:
			for(int i = 0;i != 214;i++)
				lpNc[i].loyal = 100;
			return TSIRO;
		case ID_MAX_EXP:
			for(int i = 0;i != 532;i++)
				lpNge[i].expgov = lpNge[i].expbat = lpNge[i].expint = 2000;
			return TBUSHO;
		case ID_CALC_EXP:
			for(int i = 0;i != 532;i++)
			{
				int age = *(WORD*)lpByte - lpNge[i].birth;
				int life = (lpNge[i].life + 1) * 12;
				if(age < 0)continue;
				//500�`2000�͈̔�
				int exp = min(((age - 10) * 1500) / life + 500,2000);

				lpNge[i].expgov = lpNge[i].expbat = lpNge[i].expint = exp;
			}
			return TBUSHO;
		case ID_CALC_EXP2:
			for(int i = 0;i != 532;i++)
			{
				int age = *(WORD*)lpByte - lpNge[i].birth;
				int life = (lpNge[i].life + 1) * 12;
				if(age < 0)continue;
				//500�`2000�͈̔�
				int exp = min(((age - 10) * 1500) / life + 500,2000);

				lpNge[i].expgov = max(lpNge[i].expgov,exp);
				lpNge[i].expbat = max(lpNge[i].expbat,exp);
				lpNge[i].expint = max(lpNge[i].expint,exp);;
			}
			return TBUSHO;
		case ID_YOUNG:
			for(int i = 0;i != 532;i++)
			{
				int year = *(WORD*)lpByte;
				int birth = lpNge[i].birth + 10;
				if(birth > year)
					birth = year;
				lpNge[i].birth = birth;
			}
			return TBUSHO;
		case ID_CHANGEATTACH:
			{
				AttachDlg dlg(NB6BUSHOU);
				dlg.DoModal(hwnd,(LPARAM)lpByte);
				return TBUSHO;
			}
		case ID_SOLDIER:
			for(int i = 0;i != 532;i++)
			{
				if(lpNgen[i].State >= 0 && lpNgen[i].State <= 2)
				{
					int sol = lpNge[i].soldier;
					if(lpNgen[i].position == 1 || lpNgen[i].position == 2)//�喼 �h�V
						sol = max(100,sol);
					if(lpNgen[i].position == 3)//�ƘV
						sol = max(75,sol);
					if(lpNgen[i].position == 4)//����
						sol = max(55,sol);
					if(lpNgen[i].position == 5)//���叫
						sol = max(40,sol);
					if(lpNgen[i].position == 6)//���y��
						sol = max(30,sol);
					lpNge[i].soldier = sol;
					lpNge[i].training = max(75,lpNge[i].training);
					lpNge[i].solloyal = max(75,lpNge[i].solloyal);
				}
			}
			return TBUSHO;
		case ID_SENIORITY:
			for(int i = 0;i != 532;i++)
			{
				int age = *(WORD*)lpByte - lpNge[i].birth;
				int life = (lpNge[i].life + 1) * 12;
				if(age < 0)continue;

				int exp = (age * 100) / life;

				if(lpNgen[i].State >= 1 && lpNgen[i].State <= 2)
				{
					if(exp > 35 && lpNgen[i].position == 6)
						lpNgen[i].position = 5;
					if(exp > 45 && lpNgen[i].position == 5)
						lpNgen[i].position = 4;
					if(exp > 60 && lpNgen[i].position == 4)
						lpNgen[i].position = 3;
					if(exp > 80 && lpNgen[i].position == 3)
						lpNgen[i].position = 2;
				}
			}
			return TBUSHO;
		case ID_DUMP:
			SaveFile("dumphead.bin",lpByte,0xAE2);
			SaveFile("dumptail.bin",lpByte + 0xEDB0,105416 - 0xEDB0);
			break;
		case ID_TEST:
			{
			}
			return -1;
		}
		return -1;
	}

	static void NB6NameNormalize(CString &str,bool bDecode)
	{
		const char* chosokabe = "\xec\x8c\xec\x8d\xec\x8e";//���@�䕔
		const char* kosokabe = "\xec\x8f\xec\x8d\xec\x8e";//���@�䕔
		if(bDecode)
		{
			str.Replace(chosokabe,"���@�䕔");
			str.Replace(kosokabe,"���@�䕔");
		}else
		{
			str.Replace("���@�䕔",chosokabe);
			str.Replace("���@�䕔",kosokabe);
		}
	}
private:
	void SetColumn(CGridCtrl* pGrid,int nTab)
	{
		//����
		pGrid->AddColumn("Index",35,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
		switch(nTab)
		{
		case TDAIMYO:
			pGrid->AddColumn("�喼�ԍ�",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�喼",75);
			pGrid->AddColumn("���l",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�G�Α喼",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
			pGrid->AddColumn("�F�D�喼",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
			pGrid->AddColumn("�z��",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�]��",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("��E",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
 			pGrid->AddColumnLookup("��E",0,"");
 			pGrid->AddColumnLookup("��E",1,"���Α叫�R");
 			pGrid->AddColumnLookup("��E",2,"�Ǘ�");
 			pGrid->AddColumnLookup("��E",4,"�֓��Ǘ�");
 			pGrid->AddColumnLookup("��E",8,"���B�T��");
 			pGrid->AddColumnLookup("��E",0x10,"�����T��");
 			pGrid->AddColumnLookup("��E",0x20,"��B�T��");
 			pGrid->AddColumnLookup("��E",0x40,"�H�B�T��");
            break;
		case TGUNDAN://�R�c
			pGrid->AddColumn("�R�c�ԍ�",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",60);
			pGrid->AddColumn("�R�c�����ԍ�",80,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",60);
			pGrid->AddColumn("�R�c��",75);
			pGrid->AddColumn("�s����",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("��",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("��",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�R�n",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�S�C",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("��E",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
 			pGrid->AddColumnLookup("��E",0,"");
 			pGrid->AddColumnLookup("��E",1,"���Α叫�R");
 			pGrid->AddColumnLookup("��E",2,"�Ǘ�");
 			pGrid->AddColumnLookup("��E",3,"�֓��Ǘ�");
 			pGrid->AddColumnLookup("��E",4,"���B�T��");
 			pGrid->AddColumnLookup("��E",5,"�����T��");
 			pGrid->AddColumnLookup("��E",6,"��B�T��");
 			pGrid->AddColumnLookup("��E",7,"�H�B�T��");
			break;
		case TSIRO://��
			pGrid->AddColumn("��ԍ�",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�閼",60,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("����",90);
			pGrid->AddColumn("��s",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�΍�",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�Ő�/10",45,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�ŏ�",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�l��",60,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("������",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT);
 			pGrid->AddColumnLookup("����",2,"A");
 			pGrid->AddColumnLookup("����",1,"B");
			pGrid->AddColumnLookup("����",0,"C");
			pGrid->AddColumn("�b��",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�n�Y",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�`",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("��R",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("���R",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�Ꝅ",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�w�ʒu",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�x�ʒu",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
//			pGrid->AddColumn("���",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
           break;
		case TBUSHO://����
			pGrid->AddColumn("�����ԍ�",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("��",50,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("��",50,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("����",50);
			pGrid->AddColumn("�g��",50,CGridCtrl::EDIT_DROPDOWNLIST);
			pGrid->AddColumn("��ԍ�",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("�g��",0,"�B��");
			pGrid->AddColumnLookup("�g��",1,"�喼");
 			pGrid->AddColumnLookup("�g��",2,"�h�V");
			pGrid->AddColumnLookup("�g��",3,"�ƘV");
 			pGrid->AddColumnLookup("�g��",4,"����");
			pGrid->AddColumnLookup("�g��",5,"���叫");
			pGrid->AddColumnLookup("�g��",6,"���y��");
			pGrid->AddColumn("���",30);
			pGrid->AddColumn("���N",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�d��",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("��]",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("���n",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("���K",30,CGridCtrl::EDIT_DROPDOWNLIST);
 			pGrid->AddColumnLookup("���K",2,"�`");
			pGrid->AddColumnLookup("���K",1,"�a");
 			pGrid->AddColumnLookup("���K",0,"�b");
			pGrid->AddColumn("��n",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("���",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("��K",30,CGridCtrl::EDIT_DROPDOWNLIST);
 			pGrid->AddColumnLookup("��K",2,"�`");
			pGrid->AddColumnLookup("��K",1,"�a");
 			pGrid->AddColumnLookup("��K",0,"�b");
			pGrid->AddColumn("�q�n",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�q��",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�q�K",30,CGridCtrl::EDIT_DROPDOWNLIST);
 			pGrid->AddColumnLookup("�q�K",2,"�`");
			pGrid->AddColumnLookup("�q�K",1,"�a");
 			pGrid->AddColumnLookup("�q�K",0,"�b");
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�M��",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�P��",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�m�C",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",40,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("����",0,"���y");
			pGrid->AddColumnLookup("����",1,"�R�n");
 			pGrid->AddColumnLookup("����",2,"�S�C");
			pGrid->AddColumnLookup("����",3,"�R�S");
			pGrid->AddColumnLookup("����",0xFF,"����");
			pGrid->AddColumn("���K",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumnLookup("���K",5,"�r");
			pGrid->AddColumnLookup("���K",4,"�`");
			pGrid->AddColumnLookup("���K",3,"�a");
 			pGrid->AddColumnLookup("���K",2,"�b");
			pGrid->AddColumnLookup("���K",1,"�c");
 			pGrid->AddColumnLookup("���K",0,"�d");
			pGrid->AddColumn("�R�K",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumnLookup("�R�K",5,"�r");
			pGrid->AddColumnLookup("�R�K",4,"�`");
			pGrid->AddColumnLookup("�R�K",3,"�a");
 			pGrid->AddColumnLookup("�R�K",2,"�b");
			pGrid->AddColumnLookup("�R�K",1,"�c");
 			pGrid->AddColumnLookup("�R�K",0,"�d");
			pGrid->AddColumn("�S�K",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("�S�K",5,"�r");
			pGrid->AddColumnLookup("�S�K",4,"�`");
 			pGrid->AddColumnLookup("�S�K",3,"�a");
			pGrid->AddColumnLookup("�S�K",2,"�b");
			pGrid->AddColumnLookup("�S�K",1,"�c");
			pGrid->AddColumnLookup("�S�K",0,"�d");
			pGrid->AddColumn("���K",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("���K",5,"�r");
			pGrid->AddColumnLookup("���K",4,"�`");
 			pGrid->AddColumnLookup("���K",3,"�a");
			pGrid->AddColumnLookup("���K",2,"�b");
			pGrid->AddColumnLookup("���K",1,"�c");
			pGrid->AddColumnLookup("���K",0,"�d");
			pGrid->AddColumn("�ꊅ",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�ÎE",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("���o",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�ِ�",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�ē�",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);

			pGrid->AddColumn("������",80,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
			//pGrid->AddColumn("��",80);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�v�l",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�`��",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�E��",60,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("�E��",0,"�|");
			pGrid->AddColumnLookup("�E��",0x10,"�E��");
 			pGrid->AddColumnLookup("�E��",0x20,"���l");
			pGrid->AddColumnLookup("�E��",0x30,"����");
			pGrid->AddColumnLookup("�E��",0x40,"�m��");
#ifdef TENSHOUKI_JP
			pGrid->AddColumnLookup("�E��",0x55,"-TSMod-");
			pGrid->AddColumnLookup("�E��",0x50,"�C��");
			pGrid->AddColumnLookup("�E��",0x60,"����");
			pGrid->AddColumnLookup("�E��",0x70,"���l");
			pGrid->AddColumnLookup("�E��",0x80,"����");
			pGrid->AddColumnLookup("�E��",0x90,"�؎x�O");
			pGrid->AddColumnLookup("�E��",0xA0,"�R�m");
			pGrid->AddColumnLookup("�E��",0xB0,"���p�m");
			pGrid->AddColumnLookup("�E��",0xC0,"����");
			pGrid->AddColumnLookup("�E��",0xD0,"�|");
#endif
			pGrid->AddColumn("��C",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�R�S",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�S�D",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�E��",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�Ɨ�",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�s��",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�⍦",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
            break;
		case TKAHO://�ƕ�
			pGrid->AddColumn("�ƕ�ԍ�",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("���O",80,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("����",70,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("����",0,"���q");
			pGrid->AddColumnLookup("����",1,"����");
 			pGrid->AddColumnLookup("����",2,"����");
			pGrid->AddColumnLookup("����",3,"����");
 			pGrid->AddColumnLookup("����",4,"�ԓ�");
			pGrid->AddColumnLookup("����",5,"����");
 			pGrid->AddColumnLookup("����",6,"��");
			pGrid->AddColumnLookup("����",7,"�");
 			pGrid->AddColumnLookup("����",8,"�n");
			pGrid->AddColumnLookup("����",9,"��");
 			pGrid->AddColumnLookup("����",0xA,"�w�H�D");
			pGrid->AddColumnLookup("����",0xB,"���q�{");
 			pGrid->AddColumnLookup("����",0xC,"�����Ԗ{");
			pGrid->AddColumnLookup("����",0xD,"����");
 			pGrid->AddColumnLookup("����",0xE,"�n���V");
			pGrid->AddColumnLookup("����",0xF,"��؎��v");
 			pGrid->AddColumnLookup("����",0x10,"���n��");
			pGrid->AddColumnLookup("����",0x11,"�\����");
 			pGrid->AddColumnLookup("����",0x12,"����");
			pGrid->AddColumnLookup("����",0x13,"�ʎ蔠");
 			pGrid->AddColumnLookup("����",0x14,"�r�[�h���t");
			pGrid->AddColumnLookup("����",0x15,"���ዾ");
 			pGrid->AddColumnLookup("����",0x16,"���\����");
			pGrid->AddColumnLookup("����",0x17,"����");
			pGrid->AddColumn("�摜",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("���L�����ԍ�",80,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�㏸�l",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("���o��",45,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("����",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			break;
		case TKANI://����
			pGrid->AddColumn("���ʔԍ�",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("���O",80,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("����",70,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("����",0,"�����");
			pGrid->AddColumnLookup("����",1,"�]���");
 			pGrid->AddColumnLookup("����",2,"�����");
			pGrid->AddColumnLookup("����",3,"�]���");
 			pGrid->AddColumnLookup("����",4,"���O��");
			pGrid->AddColumnLookup("����",5,"�]�O��");
 			pGrid->AddColumnLookup("����",6,"���l�ʏ�");
			pGrid->AddColumnLookup("����",7,"���l�ʉ�");
 			pGrid->AddColumnLookup("����",8,"�]�l�ʏ�");
			pGrid->AddColumnLookup("����",9,"�]�l�ʉ�");
 			pGrid->AddColumnLookup("����",0xA,"���܈ʏ�");
			pGrid->AddColumnLookup("����",0xB,"���܈ʉ�");
 			pGrid->AddColumnLookup("����",0xC,"�]�܈ʏ�");
			pGrid->AddColumnLookup("����",0xD,"�]�܈ʉ�");
 			pGrid->AddColumnLookup("����",0xE,"���Z�ʏ�");
			pGrid->AddColumnLookup("����",0xF,"���Z�ʉ�");
 			pGrid->AddColumnLookup("����",0x10,"�]�Z�ʏ�");
			pGrid->AddColumnLookup("����",0x11,"�]�Z�ʉ�");
 			pGrid->AddColumnLookup("����",0x12,"�����ʏ�");
			pGrid->AddColumnLookup("����",0x13,"�����ʉ�");
 			pGrid->AddColumnLookup("����",0x14,"�]���ʏ�");
			pGrid->AddColumnLookup("����",0x15,"�]���ʉ�");
 			pGrid->AddColumnLookup("����",0x16,"�����ʏ�");
			pGrid->AddColumn("���L�����ԍ�",80,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("�㏸�l",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			break;
		}
	}

	void SetGrid(CGridCtrl* pGrid,int nTab)
	{
		//char szBuf[64];
		CString str;
		BYTE *lpByte = (BYTE*)m_lp + (105416 * m_Slot) + 0x50F;

		ATLASSERT(sizeof(NB6GUNDAN) == 21);
		ATLASSERT(sizeof(NB6CASTLE) == 33);
		ATLASSERT(sizeof(NB6CASTLEMAX) == 8);
		ATLASSERT(sizeof(NB6GENERALNAME) == 18);
		ATLASSERT(sizeof(NB6GENERAL) == 47);
		ATLASSERT(sizeof(NB6DAIMYO) == 22);
		ATLASSERT(sizeof(NB6KAHOU) == 19);
		ATLASSERT(sizeof(NB6KANI) == 18);
		NB6GENERAL* lpNge = (NB6GENERAL*)(lpByte + 2786);
		NB6GENERALNAME* lpNgen = (NB6GENERALNAME*)(lpByte + 27790);
		NB6CASTLE* lpNc = (NB6CASTLE*)(lpByte + 37366);
		NB6CASTLEMAX* lpNcm = (NB6CASTLEMAX*)(lpByte + 44428);
		NB6GUNDAN* lpNg = (NB6GUNDAN*)(lpByte + 46140);
		NB6DAIMYO* lpNd = (NB6DAIMYO*)(lpByte + 0xc5ca);
		NB6KAHOU* lpNkh = (NB6KAHOU*)(lpByte + 0xD334);
		NB6KANI* lpNki = (NB6KANI*)(lpByte + 0xDC1C);

		switch(nTab)
		{
		case TDAIMYO:
			pGrid->ClearColumnLookup("�G�Α喼");
			pGrid->ClearColumnLookup("�F�D�喼");
 			pGrid->AddColumnLookup("�G�Α喼",0,"�| �| �| �|");
			pGrid->AddColumnLookup("�F�D�喼",0,"�| �| �| �|");
 			pGrid->AddColumnLookup("�G�Α喼",0xFF,"�| �| �| �|");//�Ӗ��������Ⴄ��������Ȃ�
			pGrid->AddColumnLookup("�F�D�喼",0xFF,"�| �| �| �|");
			for(int i = 0;i != 72;i++)
			{
				if(lpNd[i].attach == 0xFFFF)continue;
				str.Format("%s%s",lpNgen[lpNd[i].attach - 1].familyname,lpNgen[lpNd[i].attach - 1].fastname);
				NB6NameNormalize(str,true);
 				pGrid->AddColumnLookup("�G�Α喼",i + 1,(LPCSTR)str);
 				pGrid->AddColumnLookup("�F�D�喼",i + 1,(LPCSTR)str);

				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"�喼�ԍ�",i+1);
				pGrid->SetItem(nItem,"�喼",(LPCSTR)str);
				pGrid->SetItem(nItem,"���l",lpNd[i].merchant);
				pGrid->SetItem(nItem,"����",lpNd[i].tyotei);
				pGrid->SetItem(nItem,"�G�Α喼",lpNd[i].hostile);
				pGrid->SetItem(nItem,"�F�D�喼",lpNd[i].friendship);
				pGrid->SetItem(nItem,"�z��",lpNd[i].bPropagate);
				pGrid->SetItem(nItem,"�]��",lpNd[i].bHyotei);
				pGrid->SetItem(nItem,"��E",lpNd[i].position);
			}
			break;
		case TGUNDAN://�R�c
			for(int i = 0;i != 214;i++)
			{
				if(lpNg[i].residence == 0xffff)continue;

				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"�R�c�ԍ�",i+1);
				str.Format("%s��",lpNgen[lpNd[lpNg[i].attach - 1].attach - 1].familyname);
				NB6NameNormalize(str,true);
				pGrid->SetItem(nItem,"����",(LPCSTR)str);
				pGrid->SetItem(nItem,"�R�c�����ԍ�",lpNg[i].number);
				pGrid->SetItem(nItem,"����",lpNc[lpNg[i].residence - 1].name);
				str.Format("%s%s",lpNgen[lpNg[i].leader - 1].familyname,lpNgen[lpNg[i].leader - 1].fastname);
				NB6NameNormalize(str,true);
				pGrid->SetItem(nItem,"�R�c��",(LPCSTR)str);
				pGrid->SetItem(nItem,"�s����",lpNg[i].act);
				pGrid->SetItem(nItem,"��",lpNg[i].money);
				pGrid->SetItem(nItem,"��",lpNg[i].rise);
				pGrid->SetItem(nItem,"�R�n",lpNg[i].horse);
				pGrid->SetItem(nItem,"�S�C",lpNg[i].gun);

				// ���V�i���I�G�f�B�^�[�̃o�O�Ή�
				if ( lpNg[i].position == 15 ) {
					lpNg[i].position = 0;
				}
				pGrid->SetItem(nItem,"��E",lpNg[i].position);
			}
			break;
		case TSIRO://��
			for(int i = 0;i != 214;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"��ԍ�",i+1);
				pGrid->SetItem(nItem,"�閼",lpNc[i].name);
				if(lpNc[i].attach != 0xFFFF)
				{
					NB6GENERALNAME *lpName = &lpNgen[lpNg[lpNc[i].attach - 1].leader - 1];
					str.Format("%s%s�R�c",lpName->familyname,lpName->fastname);
					NB6NameNormalize(str,true);
				}else str = "������";
				pGrid->SetItem(nItem,"����",(LPCSTR)str);
				pGrid->SetItem(nItem,"��s",lpNc[i].build);
				pGrid->SetItem(nItem,"�΍�",lpNc[i].grain);
				pGrid->SetItem(nItem,"����",lpNc[i].town);
				pGrid->SetItem(nItem,"�l��",lpNc[i].population);
				pGrid->SetItem(nItem,"����",lpNc[i].loyal);
				pGrid->SetItem(nItem,"������",lpNc[i].soldier);
				pGrid->SetItem(nItem,"����",lpNc[i].quality);

				pGrid->SetItem(nItem,"�b��",lpNc[i].bSmith);
				pGrid->SetItem(nItem,"�n�Y",lpNc[i].bHorse);
				pGrid->SetItem(nItem,"�`",lpNc[i].bPort);
				pGrid->SetItem(nItem,"����",lpNc[i].blPort);
				pGrid->SetItem(nItem,"��R",lpNc[i].bSilver);
				pGrid->SetItem(nItem,"���R",lpNc[i].bGold);
				pGrid->SetItem(nItem,"�Ꝅ",lpNc[i].bRevolt);
				pGrid->SetItem(nItem,"����",lpNc[i].bLarge);
//				pGrid->SetItem(nItem,"���",lpNc[i].title);

				pGrid->SetItem(nItem,"�Ő�/10",lpNcm[i].maxgrain);
				pGrid->SetItem(nItem,"�ŏ�",lpNcm[i].maxtown);
				pGrid->SetItem(nItem,"�w�ʒu",lpNcm[i].x);
				pGrid->SetItem(nItem,"�x�ʒu",lpNcm[i].y);
			}
			break;
		case TBUSHO://����
			pGrid->ClearColumnLookup("������");
 			pGrid->AddColumnLookup("������",0xFFFF,"����");
			for(int i = 0;i < 72;i++)
			{
				if(lpNd[i].attach == 0xFFFF)continue;
				str.Format("%s%s",lpNgen[lpNd[i].attach - 1].familyname,lpNgen[lpNd[i].attach - 1].fastname);
				NB6NameNormalize(str,true);
	 			pGrid->AddColumnLookup("������",i + 1,(LPCSTR)str);
			}

			for(int i = 0;i != 532;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				str = lpNgen[i].familyname;
				NB6NameNormalize(str,true);
				pGrid->SetItem(nItem,"�����ԍ�",i+1);
				pGrid->SetItem(nItem,"��",(LPCSTR)str);
				pGrid->SetItem(nItem,"��",lpNgen[i].fastname);
				pGrid->SetItem(nItem,"��ԍ�",lpNgen[i].number);
				if(lpNge[i].attach != 0xFFFF && lpNd[lpNge[i].attach - 1].attach  != 0xFFFF)
				{
					str.Format("%s��",lpNgen[lpNd[lpNge[i].attach - 1].attach - 1].familyname);
					NB6NameNormalize(str,true);
					pGrid->SetItem(nItem,"����",(LPCSTR)str);
				}

				pGrid->SetItem(nItem,"�g��",lpNgen[i].position);
				char *tState[] = {"�喼","�R��","����","�B��","�Q�l","�P","undef6","���S"};
				pGrid->SetItem(nItem,"���",tState[lpNgen[i].State & 7]);
				pGrid->SetItem(nItem,"���N",lpNge[i].birth + 1454);
				pGrid->SetItem(nItem,"�d��",lpNge[i].work);
				pGrid->SetItem(nItem,"��]",lpNge[i].ambition);
				pGrid->SetItem(nItem,"����",lpNge[i].charm);
				pGrid->SetItem(nItem,"���n",lpNge[i].expgov);
				pGrid->SetItem(nItem,"����",lpNge[i].maxgov);
				pGrid->SetItem(nItem,"���K",lpNge[i].aptitudegov);
				pGrid->SetItem(nItem,"��n",lpNge[i].expbat);
				pGrid->SetItem(nItem,"���",lpNge[i].maxbat);
				pGrid->SetItem(nItem,"��K",lpNge[i].aptitudebat);
				pGrid->SetItem(nItem,"�q�n",lpNge[i].expint);
				pGrid->SetItem(nItem,"�q��",lpNge[i].maxint);
				pGrid->SetItem(nItem,"�q�K",lpNge[i].aptitudeint);
				pGrid->SetItem(nItem,"����",lpNge[i].loyal);
				pGrid->SetItem(nItem,"�M��",lpNge[i].merits);
				pGrid->SetItem(nItem,"����",lpNge[i].soldier);
				pGrid->SetItem(nItem,"�P��",lpNge[i].training);
				pGrid->SetItem(nItem,"�m�C",lpNge[i].solloyal);
				pGrid->SetItem(nItem,"����",lpNge[i].form);
				//if(i < 500 && lpNge[i].spouse != 0xFF)
				//{
				//	NB6GENERALNAME *lpName;
				//	lpName = &lpNgen[lpNge[i].spouse + 500];
				//	wsprintf(szBuf,"%s%s",lpName->familyname,lpName->fastname);
				//	pGrid->SetItem(nItem,"��",szBuf);
				//}
				pGrid->SetItem(nItem,"���K",lpNge[i].aptitudeasi);
				pGrid->SetItem(nItem,"�R�K",lpNge[i].aptitudekib);
				pGrid->SetItem(nItem,"�S�K",(lpNge[i]._aptitudegun_lo << 2)+lpNge[i]._aptitudegun_hi); // �r�b�g�t�B�[���h���Q�Ɋ���Ă邽�ߗv���Z
				pGrid->SetItem(nItem,"���K",lpNge[i].aptitudesui);
 				pGrid->SetItem(nItem,"�ꊅ",lpNge[i].tech_ikkatsu);
				pGrid->SetItem(nItem,"�ÎE",lpNge[i].tech_ansatsu);
				pGrid->SetItem(nItem,"���o",lpNge[i].tech_ryusyutsu);
				pGrid->SetItem(nItem,"����",lpNge[i].tech_sendou);
				pGrid->SetItem(nItem,"����",lpNge[i].tech_ryugen);
				pGrid->SetItem(nItem,"�ِ�",lpNge[i].tech_benzetsu);
				pGrid->SetItem(nItem,"�ē�",lpNge[i].tech_yakiuchi);
				pGrid->SetItem(nItem,"����",lpNge[i].tech_chohatsu);
				pGrid->SetItem(nItem,"������",lpNge[i].rise);
				pGrid->SetItem(nItem,"����",lpNge[i].blood);
				pGrid->SetItem(nItem,"�v�l",lpNge[i].algo);
				pGrid->SetItem(nItem,"�`��",lpNge[i].duty);
				pGrid->SetItem(nItem,"����",lpNge[i].affinity);
				pGrid->SetItem(nItem,"�E��",lpNge[i].job);
				pGrid->SetItem(nItem,"��C",lpNge[i].biggun);
				pGrid->SetItem(nItem,"�R�S",lpNge[i].horsegun);
				pGrid->SetItem(nItem,"�S�D",lpNge[i].steelship);
				pGrid->SetItem(nItem,"����",lpNge[i].life);
				pGrid->SetItem(nItem,"�E��",lpNge[i].brave);
				pGrid->SetItem(nItem,"�Ɨ�",(lpNge[i]._independence_lo << 2)+lpNge[i]._independence_hi); // �r�b�g�t�B�[���h���Q�Ɋ���Ă邽�ߗv���Z
				pGrid->SetItem(nItem,"����",lpNge[i].tone);
				pGrid->SetItem(nItem,"�s��",lpNge[i].bEnd);
				pGrid->SetItem(nItem,"����",lpNge[i].bRise);
				pGrid->SetItem(nItem,"�⍦",lpNge[i].grudge);
			}
			break;
		case TKAHO://�ƕ�
			for(int i = 0;i < 120;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"�ƕ�ԍ�",i+1);

				pGrid->SetItem(nItem,"���O",lpNkh[i].name);
				pGrid->SetItem(nItem,"����",lpNkh[i].type);
				pGrid->SetItem(nItem,"�摜",lpNkh[i].face);
				pGrid->SetItem(nItem,"���L�����ԍ�",lpNkh[i].attach);
				pGrid->SetItem(nItem,"�㏸�l",lpNkh[i].param);
				pGrid->SetItem(nItem,"���o��",lpNkh[i].bMitojo);
				pGrid->SetItem(nItem,"����",lpNkh[i].bLost);
			}
			break;
		case TKANI://����
			for(int i = 0;i < 250;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"���ʔԍ�",i+1);

				pGrid->SetItem(nItem,"���O",lpNki[i].name);
				pGrid->SetItem(nItem,"����",lpNki[i].rank);
				pGrid->SetItem(nItem,"���L�����ԍ�",lpNki[i].attach);
				pGrid->SetItem(nItem,"�㏸�l",lpNki[i].param);
			}
			break;

		}
	}

	void GetGrid(CGridCtrl* pGrid,int nTab)
	{
		int index,nCount;
		CString str;
		BYTE *lpByte = (BYTE*)m_lp + (105416 * m_Slot) + 0x50F;

		NB6GENERAL* lpNge = (NB6GENERAL*)(lpByte + 2786);
		NB6GENERALNAME* lpNgen = (NB6GENERALNAME*)(lpByte + 27790);
		NB6CASTLE* lpNc = (NB6CASTLE*)(lpByte + 37366);
		NB6CASTLEMAX* lpNcm = (NB6CASTLEMAX*)(lpByte + 44428);
		NB6GUNDAN* lpNg = (NB6GUNDAN*)(lpByte + 46140);
		NB6DAIMYO* lpNd = (NB6DAIMYO*)(lpByte + 0xc5ca);
		NB6KAHOU* lpNkh = (NB6KAHOU*)(lpByte + 0xD334);
		NB6KANI* lpNki = (NB6KANI*)(lpByte + 0xDC1C);

		nCount = pGrid->GetRowCount();

		switch(nTab)
		{
		case TDAIMYO:
			for(int i = 0;i != nCount;i++)
			{
				index = pGrid->GetItem(i,"Index");
				lpNd[index].merchant = pGrid->GetItem(i,"���l");
				lpNd[index].tyotei = pGrid->GetItem(i,"����");
				lpNd[index].hostile = pGrid->GetItem(i,"�G�Α喼");
				lpNd[index].friendship = pGrid->GetItem(i,"�F�D�喼");
				lpNd[index].bPropagate = pGrid->GetItem(i,"�z��");
				lpNd[index].bHyotei = pGrid->GetItem(i,"�]��");
				lpNd[index].position = pGrid->GetItem(i,"��E");
			}
			break;
		case TGUNDAN://�R�c
			for(int i = 0;i != nCount;i++)
			{
				index = pGrid->GetItem(i,"Index");
				lpNg[index].act = pGrid->GetItem(i,"�s����");
				lpNg[index].money = pGrid->GetItem(i,"��");
				lpNg[index].rise = pGrid->GetItem(i,"��");
				lpNg[index].horse = pGrid->GetItem(i,"�R�n");
				lpNg[index].gun = pGrid->GetItem(i,"�S�C");
				lpNg[index].position = pGrid->GetItem(i,"��E");
			}
			break;
		case TSIRO://��
			for(int i = 0;i != nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");
				vt = pGrid->GetItem(i,"�閼");
				if(CGridCtrl::IsNull(vt))
                    lpNc[index].name[0] = 0;
				else lstrcpyn(lpNc[index].name,(_bstr_t)vt,9);
				lpNc[index].build = pGrid->GetItem(i,"��s");
				lpNc[index].grain = pGrid->GetItem(i,"�΍�");
				lpNc[index].town = pGrid->GetItem(i,"����");
				lpNc[index].population = pGrid->GetItem(i,"�l��");
				lpNc[index].loyal = pGrid->GetItem(i,"����");
				lpNc[index].soldier = pGrid->GetItem(i,"������");
				lpNc[index].quality = pGrid->GetItem(i,"����");
				lpNc[index].bSmith = pGrid->GetItem(i,"�b��");
				lpNc[index].bHorse = pGrid->GetItem(i,"�n�Y");
				lpNc[index].bPort = pGrid->GetItem(i,"�`");
				lpNc[index].blPort = pGrid->GetItem(i,"����");
				lpNc[index].bSilver = pGrid->GetItem(i,"��R");
				lpNc[index].bGold = pGrid->GetItem(i,"���R");
				lpNc[index].bRevolt = pGrid->GetItem(i,"�Ꝅ");
				lpNc[index].bLarge = pGrid->GetItem(i,"����");

				lpNcm[index].maxgrain = pGrid->GetItem(i,"�Ő�/10");
				lpNcm[index].maxtown = pGrid->GetItem(i,"�ŏ�");
				lpNcm[index].x = pGrid->GetItem(i,"�w�ʒu");
				lpNcm[index].y = pGrid->GetItem(i,"�x�ʒu");
			}
			break;
		case TBUSHO://����
			for(int i = 0;i != nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");

				vt = pGrid->GetItem(i,"��");
				if(CGridCtrl::IsNull(vt))
                    lpNgen[index].familyname[0] = 0;
				else
				{
					str = (char*)((_bstr_t)vt);
					NB6NameNormalize(str,false);
					lstrcpyn(lpNgen[index].familyname,str.GetBuffer(),7);
				}
				vt = pGrid->GetItem(i,"��");
				if(CGridCtrl::IsNull(vt))
                    lpNgen[index].fastname[0] = 0;
				else lstrcpyn(lpNgen[index].fastname,(_bstr_t)vt,7);

				lpNgen[index].number = pGrid->GetItem(i,"��ԍ�");
				lpNgen[index].position = pGrid->GetItem(i,"�g��");
				lpNge[index].birth = (int)(pGrid->GetItem(i,"���N")) - 1454;
				lpNge[index].work = pGrid->GetItem(i,"�d��");
				lpNge[index].ambition = pGrid->GetItem(i,"��]");
				lpNge[index].charm = pGrid->GetItem(i,"����");
				lpNge[index].expgov = pGrid->GetItem(i,"���n");
				lpNge[index].maxgov = pGrid->GetItem(i,"����");
				lpNge[index].aptitudegov = pGrid->GetItem(i,"���K");
				lpNge[index].expbat = pGrid->GetItem(i,"��n");
				lpNge[index].maxbat = pGrid->GetItem(i,"���");
				lpNge[index].aptitudebat = pGrid->GetItem(i,"��K");
				lpNge[index].expint = pGrid->GetItem(i,"�q�n");
				lpNge[index].maxint = pGrid->GetItem(i,"�q��");
				lpNge[index].aptitudeint = pGrid->GetItem(i,"�q�K");
				lpNge[index].loyal = pGrid->GetItem(i,"����");
				lpNge[index].merits = pGrid->GetItem(i,"�M��");
				lpNge[index].soldier = pGrid->GetItem(i,"����");
				lpNge[index].training = pGrid->GetItem(i,"�P��");
				lpNge[index].solloyal = pGrid->GetItem(i,"�m�C");
				lpNge[index].form = pGrid->GetItem(i,"����");
				lpNge[index].aptitudeasi = pGrid->GetItem(i,"���K");
				lpNge[index].aptitudekib = pGrid->GetItem(i,"�R�K");
				int iAptitudeGun = pGrid->GetItem(i,"�S�K");
				lpNge[index]._aptitudegun_hi = iAptitudeGun & 0x0003;  // 0b000000HH
				lpNge[index]._aptitudegun_lo = iAptitudeGun >> 2;	   // 0b00000L00
				lpNge[index].aptitudesui = pGrid->GetItem(i,"���K");
				lpNge[index].tech_ikkatsu	= pGrid->GetItem(i,"�ꊅ");
				lpNge[index].tech_ansatsu	= pGrid->GetItem(i,"�ÎE");
				lpNge[index].tech_ryusyutsu	= pGrid->GetItem(i,"���o");
				lpNge[index].tech_sendou	= pGrid->GetItem(i,"����");
				lpNge[index].tech_ryugen	= pGrid->GetItem(i,"����");
				lpNge[index].tech_benzetsu	= pGrid->GetItem(i,"�ِ�");
				lpNge[index].tech_yakiuchi	= pGrid->GetItem(i,"�ē�");
				lpNge[index].tech_chohatsu	= pGrid->GetItem(i,"����");
				lpNge[index].rise = pGrid->GetItem(i,"������");
				lpNge[index].blood = pGrid->GetItem(i,"����");
				lpNge[index].algo = pGrid->GetItem(i,"�v�l");
				lpNge[index].duty = pGrid->GetItem(i,"�`��");
				lpNge[index].affinity = pGrid->GetItem(i,"����");
				lpNge[index].job = pGrid->GetItem(i,"�E��");
				lpNge[index].biggun = pGrid->GetItem(i,"��C");
				lpNge[index].horsegun = pGrid->GetItem(i,"�R�S");
				lpNge[index].steelship = pGrid->GetItem(i,"�S�D");
				lpNge[index].life = pGrid->GetItem(i,"����");
				lpNge[index].brave = pGrid->GetItem(i,"�E��");
				int iIndependence = pGrid->GetItem(i,"�Ɨ�");
				lpNge[index]._independence_hi = iIndependence & 0x0003;  // 0b000000HH
				lpNge[index]._independence_lo = iIndependence >> 2;      // 0b00000L00
				lpNge[index].tone = pGrid->GetItem(i,"����");
				lpNge[index].bEnd = pGrid->GetItem(i,"�s��");
				lpNge[index].bRise = pGrid->GetItem(i,"����");
				lpNge[index].grudge = pGrid->GetItem(i,"�⍦");
			}
			break;
		case TKAHO://�ƕ�
			for(int i = 0;i < nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");
				vt = pGrid->GetItem(i,"���O");
				if(CGridCtrl::IsNull(vt))
                    lpNc[index].name[0] = 0;
				else lstrcpyn(lpNkh[index].name,(_bstr_t)vt,13);

				lpNkh[index].type = pGrid->GetItem(i,"����");
				lpNkh[index].face = pGrid->GetItem(i,"�摜");
				lpNkh[index].attach = pGrid->GetItem(i,"���L�����ԍ�");
				lpNkh[index].param = pGrid->GetItem(i,"�㏸�l");
				lpNkh[index].bMitojo = pGrid->GetItem(i,"���o��");
				lpNkh[index].bLost = pGrid->GetItem(i,"����");
			}
			break;
		case TKANI://����
			for(int i = 0;i < nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");
				vt = pGrid->GetItem(i,"���O");
				if(CGridCtrl::IsNull(vt))
                    lpNc[index].name[0] = 0;
				else lstrcpyn(lpNki[index].name,(_bstr_t)vt,13);

				lpNki[index].rank = pGrid->GetItem(i,"����");
				lpNki[index].attach = pGrid->GetItem(i,"���L�����ԍ�");
				lpNki[index].param = pGrid->GetItem(i,"�㏸�l");
			}
			break;
		}
	}

};

class CDatawrapp
{
	//dwMax ���E�T�C�Y
	BOOL LoadFile(LPCSTR fname,LPVOID &lp,DWORD &dwSize,FILETIME *pft,DWORD dwMax = 0)
	{
		HANDLE hf;
		BOOL bResult;
		DWORD dwResult,fSize;

		dwSize = 0;
		lp = NULL;

		hf = ::CreateFile(fname,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hf == INVALID_HANDLE_VALUE)
			return 0;

		GetFileTime(hf,NULL,NULL,pft);

		fSize = GetFileSize(hf,NULL);
		if(dwMax == 0)
			dwSize = fSize;
		else dwSize = min(fSize,dwMax);

		lp = GlobalAlloc(GMEM_FIXED,dwSize);
		if(lp == NULL)
		{
			::CloseHandle(hf);
			return FALSE;
		}

		bResult = ::ReadFile(hf,lp,dwSize,&dwResult,NULL);
		::CloseHandle(hf);
		if(!bResult)
		{
			GlobalFree(lp);
			lp = NULL;
			return FALSE;
		}
		return TRUE;
	}

	CData *m_data;
public:
	CDatawrapp(void)
	{
		m_data = NULL;
	}

	virtual ~CDatawrapp(void)
	{
		Clear();
	}

	void Clear()
	{
		if(m_data)
			delete m_data;
		m_data = NULL;
	}

	BOOL Open(HWND hParent,LPSTR szFile)
	{
		LPVOID lp;
		DWORD dwSize;
		FILETIME ft;

		Clear();
		BOOL bRes = LoadFile(szFile,lp,dwSize,&ft,1024 * (1024*10) );//max 1M byte
		if(bRes)
		{
			LPSTR szName = PathFindFileName(szFile);

			_strupr(szName);

			//NB6
			if(/*dwSize == 844623 &&*/ strstr(szName,"SAVEDAT") && !memcmp("1995.08.16",lp,11) )
			{
				m_data = new CNB6Data;
				CNB6SlotDlg slot;
				if(slot.DoModal(hParent,(LPARAM)lp) != IDCANCEL)
				{
					m_data->SetParam(lp,dwSize,ft,szFile,slot.m_Current);
					return TRUE;
				}
			}
		}
		Clear();
		return FALSE;
	}

	BOOL Reload()
	{
		LPVOID lp;
		DWORD dwSize;
		FILETIME ft;
		CString fn = m_data->m_Filename;
		int slot = m_data->m_Slot;

		Clear();
		BOOL bRes = LoadFile(fn,lp,dwSize,&ft,1024 * (1024*10));//max 1M byte
		if(bRes)
		{
			LPSTR szName = PathFindFileName(fn);

			_strupr(szName);

			//NB6
			if(/*dwSize == 844623 &&*/ strstr(szName,"SAVEDAT") && !memcmp("1995.08.16",lp,11))
			{
				m_data = new CNB6Data;
				m_data->SetParam(lp,dwSize,ft,fn,slot);
				return TRUE;
			}
		}
		Clear();
		return FALSE;
	}

	BOOL Save(LPSTR szFile = NULL) const
	{
		return (m_data) ? m_data->Save(szFile):FALSE;
	}

	int GetTabCount() const
	{
		return (m_data) ? m_data->GetTabCount():0;
	}

	void GetTabText(int nTab,LPSTR pszText,int nMax) const
	{
		if(m_data)
			m_data->GetTabText(nTab,pszText,nMax);
	}

	void SetGridData(CGridCtrl* pGrid,int nTab)
	{
		ATLASSERT(nTab < GetTabCount());
		ATLASSERT(pGrid);

		pGrid->SetRedraw(FALSE);
		if(m_data)
		{
			m_data->SetColumn(pGrid,nTab);
			m_data->SetGrid(pGrid,nTab);
		}
		pGrid->SetRedraw(TRUE);
		pGrid->SetFont(AtlGetDefaultGuiFont());
		return;
	}

	void UpdateGridData(CGridCtrl* pGrid,int nTab)
	{
		ATLASSERT(nTab < GetTabCount());
		ATLASSERT(pGrid);

		pGrid->SetRedraw(FALSE);
		if(m_data)
		{
			//SetColumn(pGrid,nTab);
			pGrid->DeleteAllItems();
			m_data->SetGrid(pGrid,nTab);
		}
		pGrid->SetRedraw(TRUE);
		return;
	}

	void GetGridData(CGridCtrl* pGrid,int nTab)
	{
		ATLASSERT(nTab < GetTabCount());
		ATLASSERT(pGrid);

		if(m_data)
			m_data->GetGrid(pGrid,nTab);

		return;
	}

	int Command(WORD wID, HWND hwnd)
	{
		return (m_data) ? m_data->Command(wID,hwnd):-1;
	}

	BOOL CheckUpdate(HWND hParent)
	{
		if(!m_data)
			return FALSE;

		FILETIME ft;
		HANDLE hf = ::CreateFile(m_data->m_Filename,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hf == INVALID_HANDLE_VALUE)
			return FALSE;

		BOOL result = GetFileTime(hf,NULL,NULL,&ft);
		::CloseHandle(hf);
		if(result)
		{
			if(CompareFileTime(&m_data->m_ft,&ft) != 0)
			{
				int res = MessageBox(hParent,"�����[�h���܂����H","�t�@�C���͍X�V����Ă��܂�",MB_YESNO|MB_ICONQUESTION);
				if(res == IDYES)
					return TRUE;
				else
					m_data->m_ft = ft;
			}
		}
		return FALSE;
	}
};




/******************************************************************************************
	AttachDlg
******************************************************************************************/
	LRESULT AttachDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
	{
		m_lpByte = (BYTE*)lParam;
		m_list = GetDlgItem(IDC_LIST1);
		m_combor = GetDlgItem(IDC_COMBO2);

		NB6GENERAL* lpNge = (NB6GENERAL*)(m_lpByte + 2786);
		NB6DAIMYO* lpNd = (NB6DAIMYO*)(m_lpByte + 0xc5ca);
		NB6GENERALNAME* lpNgen = (NB6GENERALNAME*)(m_lpByte + 27790);

		switch(m_Mode)
		{
		case NB6BUSHOU:
			for(int i = 0;i != 532;i++)
			{
				char *state = NULL;
				if(lpNgen[i].State == 2)
					state = "����";
				else if(lpNgen[i].State == 4)
 					state = "�Q�l";
				else if(lpNgen[i].State == 3)
					state = "�B��";
				else if (lpNgen[i].State == 6)
					state = "�ҋ@";
				/*				else if(lpNgen[i].State == 7)
					state = "���S";*/

			   if(state)
				{
					CString str;

					str.Format("%s|%s%s",state,lpNgen[i].familyname,lpNgen[i].fastname);
					CNB6Data::NB6NameNormalize(str,true);
					int index = m_list.AddString(str);
					m_list.SetItemData(index,i);
				}
			}
		}
		return 1;
	}

	void AttachDlg::ComboUpdate()
	{
		int count = m_list.GetCount();
		if(count == LB_ERR || count == 0)return;
		int i=0;
		for(i = 0;i != count;i++)
			if(m_list.GetSel(i) > 0)
				break;
        if(i == count)return;
		int nItem =	m_list.GetItemData(i);

		switch(m_Mode)
		{
		case NB6BUSHOU:
			{
				NB6GENERAL* lpNge = (NB6GENERAL*)(m_lpByte + 2786);
				NB6DAIMYO* lpNd = (NB6DAIMYO*)(m_lpByte + 0xc5ca);
				NB6GENERALNAME* lpNgen = (NB6GENERALNAME*)(m_lpByte + 27790);
				m_combor.ResetContent();

				NB6GENERAL* lpLeft = &lpNge[nItem];
				typedef std::vector<int> vec_d;
				vec_d vRight;

				for(int i = 0;i != 72;i++)
				{
					if(lpNd[i].attach != 0xFFFF)
						vRight.push_back(i);
				}
				std::sort(vRight.begin(),vRight.end(),NB6CompareAttach(m_lpByte,&lpNge[nItem]));

				for(vec_d::iterator it = vRight.begin();it != vRight.end();it++)
				{
					CString str;
					int x = abs(lpNge[lpNd[*it].attach - 1].affinity - lpNge[nItem].affinity);
					x = x > 8 ? 16 - x:x;
					str.Format("%d-%s%s",x,lpNgen[lpNd[*it].attach - 1].familyname,lpNgen[lpNd[*it].attach - 1].fastname);
					CNB6Data::NB6NameNormalize(str,true);
					int index = m_combor.AddString(str);
					m_combor.SetItemData(index,*it);
					if(lpNge[nItem].attach - 1 == *it)
						m_combor.SetCurSel(index);
				}
			}
			break;
		}
	}

#pragma warning(pop)