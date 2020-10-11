// SIPLoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mobilePhone.h"
#include "SIPLoginDlg.h"
#include "optiondlg.h"
#include <tpcshell.h>
#include <connmgr.h>
#include "iniwr.h"
#include "utils.h"
// CSIPLoginDlg 对话框

IMPLEMENT_DYNAMIC(CSIPLoginDlg, CDialog)

CSIPLoginDlg::CSIPLoginDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSIPLoginDlg::IDD, pParent)
, m_strStartupPath(_T(""))
{

}

CSIPLoginDlg::~CSIPLoginDlg()
{
}

void CSIPLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cboNetList);
}


BEGIN_MESSAGE_MAP(CSIPLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CSIPLoginDlg::OnBnClickedButton1)
	ON_WM_CREATE()
	ON_COMMAND(ID_32783, &CSIPLoginDlg::OnLogin)
	ON_COMMAND(ID_32784, &CSIPLoginDlg::OnCancel)
	ON_COMMAND(ID_32809, &CSIPLoginDlg::OnAcountSet)
END_MESSAGE_MAP()


// CSIPLoginDlg 消息处理程序

void CSIPLoginDlg::OnBnClickedButton1()
{
	CDialog::OnOK();
}

int CSIPLoginDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;



	return 0;
}

BOOL CSIPLoginDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();


	InitNetList();
	CUtils::fullScreenAndMenu(m_hWnd,IDR_MENU1);
	return 0;
}

void CSIPLoginDlg::OnLogin()
{
	CDialog::OnOK();
}

void CSIPLoginDlg::OnCancel()
{
	CDialog::OnCancel();
}


void CSIPLoginDlg::InitNetList(void)
{
	HRESULT hResult = E_FAIL;
	CONNMGR_DESTINATION_INFO DestInfo;
	int index = 0; 
	int nIndex = 0;
	hResult = ConnMgrEnumDestinations(index++, &DestInfo);
	while(SUCCEEDED(hResult)) 
	{

		CONNMGR_DESTINATION_INFO * lpDestInfo = new CONNMGR_DESTINATION_INFO;
		*lpDestInfo = DestInfo;
		nIndex = m_cboNetList.AddString(DestInfo.szDescription);
		if(nIndex >= 0)
		{
			m_cboNetList.SetItemDataPtr(nIndex, lpDestInfo);
		}
		hResult = ConnMgrEnumDestinations(index++, &DestInfo);
	}

	if(m_cboNetList.GetCount() > 0)
	{
		m_cboNetList.SetCurSel(0);
	}

	CIniWR hIni;
	CString strNetwork;
	hIni.GetString(_T("OPTION"), _T("Network"), strNetwork.GetBuffer(MAX_PATH), MAX_PATH, m_strStartupPath + _T("\\LibFetion.ini"));
	strNetwork.ReleaseBuffer();
	
	for(int i = 0; i < m_cboNetList.GetCount(); i++)
	{
		CString strNet;
		m_cboNetList.GetLBText(i, strNet);
		if(0 == strNetwork.Compare(strNet))
		{
			m_cboNetList.SetCurSel(i);
			break;
		}
	}
}
void CSIPLoginDlg::OnAcountSet()
{
	COptionDlg pdlg;
	/*pdlg.m_sUsrName=m_sUsrName;
	pdlg.m_sPassword=m_sPassword;
	pdlg.m_sDomain=m_sDomain;
	pdlg.m_sProxy=m_sProxy;
	pdlg.m_sPort=m_sPort;*/
	CIniWR hIni;
	CString strNetwork;
	hIni.GetString(_T("OPTION"), _T("Network"), strNetwork.GetBuffer(MAX_PATH), MAX_PATH, m_strStartupPath + _T("\\LibFetion.ini"));
	if(pdlg.DoModal() ==  IDOK)
	{
	/**	m_sUsrName=pdlg.m_sUsrName;
		m_sPassword=pdlg.m_sPassword;
		m_sDomain=pdlg.m_sDomain;
		m_sProxy=pdlg.m_sProxy;
		m_sPort=pdlg.m_sPort;
		**/
	}
}
