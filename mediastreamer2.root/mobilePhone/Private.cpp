#include "StdAfx.h"
#include "Private.h"
#include <eXosip2/eXosip.h>
#include <osipparser2/osip_message.h>
#include "configure.h"
#include "misc.h"
#include "staticManager.h"
#include "GeneralState.h"
CPrivate::CPrivate(void)
{
}

CPrivate::~CPrivate(void)
{
}

LinphoneAuthInfo *CPrivate::linphone_auth_info_new(const char *username, const char *userid,
										 const char *passwd, const char *ha1,const char *realm)
{
	LinphoneAuthInfo *obj=(LinphoneAuthInfo *)ms_new0(LinphoneAuthInfo,1);
	if (username!=NULL && (strlen(username)>0) ) obj->username=ms_strdup(username);
	if (userid!=NULL && (strlen(userid)>0)) obj->userid=ms_strdup(userid);
	if (passwd!=NULL && (strlen(passwd)>0)) obj->passwd=ms_strdup(passwd);
	if (ha1!=NULL && (strlen(ha1)>0)) obj->ha1=ms_strdup(ha1);
	if (realm!=NULL && (strlen(realm)>0)) obj->realm=ms_strdup(realm);
	obj->works=FALSE;
	obj->first_time=TRUE;
	return obj;
}

void CPrivate::linphone_auth_info_set_passwd(LinphoneAuthInfo *info, const char *passwd){
	if (info->passwd!=NULL) {
		ms_free(info->passwd);
		info->passwd=NULL;
	}
	if (passwd!=NULL && (strlen(passwd)>0)) info->passwd=ms_strdup(passwd);
}

void CPrivate::linphone_auth_info_set_username(LinphoneAuthInfo *info, const char *username){
	if (info->username){
		ms_free(info->username);
		info->username=NULL;
	}
	if (username && strlen(username)>0) info->username=ms_strdup(username);
}

void CPrivate::linphone_auth_info_destroy(LinphoneAuthInfo *obj){
	if (obj->username!=NULL) ms_free(obj->username);
	if (obj->userid!=NULL) ms_free(obj->userid);
	if (obj->passwd!=NULL) ms_free(obj->passwd);
	if (obj->ha1!=NULL) ms_free(obj->ha1);
	if (obj->realm!=NULL) ms_free(obj->realm);
	ms_free(obj);
}

void CPrivate::linphone_auth_info_write_config(CConfigure *config, LinphoneAuthInfo *obj, int pos)
{
	char key[50];
	sprintf(key,"auth_info_%i",pos);
	config->lp_config_clean_section(key);

	if (obj==NULL){
		return;
	}		
	if (obj->username!=NULL){
		config->lp_config_set_string(key,"username",obj->username);
	}
	if (obj->userid!=NULL){
		config->lp_config_set_string(key,"userid",obj->userid);
	}
	if (obj->passwd!=NULL){
		config->lp_config_set_string(key,"passwd",obj->passwd);
	}
	if (obj->ha1!=NULL){
		config->lp_config_set_string(key,"ha1",obj->ha1);
	}
	if (obj->realm!=NULL){
		config->lp_config_set_string(key,"realm",obj->realm);
	}
}

LinphoneAuthInfo *CPrivate::linphone_auth_info_new_from_config_file(CConfigure * config, int pos)
{
	char key[50];
	const char *username,*userid,*passwd,*ha1,*realm;

	sprintf(key,"auth_info_%i",pos);
	if (!config->lp_config_has_section(key)){
		return NULL;
	}

	username=config->lp_config_get_string(key,"username",NULL);
	userid=config->lp_config_get_string(key,"userid",NULL);
	passwd=config->lp_config_get_string(key,"passwd",NULL);
	ha1=config->lp_config_get_string(key,"ha1",NULL);
	realm=config->lp_config_get_string(key,"realm",NULL);
	return linphone_auth_info_new(username,userid,passwd,ha1,realm);
}
bool_t CPrivate::key_match(const char *tmp1, const char *tmp2){
	if (tmp1==NULL && tmp2==NULL) return TRUE;
	if (tmp1!=NULL && tmp2!=NULL && strcmp(tmp1,tmp2)==0) return TRUE;
	return FALSE;

}

