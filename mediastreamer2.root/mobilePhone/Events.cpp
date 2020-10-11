#include "StdAfx.h"
#include "Events.h"
#include <eXosip2/eXosip.h>
#include <osipparser2/osip_message.h>
#include <osipparser2/osip_parser.h>
#include <mediastreamer2/mediastream.h>
#include "staticManager.h"
#include "proxy.h"
#include "GeneralState.h"
#include "sdphandler.h"
#include "private.h"
#include "misc.h"
#include "phoneCore.h"
#include "ProxyConfig.h"
CEvents::CEvents(void)
{
}

CEvents::~CEvents(void)
{
}


void CEvents::linphone_registration_success(CLinphoneCore *lc,eXosip_event_t *ev){
	CProxyConfig *cfg;
	osip_uri_t *requri=osip_message_get_uri(ev->request);
	char *msg;
	char *ru;
	osip_header_t *h=NULL;
	osip_uri_to_str(requri,&ru);
	msg=ms_strdup_printf(("Registration on %s successful."),ru);
	lc->vtable->linphone_gtk_display_status(lc,msg);
	ms_free(msg);
	osip_free(ru);
	cfg=lc->pProxy->linphone_core_get_proxy_config_from_rid(lc,ev->rid);
	ms_return_if_fail(cfg!=NULL);
	lc->pGeneralState->gstate_new_state(lc, GSTATE_REG_OK, NULL);
	osip_message_get_expires(ev->request,0,&h);
	if (h!=NULL && atoi(h->hvalue)!=0){
		cfg->registered=TRUE;
		lc->pProxy->linphone_proxy_config_register_again_with_updated_contact(cfg,ev->request,ev->response);
	}else cfg->registered=FALSE;
}


int CEvents::linphone_accept_audio_offer(sdp_context_t *ctx,sdp_payload_t *payload)
{
	RtpProfile *remote_profile;
	StreamParams *params;
	SupportLevel supported;
	LinphoneCall *call=(LinphoneCall*)sdp_context_get_user_pointer(ctx);
	CLinphoneCore *lc=call->core;
	PayloadType *lpt=NULL;

	params=&call->audio_params;
	remote_profile=call->profile;
	/* see if this codec is supported in our local rtp profile*/
	supported=linphone_payload_is_supported(lc,payload,lc->local_profile,remote_profile,TRUE,&lpt);
	if (supported==Unsupported) {
		ms_message("Refusing audio codec %i (%s)",payload->pt,payload->a_rtpmap);
		return -1;
	}
	if (lc->sip_conf.only_one_codec && params->initialized){
		ms_message("Only one codec has to be accepted.");
		return -1;
	}
	if (supported==SupportedAndValid) {		
		if (params->initialized==0){
			/* this is the first codec we accept, it is going to be used*/
			params->localport=lc->rtp_conf.audio_rtp_port;
			payload->localport=params->natd_port>0 ? 
				params->natd_port : lc->rtp_conf.audio_rtp_port;
			params->line=payload->line;
			params->pt=payload->pt; /* remember the first payload accepted */
			if (payload->relay_host!=NULL){
				strncpy(params->remoteaddr,payload->relay_host,sizeof(params->remoteaddr)-1);
				params->remoteport=payload->relay_port;
				params->remotertcpport=payload->relay_port;
				params->relay_session_id=payload->relay_session_id;
			}else{
				strncpy(params->remoteaddr,payload->c_addr,sizeof(params->remoteaddr)-1);
				params->remoteport=payload->remoteport;
				params->remotertcpport=payload->remoteport+1;
			}
			params->initialized=1;
			/* we can now update the allocated bandwidth for audio, and then video*/
			lc->pMisc->linphone_core_update_allocated_audio_bandwidth_in_call(lc,lpt);
			/* give our download bandwidth constraint*/
			payload->b_as_bandwidth=(lc->dw_audio_bw>0) ? lc->dw_audio_bw : 0;
		}else{
			/* refuse all other audio lines*/
			if(params->line!=payload->line) {
				ms_message("Only one audio line can be accepted.");
				//abort();
				return -1;
			}
		}
	}
	return 0;
}

void *CEvents::sdp_context_get_user_pointer(sdp_context_t * ctx){
	return ctx->reference;
}


