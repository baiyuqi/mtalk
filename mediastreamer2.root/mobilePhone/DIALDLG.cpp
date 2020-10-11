// DIALDLG.cpp : 实现文件
//

#include "stdafx.h"
#include "mobilePhone.h"
#include "DIALDLG.h"
#include "PhoneCore.h"
#include "mobilephoneDlg.h"
#include "videodlg.h"
// CDIALDLG 对话框

IMPLEMENT_DYNAMIC(CDIALDLG, CDialog)

CDIALDLG::CDIALDLG(CWnd* pParent /*=NULL*/)
	: CDialog(CDIALDLG::IDD, pParent)
	, m_sPhoneNumber(_T("test2"))
{
   if(pParent==NULL)
   {
	   ::OutputDebugString(_T("CDIALDLG 父窗口不能为空，请检查代码！"));
   }
   else
   {
	   this->m_pMianDlg = (CmobilePhoneDlg*)pParent;
   }
   isDial=true;;
}

CDIALDLG::~CDIALDLG()
{
}

void CDIALDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PHONENUMBER, m_sPhoneNumber);
}


BEGIN_MESSAGE_MAP(CDIALDLG, CDialog)
	ON_BN_CLICKED(IDC_DIAL1, &CDIALDLG::OnBnClickedDial1)
	ON_COMMAND(ID_32800, &CDIALDLG::On32800)
	ON_BN_CLICKED(IDC_BTNVIDEO, &CDIALDLG::OnBnClickedBtnvideo)
	ON_BN_CLICKED(IDC_BTNDEL, &CDIALDLG::OnBnClickedBtndel)
	ON_BN_CLICKED(IDC_HANGUP, &CDIALDLG::OnBnClickedHangup)
	ON_BN_CLICKED(IDC_ANSWER, &CDIALDLG::OnBnClickedAnswer)
END_MESSAGE_MAP()


// CDIALDLG 消息处理程序

void CDIALDLG::OnBnClickedDial1()
{
	this->m_sPhoneNumber +="1";
	this->UpdateData(false);
}

BOOL CDIALDLG::OnInitDialog(void)
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
	mbi.dwFlags |= SHCMBF_HMENU;
	mbi.nToolBarId = IDR_MENU3;
	mbi.hInstRes   = AfxGetInstanceHandle();

	SHCreateMenuBar(&mbi);
	setMode();
	return 0;
}
//返回
void CDIALDLG::On32800()
{
	CDialog::OnCancel();
}

void CDIALDLG::OnBnClickedBtnvideo()
{
	this->UpdateData();
	if(m_sPhoneNumber.IsEmpty())
	{
		AfxMessageBox(_T("请输入呼叫号码或者标示！"));
		return;
	}
	m_pMianDlg->m_pCallDlg->call(this->m_sPhoneNumber);
	m_pMianDlg->m_pCallDlg->DoModal();
}

void CDIALDLG::OnBnClickedBtndel()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CDIALDLG::OnBnClickedHangup()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CDIALDLG::OnBnClickedAnswer()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CDIALDLG::setMode()
{
	if(this->isDial){
		GetDlgItem(IDC_ANSWER)->EnableWindow(false);
		GetDlgItem(IDC_HANGUP)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_DIAL1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL6)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL7)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL8)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL9)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL0)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL10)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIAL12)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNAUDIO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNDEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTNVIDEO)->ShowWindow(SW_SHOW);

		
	}
	else
	{
		GetDlgItem(IDC_ANSWER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_HANGUP)->ShowWindow(SW_SHOW);

	    GetDlgItem(IDC_DIAL1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL0)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIAL12)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNAUDIO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNDEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNVIDEO)->ShowWindow(SW_HIDE);



	}
}
