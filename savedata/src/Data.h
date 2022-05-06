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
	//NB6î‰äróp
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

		// édäØëÂñºÇÉäÉZÉbÉgÇ∑ÇÈÇ©Ç«Ç§Ç©éñëOÇ…ÉÜÅ[ÉUÅ[Ç…ñ‚Ç§
		int ret ;
		ret = ::MessageBox( NULL , "ïêè´Çà⁄ìÆÇ∑ÇÈÇ…Ç†ÇΩÇËÅA\nédäØîNêîÇÉäÉZÉbÉgÇµÇ‹Ç∑Ç©ÅH" , "édäØîNêîÉäÉZÉbÉgÅH" , MB_YESNO | MB_APPLMODAL ) ;
		if( ret == IDYES ) {
			isWorkResetOnAttachDaimyo = true; // ëÂñºïœçXÇ≈édäØîNêîÇÉäÉZÉbÉg
		} else {
			isWorkResetOnAttachDaimyo = false; // édäØîNêîÇÕÇªÇÃÇ‹Ç‹
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

		// édäØîNêîÉäÉZÉbÉgñ‚Ç§Ç©ÅH Ç≈YesÇæÇ¡ÇΩÇÁÅc
		if( isWorkResetOnAttachDaimyo ) {
			lpLeft->work = 0; // édäØîNêîÇÉäÉZÉbÉg
		}

		lpNgen[bushou].State = 2;
		//ì‡âûè¡
		lpLeft->rise = 0xFFFF;
		lpLeft->bRise = 0;

		//èÈéÂòQêlâBãèÉäÉXÉgçXêV
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

		//à⁄ìÆå≥èàóù
		if(lpLeft->prev != 0xFFFF && lpNge[lpLeft->prev - 1].next == bushou + 1)
			lpNge[lpLeft->prev - 1].next = lpLeft->next;
		if(lpLeft->next != 0xFFFF)
			lpNge[lpLeft->next - 1].prev = lpLeft->prev;

		//à⁄ìÆêÊèàóù
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
		const char* pszNB6TabString[] = {"ëÂñº","åRíc","èÈ","ïêè´","â∆ïÛ","äØà "};

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
				//500Å`2000ÇÃîÕàÕ
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
				//500Å`2000ÇÃîÕàÕ
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
					if(lpNgen[i].position == 1 || lpNgen[i].position == 2)//ëÂñº èhòV
						sol = max(100,sol);
					if(lpNgen[i].position == 3)//â∆òV
						sol = max(75,sol);
					if(lpNgen[i].position == 4)//ïîè´
						sol = max(55,sol);
					if(lpNgen[i].position == 5)//éòëÂè´
						sol = max(40,sol);
					if(lpNgen[i].position == 6)//ë´åyì™
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
		const char* chosokabe = "\xec\x8c\xec\x8d\xec\x8e";//í∑è@â‰ïî
		const char* kosokabe = "\xec\x8f\xec\x8d\xec\x8e";//çÅè@â‰ïî
		if(bDecode)
		{
			str.Replace(chosokabe,"í∑è@â‰ïî");
			str.Replace(kosokabe,"çÅè@â‰ïî");
		}else
		{
			str.Replace("í∑è@â‰ïî",chosokabe);
			str.Replace("çÅè@â‰ïî",kosokabe);
		}
	}
