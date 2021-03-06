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
	//NB6比較用
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

		// 仕官大名をリセットするかどうか事前にユーザーに問う
		int ret ;
		ret = ::MessageBox( NULL , "武将を移動するにあたり、\n仕官年数をリセットしますか？" , "仕官年数リセット？" , MB_YESNO | MB_APPLMODAL ) ;
		if( ret == IDYES ) {
			isWorkResetOnAttachDaimyo = true; // 大名変更で仕官年数をリセット
		} else {
			isWorkResetOnAttachDaimyo = false; // 仕官年数はそのまま
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

		// 仕官年数リセット問うか？ でYesだったら…
		if( isWorkResetOnAttachDaimyo ) {
			lpLeft->work = 0; // 仕官年数をリセット
		}

		lpNgen[bushou].State = 2;
		//内応消
		lpLeft->rise = 0xFFFF;
		lpLeft->bRise = 0;

		//城主浪人隠居リスト更新
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

		//移動元処理
		if(lpLeft->prev != 0xFFFF && lpNge[lpLeft->prev - 1].next == bushou + 1)
			lpNge[lpLeft->prev - 1].next = lpLeft->next;
		if(lpLeft->next != 0xFFFF)
			lpNge[lpLeft->next - 1].prev = lpLeft->prev;

		//移動先処理
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
		const char* pszNB6TabString[] = {"大名","軍団","城","武将","家宝","官位"};

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
				//500〜2000の範囲
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
				//500〜2000の範囲
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
					if(lpNgen[i].position == 1 || lpNgen[i].position == 2)//大名 宿老
						sol = max(100,sol);
					if(lpNgen[i].position == 3)//家老
						sol = max(75,sol);
					if(lpNgen[i].position == 4)//部将
						sol = max(55,sol);
					if(lpNgen[i].position == 5)//侍大将
						sol = max(40,sol);
					if(lpNgen[i].position == 6)//足軽頭
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
		const char* chosokabe = "\xec\x8c\xec\x8d\xec\x8e";//長宗我部
		const char* kosokabe = "\xec\x8f\xec\x8d\xec\x8e";//香宗我部
		if(bDecode)
		{
			str.Replace(chosokabe,"長宗我部");
			str.Replace(kosokabe,"香宗我部");
		}else
		{
			str.Replace("長宗我部",chosokabe);
			str.Replace("香宗我部",kosokabe);
		}
	}