char * CPrivate::remove_quotes(char * input){
	char *tmp;
	if (*input=='"') input++;
	tmp=strchr(input,'"');
	if (tmp) *tmp='\0';
	return input;
}

int CPrivate::realm_match(const char *realm1, const char *realm2){
	if (realm1==NULL && realm2==NULL) return TRUE;
	if (realm1!=NULL && realm2!=NULL){
		if (strcmp(realm1,realm2)==0) return TRUE;
		else{
			char tmp1[128];
			char tmp2[128];
			char *p1,*p2;
			strncpy(tmp1,realm1,sizeof(tmp1)-1);
			strncpy(tmp2,realm2,sizeof(tmp2)-1);
			p1=remove_quotes(tmp1);
			p2=remove_quotes(tmp2);
			return strcmp(p1,p2)==0;
		}
	}
	return FALSE;
}

int CPrivate::auth_info_compare(const void *pinfo,const void *pref){
	LinphoneAuthInfo *info=(LinphoneAuthInfo*)pinfo;
	LinphoneAuthInfo *ref=(LinphoneAuthInfo*)pref;
	if (realm_match(info->realm,ref->realm) && key_match(info->username,ref->username)) return 0;
	return -1;
}

int CPrivate::auth_info_compare_only_realm(const void *pinfo,const void *pref){
	LinphoneAuthInfo *info=(LinphoneAuthInfo*)pinfo;
	LinphoneAuthInfo *ref=(LinphoneAuthInfo*)pref;
	if (realm_match(info->realm,ref->realm) ) return 0;
	return -1;
}


LinphoneAuthInfo *CPrivate::linphone_core_find_auth_info(CLinphoneCore *lc, const char *realm, const char *username)
{
	LinphoneAuthInfo ref;
	MSList *elem;
	ref.realm=(char*)realm;
	ref.username=(char*)username;
	elem=ms_list_find_custom(lc->auth_info,&CPrivate::auth_info_compare,(void*)&ref);
	if (elem==NULL) {
		elem=ms_list_find_custom(lc->auth_info,&CPrivate::auth_info_compare_only_realm,(void*)&ref);
		if (elem==NULL) return NULL;
	}
	return (LinphoneAuthInfo*)elem->data;
}

void CPrivate::linphone_core_add_auth_info(CLinphoneCore *lc, LinphoneAuthInfo *info)
{
	int n;
	MSList *elem;
	char *userid;
	if (info->userid==NULL || info->userid[0]=='\0') userid=info->username;
	else userid=info->userid;
	eXosip_lock();
	eXosip_add_authentication_info(info->username,userid,
		info->passwd,info->ha1,info->realm);
	eXosip_unlock();
	/* if the user was prompted, re-allow automatic_action */
	if (lc->automatic_action>0) lc->automatic_action--;
	/* find if we are attempting to modify an existing auth info */
	elem=ms_list_find_custom(lc->auth_info,auth_info_compare,info);
	if (elem!=NULL){
		linphone_auth_info_destroy((LinphoneAuthInfo*)elem->data);
		elem->data=(void *)info;
		n=ms_list_position(lc->auth_info,elem);
	}else {
		lc->auth_info=ms_list_append(lc->auth_info,(void *)info);
		n=ms_list_size(lc->auth_info)-1;
	}
}

void CPrivate::linphone_core_abort_authentication(CLinphoneCore *lc,  LinphoneAuthInfo *info){
	if (lc->automatic_action>0) lc->automatic_action--;
}

void CPrivate::linphone_core_remove_auth_info(CLinphoneCore *lc, LinphoneAuthInfo *info){
	int len=ms_list_size(lc->auth_info);
	int newlen;
	int i;
	MSList *elem;
	lc->auth_info=ms_list_remove(lc->auth_info,info);
	newlen=ms_list_size(lc->auth_info);
	/*printf("len=%i newlen=%i\n",len,newlen);*/
	linphone_auth_info_destroy(info);
	for (i=0;i<len;i++){
		linphone_auth_info_write_config(lc->pConfig,NULL,i);
	}
	for (elem=lc->auth_info,i=0;elem!=NULL;elem=ms_list_next(elem),i++){
		linphone_auth_info_write_config(lc->pConfig,(LinphoneAuthInfo*)elem->data,i);
	}

}

