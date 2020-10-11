#pragma once
class CLinphoneCore;
typedef struct _LinphoneFriend LinphoneFriend;
typedef struct _LinphoneGeneralState LinphoneGeneralState;
typedef struct _LinphoneChatRoom LinphoneChatRoom;
typedef struct _LinphoneCallLog LinphoneCallLog;
class CmobilePhoneDlg;
class CStaticManager
{
public:
	CStaticManager(CmobilePhoneDlg * pWnd);
	~CStaticManager(void);
public:
 void linphone_gtk_show(CLinphoneCore *lc);
 void linphone_gtk_inv_recv(CLinphoneCore *lc, const char *from);
 void linphone_gtk_bye_recv(CLinphoneCore *lc, const char *from);
 void linphone_gtk_notify_recv(CLinphoneCore *lc, LinphoneFriend * fid, const char *url, const char *status, const char *img);
 void linphone_gtk_new_unknown_subscriber(CLinphoneCore *lc, LinphoneFriend *lf, const char *url);
 void linphone_gtk_auth_info_requested(CLinphoneCore *lc, const char *realm, const char *username);
 void linphone_gtk_display_status(const CLinphoneCore *lc, const char *status);
 void linphone_gtk_display_message(CLinphoneCore *lc, const char *msg);
 void linphone_gtk_display_warning(CLinphoneCore *lc, const char *warning);
 void linphone_gtk_display_url(CLinphoneCore *lc, const char *msg, const char *url);
 void linphone_gtk_display_question(CLinphoneCore *lc, const char *question);
 void linphone_gtk_call_log_updated(CLinphoneCore *lc, LinphoneCallLog *cl);
 void linphone_gtk_general_state(CLinphoneCore *lc, LinphoneGeneralState *gstate);
 void linphone_gtk_refer_received(CLinphoneCore *lc, const char *refer_to);
 void linphone_gtk_text_received(CLinphoneCore *lc, LinphoneChatRoom *room, const char *from, const char *message);
 static void  linphone_gtk_dtmf_received(CLinphoneCore *lc, int dtmf);
public:
	CmobilePhoneDlg*  m_pMainDlg;
};
