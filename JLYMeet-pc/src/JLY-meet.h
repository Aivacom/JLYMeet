#pragma once

#ifndef __AFXWIN_H__
	#error "Include "stdafx.h" before including this header file for creating PCH file"
#endif

#include "resource.h"
#include "NetHttp\Postoffice.h"

class CJLYVideoDemoApp : public CWinApp
{
public:
	CJLYVideoDemoApp();

public:
	virtual BOOL InitInstance();

	CPostoffice& getPostOffice();

	CString GetModuleDir();

	DECLARE_MESSAGE_MAP()

private:
	void initCrashReport();

private:
	CPostoffice mPostOffice;
};

extern CJLYVideoDemoApp theApp;