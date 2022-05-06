// SaveDataEditorView.h : interface of the CSaveDataEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include "data.h"


class CSaveDataEditorView : public CWindowImpl<CSaveDataEditorView,CTabCtrl>
{
public:
	// コマンドライン引数のグローバル変数で
	static LPTSTR mlpstrCmdLine;

public:
	DECLARE_WND_SUPERCLASS(NULL, CTabCtrl::GetWndClassName())

	//CData m_data;
	CDatawrapp m_data;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

private:
    BEGIN_MSG_MAP_EX(CSaveDataEditorView)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_SIZE(OnSize)
        MSG_WM_DESTROY(OnDestroy)
		MSG_WM_DROPFILES(OnDrop)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
		COMMAND_ID_HANDLER(ID_SELECTALL, OnSelectAll)
		MSG_WM_COMMAND(OnCommand)
        REFLECTED_NOTIFY_CODE_HANDLER_EX(TCN_SELCHANGING, OnTabSelChanging)
        REFLECTED_NOTIFY_CODE_HANDLER_EX(TCN_SELCHANGE, OnTabSelChange)
        DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

    LRESULT OnCreate(LPCREATESTRUCT lpcs)
	{
        LRESULT lRet = DefWindowProc();

        SetFont(AtlGetDefaultGuiFont());
		DragAcceptFiles(TRUE);

		// コマンドライン引数にすでにファイルが指定されているならば、ファイルを開く
		if(mlpstrCmdLine && !PathIsDirectory(mlpstrCmdLine)) {
            Open(mlpstrCmdLine);
		}

        return lRet;
    }

    void OnSize(UINT uType, CSize size)
	{
        // カレントタブ内のエディットコントロールをリサイズ
        int nIndex = GetCurSel();
        if(nIndex != -1)
            ResizeGrid(nIndex);

        SetMsgHandled(false);
    }

    void OnDestroy()
	{
        RemoveAllTab();
        SetMsgHandled(false);
    }

    LRESULT OnTabSelChanging(LPNMHDR pnmh){
        ShowCurGrid(false);
        return 0;
    }

    LRESULT OnTabSelChange(LPNMHDR pnmh){
        ShowCurGrid(true);
        return 0;
    }

    void RemoveAllTab()
	{
		// すべてのタブを削除
        int nCount = GetItemCount();
        for(int i=nCount-1; i >= 0; i--)
            RemoveTab(i);
	}

	void RemoveTab(int nIndex)
	{
        CGridCtrl* grid = GetGridCtrl(nIndex);
        grid->DestroyWindow();
        delete grid;
        DeleteItem(nIndex);

        // カレントタブを先頭のタブに変更
        SetCurSel(0);
        ShowCurGrid(true);
    }

    CGridCtrl* GetGridCtrl(int nIndex)
	{
        TCITEM ti;
        ti.mask = TCIF_PARAM;
        GetItem(nIndex, &ti);
        return (CGridCtrl*)ti.lParam;
    }

    CGridCtrl* GetCurGridCtrl()
	{
		int nIndex = GetCurSel();
		if(nIndex == -1)
			return NULL;
		CGridCtrl* grid = GetGridCtrl(nIndex);
		return grid;
	}

    void ResizeGrid(int nIndex)
	{
		CRect rcTabClient;
		GetClientRect(rcTabClient);
		AdjustRect(FALSE, rcTabClient);
		CGridCtrl* grid = GetGridCtrl(nIndex);
		grid->MoveWindow(rcTabClient);
    }

