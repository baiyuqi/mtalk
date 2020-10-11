// OptionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mobilePhone.h"
#include "OptionDlg.h"

// COptionDlg �Ի���

IMPLEMENT_DYNAMIC(COptionDlg, CDialog)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent)
	, m_sUsrName(_T(""))
	, m_sPassword(_T(""))
	, m_sDomain(_T(""))
	, m_sProxy(_T(""))
	, m_sPort(_T(""))
	, m_sName(_T(""))
{

}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sUsrName);
	DDX_Text(pDX, IDC_EDIT2, m_sPassword);
	DDX_Text(pDX, IDC_EDIT4, m_sDomain);
	DDX_Text(pDX, IDC_EDIT3, m_sProxy);
	DDX_Text(pDX, IDC_EDIT5, m_sPort);
	DDX_Text(pDX, IDC_EDIT6, m_sName);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COptionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// COptionDlg ��Ϣ�������

void COptionDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