SupportLevel CEvents::linphone_payload_is_supported(CLinphoneCore *lc, sdp_payload_t *payload,RtpProfile *local_profile,RtpProfile *dialog_profile, bool_t answering, PayloadType **local_payload_type)
{
	int localpt;
	SupportLevel ret;
	if (payload->a_rtpmap!=NULL){
		localpt=find_payload_type_number_best_match(local_profile,payload->a_rtpmap,payload->a_fmtp);
	}else{
		localpt=payload->pt;
		OutputDebugString(_T("payload has no rtpmap.\r\n"));
	}
	
	if (localpt>=0 && localpt <128 ){
		/* this payload is understood, but does the user want to use it ?? */
		PayloadType *rtppayload;
		rtppayload=rtp_profile_get_payload(local_profile,localpt);
		if (rtppayload==NULL) {
			OutputDebugString(_T("strange error !!"));
			return Unsupported;
		}
		*local_payload_type=rtppayload;
		if (strcmp(rtppayload->mime_type,"telephone-event")!=0){
			if (answering && !lc->pMisc->linphone_core_check_payload_type_usability(lc,rtppayload) ){
				OutputDebugString(_T("payload %s is not usable"));
				return Unsupported;
			}
			if ( !lc->pMisc->payload_type_enabled(rtppayload)) {
				OutputDebugString(_T("payload %s is not enabled."));
				return Unsupported;
			}
			ret=SupportedAndValid;
		}else ret=Supported;
		if (dialog_profile!=NULL){
			int dbw,ubw;
			/* this payload is supported in our local rtp profile, so add it to the dialog rtp
			profile */
			rtppayload=payload_type_clone(rtppayload);
			if (rtp_profile_get_payload(dialog_profile,payload->pt)!=NULL){
				OutputDebugString(_T("Payload %s type already entered, should not happen !"));
			}
			rtp_profile_set_payload(dialog_profile,payload->pt,rtppayload);
			/* add to the rtp payload type some other parameters (bandwidth) */
			if (rtppayload->type==PAYLOAD_VIDEO){
				dbw=lc->dw_video_bw;
				ubw=lc->up_video_bw;
			}else{ 
				dbw=lc->dw_audio_bw;
				ubw=lc->up_audio_bw;
			}
			if (payload->b_as_bandwidth!=0){
				ms_message("Remote bandwidth constraint: %i",payload->b_as_bandwidth);
				/*obey to remote bandwidth constraint AND our own upbandwidth constraint*/
				rtppayload->normal_bitrate=1000*lc->pPrivate->get_min_bandwidth(
					payload->b_as_bandwidth, ubw);
			}else{
				/*limit to upload bandwidth if exist, else no limit*/
				if (ubw>0) rtppayload->normal_bitrate=1000*ubw;
				else {
					if (rtppayload->type!=PAYLOAD_VIDEO){
						rtppayload->normal_bitrate=-1; /*allow speex to use maximum bitrate*/
					}
				}
			}
			if (payload->a_fmtp!=NULL){
				payload_type_set_send_fmtp(rtppayload,payload->a_fmtp);
			}
			payload->a_fmtp=rtppayload->recv_fmtp;
			if (payload->a_ptime>0){
				char tmp[30];
				snprintf(tmp,sizeof(tmp),"ptime=%i",payload->a_ptime);
				payload_type_append_send_fmtp(rtppayload,tmp);
				ms_message("%s attribute added to fmtp",tmp);
			}
		}
		return ret;
	}
	return Unsupported;
}

int CEvents::find_payload_type_number_best_match(RtpProfile *prof, const char *rtpmap, const char *fmtp){
	int localpt=rtp_profile_get_payload_number_from_rtpmap(prof,rtpmap);
	PayloadType *pt;
	char value[10];
	if (localpt<0) return -1;
	pt=rtp_profile_get_payload(prof,localpt);
	if (strcasecmp(pt->mime_type,"H264")==0){
		/*hack for H264: need to answer with same packetization-mode*/
		PayloadType tmp;
		memset(&tmp,0,sizeof(tmp));
		tmp.mime_type="H264";
		tmp.clock_rate=pt->clock_rate;
		if (fmtp && fmtp_get_value(fmtp,"packetization-mode",value,sizeof(value))){
			tmp.recv_fmtp=(atoi(value)==1) ? "packetization-mode=1" : NULL;
		}
		localpt=find_payload_type_number(prof,&tmp);
	}
	return localpt;
}
 int CEvents::find_payload_type_number(RtpProfile *prof, PayloadType *pt){
	int candidate=-1,i;
	PayloadType *it;
	for(i=0;i<127;++i){
		it=rtp_profile_get_payload(prof,i);
		if (it!=NULL && strcasecmp(pt->mime_type,it->mime_type)==0 
			&& (pt->clock_rate==it->clock_rate || pt->clock_rate<=0) ){
			if ( (pt->recv_fmtp && it->recv_fmtp && strcasecmp(pt->recv_fmtp,it->recv_fmtp)==0) ||
				(pt->recv_fmtp==NULL && it->recv_fmtp==NULL) ){
				/*exact match*/
				return i;
			}else candidate=i;
		}
	}
	if (candidate==-1) ms_fatal("Should not happen.");
	return candidate;
}
 int CEvents::linphone_set_audio_offer(sdp_context_t *ctx)
{
	LinphoneCall *call=(LinphoneCall*)sdp_context_get_user_pointer(ctx);
	CLinphoneCore *lc=call->core;
	PayloadType *codec;
	MSList *elem;
	sdp_payload_t payload;
	
	
	elem=lc->codecs_conf.audio_codecs;
	while(elem!=NULL){
		codec=(PayloadType*) elem->data;
		if (lc->pMisc->linphone_core_check_payload_type_usability(lc,codec) && lc->pMisc->payload_type_enabled(codec)){
			lc->pSdpHandler->sdp_payload_init(&payload);
			payload.a_rtpmap=ortp_strdup_printf("%s/%i/1",codec->mime_type,codec->clock_rate);
			payload.pt=rtp_profile_get_payload_number_from_rtpmap(lc->local_profile,payload.a_rtpmap);
			payload.localport=call->audio_params.natd_port > 0 ? 
						call->audio_params.natd_port : lc->rtp_conf.audio_rtp_port;
			if (strcasecmp(codec->mime_type,"iLBC")==0){
				/* prefer the 30 ms mode */
				payload.a_fmtp="ptime=30";
			}
			lc->pSdpHandler->sdp_context_add_audio_payload(ctx,&payload);
			ms_free(payload.a_rtpmap);
		}
		elem=ms_list_next(elem);
	}
	/* add telephone-event payload*/
	lc->pSdpHandler->sdp_payload_init(&payload);
	payload.pt=rtp_profile_get_payload_number_from_mime(lc->local_profile,"telephone-event");
	payload.a_rtpmap="telephone-event/8000";
	payload.a_fmtp="0-11";
	if (lc->dw_audio_bw>0) payload.b_as_bandwidth=lc->dw_audio_bw;
	lc->pSdpHandler->sdp_context_add_audio_payload(ctx,&payload);
	return 0;
}