private:
	void SetColumn(CGridCtrl* pGrid,int nTab)
	{
		//共通
		pGrid->AddColumn("Index",35,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
		switch(nTab)
		{
		case TDAIMYO:
			pGrid->AddColumn("大名番号",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("大名",75);
			pGrid->AddColumn("商人",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("朝廷",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("敵対大名",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
			pGrid->AddColumn("友好大名",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
			pGrid->AddColumn("布教",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("評定",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("役職",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
 			pGrid->AddColumnLookup("役職",0,"");
 			pGrid->AddColumnLookup("役職",1,"征夷大将軍");
 			pGrid->AddColumnLookup("役職",2,"管領");
 			pGrid->AddColumnLookup("役職",4,"関東管領");
 			pGrid->AddColumnLookup("役職",8,"奥州探題");
 			pGrid->AddColumnLookup("役職",0x10,"西国探題");
 			pGrid->AddColumnLookup("役職",0x20,"九州探題");
 			pGrid->AddColumnLookup("役職",0x40,"羽州探題");
            break;
		case TGUNDAN://軍団
			pGrid->AddColumn("軍団番号",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("所属",60);
			pGrid->AddColumn("軍団割当番号",80,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("居城",60);
			pGrid->AddColumn("軍団長",75);
			pGrid->AddColumn("行動力",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("金",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("米",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("騎馬",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("鉄砲",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("役職",85,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
 			pGrid->AddColumnLookup("役職",0,"");
 			pGrid->AddColumnLookup("役職",1,"征夷大将軍");
 			pGrid->AddColumnLookup("役職",2,"管領");
 			pGrid->AddColumnLookup("役職",3,"関東管領");
 			pGrid->AddColumnLookup("役職",4,"奥州探題");
 			pGrid->AddColumnLookup("役職",5,"西国探題");
 			pGrid->AddColumnLookup("役職",6,"九州探題");
 			pGrid->AddColumnLookup("役職",7,"羽州探題");
			break;
		case TSIRO://城
			pGrid->AddColumn("城番号",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("城名",60,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("所属",90);
			pGrid->AddColumn("城郭",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("石高",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("最石/10",45,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("商業",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("最商",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("人口",60,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("民忠",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("徴兵可",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("兵質",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT);
 			pGrid->AddColumnLookup("兵質",2,"A");
 			pGrid->AddColumnLookup("兵質",1,"B");
			pGrid->AddColumnLookup("兵質",0,"C");
			pGrid->AddColumn("鍛冶",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("馬産",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("港",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("国際",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("銀山",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("金山",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("一揆",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("巨城",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("Ｘ位置",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("Ｙ位置",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
//			pGrid->AddColumn("城称",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
           break;
		case TBUSHO://武将
			pGrid->AddColumn("武将番号",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("姓",50,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("名",50,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("所属",50);
			pGrid->AddColumn("身分",50,CGridCtrl::EDIT_DROPDOWNLIST);
			pGrid->AddColumn("顔番号",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("身分",0,"隠居");
			pGrid->AddColumnLookup("身分",1,"大名");
 			pGrid->AddColumnLookup("身分",2,"宿老");
			pGrid->AddColumnLookup("身分",3,"家老");
 			pGrid->AddColumnLookup("身分",4,"部将");
			pGrid->AddColumnLookup("身分",5,"侍大将");
			pGrid->AddColumnLookup("身分",6,"足軽頭");
			pGrid->AddColumn("状態",30);
			pGrid->AddColumn("生年",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("仕官",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("野望",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("魅力",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("政熟",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("政才",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("政適",30,CGridCtrl::EDIT_DROPDOWNLIST);
 			pGrid->AddColumnLookup("政適",2,"Ａ");
			pGrid->AddColumnLookup("政適",1,"Ｂ");
 			pGrid->AddColumnLookup("政適",0,"Ｃ");
			pGrid->AddColumn("戦熟",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("戦才",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("戦適",30,CGridCtrl::EDIT_DROPDOWNLIST);
 			pGrid->AddColumnLookup("戦適",2,"Ａ");
			pGrid->AddColumnLookup("戦適",1,"Ｂ");
 			pGrid->AddColumnLookup("戦適",0,"Ｃ");
			pGrid->AddColumn("智熟",35,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("智才",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("智適",30,CGridCtrl::EDIT_DROPDOWNLIST);
 			pGrid->AddColumnLookup("智適",2,"Ａ");
			pGrid->AddColumnLookup("智適",1,"Ｂ");
 			pGrid->AddColumnLookup("智適",0,"Ｃ");
			pGrid->AddColumn("忠誠",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("勲功",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("兵数",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("訓練",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("士気",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("兵態",40,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("兵態",0,"足軽");
			pGrid->AddColumnLookup("兵態",1,"騎馬");
 			pGrid->AddColumnLookup("兵態",2,"鉄砲");
			pGrid->AddColumnLookup("兵態",3,"騎鉄");
			pGrid->AddColumnLookup("兵態",0xFF,"無し");
			pGrid->AddColumn("足適",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumnLookup("足適",5,"Ｓ");
			pGrid->AddColumnLookup("足適",4,"Ａ");
			pGrid->AddColumnLookup("足適",3,"Ｂ");
 			pGrid->AddColumnLookup("足適",2,"Ｃ");
			pGrid->AddColumnLookup("足適",1,"Ｄ");
 			pGrid->AddColumnLookup("足適",0,"Ｅ");
			pGrid->AddColumn("騎適",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumnLookup("騎適",5,"Ｓ");
			pGrid->AddColumnLookup("騎適",4,"Ａ");
			pGrid->AddColumnLookup("騎適",3,"Ｂ");
 			pGrid->AddColumnLookup("騎適",2,"Ｃ");
			pGrid->AddColumnLookup("騎適",1,"Ｄ");
 			pGrid->AddColumnLookup("騎適",0,"Ｅ");
			pGrid->AddColumn("鉄適",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("鉄適",5,"Ｓ");
			pGrid->AddColumnLookup("鉄適",4,"Ａ");
 			pGrid->AddColumnLookup("鉄適",3,"Ｂ");
			pGrid->AddColumnLookup("鉄適",2,"Ｃ");
			pGrid->AddColumnLookup("鉄適",1,"Ｄ");
			pGrid->AddColumnLookup("鉄適",0,"Ｅ");
			pGrid->AddColumn("水適",30,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("水適",5,"Ｓ");
			pGrid->AddColumnLookup("水適",4,"Ａ");
 			pGrid->AddColumnLookup("水適",3,"Ｂ");
			pGrid->AddColumnLookup("水適",2,"Ｃ");
			pGrid->AddColumnLookup("水適",1,"Ｄ");
			pGrid->AddColumnLookup("水適",0,"Ｅ");
			pGrid->AddColumn("一喝",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("暗殺",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("流出",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("煽動",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("流言",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("弁舌",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("焼討",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("挑発",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);

			pGrid->AddColumn("内応先",80,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::LEFT,VT_INT);
			//pGrid->AddColumn("妻",80);
			pGrid->AddColumn("血筋",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("思考",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("義理",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("相性",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("職業",60,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("職業",0,"−");
			pGrid->AddColumnLookup("職業",0x10,"忍者");
 			pGrid->AddColumnLookup("職業",0x20,"茶人");
			pGrid->AddColumnLookup("職業",0x30,"剣豪");
			pGrid->AddColumnLookup("職業",0x40,"僧侶");
#ifdef TENSHOUKI_JP
			pGrid->AddColumnLookup("職業",0x55,"-TSMod-");
			pGrid->AddColumnLookup("職業",0x50,"海賊");
			pGrid->AddColumnLookup("職業",0x60,"高家");
			pGrid->AddColumnLookup("職業",0x70,"商人");
			pGrid->AddColumnLookup("職業",0x80,"庶民");
			pGrid->AddColumnLookup("職業",0x90,"切支丹");
			pGrid->AddColumnLookup("職業",0xA0,"騎士");
			pGrid->AddColumnLookup("職業",0xB0,"魔術士");
			pGrid->AddColumnLookup("職業",0xC0,"魔族");
			pGrid->AddColumnLookup("職業",0xD0,"−");
#endif
			pGrid->AddColumn("大砲",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("騎鉄",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("鉄船",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("寿命",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("勇猛",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("独立",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("口調",30,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("行動",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("内応",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("遺恨",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
            break;
		case TKAHO://家宝
			pGrid->AddColumn("家宝番号",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("名前",80,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("分類",70,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("分類",0,"茶碗");
			pGrid->AddColumnLookup("分類",1,"茶壺");
 			pGrid->AddColumnLookup("分類",2,"茶入");
			pGrid->AddColumnLookup("分類",3,"茶釜");
 			pGrid->AddColumnLookup("分類",4,"花入");
			pGrid->AddColumnLookup("分類",5,"刀剣");
 			pGrid->AddColumnLookup("分類",6,"槍");
			pGrid->AddColumnLookup("分類",7,"具足");
 			pGrid->AddColumnLookup("分類",8,"馬");
			pGrid->AddColumnLookup("分類",9,"鞍");
 			pGrid->AddColumnLookup("分類",0xA,"陣羽織");
			pGrid->AddColumnLookup("分類",0xB,"巻子本");
 			pGrid->AddColumnLookup("分類",0xC,"明朝綴本");
			pGrid->AddColumnLookup("分類",0xD,"香木");
 			pGrid->AddColumnLookup("分類",0xE,"地球儀");
			pGrid->AddColumnLookup("分類",0xF,"南蛮時計");
 			pGrid->AddColumnLookup("分類",0x10,"水墨画");
			pGrid->AddColumnLookup("分類",0x11,"十字架");
 			pGrid->AddColumnLookup("分類",0x12,"聖書");
			pGrid->AddColumnLookup("分類",0x13,"玉手箱");
 			pGrid->AddColumnLookup("分類",0x14,"ビードロ杯");
			pGrid->AddColumnLookup("分類",0x15,"遠眼鏡");
 			pGrid->AddColumnLookup("分類",0x16,"ルソン壺");
			pGrid->AddColumnLookup("分類",0x17,"煙草");
			pGrid->AddColumn("画像",40,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("所有武将番号",80,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("上昇値",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("未登場",45,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("消失",30,CGridCtrl::EDIT_CHECK,CGridCtrl::RIGHT,VT_INT);
			break;
		case TKANI://官位
			pGrid->AddColumn("官位番号",60,CGridCtrl::EDIT_NONE,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("名前",80,CGridCtrl::EDIT_TEXT);
			pGrid->AddColumn("分類",70,CGridCtrl::EDIT_DROPDOWNLIST,CGridCtrl::RIGHT,VT_INT);
 			pGrid->AddColumnLookup("分類",0,"正一位");
			pGrid->AddColumnLookup("分類",1,"従一位");
 			pGrid->AddColumnLookup("分類",2,"正二位");
			pGrid->AddColumnLookup("分類",3,"従二位");
 			pGrid->AddColumnLookup("分類",4,"正三位");
			pGrid->AddColumnLookup("分類",5,"従三位");
 			pGrid->AddColumnLookup("分類",6,"正四位上");
			pGrid->AddColumnLookup("分類",7,"正四位下");
 			pGrid->AddColumnLookup("分類",8,"従四位上");
			pGrid->AddColumnLookup("分類",9,"従四位下");
 			pGrid->AddColumnLookup("分類",0xA,"正五位上");
			pGrid->AddColumnLookup("分類",0xB,"正五位下");
 			pGrid->AddColumnLookup("分類",0xC,"従五位上");
			pGrid->AddColumnLookup("分類",0xD,"従五位下");
 			pGrid->AddColumnLookup("分類",0xE,"正六位上");
			pGrid->AddColumnLookup("分類",0xF,"正六位下");
 			pGrid->AddColumnLookup("分類",0x10,"従六位上");
			pGrid->AddColumnLookup("分類",0x11,"従六位下");
 			pGrid->AddColumnLookup("分類",0x12,"正七位上");
			pGrid->AddColumnLookup("分類",0x13,"正七位下");
 			pGrid->AddColumnLookup("分類",0x14,"従七位上");
			pGrid->AddColumnLookup("分類",0x15,"従七位下");
 			pGrid->AddColumnLookup("分類",0x16,"正八位上");
			pGrid->AddColumn("所有武将番号",80,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
			pGrid->AddColumn("上昇値",50,CGridCtrl::EDIT_INT,CGridCtrl::RIGHT,VT_INT);
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
			pGrid->ClearColumnLookup("敵対大名");
			pGrid->ClearColumnLookup("友好大名");
 			pGrid->AddColumnLookup("敵対大名",0,"− − − −");
			pGrid->AddColumnLookup("友好大名",0,"− − − −");
 			pGrid->AddColumnLookup("敵対大名",0xFF,"− − − −");//意味が少し違うかもしれない
			pGrid->AddColumnLookup("友好大名",0xFF,"− − − −");
			for(int i = 0;i != 72;i++)
			{
				if(lpNd[i].attach == 0xFFFF)continue;
				str.Format("%s%s",lpNgen[lpNd[i].attach - 1].familyname,lpNgen[lpNd[i].attach - 1].fastname);
				NB6NameNormalize(str,true);
 				pGrid->AddColumnLookup("敵対大名",i + 1,(LPCSTR)str);
 				pGrid->AddColumnLookup("友好大名",i + 1,(LPCSTR)str);

				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"大名番号",i+1);
				pGrid->SetItem(nItem,"大名",(LPCSTR)str);
				pGrid->SetItem(nItem,"商人",lpNd[i].merchant);
				pGrid->SetItem(nItem,"朝廷",lpNd[i].tyotei);
				pGrid->SetItem(nItem,"敵対大名",lpNd[i].hostile);
				pGrid->SetItem(nItem,"友好大名",lpNd[i].friendship);
				pGrid->SetItem(nItem,"布教",lpNd[i].bPropagate);
				pGrid->SetItem(nItem,"評定",lpNd[i].bHyotei);
				pGrid->SetItem(nItem,"役職",lpNd[i].position);
			}
			break;
		case TGUNDAN://軍団
			for(int i = 0;i != 214;i++)
			{
				if(lpNg[i].residence == 0xffff)continue;

				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"軍団番号",i+1);
				str.Format("%s家",lpNgen[lpNd[lpNg[i].attach - 1].attach - 1].familyname);
				NB6NameNormalize(str,true);
				pGrid->SetItem(nItem,"所属",(LPCSTR)str);
				pGrid->SetItem(nItem,"軍団割当番号",lpNg[i].number);
				pGrid->SetItem(nItem,"居城",lpNc[lpNg[i].residence - 1].name);
				str.Format("%s%s",lpNgen[lpNg[i].leader - 1].familyname,lpNgen[lpNg[i].leader - 1].fastname);
				NB6NameNormalize(str,true);
				pGrid->SetItem(nItem,"軍団長",(LPCSTR)str);
				pGrid->SetItem(nItem,"行動力",lpNg[i].act);
				pGrid->SetItem(nItem,"金",lpNg[i].money);
				pGrid->SetItem(nItem,"米",lpNg[i].rise);
				pGrid->SetItem(nItem,"騎馬",lpNg[i].horse);
				pGrid->SetItem(nItem,"鉄砲",lpNg[i].gun);

				// ★シナリオエディターのバグ対応
				if ( lpNg[i].position == 15 ) {
					lpNg[i].position = 0;
				}
				pGrid->SetItem(nItem,"役職",lpNg[i].position);
			}
			break;
		case TSIRO://城
			for(int i = 0;i != 214;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"城番号",i+1);
				pGrid->SetItem(nItem,"城名",lpNc[i].name);
				if(lpNc[i].attach != 0xFFFF)
				{
					NB6GENERALNAME *lpName = &lpNgen[lpNg[lpNc[i].attach - 1].leader - 1];
					str.Format("%s%s軍団",lpName->familyname,lpName->fastname);
					NB6NameNormalize(str,true);
				}else str = "無所属";
				pGrid->SetItem(nItem,"所属",(LPCSTR)str);
				pGrid->SetItem(nItem,"城郭",lpNc[i].build);
				pGrid->SetItem(nItem,"石高",lpNc[i].grain);
				pGrid->SetItem(nItem,"商業",lpNc[i].town);
				pGrid->SetItem(nItem,"人口",lpNc[i].population);
				pGrid->SetItem(nItem,"民忠",lpNc[i].loyal);
				pGrid->SetItem(nItem,"徴兵可",lpNc[i].soldier);
				pGrid->SetItem(nItem,"兵質",lpNc[i].quality);

				pGrid->SetItem(nItem,"鍛冶",lpNc[i].bSmith);
				pGrid->SetItem(nItem,"馬産",lpNc[i].bHorse);
				pGrid->SetItem(nItem,"港",lpNc[i].bPort);
				pGrid->SetItem(nItem,"国際",lpNc[i].blPort);
				pGrid->SetItem(nItem,"銀山",lpNc[i].bSilver);
				pGrid->SetItem(nItem,"金山",lpNc[i].bGold);
				pGrid->SetItem(nItem,"一揆",lpNc[i].bRevolt);
				pGrid->SetItem(nItem,"巨城",lpNc[i].bLarge);
//				pGrid->SetItem(nItem,"城称",lpNc[i].title);

				pGrid->SetItem(nItem,"最石/10",lpNcm[i].maxgrain);
				pGrid->SetItem(nItem,"最商",lpNcm[i].maxtown);
				pGrid->SetItem(nItem,"Ｘ位置",lpNcm[i].x);
				pGrid->SetItem(nItem,"Ｙ位置",lpNcm[i].y);
			}
			break;
		case TBUSHO://武将
			pGrid->ClearColumnLookup("内応先");
 			pGrid->AddColumnLookup("内応先",0xFFFF,"無し");
			for(int i = 0;i < 72;i++)
			{
				if(lpNd[i].attach == 0xFFFF)continue;
				str.Format("%s%s",lpNgen[lpNd[i].attach - 1].familyname,lpNgen[lpNd[i].attach - 1].fastname);
				NB6NameNormalize(str,true);
	 			pGrid->AddColumnLookup("内応先",i + 1,(LPCSTR)str);
			}

			for(int i = 0;i != 532;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				str = lpNgen[i].familyname;
				NB6NameNormalize(str,true);
				pGrid->SetItem(nItem,"武将番号",i+1);
				pGrid->SetItem(nItem,"姓",(LPCSTR)str);
				pGrid->SetItem(nItem,"名",lpNgen[i].fastname);
				pGrid->SetItem(nItem,"顔番号",lpNgen[i].number);
				if(lpNge[i].attach != 0xFFFF && lpNd[lpNge[i].attach - 1].attach  != 0xFFFF)
				{
					str.Format("%s家",lpNgen[lpNd[lpNge[i].attach - 1].attach - 1].familyname);
					NB6NameNormalize(str,true);
					pGrid->SetItem(nItem,"所属",(LPCSTR)str);
				}

				pGrid->SetItem(nItem,"身分",lpNgen[i].position);
				char *tState[] = {"大名","軍長","現役","隠居","浪人","姫","undef6","死亡"};
				pGrid->SetItem(nItem,"状態",tState[lpNgen[i].State & 7]);
				pGrid->SetItem(nItem,"生年",lpNge[i].birth + 1454);
				pGrid->SetItem(nItem,"仕官",lpNge[i].work);
				pGrid->SetItem(nItem,"野望",lpNge[i].ambition);
				pGrid->SetItem(nItem,"魅力",lpNge[i].charm);
				pGrid->SetItem(nItem,"政熟",lpNge[i].expgov);
				pGrid->SetItem(nItem,"政才",lpNge[i].maxgov);
				pGrid->SetItem(nItem,"政適",lpNge[i].aptitudegov);
				pGrid->SetItem(nItem,"戦熟",lpNge[i].expbat);
				pGrid->SetItem(nItem,"戦才",lpNge[i].maxbat);
				pGrid->SetItem(nItem,"戦適",lpNge[i].aptitudebat);
				pGrid->SetItem(nItem,"智熟",lpNge[i].expint);
				pGrid->SetItem(nItem,"智才",lpNge[i].maxint);
				pGrid->SetItem(nItem,"智適",lpNge[i].aptitudeint);
				pGrid->SetItem(nItem,"忠誠",lpNge[i].loyal);
				pGrid->SetItem(nItem,"勲功",lpNge[i].merits);
				pGrid->SetItem(nItem,"兵数",lpNge[i].soldier);
				pGrid->SetItem(nItem,"訓練",lpNge[i].training);
				pGrid->SetItem(nItem,"士気",lpNge[i].solloyal);
				pGrid->SetItem(nItem,"兵態",lpNge[i].form);
				//if(i < 500 && lpNge[i].spouse != 0xFF)
				//{
				//	NB6GENERALNAME *lpName;
				//	lpName = &lpNgen[lpNge[i].spouse + 500];
				//	wsprintf(szBuf,"%s%s",lpName->familyname,lpName->fastname);
				//	pGrid->SetItem(nItem,"妻",szBuf);
				//}
				pGrid->SetItem(nItem,"足適",lpNge[i].aptitudeasi);
				pGrid->SetItem(nItem,"騎適",lpNge[i].aptitudekib);
				pGrid->SetItem(nItem,"鉄適",(lpNge[i]._aptitudegun_lo << 2)+lpNge[i]._aptitudegun_hi); // ビットフィールドが２つに割れてるため要演算
				pGrid->SetItem(nItem,"水適",lpNge[i].aptitudesui);
 				pGrid->SetItem(nItem,"一喝",lpNge[i].tech_ikkatsu);
				pGrid->SetItem(nItem,"暗殺",lpNge[i].tech_ansatsu);
				pGrid->SetItem(nItem,"流出",lpNge[i].tech_ryusyutsu);
				pGrid->SetItem(nItem,"煽動",lpNge[i].tech_sendou);
				pGrid->SetItem(nItem,"流言",lpNge[i].tech_ryugen);
				pGrid->SetItem(nItem,"弁舌",lpNge[i].tech_benzetsu);
				pGrid->SetItem(nItem,"焼討",lpNge[i].tech_yakiuchi);
				pGrid->SetItem(nItem,"挑発",lpNge[i].tech_chohatsu);
				pGrid->SetItem(nItem,"内応先",lpNge[i].rise);
				pGrid->SetItem(nItem,"血筋",lpNge[i].blood);
				pGrid->SetItem(nItem,"思考",lpNge[i].algo);
				pGrid->SetItem(nItem,"義理",lpNge[i].duty);
				pGrid->SetItem(nItem,"相性",lpNge[i].affinity);
				pGrid->SetItem(nItem,"職業",lpNge[i].job);
				pGrid->SetItem(nItem,"大砲",lpNge[i].biggun);
				pGrid->SetItem(nItem,"騎鉄",lpNge[i].horsegun);
				pGrid->SetItem(nItem,"鉄船",lpNge[i].steelship);
				pGrid->SetItem(nItem,"寿命",lpNge[i].life);
				pGrid->SetItem(nItem,"勇猛",lpNge[i].brave);
				pGrid->SetItem(nItem,"独立",(lpNge[i]._independence_lo << 2)+lpNge[i]._independence_hi); // ビットフィールドが２つに割れてるため要演算
				pGrid->SetItem(nItem,"口調",lpNge[i].tone);
				pGrid->SetItem(nItem,"行動",lpNge[i].bEnd);
				pGrid->SetItem(nItem,"内応",lpNge[i].bRise);
				pGrid->SetItem(nItem,"遺恨",lpNge[i].grudge);
			}
			break;
		case TKAHO://家宝
			for(int i = 0;i < 120;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"家宝番号",i+1);

				pGrid->SetItem(nItem,"名前",lpNkh[i].name);
				pGrid->SetItem(nItem,"分類",lpNkh[i].type);
				pGrid->SetItem(nItem,"画像",lpNkh[i].face);
				pGrid->SetItem(nItem,"所有武将番号",lpNkh[i].attach);
				pGrid->SetItem(nItem,"上昇値",lpNkh[i].param);
				pGrid->SetItem(nItem,"未登場",lpNkh[i].bMitojo);
				pGrid->SetItem(nItem,"消失",lpNkh[i].bLost);
			}
			break;
		case TKANI://官位
			for(int i = 0;i < 250;i++)
			{
				int nItem = pGrid->AddRow();
				pGrid->SetItem(nItem,"Index",i);
				pGrid->SetItem(nItem,"官位番号",i+1);

				pGrid->SetItem(nItem,"名前",lpNki[i].name);
				pGrid->SetItem(nItem,"分類",lpNki[i].rank);
				pGrid->SetItem(nItem,"所有武将番号",lpNki[i].attach);
				pGrid->SetItem(nItem,"上昇値",lpNki[i].param);
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
				lpNd[index].merchant = pGrid->GetItem(i,"商人");
				lpNd[index].tyotei = pGrid->GetItem(i,"朝廷");
				lpNd[index].hostile = pGrid->GetItem(i,"敵対大名");
				lpNd[index].friendship = pGrid->GetItem(i,"友好大名");
				lpNd[index].bPropagate = pGrid->GetItem(i,"布教");
				lpNd[index].bHyotei = pGrid->GetItem(i,"評定");
				lpNd[index].position = pGrid->GetItem(i,"役職");
			}
			break;
		case TGUNDAN://軍団
			for(int i = 0;i != nCount;i++)
			{
				index = pGrid->GetItem(i,"Index");
				lpNg[index].act = pGrid->GetItem(i,"行動力");
				lpNg[index].money = pGrid->GetItem(i,"金");
				lpNg[index].rise = pGrid->GetItem(i,"米");
				lpNg[index].horse = pGrid->GetItem(i,"騎馬");
				lpNg[index].gun = pGrid->GetItem(i,"鉄砲");
				lpNg[index].position = pGrid->GetItem(i,"役職");
			}
			break;
		case TSIRO://城
			for(int i = 0;i != nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");
				vt = pGrid->GetItem(i,"城名");
				if(CGridCtrl::IsNull(vt))
                    lpNc[index].name[0] = 0;
				else lstrcpyn(lpNc[index].name,(_bstr_t)vt,9);
				lpNc[index].build = pGrid->GetItem(i,"城郭");
				lpNc[index].grain = pGrid->GetItem(i,"石高");
				lpNc[index].town = pGrid->GetItem(i,"商業");
				lpNc[index].population = pGrid->GetItem(i,"人口");
				lpNc[index].loyal = pGrid->GetItem(i,"民忠");
				lpNc[index].soldier = pGrid->GetItem(i,"徴兵可");
				lpNc[index].quality = pGrid->GetItem(i,"兵質");
				lpNc[index].bSmith = pGrid->GetItem(i,"鍛冶");
				lpNc[index].bHorse = pGrid->GetItem(i,"馬産");
				lpNc[index].bPort = pGrid->GetItem(i,"港");
				lpNc[index].blPort = pGrid->GetItem(i,"国際");
				lpNc[index].bSilver = pGrid->GetItem(i,"銀山");
				lpNc[index].bGold = pGrid->GetItem(i,"金山");
				lpNc[index].bRevolt = pGrid->GetItem(i,"一揆");
				lpNc[index].bLarge = pGrid->GetItem(i,"巨城");

				lpNcm[index].maxgrain = pGrid->GetItem(i,"最石/10");
				lpNcm[index].maxtown = pGrid->GetItem(i,"最商");
				lpNcm[index].x = pGrid->GetItem(i,"Ｘ位置");
				lpNcm[index].y = pGrid->GetItem(i,"Ｙ位置");
			}
			break;
		case TBUSHO://武将
			for(int i = 0;i != nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");

				vt = pGrid->GetItem(i,"姓");
				if(CGridCtrl::IsNull(vt))
                    lpNgen[index].familyname[0] = 0;
				else
				{
					str = (char*)((_bstr_t)vt);
					NB6NameNormalize(str,false);
					lstrcpyn(lpNgen[index].familyname,str.GetBuffer(),7);
				}
				vt = pGrid->GetItem(i,"名");
				if(CGridCtrl::IsNull(vt))
                    lpNgen[index].fastname[0] = 0;
				else lstrcpyn(lpNgen[index].fastname,(_bstr_t)vt,7);

				lpNgen[index].number = pGrid->GetItem(i,"顔番号");
				lpNgen[index].position = pGrid->GetItem(i,"身分");
				lpNge[index].birth = (int)(pGrid->GetItem(i,"生年")) - 1454;
				lpNge[index].work = pGrid->GetItem(i,"仕官");
				lpNge[index].ambition = pGrid->GetItem(i,"野望");
				lpNge[index].charm = pGrid->GetItem(i,"魅力");
				lpNge[index].expgov = pGrid->GetItem(i,"政熟");
				lpNge[index].maxgov = pGrid->GetItem(i,"政才");
				lpNge[index].aptitudegov = pGrid->GetItem(i,"政適");
				lpNge[index].expbat = pGrid->GetItem(i,"戦熟");
				lpNge[index].maxbat = pGrid->GetItem(i,"戦才");
				lpNge[index].aptitudebat = pGrid->GetItem(i,"戦適");
				lpNge[index].expint = pGrid->GetItem(i,"智熟");
				lpNge[index].maxint = pGrid->GetItem(i,"智才");
				lpNge[index].aptitudeint = pGrid->GetItem(i,"智適");
				lpNge[index].loyal = pGrid->GetItem(i,"忠誠");
				lpNge[index].merits = pGrid->GetItem(i,"勲功");
				lpNge[index].soldier = pGrid->GetItem(i,"兵数");
				lpNge[index].training = pGrid->GetItem(i,"訓練");
				lpNge[index].solloyal = pGrid->GetItem(i,"士気");
				lpNge[index].form = pGrid->GetItem(i,"兵態");
				lpNge[index].aptitudeasi = pGrid->GetItem(i,"足適");
				lpNge[index].aptitudekib = pGrid->GetItem(i,"騎適");
				int iAptitudeGun = pGrid->GetItem(i,"鉄適");
				lpNge[index]._aptitudegun_hi = iAptitudeGun & 0x0003;  // 0b000000HH
				lpNge[index]._aptitudegun_lo = iAptitudeGun >> 2;	   // 0b00000L00
				lpNge[index].aptitudesui = pGrid->GetItem(i,"水適");
				lpNge[index].tech_ikkatsu	= pGrid->GetItem(i,"一喝");
				lpNge[index].tech_ansatsu	= pGrid->GetItem(i,"暗殺");
				lpNge[index].tech_ryusyutsu	= pGrid->GetItem(i,"流出");
				lpNge[index].tech_sendou	= pGrid->GetItem(i,"煽動");
				lpNge[index].tech_ryugen	= pGrid->GetItem(i,"流言");
				lpNge[index].tech_benzetsu	= pGrid->GetItem(i,"弁舌");
				lpNge[index].tech_yakiuchi	= pGrid->GetItem(i,"焼討");
				lpNge[index].tech_chohatsu	= pGrid->GetItem(i,"挑発");
				lpNge[index].rise = pGrid->GetItem(i,"内応先");
				lpNge[index].blood = pGrid->GetItem(i,"血筋");
				lpNge[index].algo = pGrid->GetItem(i,"思考");
				lpNge[index].duty = pGrid->GetItem(i,"義理");
				lpNge[index].affinity = pGrid->GetItem(i,"相性");
				lpNge[index].job = pGrid->GetItem(i,"職業");
				lpNge[index].biggun = pGrid->GetItem(i,"大砲");
				lpNge[index].horsegun = pGrid->GetItem(i,"騎鉄");
				lpNge[index].steelship = pGrid->GetItem(i,"鉄船");
				lpNge[index].life = pGrid->GetItem(i,"寿命");
				lpNge[index].brave = pGrid->GetItem(i,"勇猛");
				int iIndependence = pGrid->GetItem(i,"独立");
				lpNge[index]._independence_hi = iIndependence & 0x0003;  // 0b000000HH
				lpNge[index]._independence_lo = iIndependence >> 2;      // 0b00000L00
				lpNge[index].tone = pGrid->GetItem(i,"口調");
				lpNge[index].bEnd = pGrid->GetItem(i,"行動");
				lpNge[index].bRise = pGrid->GetItem(i,"内応");
				lpNge[index].grudge = pGrid->GetItem(i,"遺恨");
			}
			break;
		case TKAHO://家宝
			for(int i = 0;i < nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");
				vt = pGrid->GetItem(i,"名前");
				if(CGridCtrl::IsNull(vt))
                    lpNc[index].name[0] = 0;
				else lstrcpyn(lpNkh[index].name,(_bstr_t)vt,13);

				lpNkh[index].type = pGrid->GetItem(i,"分類");
				lpNkh[index].face = pGrid->GetItem(i,"画像");
				lpNkh[index].attach = pGrid->GetItem(i,"所有武将番号");
				lpNkh[index].param = pGrid->GetItem(i,"上昇値");
				lpNkh[index].bMitojo = pGrid->GetItem(i,"未登場");
				lpNkh[index].bLost = pGrid->GetItem(i,"消失");
			}
			break;
		case TKANI://官位
			for(int i = 0;i < nCount;i++)
			{
				_variant_t vt;
				index = pGrid->GetItem(i,"Index");
				vt = pGrid->GetItem(i,"名前");
				if(CGridCtrl::IsNull(vt))
                    lpNc[index].name[0] = 0;
				else lstrcpyn(lpNki[index].name,(_bstr_t)vt,13);

				lpNki[index].rank = pGrid->GetItem(i,"分類");
				lpNki[index].attach = pGrid->GetItem(i,"所有武将番号");
				lpNki[index].param = pGrid->GetItem(i,"上昇値");
			}
			break;
		}
	}

};

class CDatawrapp
{
	//dwMax 限界サイズ
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
				int res = MessageBox(hParent,"リロードしますか？","ファイルは更新されています",MB_YESNO|MB_ICONQUESTION);
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
					state = "現役";
				else if(lpNgen[i].State == 4)
 					state = "浪人";
				else if(lpNgen[i].State == 3)
					state = "隠居";
				else if (lpNgen[i].State == 6)
					state = "待機";
				/*				else if(lpNgen[i].State == 7)
					state = "死亡";*/

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