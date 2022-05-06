/////////////////////////////////////////////////////////////////////////////
// WTL Grid	Control	version	1.0	(June 1st, 2003)
//
// Written by Bj�rnar Henden	(bh@bhenden.org)
// Copyright (c) 2003 Bj�rnar Henden.
// Thanks to Bjarke	Viksoe for atlgdix.h
 //The CGridCtrl::CGridDC class	uses some routines by Noel Frankinet.

 //This	code may be	used in	compiled form in any way you desire. This
 //file	may	be redistributed by	any	means PROVIDING	it is 
 //not sold	for	profit without the authors written consent,	and	
 //providing that this notice and the authors name is included.	

 //This	file is	provided "as is" with no expressed or implied warranty.
 //The author accepts no liability if it causes	any	damage to you or your
 //computer	whatsoever.
//
// HISTORY
// 
// Version 1.0 (June 1st, 2003)
//	- Initial release

//�ύX�_
//���Ԋ֌W�폜
//EDIT_INT�ǉ�
//Sort
//�����I��

#pragma	once

#ifndef	__BHWTLGRID_H__
#define	__BHWTLGRID_H__

#ifndef	__ATLGDIX_H__
#include "atlgdix.h"
#endif

#ifndef	_INC_COMUTIL
#include <comutil.h>
#endif

#ifndef	_INC_COMDEF
#include <comdef.h>
#endif

#ifndef	__ATLSTR_H__
#error You must	include	atlstr.h before	including atlctrls.h
#endif

#ifndef	__ATLCTRLS_H__
#include <atlctrls.h>
#endif

#ifndef	__ATLSCRL_H__
#include <atlscrl.h>
#endif

#ifndef	__ATLCOLL_H__
#include <atlcoll.h>
#endif

#ifndef	__ATLMISC_H__
#define	_ATL_TMP_NO_CSTRING
#include <atlmisc.h>
#endif

#define	COL_MINWIDTH		20		// Minimum column width
#define	COL_NUMWIDTH		32		// Width of	row	number column

#define	GS_EX_LINENUMBERS	0x01	// Show	lines numbers
#define	GS_EX_CONTEXTMENU	0x02	// Allow context menu
#define	GS_EX_READONLY		0x04	// Grid	is read	only
#define	GS_EX_VGRIDLINES	0x08	// Vertical	lines
#define	GS_EX_HGRIDLINES	0x10	// Horizontal lines

class CGridCtrl	: public CWindowImpl<CGridCtrl>	{
	friend class CGrid;
	friend class CRow;
	friend class CColumn;
	friend class CCell;
	friend class CEditText;
public:
	DECLARE_WND_CLASS_EX("BHWTLGRID",CS_DBLCLKS,COLOR_BTNFACE)

	BEGIN_MSG_MAP(CGridCtrl)
		MESSAGE_HANDLER(WM_SETREDRAW, OnSetRedraw)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MSG_WM_PAINT(OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,	OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_CONTEXTMENU,	OnContextMenu)
		COMMAND_ID_HANDLER(ID_GRID_ADDROW, OnAddRow)
		COMMAND_ID_HANDLER(ID_GRID_EDITROW,	OnEditRow)
		COMMAND_ID_HANDLER(ID_GRID_DELETEROW, OnDeleteRow)
	END_MSG_MAP()

	// Forward class definitions
public:
	class CListener;
protected:
	class CEditText;
	class CEditDropdownlist;
	class CColumn;
	class CRow;
	class CGrid;
	class CGridDC;
	class CCellNone;
	class CCellText;
	class CCellDropdownlist;
	class CCellDropdown;
	class CCellDate;


// Handler prototypes (uncomment arguments if needed):
//	LRESULT	MessageHandler(UINT	/*uMsg*/, WPARAM /*wParam*/, LPARAM	/*lParam*/,	BOOL& /*bHandled*/)
//	LRESULT	CommandHandler(WORD	/*wNotifyCode*/, WORD /*wID*/, HWND	/*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT	NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/,	BOOL& /*bHandled*/)

	LRESULT	OnSetRedraw(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		//DefWindowProc(uMsg,wParam,lParam);
		if(m_bRedraw = wParam!=FALSE) {
			UpdateScrollSize();
		}
		return 0;
	}

	LRESULT	OnCreate(UINT /*uMsg*/,	WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&	/*bHandled*/) {
		CFontHandle	hFont =	AtlGetDefaultGuiFont();
		if(hFont) {
			LOGFONT	lf;
			hFont.GetLogFont(lf);

			m_grid.m_font.CreateFontIndirect(&lf);
//			lf.lfWeight	= FW_BOLD;
			m_grid.m_font_bold.CreateFontIndirect(&lf);
		} else {
			m_grid.m_font.CreateFont(8,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"MS Sans	Serif");
			m_grid.m_font_bold.CreateFont(8,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"MS Sans Serif");
		}

		m_grid.Create(m_hWnd,rcDefault,NULL,WS_CHILD|WS_VISIBLE|WS_TABSTOP,0);
		return 0;
	}
	
	LRESULT	OnSetFocus(UINT	/*uMsg*/, WPARAM /*wParam*/, LPARAM	/*lParam*/,	BOOL& /*bHandled*/)	{
		m_grid.SetFocus();
		return 0;
	}

	LRESULT	OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CPoint pt((DWORD)lParam);

		if(m_nDragCol >= 0)
		{
			long x = pt.x;
			if(x - COL_MINWIDTH	< m_nDragMin)
				x =	m_nDragMin + COL_MINWIDTH;
			m_grid.DoDrag(x,true);
			return 0;
		}

		CPoint ptScroll;
		m_grid.GetScrollOffset(ptScroll);
		pt += ptScroll;

		if(pt.y	- ptScroll.y < GetHeaderHeight())
		{
			long x = 0;
			if(ShowLineNumbers())
				x += COL_NUMWIDTH;
			for(long i = 0;i < GetColumnCount();i++)
			{
				if(m_grid.m_columns[i]->OnEdge(x,pt.x))
				{
					SetCursor(LoadCursor(NULL,IDC_SIZEWE));
					return 0;
				}
				x += m_grid.m_columns[i]->m_nWidth;
			}
		}

		SetCursor(LoadCursor(NULL,IDC_ARROW));