int CEvents::linphone_read_audio_answer(sdp_context_t *ctx,sdp_payload_t *payload)
{
	LinphoneCall *call=(LinphoneCall*)sdp_context_get_user_pointer(ctx);
	CLinphoneCore *lc=call->core;
	StreamParams *params;
	SupportLevel supported;
	PayloadType *lpt=NULL;
	
	/* paranoid check: see if this codec is supported in our local rtp profile*/
	supported=linphone_payload_is_supported(lc, payload,lc->local_profile,call->profile,FALSE,&lpt);
	if (supported==Unsupported) {
		OutputDebugString(_T("This remote sip phone did not answer properly to my sdp offer: rtpmap=%s"));
		return 0;
	}
	if (supported==SupportedAndValid){
		params=&call->audio_params;
		if (params->initialized==0){
			/* this is the first codec we accept, this is the one that is going to be used (at least for sending
			data.*/
			params->localport=lc->rtp_conf.audio_rtp_port;
			params->line=payload->line;
			params->pt=payload->pt; /* remember the first payload accepted */
			if (payload->relay_host!=NULL){
				strncpy(params->remoteaddr,payload->relay_host,sizeof(params->remoteaddr)-1);
				params->remoteport=payload->relay_port;
				params->remotertcpport=payload->relay_port;
				params->relay_session_id=payload->relay_session_id;
			}else{
				strncpy(params->remoteaddr,payload->c_addr,sizeof(params->remoteaddr)-1);
				params->remoteport=payload->remoteport;
				params->remotertcpport=payload->remoteport+1;
			}
			params->initialized=1;
			/* we can now update the allocated bandwidth for audio, and then video*/
			lc->pMisc->linphone_core_update_allocated_audio_bandwidth_in_call(lc,lpt);
		}
	}
	return 0;
}

int CEvents::linphone_call_accepted(CLinphoneCore *lc, eXosip_event_t *ev)
{
	LinphoneCall *call=lc->call;
	sdp_message_t *sdp;
	const char *sdpanswer=NULL;
	osip_message_t *msg=NULL;
	int err;
	if (call==NULL){
		OutputDebugString(_T("No call to accept."));
		return 0;
	}
	linphone_call_proceeding(lc,ev);
	if (!linphone_call_matches_event(lc->call,ev)) return 0;
	call->auth_pending=FALSE;
	if (call->state==LCStateAVRunning){
		return 0; /*already accepted*/
	}
	linphone_call_init_media_params(call);
	sdp=eXosip_get_sdp_info(ev->response);
	if (!lc->sip_conf.sdp_200_ack){
		err=0;
		lc->pSdpHandler->sdp_context_read_answer(call->sdpctx,sdp);
	}else{
		/*we receive a 200OK with an sdp offer*/
		err=linphone_answer_sdp(lc,ev,sdp);
		if (err==0) sdpanswer=call->sdpctx->answerstr;
	}
	if (err==0){
		lc->pGeneralState->gstate_new_state(lc, GSTATE_CALL_OUT_CONNECTED, NULL);
		linphone_connect_incoming(lc);
	}
	/*send the ack once streams are started*/
	eXosip_call_build_ack(ev->did,&msg);
	if (sdpanswer!=NULL) lc->linphone_set_sdp(msg,sdpanswer);
	eXosip_call_send_ack(ev->did,msg);
	if (err!=0){
		/*send a bye*/
		OutputDebugString(_T("Incompatible SDP offer received in 200Ok, need to abort the call"));
		lc->linphone_core_terminate_call(NULL);
	}
	sdp_message_free(sdp);
	return 0;
}

void CEvents::linphone_call_proceeding(CLinphoneCore *lc, eXosip_event_t *ev){
	if (lc->call==NULL || (lc->call->cid!=-1 && !linphone_call_matches_event(lc->call,ev)) ) {
		OutputDebugString(_T("This call has been canceled."));
		eXosip_lock();
		eXosip_call_terminate(ev->cid,ev->did);
		eXosip_unlock();
		return;
	}
	lc->call->cid=ev->cid;
	lc->call->did=ev->did;
	lc->call->tid=ev->tid;
}
bool_t CEvents::linphone_call_matches_event(LinphoneCall *call, eXosip_event_t *ev){
	return call->cid==ev->cid;
}
void CEvents::linphone_call_init_media_params(LinphoneCall *call){
	memset(&call->audio_params,0,sizeof(call->audio_params));
	memset(&call->video_params,0,sizeof(call->video_params));
}

int CEvents::linphone_answer_sdp(CLinphoneCore *lc, eXosip_event_t *ev, sdp_message_t *sdp){
	int status=200;
	sdp_context_t *ctx=NULL;
	
	ctx=lc->call->sdpctx;
	/* get the result of the negociation */
	lc->pSdpHandler->sdp_context_get_answer(ctx,sdp);
	status=lc->pSdpHandler->sdp_context_get_status(ctx);

	if (status==200){
		lc->linphone_core_init_media_streams();
		return 0;
	}else{
		if (status==-1) status=415;
	}
	return -1;
}

