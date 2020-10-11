#include "StdAfx.h"
#include "Presence.h"
#include "friend.h"
#include "private.h"
#include "misc.h"
#include "staticManager.h"
CPresence::CPresence(void)
{
}

CPresence::~CPresence(void)
{
}

void CPresence::linphone_core_add_subscriber(CLinphoneCore *lc, const char *subscriber, int did, int nid){
	LinphoneFriend *fl=lc->pFriend->linphone_friend_new_with_addr(subscriber);
	if (fl==NULL) return ;
	fl->in_did=did;
	lc->pFriend->linphone_friend_set_nid(fl,nid);
	lc->pFriend->linphone_friend_set_inc_subscribe_policy(fl,LinphoneSPAccept);
	fl->inc_subscribe_pending=TRUE;
	lc->subscribers=ms_list_append(lc->subscribers,(void *)fl);

		char *clean_subscriber;	/* we need to remove tags...*/
		lc->pMisc->from_2char_without_params(fl->url,&clean_subscriber);
		lc->vtable->linphone_gtk_new_unknown_subscriber(lc,fl,clean_subscriber);
		ms_free(clean_subscriber);
	
}

void CPresence::linphone_core_reject_subscriber(CLinphoneCore *lc, LinphoneFriend *lf){
	lc->pFriend->linphone_friend_set_inc_subscribe_policy(lf,LinphoneSPDeny);
}

void CPresence:: __do_notify(void * data, void * user_data){
	int *tab=(int*)user_data;
	LinphoneFriend *lf=(LinphoneFriend*)data;
	lf->lc->pFriend->linphone_friend_notify(lf,tab[0],(LinphoneOnlineStatus)tab[1]);
}

void CPresence::__linphone_core_notify_all_friends(CLinphoneCore *lc, int ss, int os){
	int tab[2];
	tab[0]=ss;
	tab[1]=os;
	ms_list_for_each2(lc->friends,CPresence::__do_notify,(void *)tab);
}

void CPresence::linphone_core_notify_all_friends(CLinphoneCore *lc, LinphoneOnlineStatus os){
	ms_message("Notifying all friends that we are in status %i",os);
	__linphone_core_notify_all_friends(lc,EXOSIP_SUBCRSTATE_ACTIVE,os);
}

/* check presence state before answering to call; returns TRUE if we can proceed, else answer the appropriate answer
to close the dialog*/
bool_t CPresence::linphone_core_check_presence(CLinphoneCore *lc){
	return TRUE;
}

void CPresence::linphone_subscription_new(CLinphoneCore *lc, eXosip_event_t *ev){
	LinphoneFriend *lf=NULL;
	osip_from_t *from=ev->request->from;
	char *tmp;
	osip_message_t *msg=NULL;

	osip_from_to_str(ev->request->from,&tmp);

	ms_message("Receiving new subscription from %s.",tmp);
	/* check if we answer to this subscription */
	if (lc->pFriend->find_friend(lc->friends,from,&lf)!=NULL){
		lf->in_did=ev->did;
		lc->pFriend->linphone_friend_set_nid(lf,ev->nid);
		eXosip_insubscription_build_answer(ev->tid,202,&msg);
		eXosip_insubscription_send_answer(ev->tid,202,msg);
		__eXosip_wakeup_event();
		lc->pFriend->linphone_friend_done(lf);	/*this will do all necessary actions */
	}else{
		/* check if this subscriber is in our black list */
		if (lc->pFriend->find_friend(lc->subscribers,from,&lf)){
			if (lf->pol==LinphoneSPDeny){
				ms_message("Rejecting %s because we already rejected it once.",from);
				eXosip_insubscription_send_answer(ev->tid,401,NULL);
			}
			else {
				/* else it is in wait for approval state, because otherwise it is in the friend list.*/
				ms_message("New subscriber found in friend list, in %s state.",lc->pFriend->__policy_enum_to_str(lf->pol));
			}
		}else {
			eXosip_insubscription_build_answer(ev->tid,202,&msg);
			eXosip_insubscription_send_answer(ev->tid,202,msg);
			linphone_core_add_subscriber(lc,tmp,ev->did,ev->nid);
		}
	}
	osip_free(tmp);
}

