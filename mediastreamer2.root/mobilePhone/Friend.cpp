#include "StdAfx.h"
#include "Friend.h"
#include <eXosip2/eXosip.h>
#include <osipparser2/osip_message.h>
#include "staticManager.h"
#include "ProxyConfig.h"
#include "configure.h"
CFriend::CFriend(void)
{
}

CFriend::~CFriend(void)
{
}


const char *CFriend::linphone_online_status_to_string(LinphoneOnlineStatus ss){
	const char *str=NULL;
	switch(ss){
		case LINPHONE_STATUS_UNKNOWN:
			str=("Unknown");
			break;
		case LINPHONE_STATUS_ONLINE:
			str=("Online");
			break;
		case LINPHONE_STATUS_BUSY:
			str=("Busy");
			break;
		case LINPHONE_STATUS_BERIGHTBACK:
			str=("Be right back");
			break;
		case LINPHONE_STATUS_AWAY:
			str=("Away");
			break;
		case LINPHONE_STATUS_ONTHEPHONE:
			str=("On the phone");
			break;
		case LINPHONE_STATUS_OUTTOLUNCH:
			str=("Out to lunch");
			break;
		case LINPHONE_STATUS_NOT_DISTURB:
			str=("Do not disturb");
			break;
		case LINPHONE_STATUS_MOVED:
			str=("Moved");
			break;
		case LINPHONE_STATUS_ALT_SERVICE:
			str=("Using another messaging service");
			break;
		case LINPHONE_STATUS_OFFLINE:
			str=("Offline");
			break;
		case LINPHONE_STATUS_PENDING:
			str=("Pending");
			break;
		case LINPHONE_STATUS_CLOSED:
			str=("Closed");
			break;
		default:
			str=("Unknown-bug");
	}
	return str;
}

 int CFriend::friend_data_compare(const void * a, const void * b, void * data){
	osip_from_t *fa=((LinphoneFriend*)a)->url;
	osip_from_t *fb=((LinphoneFriend*)b)->url;
	char *ua,*ub;
	ua=fa->url->username;
	ub=fb->url->username;
	if (ua!=NULL && ub!=NULL) {
		//printf("Comparing usernames %s,%s\n",ua,ub);
		return strcasecmp(ua,ub);
	}
	else {
		/* compare hosts*/
		ua=fa->url->host;
		ub=fb->url->host;
		if (ua!=NULL && ub!=NULL){
			int ret=strcasecmp(ua,ub);
			//printf("Comparing hostnames %s,%s,res=%i\n",ua,ub,ret);
			return ret;
		}
		else return -1;
	}
}

 int CFriend::friend_compare(const void * a, const void * b){
	return friend_data_compare(a,b,NULL);
}


MSList *CFriend::find_friend(MSList *fl, const osip_from_t *friend1, LinphoneFriend **lf){
	MSList *res=NULL;
	LinphoneFriend dummy;
	if (lf!=NULL) *lf=NULL;
	dummy.url=(osip_from_t*)friend1;
	res=ms_list_find_custom(fl,CFriend::friend_compare,&dummy);
	if (lf!=NULL && res!=NULL) *lf=(LinphoneFriend*)res->data;
	return res;
}

LinphoneFriend *CFriend::linphone_find_friend_by_nid(MSList *l, int nid){
	MSList *elem;
	for (elem=l;elem!=NULL;elem=elem->next){
		LinphoneFriend *lf=(LinphoneFriend*)elem->data;
		if (lf->nid==nid) return lf;
	}
	return NULL;
}

LinphoneFriend *CFriend::linphone_find_friend_by_sid(MSList *l, int sid){
	MSList *elem;
	for (elem=l;elem!=NULL;elem=elem->next){
		LinphoneFriend *lf=(LinphoneFriend*)elem->data;
		if (lf->sid==sid) return lf;
	}
	return NULL;
}

