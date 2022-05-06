// SaveDataEditor.cpp : main source file for SaveDataEditor.exe
//

#include "stdafx.h"
#include <locale>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "gridctrl.h"
#include "SaveDataEditorView.h"
#include "aboutdlg.h"
#include "MainFrm.h"

CAppModule _Module;

LPTSTR CSaveDataEditorView::mlpstrCmdLine = NULL;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	std::locale::global(std::locale("japanese"));

	// コマンドライン引数の保存
	CSaveDataEditorView::mlpstrCmdLine = lpstrCmdLine;

	HRESULT hRes;
//	hRes = ::CoInitialize(NULL);

    // If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
//	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

//	AtlAxWinInit();
	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
//	::CoUninitialize();

	return nRet;
}
