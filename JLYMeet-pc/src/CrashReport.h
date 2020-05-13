/** 
@file
@brief CrashReport头文件
@version 2011-7-10 bobdeng
*/
#pragma once

#include <tchar.h>
#include <assert.h>

namespace CrashReport
{
	//初始化Bugreport
	//lpszProductId		- 产品ID，用于产品标识，比如yy语音的ID为YY
	//lpszProductVer:	- 产品的版本号，如4.12.0.0
	//lpszProductName	- 产品名字，显示在弹出的崩溃界面里面
	BOOL InitBugReport(LPCTSTR lpszProductId, LPCTSTR lpszProductVer, LPCTSTR lpszProductName);

    // 获取通过InitBugReport或SetProductNameAndId设置的lpszProductId
    LPCSTR GetProductId();

    // 获取通过InitBugReport或SetProductNameAndId设置的lpszProductVer
    LPCSTR GetProductVer();

    // 获取通过InitBugReport或SetProductNameAndId设置的lpszProductName
    LPCSTR GetProductName();

	// 设置CrashReport.dll的路径
	void SetDllPath(LPCTSTR lpszDllPath);
	
	//启动死锁检测
	BOOL InitDeadlockDetector();
	
	//启动异常退出统计
	BOOL InitExcepStat();

	//设置用户的标识，比如YY号码，这个字段在后台上报页面可以查看，方便联系用户
	void SetUserId(const wchar_t *lpszUserId);

	//设置版本信息
	//lpszProductVer	- 同InitBugReport参数lpszProductVer
	void SetProductVer(LPCTSTR lpszProductVer);

    //设置产品名称和id （lpszProductId用于识别产品）
    void SetProductNameAndId(LPCTSTR lpszProductName, LPCTSTR lpszProductId);

	//设置版本详细描叙信息
	//lpszVerDetail		- 用于标识小版本，比如开发版，如：YY dev (2011.09.28) r123908
	void SetProductVerDetail(LPCTSTR lpszVerDetail);

	//设置崩溃重启程序路径
	//如果不需要重启，就不用调用这个函数，崩溃界面复选框会自动隐藏
	void SetRestartExePath(LPCTSTR lpszPath);

	//添加自定义上报文件
	void AddCustomReportFile(LPCTSTR lpszPath);

	//设置自定义属性,例如频道ID（包含在上报信息中，后台可以查看，方便记录更多的信息）
	void SetCustomProperty(const wchar_t* lpszKey, const wchar_t* lpszValue);
	BOOL GetCustomProperty(const wchar_t* lpszKey, wchar_t* lpszValue, int cch);

	//是否崩溃后显示上报界面（默认是显示）
	void SetShowReportUI(BOOL bShow);

	//支持动态发布的版本
	void AddDynaReleaseVer(LPCTSTR lpszDynaVer);
	void RemoveDynaReleaseVer(LPCTSTR lpszDynaVer);

	BOOL SetCustomData(int index, const void* lpData, int cchData);

    void SetCatchPureCall(BOOL setting);
	
	void DestroyBugReport();
	
	//函数实现**************************************************************************************************
	__declspec(selectany) HMODULE g_hModule = NULL;
	__declspec(selectany) TCHAR g_szDllPath[MAX_PATH] = {0};
	
	static void * GetProc(LPCSTR lpProcName)
	{
		if (!g_hModule)
		{	
			if(_tcslen(g_szDllPath)==0)
			{
				TCHAR szDllPath[_MAX_PATH] = {0};
				if (GetModuleFileName(NULL, szDllPath, _MAX_PATH))
				{
					TCHAR *p = _tcsrchr(szDllPath, _T('\\'));
					if (p)
						*p = NULL;
					_tcsncat_s(szDllPath, _countof(szDllPath), _T("\\crashreport.dll"), _TRUNCATE);
				}				
				g_hModule = ::LoadLibrary(szDllPath);
			}		
			else
			{
				TCHAR szDllPath[_MAX_PATH+100] = {0};		
				const TCHAR *p1 = _tcsstr(g_szDllPath,_T("crashreport.dll"));	
				if(!p1)
				{	
			    	_sntprintf_s(szDllPath, _countof(szDllPath), _TRUNCATE, _T("%s%s"),g_szDllPath, _T("\\crashreport.dll"));
			    	 g_hModule = ::LoadLibrary(szDllPath);		
			    }
			    else
			    {
				    g_hModule = ::LoadLibrary(g_szDllPath);					
				}
			}			
		}
		
		void *pProc = (void *)::GetProcAddress(g_hModule, lpProcName);
		return pProc;
	}

	//初始化Bugreport
	static BOOL InitBugReport(LPCTSTR lpszProductId, LPCTSTR lpszProductVer, LPCTSTR lpszProductName)
	{
		static void *pProc = GetProc("InitBugReport");
		if (pProc)
			return ((BOOL (__cdecl *)(LPCTSTR, LPCTSTR, LPCTSTR))pProc)(lpszProductId, lpszProductVer, lpszProductName);
		return FALSE;
	}

    static LPCSTR GetProductId()
    {
        static void *pProc = GetProc("GetProductId");
        if (pProc)
            return ((LPCSTR (__cdecl *)())pProc)();
        return NULL;
    }