    void ShowCurGrid(bool bShow)
	{
		int nIndex = GetCurSel();
		if(nIndex != -1)
		{
			CGridCtrl* grid = GetGridCtrl(nIndex);
			if(bShow){
				ResizeGrid(nIndex);
				grid->ShowWindow(SW_SHOW);
				grid->SetFocus();
			}else{
				grid->ShowWindow(SW_HIDE);
			}
		}
	}

	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFileDialog fd(TRUE,0,0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"天翔記 with PowerupKit SAVEDATA(savedat.n6p)\0savedat*.n6p\0\0");
		if(fd.DoModal(m_hWnd) == IDOK)
			Open(fd.m_szFileName);
		return 0;
	}

	void OnDrop(HDROP hDrop)
	{
		char fn[_MAX_PATH];
		DragQueryFile(hDrop, 0, fn, _MAX_PATH);
		DragFinish(hDrop);

		if(!PathIsDirectory(fn))
            Open(fn);
	}

	void Open(LPSTR szFile)
	{
		RemoveAllTab();
		if(m_data.Open(m_hWnd,szFile))
		{
			char szTab[64];
			int nTab = m_data.GetTabCount();
			for(int i = 0;i < nTab;i++)
			{
				m_data.GetTabText(i,szTab,sizeof(szTab));
				AddTab(i,szTab);
			}
		}
	}

	void Reload()
	{
		if(m_data.Reload())
		{
			int nTab = m_data.GetTabCount();
			for(int i = 0;i < nTab;i++)
				m_data.UpdateGridData(GetGridCtrl(i),i);
		}else
            RemoveAllTab();
	}

 	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(m_data.GetTabCount() == 0)
			return 0;
		Save();
		return 0;
	}

 	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(m_data.GetTabCount() == 0)
			return 0;
		CFileDialog fd(FALSE,0,0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"天翔記 with PowerupKit SAVEDATA(savedat.n6p)\0savedat*.n6p\0\0");
		if(fd.DoModal(m_hWnd) == IDOK)
			Save(fd.m_szFileName);
		return 0;
	}

	void Save(LPSTR szFile = NULL)
	{
		int nTab = m_data.GetTabCount();
		for(int i = 0;i < nTab;i++)
		{
			m_data.GetGridData(GetGridCtrl(i),i);
		}
		if(!m_data.Save(szFile))
			AtlMessageBox(m_hWnd,"失敗しました");
	}

	void AddTab(int nTab,LPCSTR str)
	{
        // カレントタブ内のエディットコントロールを非表示にする
        ShowCurGrid(false);

		CGridCtrl *grid = new CGridCtrl;
		grid->Create(m_hWnd,rcDefault,NULL,WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
		grid->SetExtendedGridStyle(0,GS_EX_LINENUMBERS);

		m_data.SetGridData(grid,nTab);

		// タブを挿入
        TCITEM ti;
        ti.mask = TCIF_TEXT | TCIF_PARAM;
        ti.pszText = (LPTSTR)(LPCTSTR)str;
		ti.lParam = (LPARAM)grid;
		int nIndex = InsertItem(nTab, &ti);

        // 挿入したタブをカレントにしてフォーカスを与える
        SetCurSel(nIndex);
        grid->SetFocus();

        ResizeGrid(nIndex);
    }

 	LRESULT OnSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CGridCtrl* pGrid = GetCurGridCtrl();
		if(pGrid)
			pGrid->SelectAll();

		return 0;
	}

	void OnCommand(UINT, WORD wID, HWND hwnd)
	{
		int nTab = m_data.GetTabCount();
		if(nTab == 0)
		{
			SetMsgHandled(false);
			MessageBeep(MB_OK);
			return;
		}

		if(wID == ID_TABCHANGE)//ctrl+tab
		{
            int nCur = GetCurSel() + 1;
	        ShowCurGrid(false);
			SetCurSel(nCur % nTab);
	        ShowCurGrid(true);
			return;
		}

		for(int i = 0;i < nTab;i++)
		{
			m_data.GetGridData(GetGridCtrl(i),i);
		}
		int nChange = m_data.Command(wID,m_hWnd);

		if(nChange != -1)
            m_data.UpdateGridData(GetGridCtrl(nChange),nChange);
	}


};
