// stdafx.h: include files for standard system include files,
// Or frequently used but not changed often
// Project-specific include files

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include "targetver.h"

// Some CString constructors will be explicit
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      

// Turn off MFC's hiding of some common but often overlooked warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>        // MFC core components and standard components
#include <afxext.h>         // MFC Expand


#include <afxdisp.h>        // MFC Automation

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC Internet Explorer 4 support for public controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC Windows support for public controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>    // MFC support for ribbon and control bar


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