    static LPCSTR GetProductVer()
    {
        static void *pProc = GetProc("GetProductVer");
        if (pProc)
            return ((LPCSTR (__cdecl *)())pProc)();
        return NULL;

    }

    static LPCSTR GetProductName()
    {
        static void *pProc = GetProc("GetProductName");
        if (pProc)
            return ((LPCSTR (__cdecl *)())pProc)();
        return NULL;
    }

	
	// 设置CrashReport.dll的路径
	static void SetDllPath(LPCTSTR lpszDllPath)
	{
		wcsncpy_s(g_szDllPath,MAX_PATH,lpszDllPath,_tcslen(lpszDllPath));
	}
	
	//初始化死锁检测
	static BOOL InitDeadlockDetector()
	{
		static void *pProc = GetProc("InitDeadlockDetector");
		if (pProc)
			return ((BOOL (__cdecl *)())pProc)();
		return FALSE;
	}
	
	//初始化死锁检测
	static BOOL InitExcepStat()
	{
		static void *pProc = GetProc("InitExcepStat");
		if (pProc)
			return ((BOOL (__cdecl *)())pProc)();
		return FALSE;
	}
		

	//设置用户标识（YY号码）
	static void SetUserId(const wchar_t *lpszUserId)
	{
		static void *pProc = GetProc("SetUserId");
		if (pProc)
			((void (__cdecl *)(const wchar_t *))pProc)(lpszUserId);
	}

	//设置版本信息
	static void SetProductVer(LPCTSTR lpszProductVer)
	{
		static void *pProc = GetProc("SetProductVer");
		if (pProc)
			((void (__cdecl *)(LPCTSTR))pProc)(lpszProductVer);
	}

    static void SetProductNameAndId(LPCTSTR lpszProductName, LPCTSTR lpszProductId)
    {
        static void *pProc = GetProc("SetProductNameAndId");
        if (pProc)
            ((void (__cdecl *)(LPCTSTR, LPCTSTR))pProc)(lpszProductName, lpszProductId);
    }

	static void SetRestartExePath(LPCTSTR lpszPath)
	{
		static void *pProc = GetProc("SetRestartExePath");
		if (pProc)
			((void (__cdecl *)(LPCTSTR))pProc)(lpszPath);
	}

	//设置版本详细描叙信息
	static void SetProductVerDetail(LPCTSTR lpszVerDetail)
	{
		static void *pProc = GetProc("SetProductVerDetail");
		if (pProc)
			((void (__cdecl *)(LPCTSTR))pProc)(lpszVerDetail);
	}

	static BOOL SetCustomData(int index, const void* lpData, int cchData)
	{
		static void *pProc = GetProc("SetCustomData");
		if (pProc)
			return ((BOOL (__cdecl *)(int, const void*, int))pProc)(index, lpData, cchData);
		return FALSE;
	}

	//添加自定义上报文件
	static void AddCustomReportFile(LPCTSTR lpszPath)
	{
		static void *pProc = GetProc("AddCustomReportFile");
		if (pProc)
			((void (__cdecl *)(LPCTSTR))pProc)(lpszPath);
	}

	//设置自定义属性,例如频道ID（包含在上报信息中）
	static void SetCustomProperty(const wchar_t* lpszKey, const wchar_t* lpszValue)
	{
		static void *pProc = GetProc("SetCustomProperty");
		if (pProc)
			((void (__cdecl *)(const wchar_t*, const wchar_t*))pProc)(lpszKey, lpszValue);
	}

	static BOOL GetCustomProperty(const wchar_t* lpszKey, wchar_t* lpszValue, int cch)
	{
		static void *pProc = GetProc("GetCustomProperty");
		if (pProc)
			return ((BOOL (__cdecl *)(const wchar_t*, wchar_t*, int))pProc)(lpszKey, lpszValue, cch);
		return FALSE;
	}

	//是否崩溃后显示上报界面
	static void SetShowReportUI(BOOL bShow)
	{
		static void *pProc = GetProc("SetShowReportUI");
		if (pProc)
			((void (__cdecl *)(BOOL))pProc)(bShow);
	}

	//支持动态发布的版本
	static void AddDynaReleaseVer(LPCTSTR lpszDynaVer)
	{
		static void *pProc = GetProc("AddDynaReleaseVer");
		if (pProc)
			((void (__cdecl *)(LPCTSTR))pProc)(lpszDynaVer);
	}
	
	static void RemoveDynaReleaseVer(LPCTSTR lpszDynaVer)
	{
		static void *pProc = GetProc("RemoveDynaReleaseVer");
		if (pProc)
			((void (__cdecl *)(LPCTSTR))pProc)(lpszDynaVer);
	}

    static void SetCatchPureCall(BOOL setting)
    {
        static void *pProc = GetProc("SetCatchPureCall");
        if (pProc)
            ((void (__cdecl *)(BOOL))pProc)(setting);
    }
	
	static void DestroyBugReport()
	{
	  	static void *pProc = GetProc("DestroyBugReport");
		if (pProc)
			((void (__cdecl *)(void))pProc)();  
	}
}
