// VideoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mobilePhone.h"
#include "VideoDlg.h"
#include "mobilePhoneDlg.h"
#include "linphoneCore.h"
#include "PhoneCore.h"
#include "utils.h"
// CVideoDlg 对话框

IMPLEMENT_DYNAMIC(CVideoDlg, CDialog)

CVideoDlg::CVideoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoDlg::IDD, pParent)
{
	m_pMainDlg = (CmobilePhoneDlg*)pParent;
}

CVideoDlg::~CVideoDlg()
{
}

void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CVideoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	RECT rc;
	GetWindowRect(&rc);
	rc.top -= DIALOGTOP;
	MoveWindow(rc.left,rc.top,rc.right,rc.bottom,FALSE);
	SHFullScreen(this->m_hWnd,SHFS_HIDETASKBAR);
    
	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = this->m_hWnd;
	mbi.dwFlags |= SHCMBF_HMENU|SHCMBF_HIDESIPBUTTON;
	mbi.nToolBarId = IDR_MENU4;
	mbi.hInstRes   = AfxGetInstanceHandle();

	SHCreateMenuBar(&mbi);
	
    if(this->m_pParentWnd!=NULL)
	{
	 m_pMainDlg->m_pPhoneCore->lc->m_VHwnd = this->GetDlgItem(IDC_VIDEOTEL)->GetSafeHwnd();
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CVideoDlg, CDialog)

	ON_COMMAND(ID_32802, &CVideoDlg::OnReturn)
	ON_BN_CLICKED(IDC_HANGUP, &CVideoDlg::OnBnClickedHangup)
END_MESSAGE_MAP()


// CVideoDlg 消息处理程序

void CVideoDlg::OnBnClickedComplete()
{
	if(this->m_pParentWnd!=NULL)
	{
	 m_pMainDlg->m_pPhoneCore->lc->linphone_core_enable_video_preview(FALSE);
	}
	CDialog::OnCancel();
}

void CVideoDlg::OnReturn()
{
	if(this->m_pParentWnd!=NULL)
	{
	 m_pMainDlg->m_pPhoneCore->lc->linphone_core_enable_video_preview(FALSE);
	}
	CDialog::OnCancel();
}

BOOL CVideoDlg::call(CString name)
{
	//this->ShowWindow(SW_SHOW);
	char* sip = CUtils::WideCharToMultiByte(name);
	char* url = new char[128];
	sprintf(url,"sip:%s@nist.gov",sip);
	delete []sip;
	m_pMainDlg->m_pPhoneCore->lc->linphone_core_invite(url);
	return 0;
}

bool CVideoDlg::videoPreview(void)
{
	if(this->m_pParentWnd!=NULL)
	{
	 m_pMainDlg->m_pPhoneCore->lc->linphone_core_enable_video_preview(TRUE);
	}
	return false;
}

void CVideoDlg::OnBnClickedHangup()
{
	m_pMainDlg->m_pPhoneCore->lc->linphone_core_terminate_call(NULL);
	this->OnOK();
}

void CVideoDlg::OnCallEnd(void)
{
	this->OnOK();
}
