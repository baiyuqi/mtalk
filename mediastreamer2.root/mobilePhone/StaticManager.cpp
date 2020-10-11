#include "StdAfx.h"
#include "resourceppc.h"
#include "StaticManager.h"
#include "mobilePhoneDlg.h"
#include "linphoneCore.h"
#include "videodlg.h"
#include "utils.h"
#include  "DIALDLG.h"
CStaticManager::CStaticManager(CmobilePhoneDlg * pWnd)
{
	this->m_pMainDlg = pWnd;
}
CStaticManager::~CStaticManager(void)
{
}
void CStaticManager::linphone_gtk_show(CLinphoneCore *lc)
{
	OutputDebugString(_T("1\r\n"));
}
 void CStaticManager::linphone_gtk_inv_recv(CLinphoneCore *lc, const char *from){
	m_pMainDlg->m_pDialDlg->m_sPhoneNumber = CUtils::MultiByteToWideChar(from);
	m_pMainDlg->m_pDialDlg->isDial =false;
	m_pMainDlg->m_pDialDlg->DoModal();
	 OutputDebugString(_T("2\r\n"));
}
 void CStaticManager::linphone_gtk_bye_recv(CLinphoneCore *lc, const char *from){
	 OutputDebugString(_T("3\r\n"));
}
 void CStaticManager::linphone_gtk_notify_recv(CLinphoneCore *lc, LinphoneFriend * fid, const char *url, const char *status, const char *img){
	 OutputDebugString(_T("4\r\n"));
}
 void CStaticManager::linphone_gtk_new_unknown_subscriber(CLinphoneCore *lc, LinphoneFriend *lf, const char *url){
	 OutputDebugString(_T("5\r\n"));
}
 void CStaticManager::linphone_gtk_auth_info_requested(CLinphoneCore *lc, const char *realm, const char *username){
	 OutputDebugString(_T("6\r\n"));
}
 void CStaticManager::linphone_gtk_display_status(const CLinphoneCore *lc, const char *status){   
	OutputDebugString(CUtils::MultiByteToWideChar(status)+_T("7\r\n"));
	if(status!=NULL)
		m_pMainDlg->GetDlgItem(IDC_SYSSTATE)->SetWindowText(CUtils::MultiByteToWideChar(status));
}
 void CStaticManager::linphone_gtk_display_message(CLinphoneCore *lc, const char *msg){
	 OutputDebugString(_T("8\r\n"));
	//  if(msg!=NULL)
	//	  this->m_pWnd->GetDlgItem(IDC_EWRNING)->SetWindowText(msg);   
}
 void CStaticManager::linphone_gtk_display_warning(CLinphoneCore *lc, const char *warning){
	 OutputDebugString(_T("9\r\n"));
 // if(warning!=NULL)
//		  this->m_pWnd->GetDlgItem(IDC_EWRNING)->SetWindowText(warning);   
}
 void CStaticManager::linphone_gtk_display_url(CLinphoneCore *lc, const char *msg, const char *url){
	 OutputDebugString(_T("10\r\n"));
}
 void CStaticManager::linphone_gtk_display_question(CLinphoneCore *lc, const char *question){
	 OutputDebugString(_T("11\r\n"));
}
 void CStaticManager::linphone_gtk_call_log_updated(CLinphoneCore *lc, LinphoneCallLog *cl){
	 OutputDebugString(_T("12\r\n"));
}
void CStaticManager::linphone_gtk_general_state(CLinphoneCore *lc, LinphoneGeneralState *gstate){
	 	switch(gstate->new_state){
		case GSTATE_CALL_OUT_CONNECTED:
			OutputDebugString(_T("13\r\n"));
		case GSTATE_CALL_IN_CONNECTED:
			OutputDebugString(_T("14\r\n"));
			//linphone_gtk_in_call_view_set_in_call();
		break;
		case GSTATE_CALL_ERROR:
			OutputDebugString(_T("15\r\n"));
			//lc->linphone_gtk_call_terminated(gstate->message);
		break;
		case GSTATE_CALL_END:
			OutputDebugString(_T("16\r\n"));
			//lc->linphone_gtk_call_terminated(NULL);
			m_pMainDlg->m_pCallDlg->OnCallEnd();
		break;
		case GSTATE_POWER_ON:
			OutputDebugString(_T("GSTATE_POWER_ON\r\n"));
		break;
		case GSTATE_POWER_STARTUP:
			OutputDebugString(_T("GSTATE_POWER_STARTUP\r\n"));
		break;
		case GSTATE_REG_OK:
			OutputDebugString(_T("GSTATE_REG_OK\r\n"));
		break;
		case GSTATE_REG_FAILED:
			OutputDebugString(_T("GSTATE_REG_FAILED\r\n"));
		break;
		default:
			OutputDebugString(_T("17\r\n"));
		break;
	}
}
 void CStaticManager::linphone_gtk_refer_received(CLinphoneCore *lc, const char *refer_to){
	 OutputDebugString(_T("18\r\n"));
}
 void CStaticManager::linphone_gtk_text_received(CLinphoneCore *lc, LinphoneChatRoom *room, const char *from, const char *message)
 {
	 OutputDebugString(_T("19\r\n"));
 }

 void  CStaticManager::linphone_gtk_dtmf_received(CLinphoneCore *lc, int dtmf){
	 OutputDebugString(_T("20\r\n"));
 }