const MSList *CPrivate::linphone_core_get_auth_info_list(const CLinphoneCore *lc){
	return lc->auth_info;
}

void CPrivate::linphone_core_clear_all_auth_info(CLinphoneCore *lc){
	MSList *elem;
	int i;
	eXosip_lock();
	eXosip_clear_authentication_info();
	eXosip_unlock();
	for(i=0,elem=lc->auth_info;elem!=NULL;elem=ms_list_next(elem),i++){
		LinphoneAuthInfo *info=(LinphoneAuthInfo*)elem->data;
		linphone_auth_info_destroy(info);
		linphone_auth_info_write_config(lc->pConfig,NULL,i);
	}
	ms_list_free(lc->auth_info);
	lc->auth_info=NULL;
}

void CPrivate::linphone_authentication_ok(CLinphoneCore *lc, eXosip_event_t *ev){
	char *prx_realm=NULL,*www_realm=NULL;
	osip_proxy_authorization_t *prx_auth;
	osip_authorization_t *www_auth;
	osip_message_t *msg=ev->request;
	char *username;
	LinphoneAuthInfo *as=NULL;

	username=osip_uri_get_username(msg->from->url);
	osip_message_get_proxy_authorization(msg,0,&prx_auth);
	osip_message_get_authorization(msg,0,&www_auth);
	if (prx_auth!=NULL)
		prx_realm=osip_proxy_authorization_get_realm(prx_auth);
	if (www_auth!=NULL)
		www_realm=osip_authorization_get_realm(www_auth);

	if (prx_realm==NULL && www_realm==NULL){
		ms_message("No authentication info in the request, ignoring");
		return;
	}
	/* see if we already have this auth information , not to ask it everytime to the user */
	if (prx_realm!=NULL)
		as=linphone_core_find_auth_info(lc,prx_realm,username);
	if (www_realm!=NULL) 
		as=linphone_core_find_auth_info(lc,www_realm,username);
	if (as){
		ms_message("Authentication for user=%s realm=%s is working.",username,prx_realm ? prx_realm : www_realm);
		as->works=TRUE;
	}
}


void CPrivate::linphone_core_find_or_ask_for_auth_info(CLinphoneCore *lc,const char *username,const char* realm, int tid)
{
	LinphoneAuthInfo *as=linphone_core_find_auth_info(lc,realm,username);
	if ( as==NULL || (as!=NULL && as->works==FALSE && as->first_time==FALSE)){
		lc->vtable->linphone_gtk_auth_info_requested(lc,realm,username);
		lc->automatic_action++;/*suspends eXosip_automatic_action until the user supplies a password */

	}
	if (as) as->first_time=FALSE;
}

void CPrivate::linphone_process_authentication(CLinphoneCore *lc, eXosip_event_t *ev)
{
	char *prx_realm=NULL,*www_realm=NULL;
	osip_proxy_authenticate_t *prx_auth;
	osip_www_authenticate_t *www_auth;
	osip_message_t *resp=ev->response;
	char *username;

	if (strcmp(ev->request->sip_method,"REGISTER")==0) {
		lc->pGeneralState->gstate_new_state(lc, GSTATE_REG_FAILED, "Authentication required");
	}

	username=osip_uri_get_username(resp->from->url);
	prx_auth=(osip_proxy_authenticate_t*)osip_list_get(&resp->proxy_authenticates,0);
	www_auth=(osip_proxy_authenticate_t*)osip_list_get(&resp->www_authenticates,0);
	if (prx_auth!=NULL)
		prx_realm=osip_proxy_authenticate_get_realm(prx_auth);
	if (www_auth!=NULL)
		www_realm=osip_www_authenticate_get_realm(www_auth);

	if (prx_realm==NULL && www_realm==NULL){
		OutputDebugString(_T("No realm in the server response."));
		return;
	}
	/* see if we already have this auth information , not to ask it everytime to the user */
	if (prx_realm!=NULL) 
		linphone_core_find_or_ask_for_auth_info(lc,username,prx_realm,ev->tid);
	if (www_realm!=NULL) 
		linphone_core_find_or_ask_for_auth_info(lc,username,www_realm,ev->tid);
}