void CPresence::linphone_notify_recv(CLinphoneCore *lc, eXosip_event_t *ev)
{
	const char *status=("Gone");
	const char *img="sip-closed.png";
	char *tmp;
	LinphoneFriend *lf;
	osip_from_t *friend1=NULL;
	osip_from_t *from=NULL;
	osip_body_t *body=NULL;
	LinphoneOnlineStatus estatus=LINPHONE_STATUS_UNKNOWN;
	ms_message("Receiving notify with sid=%i,nid=%i",ev->sid,ev->nid);
	if (ev->request!=NULL){
		from=ev->request->from;
		osip_message_get_body(ev->request,0,&body);
		if (body==NULL){
			OutputDebugString(_T("No body in NOTIFY"));
			return;
		}
		if (strstr(body->body,"pending")!=NULL){
			status=("Waiting for Approval");
			img="sip-wfa.png";
			estatus=LINPHONE_STATUS_PENDING;
		}else if ((strstr(body->body,"online")!=NULL) || (strstr(body->body,"open")!=NULL)) {
			status=("Online");
			img="sip-online.png";
			estatus=LINPHONE_STATUS_ONLINE;
		}else if (strstr(body->body,"busy")!=NULL){
			status=("Busy");
			img="sip-busy.png";
			estatus=LINPHONE_STATUS_BUSY;
		}else if (strstr(body->body,"berightback")!=NULL
				|| strstr(body->body,"in-transit")!=NULL ){
			status=("Be Right Back");
			img="sip-bifm.png";
			estatus=LINPHONE_STATUS_BERIGHTBACK;
		}else if (strstr(body->body,"away")!=NULL){
			status=("Away");
			img="sip-away.png";
			estatus=LINPHONE_STATUS_AWAY;
		}else if (strstr(body->body,"onthephone")!=NULL
			|| strstr(body->body,"on-the-phone")!=NULL){
			status=("On The Phone");
			img="sip-otp.png";
			estatus=LINPHONE_STATUS_ONTHEPHONE;
		}else if (strstr(body->body,"outtolunch")!=NULL
				|| strstr(body->body,"meal")!=NULL){
			status=("Out To Lunch");
			img="sip-otl.png";
			estatus=LINPHONE_STATUS_OUTTOLUNCH;
		}else if (strstr(body->body,"closed")!=NULL){
			status=("Closed");
			img="sip-away.png";
			estatus=LINPHONE_STATUS_CLOSED;
		}else{
			status=("Gone");
			img="sip-closed.png";
			estatus=LINPHONE_STATUS_OFFLINE;
		}
		ms_message("We are notified that sip:%s@%s has online status %s",from->url->username,from->url->host,status);
	}
	lf=lc->pFriend->linphone_find_friend_by_sid(lc->friends,ev->sid);
	if (lf!=NULL){
		friend1=lf->url;
		lc->pMisc->from_2char_without_params(friend1,&tmp);
		lf->status=estatus;
		lc->vtable->linphone_gtk_notify_recv(lc,(LinphoneFriend*)lf,tmp,status,img);
		ms_free(tmp);
		if (ev->ss_status==EXOSIP_SUBCRSTATE_TERMINATED) {
			lf->sid=-1;
			lf->out_did=-1;
			ms_message("Outgoing subscription terminated by remote.");
		}
	}else{
		ms_message("But this person is not part of our friend list, so we don't care.");
	}
}

void CPresence::linphone_subscription_answered(CLinphoneCore *lc, eXosip_event_t *ev){
	LinphoneFriend *lf;
	osip_from_t *from=ev->response->to;
	lc->pFriend->find_friend(lc->friends,from,&lf);
	if (lf!=NULL){
		lf->out_did=ev->did;
		lc->pFriend->linphone_friend_set_sid(lf,ev->sid);
	}else{
		OutputDebugString(_T("Receiving answer for unknown subscribe sip:%s@%s"));
	}
}
void CPresence::linphone_subscription_closed(CLinphoneCore *lc,eXosip_event_t *ev){
	LinphoneFriend *lf;
	osip_from_t *from=ev->request->from;
	lf=lc->pFriend->linphone_find_friend_by_nid(lc->friends,ev->nid);
	if (lf!=NULL){
		lf->in_did=-1;
		lc->pFriend->linphone_friend_set_nid(lf,-1);
	}else{
		OutputDebugString(_T("Receiving unsuscribe for unknown in-subscribtion from sip:%s@%s"));
	}
}
