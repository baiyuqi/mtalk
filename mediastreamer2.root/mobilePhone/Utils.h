#pragma once

class CUtils
{
public:
	CUtils(void);
	~CUtils(void);
	static char* WideCharToMultiByte(CString source);
	// ����ȫ���ͼ��ز˵�
	static bool fullScreenAndMenu(HWND hWnd, DWORD menuId,bool shuru = true);
	static CString MultiByteToWideChar(const char* source);
};
