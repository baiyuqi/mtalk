#pragma once

class CUtils
{
public:
	CUtils(void);
	~CUtils(void);
	static char* WideCharToMultiByte(CString source);
	// 设置全屏和加载菜单
	static bool fullScreenAndMenu(HWND hWnd, DWORD menuId,bool shuru = true);
	static CString MultiByteToWideChar(const char* source);
};
