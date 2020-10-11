#include "StdAfx.h"
#include "PhoneCore.h"
#include <eXosip2/eXosip.h>
#include "Configure.h"
#include "SdpHandler.h"
#include "misc.h"
#include "staticManager.h"
#include "Presence.h"
#include "private.h"
#include "sipsetup.h"
#include "GeneralState.h"
#include "proxy.h"
#include "events.h"
#include "proxyConfig.h"
#define ngettext(singular, plural, number)	(((number)==1)?(singular):(plural))
CPhoneCore::CPhoneCore(void)
{
	lc = new CLinphoneCore(this);
	m_pEvents = new CEvents();
	linphone_sdphandler = new sdp_handler_t();

	linphone_sdphandler->accept_audio_codecs=CEvents::linphone_accept_audio_offer;
	linphone_sdphandler->set_audio_codecs = CEvents::linphone_set_audio_offer;
	linphone_sdphandler->get_audio_codecs = CEvents::linphone_read_audio_answer;

	linphone_sdphandler->accept_video_codecs=CEvents::linphone_accept_video_offer;
	linphone_sdphandler->set_video_codecs = CEvents::linphone_set_video_offer;
	linphone_sdphandler->get_video_codecs = CEvents::linphone_read_video_answer;


}

CPhoneCore::~CPhoneCore(void)
{
   if(lc!=NULL)
       delete lc;
   if(m_pEvents!=NULL)
       delete m_pEvents;
   if(linphone_sdphandler!=NULL)
	   delete linphone_sdphandler;
}

#ifdef VIDEO_ENABLED

static PayloadType * payload_type_h264_packetization_mode_1=NULL;
static PayloadType * linphone_h263_1998=NULL;
static PayloadType * linphone_mp4v_es=NULL;
static PayloadType * linphone_h263_old=NULL;
#endif
void CPhoneCore::linphone_core_init (CStaticManager *vtable, const char *config_path, void * userdata)
{

	lc->data=userdata;
	lc->vtable = vtable;

	lc->pGeneralState->gstate_initialize(lc);
	lc->pGeneralState->gstate_new_state(lc, GSTATE_POWER_STARTUP, NULL);

	ortp_init();
	rtp_profile_set_payload(&av_profile,115,&payload_type_lpc1015);
	rtp_profile_set_payload(&av_profile,110,&payload_type_speex_nb);
	rtp_profile_set_payload(&av_profile,111,&payload_type_speex_wb);
	rtp_profile_set_payload(&av_profile,112,&payload_type_ilbc);
	rtp_profile_set_payload(&av_profile,116,&payload_type_truespeech);
	rtp_profile_set_payload(&av_profile,101,&payload_type_telephone_event);

#ifdef VIDEO_ENABLED
	rtp_profile_set_payload(&av_profile,97,&payload_type_theora);

	linphone_h263_1998=payload_type_clone(&payload_type_h263_1998);
	payload_type_set_recv_fmtp(linphone_h263_1998,"CIF=1;QCIF=1");
	rtp_profile_set_payload(&av_profile,98,linphone_h263_1998);

	linphone_h263_old=payload_type_clone(&payload_type_h263);
	payload_type_set_recv_fmtp(linphone_h263_old,"QCIF=2");
	rtp_profile_set_payload(&av_profile,34,linphone_h263_old);

	linphone_mp4v_es=payload_type_clone(&payload_type_mp4v);
	payload_type_set_recv_fmtp(linphone_mp4v_es,"profile-level-id=3");
	rtp_profile_set_payload(&av_profile,99,linphone_mp4v_es);
	rtp_profile_set_payload(&av_profile,100,&payload_type_x_snow);
	payload_type_h264_packetization_mode_1=payload_type_clone(&payload_type_h264);
	payload_type_set_recv_fmtp(payload_type_h264_packetization_mode_1,"packetization-mode=1");
	rtp_profile_set_payload(&av_profile,103,payload_type_h264_packetization_mode_1);
	rtp_profile_set_payload(&av_profile,102,&payload_type_h264);
#endif

	ms_init();

	lc->pConfig=new CConfigure(config_path);

#ifdef VINCENT_MAURY_RSVP
	/* default qos parameters : rsvp on, rpc off */
	lc->rsvp_enable = 1;
	lc->rpc_enable = 0;
#endif
	//	lc->pSipsetup->sip_setup_register_all();
	lc->sound_config_read();
	lc->net_config_read();
	lc->rtp_config_read();
	lc->codecs_config_read();
	lc->sip_config_read(); /* this will start eXosip*/
	video_config_read();
	//autoreplier_config_init(&lc->autoreplier_conf);
	lc->prev_mode=LINPHONE_STATUS_ONLINE;
	lc->presence_mode=LINPHONE_STATUS_ONLINE;
	lc->max_call_logs=15;
	lc->ui_config_read();
	ms_mutex_init(&lc->lock,NULL);
	lc->vtable->linphone_gtk_display_status(lc,("Ready"));
	lc->pGeneralState->gstate_new_state(lc, GSTATE_POWER_ON, NULL);
	lc->ready=TRUE;
}



