/** 
@file
@brief CrashReportͷ�ļ�
@version 2011-7-10 bobdeng
*/
#pragma once

#include <tchar.h>
#include <assert.h>

namespace CrashReport
{
	//��ʼ��Bugreport
	//lpszProductId		- ��ƷID�����ڲ�Ʒ��ʶ������yy������IDΪYY
	//lpszProductVer:	- ��Ʒ�İ汾�ţ���4.12.0.0
	//lpszProductName	- ��Ʒ���֣���ʾ�ڵ����ı�����������
	BOOL InitBugReport(LPCTSTR lpszProductId, LPCTSTR lpszProductVer, LPCTSTR lpszProductName);

    // ��ȡͨ��InitBugReport��SetProductNameAndId���õ�lpszProductId
    LPCSTR GetProductId();

    // ��ȡͨ��InitBugReport��SetProductNameAndId���õ�lpszProductVer
    LPCSTR GetProductVer();

    // ��ȡͨ��InitBugReport��SetProductNameAndId���õ�lpszProductName
    LPCSTR GetProductName();

	// ����CrashReport.dll��·��
	void SetDllPath(LPCTSTR lpszDllPath);
	
	//�����������
	BOOL InitDeadlockDetector();
	
	//�����쳣�˳�ͳ��
	BOOL InitExcepStat();

	//�����û��ı�ʶ������YY���룬����ֶ��ں�̨�ϱ�ҳ����Բ鿴��������ϵ�û�
	void SetUserId(const wchar_t *lpszUserId);

	//���ð汾��Ϣ
	//lpszProductVer	- ͬInitBugReport����lpszProductVer
	void SetProductVer(LPCTSTR lpszProductVer);

    //���ò�Ʒ���ƺ�id ��lpszProductId����ʶ���Ʒ��
    void SetProductNameAndId(LPCTSTR lpszProductName, LPCTSTR lpszProductId);

	//���ð汾��ϸ������Ϣ
	//lpszVerDetail		- ���ڱ�ʶС�汾�����翪���棬�磺YY dev (2011.09.28) r123908
	void SetProductVerDetail(LPCTSTR lpszVerDetail);

	//���ñ�����������·��
	//�������Ҫ�������Ͳ��õ�������������������渴ѡ����Զ�����
	void SetRestartExePath(LPCTSTR lpszPath);

	//����Զ����ϱ��ļ�
	void AddCustomReportFile(LPCTSTR lpszPath);

	//�����Զ�������,����Ƶ��ID���������ϱ���Ϣ�У���̨���Բ鿴�������¼�������Ϣ��
	void SetCustomProperty(const wchar_t* lpszKey, const wchar_t* lpszValue);
	BOOL GetCustomProperty(const wchar_t* lpszKey, wchar_t* lpszValue, int cch);

	//�Ƿ��������ʾ�ϱ����棨Ĭ������ʾ��
	void SetShowReportUI(BOOL bShow);

	//֧�ֶ�̬�����İ汾
	void AddDynaReleaseVer(LPCTSTR lpszDynaVer);
	void RemoveDynaReleaseVer(LPCTSTR lpszDynaVer);

	BOOL SetCustomData(int index, const void* lpData, int cchData);

    void SetCatchPureCall(BOOL setting);
	
	void DestroyBugReport();
	
	//����ʵ��**************************************************************************************************
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

	//��ʼ��Bugreport
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

	
	// ����CrashReport.dll��·��
	static void SetDllPath(LPCTSTR lpszDllPath)
	{
		wcsncpy_s(g_szDllPath,MAX_PATH,lpszDllPath,_tcslen(lpszDllPath));
	}
	
	//��ʼ���������
	static BOOL InitDeadlockDetector()
	{
		static void *pProc = GetProc("InitDeadlockDetector");
		if (pProc)
			return ((BOOL (__cdecl *)())pProc)();
		return FALSE;
	}
	
	//��ʼ���������
	static BOOL InitExcepStat()
	{
		static void *pProc = GetProc("InitExcepStat");
		if (pProc)
			return ((BOOL (__cdecl *)())pProc)();
		return FALSE;
	}
		

	//�����û���ʶ��YY���룩
	static void SetUserId(const wchar_t *lpszUserId)
	{
		static void *pProc = GetProc("SetUserId");
		if (pProc)
			((void (__cdecl *)(const wchar_t *))pProc)(lpszUserId);
	}

	//���ð汾��Ϣ
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

	//���ð汾��ϸ������Ϣ
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

	//����Զ����ϱ��ļ�
	static void AddCustomReportFile(LPCTSTR lpszPath)
	{
		static void *pProc = GetProc("AddCustomReportFile");
		if (pProc)
			((void (__cdecl *)(LPCTSTR))pProc)(lpszPath);
	}

	//�����Զ�������,����Ƶ��ID���������ϱ���Ϣ�У�
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

	//�Ƿ��������ʾ�ϱ�����
	static void SetShowReportUI(BOOL bShow)
	{
		static void *pProc = GetProc("SetShowReportUI");
		if (pProc)
			((void (__cdecl *)(BOOL))pProc)(bShow);
	}

	//֧�ֶ�̬�����İ汾
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
