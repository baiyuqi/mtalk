#pragma once
#include "linphoneCore.h"
typedef struct eXosip_event eXosip_event_t;

typedef struct _sdp_handler  sdp_handler_t;
class CEvents;
class CPhoneCore
{
public:
	CPhoneCore(void);
	~CPhoneCore(void);
public:
	CLinphoneCore *lc;
	CEvents *m_pEvents;
	sdp_handler_t* linphone_sdphandler;
public:
	void linphone_core_init (CStaticManager *vtable, const char *config_path, void * userdata);
	void linphone_core_iterate();
	void linphone_core_process_event(eXosip_event_t *ev);

	void linphone_core_start_media_streams(LinphoneCall *call);
	void  post_configure_audio_streams();
	void CPhoneCore::parametrize_equalizer(AudioStream *st);
	LinphoneCall * linphone_call_new_outgoing(const osip_from_t *from, const osip_to_t *to);
	void  linphone_call_init_common(LinphoneCall *call, char *from, char *to);
		void video_config_read();

	
LinphoneCallLog * linphone_call_log_new(LinphoneCall *call, char *from, char *to);
void linphone_call_log_completed(LinphoneCallLog *calllog, LinphoneCall *call);
void linphone_call_log_destroy(LinphoneCallLog *cl);
int user_register(void);
};