void CEvents::linphone_connect_incoming(CLinphoneCore *lc){
	lc->vtable->linphone_gtk_show(lc);
	lc->vtable->linphone_gtk_display_status(lc,("Connected."));
	lc->call->state=LCStateAVRunning;
	if (lc->ringstream!=NULL){
		ring_stop(lc->ringstream);
		lc->ringstream=NULL;
	}
	if (lc->audiostream->ticker!=NULL){
		/*case where we accepted early media */
		lc->linphone_core_stop_media_streams();
		lc->linphone_core_init_media_streams();
	}
	lc->pPhoneCore->linphone_core_start_media_streams(lc->call);
}


int  CEvents::linphone_call_terminated(CLinphoneCore *lc, eXosip_event_t *ev)
{
	/*stop ringing if necessary*/
	if (lc->call!=NULL){
		if (lc->call->cid!=ev->cid){
			/* this is not current call */
			ms_message("call %i terminated, this was not current call.",ev->cid);
			return 0;
		}
	}
	
	ms_message("Current call terminated...");
	if (lc->ringstream!=NULL) {
		ring_stop(lc->ringstream);
		lc->ringstream=NULL;
	}
	lc->linphone_core_stop_media_streams();
	lc->vtable->linphone_gtk_show(lc);
	lc->vtable->linphone_gtk_display_status(lc,("Call terminated."));
	lc->pGeneralState->gstate_new_state(lc, GSTATE_CALL_END, NULL);

		char *from;
		osip_from_to_str(ev->request->from,&from);
		lc->vtable->linphone_gtk_bye_recv(lc,from);
		osip_free(from);

	if (lc->call!=NULL){
		lc->linphone_call_destroy(lc->call);
		lc->call=NULL;
	}
	return 0;
}


