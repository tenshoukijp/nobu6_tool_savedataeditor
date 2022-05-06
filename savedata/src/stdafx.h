// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0501
//#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0600
#define _RICHEDIT_VER	0x0200

#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlframe.h>

#include "atlgdix.h"
#include <comutil.h>
#include <comdef.h>

#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlscrl.h>
#include <atlcoll.h>
#include <atlctl.h>

#include <atlcom.h>
#include <atlhost.h>
#include <atlcrack.h>
#define _ATL_TMP_NO_CSTRING
#include <atlmisc.h>

#include <functional>
#include <algorithm>
#include <vector>