//-------char.h

LinphoneChatRoom * CPrivate::linphone_core_create_chat_room(CLinphoneCore *lc, const char *to){
	char *real_url=NULL;
	osip_from_t *parsed_url=NULL;
	char *route;
	if (lc->linphone_core_interpret_url(to,&real_url,&parsed_url,&route)){
		LinphoneChatRoom *cr=(LinphoneChatRoom *)ms_new0(LinphoneChatRoom,1);
		cr->lc=lc;
		cr->peer=real_url;
		cr->peer_url=parsed_url;
		cr->route=route;
		lc->chatrooms=ms_list_append(lc->chatrooms,(void *)cr);
		return cr;
	}
	return NULL;
}


void CPrivate::linphone_chat_room_destroy(LinphoneChatRoom *cr){
	CLinphoneCore *lc=cr->lc;
	lc->chatrooms=ms_list_remove(lc->chatrooms,(void *) cr);
	osip_from_free(cr->peer_url);
	ms_free(cr->peer);
	ms_free(cr->route);
}

void CPrivate::linphone_chat_room_send_message(LinphoneChatRoom *cr, const char *msg){
	const char *identity=cr->lc->linphone_core_get_identity();
	osip_message_t *sip=NULL;
	eXosip_message_build_request(&sip,"MESSAGE",cr->peer,identity,cr->route);
	osip_message_set_content_type(sip,"text/plain");
	osip_message_set_body(sip,msg,strlen(msg));
	eXosip_message_send_request(sip);
}

bool_t CPrivate::linphone_chat_room_matches(LinphoneChatRoom *cr, osip_from_t *from){
	if (cr->peer_url->url->username && from->url->username && 
		strcmp(cr->peer_url->url->username,from->url->username)==0) return TRUE;
	return FALSE;
}

void CPrivate::linphone_chat_room_text_received(LinphoneChatRoom *cr, CLinphoneCore *lc, const char *from, const char *msg){
	lc->vtable->linphone_gtk_text_received(lc, cr, from, msg);
}

void CPrivate::linphone_core_text_received(CLinphoneCore *lc, eXosip_event_t *ev){
	MSList *elem;
	const char *msg;
	LinphoneChatRoom *cr=NULL;
	char *cleanfrom;
	osip_from_t *from_url=ev->request->from;
	osip_body_t *body=NULL;

	osip_message_get_body(ev->request,0,&body);
	if (body==NULL){
		OutputDebugString(_T("Could not get text message from SIP body"));
		return;
	}
	msg=body->body;
	lc->pMisc->from_2char_without_params(from_url,&cleanfrom);
	for(elem=lc->chatrooms;elem!=NULL;elem=ms_list_next(elem)){
		cr=(LinphoneChatRoom*)elem->data;
		if (linphone_chat_room_matches(cr,from_url)){
			break;
		}
		cr=NULL;
	}
	if (cr==NULL){
		/* create a new chat room */
		cr=linphone_core_create_chat_room(lc,cleanfrom);
	}
	linphone_chat_room_text_received(cr,lc,cleanfrom,msg);
	osip_free(cleanfrom);
}


void CPrivate::linphone_chat_room_set_user_data(LinphoneChatRoom *cr, void * ud){
	cr->user_data=ud;
}
void * CPrivate::linphone_chat_room_get_user_data(LinphoneChatRoom *cr){
	return cr->user_data;
}


//---enum 

#define DNS_ANSWER_MAX_SIZE 2048