void CPhoneCore::linphone_core_iterate()
{

	eXosip_event_t *ev;
	bool_t disconnected=FALSE;
	int disconnect_timeout = lc->linphone_core_get_nortp_timeout(); 
	time_t curtime=time(NULL);
	int elapsed;
	bool_t one_second_elapsed=FALSE;

	if (curtime-lc->prevtime>=1){
		lc->prevtime=curtime;
		one_second_elapsed=TRUE;
	}

	if (preview_finished){
		preview_finished=0;
		ring_stop(lc->ringstream);
		lc->ringstream=NULL;
		lc->lc_callback_obj_invoke(&lc->preview_finished_cb);
	}

	if (lc->exosip_running){
		while((ev=eXosip_event_wait(0,0))!=NULL){
			linphone_core_process_event(ev);
		}
		if (lc->automatic_action==0) {
			eXosip_lock();
			eXosip_automatic_action();
			eXosip_unlock();
		}
	}

	//proxy_update(curtime);

	if (lc->call!=NULL){
		LinphoneCall *call=lc->call;

		if (call->dir==LinphoneCallIncoming && call->state==LCStateRinging){
			elapsed=curtime-call->start_time;
			ms_message("incoming call ringing for %i seconds",elapsed);
			if (elapsed>lc->sip_conf.inc_timeout){
				lc->linphone_core_terminate_call(NULL);
			}
		}else if (call->state==LCStateAVRunning){
			if (one_second_elapsed){
				RtpSession *as=NULL,*vs=NULL;
				lc->prevtime=curtime;
				if (lc->audiostream!=NULL)
					as=lc->audiostream->session;
				if (lc->videostream!=NULL)
					vs=lc->videostream->session;
				lc->display_bandwidth(as,vs);
			}
#ifdef VIDEO_ENABLED
			if (lc->videostream!=NULL)	
				video_stream_iterate(lc->videostream);
#endif
			if (lc->audiostream!=NULL && disconnect_timeout>0)
				disconnected=!audio_stream_alive(lc->audiostream,disconnect_timeout);
		}
	}
	if (lc->linphone_core_video_preview_enabled()){
		if (lc->previewstream==NULL)
			lc->toggle_video_preview(TRUE);
#ifdef VIDEO_ENABLED
		else video_stream_iterate(lc->previewstream);
#endif
	}else{
		if (lc->previewstream!=NULL)
			lc->toggle_video_preview(FALSE);
	}
	if (disconnected)
		lc->linphone_core_disconnected();
	//if (one_second_elapsed && lc->pConfig->lp_config_needs_commit()){
	//	lc->pConfig->lp_config_sync();
	//}
}