private:
	void SetColumn(CGridCtrl* pGrid,int nTab)
	{
		//ã§í 
		pGrid->AddColumn("Index",35,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
		switch(nTab)
		{
		case TDAIMYO:
			pGrid->AddColumn("ëÂñºî‘çÜ",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ëÂñº",75);
			pGrid->AddColumn("è§êl",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("í©íÏ",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ìGëŒëÂñº",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
			pGrid->AddColumn("óFçDëÂñº",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
			pGrid->AddColumn("ïzã≥",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ï]íË",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ñêE",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
 			pGrid->AddColumnLookup("ñêE",0,"");
 			pGrid->AddColumnLookup("ñêE",1,"ê™àŒëÂè´åR");
 			pGrid->AddColumnLookup("ñêE",2,"ä«óÃ");
 			pGrid->AddColumnLookup("ñêE",4,"ä÷ìåä«óÃ");
 			pGrid->AddColumnLookup("ñêE",8,"âúèBíTëË");
 			pGrid->AddColumnLookup("ñêE",0x10,"êºçëíTëË");
 			pGrid->AddColumnLookup("ñêE",0x20,"ã„èBíTëË");
 			pGrid->AddColumnLookup("ñêE",0x40,"âHèBíTëË");
            break;
		case TGUNDAN://åRíc
			pGrid->AddColumn("åRícî‘çÜ",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("èäëÆ",60);
			pGrid->AddColumn("åRícäÑìñî‘çÜ",80,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ãèèÈ",60);
			pGrid->AddColumn("åRící∑",75);
			pGrid->AddColumn("çsìÆóÕ",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ã‡",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ïƒ",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ãRîn",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ìSñC",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ñêE",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
 			pGrid->AddColumnLookup("ñêE",0,"");
 			pGrid->AddColumnLookup("ñêE",1,"ê™àŒëÂè´åR");
 			pGrid->AddColumnLookup("ñêE",2,"ä«óÃ");
 			pGrid->AddColumnLookup("ñêE",3,"ä÷ìåä«óÃ");
 			pGrid->AddColumnLookup("ñêE",4,"âúèBíTëË");
 			pGrid->AddColumnLookup("ñêE",5,"êºçëíTëË");
 			pGrid->AddColumnLookup("ñêE",6,"ã„èBíTëË");
 			pGrid->AddColumnLookup("ñêE",7,"âHèBíTëË");
			break;
		case TSIRO://èÈ
			pGrid->AddColumn("èÈî‘çÜ",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("èÈñº",60,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("èäëÆ",90);
			pGrid->AddColumn("èÈäs",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("êŒçÇ",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ç≈êŒ/10",45,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("è§ã∆",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ç≈è§",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("êlå˚",60,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ñØíâ",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("í•ï∫â¬",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ï∫éø",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT);
 			pGrid->AddColumnLookup("ï∫éø",2,"A");
 			pGrid->AddColumnLookup("ï∫éø",1,"B");
			pGrid->AddColumnLookup("ï∫éø",0,"C");
			pGrid->AddColumn("íbñË",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("înéY",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ç`",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("çëç€",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ã‚éR",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ã‡éR",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("àÍùÑ",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ãêèÈ",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("Çwà íu",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("Çxà íu",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
//			pGrid->AddColumn("èÈèÃ",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
           break;
		case TBUSHO://ïêè´
			pGrid->AddColumn("ïêè´î‘çÜ",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ê©",50,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("ñº",50,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("èäëÆ",50);
			pGrid->AddColumn("êgï™",50,CGridCtrl::EDIT_DROPDOWNLIST);
			pGrid->AddColumn("äÁî‘çÜ",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("êgï™",0,"âBãè");
			pGrid->AddColumnLookup("êgï™",1,"ëÂñº");
 			pGrid->AddColumnLookup("êgï™",2,"èhòV");
			pGrid->AddColumnLookup("êgï™",3,"â∆òV");
 			pGrid->AddColumnLookup("êgï™",4,"ïîè´");
			pGrid->AddColumnLookup("êgï™",5,"éòëÂè´");
			pGrid->AddColumnLookup("êgï™",6,"ë´åyì™");
			pGrid->AddColumn("èÛë‘",30);
			pGrid->AddColumn("ê∂îN",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("édäØ",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ñÏñ]",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ñ£óÕ",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ê≠èn",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ê≠çÀ",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ê≠ìK",30,CGridCtrl::EDIT_DROPDOWNLIST);
 			pGrid->AddColumnLookup("ê≠ìK",2,"Ç`");
			pGrid->AddColumnLookup("ê≠ìK",1,"Ça");
 			pGrid->AddColumnLookup("ê≠ìK",0,"Çb");
			pGrid->AddColumn("êÌèn",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("êÌçÀ",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("êÌìK",30,CGridCtrl::EDIT_DROPDOWNLIST);
 			pGrid->AddColumnLookup("êÌìK",2,"Ç`");
			pGrid->AddColumnLookup("êÌìK",1,"Ça");
 			pGrid->AddColumnLookup("êÌìK",0,"Çb");
			pGrid->AddColumn("íqèn",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("íqçÀ",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("íqìK",30,CGridCtrl::EDIT_DROPDOWNLIST);
 			pGrid->AddColumnLookup("íqìK",2,"Ç`");
			pGrid->AddColumnLookup("íqìK",1,"Ça");
 			pGrid->AddColumnLookup("íqìK",0,"Çb");
			pGrid->AddColumn("íâêΩ",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("åMå˜",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ï∫êî",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("åPó˚",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("émãC",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ï∫ë‘",40,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("ï∫ë‘",0,"ë´åy");
			pGrid->AddColumnLookup("ï∫ë‘",1,"ãRîn");
 			pGrid->AddColumnLookup("ï∫ë‘",2,"ìSñC");
			pGrid->AddColumnLookup("ï∫ë‘",3,"ãRìS");
			pGrid->AddColumnLookup("ï∫ë‘",0xFF,"ñ≥Çµ");
			pGrid->AddColumn("ë´ìK",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumnLookup("ë´ìK",5,"Çr");
			pGrid->AddColumnLookup("ë´ìK",4,"Ç`");
			pGrid->AddColumnLookup("ë´ìK",3,"Ça");
 			pGrid->AddColumnLookup("ë´ìK",2,"Çb");
			pGrid->AddColumnLookup("ë´ìK",1,"Çc");
 			pGrid->AddColumnLookup("ë´ìK",0,"Çd");
			pGrid->AddColumn("ãRìK",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumnLookup("ãRìK",5,"Çr");
			pGrid->AddColumnLookup("ãRìK",4,"Ç`");
			pGrid->AddColumnLookup("ãRìK",3,"Ça");
 			pGrid->AddColumnLookup("ãRìK",2,"Çb");
			pGrid->AddColumnLookup("ãRìK",1,"Çc");
 			pGrid->AddColumnLookup("ãRìK",0,"Çd");
			pGrid->AddColumn("ìSìK",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("ìSìK",5,"Çr");
			pGrid->AddColumnLookup("ìSìK",4,"Ç`");
 			pGrid->AddColumnLookup("ìSìK",3,"Ça");
			pGrid->AddColumnLookup("ìSìK",2,"Çb");
			pGrid->AddColumnLookup("ìSìK",1,"Çc");
			pGrid->AddColumnLookup("ìSìK",0,"Çd");
			pGrid->AddColumn("êÖìK",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("êÖìK",5,"Çr");
			pGrid->AddColumnLookup("êÖìK",4,"Ç`");
 			pGrid->AddColumnLookup("êÖìK",3,"Ça");
			pGrid->AddColumnLookup("êÖìK",2,"Çb");
			pGrid->AddColumnLookup("êÖìK",1,"Çc");
			pGrid->AddColumnLookup("êÖìK",0,"Çd");
			pGrid->AddColumn("àÍäÖ",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("à√éE",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ó¨èo",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ê¯ìÆ",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ó¨åæ",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ïŸê„",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("èƒì¢",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("íßî≠",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);

			pGrid->AddColumn("ì‡âûêÊ",80,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
			//pGrid->AddColumn("ç»",80);
			pGrid->AddColumn("ååãÿ",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("évçl",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ã`óù",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ëäê´",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("êEã∆",60,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("êEã∆",0,"Å|");
			pGrid->AddColumnLookup("êEã∆",0x10,"îEé“");
 			pGrid->AddColumnLookup("êEã∆",0x20,"íÉêl");
			pGrid->AddColumnLookup("êEã∆",0x30,"åïçã");
			pGrid->AddColumnLookup("êEã∆",0x40,"ëmóµ");
#ifdef TENSHOUKI_JP
			pGrid->AddColumnLookup("êEã∆",0x55,"-TSMod-");
			pGrid->AddColumnLookup("êEã∆",0x50,"äCëØ");
			pGrid->AddColumnLookup("êEã∆",0x60,"çÇâ∆");
			pGrid->AddColumnLookup("êEã∆",0x70,"è§êl");
			pGrid->AddColumnLookup("êEã∆",0x80,"èéñØ");
			pGrid->AddColumnLookup("êEã∆",0x90,"êÿéxíO");
			pGrid->AddColumnLookup("êEã∆",0xA0,"ãRém");
			pGrid->AddColumnLookup("êEã∆",0xB0,"ñÇèpém");
			pGrid->AddColumnLookup("êEã∆",0xC0,"ñÇë∞");
			pGrid->AddColumnLookup("êEã∆",0xD0,"Å|");
#endif
			pGrid->AddColumn("ëÂñC",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ãRìS",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ìSëD",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("éıñΩ",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("óEñ“",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ì∆óß",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("å˚í≤",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("çsìÆ",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ì‡âû",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("à‚ç¶",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
            break;
		case TKAHO://â∆ïÛ
			pGrid->AddColumn("â∆ïÛî‘çÜ",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ñºëO",80,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("ï™óﬁ",70,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("ï™óﬁ",0,"íÉòq");
			pGrid->AddColumnLookup("ï™óﬁ",1,"íÉö‚");
 			pGrid->AddColumnLookup("ï™óﬁ",2,"íÉì¸");
			pGrid->AddColumnLookup("ï™óﬁ",3,"íÉäò");
 			pGrid->AddColumnLookup("ï™óﬁ",4,"â‘ì¸");
			pGrid->AddColumnLookup("ï™óﬁ",5,"ìÅåï");
 			pGrid->AddColumnLookup("ï™óﬁ",6,"ëÑ");
			pGrid->AddColumnLookup("ï™óﬁ",7,"ãÔë´");
 			pGrid->AddColumnLookup("ï™óﬁ",8,"în");
			pGrid->AddColumnLookup("ï™óﬁ",9,"à∆");
 			pGrid->AddColumnLookup("ï™óﬁ",0xA,"êwâHêD");
			pGrid->AddColumnLookup("ï™óﬁ",0xB,"ä™éqñ{");
 			pGrid->AddColumnLookup("ï™óﬁ",0xC,"ñæí©í‘ñ{");
			pGrid->AddColumnLookup("ï™óﬁ",0xD,"çÅñÿ");
 			pGrid->AddColumnLookup("ï™óﬁ",0xE,"ínãÖãV");
			pGrid->AddColumnLookup("ï™óﬁ",0xF,"ìÏîÿéûåv");
 			pGrid->AddColumnLookup("ï™óﬁ",0x10,"êÖñnâÊ");
			pGrid->AddColumnLookup("ï™óﬁ",0x11,"è\éöâÀ");
 			pGrid->AddColumnLookup("ï™óﬁ",0x12,"êπèë");
			pGrid->AddColumnLookup("ï™óﬁ",0x13,"ã éËî†");
 			pGrid->AddColumnLookup("ï™óﬁ",0x14,"ÉrÅ[ÉhÉçît");
			pGrid->AddColumnLookup("ï™óﬁ",0x15,"âìä·ãæ");
 			pGrid->AddColumnLookup("ï™óﬁ",0x16,"ÉãÉ\Éìö‚");
			pGrid->AddColumnLookup("ï™óﬁ",0x17,"âåëê");
			pGrid->AddColumn("âÊëú",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("èäóLïêè´î‘çÜ",80,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("è„è∏íl",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ñ¢ìoèÍ",45,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("è¡é∏",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			break;
		case TKANI://äØà 
			pGrid->AddColumn("äØà î‘çÜ",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("ñºëO",80,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("ï™óﬁ",70,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("ï™óﬁ",0,"ê≥àÍà ");
			pGrid->AddColumnLookup("ï™óﬁ",1,"è]àÍà ");
 			pGrid->AddColumnLookup("ï™óﬁ",2,"ê≥ìÒà ");
			pGrid->AddColumnLookup("ï™óﬁ",3,"è]ìÒà ");
 			pGrid->AddColumnLookup("ï™óﬁ",4,"ê≥éOà ");
			pGrid->AddColumnLookup("ï™óﬁ",5,"è]éOà ");
 			pGrid->AddColumnLookup("ï™óﬁ",6,"ê≥élà è„");
			pGrid->AddColumnLookup("ï™óﬁ",7,"ê≥élà â∫");
 			pGrid->AddColumnLookup("ï™óﬁ",8,"è]élà è„");
			pGrid->AddColumnLookup("ï™óﬁ",9,"è]élà â∫");
 			pGrid->AddColumnLookup("ï™óﬁ",0xA,"ê≥å‹à è„");
			pGrid->AddColumnLookup("ï™óﬁ",0xB,"ê≥å‹à â∫");
 			pGrid->AddColumnLookup("ï™óﬁ",0xC,"è]å‹à è„");
			pGrid->AddColumnLookup("ï™óﬁ",0xD,"è]å‹à â∫");
 			pGrid->AddColumnLookup("ï™óﬁ",0xE,"ê≥òZà è„");
			pGrid->AddColumnLookup("ï™óﬁ",0xF,"ê≥òZà â∫");
 			pGrid->AddColumnLookup("ï™óﬁ",0x10,"è]òZà è„");
			pGrid->AddColumnLookup("ï™óﬁ",0x11,"è]òZà â∫");
 			pGrid->AddColumnLookup("ï™óﬁ",0x12,"ê≥éµà è„");
			pGrid->AddColumnLookup("ï™óﬁ",0x13,"ê≥éµà â∫");
 			pGrid->AddColumnLookup("ï™óﬁ",0x14,"è]éµà è„");
			pGrid->AddColumnLookup("ï™óﬁ",0x15,"è]éµà â∫");
 			pGrid->AddColumnLookup("ï™óﬁ",0x16,"ê≥î™à è„");
			pGrid->AddColumn("èäóLïêè´î‘çÜ",80,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("è„è∏íl",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
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
			pGrid->ClearColumnLookup("ìGëŒëÂñº");
			pGrid->ClearColumnLookup("óFçDëÂñº");
 			pGrid->AddColumnLookup("ìGëŒëÂñº",0,"Å| Å| Å| Å|");
			pGrid->AddColumnLookup("óFçDëÂñº",0,"Å| Å| Å| Å|");
 			pGrid->AddColumnLookup("ìGëŒëÂñº",0xFF,"Å| Å| Å| Å|");//à”ñ°Ç™è≠Çµà·Ç§Ç©Ç‡ÇµÇÍÇ»Ç¢
			pGrid->AddColumnLookup("óFçDëÂñº",0xFF,"Å| Å| Å| Å|");
			for(int i = 0;i != 72;i++)
			{
				if(lpNd[i].attach == 0xFFFF)continue;
				str.Format("%s%s",lpNgen[lpNd[i].attach - 1].familyname,lpNgen[lpNd[i].attach - 1].fastname);
				NB6NameNormalize(str,true);
 				pGrid->AddColumnLookup("ìGëŒëÂñº",i + 1,(LPCSTR)str);
 				pGrid->AddColumnLookup("óFçDëÂñº",i + 1,(LPCSTR)str);

				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"ëÂñºî‘çÜ",i+1);
				pGrid->SetItem(nItem,"ëÂñº",(LPCSTR)str);
				pGrid->SetItem(nItem,"è§êl",lpNd[i].merchant);
				pGrid->SetItem(nItem,"í©íÏ",lpNd[i].tyotei);
				pGrid->SetItem(nItem,"ìGëŒëÂñº",lpNd[i].hostile);
				pGrid->SetItem(nItem,"óFçDëÂñº",lpNd[i].friendship);
				pGrid->SetItem(nItem,"ïzã≥",lpNd[i].bPropagate);
				pGrid->SetItem(nItem,"ï]íË",lpNd[i].bHyotei);
				pGrid->SetItem(nItem,"ñêE",lpNd[i].position);
			}
			break;
		case TGUNDAN://åRíc
			for(int i = 0;i != 214;i++)
			{
				if(lpNg[i].residence == 0xffff)continue;

				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"åRícî‘çÜ",i+1);
				str.Format("%sâ∆",lpNgen[lpNd[lpNg[i].attach - 1].attach - 1].familyname);
				NB6NameNormalize(str,true);
				pGrid->SetItem(nItem,"èäëÆ",(LPCSTR)str);
				pGrid->SetItem(nItem,"åRícäÑìñî‘çÜ",lpNg[i].number);
				pGrid->SetItem(nItem,"ãèèÈ",lpNc[lpNg[i].residence - 1].name);
				str.Format("%s%s",lpNgen[lpNg[i].leader - 1].familyname,lpNgen[lpNg[i].leader - 1].fastname);
				NB6NameNormalize(str,true);
				pGrid->SetItem(nItem,"åRící∑",(LPCSTR)str);
				pGrid->SetItem(nItem,"çsìÆóÕ",lpNg[i].act);
				pGrid->SetItem(nItem,"ã‡",lpNg[i].money);
				pGrid->SetItem(nItem,"ïƒ",lpNg[i].rise);
				pGrid->SetItem(nItem,"ãRîn",lpNg[i].horse);
				pGrid->SetItem(nItem,"ìSñC",lpNg[i].gun);

				// ÅöÉVÉiÉäÉIÉGÉfÉBÉ^Å[ÇÃÉoÉOëŒâû
				if ( lpNg[i].position == 15 ) {
					lpNg[i].position = 0;
				}
				pGrid->SetItem(nItem,"ñêE",lpNg[i].position);
			}
			break;
		case TSIRO://èÈ
			for(int i = 0;i != 214;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"èÈî‘çÜ",i+1);
				pGrid->SetItem(nItem,"èÈñº",lpNc[i].name);
				if(lpNc[i].attach != 0xFFFF)
				{
					NB6GENERALNAME *lpName = &lpNgen[lpNg[lpNc[i].attach - 1].leader - 1];
					str.Format("%s%såRíc",lpName->familyname,lpName->fastname);
					NB6NameNormalize(str,true);
				}else str = "ñ≥èäëÆ";
				pGrid->SetItem(nItem,"èäëÆ",(LPCSTR)str);
				pGrid->SetItem(nItem,"èÈäs",lpNc[i].build);
				pGrid->SetItem(nItem,"êŒçÇ",lpNc[i].grain);
				pGrid->SetItem(nItem,"è§ã∆",lpNc[i].town);
				pGrid->SetItem(nItem,"êlå˚",lpNc[i].population);
				pGrid->SetItem(nItem,"ñØíâ",lpNc[i].loyal);
				pGrid->SetItem(nItem,"í•ï∫â¬",lpNc[i].soldier);
				pGrid->SetItem(nItem,"ï∫éø",lpNc[i].quality);

				pGrid->SetItem(nItem,"íbñË",lpNc[i].bSmith);
				pGrid->SetItem(nItem,"înéY",lpNc[i].bHorse);
				pGrid->SetItem(nItem,"ç`",lpNc[i].bPort);
				pGrid->SetItem(nItem,"çëç€",lpNc[i].blPort);
				pGrid->SetItem(nItem,"ã‚éR",lpNc[i].bSilver);
				pGrid->SetItem(nItem,"ã‡éR",lpNc[i].bGold);
				pGrid->SetItem(nItem,"àÍùÑ",lpNc[i].bRevolt);
				pGrid->SetItem(nItem,"ãêèÈ",lpNc[i].bLarge);
//				pGrid->SetItem(nItem,"èÈèÃ",lpNc[i].title);

				pGrid->SetItem(nItem,"ç≈êŒ/10",lpNcm[i].maxgrain);
				pGrid->SetItem(nItem,"ç≈è§",lpNcm[i].maxtown);
				pGrid->SetItem(nItem,"Çwà íu",lpNcm[i].x);
				pGrid->SetItem(nItem,"Çxà íu",lpNcm[i].y);
			}
			break;
		case TBUSHO://ïêè´
			pGrid->ClearColumnLookup("ì‡âûêÊ");
 			pGrid->AddColumnLookup("ì‡âûêÊ",0xFFFF,"ñ≥Çµ");
			for(int i = 0;i < 72;i++)
			{
				if(lpNd[i].attach == 0xFFFF)continue;
				str.Format("%s%s",lpNgen[lpNd[i].attach - 1].familyname,lpNgen[lpNd[i].attach - 1].fastname);
				NB6NameNormalize(str,true);
	 			pGrid->AddColumnLookup("ì‡âûêÊ",i + 1,(LPCSTR)str);
			}

			for(int i = 0;i != 532;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				str = lpNgen[i].familyname;
				NB6NameNormalize(str,true);
				pGrid->SetItem(nItem,"ïêè´î‘çÜ",i+1);
				pGrid->SetItem(nItem,"ê©",(LPCSTR)str);
				pGrid->SetItem(nItem,"ñº",lpNgen[i].fastname);
				pGrid->SetItem(nItem,"äÁî‘çÜ",lpNgen[i].number);
				if(lpNge[i].attach != 0xFFFF && lpNd[lpNge[i].attach - 1].attach  != 0xFFFF)
				{
					str.Format("%sâ∆",lpNgen[lpNd[lpNge[i].attach - 1].attach - 1].familyname);
					NB6NameNormalize(str,true);
					pGrid->SetItem(nItem,"èäëÆ",(LPCSTR)str);
				}

				pGrid->SetItem(nItem,"êgï™",lpNgen[i].position);
				char *tState[] = {"ëÂñº","åRí∑","åªñ","âBãè","òQêl","ïP","undef6","éÄñS"};
				pGrid->SetItem(nItem,"èÛë‘",tState[lpNgen[i].State & 7]);
				pGrid->SetItem(nItem,"ê∂îN",lpNge[i].birth + 1454);
				pGrid->SetItem(nItem,"édäØ",lpNge[i].work);
				pGrid->SetItem(nItem,"ñÏñ]",lpNge[i].ambition);
				pGrid->SetItem(nItem,"ñ£óÕ",lpNge[i].charm);
				pGrid->SetItem(nItem,"ê≠èn",lpNge[i].expgov);
				pGrid->SetItem(nItem,"ê≠çÀ",lpNge[i].maxgov);
				pGrid->SetItem(nItem,"ê≠ìK",lpNge[i].aptitudegov);
				pGrid->SetItem(nItem,"êÌèn",lpNge[i].expbat);
				pGrid->SetItem(nItem,"êÌçÀ",lpNge[i].maxbat);
				pGrid->SetItem(nItem,"êÌìK",lpNge[i].aptitudebat);
				pGrid->SetItem(nItem,"íqèn",lpNge[i].expint);
				pGrid->SetItem(nItem,"íqçÀ",lpNge[i].maxint);
				pGrid->SetItem(nItem,"íqìK",lpNge[i].aptitudeint);
				pGrid->SetItem(nItem,"íâêΩ",lpNge[i].loyal);
				pGrid->SetItem(nItem,"åMå˜",lpNge[i].merits);
				pGrid->SetItem(nItem,"ï∫êî",lpNge[i].soldier);
				pGrid->SetItem(nItem,"åPó˚",lpNge[i].training);
				pGrid->SetItem(nItem,"émãC",lpNge[i].solloyal);
				pGrid->SetItem(nItem,"ï∫ë‘",lpNge[i].form);
				//if(i < 500 && lpNge[i].spouse != 0xFF)
				//{
				//	NB6GENERALNAME *lpName;
				//	lpName = &lpNgen[lpNge[i].spouse + 500];
				//	wsprintf(szBuf,"%s%s",lpName->familyname,lpName->fastname);
				//	pGrid->SetItem(nItem,"ç»",szBuf);
				//}
				pGrid->SetItem(nItem,"ë´ìK",lpNge[i].aptitudeasi);
				pGrid->SetItem(nItem,"ãRìK",lpNge[i].aptitudekib);
				pGrid->SetItem(nItem,"ìSìK",(lpNge[i]._aptitudegun_lo << 2)+lpNge[i]._aptitudegun_hi); // ÉrÉbÉgÉtÉBÅ[ÉãÉhÇ™ÇQÇ¬Ç…äÑÇÍÇƒÇÈÇΩÇﬂóvââéZ
				pGrid->SetItem(nItem,"êÖìK",lpNge[i].aptitudesui);
 				pGrid->SetItem(nItem,"àÍäÖ",lpNge[i].tech_ikkatsu);
				pGrid->SetItem(nItem,"à√éE",lpNge[i].tech_ansatsu);
				pGrid->SetItem(nItem,"ó¨èo",lpNge[i].tech_ryusyutsu);
				pGrid->SetItem(nItem,"ê¯ìÆ",lpNge[i].tech_sendou);
				pGrid->SetItem(nItem,"ó¨åæ",lpNge[i].tech_ryugen);
				pGrid->SetItem(nItem,"ïŸê„",lpNge[i].tech_benzetsu);
				pGrid->SetItem(nItem,"èƒì¢",lpNge[i].tech_yakiuchi);
				pGrid->SetItem(nItem,"íßî≠",lpNge[i].tech_chohatsu);
				pGrid->SetItem(nItem,"ì‡âûêÊ",lpNge[i].rise);
				pGrid->SetItem(nItem,"ååãÿ",lpNge[i].blood);
				pGrid->SetItem(nItem,"évçl",lpNge[i].algo);
				pGrid->SetItem(nItem,"ã`óù",lpNge[i].duty);
				pGrid->SetItem(nItem,"ëäê´",lpNge[i].affinity);
				pGrid->SetItem(nItem,"êEã∆",lpNge[i].job);
				pGrid->SetItem(nItem,"ëÂñC",lpNge[i].biggun);
				pGrid->SetItem(nItem,"ãRìS",lpNge[i].horsegun);
				pGrid->SetItem(nItem,"ìSëD",lpNge[i].steelship);
				pGrid->SetItem(nItem,"éıñΩ",lpNge[i].life);
				pGrid->SetItem(nItem,"óEñ“",lpNge[i].brave);
				pGrid->SetItem(nItem,"ì∆óß",(lpNge[i]._independence_lo << 2)+lpNge[i]._independence_hi); // ÉrÉbÉgÉtÉBÅ[ÉãÉhÇ™ÇQÇ¬Ç…äÑÇÍÇƒÇÈÇΩÇﬂóvââéZ
				pGrid->SetItem(nItem,"å˚í≤",lpNge[i].tone);
				pGrid->SetItem(nItem,"çsìÆ",lpNge[i].bEnd);
				pGrid->SetItem(nItem,"ì‡âû",lpNge[i].bRise);
				pGrid->SetItem(nItem,"à‚ç¶",lpNge[i].grudge);
			}
			break;
		case TKAHO://â∆ïÛ
			for(int i = 0;i < 120;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"â∆ïÛî‘çÜ",i+1);

				pGrid->SetItem(nItem,"ñºëO",lpNkh[i].name);
				pGrid->SetItem(nItem,"ï™óﬁ",lpNkh[i].type);
				pGrid->SetItem(nItem,"âÊëú",lpNkh[i].face);
				pGrid->SetItem(nItem,"èäóLïêè´î‘çÜ",lpNkh[i].attach);
				pGrid->SetItem(nItem,"è„è∏íl",lpNkh[i].param);
				pGrid->SetItem(nItem,"ñ¢ìoèÍ",lpNkh[i].bMitojo);
				pGrid->SetItem(nItem,"è¡é∏",lpNkh[i].bLost);
			}
			break;
		case TKANI://äØà 
			for(int i = 0;i < 250;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"äØà î‘çÜ",i+1);

				pGrid->SetItem(nItem,"ñºëO",lpNki[i].name);
				pGrid->SetItem(nItem,"ï™óﬁ",lpNki[i].rank);
				pGrid->SetItem(nItem,"èäóLïêè´î‘çÜ",lpNki[i].attach);
				pGrid->SetItem(nItem,"è„è∏íl",lpNki[i].param);
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
				lpNd[index].merchant = pGrid->GetItem(i,"è§êl");
				lpNd[index].tyotei = pGrid->GetItem(i,"í©íÏ");
				lpNd[index].hostile = pGrid->GetItem(i,"ìGëŒëÂñº");
				lpNd[index].friendship = pGrid->GetItem(i,"óFçDëÂñº");
				lpNd[index].bPropagate = pGrid->GetItem(i,"ïzã≥");
				lpNd[index].bHyotei = pGrid->GetItem(i,"ï]íË");
				lpNd[index].position = pGrid->GetItem(i,"ñêE");
			}
			break;
		case TGUNDAN://åRíc
			for(int i = 0;i != nCount;i++)
			{
				index = pGrid->GetItem(i,"Index");
				lpNg[index].act = pGrid->GetItem(i,"çsìÆóÕ");
				lpNg[index].money = pGrid->GetItem(i,"ã‡");
				lpNg[index].rise = pGrid->GetItem(i,"ïƒ");
				lpNg[index].horse = pGrid->GetItem(i,"ãRîn");
				lpNg[index].gun = pGrid->GetItem(i,"ìSñC");
				lpNg[index].position = pGrid->GetItem(i,"ñêE");
			}
			break;
		case TSIRO://èÈ
			for(int i = 0;i != nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");
				vt = pGrid->GetItem(i,"èÈñº");
				if(CGridCtrl::IsNull(vt))
                    lpNc[index].name[0] = 0;
				else lstrcpyn(lpNc[index].name,(_bstr_t)vt,9);
				lpNc[index].build = pGrid->GetItem(i,"èÈäs");
				lpNc[index].grain = pGrid->GetItem(i,"êŒçÇ");
				lpNc[index].town = pGrid->GetItem(i,"è§ã∆");
				lpNc[index].population = pGrid->GetItem(i,"êlå˚");
				lpNc[index].loyal = pGrid->GetItem(i,"ñØíâ");
				lpNc[index].soldier = pGrid->GetItem(i,"í•ï∫â¬");
				lpNc[index].quality = pGrid->GetItem(i,"ï∫éø");
				lpNc[index].bSmith = pGrid->GetItem(i,"íbñË");
				lpNc[index].bHorse = pGrid->GetItem(i,"înéY");
				lpNc[index].bPort = pGrid->GetItem(i,"ç`");
				lpNc[index].blPort = pGrid->GetItem(i,"çëç€");
				lpNc[index].bSilver = pGrid->GetItem(i,"ã‚éR");
				lpNc[index].bGold = pGrid->GetItem(i,"ã‡éR");
				lpNc[index].bRevolt = pGrid->GetItem(i,"àÍùÑ");
				lpNc[index].bLarge = pGrid->GetItem(i,"ãêèÈ");

				lpNcm[index].maxgrain = pGrid->GetItem(i,"ç≈êŒ/10");
				lpNcm[index].maxtown = pGrid->GetItem(i,"ç≈è§");
				lpNcm[index].x = pGrid->GetItem(i,"Çwà íu");
				lpNcm[index].y = pGrid->GetItem(i,"Çxà íu");
			}
			break;
		case TBUSHO://ïêè´
			for(int i = 0;i != nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");

				vt = pGrid->GetItem(i,"ê©");
				if(CGridCtrl::IsNull(vt))
                    lpNgen[index].familyname[0] = 0;
				else
				{
					str = (char*)((_bstr_t)vt);
					NB6NameNormalize(str,false);
					lstrcpyn(lpNgen[index].familyname,str.GetBuffer(),7);
				}
				vt = pGrid->GetItem(i,"ñº");
				if(CGridCtrl::IsNull(vt))
                    lpNgen[index].fastname[0] = 0;
				else lstrcpyn(lpNgen[index].fastname,(_bstr_t)vt,7);

				lpNgen[index].number = pGrid->GetItem(i,"äÁî‘çÜ");
				lpNgen[index].position = pGrid->GetItem(i,"êgï™");
				lpNge[index].birth = (int)(pGrid->GetItem(i,"ê∂îN")) - 1454;
				lpNge[index].work = pGrid->GetItem(i,"édäØ");
				lpNge[index].ambition = pGrid->GetItem(i,"ñÏñ]");
				lpNge[index].charm = pGrid->GetItem(i,"ñ£óÕ");
				lpNge[index].expgov = pGrid->GetItem(i,"ê≠èn");
				lpNge[index].maxgov = pGrid->GetItem(i,"ê≠çÀ");
				lpNge[index].aptitudegov = pGrid->GetItem(i,"ê≠ìK");
				lpNge[index].expbat = pGrid->GetItem(i,"êÌèn");
				lpNge[index].maxbat = pGrid->GetItem(i,"êÌçÀ");
				lpNge[index].aptitudebat = pGrid->GetItem(i,"êÌìK");
				lpNge[index].expint = pGrid->GetItem(i,"íqèn");
				lpNge[index].maxint = pGrid->GetItem(i,"íqçÀ");
				lpNge[index].aptitudeint = pGrid->GetItem(i,"íqìK");
				lpNge[index].loyal = pGrid->GetItem(i,"íâêΩ");
				lpNge[index].merits = pGrid->GetItem(i,"åMå˜");
				lpNge[index].soldier = pGrid->GetItem(i,"ï∫êî");
				lpNge[index].training = pGrid->GetItem(i,"åPó˚");
				lpNge[index].solloyal = pGrid->GetItem(i,"émãC");
				lpNge[index].form = pGrid->GetItem(i,"ï∫ë‘");
				lpNge[index].aptitudeasi = pGrid->GetItem(i,"ë´ìK");
				lpNge[index].aptitudekib = pGrid->GetItem(i,"ãRìK");
				int iAptitudeGun = pGrid->GetItem(i,"ìSìK");
				lpNge[index]._aptitudegun_hi = iAptitudeGun & 0x0003;  // 0b000000HH
				lpNge[index]._aptitudegun_lo = iAptitudeGun >> 2;	   // 0b00000L00
				lpNge[index].aptitudesui = pGrid->GetItem(i,"êÖìK");
				lpNge[index].tech_ikkatsu	= pGrid->GetItem(i,"àÍäÖ");
				lpNge[index].tech_ansatsu	= pGrid->GetItem(i,"à√éE");
				lpNge[index].tech_ryusyutsu	= pGrid->GetItem(i,"ó¨èo");
				lpNge[index].tech_sendou	= pGrid->GetItem(i,"ê¯ìÆ");
				lpNge[index].tech_ryugen	= pGrid->GetItem(i,"ó¨åæ");
				lpNge[index].tech_benzetsu	= pGrid->GetItem(i,"ïŸê„");
				lpNge[index].tech_yakiuchi	= pGrid->GetItem(i,"èƒì¢");
				lpNge[index].tech_chohatsu	= pGrid->GetItem(i,"íßî≠");
				lpNge[index].rise = pGrid->GetItem(i,"ì‡âûêÊ");
				lpNge[index].blood = pGrid->GetItem(i,"ååãÿ");
				lpNge[index].algo = pGrid->GetItem(i,"évçl");
				lpNge[index].duty = pGrid->GetItem(i,"ã`óù");
				lpNge[index].affinity = pGrid->GetItem(i,"ëäê´");
				lpNge[index].job = pGrid->GetItem(i,"êEã∆");
				lpNge[index].biggun = pGrid->GetItem(i,"ëÂñC");
				lpNge[index].horsegun = pGrid->GetItem(i,"ãRìS");
				lpNge[index].steelship = pGrid->GetItem(i,"ìSëD");
				lpNge[index].life = pGrid->GetItem(i,"éıñΩ");
				lpNge[index].brave = pGrid->GetItem(i,"óEñ“");
				int iIndependence = pGrid->GetItem(i,"ì∆óß");
				lpNge[index]._independence_hi = iIndependence & 0x0003;  // 0b000000HH
				lpNge[index]._independence_lo = iIndependence >> 2;      // 0b00000L00
				lpNge[index].tone = pGrid->GetItem(i,"å˚í≤");
				lpNge[index].bEnd = pGrid->GetItem(i,"çsìÆ");
				lpNge[index].bRise = pGrid->GetItem(i,"ì‡âû");
				lpNge[index].grudge = pGrid->GetItem(i,"à‚ç¶");
			}
			break;
		case TKAHO://â∆ïÛ
			for(int i = 0;i < nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");
				vt = pGrid->GetItem(i,"ñºëO");
				if(CGridCtrl::IsNull(vt))
                    lpNc[index].name[0] = 0;
				else lstrcpyn(lpNkh[index].name,(_bstr_t)vt,13);

				lpNkh[index].type = pGrid->GetItem(i,"ï™óﬁ");
				lpNkh[index].face = pGrid->GetItem(i,"âÊëú");
				lpNkh[index].attach = pGrid->GetItem(i,"èäóLïêè´î‘çÜ");
				lpNkh[index].param = pGrid->GetItem(i,"è„è∏íl");
				lpNkh[index].bMitojo = pGrid->GetItem(i,"ñ¢ìoèÍ");
				lpNkh[index].bLost = pGrid->GetItem(i,"è¡é∏");
			}
			break;
		case TKANI://äØà 
			for(int i = 0;i < nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");
				vt = pGrid->GetItem(i,"ñºëO");
				if(CGridCtrl::IsNull(vt))
                    lpNc[index].name[0] = 0;
				else lstrcpyn(lpNki[index].name,(_bstr_t)vt,13);

				lpNki[index].rank = pGrid->GetItem(i,"ï™óﬁ");
				lpNki[index].attach = pGrid->GetItem(i,"èäóLïêè´î‘çÜ");
				lpNki[index].param = pGrid->GetItem(i,"è„è∏íl");
			}
			break;
		}
	}

};

class CDatawrapp
{
	//dwMax å¿äEÉTÉCÉY
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
				int res = MessageBox(hParent,"ÉäÉçÅ[ÉhÇµÇ‹Ç∑Ç©ÅH","ÉtÉ@ÉCÉãÇÕçXêVÇ≥ÇÍÇƒÇ¢Ç‹Ç∑",MB_YESNO|MB_ICONQUESTION);
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
					state = "åªñ";
				else if(lpNgen[i].State == 4)
 					state = "òQêl";
				else if(lpNgen[i].State == 3)
					state = "âBãè";
				else if (lpNgen[i].State == 6)
					state = "ë“ã@";
				/*				else if(lpNgen[i].State == 7)
					state = "éÄñS";*/

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