char *CPrivate::create_enum_domain(const char *number){
	int len=strlen(number);
	char *domain=(char *)ms_malloc((len*2)+10);
	int i,j;
	
	for (i=0,j=len-1;j>=0;j--){
		domain[i]=number[j];
		i++;
		domain[i]='.';
		i++;
	}
	strcpy(&domain[i],"e164.arpa");
	ms_message("enum domain for %s is %s",number,domain);
	return domain;
}


bool_t CPrivate::is_a_number(const char *str){
	char *p=(char *)str;
	bool_t res=FALSE;
	bool_t space_found=FALSE;
	for(;;p++){
		switch(p[0]){
			case '9':
			case '8':
			case '7':
			case '6':
			case '5':
			case '4':
			case '3':
			case '2':
			case '1':
			case '0':
				res=TRUE;
				if (space_found) return FALSE; /* avoid splited numbers */
				break;
			case '\0':
				return res;
				break;
			case ' ':
				space_found=TRUE;
				break;
			default:
				return FALSE;
		}
	}
	return FALSE;
}
//4970072278724
bool_t CPrivate::is_enum(const char *sipaddress, char **enum_domain){
	char *p;
	p=(char *)strstr(sipaddress,"sip:");
	if (p==NULL) return FALSE; /* enum should look like sip:4369959250*/
	else p+=4;
	if (is_a_number(p)){
		if (enum_domain!=NULL){
			*enum_domain=create_enum_domain(p);
		}
		return TRUE;
	}
	return FALSE;
}



int CPrivate::enum_lookup(const char *enum_domain, enum_lookup_res_t **res){
	int err;
	//char dns_answer[DNS_ANSWER_MAX_SIZE];
	char *begin,*end;
	char *host_result, *command;
	int i;
	bool_t forkok;
	/*
	ns_msg handle;
	int count;
	
	memset(&handle,0,sizeof(handle));
	*res=NULL;
	ms_message("Resolving %s...",enum_domain);
	
	err=res_search(enum_domain,ns_c_in,ns_t_naptr,dns_answer,DNS_ANSWER_MAX_SIZE);
	if (err<0){
		ms_warning("Error resolving enum:",herror(h_errno));
		return -1;
	}
	ns_initparse(dns_answer,DNS_ANSWER_MAX_SIZE,&handle);
	count=ns_msg_count(handle,ns_s_an);
	
	for(i=0;i<count;i++){
		ns_rr rr;
		memset(&rr,0,sizeof(rr));
		ns_parserr(&handle,ns_s_an,i,&rr);
		ms_message("data=%s",ns_rr_rdata(rr));
	}
	*/
	command=ms_strdup_printf("host -t naptr %s",enum_domain);
	forkok=CMisc::lp_spawn_command_line_sync(command,&host_result,&err);
	ms_free(command);
	if (forkok){
		if (err!=0){
			OutputDebugString(_T("Host exited with %i error status."));
			return -1;
		}
	}else{
		OutputDebugString(_T("Could not spawn the \'host\' command."));
		return -1;
	}		
	ms_message("Answer received from dns (err=%i): %s",err,host_result);
	
	begin=strstr(host_result,"sip:");
	if (begin==0) {
		OutputDebugString(_T("No sip address found in dns naptr answer."));
		return -1;
	}
	*res=(enum_lookup_res_t *)ms_malloc0(sizeof(enum_lookup_res_t));
	err=0;
	for(i=0;i<MAX_ENUM_LOOKUP_RESULTS;i++){
		end=strstr(begin,"!");
		if (end==NULL) goto parse_error;
		end[0]='\0';
		(*res)->sip_address[i]=ms_strdup(begin);
		err++;
		begin=strstr(end+1,"sip:");
		if (begin==NULL) break;
	}
	ms_free(host_result);
	return err;

	parse_error:
		ms_free(*res);
		ms_free(host_result);
		*res=NULL;
		OutputDebugString(_T("Parse error in enum_lookup()."));
		return -1;
}

void CPrivate::enum_lookup_res_free(enum_lookup_res_t *res){
	int i;
	for (i=0;i<MAX_ENUM_LOOKUP_RESULTS;i++){
		if (res->sip_address[i]!=NULL) ms_free(res->sip_address[i]);
	}
	ms_free(res);
}