void CPhoneCore::linphone_core_process_event(eXosip_event_t *ev)
{
	switch(ev->type){
		case EXOSIP_CALL_ANSWERED:
			ms_message("CALL_ANSWERED\n");
			m_pEvents->linphone_call_accepted(lc,ev);
			lc->pPrivate->linphone_authentication_ok(lc,ev);
			break;
		case EXOSIP_CALL_CLOSED:
		case EXOSIP_CALL_CANCELLED:
			ms_message("CALL_CLOSED or CANCELLED\n");
			m_pEvents->linphone_call_terminated(lc,ev);
			break;
		case EXOSIP_CALL_TIMEOUT:
		case EXOSIP_CALL_NOANSWER:
			ms_message("CALL_TIMEOUT or NOANSWER\n");
			m_pEvents->linphone_call_failure(lc,ev);
			break;
		case EXOSIP_CALL_REQUESTFAILURE:
		case EXOSIP_CALL_GLOBALFAILURE:
		case EXOSIP_CALL_SERVERFAILURE:
			ms_message("CALL_REQUESTFAILURE or GLOBALFAILURE or SERVERFAILURE\n");
			m_pEvents->linphone_call_failure(lc,ev);
			break;
		case EXOSIP_CALL_INVITE:
			ms_message("CALL_NEW\n");
			/* CALL_NEW is used twice in qos mode : 
			* when you receive invite (textinfo = "With QoS" or "Without QoS")
			* and when you receive update (textinfo = "New Call") */
			m_pEvents->linphone_inc_new_call(lc,ev);
			break;
		case EXOSIP_CALL_REINVITE:
			m_pEvents->linphone_handle_reinvite(lc,ev);
			break;
		case EXOSIP_CALL_ACK:
			ms_message("CALL_ACK");
			m_pEvents->linphone_handle_ack(lc,ev);
			break;
		case EXOSIP_CALL_REDIRECTED:
			ms_message("CALL_REDIRECTED");
			m_pEvents->linphone_call_redirected(lc,ev);
			break;
		case EXOSIP_CALL_PROCEEDING:
			ms_message("CALL_PROCEEDING");
			m_pEvents->linphone_call_proceeding(lc,ev);
			break;
		case EXOSIP_CALL_RINGING:
			ms_message("CALL_RINGING");
			m_pEvents->linphone_call_ringing(lc,ev);
			break;
		case EXOSIP_CALL_MESSAGE_NEW:
			ms_message("EXOSIP_CALL_MESSAGE_NEW");
			m_pEvents->linphone_call_message_new(lc,ev);
			break;
		case EXOSIP_CALL_MESSAGE_REQUESTFAILURE:
			if (ev->did<0 && ev->response && 
				(ev->response->status_code==407 || ev->response->status_code==401)){
					eXosip_default_action(ev);
			}
			break;
		case EXOSIP_IN_SUBSCRIPTION_NEW:
			ms_message("CALL_SUBSCRIPTION_NEW or UPDATE");
			lc->pPresence->linphone_subscription_new(lc,ev);
			break;
		case EXOSIP_SUBSCRIPTION_UPDATE:
			break;
		case EXOSIP_SUBSCRIPTION_NOTIFY:
			ms_message("CALL_SUBSCRIPTION_NOTIFY");
			lc->pPresence->linphone_notify_recv(lc,ev);
			break;
		case EXOSIP_SUBSCRIPTION_ANSWERED:
			ms_message("EXOSIP_SUBSCRIPTION_ANSWERED, ev->sid=%i\n",ev->sid);
			lc->pPresence->linphone_subscription_answered(lc,ev);
			break;
		case EXOSIP_SUBSCRIPTION_CLOSED:
			ms_message("EXOSIP_SUBSCRIPTION_CLOSED\n");
			lc->pPresence->linphone_subscription_closed(lc,ev);
			break;
		case EXOSIP_CALL_RELEASED:
			ms_message("CALL_RELEASED\n");
			m_pEvents->linphone_call_released(lc, ev->cid);
			break;
		case EXOSIP_REGISTRATION_FAILURE:
			ms_message("REGISTRATION_FAILURE\n");
			m_pEvents->linphone_registration_faillure(lc,ev);
			break;
		case EXOSIP_REGISTRATION_SUCCESS:
			lc->pPrivate->linphone_authentication_ok(lc,ev);
			this->m_pEvents->linphone_registration_success(lc,ev);
			break;
		case EXOSIP_MESSAGE_NEW:
			m_pEvents->linphone_other_request(lc,ev);
			break;
		case EXOSIP_MESSAGE_REQUESTFAILURE:
			if (ev->response && (ev->response->status_code == 407 || ev->response->status_code == 401)){
				/*the user is expected to have registered to the proxy, thus password is known*/
				eXosip_default_action(ev);
			}
			break;
		default:
			ms_message("Unhandled exosip event !");
			break;
	}
	eXosip_event_free(ev);
}