void CFriend::__linphone_friend_do_subscribe(LinphoneFriend *fr){
	char *friend1=NULL;
	const char *route=NULL;
	const char *from=NULL;
	osip_message_t *msg=NULL;
	osip_from_to_str(fr->url,&friend1);
	if (fr->proxy!=NULL){
		route=fr->proxy->reg_route;
		from=fr->proxy->reg_identity;
	}else from=fr->lc->linphone_core_get_primary_contact();
	if (fr->sid<0){
		/* people for which we don't have yet an answer should appear as offline */
		fr->lc->vtable->linphone_gtk_notify_recv(fr->lc,(LinphoneFriend*)fr,friend1,("Gone"),"sip-closed.png");
	}
	eXosip_lock();
	eXosip_subscribe_build_initial_request(&msg,friend1,from,route,"presence",600);
	eXosip_subscribe_send_initial_request(msg);
	eXosip_unlock();
	osip_free(friend1);
}


LinphoneFriend * CFriend::linphone_friend_new(){
	LinphoneFriend *obj=(LinphoneFriend *)ms_new0(LinphoneFriend,1);
	obj->out_did=-1;
	obj->in_did=-1;
	obj->nid=-1;
	obj->sid=-1;
	obj->pol=LinphoneSPAccept;
	obj->status=LINPHONE_STATUS_OFFLINE;
	obj->subscribe=TRUE;
	return obj;	
}

LinphoneFriend *CFriend::linphone_friend_new_with_addr(const char *addr){
	LinphoneFriend *fr=linphone_friend_new();
	if (linphone_friend_set_sip_addr(fr,addr)<0){
		linphone_friend_destroy(fr);
		return NULL;
	}
	return fr;
}


void CFriend::linphone_core_interpret_friend_uri(CLinphoneCore *lc, const char *uri, char **result){
	int err;
	osip_from_t *fr=NULL;
	osip_from_init(&fr);
	err=osip_from_parse(fr,uri);
	if (err<0){
		char *tmp=NULL;
		if (strchr(uri,'@')!=NULL){
			/*try adding sip:*/
			tmp=ms_strdup_printf("sip:%s",uri);
		}else if (lc->default_proxy!=NULL){
			/*try adding domain part from default current proxy*/
			osip_from_t *id=NULL;
			osip_from_init(&id);
			if (osip_from_parse(id,lc->linphone_core_get_identity())==0){
				if (id->url->port!=NULL && strlen(id->url->port)>0)
					tmp=ms_strdup_printf("sip:%s@%s:%s",uri,id->url->host,id->url->port);
				else tmp=ms_strdup_printf("sip:%s@%s",uri,id->url->host);
			}
			osip_from_free(id);
		}
		if (osip_from_parse(fr,tmp)==0){
			/*looks good */
			ms_message("%s interpreted as %s",uri,tmp);
			*result=tmp;
		}else *result=NULL;
	}else *result=ms_strdup(uri);
	osip_from_free(fr);
}

int CFriend::linphone_friend_set_sip_addr(LinphoneFriend *lf, const char *addr){
	int err;
	osip_from_t *fr=NULL;
	osip_from_init(&fr);
	err=osip_from_parse(fr,addr);
	if (err<0) {
		OutputDebugString(_T("Invalid friend sip uri: %s"));
		osip_from_free(fr);
		return -1;
	}
	if (lf->url!=NULL) osip_from_free(lf->url);	
	lf->url=fr;
	return 0;
}

int CFriend::linphone_friend_set_name(LinphoneFriend *lf, const char *name){
	osip_from_t *fr=lf->url;
	if (fr==NULL){
		OutputDebugString(_T("linphone_friend_set_sip_addr() must be called before linphone_friend_set_name()."));
		return -1;
	}
	if (fr->displayname!=NULL){
		osip_free(fr->displayname);
		fr->displayname=NULL;
	}
	if (name && name[0]!='\0'){
		fr->displayname=osip_strdup(name);
	}
	return 0;
}

int CFriend::linphone_friend_send_subscribe(LinphoneFriend *fr, bool_t val){
	fr->subscribe=val;
	return 0;
}

int CFriend::linphone_friend_set_inc_subscribe_policy(LinphoneFriend *fr, LinphoneSubscribePolicy pol)
{
	fr->pol=pol;
	return 0;
}