int  CEvents::linphone_call_failure(CLinphoneCore *lc, eXosip_event_t *ev)
{
	const char *reason="";
	char *msg486=("User is busy.");
	char *msg480=("User is temporarily unavailable.");
	char *msg487=("Request Cancelled.");
	/*char *retrymsg=_("%s. Retry after %i minute(s).");*/
	char *msg600=("User does not want to be disturbed.");
	char *msg603=("Call declined.");
	char* tmpmsg=msg486;
	int code;
	LinphoneCall *call=lc->call;

	if (call){
		/*check that the faillure is related to this call, not an old one*/
		if (!linphone_call_matches_event(call,ev)) {
			OutputDebugString(_T("Failure reported for an old call."));
			return 0;
		}
	}

	if (ev->response){
		code=osip_message_get_status_code(ev->response);
		reason=osip_message_get_reason_phrase(ev->response);
	}else code=-110;
	lc->vtable->linphone_gtk_show(lc);
	
	switch(code)
	{
		case 401:
		case 407:
			if (lc->call!=NULL)
				lc->pPrivate->linphone_process_authentication(lc,ev);
			return 0;
			break;
		case 400:
			lc->vtable->linphone_gtk_display_status(lc,("Bad request"));
		break;
		case 404:
			lc->vtable->linphone_gtk_display_status(lc,("User cannot be found at given address."));
		break;
		case 415:
			lc->vtable->linphone_gtk_display_status(lc,("Remote user cannot support any of proposed codecs."));
		break;
		case 480:
			tmpmsg=msg480;
		case 486:
			/*
			msg_header_getbyname(msg,"retry-after",0,&retry);
			if (retry!=NULL)
			{
				umsg=g_malloc(strlen(tmpmsg)+strlen(retrymsg)+13);
				sprintf(umsg,retrymsg,tmpmsg,atoi(retry->hvalue)/60);
				lc->vtable.display_message(lc,umsg);
				ms_free(umsg);
			}*/		
			lc->vtable->linphone_gtk_display_message(lc,tmpmsg);
		break;
		case 487:
			lc->vtable->linphone_gtk_display_status(lc,msg487);
		break;	
		case 600:
			lc->vtable->linphone_gtk_display_message(lc,msg600);
		break;
		case 603:
			lc->vtable->linphone_gtk_display_status(lc,msg603);
		break;
		case -110:  /* time out, call leg is lost */
			lc->vtable->linphone_gtk_display_status(lc,("Timeout."));
		break;
		case -111:
			lc->vtable->linphone_gtk_display_status(lc,("Remote host was found but refused connection."));
		break;
		
		default:
			if (code>0)
			{
				lc->vtable->linphone_gtk_display_status(lc,reason);
			}
			else OutputDebugString(_T("failure_cb unknown code=%i\n"));	
	}
	if (lc->ringstream!=NULL) {
		ring_stop(lc->ringstream);
		lc->ringstream=NULL;
	}
	lc->linphone_core_stop_media_streams();
	if (call!=NULL) {
		lc->linphone_call_destroy(call);
		lc->pGeneralState->gstate_new_state(lc, GSTATE_CALL_ERROR, NULL);
		lc->call=NULL;
	}
	return 0;
}
int CEvents::linphone_inc_new_call(CLinphoneCore *lc, eXosip_event_t *ev)
{
	sdp_message_t *sdp=NULL;
	osip_from_t *from_url=ev->request->from;
	char *barmesg;
	char *from;
	char *to;
	int err;

	osip_from_to_str(ev->request->from,&from);
	osip_to_to_str(ev->request->to,&to);	
	
	/* first check if we can answer successfully to this invite */
	if (lc->presence_mode!=LINPHONE_STATUS_ONLINE){
		ms_message("Not present !! presence mode : %d\n",lc->presence_mode);
		eXosip_lock();
		if (lc->presence_mode==LINPHONE_STATUS_BUSY)
			eXosip_call_send_answer(ev->tid,486,NULL);
		else if (lc->presence_mode==LINPHONE_STATUS_AWAY
			 ||lc->presence_mode==LINPHONE_STATUS_BERIGHTBACK
			 ||lc->presence_mode==LINPHONE_STATUS_ONTHEPHONE
			 ||lc->presence_mode==LINPHONE_STATUS_OUTTOLUNCH
			 ||lc->presence_mode==LINPHONE_STATUS_OFFLINE)
		  eXosip_call_send_answer(ev->tid,480,NULL);
		else if (lc->presence_mode==LINPHONE_STATUS_NOT_DISTURB)
		  eXosip_call_send_answer(ev->tid,480,NULL);
		else if (lc->alt_contact!=NULL && lc->presence_mode==LINPHONE_STATUS_MOVED)
		  {
			osip_message_t *msg;
			eXosip_call_build_answer(ev->tid,302,&msg);
			osip_message_set_contact(msg,lc->alt_contact);
			eXosip_call_send_answer(ev->tid,302,msg);
		  }
		else if (lc->alt_contact!=NULL && lc->presence_mode==LINPHONE_STATUS_ALT_SERVICE)
		  {
			osip_message_t *msg;
			eXosip_call_build_answer(ev->tid,380,&msg);
			osip_message_set_contact(msg,lc->alt_contact);
			eXosip_call_send_answer(ev->tid,380,msg);
		  }
		else
		  eXosip_call_send_answer(ev->tid,486,NULL);
		eXosip_unlock();
		goto end;
	}
	if (lc->call!=NULL){/*busy*/
		eXosip_lock();
		eXosip_call_send_answer(ev->tid,486,NULL);
		eXosip_unlock();
		goto end;
	}
	lc->call=linphone_call_new_incoming(lc,from,to,ev->cid,ev->did,ev->tid);
	sdp=eXosip_get_sdp_info(ev->request);
	if (sdp==NULL){
		ms_message("No sdp body in invite, 200-ack scheme");
		err=0;
	}else{
		err=linphone_answer_sdp(lc,ev,sdp);
	}
	if (!err){
		char *tmp;
		if (lc->pMisc->from_2char_without_params(from_url,&tmp)!=0){
			tmp=ms_strdup("Unknown user");
		}
		lc->pGeneralState->gstate_new_state(lc, GSTATE_CALL_IN_INVITE, tmp);
		barmesg=ortp_strdup_printf("%s %s",tmp,_T("is contacting you."));
		lc->vtable->linphone_gtk_show(lc);
		lc->vtable->linphone_gtk_display_status(lc,barmesg);

		/* play the ring */
		if (lc->sound_conf.ring_sndcard!=NULL){
			ms_message("Starting local ring...");
			lc->ringstream=ring_start(lc->sound_conf.local_ring,2000,lc->sound_conf.ring_sndcard);
		}
		linphone_call_set_state(lc->call,LCStateRinging);
		eXosip_lock();
		eXosip_call_send_answer(ev->tid,180,NULL);
		eXosip_unlock();

		lc->vtable->linphone_gtk_inv_recv(lc,tmp);
		ms_free(barmesg);
		osip_free(tmp);		
	}else{
		OutputDebugString(_T("Error during sdp negociation. "));
		eXosip_lock();
		eXosip_call_send_answer(ev->tid,415,NULL);
		eXosip_unlock();
		lc->linphone_call_destroy(lc->call);
		lc->call=NULL;
	}
	end:
	osip_free(from);
	osip_free(to);
	if (sdp) sdp_message_free(sdp);
	return 0;
}
LinphoneCall * CEvents::linphone_call_new_incoming(CLinphoneCore *lc, const char *from, const char *to, int cid, int did, int tid){
	LinphoneCall *call=(LinphoneCall *)ms_new0(LinphoneCall,1);
	osip_from_t *me= lc->linphone_core_get_primary_contact_parsed();
	osip_from_t *from_url=NULL;
	call->dir=LinphoneCallIncoming;
	call->cid=cid;
	call->did=did;
	call->tid=tid;
	call->core=lc;
	osip_from_init(&from_url);
	osip_from_parse(from_url, from);
	lc->linphone_core_get_local_ip(from_url->url->host,call->localip);
	lc->pPhoneCore->linphone_call_init_common(call, osip_strdup(from), osip_strdup(to));
	call->sdpctx=lc->pSdpHandler->sdp_handler_create_context(lc->pPhoneCore->linphone_sdphandler,
		call->audio_params.natd_port>0 ? call->audio_params.natd_addr : call->localip,
		me->url->username,NULL);
	lc->pSdpHandler->sdp_context_set_user_pointer(call->sdpctx,(void*)call);
	lc->discover_mtu(from_url->url->host);
	osip_from_free(me);
	osip_from_free(from_url);
	return call;
}

