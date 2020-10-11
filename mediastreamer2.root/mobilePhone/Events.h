#pragma once
#include "linphoneCore.h"
typedef struct eXosip_event eXosip_event_t;
typedef struct _sdp_context sdp_context_t;
typedef struct _sdp_payload sdp_payload_t;
typedef struct sdp_message sdp_message_t;
typedef enum {
	Unsupported,
	Supported,
	SupportedAndValid  /* valid= the presence of this codec is enough to make a call */
}SupportLevel;

class CEvents
{
public:
	CEvents(void);
	~CEvents(void);
public:
	
	static int linphone_accept_audio_offer(sdp_context_t *ctx,sdp_payload_t *payload);
	static void *sdp_context_get_user_pointer(sdp_context_t * ctx);
    static	SupportLevel CEvents::linphone_payload_is_supported(CLinphoneCore *lc, sdp_payload_t *payload,RtpProfile *local_profile,RtpProfile *dialog_profile, bool_t answering, PayloadType **local_payload_type);
    static	int find_payload_type_number_best_match(RtpProfile *prof, const char *rtpmap, const char *fmtp);
	static int find_payload_type_number(RtpProfile *prof, PayloadType *pt);
	static void linphone_call_proceeding(CLinphoneCore *lc, eXosip_event_t *ev);

	static int linphone_set_audio_offer(sdp_context_t *ctx);
	static int linphone_read_audio_answer(sdp_context_t *ctx,sdp_payload_t *payload);
	static bool_t linphone_call_matches_event(LinphoneCall *call, eXosip_event_t *ev);


	 int linphone_answer_sdp(CLinphoneCore *lc, eXosip_event_t *ev, sdp_message_t *sdp);

	int linphone_call_accepted(CLinphoneCore *lc, eXosip_event_t *ev);
	void linphone_call_init_media_params(LinphoneCall *call);
    void linphone_connect_incoming(CLinphoneCore *lc);
	void linphone_registration_success(CLinphoneCore *lc,eXosip_event_t *ev);
    int linphone_call_terminated(CLinphoneCore *lc, eXosip_event_t *ev);
	int linphone_call_failure(CLinphoneCore *lc, eXosip_event_t *ev);
	int linphone_inc_new_call(CLinphoneCore *lc, eXosip_event_t *ev);
	LinphoneCall * linphone_call_new_incoming(CLinphoneCore *lc, const char *from, const char *to, int cid, int did, int tid);
    void CEvents::linphone_handle_reinvite(CLinphoneCore *lc, eXosip_event_t *ev);
	void linphone_handle_ack(CLinphoneCore *lc, eXosip_event_t *ev);
	void linphone_call_redirected(CLinphoneCore *lc, eXosip_event_t *ev);
	void linphone_do_automatic_redirect(CLinphoneCore *lc, const char *contact);
	void linphone_call_ringing(CLinphoneCore *lc, eXosip_event_t *ev);
	void linphone_call_message_new(CLinphoneCore *lc, eXosip_event_t *ev);
	void linphone_process_dtmf_relay(CLinphoneCore *lc, eXosip_event_t *ev);
	void linphone_process_media_control_xml(CLinphoneCore *lc, eXosip_event_t *ev);
	int linphone_call_released(CLinphoneCore *lc, int cid);
	void linphone_registration_faillure(CLinphoneCore *lc, eXosip_event_t *ev);
    void linphone_other_request(CLinphoneCore *lc, eXosip_event_t *ev);
    bool_t comes_from_local_if(osip_message_t *msg);


static int linphone_set_video_offer(sdp_context_t *ctx);
static int linphone_accept_video_offer(sdp_context_t *ctx,sdp_payload_t *payload);
static int linphone_read_video_answer(sdp_context_t *ctx,sdp_payload_t *payload);

};