int CFriend::linphone_friend_set_proxy(LinphoneFriend *fr, CProxyConfig *cfg){
	fr->proxy=cfg;
	return 0;
}

void CFriend::linphone_friend_set_sid(LinphoneFriend *lf, int sid){
	lf->sid=sid;
}
void CFriend::linphone_friend_set_nid(LinphoneFriend *lf, int nid){
	lf->nid=nid;
	lf->inc_subscribe_pending=TRUE;
}

void CFriend::add_presence_body(osip_message_t *notify, LinphoneOnlineStatus online_status)
{
	char buf[1000];
#ifdef SUPPORT_MSN
	int atom_id = 1000;
#endif
	char *contact_info;

	osip_contact_t *ct=NULL;
	osip_message_get_contact(notify,0,&ct);
	osip_contact_to_str(ct,&contact_info);

#ifdef SUPPORT_MSN

	if (online_status==LINPHONE_STATUS_ONLINE)
	{
		sprintf(buf, "<?xml version=\"1.0\"?>\n\
					 <!DOCTYPE presence\n\
					 PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">\n\
					 <presence>\n\
					 <presentity uri=\"%s;method=SUBSCRIBE\" />\n\
					 <atom id=\"%i\">\n\
					 <address uri=\"%s;user=ip\" priority=\"0.800000\">\n\
					 <status status=\"open\" />\n\
					 <msnsubstatus substatus=\"online\" />\n\
					 </address>\n\
					 </atom>\n\
					 </presence>", contact_info, atom_id, contact_info);

	}
	else if (online_status==LINPHONE_STATUS_BUSY)
	{
		sprintf(buf, "<?xml version=\"1.0\"?>\n\
					 <!DOCTYPE presence\n\
					 PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">\n\
					 <presence>\n\
					 <presentity uri=\"%s;method=SUBSCRIBE\" />\n\
					 <atom id=\"%i\">\n\
					 <address uri=\"%s;user=ip\" priority=\"0.800000\">\n\
					 <status status=\"inuse\" />\n\
					 <msnsubstatus substatus=\"busy\" />\n\
					 </address>\n\
					 </atom>\n\
					 </presence>", contact_info, atom_id, contact_info);

	}
	else if (online_status==LINPHONE_STATUS_BERIGHTBACK)
	{
		sprintf(buf, "<?xml version=\"1.0\"?>\n\
					 <!DOCTYPE presence\n\
					 PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">\n\
					 <presence>\n\
					 <presentity uri=\"%s;method=SUBSCRIBE\" />\n\
					 <atom id=\"%i\">\n\
					 <address uri=\"%s;user=ip\" priority=\"0.800000\">\n\
					 <status status=\"inactive\" />\n\
					 <msnsubstatus substatus=\"berightback\" />\n\
					 </address>\n\
					 </atom>\n\
					 </presence>", contact_info, atom_id, contact_info);

	}
	else if (online_status==LINPHONE_STATUS_AWAY)
	{
		sprintf(buf, "<?xml version=\"1.0\"?>\n\
					 <!DOCTYPE presence\n\
					 PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">\n\
					 <presence>\n\
					 <presentity uri=\"%s;method=SUBSCRIBE\" />\n\
					 <atom id=\"%i\">\n\
					 <address uri=\"%s;user=ip\" priority=\"0.800000\">\n\
					 <status status=\"inactive\" />\n\
					 <msnsubstatus substatus=\"away\" />\n\
					 </address>\n\
					 </atom>\n\
					 </presence>", contact_info, atom_id, contact_info);

	}
	else if (online_status==LINPHONE_STATUS_ONTHEPHONE)
	{
		sprintf(buf, "<?xml version=\"1.0\"?>\n\
					 <!DOCTYPE presence\n\
					 PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">\n\
					 <presence>\n\
					 <presentity uri=\"%s;method=SUBSCRIBE\" />\n\
					 <atom id=\"%i\">\n\
					 <address uri=\"%s;user=ip\" priority=\"0.800000\">\n\
					 <status status=\"inuse\" />\n\
					 <msnsubstatus substatus=\"onthephone\" />\n\
					 </address>\n\
					 </atom>\n\
					 </presence>", contact_info, atom_id, contact_info);

	}
	else if (online_status==LINPHONE_STATUS_OUTTOLUNCH)
	{
		sprintf(buf, "<?xml version=\"1.0\"?>\n\
					 <!DOCTYPE presence\n\
					 PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">\n\
					 <presence>\n\
					 <presentity uri=\"%s;method=SUBSCRIBE\" />\n\
					 <atom id=\"%i\">\n\
					 <address uri=\"%s;user=ip\" priority=\"0.800000\">\n\
					 <status status=\"inactive\" />\n\
					 <msnsubstatus substatus=\"outtolunch\" />\n\
					 </address>\n\
					 </atom>\n\
					 </presence>", contact_info, atom_id, contact_info);

	}
	else
	{
		sprintf(buf, "<?xml version=\"1.0\"?>\n\
					 <!DOCTYPE presence\n\
					 PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">\n\
					 <presence>\n\
					 <presentity uri=\"%s;method=SUBSCRIBE\" />\n\
					 <atom id=\"%i\">\n\
					 <address uri=\"%s;user=ip\" priority=\"0.800000\">\n\
					 <status status=\"inactive\" />\n\
					 <msnsubstatus substatus=\"away\" />\n\
					 </address>\n\
					 </atom>\n\
					 </presence>", contact_info, atom_id, contact_info);
	}

	osip_message_set_body(notify, buf, strlen(buf));
	osip_message_set_content_type(notify, "application/xpidf+xml");
#else

	if (online_status==LINPHONE_STATUS_ONLINE)
	{
		sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
					   <presence xmlns=\"urn:ietf:params:xml:ns:pidf\"\n\
					   entity=\"%s\">\n\
					   <tuple id=\"sg89ae\">\n\
					   <status>\n\
					   <basic>open</basic>\n\
					   </status>\n\
					   <contact priority=\"0.8\">%s</contact>\n\
					   <note>online</note>\n\
					   </tuple>\n\
					   </presence>",
					   contact_info, contact_info);
	}
	else if (online_status==LINPHONE_STATUS_BUSY)
	{
		sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
					 <presence xmlns=\"urn:ietf:params:xml:ns:pidf\"\n\
					 xmlns:es=\"urn:ietf:params:xml:ns:pidf:status:rpid-status\"\n\
					 entity=\"%s\">\n\
					 <tuple id=\"sg89ae\">\n\
					 <status>\n\
					 <basic>open</basic>\n\
					 <es:activities>\n\
					 <es:activity>busy</es:activity>\n\
					 </es:activities>\n\
					 </status>\n\
					 <contact priority=\"0.8\">%s</contact>\n\
					 <note>busy</note>\n\
					 </tuple>\n\
					 </presence>",
					 contact_info, contact_info);
	}
	else if (online_status==LINPHONE_STATUS_BERIGHTBACK)
	{
		sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
					 <presence xmlns=\"urn:ietf:params:xml:ns:pidf\"\n\
					 xmlns:es=\"urn:ietf:params:xml:ns:pidf:status:rpid-status\"\n\
					 entity=\"%s\">\n\
					 <tuple id=\"sg89ae\">\n\
					 <status>\n\
					 <basic>open</basic>\n\
					 <es:activities>\n\
					 <es:activity>in-transit</es:activity>\n\
					 </es:activities>\n\
					 </status>\n\
					 <contact priority=\"0.8\">%s</contact>\n\
					 <note>be right back</note>\n\
					 </tuple>\n\
					 </presence>",
					 contact_info, contact_info);
	}
	else if (online_status==LINPHONE_STATUS_AWAY)
	{
		sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
					 <presence xmlns=\"urn:ietf:params:xml:ns:pidf\"\n\
					 xmlns:es=\"urn:ietf:params:xml:ns:pidf:status:rpid-status\"\n\
					 entity=\"%s\">\n\
					 <tuple id=\"sg89ae\">\n\
					 <status>\n\
					 <basic>open</basic>\n\
					 <es:activities>\n\
					 <es:activity>away</es:activity>\n\
					 </es:activities>\n\
					 </status>\n\
					 <contact priority=\"0.8\">%s</contact>\n\
					 <note>away</note>\n\
					 </tuple>\n\
					 </presence>",
					 contact_info, contact_info);
	}
	else if (online_status==LINPHONE_STATUS_ONTHEPHONE)
	{
		sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
					 <presence xmlns=\"urn:ietf:params:xml:ns:pidf\"\n\
					 xmlns:es=\"urn:ietf:params:xml:ns:pidf:status:rpid-status\"\n\
					 entity=\"%s\">\n\
					 <tuple id=\"sg89ae\">\n\
					 <status>\n\
					 <basic>open</basic>\n\
					 <es:activities>\n\
					 <es:activity>on-the-phone</es:activity>\n\
					 </es:activities>\n\
					 </status>\n\
					 <contact priority=\"0.8\">%s</contact>\n\
					 <note>on the phone</note>\n\
					 </tuple>\n\
					 </presence>",
					 contact_info, contact_info);
	}
	else if (online_status==LINPHONE_STATUS_OUTTOLUNCH)
	{
		sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
					 <presence xmlns=\"urn:ietf:params:xml:ns:pidf\"\n\
					 xmlns:es=\"urn:ietf:params:xml:ns:pidf:status:rpid-status\"\n\
					 entity=\"%s\">\n\
					 <tuple id=\"sg89ae\">\n\
					 <status>\n\
					 <basic>open</basic>\n\
					 <es:activities>\n\
					 <es:activity>meal</es:activity>\n\
					 </es:activities>\n\
					 </status>\n\
					 <contact priority=\"0.8\">%s</contact>\n\
					 <note>out to lunch</note>\n\
					 </tuple>\n\
					 </presence>",
					 contact_info, contact_info);
	}
	else
	{
		/* */
		sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
					 <presence xmlns=\"urn:ietf:params:xml:ns:pidf\"\n\
					 xmlns:es=\"urn:ietf:params:xml:ns:pidf:status:rpid-status\"\n\
					 entity=\"%s\">\n%s",
					 contact_info,
					 "<tuple id=\"sg89ae\">\n\
					 <status>\n\
					 <basic>closed</basic>\n\
					 <es:activities>\n\
					 <es:activity>permanent-absence</e:activity>\n\
					 </es:activities>\n\
					 </status>\n\
					 </tuple>\n\
					 \n</presence>\n");
	}
	osip_message_set_body(notify, buf, strlen(buf));
	osip_message_set_content_type(notify, "application/pidf+xml");