void CEvents::linphone_handle_reinvite(CLinphoneCore *lc, eXosip_event_t *ev){
	sdp_message_t *sdp=eXosip_get_sdp_info(ev->request);
	sdp_context_t *ctx;
	LinphoneCall *call=lc->call;
	char *answer;
	int status;
	if (sdp==NULL){
		OutputDebugString(_T("No sdp in reinvite !"));
		eXosip_lock();
		eXosip_call_send_answer(ev->tid,603,NULL);
		eXosip_unlock();
		return;
	}
	ctx=call->sdpctx;
	/* get the result of the negociation */
	linphone_call_init_media_params(call);
	answer=lc->pSdpHandler->sdp_context_get_answer(ctx,sdp);
	status=lc->pSdpHandler->sdp_context_get_status(ctx);
	if (status==200){
		osip_message_t *msg=NULL;
		lc->linphone_core_stop_media_streams();
		lc->linphone_core_init_media_streams();
		eXosip_lock();
		if (eXosip_call_build_answer(ev->tid,200,&msg)<0){
			OutputDebugString(_T("Reinvite for closed call ?"));
                        eXosip_unlock();
                        lc->linphone_core_stop_media_streams();
			sdp_message_free(sdp);
			return ;
		}
		answer=call->sdpctx->answerstr;	/* takes the sdp already computed*/
		lc->linphone_set_sdp(msg,answer);
		eXosip_call_send_answer(ev->tid,200,msg);
		eXosip_unlock();
		lc->pPhoneCore->linphone_core_start_media_streams(call);
	}else{
		eXosip_lock();
		eXosip_call_send_answer(ev->tid,status,NULL);
		eXosip_unlock();
	}
	sdp_message_free(sdp);
}
void CEvents::linphone_handle_ack(CLinphoneCore *lc, eXosip_event_t *ev){
	sdp_message_t *sdp=eXosip_get_sdp_info(ev->ack);
	if (sdp){
		lc->pSdpHandler->sdp_context_read_answer(lc->call->sdpctx,sdp);
		linphone_connect_incoming(lc);
		sdp_message_free(sdp);
	}
}
void CEvents::linphone_call_redirected(CLinphoneCore *lc, eXosip_event_t *ev){
	int code=osip_message_get_status_code(ev->response);
	char *contact=NULL;
	osip_contact_t *ct;
	osip_message_get_contact(ev->response,0,&ct);
	if (ct) osip_contact_to_str(ct,&contact);
	switch(code){
		case 380:
			lc->vtable->linphone_gtk_display_url(lc,("User is not reachable at the moment but he invites you\nto contact him using the following alternate resource:"),contact);
			if (lc->call!=NULL) lc->linphone_call_destroy(lc->call);
			lc->call=NULL;
			break;
		case 302:
			linphone_do_automatic_redirect(lc,contact);
			break;
	}
	if (contact) osip_free(contact);
}

void CEvents::linphone_do_automatic_redirect(CLinphoneCore *lc, const char *contact){
	char *msg=ortp_strdup_printf(("Redirected to %s..."),contact);
	lc->vtable->linphone_gtk_display_status(lc,msg);
	ms_free(msg);
	if (lc->call!=NULL) lc->linphone_call_destroy(lc->call);
	lc->call=NULL;
	lc->linphone_core_invite(contact);
}
void CEvents::linphone_call_ringing(CLinphoneCore *lc, eXosip_event_t *ev){
	sdp_message_t *sdp=eXosip_get_sdp_info(ev->response);
	LinphoneCall *call=lc->call;
	
	linphone_call_proceeding(lc,ev);
	if (call==NULL) return;
	if (sdp==NULL){
		if (lc->ringstream!=NULL) return;	/*already ringing !*/
		if (lc->sound_conf.play_sndcard!=NULL){
			ms_message("Remote ringing...");
			lc->ringstream=ring_start(lc->sound_conf.remote_ring,2000,lc->sound_conf.play_sndcard);
		}
	}else{
		/*accept early media */
		StreamParams *audio_params;
		if (call==NULL){
			OutputDebugString(_T("No call ?"));
			goto end;
		}
		if (lc->audiostream->ticker!=NULL){
			/*streams already started */
			ms_message("Early media already started.");
			goto end;
		}
		audio_params=&call->audio_params;
		lc->pSdpHandler->sdp_context_read_answer(lc->call->sdpctx,sdp);
		lc->vtable->linphone_gtk_show(lc);
		lc->vtable->linphone_gtk_display_status(lc,("Early media."));
		lc->pGeneralState->gstate_new_state(lc, GSTATE_CALL_OUT_CONNECTED, NULL);
		if (lc->ringstream!=NULL){
			ring_stop(lc->ringstream);
			lc->ringstream=NULL;
		}
		ms_message("Doing early media...");
		lc->pPhoneCore->linphone_core_start_media_streams(call);
	}
	call->state=LCStateRinging;
	goto end;
	end:
		sdp_message_free(sdp);

}

void CEvents::linphone_call_message_new(CLinphoneCore *lc, eXosip_event_t *ev){
	if (ev->request){
		if (MSG_IS_INFO(ev->request)){
			osip_content_type_t *ct;
			ct=osip_message_get_content_type(ev->request);
			if (ct && ct->subtype){
				if (strcmp(ct->subtype,"media_control+xml")==0)
					linphone_process_media_control_xml(lc,ev);
				else if (strcmp(ct->subtype,"dtmf-relay")==0)
					linphone_process_dtmf_relay(lc,ev);
				else ms_message("Unhandled SIP INFO.");
			}
		}
	}else OutputDebugString(_T("linphone_call_message_new: No request ?"));
}

