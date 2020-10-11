// mobilePhoneDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mobilePhone.h"
#include "mobilePhoneDlg.h"
#include "PhoneCore.h"
#include "staticManager.h"
#include "SIPLoginDlg.h"
#include "DIALDLG.h"
#include "VideoDlg.h"
#include "mplayer.h"
#include "utils.h"
#include  "gpsdlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmobilePhoneDlg 对话框

CmobilePhoneDlg::CmobilePhoneDlg(CWnd* pParent /*=NULL*/)
: CDialog(CmobilePhoneDlg::IDD, pParent)
, m_strStartupPath(_T(""))
, m_sSysState(_T(""))
{
	m_pCallDlg  = new CVideoDlg(this) ;
	 m_pDialDlg = new CDIALDLG(this);
	 m_pGpsDlg = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmobilePhoneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);;
	DDX_Control(pDX, IDC_TREE1, m_pFriends);
	//DDX_Text(pDX, IDC_SYSSTATE, m_sSysState);
}

BEGIN_MESSAGE_MAP(CmobilePhoneDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()

	ON_WM_TIMER()
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_32785, &CmobilePhoneDlg::OnLongin)
	ON_COMMAND(IDD_DIAlPANEL, &CmobilePhoneDlg::OnDialpanel)
	ON_COMMAND(ID_32787, &CmobilePhoneDlg::OnExit)
	ON_COMMAND(ID_32801, &CmobilePhoneDlg::OnVideoTest)
	ON_COMMAND(ID_32804, &CmobilePhoneDlg::OnMediaPlay)
	ON_COMMAND(ID_32810, &CmobilePhoneDlg::OnOpenGpsDlg)
END_MESSAGE_MAP()


// CmobilePhoneDlg 消息处理程序

BOOL CmobilePhoneDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    
	GetStartupPath();
	this->initFriendTree();
	OnLongin();

	CUtils::fullScreenAndMenu(m_hWnd,IDR_MENU2,false);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CmobilePhoneDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_MOBILEPHONE_DIALOG_WIDE) : 
		MAKEINTRESOURCE(IDD_MOBILEPHONE_DIALOG));
	}
}
#endif




void CmobilePhoneDlg::OnDestroy()
{
	CDialog::OnDestroy();
	this->KillTimer(1);
	Sleep(300);
	if(	m_pCallDlg !=NULL)
		delete m_pCallDlg;
	if(m_pDialDlg != NULL)
		delete m_pDialDlg;
	if(m_pPhoneCore!=NULL)
	    delete m_pPhoneCore;
}

void CmobilePhoneDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(1==nIDEvent)
	{
		this->m_pPhoneCore->linphone_core_iterate();
	}

	CDialog::OnTimer(nIDEvent);
}



void CmobilePhoneDlg::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDialog::OnSettingChange(uFlags, lpszSection);

	// TODO: 在此处添加消息处理程序代码
}



void CmobilePhoneDlg::OnLongin()
{
	CSIPLoginDlg dlg;
	dlg.m_strStartupPath = this->m_strStartupPath;
	
	if(dlg.DoModal() == IDOK)
	{
	char* tp = CUtils::WideCharToMultiByte(m_strStartupPath);
	char config_file[1024];
	snprintf(config_file,sizeof(config_file),"%s\\%s",tp,"phone.ini");
	m_pPhoneCore = new CPhoneCore();
	this->m_pStateManager = new CStaticManager(this);

	m_pPhoneCore->linphone_core_init(m_pStateManager,config_file,NULL);
	m_pPhoneCore->lc->linphone_core_enable_video_preview(FALSE);
	SetTimer(1,300,NULL);
	
	this->m_pPhoneCore->user_register();
    delete tp;
	}
}

void CmobilePhoneDlg::OnDialpanel()
{
	m_pDialDlg->isDial = true;
	m_pDialDlg->DoModal();


}

void CmobilePhoneDlg::OnExit()
{
	this->OnOK();
}

void CmobilePhoneDlg::OnVideoTest()
{
	if(m_pCallDlg == NULL)
	{
		m_pCallDlg = new CVideoDlg(this);
	}
	m_pCallDlg->videoPreview();
	m_pCallDlg->DoModal();
}

void CmobilePhoneDlg::initFriendTree(void)
{
	CImageList *m_imglist = new CImageList;
    m_imglist->Create(IDB_BITMAP2, 32, 0, RGB(0,128,128));

	m_pFriends.SetImageList(m_imglist, TVSIL_NORMAL);

	this->m_pFriends.InsertItem(_T("所有好友"));
	HTREEITEM online = this->m_pFriends.InsertItem(_T("在线"),this->m_pFriends.GetRootItem());
	m_pFriends.InsertItem(_T("a1"),1,2,online);
	m_pFriends.InsertItem(_T("a2"),1,2,online);
	m_pFriends.InsertItem(_T("a3"),1,2,online);
	this->m_pFriends.InsertItem(_T("离线"),this->m_pFriends.GetRootItem());
}


void CmobilePhoneDlg::OnMediaPlay()
{
	CMPlayer play;
	play.DoModal();
}


CString CmobilePhoneDlg::GetStartupPath(void)
{
   if(m_strStartupPath == _T(""))
    {
        TCHAR cPath[MAX_PATH];   
        CString filePath;
        GetModuleFileName( NULL, cPath, MAX_PATH );
        filePath = cPath;
        filePath = filePath.Left(filePath.ReverseFind('\\'));
        m_strStartupPath = filePath;
    }
    return m_strStartupPath;
}

void CmobilePhoneDlg::UpdateSysState(CString state)
{
	m_sSysState = _T("fdsd");
	UpdateData(false);
}

void CmobilePhoneDlg::OnOpenGpsDlg()
{
	if(m_pGpsDlg == NULL)
	{
		m_pGpsDlg = new CGPSDlg(this);
	}
	m_pGpsDlg->DoModal();

}