#endif
	osip_free(contact_info);
}


void CFriend::linphone_friend_notify(LinphoneFriend *lf, int ss, LinphoneOnlineStatus os){
	//printf("Wish to notify %p, lf->nid=%i\n",lf,lf->nid);
	if (lf->in_did!=-1){
		osip_message_t *msg=NULL;
		const char *identity;
		if (lf->proxy!=NULL) identity=lf->proxy->reg_identity;
		else identity=lf->lc->linphone_core_get_primary_contact();
		eXosip_lock();
		eXosip_insubscription_build_notify(lf->in_did,ss,0,&msg);
		if (msg!=NULL){
			osip_message_set_contact(msg,identity);
			add_presence_body(msg,os);
			eXosip_insubscription_send_request(lf->in_did,msg);
		}else OutputDebugString(_T("could not create notify for incoming subscription."));
		eXosip_unlock();
	}
}

 void CFriend::linphone_friend_unsubscribe(LinphoneFriend *lf){
	if (lf->out_did!=-1) {
		osip_message_t *msg=NULL;
		eXosip_lock();
		eXosip_subscribe_build_refresh_request(lf->out_did,&msg);
		if (msg){
			osip_message_set_expires(msg,"0");
			eXosip_subscribe_send_refresh_request(lf->out_did,msg);
		}else OutputDebugString(_T("Could not build subscribe refresh request !"));
		eXosip_unlock();
	}
}