		return 0;
	}

	LRESULT	OnLButtonDown(UINT /*uMsg*/, WPARAM	/*wParam*/,	LPARAM lParam, BOOL& /*bHandled*/)
	{
		CPoint pt((DWORD)lParam);
		if(pt.y	< GetHeaderHeight())
		{
			CPoint ptScroll;
			m_grid.GetScrollOffset(ptScroll);
			pt += ptScroll;

			long x = 0;
			if(ShowLineNumbers())
				x += COL_NUMWIDTH; 
			for(long i = 0;i < GetColumnCount();i++)
			{
				if(m_grid.m_columns[i]->OnEdge(x,pt.x))
				{
					SetCursor(LoadCursor(NULL,IDC_SIZEWE));
					m_grid.DoDrag(pt.x - ptScroll.x,false);
					SetCapture();
					m_nDragCol = i;
					m_nDragMin = x - ptScroll.x;
					return 0;
				}else if(m_grid.m_columns[i]->OnFace(x,pt.x))//sort
				{
					SetCapture();
					m_nPushCol = i;
					Invalidate();
					//m_grid.Invalidate();
					return 0;
				}
				x += m_grid.m_columns[i]->m_nWidth;
			}
		}
		return 0;
	}
	
	LRESULT	OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CPoint pt((DWORD)lParam);

		if(m_nDragCol >= 0)
		{
			long x = pt.x;
			if(x-COL_MINWIDTH<m_nDragMin)
				x =	m_nDragMin + COL_MINWIDTH;

			CPoint ptScroll;
			m_grid.GetScrollOffset(ptScroll);
			pt += ptScroll;

			SetCursor(LoadCursor(NULL,IDC_ARROW));
			m_grid.m_columns[m_nDragCol]->m_nWidth = x - m_nDragMin;
			ReleaseCapture();
			m_nDragCol = -1;
			m_grid.SizeControls();
			Invalidate();
			m_grid.Invalidate();
			
			UpdateScrollSize();
			m_grid.SetScrollOffset(ptScroll);
		}
		if(m_nPushCol != -1)
		{
			if(pt.y	< GetHeaderHeight())
			{
				CPoint ptScroll;
				m_grid.GetScrollOffset(ptScroll);
				pt += ptScroll;
				long x = 0;
				if(ShowLineNumbers())
					x += COL_NUMWIDTH; 
				for(long i = 0;i < m_nPushCol;i++)
					x += m_grid.m_columns[i]->m_nWidth;

				if(m_grid.m_columns[m_nPushCol]->OnFace(x,pt.x))
				{
					SortColumn(m_nPushCol);
				}
			}
			ReleaseCapture();
			m_nPushCol = -1;
			Invalidate();
			m_grid.Invalidate();
		}
		return 0;
	}
	
	LRESULT	OnPaint(HDC hdc)
	{
		if(hdc != NULL)
			DoPaint(hdc);
		else {
			CPaintDC dc(m_hWnd);
			DoPaint(dc.m_hDC);
		}

		return 0;
	}
	
	LRESULT	OnSize(UINT	/*uMsg*/, WPARAM /*wParam*/, LPARAM	lParam,	BOOL& /*bHandled*/)	{
		ATLASSERT(IsWindow());
			
		Invalidate();
		long top = GetHeaderHeight();
		m_grid.SetWindowPos(NULL,0,top,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)-top,0);
		UpdateScrollSize();

		return 0;
	}
	
	LRESULT	OnContextMenu(UINT /*uMsg*/, WPARAM	/*wParam*/,	LPARAM lParam, BOOL& bHandled) {
		if(!(m_grid.m_dwStyle &	GS_EX_CONTEXTMENU))	{
			bHandled = FALSE;
			return 0;
		}

		CPoint pt((DWORD)lParam);
		
		if(pt.x<0 && pt.y<0) {
			pt.x = 10;
			pt.y = 10;
			MapWindowPoints(NULL,&pt,1);
		}

		CMenu menu;
		menu.CreatePopupMenu();
		UINT uFlags	= 0;
		menu.AppendMenu(MF_STRING|uFlags,ID_GRID_ADDROW,"Add Row\tIns");
		uFlags = m_grid.GetSelectedCount() ? 0 :	MF_GRAYED;
		menu.AppendMenu(MF_STRING|uFlags,ID_GRID_DELETEROW,"Delete Row\tDel");
		menu.AppendMenu(MF_STRING|uFlags,ID_GRID_EDITROW,"Edit Row\tEnter");
		menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,m_hWnd);

		return 0;
	}
	
	LRESULT	OnAddRow(WORD /*wNotifyCode*/, WORD	/*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		long row = AddRow();
		if(m_grid.m_pListener) m_grid.m_pListener->OnNewRow(GetWindowLong(GWL_ID),row);
		m_grid.SelectRow(row);
		EnsureVisible(row);
		m_grid.EditRow();
		return 0;
	}

	LRESULT	OnEditRow(WORD /*wNotifyCode*/,	WORD /*wID*/, HWND /*hWndCtl*/,	BOOL& /*bHandled*/)	{
		ATLASSERT(m_grid.GetSelectedCount());
		m_grid.EditRow();
		return 0;
	}

	LRESULT	OnDeleteRow(WORD /*wNotifyCode*/, WORD /*wID*/,	HWND /*hWndCtl*/, BOOL&	/*bHandled*/) {
		if(m_grid.GetSelectedCount())
			DeleteItem(GetSelectedRow());
		return 0;
	}

	void DoPaint(CDCHandle dc2)	{
		if(!m_bRedraw) return;
		CMemDC dc(dc2);
		CGridDC	mdc(dc);
		HFONT hSaveFont	= dc.SelectFont(m_grid.m_font_bold);
		long x = 0;

		CPoint ptScroll;
		m_grid.GetScrollOffset(ptScroll);
		x -= ptScroll.x;

		CRect rcClient;
		GetClientRect(rcClient);
		dc.FillRect(&rcClient,(HBRUSH)(COLOR_BTNFACE+1));

		if(ShowLineNumbers()) {
			CRect rc(x,0,x+COL_NUMWIDTH,GetHeaderHeight());
			mdc.DrawButton(rc,false);
			x += COL_NUMWIDTH;
		}

		for(long i = 0;i < m_grid.m_columns.GetSize();i++)
		{
			CRect rc(x,0,x+m_grid.m_columns[i]->m_nWidth,GetHeaderHeight());
			mdc.DrawButton(rc,m_nPushCol == i);
			rc.DeflateRect(2,1);
			rc.bottom =	rc.top + m_grid.m_nControlHeight;
			mdc.DrawText(rc,m_grid.m_columns[i]->m_strTitle,m_grid.m_columns[i]->m_nAlign);
			x += m_grid.m_columns[i]->m_nWidth;
		}

		dc.SelectFont(hSaveFont);
	}

	void UpdateScrollSize()	{
		ATLASSERT(IsWindow() &&	m_grid.IsWindow());
		CPoint ptScroll;
		CRect rcClient;
		SIZE sz;

		m_grid.GetClientRect(rcClient);
		m_grid.GetScrollOffset(ptScroll);
		m_grid.SetScrollSize(m_grid.GetClientWidth(true),m_grid.GetClientHeight(true),FALSE);
		m_grid.SetScrollLine(100,GetRowHeight());
		m_grid.SetScrollPage(rcClient.Width(),rcClient.Height()	/ GetRowHeight() * GetRowHeight());

		m_grid.GetScrollSize(sz);
		if(ptScroll.y +	rcClient.Height() >= sz.cy && sz.cy>rcClient.Height())
			ptScroll.y = sz.cy - rcClient.Height();
		m_grid.SetScrollOffset(ptScroll);
	}

	long GetRowHeight()	const {
		return m_grid.GetRowHeight();
	}

	long GetHeaderHeight() const {
		return m_grid.GetRowHeight();;
	}

	long GetColumnIndex(LPCTSTR	pszName) const {
		long count = m_grid.m_columns.GetSize();
		while(count--) {
			if(!m_grid.m_columns[count]->m_strName.CompareNoCase(pszName))
				return count;
		}
		return -1;
	}
	
	bool ShowLineNumbers() const {
		return m_grid.m_dwStyle	& GS_EX_LINENUMBERS;
	}
	static bool	ChangeType(_variant_t& vt,VARTYPE type)	{
		try	{
			vt.ChangeType(type);
			return true;
		}
#ifdef _DEBUG
		catch(_com_error& e) {
			ATL::CString err;
			err.Format("%s\n%s",(LPCTSTR)e.Description(),(LPCTSTR)e.ErrorMessage());
			AtlMessageBox(GetActiveWindow(),(LPCTSTR)err);
			MessageBeep(MB_ICONEXCLAMATION);
			return false;
		}
#else
		catch(_com_error&) {
			MessageBeep(MB_ICONEXCLAMATION);
			return false;
		}
#endif
	}
	