void CPhoneCore::linphone_core_start_media_streams(LinphoneCall *call){
	osip_from_t *me=lc->linphone_core_get_primary_contact_parsed();
	const char *tool="linphone-1.0";
	/* adjust rtp jitter compensation. It must be at least the latency of the sound card */
	int jitt_comp=MAX(lc->sound_conf.latency,lc->rtp_conf.audio_jitt_comp);

	if (call->media_start_time==0) call->media_start_time=time(NULL);

	char *cname=ortp_strdup_printf("%s@%s",me->url->username,me->url->host);
	{
		StreamParams *audio_params=&call->audio_params;
		if (!lc->use_files){
			MSSndCard *playcard=lc->sound_conf.play_sndcard;
			MSSndCard *captcard=lc->sound_conf.capt_sndcard;
			if (playcard==NULL) {
				OutputDebugString(_T("No card defined for playback !"));
				goto end;
			}
			if (captcard==NULL) {
				OutputDebugString(_T("No card defined for capture !"));
				goto end;
			}
			if (audio_params->relay_session_id!=NULL) 
				audio_stream_set_relay_session_id(lc->audiostream,audio_params->relay_session_id);
			audio_stream_start_now(
				lc->audiostream,
				call->profile,
				audio_params->remoteaddr,
				audio_params->remoteport,
				audio_params->remotertcpport,
				audio_params->pt,
				jitt_comp,
				playcard,
				captcard,
				lc->linphone_core_echo_cancelation_enabled());
		}else
		{
			audio_stream_start_with_files(
				lc->audiostream,
				call->profile,
				audio_params->remoteaddr,
				audio_params->remoteport,
				audio_params->remotertcpport,
				audio_params->pt,
				100,
				lc->play_file,
				lc->rec_file);
		}
		post_configure_audio_streams();	
		audio_stream_set_rtcp_information(lc->audiostream, cname, tool);
	}
#ifdef VIDEO_ENABLED
	{
		/* shutdown preview */
		if (lc->previewstream!=NULL) {
			video_preview_stop(lc->previewstream);
			lc->previewstream=NULL;
		}
		if (lc->video_conf.display || lc->video_conf.capture) {
			StreamParams *video_params=&call->video_params;

			if (video_params->remoteport>0){
				if (video_params->relay_session_id!=NULL) 
					video_stream_set_relay_session_id(lc->videostream,video_params->relay_session_id);
				video_stream_set_sent_video_size(lc->videostream,lc->linphone_core_get_preferred_video_size());
//				video_stream_enable_self_view(lc->videostream,lc->video_conf.selfview);
				if (lc->video_conf.display && lc->video_conf.capture)
					video_stream_start(lc->videostream,
					call->profile, video_params->remoteaddr, video_params->remoteport,
					video_params->remotertcpport,
					video_params->pt, jitt_comp, lc->video_conf.device,lc->m_VHwnd);
				else if (lc->video_conf.display)
					video_stream_recv_only_start(lc->videostream,
					call->profile, video_params->remoteaddr, video_params->remoteport,
					video_params->pt, jitt_comp);
				else if (lc->video_conf.capture)
					video_stream_send_only_start(lc->videostream,
					call->profile, video_params->remoteaddr, video_params->remoteport,
					video_params->remotertcpport,
					video_params->pt, jitt_comp, lc->video_conf.device);
				video_stream_set_rtcp_information(lc->videostream, cname,tool);
			}
		}
	}
#endif
	goto end;
end:
	ms_free(cname);
	osip_from_free(me);
	lc->call->state=LCStateAVRunning;
}