void CFriend::linphone_friend_destroy(LinphoneFriend *lf){
	linphone_friend_notify(lf,EXOSIP_SUBCRSTATE_TERMINATED,LINPHONE_STATUS_CLOSED);
	linphone_friend_unsubscribe(lf);
	if (lf->url!=NULL) osip_from_free(lf->url);
	ms_free(lf);
}

void CFriend::linphone_friend_check_for_removed_proxy(LinphoneFriend *lf, CProxyConfig *cfg){
	if (lf->proxy==cfg){
		lf->proxy=NULL;
	}
}

char *CFriend::linphone_friend_get_addr(LinphoneFriend *lf){
	char *ret,*tmp;
	if (lf->url==NULL) return NULL;
	osip_uri_to_str(lf->url->url,&tmp);
	ret=ms_strdup(tmp);
	osip_free(tmp);
	return ret;
}

char *CFriend::linphone_friend_get_name(LinphoneFriend *lf){
	if (lf->url==NULL) return NULL;
	if (lf->url->displayname==NULL) return NULL;
	return ms_strdup(lf->url->displayname);
}

char * CFriend::linphone_friend_get_url(LinphoneFriend *lf){
	char *tmp,*ret;
	if (lf->url==NULL) return NULL;
	osip_from_to_str(lf->url,&tmp);
	ret=ms_strdup(tmp);
	ms_free(tmp);
	return ret;
}

