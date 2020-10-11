#include "StdAfx.h"
#include "Utils.h"

CUtils::CUtils(void)
{
}

CUtils::~CUtils(void)
{
}

char* CUtils::WideCharToMultiByte(CString source)
{
	DWORD dwNum = ::WideCharToMultiByte(CP_OEMCP,NULL,source,-1,NULL,0,NULL,FALSE);
	char *psText =  new char[dwNum];
	if(!psText)
	{
		delete []psText;
	}
	::WideCharToMultiByte(CP_OEMCP,NULL,source,-1,psText,dwNum,NULL,FALSE);
	return psText;
}

// 设置全屏和加载菜单
bool CUtils::fullScreenAndMenu(HWND hWnd, DWORD menuId,bool shuru)
{
	RECT rc;
	GetWindowRect(hWnd,&rc);
	rc.top -= DIALOGTOP;
	MoveWindow(hWnd,rc.left,rc.top,rc.right,rc.bottom,FALSE);
	SHFullScreen(hWnd,SHFS_HIDETASKBAR);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hWnd;
	if(shuru)
		mbi.dwFlags |= SHCMBF_HMENU;
	else
		mbi.dwFlags |= SHCMBF_HMENU|SHCMBF_HIDESIPBUTTON;
	mbi.nToolBarId = menuId;
	mbi.hInstRes   = AfxGetInstanceHandle();
	SHCreateMenuBar(&mbi);
	return true;
}

CString CUtils::MultiByteToWideChar(const char* source)
{
	DWORD dwNum = ::MultiByteToWideChar (CP_ACP, 0, source, -1, NULL, 0);
	wchar_t *pwText;
	pwText = new wchar_t[dwNum];
	if(!pwText)
	{
		delete []pwText;
	}

	::MultiByteToWideChar (CP_ACP, 0, source, -1, pwText, dwNum);

	return pwText;
}