void  CPhoneCore::post_configure_audio_streams(){
	AudioStream *st=lc->audiostream;
	if (st->volrecv && st->volsend){
		float speed=lc->pConfig->lp_config_get_float("sound","el_speed",-1);
		float thres=lc->pConfig->lp_config_get_float("sound","el_thres",-1);
		float force=lc->pConfig->lp_config_get_float("sound","el_force",-1);
		float gain=lc->pConfig->lp_config_get_float("sound","mic_gain",-1);
		MSFilter *f=NULL;
		if (st->el_type==ELControlMic){
			f=st->volsend;
			if (speed==-1) speed=0.03f;
			if (force==-1) force=10.f;
		}
		else if (st->el_type==ELControlSpeaker){
			f=st->volrecv;
			if (speed==-1) speed=0.02f;
			if (force==-1) force=5.0f;
		}else {
			ms_fatal("Should not happen");
			return;
		}
		if (speed!=-1)
			ms_filter_call_method(f,MS_VOLUME_SET_EA_SPEED,&speed);
		if (thres!=-1)
			ms_filter_call_method(f,MS_VOLUME_SET_EA_THRESHOLD,&thres);
		if (force!=-1)
			ms_filter_call_method(f,MS_VOLUME_SET_EA_FORCE,&force);
		if (gain!=-1)
			audio_stream_set_mic_gain(st,gain);
	}
	parametrize_equalizer(st);

	/* replace by our default action*/
	audio_stream_play_received_dtmfs(lc->audiostream,FALSE);
	rtp_session_signal_connect(lc->audiostream->session,"telephone-event",(RtpCallback)CLinphoneCore::linphone_core_dtmf_received,(unsigned long)lc);

}

void CPhoneCore::parametrize_equalizer(AudioStream *st){
	if (st->equalizer){
		MSFilter *f=st->equalizer;
		int enabled=lc->pConfig->lp_config_get_int("sound","eq_active",0);
		const char *gains=lc->pConfig->lp_config_get_string("sound","eq_gains",NULL);
		ms_filter_call_method(f,MS_EQUALIZER_SET_ACTIVE,&enabled);
		if (enabled){
			if (gains){
				do{
					int bytes;
					MSEqualizerGain g;
					if (sscanf(gains,"%f:%f:%f %n",&g.frequency,&g.gain,&g.width,&bytes)==3){
						ms_message("Read equalizer gains: %f(~%f) --> %f",g.frequency,g.width,g.gain);
						ms_filter_call_method(f,MS_EQUALIZER_SET_GAIN,&g);
						gains+=bytes;
					}else break;
				}while(1);
			}
		}
	}
}


LinphoneCall * CPhoneCore::linphone_call_new_outgoing(const osip_from_t *from, const osip_to_t *to)
{
	LinphoneCall *call=(LinphoneCall *)ms_new0(LinphoneCall,1);
	char *fromstr=NULL,*tostr=NULL;
	call->dir=LinphoneCallOutgoing;
	call->cid=-1;
	call->did=-1;
	call->tid=-1;
	call->core=lc;
	lc->linphone_core_get_local_ip(to->url->host,call->localip);
	osip_from_to_str(from,&fromstr);
	osip_to_to_str(to,&tostr);
	linphone_call_init_common(call,fromstr,tostr);
	call->sdpctx=lc->pSdpHandler->sdp_handler_create_context(linphone_sdphandler,
		call->audio_params.natd_port>0 ? call->audio_params.natd_addr : call->localip,
		from->url->username,NULL);
	lc->pSdpHandler->sdp_context_set_user_pointer(call->sdpctx,(void*)call);
	lc->discover_mtu(to->url->host);
	return call;
}
void  CPhoneCore::linphone_call_init_common(LinphoneCall *call, char *from, char *to){
	call->state=LCStateInit;
	call->start_time=time(NULL);
	call->media_start_time=0;
	call->log=linphone_call_log_new(call, from, to);
	lc->pPresence->linphone_core_notify_all_friends(call->core,LINPHONE_STATUS_ONTHEPHONE);
	if (lc->linphone_core_get_firewall_policy()==LINPHONE_POLICY_USE_STUN) 
		lc->pMisc->linphone_core_run_stun_tests(call->core,call);
	call->profile=rtp_profile_new("Call RTP profile");
}