void CEvents::linphone_process_media_control_xml(CLinphoneCore *lc, eXosip_event_t *ev){
	osip_body_t *body=NULL;
	osip_message_get_body(ev->request,0,&body);
	if (body && body->body!=NULL &&
		strstr(body->body,"picture_fast_update")){
		osip_message_t *ans=NULL;
		ms_message("Receiving VFU request !");
#ifdef VIDEO_ENABLED
		if (lc->videostream)
			video_stream_send_vfu(lc->videostream);
#endif
		eXosip_call_build_answer(ev->tid,200,&ans);
		if (ans)
			eXosip_call_send_answer(ev->tid,200,ans);
	}
}
void CEvents::linphone_process_dtmf_relay(CLinphoneCore *lc, eXosip_event_t *ev){
	osip_body_t *body=NULL;
	osip_message_get_body(ev->request,0,&body);
	if (body && body->body!=NULL){
		osip_message_t *ans=NULL;
		const char *name=strstr(body->body,"Signal");
		if (name==NULL) name=strstr(body->body,"signal");
		if (name==NULL) {
			OutputDebugString(_T("Could not extract the dtmf name from the SIP INFO."));
		}else{
			char tmp[2];
			name+=strlen("signal");
			if (sscanf(name," = %1s",tmp)==1){
				ms_message("Receiving dtmf %s via SIP INFO.",tmp);
			
				lc->vtable->linphone_gtk_dtmf_received(lc, tmp[0]);
			}
		}
		
		eXosip_call_build_answer(ev->tid,200,&ans);
		if (ans)
			eXosip_call_send_answer(ev->tid,200,ans);
	}
}
int CEvents::linphone_call_released(CLinphoneCore *lc, int cid){
	LinphoneCall *call=lc->call;
	if (call!=NULL && call->cid==cid){
		
		lc->linphone_call_destroy(lc->call);
		lc->call=NULL;
		lc->vtable->linphone_gtk_display_status(lc,("Could not reach destination."));
		
		lc->pGeneralState->gstate_new_state(lc, GSTATE_CALL_ERROR, NULL);
	}
	return 0;
}

void CEvents::linphone_registration_faillure(CLinphoneCore *lc, eXosip_event_t *ev){
	int status_code=0;
	char *msg;
	const char *reason=NULL;
	osip_uri_t *requri=osip_message_get_uri(ev->request);
	char *ru;
	CProxyConfig *cfg;
	
	if (ev->response){
		status_code=osip_message_get_status_code(ev->response);
		reason=osip_message_get_reason_phrase(ev->response);
	}
	switch(status_code){
		case 401:
		case 407:
			lc->pPrivate->linphone_process_authentication(lc,ev);
			break;
		default:
			cfg=lc->pProxy->linphone_core_get_proxy_config_from_rid(lc,ev->rid);
			/* if contact is up to date, process the failure, otherwise resend a new register with
				updated contact first, just in case the faillure is due to incorrect contact */
			if (lc->pProxy->linphone_proxy_config_register_again_with_updated_contact(cfg,ev->request,ev->response))
				return; /*we are retrying with an updated contact*/
			if (status_code==403) lc->pProxy->linphone_proxy_config_process_authentication_failure(lc,ev);
			osip_uri_to_str(requri,&ru);
			msg=ortp_strdup_printf(("Registration on %s failed: %s"),ru,(reason!=NULL) ? reason : ("no response timeout"));
			lc->vtable->linphone_gtk_display_status(lc,msg);
			lc->pGeneralState->gstate_new_state(lc, GSTATE_REG_FAILED, msg);
			ms_free(msg);
			osip_free(ru);
	}
}

void CEvents::linphone_other_request(CLinphoneCore *lc, eXosip_event_t *ev){
	ms_message("in linphone_other_request");
	if (ev->request==NULL) return;
	if (strcmp(ev->request->sip_method,"MESSAGE")==0){
		lc->pPrivate->linphone_core_text_received(lc,ev);
		eXosip_message_send_answer(ev->tid,200,NULL);
	}else if (strcmp(ev->request->sip_method,"OPTIONS")==0){
		osip_message_t *options=NULL;
		eXosip_options_build_answer(ev->tid,200,&options);
		osip_message_set_allow(options,"INVITE, ACK, BYE, CANCEL, OPTIONS, MESSAGE, SUBSCRIBE, NOTIFY, INFO");
		osip_message_set_accept(options,"application/sdp");
		eXosip_options_send_answer(ev->tid,200,options);
	}else if (strcmp(ev->request->sip_method,"WAKEUP")==0
		&& comes_from_local_if(ev->request)) {
		eXosip_message_send_answer(ev->tid,200,NULL);
		ms_message("Receiving WAKEUP request !");

		lc->vtable->linphone_gtk_show(lc);
	}else if (strncmp(ev->request->sip_method, "REFER", 5) == 0){
		ms_message("Receiving REFER request !");
		if (comes_from_local_if(ev->request)) {
			osip_header_t *h=NULL;
			osip_message_header_get_byname(ev->request,"Refer-To",0,&h);
			eXosip_message_send_answer(ev->tid,200,NULL);
			if (h){

					lc->vtable->linphone_gtk_refer_received(lc,h->hvalue);
			}
			
		}else OutputDebugString(_T("Ignored REFER not coming from this local loopback interface."));
	}
    	else {
		char *tmp=NULL;
		size_t msglen=0;
		osip_message_to_str(ev->request,&tmp,&msglen);
		if (tmp){
			ms_message("Unsupported request received:\n%s",tmp);
			osip_free(tmp);
		}
		/*answer with a 501 Not implemented*/
		eXosip_message_send_answer(ev->tid,501,NULL);
	}
}


bool_t CEvents::comes_from_local_if(osip_message_t *msg){
	osip_via_t *via=NULL;
	osip_message_get_via(msg,0,&via);
	if (via){
		const char *host;
		host=osip_via_get_host(via);
		if (strcmp(host,"127.0.0.1")==0 || strcmp(host,"::1")==0){
			osip_generic_param_t *param=NULL;
			osip_via_param_get_byname(via,"received",&param);
			if (param==NULL) return TRUE;
			if (param->gvalue && 
				(strcmp(param->gvalue,"127.0.0.1")==0 || strcmp(param->gvalue,"::1")==0)){
				return TRUE;
			}
		}
	}
	return FALSE;
}