bool_t CFriend::linphone_friend_get_send_subscribe(const LinphoneFriend *lf){
	return lf->subscribe;
}

LinphoneSubscribePolicy CFriend::linphone_friend_get_inc_subscribe_policy(const LinphoneFriend *lf){
	return lf->pol;
}

LinphoneOnlineStatus CFriend::linphone_friend_get_status(const LinphoneFriend *lf){
	return lf->status;
}


void CFriend::linphone_friend_apply(LinphoneFriend *fr, CLinphoneCore *lc){
	if (fr->url==NULL) {
		OutputDebugString(_T("No sip url defined."));
		return;
	}
	fr->lc=lc;

	lc->pFriend->linphone_core_write_friends_config(lc);

	if (fr->inc_subscribe_pending){
		switch(fr->pol){
			case LinphoneSPWait:
				linphone_friend_notify(fr,EXOSIP_SUBCRSTATE_PENDING,LINPHONE_STATUS_PENDING);
				break;
			case LinphoneSPAccept:
				if (fr->lc!=NULL)
				{
					linphone_friend_notify(fr,EXOSIP_SUBCRSTATE_ACTIVE,fr->lc->presence_mode);
				}
				break;
			case LinphoneSPDeny:
				linphone_friend_notify(fr,EXOSIP_SUBCRSTATE_TERMINATED,LINPHONE_STATUS_CLOSED);
				break;
		}
		fr->inc_subscribe_pending=FALSE;
	}
	if (fr->subscribe && fr->out_did==-1){

		__linphone_friend_do_subscribe(fr);
	}
	ms_message("linphone_friend_apply() done.");
}

void CFriend::linphone_friend_edit(LinphoneFriend *fr){
}

void CFriend::linphone_friend_done(LinphoneFriend *fr){
	ms_return_if_fail(fr!=NULL);
	if (fr->lc==NULL) return;
	linphone_friend_apply(fr,fr->lc);
}

void CFriend::linphone_core_add_friend(CLinphoneCore *lc, LinphoneFriend *lf)
{
	ms_return_if_fail(lf->lc==NULL);
	ms_return_if_fail(lf->url!=NULL);
	lc->friends=ms_list_append(lc->friends,lf);
	linphone_friend_apply(lf,lc);
	return ;
}

void CFriend::linphone_core_remove_friend(CLinphoneCore *lc, LinphoneFriend* fl){
	MSList *el=ms_list_find(lc->friends,(void *)fl);
	if (el!=NULL){
		lc->friends=ms_list_remove_link(lc->friends,el);
		linphone_friend_destroy((LinphoneFriend*)el->data);
		lc->pFriend->linphone_core_write_friends_config(lc);
	}
}