LinphoneCallLog * CPhoneCore::linphone_call_log_new(LinphoneCall *call, char *from, char *to){
	LinphoneCallLog *cl=(LinphoneCallLog *)ms_new0(LinphoneCallLog,1);
	LPSYSTEMTIME loctime = new SYSTEMTIME;
	cl->dir=call->dir;
#ifdef WIN32
	GetLocalTime(loctime);
	//loctime=*localtime(&call->start_time);
#else
	localtime_r(&call->start_time,&loctime);
#endif
	lc->my_strftime(cl->start_date,sizeof(cl->start_date),"%c",loctime);
	cl->from=from;
	cl->to=to;
	return cl;
}


void CPhoneCore::linphone_call_log_completed(LinphoneCallLog *calllog, LinphoneCall *call){
	//LinphoneCore *lc=call->core;
	calllog->duration=time(NULL)-call->start_time;
	switch(call->state){
		case LCStateInit:
			calllog->status=LinphoneCallAborted;
			break;
		case LCStateRinging:
			if (calllog->dir==LinphoneCallIncoming){
				char *info;
				calllog->status=LinphoneCallMissed;
				lc->missed_calls++;
				info=ortp_strdup_printf(ngettext("You have missed %i call.",
					"You have missed %i calls.", lc->missed_calls),
					lc->missed_calls);
				lc->vtable->linphone_gtk_display_status(lc,info);
				ms_free(info);
			}
			else calllog->status=LinphoneCallAborted;
			break;
		case LCStateAVRunning:
			calllog->status=LinphoneCallSuccess;
			break;
	}
	lc->call_logs=ms_list_append(lc->call_logs,(void *)calllog);
	if (ms_list_size(lc->call_logs)>lc->max_call_logs){
		MSList *elem;
		elem=lc->call_logs;
		linphone_call_log_destroy((LinphoneCallLog*)elem->data);
		lc->call_logs=ms_list_remove_link(lc->call_logs,elem);
	}

	lc->vtable->linphone_gtk_call_log_updated(lc,calllog);

}

void CPhoneCore::linphone_call_log_destroy(LinphoneCallLog *cl){
	if (cl->from!=NULL) osip_free(cl->from);
	if (cl->to!=NULL) osip_free(cl->to);
	ms_free(cl);
}
int CPhoneCore::user_register(void)
{

	CProxyConfig *user = new CProxyConfig();;
	user->reg_proxy="sip:211.100.17.169:5060";
	user->reg_identity="<sip:zjt@nist.gov>";
	user->reg_route="<sip:211.100.17.169:5060;lr>";
	user->realm="nist.gov";
	user->expires = 3600;
	user->reg_sendregister = true;

	lc->pProxy->linphone_proxy_config_register(user);
	lc->pProxy->linphone_core_add_proxy_config(lc,user);
	lc->pProxy->linphone_core_set_default_proxy(lc,user);

	return 0;
}


void CPhoneCore::video_config_read()
{
#ifdef VIDEO_ENABLED
	int capture, display;
	int enabled;
	const char *str;
	int ndev;
	const char **devices;
	const MSList *elem;
	int i;

	/* retrieve all video devices */
	elem=ms_web_cam_manager_get_list(ms_web_cam_manager_get());
	ndev=ms_list_size(elem);
	devices=(const char **)ms_malloc((ndev+1)*sizeof(const char *));
	for (i=0;elem!=NULL;elem=elem->next,i++){
		devices[i]=ms_web_cam_get_string_id((MSWebCam *)elem->data);
	}
	devices[ndev]=NULL;
	lc->video_conf.cams=devices;

	str=lc->pConfig->lp_config_get_string("video","device",NULL);
	if (str && str[0]==0) str=NULL;
	lc->linphone_core_set_video_device(str);

	lc->linphone_core_set_preferred_video_size_by_name(
		lc->pConfig->lp_config_get_string("video","size","cif"));

	enabled=lc->pConfig->lp_config_get_int("video","enabled",1);
	capture=lc->pConfig->lp_config_get_int("video","capture",enabled);
	display=lc->pConfig->lp_config_get_int("video","display",enabled);

	lc->linphone_core_enable_video(capture,display);
	lc->linphone_core_enable_self_view(TRUE);
	lc->linphone_core_enable_video_preview(TRUE);
#endif
}