public:
	enum CONTROLTYPE {
		EDIT_NONE,
		EDIT_TEXT,
		EDIT_INT,
		EDIT_DROPDOWNLIST,
		EDIT_DROPDOWN,
		EDIT_CHECK
	};

	enum ALIGN {
		LEFT,
		CENTER,
		RIGHT
	};

	CGridCtrl()	: m_grid(*this)	{
		m_nDragCol = m_nPushCol	= -1;
		m_grid.m_pListener = NULL;
		m_bRedraw =	true;
	}

	virtual	BOOL PreTranslateMessage(MSG* pMsg)	{
		return m_grid.PreTranslateMessage(pMsg);
	}
	
	void AddColumn(LPCTSTR pszTitle,long nWidth,CONTROLTYPE	nType=EDIT_NONE,ALIGN nAlign=LEFT,VARTYPE vt=VT_BSTR,LPCTSTR pszName=NULL) {
		ATLASSERT(m_grid.m_rows.size()==0 && "Can't	add	columns	after rows have	been added.");
		if(!pszName) pszName = pszTitle;
		
		CColumn* col = new CColumn(m_grid);
		col->m_strTitle	= pszTitle;
		col->m_nWidth =	nWidth;
		col->m_nType = nType;
		col->m_strName = pszName;
		col->m_vt =	vt;
		col->m_nAlign =	nAlign;
		m_grid.m_columns.Add(col);
		UpdateScrollSize();
		Invalidate();

		col->CreateEdit();
	}
	
	long AddRow() {
		ATLASSERT(m_grid.m_columns.GetSize()>0);
		CRow* row =	new	CRow(m_grid,m_grid.m_columns.GetSize());
		m_grid.m_rows.push_back(row);
		long nItem = m_grid.m_rows.size() -	1;
		if(m_bRedraw) UpdateScrollSize();
		return nItem;
	}
	
	long GetSelectedRow() const	{
		return m_grid.GetCurRow();
	}

	void SetItem(long nItem,long nColumn,_variant_t	vtValue) {
		ATLASSERT(nItem<GetRowCount());
		ATLASSERT(nColumn<GetColumnCount());
		m_grid.m_rows[nItem]->SetItem(nColumn,vtValue);
		if(m_bRedraw) m_grid.DrawCell(nItem,nColumn);
	}

	void SetItem(long nItem,int	nColumn,_variant_t vtValue)	{
		SetItem(nItem,(long)nColumn,vtValue);
	}

	void SetItem(long nItem,LPCTSTR	pszColumn,_variant_t vtValue) {
		long nColumn = this->GetColumnIndex(pszColumn);
		ATLASSERT(nColumn>=0);
		SetItem(nItem,nColumn,vtValue);
	}

	_variant_t GetItem(long	nItem,long nColumn)	{
		ATLASSERT(nItem<GetRowCount());
		ATLASSERT(nColumn<GetColumnCount());
		_variant_t vt =	m_grid.m_rows[nItem]->GetItem(nColumn);
		ATLASSERT(IsNull(vt) ||	vt.vt==m_grid.m_columns[nColumn]->m_vt);
		return vt;
	}

	_variant_t GetItem(long	nItem,int nColumn) {
		return GetItem(nItem,(long)nColumn);
	}

	_variant_t GetItem(long	nItem,LPCTSTR pszColumn) {
		long nColumn = this->GetColumnIndex(pszColumn);
		ATLASSERT(nColumn>=0);
		return GetItem(nItem,nColumn);
	}

	//�ǉ�
	void SortColumn(LPCTSTR	pszColumn)
	{
		long nColumn = this->GetColumnIndex(pszColumn);
		ATLASSERT(nColumn>=0);
		SortColumn(nColumn);
	}

	std::vector<int> m_sort_flag;

	void SortColumn(long nColumn)
	{
		m_grid.EndEdit(false);
		m_sort_flag.resize(nColumn + 1,0);

		CColumn& col = *m_grid.m_columns[nColumn];

		if(!m_sort_flag[nColumn])
		{
			if(col.m_vt	== VT_INT)
				std::stable_sort(m_grid.m_rows.begin(),m_grid.m_rows.end(),compareint(nColumn));
			else
				std::stable_sort(m_grid.m_rows.begin(),m_grid.m_rows.end(),compare(nColumn));
		}else
		{
			if(col.m_vt	== VT_INT)
				std::stable_sort(m_grid.m_rows.rbegin(),m_grid.m_rows.rend(),compareint(nColumn));
			else
				std::stable_sort(m_grid.m_rows.rbegin(),m_grid.m_rows.rend(),compare(nColumn));
		}

		m_sort_flag[nColumn] ^= 1;
	}

	//�\�[�g�p
	struct compare : std::binary_function<CRow*,CRow*,bool>
	{
		bool operator()(CRow* x, CRow* y)const
		{
			_variant_t vta = x->GetItem(m_nColumn);
			_variant_t vtb = y->GetItem(m_nColumn);
			return (strcmp((_bstr_t)vta,(_bstr_t)vtb) <	0);
		}
		int	m_nColumn;
	public:
		compare(int	nColumn)
		{
			m_nColumn =	nColumn;
		}
	};
	struct compareint :	std::binary_function<CRow*,CRow*,bool>
	{
		bool operator()(CRow* x, CRow* y)const
		{
			_variant_t vta = x->GetItem(m_nColumn);
			_variant_t vtb = y->GetItem(m_nColumn);
			return (vta.intVal < vtb.intVal);
		}
		int	m_nColumn;
	public:
		compareint(int nColumn)
		{
			m_nColumn =	nColumn;
		}
	};

	_variant_t GetEditItem(long	nColumn) {
		ATLASSERT(nColumn<GetColumnCount());
		ATLASSERT(m_grid.m_bEditing);
		return m_grid.m_columns[nColumn]->GetValue();
	}

	_variant_t GetEditItem(LPCTSTR pszColumn) {
		long nColumn = this->GetColumnIndex(pszColumn);
		ATLASSERT(nColumn>=0);
		return GetEditItem(nColumn);
	}

	void SetColumnFocus(LPCTSTR	pszColumn) {
		ATLASSERT(m_grid.m_bEditing);
		long nColumn = this->GetColumnIndex(pszColumn);
		ATLASSERT(nColumn>=0);
		m_grid.m_columns[nColumn]->SetFocus();
	}

	void AddColumnLookup(long nColumn,_variant_t vtID,_variant_t vtDisplay)	{
		m_grid.m_columns[nColumn]->AddLookup(vtID,vtDisplay);
	}
	
	void AddColumnLookup(LPCTSTR pszColumn,_variant_t vtID,_variant_t vtDisplay) {
		long nColumn = GetColumnIndex(pszColumn);
		ATLASSERT(nColumn>=0);
		AddColumnLookup(nColumn,vtID,vtDisplay);
	}
	void ClearColumnLookup(long nColumn)	{
		m_grid.m_columns[nColumn]->ClearLookup();
	}
	
	void ClearColumnLookup(LPCTSTR pszColumn) {
		long nColumn = GetColumnIndex(pszColumn);
		ATLASSERT(nColumn>=0);
		ClearColumnLookup(nColumn);
	}
	
	void SetListener(CListener*	pListener) {
		m_grid.m_pListener = pListener;
	}

	void DeleteAllItems() {
		m_grid.DeleteAllItems();
	}

	void DeleteAllColumns()	{
		m_grid.DeleteAllColumns();
	}

	void SetItemData(long nItem,_variant_t vtItemData) {
		ATLASSERT(nItem>=0 && nItem<(long)m_grid.m_rows.size());
		m_grid.m_rows[nItem]->m_vtItemData = vtItemData;
	}

	_variant_t GetItemData(long	nItem) const {
		ATLASSERT(nItem>=0 && nItem<(long)m_grid.m_rows.size());
		return m_grid.m_rows[nItem]->m_vtItemData;
	}

	void DeleteItem(long nItem)	{
		ATLASSERT(nItem<(long)m_grid.m_rows.size());
		
		if(m_grid.m_pListener && !m_grid.m_pListener->OnDeleteRow(GetWindowLong(GWL_ID),nItem))
			return;

		m_grid.SelectRow(-1);
		delete m_grid.m_rows[nItem];
		m_grid.m_rows.erase(m_grid.m_rows.begin() +	nItem);
		UpdateScrollSize();
	}
	
	void DeleteItem()
	{
		long lRowCount = m_grid.m_rows.size();
		for(int i = lRowCount - 1;i >= 0;i--)
		{
			if(m_grid.IsSelectedRow(i))
			{
				delete m_grid.m_rows[i];
				m_grid.m_rows.erase(m_grid.m_rows.begin() +	i);
			}
		}
		m_grid.SetCurRow(-1);
		UpdateScrollSize();
	}

	void EnsureVisible(long	nRow) {
		CPoint ptScroll;
		CRect rcClient;
		m_grid.GetScrollOffset(ptScroll);
		m_grid.GetClientRect(rcClient);
		long nRowOffset	= nRow * GetRowHeight();
		if(nRowOffset<ptScroll.y)
			ptScroll.y = nRowOffset;
		else if(nRowOffset>ptScroll.y -	GetRowHeight() + rcClient.Height())
			ptScroll.y = nRowOffset	- rcClient.Height()	+ GetRowHeight();
		else
			return;
		m_grid.SetScrollOffset(ptScroll);
	}
	
	static bool	IsNull(const _variant_t& vt) {
		return vt.vt ==	VT_EMPTY ||	vt.vt == VT_NULL;
	}
	
	static void	CGridCtrl::SetNull(_variant_t& vt) {
		ChangeType(vt,VT_NULL);
	}
	
	void SetExtendedGridStyle(DWORD	dwStyle,DWORD dwMask=0)	{
		m_grid.m_dwStyle &=	~dwMask;
		m_grid.m_dwStyle |=	dwStyle;
		UpdateScrollSize();
	}
	
	bool GetModified(long nRow=-1) {
		ATLASSERT(nRow<(long)m_grid.m_rows.size());

		if((nRow == GetSelectedRow() || nRow<0) && m_grid.GetEditChanges())
			return true;

		if(nRow==-1) {
			long count = (long)m_grid.m_rows.size();
			while(count--) {
				if(m_grid.m_rows[count]->m_bModified)
					return true;
			}
			return false;
		} else {
			return m_grid.m_rows[nRow]->m_bModified;
		}
	}
	
	void ClearModified(long	nRow=-1) {
		if(nRow>=0)	{
			m_grid.m_rows[nRow]->m_bModified = false;
		} else {
			long count = (long)m_grid.m_rows.size();
			while(count--) m_grid.m_rows[count]->m_bModified = false;
		}
		m_grid.Invalidate();
	}
	
	long GetRowCount() const {
		return (long)m_grid.m_rows.size();
	}

	long GetColumnCount() const	{
		return m_grid.m_columns.GetSize();
	}

	void SelectAll()
	{
		m_grid.SetCurRow(0);
		m_grid.SetSelect(-1,true);
		EnsureVisible(0);
		m_grid.Invalidate();
	}

// Classes
public:
	class CListener	{
	public:
		virtual	bool OnRowChanging(UINT	uID,long nRow) {
			// Return false	to abort row change
			return true;
		}
		
		virtual	void OnRowChanged(UINT uID,long	nRow) {}
		
		virtual	void OnEdit(UINT uID,long nRow)	{}
		
		virtual	bool OnDeleteRow(UINT uID,long nRow) {
			// Return false	to abort delete
			return true;
		}
		
		virtual	void OnNewRow(UINT uID,long	nRow) {}
		
		virtual	void OnModified(UINT uID,LPCTSTR pszColumn,_variant_t vtValue) {}
		
		virtual	void OnRowActivate(UINT	uID,long nRow) {}
		
		virtual	COLORREF GetCellColor(UINT uID,long	nRow,LPCTSTR pszColumn)	{
			// Return (COLORREF)-1 for default colors
			return (COLORREF)-1;
		}

		virtual	bool OnValidate(UINT uID) {
			// Return false	if not valid
			return true;
		}
	};
