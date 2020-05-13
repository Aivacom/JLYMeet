//==========================================
// Matt Pietrek
// Microsoft Systems Journal, April 1997
// FILE: MSJEXHND.CPP
//==========================================  
#include "stdafx.h"

#include "msjexhnd.h"
#include <stdio.h>

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

//============================== Global Variables =============================

//
// Declare the static variables of the MSJExceptionHandler class
//
LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
//============================== Class Methods =============================

long __stdcall MyUnhandledFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
	long ret = EXCEPTION_EXECUTE_HANDLER;

	char szFileName[64];
	SYSTEMTIME st;
	::GetLocalTime(&st);
	sprintf_s(szFileName, "JLYMeet_%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, GetCurrentProcessId(), GetCurrentThreadId());

	HANDLE hFile = ::CreateFileA(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = lpExceptionInfo;
		ExInfo.ClientPointers = false;

		// write the dump
		BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		::CloseHandle(hFile);
	}
	return ret;
}


//=============
// Constructor 
//=============
MSJExceptionHandler::MSJExceptionHandler( )
{
	m_previousFilter = SetUnhandledExceptionFilter(&MyUnhandledFilter);
}

//============
// Destructor 
//============
MSJExceptionHandler::~MSJExceptionHandler( )
{
    SetUnhandledExceptionFilter( m_previousFilter );
}

