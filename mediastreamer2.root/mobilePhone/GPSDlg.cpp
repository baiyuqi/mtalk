// GPSDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mobilePhone.h"
#include "GPSDlg.h"
#include <commctrl.h>
#include <Gpsapi.h>
#include "GPSSink.h"    
#include "utils.h"

// CGPSDlg 对话框

IMPLEMENT_DYNAMIC(CGPSDlg, CDialog)

CGPSDlg::CGPSDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGPSDlg::IDD, pParent)
{

}

CGPSDlg::~CGPSDlg()
{
}

void CGPSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGPSDlg, CDialog)
	ON_COMMAND(ID_32814, &CGPSDlg::OnReturn)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CGPSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CUtils::fullScreenAndMenu(m_hWnd,IDR_MENUGPS,false);
	InitGPS();

	return TRUE;
}

// CGPSDlg 消息处理程序

int CGPSDlg::InitGPS(void)
{
	HRESULT hr = E_FAIL;

	pGPSSink = new CGPSSink();
	if (NULL != pGPSSink)
	{
		ShowMessage *sm = new ShowMessage();
		sm->m_pMessageWindow = this;
		sm->heightId = IDC_STAHEIGHT;
		sm->lattitudeId = IDC_STALAT;
		sm->longitudeId = IDC_STALNG;
		sm->satelliteId = IDC_STASATE;
		sm->speedId =IDC_STASPEED;
		sm->timeId = IDC_STATIME;

		pGPSSink ->SetShowMessage(sm);
	}
	pGPSController = new CGPSController();
	if (NULL == pGPSController)
	{
		return hr;
	}

	hr = pGPSController->InitDevice(pGPSSink);
	if (FAILED(hr))
	{
		return hr;
	}
	return hr;
}

void CGPSDlg::OnReturn()
{
	if(pGPSController!=NULL){
		pGPSController->UninitDevice();
	}

	delete pGPSSink;
	delete pGPSController;
    pGPSSink = NULL;
	pGPSController = NULL;
	this->OnOK();
}

void CGPSDlg::OnDestroy()
{
	CDialog::OnDestroy();
	if(pGPSController!=NULL){
		pGPSController->UninitDevice();
	}

	delete pGPSSink;
	delete pGPSController;
    
	// TODO: 在此处添加消息处理程序代码
}