protected:
	class CEditText	: public CWindowImpl<CEditText,CEdit> {
	public:
		DECLARE_WND_CLASS_EX(NULL,0,-1)

		BEGIN_MSG_MAP(CEditText)
			MESSAGE_HANDLER(WM_CHAR,OnChar)
			MESSAGE_HANDLER(WM_KILLFOCUS,OnKillFocus)
		END_MSG_MAP()

		CEditText(CColumn& column) : m_column(column) {}

		LRESULT	OnChar(UINT	/*uMsg*/, WPARAM wParam, LPARAM	lParam,	BOOL& bHandled)	{
			bHandled = FALSE;
			if(wParam==VK_RETURN ||	wParam==VK_ESCAPE) {
				m_column.m_grid.EndEdit(wParam==VK_ESCAPE);
				bHandled = TRUE;
			}
			return 0;
		}

		LRESULT	OnKillFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
			m_column.ValueModified();
			bHandled = FALSE;
			return 0;
		}
	protected:
		CColumn&	m_column;
	}; 

	class CEditCheck : public CWindowImpl<CEditCheck,CButton>
	{
	public:
		DECLARE_WND_CLASS_EX(NULL,0,-1)

		BEGIN_MSG_MAP(CEditCheck)
			MESSAGE_HANDLER(WM_CHAR,OnChar)
			MESSAGE_HANDLER(WM_KILLFOCUS,OnKillFocus)
			MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
		END_MSG_MAP()

		CEditCheck(CColumn&	column)	: m_column(column) {}

		LRESULT	OnChar(UINT	/*uMsg*/, WPARAM wParam, LPARAM	lParam,	BOOL& bHandled)	{
			bHandled = FALSE;
			if(wParam==VK_RETURN ||	wParam==VK_ESCAPE) {
				m_column.m_grid.EndEdit(wParam==VK_ESCAPE);
				bHandled = TRUE;
			}
			return 0;
		}

		LRESULT	OnKillFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
			m_column.ValueModified();
			bHandled = FALSE;
			return 0;
		}

		LRESULT	OnEraseBkgnd(UINT /*uMsg*/,	WPARAM wParam, LPARAM lParam, BOOL&	bHandled) 
		{
			return 1;
		}

	protected:
		CColumn&	m_column;
	}; 

	class CEditDropdownlist	: public CWindowImpl<CEditDropdownlist,CComboBox> {
	public :
		DECLARE_WND_CLASS_EX(NULL,0,-1)

		BEGIN_MSG_MAP(CEditDropdownlist)
			MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
			MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
			REFLECTED_COMMAND_CODE_HANDLER(CBN_SELCHANGE,OnSelChange)
			DEFAULT_REFLECTION_HANDLER()
		END_MSG_MAP()

		CEditDropdownlist(CColumn& column) : m_column(column) {}
		
		void SetValue(_variant_t vtValue) {
			long count = GetCount();
			while(count--) {
				_variant_t vt =	(_variant_t*)GetItemData(count);
				if(vt==vtValue)	{
					SetCurSel(count);
					return;
				}
			}
			SetCurSel(-1);
		}

		void ResetContent()	{
			long count = GetCount();
			while(count--) {
				_variant_t*	pValue = (_variant_t*)GetItemData(count);
				delete pValue;
			}
			CWindowImpl<CEditDropdownlist,CComboBox>::ResetContent();
		}

		LRESULT	OnKeyDown(UINT /*uMsg*/, WPARAM	wParam,	LPARAM lParam, BOOL& bHandled) {
			bHandled = FALSE;
			if(!GetDroppedState() && (wParam==VK_RETURN	|| wParam==VK_ESCAPE)) {
				m_column.m_grid.EndEdit(wParam==VK_ESCAPE);
				bHandled = TRUE;
			}
			return 0;
		}

		LRESULT	OnDestroy(UINT,WPARAM,LPARAM,BOOL& bHandled) {
			bHandled = FALSE;
			ResetContent();
			return 0;
		}

		LRESULT	OnSelChange(WORD /*wNotifyCode*/, WORD /*wID*/,	HWND /*hWndCtl*/, BOOL&	bHandled) {
			m_column.ValueModified();
			return 0;
		}
	protected:
		CColumn&	m_column;
	};

	class CEditDropdown	: public CWindowImpl<CEditDropdown,CComboBox> {
	public :
		DECLARE_WND_CLASS_EX(NULL,0,-1)

		BEGIN_MSG_MAP(CEditDropdown)
			MESSAGE_HANDLER(WM_CREATE,OnCreate)
			MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
			MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
			REFLECTED_COMMAND_CODE_HANDLER(CBN_SELCHANGE,OnSelChange)
			DEFAULT_REFLECTION_HANDLER()
		END_MSG_MAP()

		CEditDropdown(CColumn& column) : m_column(column), m_wndEdit(*this)	{}
		
		void SetValue(_variant_t vtValue) {
			long count = GetCount();
			while(count--) {
				_variant_t vt =	(_variant_t*)GetItemData(count);
				if(vt==vtValue)	{
					SetCurSel(count);
					return;
				}
			}
			SetCurSel(-1);
		}

		void ResetContent()	{
			long count = GetCount();
			while(count--) {
				_variant_t*	pValue = (_variant_t*)GetItemData(count);
				delete pValue;
			}
			CWindowImpl<CEditDropdown,CComboBox>::ResetContent();
		}

		LRESULT	OnCreate(UINT uMsg,	WPARAM wParam, LPARAM lParam, BOOL&	bHandled) {
			DefWindowProc(uMsg,wParam,lParam);
			m_wndEdit.SubclassWindow(GetDlgItem(1001));
			return 0;
		}
		
		LRESULT	OnKeyDown(UINT /*uMsg*/, WPARAM	wParam,	LPARAM lParam, BOOL& bHandled) {
			bHandled = FALSE;
			if(!GetDroppedState() && (wParam==VK_RETURN	|| wParam==VK_ESCAPE)) {
				m_column.m_grid.EndEdit(wParam==VK_ESCAPE);
				bHandled = TRUE;
			}
			return 0;
		}
		
		LRESULT	OnDestroy(UINT,WPARAM,LPARAM,BOOL& bHandled) {
			bHandled = FALSE;
			ResetContent();
			return 0;
		}
		
		LRESULT	OnSelChange(WORD /*wNotifyCode*/, WORD /*wID*/,	HWND /*hWndCtl*/, BOOL&	bHandled) {
			m_column.ValueModified();
			return 0;
		}
	protected:
		class CDropDownEdit	: public CWindowImpl<CEditText,CEdit> {
		public:
			DECLARE_WND_CLASS_EX(NULL,0,-1)

			BEGIN_MSG_MAP(CDropDownEdit)
				MESSAGE_HANDLER(WM_CHAR,OnChar)
				MESSAGE_HANDLER(WM_KEYUP,OnKeyUp)
			END_MSG_MAP()

			CDropDownEdit(CEditDropdown& dropdown) : m_dropdown(dropdown) {}
			
			LRESULT	OnChar(UINT	/*uMsg*/, WPARAM wParam, LPARAM	lParam,	BOOL& bHandled)	{
				bHandled = FALSE;
				if(!m_dropdown.GetDroppedState() &&	(wParam==VK_RETURN || wParam==VK_ESCAPE)) {
					m_dropdown.m_column.m_grid.EndEdit(wParam==VK_ESCAPE);
					bHandled = TRUE;
				}
				return 0;
			}

			LRESULT	OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				if(wParam==VK_UP ||	wParam==VK_DOWN	|| wParam==VK_LEFT || wParam==VK_RIGHT)
					return 0;

				if(wParam==VK_DELETE ||	wParam==VK_BACK	|| wParam==VK_HOME || wParam==VK_END ||	wParam==VK_TAB)
					return 0;

				ATL::CString str;
				GetWindowText(str);
				if(str.IsEmpty())
					return 0;

				long i = m_dropdown.FindString(0,str);
				if(i>=0) {
					m_dropdown.SetCurSel(i);
					ATL::CString strFull;
					m_dropdown.GetLBText(i,strFull);
					SetWindowText(str +	strFull.Mid(str.GetLength()));
					SetSel(str.GetLength(),strFull.GetLength());
				}
				return 0;
			}
		protected:
			CEditDropdown&	m_dropdown;
		}; 

		CColumn&	m_column;
		CDropDownEdit	m_wndEdit;
	};

	class CCell	{
	public:
		CCell(CONTROLTYPE type,CColumn&	column)	: m_type(type),	m_column(column) {}

		virtual	void Create(CFontHandle	hFont) {
			m_hWnd = NULL;
		}

		virtual	_bstr_t	GetDisplayText(const _variant_t	vtValue) {
			if(IsNull(vtValue))
				return _bstr_t();

			long count = m_column.m_lookup.GetSize();
			if(count>0)	{
				while(count--) {
					if(m_column.m_lookup[count]->m_vtID	== vtValue)
						return m_column.m_lookup[count]->m_vtDisplay;
				}
				ATL::CString str;
				str.Format("[%s]",(LPCTSTR)(_bstr_t)vtValue);
				return (_bstr_t)str;
			}

			return (_bstr_t)vtValue;
		}

		virtual	void SetWindowPos(RECT&	rc,DWORD dwSWPFlags) {
			if(!m_hWnd)	return;
			CWindow	wnd(m_hWnd);
			wnd.SetWindowPos(NULL,&rc,dwSWPFlags);
		}

		virtual	void SetValue(_variant_t vtValue) {
			m_column.m_vtValue = vtValue;	// FIX
		}

		virtual	void SetFocus()	{
		}

		virtual	_variant_t GetValue() {
			return m_column.m_vtValue;
		}

		virtual	void Draw(CDCHandle	dc,CRect& rc,const _variant_t& vtValue)	{
			ATL::CString str = (LPCTSTR)(_bstr_t)GetDisplayText(vtValue);

			CGridDC	mdc(dc);
			mdc.DrawText(rc,str,m_column.m_nAlign);
		}
	
		CONTROLTYPE	m_type;
		HWND		m_hWnd;
		CColumn&	m_column;
	};

	class CCellNone	: public CGridCtrl::CCell {
	public:
		CCellNone(CColumn& column) : CCell(EDIT_NONE,column) {
		}
	};

	class CCellInt : public	CGridCtrl::CCell {
	public:
		CCellInt(CColumn& column,ALIGN nAlign) : CCell(EDIT_INT,column), m_wndEdit(column)
		{}

		virtual	void Create(CFontHandle	hFont) {
			DWORD dwFlags =	0;
			if(m_column.m_nAlign==RIGHT)
				dwFlags	|= ES_RIGHT;
			else if(m_column.m_nAlign==CENTER)
				dwFlags	|= ES_CENTER;

			m_hWnd = m_wndEdit.Create(m_column.m_grid,rcDefault,NULL,WS_CHILD|WS_TABSTOP|ES_NUMBER|ES_AUTOHSCROLL|dwFlags);
			m_wndEdit.SetFont(hFont);
		}

		virtual	void SetValue(_variant_t vtValue) {
			CCell::SetValue(vtValue);
			if(IsNull(vtValue))
				m_wndEdit.SetWindowText("");
			else
				m_wndEdit.SetWindowText((_bstr_t)vtValue);
		}

		virtual	void SetFocus()	{
			m_wndEdit.SetFocus();
			m_wndEdit.SetSel(0,-1);
		}

		virtual	_variant_t GetValue() {
			_variant_t vt;
			SetNull(vt);
			ATL::CString str;
			m_wndEdit.GetWindowText(str);
			if(!str.IsEmpty()) {
				vt = (LPCTSTR)str;
				if(!ChangeType(vt,m_column.m_vt))
					vt = m_column.m_vtValue;
			}
			return vt;
		}
	protected:
		CEditText	m_wndEdit;
	};

	class CCellText	: public CGridCtrl::CCell {
	public:
		CCellText(CColumn& column,ALIGN	nAlign)	: CCell(EDIT_TEXT,column), m_wndEdit(column) {
		}

		virtual	void Create(CFontHandle	hFont) {
			DWORD dwFlags =	0;
			if(m_column.m_nAlign==RIGHT)
				dwFlags	|= ES_RIGHT;
			else if(m_column.m_nAlign==CENTER)
				dwFlags	|= ES_CENTER;

			m_hWnd = m_wndEdit.Create(m_column.m_grid,rcDefault,NULL,WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|dwFlags,WS_EX_CLIENTEDGE);
			m_wndEdit.SetFont(hFont);
		}

		virtual	void SetValue(_variant_t vtValue) {
			CCell::SetValue(vtValue);
			if(IsNull(vtValue))
				m_wndEdit.SetWindowText("");
			else
				m_wndEdit.SetWindowText((_bstr_t)vtValue);
		}

		virtual	void SetFocus()	{
			m_wndEdit.SetFocus();
			m_wndEdit.SetSel(0,-1);
		}

		virtual	_variant_t GetValue() {
			_variant_t vt;
			SetNull(vt);
			ATL::CString str;
			m_wndEdit.GetWindowText(str);
			if(!str.IsEmpty()) {
				vt = (LPCTSTR)str;
				if(!ChangeType(vt,m_column.m_vt))
					vt = m_column.m_vtValue;
			}
			return vt;
		}
	protected:
		CEditText	m_wndEdit;
	};


	class CCellCheck : public CGridCtrl::CCell {
	public:
		CCellCheck(CColumn&	column)	: CCell(EDIT_CHECK,column),	m_wndEdit(column) {
		}

		virtual	void Create(CFontHandle	hFont) {
			DWORD dwFlags =	0;

			m_hWnd = m_wndEdit.Create(m_column.m_grid,rcDefault,NULL,WS_CHILD|WS_TABSTOP|BS_AUTOCHECKBOX|dwFlags,WS_EX_TRANSPARENT);
			m_wndEdit.SetFont(hFont);
			//m_wndEdit.SetWindowText(m_column.m_strTitle);
		}

		virtual	void Draw(CDCHandle	dc,CRect& rc,const _variant_t& vtValue)	{
			if(!IsNull(vtValue))
			{
				DWORD dwFlag = 0;
				if((long)vtValue)
					dwFlag |= DFCS_CHECKED;
				rc.DeflateRect(3,3);//
				dc.DrawFrameControl(&rc,DFC_BUTTON,DFCS_BUTTONCHECK|dwFlag);
			}
		}

		virtual	void SetValue(_variant_t vtValue) {
			CCell::SetValue(vtValue);
			if(IsNull(vtValue))
				m_wndEdit.SetCheck(FALSE);
			else
				m_wndEdit.SetCheck((int)vtValue!=0);
		}

		virtual	void SetFocus()	{
			m_wndEdit.SetFocus();
		}

		virtual	_variant_t GetValue()
		{
			_variant_t vt =	(int)m_wndEdit.GetCheck();
			return vt;
		}
	protected:
		CEditCheck	m_wndEdit;
	};

	class CCellDropdownlist	: public CGridCtrl::CCell {
	public:
		CCellDropdownlist(CColumn& column) : CCell(EDIT_DROPDOWNLIST,column), m_wndEdit(column)	{
		}

		virtual	void Create(CFontHandle	hFont) {
			m_hWnd = m_wndEdit.Create(m_column.m_grid,rcDefault,NULL,WS_CHILD|WS_TABSTOP|WS_VSCROLL|CBS_AUTOHSCROLL|CBS_DROPDOWNLIST,0);
			m_wndEdit.SetFont(hFont);
		}

		virtual	void SetWindowPos(RECT&	rc,DWORD dwSWPFlags) {
			if(dwSWPFlags &	SWP_SHOWWINDOW)	{
				m_wndEdit.ResetContent();
				for(long i=0;i<m_column.m_lookup.GetSize();i++)	{
					long iItem = m_wndEdit.AddString((_bstr_t)m_column.m_lookup[i]->m_vtDisplay);
					_variant_t*	pValue = new _variant_t(m_column.m_lookup[i]->m_vtID);
					m_wndEdit.SetItemData(iItem,(DWORD_PTR)pValue);
				}
			}
			rc.bottom += 100;
			CCell::SetWindowPos(rc,dwSWPFlags);
		}

		virtual	void SetValue(_variant_t vtValue) {
			CCell::SetValue(vtValue);
			m_wndEdit.SetValue(vtValue);
		}

		virtual	void SetFocus()	{
			m_wndEdit.SetFocus();
		}

		virtual	_variant_t GetValue() {
			_variant_t vt;
			SetNull(vt);
			long nItem = m_wndEdit.GetCurSel();
			if(nItem>=0) {
				vt = (_variant_t*)m_wndEdit.GetItemData(nItem);
				if(!ChangeType(vt,m_column.m_vt))
					vt = m_column.m_vtValue;
			}
			return vt;
		}
	protected:
		CEditDropdownlist	m_wndEdit;
	};

	class CCellDropdown	: public CGridCtrl::CCell {
	public:
		CCellDropdown(CColumn& column) : CCell(EDIT_DROPDOWN,column), m_wndEdit(column)	{
		}

		virtual	void Create(CFontHandle	hFont) {
			m_hWnd = m_wndEdit.Create(m_column.m_grid,rcDefault,NULL,WS_CHILD|WS_TABSTOP|WS_VSCROLL|CBS_AUTOHSCROLL|CBS_DROPDOWN,0);
			m_wndEdit.SetFont(hFont);
		}

		virtual	void AddLookup(long	nValue,_variant_t vtDisplay) {
			long nItem = m_wndEdit.AddString((_bstr_t)vtDisplay);
			m_wndEdit.SetItemData(nItem,nValue);
		}

		virtual	void SetWindowPos(RECT&	rc,DWORD dwSWPFlags) {
			if(dwSWPFlags &	SWP_SHOWWINDOW)	{
				m_wndEdit.ResetContent();
				for(long i=0;i<m_column.m_lookup.GetSize();i++)	{
					long iItem = m_wndEdit.AddString((_bstr_t)m_column.m_lookup[i]->m_vtDisplay);
					_variant_t*	pValue = new _variant_t(m_column.m_lookup[i]->m_vtID);
					m_wndEdit.SetItemData(iItem,(DWORD_PTR)pValue);
				}
			} else if(dwSWPFlags & SWP_HIDEWINDOW) {
				m_wndEdit.ShowDropDown(FALSE);
			}
			rc.bottom += 100;
			CCell::SetWindowPos(rc,dwSWPFlags);
		}

		virtual	void SetValue(_variant_t vtValue) {
			CCell::SetValue(vtValue);
			m_wndEdit.SetValue(vtValue);
		}

		virtual	void SetFocus()	{
			m_wndEdit.SetFocus();
		}

		virtual	_variant_t GetValue() {
			_variant_t vt;
			SetNull(vt);
			long nItem = m_wndEdit.GetCurSel();
			if(nItem>=0) {
				vt = (_variant_t*)m_wndEdit.GetItemData(nItem);
				if(!ChangeType(vt,m_column.m_vt))
					vt = m_column.m_vtValue;
			}
			return vt;
		}
	protected:
		CEditDropdown	m_wndEdit;
	};

	class CColumn {
	public:
		CColumn(CGrid& grid) : m_grid(grid)	{
			m_pEdit	= NULL;
		}

		virtual	~CColumn() {
			long count = m_lookup.GetSize();
			while(count--) delete m_lookup[count];
			m_lookup.RemoveAll();
			if(m_pEdit)	{
				delete m_pEdit;
				m_pEdit	= NULL;
			}
		}
		
		bool OnEdge(long x,long	cx)	{
			return cx >	x +	m_nWidth - 2 &&	cx < x + m_nWidth +	2;
		}
		bool OnFace(long x,long	cx)	{
			return cx >	x +	2 && cx	< x	+ m_nWidth - 2;
		}

		bool GetReadOnly() {
			return m_nType == EDIT_NONE	|| (m_grid.m_dwStyle & GS_EX_READONLY);
		}

		void CreateEdit() {
			switch(m_nType)	{
				case EDIT_NONE:
					m_pEdit	= new CCellNone(*this);
					break;
				case EDIT_TEXT:
					m_pEdit	= new CCellText(*this,m_nAlign);
					break;
				case EDIT_INT:
					m_pEdit	= new CCellInt(*this,m_nAlign);
					break;
				case EDIT_DROPDOWN:
					m_pEdit	= new CCellDropdown(*this);
					break;
				case EDIT_DROPDOWNLIST:
					m_pEdit	= new CCellDropdownlist(*this);
					break;
				case EDIT_CHECK:
					m_pEdit	= new CCellCheck(*this);
					break;
			}
			if(m_pEdit)	{
				m_pEdit->Create((HFONT)m_grid.m_font);
			}
		}

		_bstr_t	GetDisplayText(_variant_t vtValue) {
			if(m_pEdit)
				return m_pEdit->GetDisplayText(vtValue);
			else if(IsNull(vtValue))
				return _bstr_t();
			else
				return vtValue;
		}
		
		void SetWindowPos(RECT&	rc,DWORD dwSWPFlags) {
			if(m_pEdit)
				m_pEdit->SetWindowPos(rc,dwSWPFlags);
		}
		
		void SetValue(_variant_t vtValue) {
			if(m_pEdit)
				m_pEdit->SetValue(vtValue);
		}
		
		void SetFocus()	{
			if(m_pEdit)
				m_pEdit->SetFocus();
		}
		
		_variant_t GetValue() {
			if(m_pEdit)
				return m_pEdit->GetValue();
			else
				return m_vtValue;
		}
		
		void AddLookup(_variant_t vtID,_variant_t vtDisplay) {
			vtID.ChangeType(m_vt);
			CData* p = new CData;
			p->m_vtID =	vtID;
			p->m_vtDisplay = vtDisplay;
			m_lookup.Add(p);
		}

		void ClearLookup()
		{
			long count = m_lookup.GetSize();
			while(count--) delete m_lookup[count];
			m_lookup.RemoveAll();
		}
		
		void ValueModified() {
			if(!m_pEdit	|| !m_grid.m_pListener)
				return;

			_variant_t vtValue = GetValue();
			
			if(vtValue != m_vtValue) {
				if(IsNull(vtValue))
					m_grid.m_pListener->OnModified(m_grid.m_wrapper.GetWindowLong(GWL_ID),m_strName,"");
				else
					m_grid.m_pListener->OnModified(m_grid.m_wrapper.GetWindowLong(GWL_ID),m_strName,vtValue);
			}
		}

		void Draw(CDCHandle	dc,CRect& rc,const _variant_t& vtValue)	{
			if(m_pEdit)	m_pEdit->Draw(dc,rc,vtValue);
		}
		typedef	struct CData {
			_variant_t	m_vtID;
			_variant_t	m_vtDisplay;
		} CData;

		CString					m_strName, m_strTitle;
		long					m_nWidth;
		long					m_nType;
		CGrid&					m_grid;
		CCell*					m_pEdit;
		CSimpleArray<CData*>	m_lookup;
		VARTYPE					m_vt;
		ALIGN					m_nAlign;
		_variant_t				m_vtValue;
	};

	class CRow {
	public:
		CRow(CGrid&	grid,long nItems) :	m_grid(grid) {
			CColumn& col = GetColumn(0);
			m_items	= new _variant_t[nItems];

			m_vtItemData = 0;
			m_bModified	= false;
			m_bSelect = false;
		}

		virtual	~CRow()	{
			delete []m_items;
		}
		
		_variant_t GetItem(long	nItem) {
			CColumn& col = GetColumn(nItem);
			_variant_t vt =	m_items[nItem];
			if(!CGridCtrl::IsNull(vt))
				if(!ChangeType(vt,col.m_vt))
					SetNull(vt);
			ATLASSERT(CGridCtrl::IsNull(vt)	|| vt.vt==col.m_vt);
			return vt;
		}
		
		_bstr_t	GetItemDisplay(long	nItem) {
			CColumn& col = GetColumn(nItem);
			return col.GetDisplayText(m_items[nItem]);
		}
		
		void SetItem(long nItem,_variant_t vtValue)	{
			CColumn& col = GetColumn(nItem);
			if(IsNull(vtValue))
				SetNull(vtValue);
			else {
				if(!ChangeType(vtValue,col.m_vt))
					vtValue	= col.m_vtValue;
			}

			m_items[nItem] = vtValue;
		}

		CColumn& GetColumn(long	nColumn) {
			ATLASSERT(nColumn>=0 &&	nColumn<m_grid.m_columns.GetSize());
			return *m_grid.m_columns[nColumn];
		}


		_variant_t*		m_items;
		CGrid&			m_grid;
		_variant_t		m_vtItemData;
		bool			m_bModified;
		bool			m_bSelect;
	};

	class CGridDC {
	public:
		CGridDC(HDC	hdc) : m_dc(hdc) {}
		
		void DrawText(CRect& rc,ATL::CString& str,ALIGN	xalign)	{
			int	old	= m_dc.SetBkMode(TRANSPARENT);
			int	len	= str.GetLength();

			long x = rc.left;
			long y = rc.top;

			SIZE sz;
			m_dc.GetTextExtent(str,str.GetLength(),&sz);

			y += rc.Height()/2 - sz.cy/2;
			
			if(sz.cx > rc.Width()) {
				ATL::CString tmp;
				long len = str.GetLength();
				do {
					tmp	= str.Left(--len) +	"...";
					m_dc.GetTextExtent(tmp,tmp.GetLength(),&sz);
				} while(sz.cx >	rc.Width() && len>3);

				m_dc.TextOut(x,y,tmp,tmp.GetLength());
			} else {
				switch(xalign) {
					case CENTER: 
						x += rc.Width()/2 -	sz.cx/2;	
						break;
					case RIGHT:	
						x += rc.Width()	- sz.cx;   
						break;
				}
				m_dc.TextOut(x,y,str,len);
			}
			m_dc.SetBkMode(old);
		}

		void DrawButton(const CRect& rc,bool bSunken) {
			m_dc.FillRect(&rc, ::GetSysColorBrush(COLOR_3DFACE));

			if(bSunken)
				m_dc.Draw3dRect(rc.left,rc.top,rc.Width(),rc.Height(),
								::GetSysColor(COLOR_3DSHADOW),
								::GetSysColor(COLOR_3DHILIGHT));
			else
				m_dc.Draw3dRect(rc.left,rc.top,rc.Width(),rc.Height(),
								::GetSysColor(COLOR_3DHILIGHT),
								::GetSysColor(COLOR_3DSHADOW));
		}
	protected:
		CDCHandle	m_dc;
	};


	class CGrid	: public CScrollWindowImpl<CGrid, CFSBWindow> {
		typedef	CScrollWindowImpl<CGrid, CFSBWindow> baseClass;
		friend class CRow;
		friend class CColumn;
		friend class CGridCtrl;
		friend class CCell;
		friend class CEditText;
		friend class CEditDropdown;
		friend class CEditDropdownlist;
		friend class CEditDate;
	public:
		DECLARE_WND_CLASS_EX(NULL,CS_DBLCLKS,-1)

		BEGIN_MSG_MAP(CGrid)
			MSG_WM_SIZE(OnSize)
			MESSAGE_HANDLER(WM_CREATE,OnCreate)
			MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
			MESSAGE_HANDLER(WM_RBUTTONDOWN,OnLButtonDown)
			MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
			MESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUp)
			MESSAGE_HANDLER(WM_RBUTTONUP,OnLButtonUp)
			MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
			CHAIN_MSG_MAP(baseClass)
			if(uMsg==WM_COMMAND	|| uMsg==WM_NOTIFY)	{
				REFLECT_NOTIFICATIONS()
			}
		END_MSG_MAP()

		void OnSize(UINT uType, CSize size)
		{
			//�X�N���[���o�[����ɏ�����̑΍�
			CSize ss;
			GetScrollSize(ss);

			if(size.cx < ss.cx)
				FlatSB_ShowScrollBar(SB_HORZ);
			if(size.cy < ss.cy)
				FlatSB_ShowScrollBar(SB_VERT);

			SetMsgHandled(false);
		}


		LRESULT	OnCreate(UINT /*uMsg*/,	WPARAM wParam, LPARAM /*lParam*/, BOOL&	bHandled) {
			bHandled = FALSE;
			// Find	minimum	height for combo box
			CComboBox box;
			box.Create(m_hWnd,rcDefault,NULL,WS_CHILD|CBS_DROPDOWNLIST,0);
			box.SetFont(m_font);
			CRect rc;
			box.GetClientRect(rc);
			m_nControlHeight = rc.Height();
			box.DestroyWindow();

			return 0;
		}

		LRESULT	OnLButtonDown(UINT /*uMsg*/, WPARAM	wParam,	LPARAM lParam, BOOL& /*bHandled*/)
		{
			CPoint pt((DWORD)lParam), ptScroll;
			GetScrollOffset(ptScroll);
			pt += ptScroll;
			
			long nColumn = GetPointColumn(pt);
			long row = GetPointRow(pt);

			//���ڕύX����@�`�F�b�N�{�b�N�X
			if(nColumn != -1 && row != -1 && m_columns[nColumn]->m_nType == EDIT_CHECK)
			{
				_variant_t vt = m_wrapper.GetItem(row,nColumn);
				vt.intVal ^= BST_CHECKED;
				m_wrapper.SetItem(row,nColumn,vt);
				Invalidate();
				return 0;
			}

			m_pt = pt;
			m_bDrag = true;
			m_DragRect.SetRectEmpty();
			SetCapture();
			SelectRow(row);
			SetFocus();

			return 0;
		}
		
		LRESULT	OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
		{
			CPoint pt((DWORD)lParam), ptScroll;;
			GetScrollOffset(ptScroll);
			pt += ptScroll;

			if(m_bDrag)
			{
				CPoint ptc;
				GetCursorPos(&ptc);
				ScreenToClient(&ptc);
				CRect rcClient;
				GetClientRect(rcClient);

				if(ptc.y < rcClient.top)
				{
			        MSG msg;
					if(::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
					{
						return 0;
					}else
                        this->ScrollLineUp();

				}
				if(ptc.y > rcClient.bottom)
				{
			        MSG msg;
					if(::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
					{
						return 0;
					}else
						this->ScrollLineDown();
				}

				long row = GetPointRow(pt);
                long row_from = GetCurRow();

				m_DragRect.SetRect(m_pt,pt);
				m_DragRect.NormalizeRect();

				if(row != -1 && row_from != -1)
				{
					if(row < row_from)
						std::swap(row,row_from);
					SetSelect(-1,false);
					for(;row_from <= row;row--)
						SetSelect(row,true);
				}
				Invalidate();
				return 0;
			}
			return 0;
		}

		LRESULT	OnLButtonUp(UINT /*uMsg*/, WPARAM	wParam,	LPARAM lParam, BOOL& /*bHandled*/) 
		{
			CPoint pt((DWORD)lParam);
			GetScrollOffset(pt);
			pt.x +=	GET_X_LPARAM(lParam);
			pt.y +=	GET_Y_LPARAM(lParam);
			
			long nColumn = GetPointColumn(pt);
			long row = GetPointRow(pt);

			if(m_bDrag)
			{
				CPoint ptScroll;
				GetScrollOffset(ptScroll);
				pt += ptScroll;

				ReleaseCapture();
				m_bDrag = false;
				Invalidate();
			}
			EditRow(row,nColumn);
			return 0;
		}

		LRESULT	OnKeyDown(UINT /*uMsg*/, WPARAM	wParam,	LPARAM /*lParam*/, BOOL& /*bHandled*/) {
			switch(wParam) {
				case VK_RETURN:
					EditRow();
					break;
				case VK_UP:
					if(GetCurRow() > 0)
						SelectRow(GetCurRow() - 1);
					else if(GetCurRow() < 0)
						SelectRow((long)m_rows.size()-1);
					break;
				case VK_DOWN:
					if(GetCurRow() < (long)m_rows.size()-1)
						SelectRow(GetCurRow() + 1);
					break;
				case VK_LEFT:
					ScrollLineLeft();
					break;
				case VK_RIGHT:
					ScrollLineRight();
					break;
				case VK_INSERT:
					if(m_dwStyle & GS_EX_CONTEXTMENU)
						m_wrapper.SendMessage(WM_COMMAND,ID_GRID_ADDROW);
					break;
				case VK_DELETE:
					m_wrapper.DeleteItem();
					break;
				case VK_HOME:
					this->ScrollTop();
					if(m_rows.size()>0)	SelectRow(0);
					break;
				case VK_END:
					this->ScrollBottom();
					if(m_rows.size()>0)	SelectRow((long)m_rows.size()-1);
					break;
				case VK_PRIOR:
					this->ScrollPageUp();
					SelectTopRow();
					break;
				case VK_NEXT:
					this->ScrollPageDown();
					SelectTopRow();
					break;
			}

			return 0;
		}

		CGrid(CGridCtrl& wrapper) :	m_wrapper(wrapper) {
			//SetScrollExtendedStyle(0,SCRL_ERASEBACKGROUND);
			m_dwStyle =	GS_EX_LINENUMBERS |	GS_EX_VGRIDLINES | GS_EX_HGRIDLINES;
			m_nSelectedRow = -1;
			m_bEditing = false;
			m_bDrag = false;
		}
		
		virtual	~CGrid() {
			FreeAll();
		}

		void SelectTopRow()	{
			CPoint pt;
			GetScrollOffset(pt);
			long nRow =	pt.y / GetRowHeight();
			if(nRow<(long)m_rows.size())
				SelectRow(nRow);
		}

		virtual	BOOL PreTranslateMessage(MSG* pMsg)	{
			if(pMsg->message==WM_KEYDOWN &&	pMsg->wParam==VK_TAB)
				return IsDialogMessage(pMsg);
			else
				return FALSE;
		}
		
		void DrawCell(long nRow,long nColumn) {
			CWindowDC dcWindow(m_hWnd);
			
			CPoint ptScroll;
			GetScrollOffset(ptScroll);
			dcWindow.SetViewportOrg(-ptScroll.x,-ptScroll.y);

			CRect rc;
			rc.left	= 0;
			if(m_wrapper.ShowLineNumbers())	rc.left	+= COL_NUMWIDTH;

			for(long i=0;i<nColumn;i++)
				rc.left	+= m_columns[i]->m_nWidth;
			rc.right = rc.left + m_columns[nColumn]->m_nWidth;
			rc.top = nRow *	GetRowHeight();
			rc.bottom =	rc.top + GetRowHeight();

			//�`�F�b�N�{�b�N�X�̂��ݎc��΍�
			CRect rcClient;
			GetClientRect(&rcClient);

			if(rc.IntersectRect(&rc,&rcClient))
				DoPaint((HDC)dcWindow,rc);
		}

		void DoPaint(CDCHandle dc2,LPRECT lpRect=NULL)
		{
			if(!m_wrapper.m_bRedraw) return;
			CRect rcClient;
			GetClientRect(&rcClient);

			CPoint ptView;
			dc2.GetViewportOrg(&ptView);

			CRect rcPaint;
			if(!lpRect)	{
				rcPaint.left = rcClient.left - ptView.x;
				rcPaint.top	= rcClient.top - ptView.y;
				rcPaint.right =	rcPaint.left + rcClient.Width();
				rcPaint.bottom = rcPaint.top + rcClient.Height();
				lpRect = &rcPaint;
			}
			CMemDC dc(dc2,lpRect);
			HFONT hOldFont = dc.SelectFont(m_font_bold);
			const long nRows = m_wrapper.GetRowCount();

			{
				RECT rc	= rcClient;
				rc.right = GetClientWidth();
				rc.bottom =	GetClientHeight();
				dc.FillRect(&rc,COLOR_WINDOW);
			}

			CGridDC	mdc(dc);
			const long nRowHeight =	GetRowHeight();
			for(long i=0;i<nRows;i++)
			{
				RECT rc	= rcClient;
				rc.top += i*nRowHeight;
				rc.bottom =	rc.top + nRowHeight;

				// No point	painting outside visible area
				if(rc.top +	ptView.y + nRowHeight <	0 || rc.top	+ ptView.y > rcClient.Height())
					continue;

				long x = 0;
				if(m_wrapper.ShowLineNumbers())	{
					ATL::CString str;
					str.Format("%d",i+1);
					dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
					dc.SelectFont(m_font);

					CRect rcb(rc);
					rcb.right =	rc.left	+ COL_NUMWIDTH;
					rcb.bottom = rc.top	+ nRowHeight;
					mdc.DrawButton(rcb,IsSelectedRow(i));
					rcb.DeflateRect(4,1);
					rcb.bottom = rcb.top + m_nControlHeight;
					mdc.DrawText(rcb,str,RIGHT);
					
					x += COL_NUMWIDTH;
				}

				rc.left	= x;
				rc.right = GetClientWidth(true);
				if(IsSelectedRow(i))
					dc.FillRect(&rc,COLOR_HIGHLIGHT);

				COLORREF crColor = GetSysColor(IsSelectedRow(i) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);
				for(long col=0;col<m_wrapper.GetColumnCount();col++)
				{
					CColumn& column	= *m_columns[col];
					if(m_pListener && !IsSelectedRow(i)) {
						COLORREF crBack	= m_pListener->GetCellColor(m_wrapper.GetWindowLong(GWL_ID),i,m_columns[col]->m_strName);
						if(crBack!=(COLORREF)-1)
							dc.FillSolidRect(x,rc.top,m_columns[col]->m_nWidth,nRowHeight,crBack);
					}

					dc.SetTextColor(crColor);
					dc.SelectFont(m_font);

					CRect rcCell;
					GetCellRect(rcCell,i,col,true,&ptView);
					m_columns[col]->Draw((HDC)dc,rcCell,m_rows[i]->m_items[col]);

					x += m_columns[col]->m_nWidth;
				}
			}

			if(m_dwStyle & GS_EX_VGRIDLINES)
			{
				long x = -1;
				if(m_wrapper.ShowLineNumbers())	x += COL_NUMWIDTH; 
				RECT rc	= rcClient;
				rc.bottom =	GetClientHeight(true);
				for(long i=0;i<m_wrapper.GetColumnCount();i++) {
					rc.left	= x	+ m_columns[i]->m_nWidth;
					rc.right = rc.left + 1;
					dc.FillRect(&rc,COLOR_3DSHADOW);

					x += m_columns[i]->m_nWidth;
				}
			} else {
				CRect rc;
				rc.left	= GetClientWidth(true) - 1;
				rc.right = rc.left + 1;
				rc.top = 0;
				rc.bottom =	GetClientHeight(true);
				dc.FillRect(&rc,COLOR_3DSHADOW);
			}

			if(m_dwStyle & GS_EX_HGRIDLINES) {
				RECT rc	= rcClient;
				rc.left--;
				if(m_wrapper.ShowLineNumbers())	rc.left	+= COL_NUMWIDTH;
				rc.right = GetClientWidth(true);
				for(long i=0;i<m_wrapper.GetRowCount();i++)	{
					rc.top = (i+1) * nRowHeight	- 1;
					rc.bottom =	rc.top + 1;
					dc.FillRect(&rc,COLOR_3DSHADOW);
				}
			} else {
				CRect rc;
				rc.left	= 0;
				if(m_wrapper.ShowLineNumbers())	rc.left	+= COL_NUMWIDTH;
				rc.right = GetClientWidth(true);
				rc.top = (long)m_rows.size() * GetRowHeight() -1;
				rc.bottom =	rc.top + 1;
				dc.FillRect(&rc,COLOR_3DSHADOW);
			}

			if(m_bDrag)
			{
				DrawFocusRect(dc.m_hDC,m_DragRect);
			}
			
			dc.SelectFont(hOldFont);
		}
		
		void DoScroll(int nType, int nScrollCode, int& cxyOffset, int cxySizeAll, int cxySizePage, int cxySizeLine)	{
			baseClass::DoScroll(nType,nScrollCode,cxyOffset,cxySizeAll,cxySizePage,cxySizeLine);
			if(nType!=SB_VERT)
				m_wrapper.Invalidate();
		}

		long GetRowHeight()	const {
			ATLASSERT(m_nControlHeight>15);
			return m_nControlHeight	+ 3;
		}
		
		long GetClientWidth(bool bGridOnly=false) const	{
			CRect rc;
			GetClientRect(&rc);

			long nWidth	= 0;
			if(m_wrapper.ShowLineNumbers())	nWidth += COL_NUMWIDTH;	
			for(long i=0;i<m_columns.GetSize();i++)
				nWidth += m_columns[i]->m_nWidth;

			if(!bGridOnly && nWidth	< rc.Width())
				nWidth = rc.Width();
			if(nWidth<1)
				nWidth = 1;
			return nWidth;
		}
		
		long GetClientHeight(bool bGridOnly=false) const {
			CRect rc;
			GetClientRect(&rc);

			long nHeight = GetRowHeight() *	m_wrapper.GetRowCount();
			if(!bGridOnly && nHeight < rc.Height())
				nHeight	= rc.Height();
			if(nHeight<1)
				nHeight	= 1;
			return nHeight;
		}

		bool SelectRow(long	row) {
			ATLASSERT(IsWindow() &&	m_wrapper.IsWindow());
			if(m_pListener && !m_pListener->OnRowChanging(m_wrapper.GetWindowLong(GWL_ID),row))
				return false;

			if(!EndEdit(false))
				return false;

			if(row < m_wrapper.GetRowCount())
				SetCurRow(row);
			else
				SetCurRow(-1);

			if(GetCurRow() >= 0)
				m_wrapper.EnsureVisible(GetCurRow());

			Invalidate();

			if(m_pListener)
				m_pListener->OnRowChanged(m_wrapper.GetWindowLong(GWL_ID),row);

			return true;
		}

		void SizeControls(DWORD	dwSWPFlags=0)
		{
			dwSWPFlags |= SWP_NOZORDER;
			long count = m_columns.GetSize();

			for(long i=0;i<count;i++)
			{
				CRect rc;
				GetCellRect(rc,GetCurRow(),i,true);
				if(m_columns[i]->m_nType == EDIT_CHECK && (rc.Width() > rc.Height()))
				{//�ʒu�����킹�悤�Ɠw�ͥ��
					int nPad = ((rc.Width() - rc.Height()) / 2) + 2;
					rc.left += nPad;
					rc.right -= nPad;
				}
				m_columns[i]->SetWindowPos(rc,dwSWPFlags);
			}
		}

		void EditRow(long row=-1,long col=-1) {
			if(m_bEditing) return;
			if(col<0) col =	0;
			if(row<0) row =	GetCurRow();
			if(row<0) return;
			m_wrapper.EnsureVisible(row);//

			if(m_pListener)	m_pListener->OnRowActivate(m_wrapper.GetWindowLong(GWL_ID),row);
			if(m_dwStyle & GS_EX_READONLY) return;

			SizeControls(SWP_SHOWWINDOW);
			for(long i=0;i<m_wrapper.GetColumnCount();i++) 
			{
				CGridCtrl::CColumn*	pCol = m_columns[i];
				if(!pCol->GetReadOnly()) {
					pCol->SetValue(m_rows[row]->GetItem(i));
					if(col>=0 && i>=col) {
						pCol->SetFocus();
						col	= -1;
					}
				}
			}

			m_bEditing = true;
			if(m_pListener)
				m_pListener->OnEdit(m_wrapper.GetWindowLong(GWL_ID),row);
		}

		bool EndEdit(bool bAbort) {
			ATLASSERT(IsWindow());
			if(!m_bEditing)	return true;

			if(!bAbort && m_pListener && !m_pListener->OnValidate(m_wrapper.GetWindowLong(GWL_ID)))
				return false;

			if(!bAbort)	
			{
				GetEditChanges();
				Invalidate();
			}
			for(long i=0;i<m_wrapper.GetColumnCount();i++)
				m_columns[i]->SetWindowPos(rcDefault,SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOZORDER);
			m_bEditing = false;

			SetFocus();
			return true;
		}

		long GetPointRow(POINT pt) const {
			long row = pt.y	/ GetRowHeight();
			if(row<0 ||	row>=m_wrapper.GetRowCount())
				return -1;
			return row;
		}

		long GetPointColumn(POINT pt) const	{
			if(m_wrapper.ShowLineNumbers())	pt.x -=	COL_NUMWIDTH;
			if(pt.x<0) return -1;
			long x = 0;
			for(long i=0;i<m_wrapper.GetColumnCount();i++) {
				x += m_columns[i]->m_nWidth;
				if(pt.x	< x)
					return i;
			}
			return -1;
		}
		
		void GetCellRect(CRect&	rc,long	row,long col,bool bInner=false,LPPOINT lpPoint=NULL) const {
			CPoint pt;
			GetScrollOffset(pt);
			rc.top = row * GetRowHeight() -	pt.y;
			rc.bottom =	rc.top + GetRowHeight()	- 1;
			rc.left	= 0;
			if(m_wrapper.ShowLineNumbers())	rc.left	+= COL_NUMWIDTH;

			for(long i=0;i<col;i++)
				rc.left	+= m_columns[i]->m_nWidth;

			rc.left	-= pt.x;
			rc.right = rc.left + m_columns[col]->m_nWidth -	1;

			if(bInner) {
				rc.DeflateRect(2,1);
				rc.bottom =	rc.top + m_nControlHeight;
			}
			if(lpPoint)	{
				rc.left	-= lpPoint->x;
				rc.right -=	lpPoint->x;
				rc.top -= lpPoint->y;
				rc.bottom -= lpPoint->y;
			}
		}
		
		void DoDrag(long mx,bool bDoDrag) {
			CDCHandle dc = GetDC();
			RECT rc;
			GetClientRect(&rc);

			int	old	= dc.SetROP2(R2_NOT);
			if(bDoDrag)	{
				dc.MoveTo(m_old_x,0);
				dc.LineTo(m_old_x,rc.bottom);
			}
			m_old_x	= mx;

			dc.MoveTo(mx,0);
			dc.LineTo(mx,rc.bottom);
			dc.SetROP2(R2_COPYPEN);
			ReleaseDC(dc);
		}

		void DeleteAllItems() {
			EndEdit(true);
			long count = (long)m_rows.size();
			while(count--) delete m_rows[count];
			m_rows.clear();
			this->SetScrollOffset(0,0,FALSE);
			SelectRow(-1);
		}

		void DeleteAllColumns()	{
			long count = m_columns.GetSize();
			while(count--) delete m_columns[count];
			m_columns.RemoveAll();
		}

		bool GetEditChanges() {
			ATLASSERT(IsWindow());
			if(!m_bEditing)
				return false;

			bool bChanged =	false;
			for(long i=0;i<m_wrapper.GetColumnCount();i++)
			{
				CGridCtrl::CColumn*	pCol = m_columns[i];
				if(!pCol->GetReadOnly())
				{
					_variant_t vtValue = pCol->GetValue();
					_variant_t vt2 = m_columns[i]->m_vtValue;
					if(IsNull(vtValue))	SetNull(vtValue);
					if(IsNull(vt2))	SetNull(vt2);
					if(vtValue != vt2)
					{
						long lRowCount = m_rows.size();
						for(int nRow = 0;nRow < lRowCount;nRow++)
						{
							if(IsSelectedRow(nRow))
							{
								m_wrapper.SetItem(nRow,i,vtValue);
								m_rows[nRow]->m_bModified = true;
							}
						}
						bChanged = true;
					}
				}
			}
			return bChanged;
		}

		void FreeAll() {
			long count = (long)m_rows.size();
			while(count--) delete m_rows[count];
			m_rows.clear();
			
			count =	m_columns.GetSize();
			while(count--) delete m_columns[count];
			m_columns.RemoveAll();
		}

		typedef std::vector<CRow*>::iterator it_row;

		bool IsSelectedRow(long nRow) const
		{
			return m_rows[nRow]->m_bSelect;
			//return nRow == m_nSelectedRow;
		}
		long GetCurRow() const
		{
			return m_nSelectedRow;
		}
		void SetCurRow(long nRow)
		{
			SetSelect(-1,false);
			m_nSelectedRow = nRow;
			if(nRow != -1)
				SetSelect(nRow,true);
			return;
		}
		void SetSelect(long nRow,bool bSelect)
		{
			if(nRow == -1)
			{
				for(it_row it = m_rows.begin();it != m_rows.end();it++)
					(*it)->m_bSelect = bSelect;
			}else
				m_rows[nRow]->m_bSelect = bSelect;
		}
		int GetSelectedCount()
		{
			int n = 0;
			for(it_row it = m_rows.begin();it != m_rows.end();it++)
			{
				if((*it)->m_bSelect)
					n++;
			}
			return n;
			//return (m_nSelectedRow >= 0) ? 1:0;
		}
	protected:
		CGridCtrl&	m_wrapper;
		long		m_nSelectedRow;
		bool		m_bEditing;
		long		m_nControlHeight;
		long		m_old_x;
		// Fra CGridCtrl
		CSimpleArray<CColumn*>	m_columns;

		//STL��
		//CAtlArray<CRow*>		m_rows;
		std::vector<CRow*> m_rows;

		CFont						m_font;
		CFont						m_font_bold;
		CListener*					m_pListener;
		DWORD						m_dwStyle;

		CPoint m_pt;
		CRect m_DragRect;
		bool m_bDrag;
	};

	// Internal	enums
protected:
	enum COM1MAND {
		ID_GRID_ADDROW = 0x9F01,
		ID_GRID_EDITROW,
		ID_GRID_DELETEROW
	};

protected:
	CGrid	m_grid;
	long	m_nDragCol;
	long	m_nPushCol;//sort
	long	m_nDragMin;
	bool	m_bRedraw;
};

#endif // __BHWTLGRID_H__