int CEvents::linphone_set_video_offer(sdp_context_t *ctx)
{
	LinphoneCall *call=(LinphoneCall*)sdp_context_get_user_pointer(ctx);
	CLinphoneCore *lc=call->core;
	PayloadType *codec;
	MSList *elem;
	bool_t firsttime=TRUE;	

	if (!lc->linphone_core_video_enabled()) return -1;

	for(elem=lc->codecs_conf.video_codecs;elem!=NULL;elem=ms_list_next(elem)){
		codec=(PayloadType*) elem->data;
		if (lc->pMisc->linphone_core_check_payload_type_usability(lc,codec) && lc->pMisc->payload_type_enabled(codec)){
			sdp_payload_t payload;
			lc->pSdpHandler->sdp_payload_init(&payload);
			payload.line=1;
			payload.a_rtpmap=ortp_strdup_printf("%s/%i",codec->mime_type,codec->clock_rate);
			payload.localport=call->video_params.natd_port>0 ? 
					call->video_params.natd_port : lc->rtp_conf.video_rtp_port;
			payload.pt=find_payload_type_number(lc->local_profile,codec);
			payload.a_fmtp=codec->recv_fmtp;
			if(firsttime){
				firsttime=FALSE;
				if (lc->dw_video_bw>0)
					payload.b_as_bandwidth=lc->dw_video_bw;
			}
			lc->pSdpHandler->sdp_context_add_video_payload(ctx,&payload);
			ms_free(payload.a_rtpmap);
		}
	}
	return 0;
}
int CEvents::linphone_accept_video_offer(sdp_context_t *ctx,sdp_payload_t *payload)
{
	LinphoneCall *call=(LinphoneCall*)sdp_context_get_user_pointer(ctx);
	CLinphoneCore *lc=call->core;
	RtpProfile *remote_profile;
	StreamParams *params;
	SupportLevel supported;
	PayloadType *lpt=NULL;

	if (!lc->linphone_core_video_enabled()) return -1;

	if (payload->remoteport==0) {
		ms_message("Video stream refused by remote.");
		return 0;
	}

	params=&call->video_params;
	remote_profile=call->profile;
	/* see if this codec is supported in our local rtp profile*/
	supported=linphone_payload_is_supported(lc,payload,lc->local_profile,remote_profile,TRUE,&lpt);
	if (supported==Unsupported) {
		ms_message("Refusing video codec %i (%s)",payload->pt,payload->a_rtpmap);
		return -1;
	}
	if (lc->sip_conf.only_one_codec && params->initialized){
		return -1;
	}
	if (supported==SupportedAndValid){
		if (params->initialized==0){
			/* this is the first codec we may accept*/
			params->localport=lc->rtp_conf.video_rtp_port;
			payload->localport=params->natd_port>0 ? params->natd_port : lc->rtp_conf.video_rtp_port;
			params->line=payload->line;
			params->pt=payload->pt; /* remember the first payload accepted */
			if (payload->relay_host!=NULL){
				strncpy(params->remoteaddr,payload->relay_host,sizeof(params->remoteaddr)-1);
				params->remoteport=payload->relay_port;
				params->remotertcpport=payload->relay_port;
				params->relay_session_id=payload->relay_session_id;
			}else{
				strncpy(params->remoteaddr,payload->c_addr,sizeof(params->remoteaddr)-1);
				params->remoteport=payload->remoteport;
				params->remotertcpport=params->remoteport+1;
			}
			params->initialized=1;
			payload->b_as_bandwidth=(lc->dw_video_bw>0) ? lc->dw_video_bw : 0;
		}else{
			/* refuse all other video lines*/
			if(params->line!=payload->line) return -1;
		}
	}
	return 0;
}


int CEvents::linphone_read_video_answer(sdp_context_t *ctx,sdp_payload_t *payload)
{
	LinphoneCall *call=(LinphoneCall*)sdp_context_get_user_pointer(ctx);
	CLinphoneCore *lc=call->core;
	StreamParams *params;
	SupportLevel supported;
	PayloadType *lpt=NULL;
	
	/* paranoid check: see if this codec is supported in our local rtp profile*/
	supported=linphone_payload_is_supported(lc, payload,lc->local_profile,call->profile,FALSE,&lpt);
	if (supported==Unsupported) {
		ms_warning("This remote sip phone did not answer properly to my sdp offer: rtpmap=%s",payload->a_rtpmap);
		return 0;
	}
	if (supported==SupportedAndValid){
		params=&call->video_params;
		if (params->initialized==0){
			/* this is the first codec we may accept*/
			params->localport=lc->rtp_conf.video_rtp_port;
			params->line=payload->line;
			params->pt=payload->pt; /* remember the first payload accepted */
			if (payload->relay_host!=NULL){
				strncpy(params->remoteaddr,payload->relay_host,sizeof(params->remoteaddr)-1);
				params->remoteport=payload->relay_port;
				params->remotertcpport=payload->relay_port;
				params->relay_session_id=payload->relay_session_id;
			}else{
				strncpy(params->remoteaddr,payload->c_addr,sizeof(params->remoteaddr)-1);
				params->remoteport=payload->remoteport;
				params->remotertcpport=payload->remoteport+1;
			}
			params->initialized=1;
		}
	}
	return 0;
}