// mobilePhone.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 
#include "mobilePhone_i.h"

// CmobilePhoneApp:
// �йش����ʵ�֣������ mobilePhone.cpp
//

class CmobilePhoneApp : public CWinApp
{
public:
	CmobilePhoneApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	BOOL ExitInstance(void);
};

extern CmobilePhoneApp theApp;