#define key_compare(key, word) strcmp((key),(word))

LinphoneSubscribePolicy CFriend::__policy_str_to_enum(const char* pol){
	if (key_compare("accept",pol)==0){
		return LinphoneSPAccept;
	}
	if (key_compare("deny",pol)==0){
		return LinphoneSPDeny;
	}
	if (key_compare("wait",pol)==0){
		return LinphoneSPWait;
	}
	OutputDebugString(_T("Unrecognized subscribe policy: %s"));
	return LinphoneSPWait;
}

CProxyConfig *CFriend::__index_to_proxy(CLinphoneCore *lc, int index){
	if (index>=0) return (CProxyConfig*)ms_list_nth_data(lc->sip_conf.proxies,index);
	else return NULL;
}

LinphoneFriend * CFriend::linphone_friend_new_from_config_file(CLinphoneCore *lc, int index){
	const char *tmp;
	char item[50];
	int a;
	LinphoneFriend *lf;
	CConfigure *config=lc->pConfig;

	sprintf(item,"friend_%i",index);

	if (!lc->pConfig->lp_config_has_section(item)){
		return NULL;
	}

	tmp=lc->pConfig->lp_config_get_string(item,"url",NULL);
	if (tmp==NULL) {
		return NULL;
	}
	lf=(LinphoneFriend *)linphone_friend_new_with_addr(tmp);
	if (lf==NULL) {
		return NULL;
	}
	tmp=lc->pConfig->lp_config_get_string(item,"pol",NULL);
	if (tmp==NULL) linphone_friend_set_inc_subscribe_policy(lf,LinphoneSPWait);
	else{
		linphone_friend_set_inc_subscribe_policy(lf,__policy_str_to_enum(tmp));
	}
	a=lc->pConfig->lp_config_get_int(item,"subscribe",0);
	linphone_friend_send_subscribe(lf,a);

	a=lc->pConfig->lp_config_get_int(item,"proxy",-1);
	if (a!=-1) {
		linphone_friend_set_proxy(lf,__index_to_proxy(lc,a));
	}
	return lf;
}

const char *CFriend::__policy_enum_to_str(LinphoneSubscribePolicy pol){
	switch(pol){
		case LinphoneSPAccept:
			return "accept";
			break;
		case LinphoneSPDeny:
			return "deny";
			break;
		case LinphoneSPWait:
			return "wait";
			break;
	}
	OutputDebugString(_T("Invalid policy enum value."));
	return "wait";
}

void CFriend::linphone_friend_write_to_config_file(CConfigure *config, LinphoneFriend *lf, int index){
	char key[50];
	char *tmp;
	int a;

	sprintf(key,"friend_%i",index);

	if (lf==NULL){
		config->lp_config_clean_section(key);
		return;
	}
	if (lf->url!=NULL){
		osip_from_to_str(lf->url,&tmp);
		if (tmp==NULL) {
			return;
		}
		config->lp_config_set_string(key,"url",tmp);
		osip_free(tmp);
	}
	config->lp_config_set_string(key,"pol",__policy_enum_to_str(lf->pol));
	config->lp_config_set_int(key,"subscribe",lf->subscribe);
	if (lf->proxy!=NULL){
		a=ms_list_index(lf->lc->sip_conf.proxies,lf->proxy);
		config->lp_config_set_int(key,"proxy",a);
	}else config->lp_config_set_int(key,"proxy",-1);
}

void CFriend::linphone_core_write_friends_config(CLinphoneCore* lc)
{
	MSList *elem;
	int i;
	if (!lc->ready) return; /*dont write config when reading it !*/
	for (elem=lc->friends,i=0; elem!=NULL; elem=ms_list_next(elem),i++){
		linphone_friend_write_to_config_file(lc->pConfig,(LinphoneFriend*)elem->data,i);
	}
	linphone_friend_write_to_config_file(lc->pConfig,NULL,i);	/* set the end */
}
