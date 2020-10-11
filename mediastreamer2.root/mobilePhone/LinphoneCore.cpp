#include "StdAfx.h"
#include "config.h"
#include "LinphoneCore.h"
#include "Configure.h"
#include "staticManager.h"
#include "misc.h"
#include "proxy.h"
#include "GeneralState.h"
#include "private.h"
#include "friend.h"
#include "sipsetup.h"
#include "phoneCore.h"
#include "SdpHandler.h"
#include "Presence.h"
#include <eXosip2/eXosip.h>
#include "ProxyConfig.h"

CLinphoneCore::CLinphoneCore(CPhoneCore * phoneCore)
{
	this->pPhoneCore = phoneCore;
    exosip_running=FALSE;
	//_ua_name="Linphone";
	//_ua_version="1.0";
    //³õÊ¼»¯net_conf
	net_conf.nat_address = NULL;
	net_conf.stun_server = NULL;
	net_conf.relay = NULL;

	codecs_conf.audio_codecs = NULL;
	codecs_conf.video_codecs = NULL;

	sip_conf.contact = NULL;
	sip_conf.guessed_contact = NULL;
	sip_conf.deleted_proxies = NULL;
	sip_conf.proxies = NULL;


	default_proxy= NULL;;
	friends= NULL;;
	auth_info= NULL;;
	ringstream= NULL;;
	call= NULL;;   /* the current call, in the future it will be a list of calls (conferencing)*/
	queued_calls= NULL;;	/* used by the autoreplier */
	call_logs= NULL;
	chatrooms= NULL;
	audiostream= NULL;  /**/
	videostream= NULL;
	previewstream= NULL;
	local_profile= NULL;
	subscribers= NULL;
	alt_contact= NULL;
	data= NULL;
	play_file= NULL;
	rec_file= NULL;
	wait_ctx= NULL;
	vtable=NULL;


	pMisc = new CMisc();
	pProxy = new CProxy();
	pPrivate = new CPrivate();
	pFriend =new CFriend();
	pSipsetup =new CSipsetup();
	pSdpHandler =new CSdpHandler();
	pPresence = new CPresence();
	pGeneralState = new CGeneralState();

}

CLinphoneCore::CLinphoneCore(void)
{
	exosip_running=FALSE;
	//_ua_name="Linphone";
	//_ua_version="1.0";

	pMisc = new CMisc();
	pProxy = new CProxy();
	pPrivate = new CPrivate();
	pFriend =new CFriend();
	pSipsetup =new CSipsetup();
	pSdpHandler =new CSdpHandler();
	pPresence = new CPresence();
}

CLinphoneCore::~CLinphoneCore(void)
{  
    linphone_core_uninit();	
	if(pMisc!=NULL)
	delete pMisc ;
	if(pProxy!=NULL)
	delete pProxy ;
	if(pPrivate!=NULL)
	delete pPrivate;
	if(pFriend!=NULL)
	delete pFriend ;
	if(pSipsetup!=NULL)
	delete pSipsetup;
	if(pSdpHandler!=NULL)
	delete pSdpHandler;
	if(pPresence!=NULL)
	delete pPresence;
	if(pConfig!=NULL)
	delete pConfig;
}


void CLinphoneCore::discover_mtu(const char *remote){
	int mtu;
	if (net_conf.mtu==0	){
		mtu=ms_discover_mtu(remote);
		if (mtu>0){
			ms_set_mtu(mtu);
			ms_message("Discovered mtu is %i, RTP payload max size is %i",
				mtu, ms_get_payload_max_size());
		}
	}
}


/*prevent a gcc bug with %c*/
size_t CLinphoneCore::my_strftime(char *s, size_t max, const char  *fmt,  LPSYSTEMTIME tm){
   
	sprintf(s,"%d-%d-%d-%d:%d:%d",   
        tm->wYear,tm->wMonth,tm->wDay,   
        tm->wHour,tm->wMinute,tm->wSecond);
	return max;
	//return strftime(s, max, fmt, tm);
}


void CLinphoneCore::net_config_read ()
{
	int tmp;
	const char *tmpstr;
	CConfigure *config=pConfig;

	tmp=pConfig->lp_config_get_int("net","download_bw",0);
	linphone_core_set_download_bandwidth(tmp);
	tmp=pConfig->lp_config_get_int("net","upload_bw",0);
	linphone_core_set_upload_bandwidth(tmp);
	linphone_core_set_stun_server(pConfig->lp_config_get_string("net","stun_server",NULL));
	tmpstr=pConfig->lp_config_get_string("net","nat_address",NULL);
	if (tmpstr!=NULL && (strlen(tmpstr)<1)) tmpstr=NULL;
	linphone_core_set_nat_address(tmpstr);
	tmp=pConfig->lp_config_get_int("net","firewall_policy",0);
	linphone_core_set_firewall_policy((LinphoneFirewallPolicy)tmp);
	tmp=pConfig->lp_config_get_int("net","nat_sdp_only",0);
	net_conf.nat_sdp_only=tmp;
	tmp=pConfig->lp_config_get_int("net","mtu",0);
	linphone_core_set_mtu(tmp);
}

void CLinphoneCore::build_sound_devices_table(){
	const char **devices;
	const char **old;
	int ndev;
	int i;
	const MSList *elem=ms_snd_card_manager_get_list(ms_snd_card_manager_get());
	ndev=ms_list_size(elem);
	devices=(const char **)ms_malloc((ndev+1)*sizeof(const char *));
	for (i=0;elem!=NULL;elem=elem->next,i++){
		devices[i]=ms_snd_card_get_string_id((MSSndCard *)elem->data);
	}
	devices[ndev]=NULL;
	old=sound_conf.cards;
	sound_conf.cards=devices;
	if (old!=NULL) ms_free(old);
}

void CLinphoneCore::sound_config_read()
{
	/*int tmp;*/
	 char *tmpbuf;
	const char *devid;

	/* retrieve all sound devices */
	build_sound_devices_table();

	devid=pConfig->lp_config_get_string("sound","playback_dev_id",NULL);
	linphone_core_set_playback_device(devid);

	devid=pConfig->lp_config_get_string("sound","ringer_dev_id",NULL);
	linphone_core_set_ringer_device(devid);

	devid=pConfig->lp_config_get_string("sound","capture_dev_id",NULL);
	linphone_core_set_capture_device(devid);

	TCHAR pathbuf[MAX_PATH];
	GetModuleFileName(NULL,pathbuf,MAX_PATH);

	char mpath[MAX_PATH];

    CString path =  CString(pathbuf);
	path.Replace(_T("mobilePhone.exe"),_T(""));

	WideCharToMultiByte(CP_ACP,    
                    WC_COMPOSITECHECK,    
					path.GetBuffer(),    
                    path.GetLength(),    
                    mpath,    
                    5,    
                    NULL,    
                    NULL);  


	tmpbuf=PACKAGE_SOUND_DIR "/" LOCAL_RING;
    linphone_core_set_ring(tmpbuf);
	tmpbuf=PACKAGE_SOUND_DIR "/" REMOTE_RING;
	linphone_core_set_ringback(tmpbuf);
	pMisc->check_sound_device(this);
	sound_conf.latency=0;

	linphone_core_enable_echo_cancelation(
		pConfig->lp_config_get_int("sound","echocancelation",0));

	linphone_core_enable_echo_limiter(pConfig->lp_config_get_int("sound","echolimiter",0));
	linphone_core_enable_agc(pConfig->lp_config_get_int("sound","agc",0));
}

void CLinphoneCore::sip_config_read()
{
	char *contact;
	const char *tmpstr;
	int port;
	int i,tmp;
	int ipv6;
	port=pConfig->lp_config_get_int("sip","use_info",0);
	linphone_core_set_use_info_for_dtmf(port);

	ipv6=pConfig->lp_config_get_int("sip","use_ipv6",-1);
	if (ipv6==-1){
		ipv6=0;
		//if (host_has_ipv6_network())
		if (FALSE)
		{
			vtable->linphone_gtk_display_message(this,("Your machine appears to be connected to an IPv6 network. By default linphone always uses IPv4. Please update your configuration if you want to use IPv6"));
		}
	}
	linphone_core_enable_ipv6(ipv6);
	port=pConfig->lp_config_get_int("sip","sip_port",5060);
	linphone_core_set_sip_port(port);

	tmpstr=pConfig->lp_config_get_string("sip","contact",NULL);
	if (tmpstr==NULL || linphone_core_set_primary_contact(tmpstr)==-1) {
		char *hostname=NULL;//getenv("HOST");
		char *username=NULL;//getenv("USER");
		//if (hostname==NULL) hostname=getenv("HOSTNAME");
		if (hostname==NULL)
			hostname="unknown-host";
		if (username==NULL){
			username="toto";
		}
		contact=ortp_strdup_printf("sip:%s@%s",username,hostname);
		linphone_core_set_primary_contact(contact);
		ms_free(contact);
	}

	tmp=pConfig->lp_config_get_int("sip","guess_hostname",1);
	linphone_core_set_guess_hostname(tmp);


	tmp=pConfig->lp_config_get_int("sip","inc_timeout",15);
	linphone_core_set_inc_timeout(tmp);

	/* get proxies config */
	for(i=0;; i++){
		CProxyConfig *cfg=pProxy->linphone_proxy_config_new_from_config_file(pConfig,i);
		if (cfg!=NULL){
			pProxy->linphone_core_add_proxy_config(this,cfg);
		}else{
			break;
		}
	}
	/* get the default proxy */
	tmp=pConfig->lp_config_get_int("sip","default_proxy",-1);
	pProxy->linphone_core_set_default_proxy_index(this,tmp);

	/* read authentication information */
	for(i=0;; i++){
		LinphoneAuthInfo *ai=pPrivate->linphone_auth_info_new_from_config_file(pConfig,i);
		if (ai!=NULL){
			pPrivate->linphone_core_add_auth_info(this,ai);
		}else{
			break;
		}
	}
	/*for test*/
	sip_conf.sdp_200_ack=pConfig->lp_config_get_int("sip","sdp_200_ack",0);
	sip_conf.only_one_codec=pConfig->lp_config_get_int("sip","only_one_codec",0);
	sip_conf.register_only_when_network_is_up=
		pConfig->lp_config_get_int("sip","register_only_when_network_is_up",0);
}

void CLinphoneCore::rtp_config_read()
{
	int port;
	int jitt_comp;
	int nortp_timeout;
	port=pConfig->lp_config_get_int("rtp","audio_rtp_port",7078);
	linphone_core_set_audio_port(port);

	port=pConfig->lp_config_get_int("rtp","video_rtp_port",9078);
	if (port==0) port=9078;
	linphone_core_set_video_port(port);

	jitt_comp=pConfig->lp_config_get_int("rtp","audio_jitt_comp",60);
	linphone_core_set_audio_jittcomp(jitt_comp);		
	jitt_comp=pConfig->lp_config_get_int("rtp","video_jitt_comp",60);
	nortp_timeout=pConfig->lp_config_get_int("rtp","nortp_timeout",30);
	linphone_core_set_nortp_timeout(nortp_timeout);	
}


PayloadType * CLinphoneCore::get_codec(CConfigure *config, char* type,int index){
	char codeckey[50];
	const char *mime,*fmtp;
	int rate,enabled;
	PayloadType *pt;

	snprintf(codeckey,50,"%s_%i",type,index);
	mime=pConfig->lp_config_get_string(codeckey,"mime",NULL);
	if (mime==NULL || strlen(mime)==0 ) return NULL;

	pt=payload_type_new();
	pt->mime_type=ms_strdup(mime);

	rate=pConfig->lp_config_get_int(codeckey,"rate",8000);
	pt->clock_rate=rate;
	fmtp=pConfig->lp_config_get_string(codeckey,"recv_fmtp",NULL);
	if (fmtp) pt->recv_fmtp=ms_strdup(fmtp);
	enabled=pConfig->lp_config_get_int(codeckey,"enabled",1);
	if (enabled ) pt->flags|=PAYLOAD_TYPE_ENABLED;
	//ms_message("Found codec %s/%i",pt->mime_type,pt->clock_rate);
	return pt;
}

void CLinphoneCore::codecs_config_read()
{
	int i;
	PayloadType *pt;
	MSList *audio_codecs=NULL;
	MSList *video_codecs=NULL;
	for (i=0;;i++){
		pt=get_codec(pConfig,"audio_codec",i);
		if (pt==NULL) break;
		audio_codecs=ms_list_append(audio_codecs,(void *)pt);
	}
	for (i=0;;i++){
		pt=get_codec(pConfig,"video_codec",i);
		if (pt==NULL) break;
		video_codecs=ms_list_append(video_codecs,(void *)pt);
	}
	linphone_core_set_audio_codecs(audio_codecs);
	linphone_core_set_video_codecs(video_codecs);
	pMisc->linphone_core_setup_local_rtp_profile(this);
}

void CLinphoneCore::ui_config_read()
{
	LinphoneFriend *lf;
	int i;
	for (i=0;(lf=pFriend->linphone_friend_new_from_config_file(this,i))!=NULL;i++){
		pFriend->linphone_core_add_friend(this,lf);
	}

}

void CLinphoneCore::autoreplier_config_init()
{
	autoreplier_config_t *config=&autoreplier_conf;
	config->enabled=pConfig->lp_config_get_int("autoreplier","enabled",0);
	config->after_seconds=pConfig->lp_config_get_int("autoreplier","after_seconds",6);
	config->max_users=pConfig->lp_config_get_int("autoreplier","max_users",1);
	config->max_rec_time=pConfig->lp_config_get_int("autoreplier","max_rec_time",60);
	config->max_rec_msg=pConfig->lp_config_get_int("autoreplier","max_rec_msg",10);
	config->message=pConfig->lp_config_get_string("autoreplier","message",NULL);
}

void CLinphoneCore::linphone_core_set_download_bandwidth( int bw){
	net_conf.download_bw=bw;
	if (bw==0){ /*infinite*/
		dw_audio_bw=-1;
		dw_video_bw=-1;
	}else {
		dw_audio_bw=MIN(audio_bw,bw);
		dw_video_bw=MAX(bw-dw_audio_bw-10,0);/*-10: security margin*/
	}
}

void CLinphoneCore::linphone_core_set_upload_bandwidth( int bw){
	net_conf.upload_bw=bw;
	if (bw==0){ /*infinite*/
		up_audio_bw=-1;
		up_video_bw=-1;
	}else{
		up_audio_bw=MIN(audio_bw,bw);
		up_video_bw=MAX(bw-up_audio_bw-10,0);/*-10: security margin*/
	}
}

int CLinphoneCore::linphone_core_get_download_bandwidth(){
	return net_conf.download_bw;
}

int CLinphoneCore::linphone_core_get_upload_bandwidth(){
	return net_conf.upload_bw;
}

const char * CLinphoneCore::linphone_core_get_version(void){
	return liblinphone_version;
}



const MSList *CLinphoneCore::linphone_core_get_audio_codecs()
{
	return codecs_conf.audio_codecs;
}

const MSList *CLinphoneCore::linphone_core_get_video_codecs()
{
	return codecs_conf.video_codecs;
}

int CLinphoneCore::linphone_core_set_primary_contact( const char *contact)
{
	osip_from_t *ctt=NULL;
	osip_from_init(&ctt);
	if (osip_from_parse(ctt,contact)!=0){
		OutputDebugString(_T("Bad contact url: %s"));
  	    OutputDebugString(_T("\r\n"));
		//("Bad contact url: %s",contact);
		osip_from_free(ctt);
		return -1;
	}
	if (sip_conf.contact!=NULL) ms_free(sip_conf.contact);
	sip_conf.contact=ms_strdup(contact);
	if (sip_conf.guessed_contact!=NULL){
		ms_free(sip_conf.guessed_contact);
		sip_conf.guessed_contact=NULL;
	}
	osip_from_free(ctt);
	return 0;
}


/*result must be an array of chars at least LINPHONE_IPADDR_SIZE */
void CLinphoneCore::linphone_core_get_local_ip( const char *dest, char *result){
	if (apply_nat_settings){
		//		apply_nat_settings();
		apply_nat_settings=FALSE;
	}
	if (linphone_core_get_firewall_policy()==LINPHONE_POLICY_USE_NAT_ADDRESS){
		strncpy(result,linphone_core_get_nat_address(),LINPHONE_IPADDR_SIZE);
		return;
	}
	if (pMisc->linphone_core_get_local_ip_for(dest,result)==0)
		return;
	/*else fallback to exosip routine that will attempt to find the most realistic interface */
	if (eXosip_guess_localip(sip_conf.ipv6_enabled ? AF_INET6 : AF_INET,result,LINPHONE_IPADDR_SIZE)<0){
		/*default to something */
		strncpy(result,sip_conf.ipv6_enabled ? "::1" : "127.0.0.1",LINPHONE_IPADDR_SIZE);
		OutputDebugString(_T("Could not find default routable ip address !")); 
	}
}

const char *CLinphoneCore::linphone_core_get_primary_contact()
{
	char *identity;
	char tmp[LINPHONE_IPADDR_SIZE];
	if (sip_conf.guess_hostname){
		if (sip_conf.guessed_contact==NULL || sip_conf.loopback_only){
			char *guessed=NULL;
			osip_from_t *url;
			if (sip_conf.guessed_contact!=NULL){
				delete sip_conf.guessed_contact;
				sip_conf.guessed_contact=NULL;
			}

			osip_from_init(&url);
			if (osip_from_parse(url,sip_conf.contact)==0){

			}else OutputDebugString(_T("Could not parse identity contact !"));
			linphone_core_get_local_ip(NULL, tmp);
			if (strcmp(tmp,"127.0.0.1")==0 || strcmp(tmp,"::1")==0 ){
				OutputDebugString(_T("Local loopback network only !"));
				sip_conf.loopback_only=TRUE;
			}else sip_conf.loopback_only=FALSE;
			osip_free(url->url->host);
			url->url->host=osip_strdup(tmp);
			if (url->url->port!=NULL){
				osip_free(url->url->port);
				url->url->port=NULL;
			}
			if (sip_conf.sip_port!=5060){
				url->url->port=ortp_strdup_printf("%i",sip_conf.sip_port);
			}
			osip_from_to_str(url,&guessed);
			sip_conf.guessed_contact=guessed;

			osip_from_free(url);

		}
		identity=sip_conf.guessed_contact;
	}else{
		identity=sip_conf.contact;
	}
	return identity;
}

void CLinphoneCore::linphone_core_set_guess_hostname( bool_t val){
	sip_conf.guess_hostname=val;
}

bool_t CLinphoneCore::linphone_core_get_guess_hostname(){
	return sip_conf.guess_hostname;
}

osip_from_t *CLinphoneCore::linphone_core_get_primary_contact_parsed(){
	int err;
	osip_from_t *contact;
	osip_from_init(&contact);
	err=osip_from_parse(contact,linphone_core_get_primary_contact());
	if (err<0) {
		osip_from_free(contact);
		return NULL;
	}
	return contact;
}

int CLinphoneCore::linphone_core_set_audio_codecs( MSList *codecs)
{
	if (codecs_conf.audio_codecs!=NULL) ms_list_free(codecs_conf.audio_codecs);
	codecs_conf.audio_codecs=codecs;
	return 0;
}

int CLinphoneCore::linphone_core_set_video_codecs( MSList *codecs)
{
	if (codecs_conf.video_codecs!=NULL) ms_list_free(codecs_conf.video_codecs);
	codecs_conf.video_codecs=codecs;
	return 0;
}

const MSList * CLinphoneCore::linphone_core_get_friend_list()
{
	return friends;
}

int CLinphoneCore::linphone_core_get_audio_jittcomp()
{
	return rtp_conf.audio_jitt_comp;
}

int CLinphoneCore::linphone_core_get_audio_port()
{
	return rtp_conf.audio_rtp_port;
}

int CLinphoneCore::linphone_core_get_video_port(){
	return rtp_conf.video_rtp_port;
}

int CLinphoneCore::linphone_core_get_nortp_timeout(){
	return rtp_conf.nortp_timeout;
}

void CLinphoneCore::linphone_core_set_audio_jittcomp( int value)
{
	rtp_conf.audio_jitt_comp=value;
}

void CLinphoneCore::linphone_core_set_audio_port( int port)
{
	rtp_conf.audio_rtp_port=port;
}

void CLinphoneCore::linphone_core_set_video_port( int port){
	rtp_conf.video_rtp_port=port;
}

void CLinphoneCore::linphone_core_set_nortp_timeout( int nortp_timeout){
	rtp_conf.nortp_timeout=nortp_timeout;
}

bool_t CLinphoneCore::linphone_core_get_use_info_for_dtmf()
{
	return sip_conf.use_info;
}

void CLinphoneCore::linphone_core_set_use_info_for_dtmf(bool_t use_info)
{
	sip_conf.use_info=use_info;
}

int CLinphoneCore::linphone_core_get_sip_port()
{
	return sip_conf.sip_port;
}



void CLinphoneCore::apply_user_agent(void){
	char ua_string[256];
	snprintf(ua_string,sizeof(ua_string),"%s/%s (eXosip2/%s)",_ua_name,_ua_version,
#ifdef HAVE_EXOSIP_GET_VERSION
		eXosip_get_version()
#else
		"unknown"
#endif
		);
	eXosip_set_user_agent(ua_string);
}

void CLinphoneCore::linphone_core_set_user_agent(const char *name, const char *ver){
	strncpy(_ua_name,name,sizeof(_ua_name)-1);
	strncpy(_ua_version,ver,sizeof(_ua_version));
}

void CLinphoneCore::linphone_core_set_sip_port(int port)
{
	const char *anyaddr;
	int err=0;
	if (port==sip_conf.sip_port) return;
	sip_conf.sip_port=port;
	if (exosip_running) eXosip_quit();
	err = eXosip_init();
	if(err!=0)
	{
		return ;
	}
	err=0;
	eXosip_set_option((eXosip_option)13,&err); /*13=EXOSIP_OPT_SRV_WITH_NAPTR, as it is an enum value, we can't use it unless we are sure of the
											   version of eXosip, which is not the case*/
	eXosip_enable_ipv6(sip_conf.ipv6_enabled);
	if (sip_conf.ipv6_enabled)
		anyaddr="::0";
	else
		anyaddr="0.0.0.0";
	err=eXosip_listen_addr (IPPROTO_UDP, anyaddr, port,
		sip_conf.ipv6_enabled ?  PF_INET6 : PF_INET, 0);
	if (err<0){
		char *msg=ortp_strdup_printf("UDP port %i seems already in use ! Cannot initialize.",port);
		OutputDebugString((LPCWSTR)msg);
		vtable->linphone_gtk_display_warning(this,msg);
		ms_free(msg);
		return;
	}
#ifdef VINCENT_MAURY_RSVP
	/* tell exosip the qos settings according to default linphone parameters */
	eXosip_set_rsvp_mode (rsvp_enable);
	eXosip_set_rpc_mode (rpc_enable);
#endif
	apply_user_agent();
	exosip_running=TRUE;
}

bool_t CLinphoneCore::linphone_core_ipv6_enabled(){
	return sip_conf.ipv6_enabled;
}
void CLinphoneCore::linphone_core_enable_ipv6( bool_t val){
	if (sip_conf.ipv6_enabled!=val){
		sip_conf.ipv6_enabled=val;
		if (exosip_running){
			/* we need to restart eXosip */
			linphone_core_set_sip_port(sip_conf.sip_port);
		}
	}
}
void CLinphoneCore::display_bandwidth(RtpSession *as, RtpSession *vs){
	ms_message("bandwidth usage: audio=[d=%.1f,u=%.1f] video=[d=%.1f,u=%.1f] kbit/sec",
		(as!=NULL) ? (rtp_session_compute_recv_bandwidth(as)*1e-3) : 0,
		(as!=NULL) ? (rtp_session_compute_send_bandwidth(as)*1e-3) : 0,
		(vs!=NULL) ? (rtp_session_compute_recv_bandwidth(vs)*1e-3) : 0,
		(vs!=NULL) ? (rtp_session_compute_send_bandwidth(vs)*1e-3) : 0);
}

void CLinphoneCore::linphone_core_disconnected(){
	vtable->linphone_gtk_display_warning(this,("Remote end seems to have disconnected, the call is going to be closed."));
	linphone_core_terminate_call(NULL);
}

void CLinphoneCore::proxy_update( time_t curtime){
	bool_t doit=FALSE;
	static time_t last_check=0;
	static bool_t last_status=FALSE;
	if (sip_conf.register_only_when_network_is_up){
		char result[LINPHONE_IPADDR_SIZE];
		/* only do the network up checking every five seconds */
		if (last_check==0 || (curtime-last_check)>=5){
			if (eXosip_guess_localip(sip_conf.ipv6_enabled ? AF_INET6 : AF_INET,result,LINPHONE_IPADDR_SIZE)==0){
				if (strcmp(result,"::1")!=0 && strcmp(result,"127.0.0.1")!=0){
					last_status=TRUE;
					ms_message("Network is up, registering now (%s)",result);
				}else last_status=FALSE;
			}
			last_check=curtime;
		}
		doit=last_status;
	}else doit=TRUE;
	if (doit) ms_list_for_each(sip_conf.proxies,(void (*)(void*))&CProxy::linphone_proxy_config_update);
}



bool_t CLinphoneCore::linphone_core_is_in_main_thread(){
	return TRUE;
}

osip_to_t * CLinphoneCore::osip_to_create(const char *to){
	osip_to_t *ret;
	osip_to_init(&ret);
	if (osip_to_parse(ret,to)<0){
		osip_to_free(ret);
		return NULL;
	}
	return ret;
}

char *CLinphoneCore::guess_route_if_any( osip_to_t *parsed_url){
	const MSList *elem=pProxy->linphone_core_get_proxy_config_list(this);
	for(;elem!=NULL;elem=elem->next){
		CProxyConfig *cfg=(CProxyConfig*)elem->data;
		char prx[256];
		if (cfg->ssctx && pSipsetup->sip_setup_context_get_proxy(cfg->ssctx,parsed_url->url->host,prx,sizeof(prx))==0){
			ms_message("We have a proxy for domain %s",parsed_url->url->host);
			if (strcmp(parsed_url->url->host,prx)!=0){
				char *route=NULL;
				osip_route_t *rt;
				osip_route_init(&rt);
				if (osip_route_parse(rt,prx)==0){
					char *rtstr;
					osip_uri_uparam_add(rt->url,osip_strdup("lr"),NULL);
					osip_route_to_str(rt,&rtstr);
					route=ms_strdup(rtstr);
					osip_free(rtstr);
				}
				osip_route_free(rt);
				ms_message("Adding a route: %s",route);
				return route;
			}
		}
	}
	return NULL;
}

bool_t CLinphoneCore::linphone_core_interpret_url( const char *url, char **real_url, osip_to_t **real_parsed_url, char **route){
	enum_lookup_res_t *enumres=NULL;
	osip_to_t *parsed_url=NULL;
	char *enum_domain=NULL;
	CProxyConfig *proxy;
	char *tmpurl;
	const char *tmproute;
	if (real_url!=NULL) *real_url=NULL;
	if (real_parsed_url!=NULL) *real_parsed_url=NULL;
	*route=NULL;
	tmproute=linphone_core_get_route();

	if (pPrivate->is_enum(url,&enum_domain)){
		vtable->linphone_gtk_display_status(this,("Looking for telephone number destination..."));
		if (pPrivate->enum_lookup(enum_domain,&enumres)<0){
			vtable->linphone_gtk_display_status(this,("Could not resolve this number."));
			ms_free(enum_domain);
			return FALSE;
		}
		ms_free(enum_domain);
		tmpurl=enumres->sip_address[0];
		if (real_url!=NULL) *real_url=ms_strdup(tmpurl);
		if (real_parsed_url!=NULL) *real_parsed_url=osip_to_create(tmpurl);
		pPrivate->enum_lookup_res_free(enumres);
		if (tmproute) *route=ms_strdup(tmproute);
		return TRUE;
	}
	/* check if we have a "sip:" */
	if (strstr(url,"sip:")==NULL){
		/* this doesn't look like a true sip uri */
		proxy=default_proxy;
		if (proxy!=NULL){
			/* append the proxy domain suffix */
			osip_from_t *uri;
			char *sipaddr;
			const char *identity=linphone_proxy_config_get_identity(proxy);
			osip_from_init(&uri);
			if (osip_from_parse(uri,identity)<0){
				osip_from_free(uri);
				return FALSE;
			}
			sipaddr=ortp_strdup_printf("sip:%s@%s",url,uri->url->host);
			if (real_parsed_url!=NULL) *real_parsed_url=osip_to_create(sipaddr);
			if (real_url!=NULL) *real_url=sipaddr;
			else ms_free(sipaddr);
#if 0
			/*if the prompted uri was auto-suffixed with proxy domain,
			then automatically set a route so that the request goes
			through the proxy*/
			if (tmproute==NULL){
				osip_route_t *rt=NULL;
				char *rtstr=NULL;
				osip_route_init(&rt);
				if (osip_route_parse(rt,linphone_proxy_config_get_addr(proxy))==0){
					osip_uri_uparam_add(rt->url,osip_strdup("lr"),NULL);
					osip_route_to_str(rt,&rtstr);
					*route=ms_strdup(rtstr);
					osip_free(rtstr);
				}
				ms_message("setting automatically a route to %s",*route);
			}
			else *route=ms_strdup(tmproute);
#else
			if (tmproute==NULL) *route=guess_route_if_any(*real_parsed_url);
			if (tmproute) *route=ms_strdup(tmproute);
#endif
			return TRUE;
		}
	}
	parsed_url=osip_to_create(url);
	if (parsed_url!=NULL){
		if (real_url!=NULL) *real_url=ms_strdup(url);
		if (real_parsed_url!=NULL) *real_parsed_url=parsed_url;
		else osip_to_free(parsed_url);
		if (tmproute) *route=ms_strdup(tmproute);
		else *route=guess_route_if_any(*real_parsed_url);
		return TRUE;
	}
	/* else we could not do anything with url given by user, so display an error */

	vtable->linphone_gtk_display_warning(this,("Could not parse given sip address. A sip url usually looks like sip:user@domain"));

	return FALSE;
}

const char * CLinphoneCore::linphone_core_get_identity(){
	CProxyConfig *proxy=NULL;
	const char *from;
	pProxy->linphone_core_get_default_proxy(this,&proxy);
	if (proxy!=NULL) {
		from=linphone_proxy_config_get_identity(proxy);
	}else from=linphone_core_get_primary_contact();
	return from;
}

const char * CLinphoneCore::linphone_core_get_route(){
	CProxyConfig *proxy=NULL;
	const char *route=NULL;
	pProxy->linphone_core_get_default_proxy(this,&proxy);
	if (proxy!=NULL) {
		route=linphone_proxy_config_get_route(proxy);
	}
	return route;
}

void CLinphoneCore::linphone_set_sdp(osip_message_t *sip, const char *sdpmesg){
	int sdplen=strlen(sdpmesg);
	char clen[10];
	snprintf(clen,sizeof(clen),"%i",sdplen);
	osip_message_set_body(sip,sdpmesg,sdplen);
	osip_message_set_content_type(sip,"application/sdp");
	osip_message_set_content_length(sip,clen);
}

CProxyConfig * CLinphoneCore::linphone_core_lookup_known_proxy( const char *uri){
	const MSList *elem;
	CProxyConfig *found_cfg=NULL;
	osip_from_t *parsed_uri;
	osip_from_init(&parsed_uri);
	osip_from_parse(parsed_uri,uri);
	for (elem=pProxy->linphone_core_get_proxy_config_list(this);elem!=NULL;elem=elem->next){
		CProxyConfig *cfg=(CProxyConfig*)elem->data;
		const char *domain=pProxy->linphone_proxy_config_get_domain(cfg);
		if (domain!=NULL && strcmp(domain,parsed_uri->url->host)==0){
			found_cfg=cfg;
			break;
		}
	}
	osip_from_free(parsed_uri);
	return found_cfg;
}

void CLinphoneCore::fix_contact(osip_message_t *msg, const char *localip,
								CProxyConfig *dest_proxy){
									osip_contact_t *ctt=NULL;
									const char *ip=NULL;
									int port=5060;

									osip_message_get_contact(msg,0,&ctt);
									if (ctt!=NULL){
										if (dest_proxy!=NULL){
											/* if we know the request will go to a known proxy for which we are registered,
											we can use the same contact address as in the register */
											pProxy->linphone_proxy_config_get_contact(dest_proxy,&ip,&port);
										}else{
											ip=localip;
											pProxy->linphone_core_get_default_proxy(this,&dest_proxy);
											port=linphone_core_get_sip_port();
										}
										if (ip!=NULL){
											osip_free(ctt->url->host);
											ctt->url->host=osip_strdup(ip);
										}
										if (port!=0){
											char tmp[10]={0};
											char *str;
											snprintf(tmp,sizeof(tmp)-1,"%i",port);
											if (ctt->url->port!=NULL)
												osip_free(ctt->url->port);
											ctt->url->port=osip_strdup(tmp);
											osip_contact_to_str(ctt,&str);
											ms_message("Contact has been fixed to %s",str);
											osip_free(str);
										}
									}
}

int CLinphoneCore::linphone_core_invite( const char *url)
{
	char *barmsg;
	int err=0;
	char *sdpmesg=NULL;
	char *route=NULL;
	const char *from=NULL;
	osip_message_t *invite=NULL;
	sdp_context_t *ctx=NULL;
	CProxyConfig *proxy=NULL;
	osip_from_t *parsed_url2=NULL;
	osip_to_t *real_parsed_url=NULL;
	char *real_url=NULL;
	CProxyConfig *dest_proxy=NULL;

	if (call!=NULL){
		vtable->linphone_gtk_display_warning(this,("Sorry, having multiple simultaneous calls is not supported yet !"));
		return -1;
	}

	pGeneralState->gstate_new_state(this, GSTATE_CALL_OUT_INVITE, url);
	pProxy->linphone_core_get_default_proxy(this,&proxy);
	if (!linphone_core_interpret_url(url,&real_url,&real_parsed_url,&route)){
		/* bad url */
		pGeneralState->gstate_new_state(this, GSTATE_CALL_ERROR, NULL);
		return -1;
	}
	dest_proxy=linphone_core_lookup_known_proxy(real_url);

	if (proxy!=dest_proxy && dest_proxy!=NULL) {
		ms_message("Overriding default proxy setting for this call:");
		ms_message("The used identity will be %s",linphone_proxy_config_get_identity(dest_proxy));
	}

	if (dest_proxy!=NULL) 
		from=linphone_proxy_config_get_identity(dest_proxy);
	else if (proxy!=NULL)
		from=linphone_proxy_config_get_identity(proxy);

	/* if no proxy or no identity defined for this proxy, default to primary contact*/
	if (from==NULL) from=linphone_core_get_primary_contact();

	err=eXosip_call_build_initial_invite(&invite,real_url,from,
		route,"Phone call");
	if (err<0){
		OutputDebugString(_T("Could not build initial invite"));
		goto end;
	}

	/* make sdp message */

	osip_from_init(&parsed_url2);
	osip_from_parse(parsed_url2,from);

	call=pPhoneCore->linphone_call_new_outgoing(parsed_url2,real_parsed_url);
	/*try to be best-effort in giving real local or routable contact address,
	except when the user choosed to override the ipaddress */
	if (linphone_core_get_firewall_policy()!=LINPHONE_POLICY_USE_NAT_ADDRESS)
		fix_contact(invite,call->localip,dest_proxy);

	barmsg=ortp_strdup_printf("%s %s", ("Contacting"), real_url);
	vtable->linphone_gtk_display_status(this,barmsg);
	ms_free(barmsg);
	if (!sip_conf.sdp_200_ack){
		ctx=call->sdpctx;
		sdpmesg=pSdpHandler->sdp_context_get_offer(ctx);
		linphone_set_sdp(invite,sdpmesg);
		linphone_core_init_media_streams();
	}
	eXosip_lock();
	err=eXosip_call_send_initial_invite(invite);
	if (err > 0)
	{
		eXosip_call_set_reference (err, NULL);
	}

	call->cid=err;
	eXosip_unlock();
	if (err<0){
		OutputDebugString(_T("Could not initiate call."));
		vtable->linphone_gtk_display_status(this,("could not call"));
		linphone_call_destroy(call);
		call=NULL;
		linphone_core_stop_media_streams();
	}

	goto end;
end:
	if (real_url!=NULL) ms_free(real_url);
	if (real_parsed_url!=NULL) osip_to_free(real_parsed_url);
	if (parsed_url2!=NULL) osip_from_free(parsed_url2);
	if (err<0)
		pGeneralState->gstate_new_state(this, GSTATE_CALL_ERROR, NULL);
	if (route!=NULL) ms_free(route);
	return (err<0) ? -1 : 0;
}

int CLinphoneCore::linphone_core_refer( const char *url)
{
	char *real_url=NULL;
	osip_to_t *real_parsed_url=NULL;
	osip_message_t *msg=NULL;
	char *route;
	if (!linphone_core_interpret_url(url,&real_url,&real_parsed_url, &route)){
		/* bad url */
		return -1;
	}
	if (route!=NULL) ms_free(route);

	if (call==NULL){
		OutputDebugString(_T("No established call to refer."));
		return -1;
	}
	call=NULL;
	eXosip_call_build_refer(call->did, real_url, &msg);
	eXosip_lock();
	eXosip_call_send_request(call->did, msg);
	eXosip_unlock();
	return 0;
}

bool_t CLinphoneCore::linphone_core_inc_invite_pending(){
	if (call!=NULL && call->dir==LinphoneCallIncoming){
		return TRUE;
	}
	return FALSE;
}

#ifdef VINCENT_MAURY_RSVP
/* on=1 for RPC_ENABLE=1...*/
int linphone_core_set_rpc_mode( int on)
{
	if (on==1)
		printf("RPC_ENABLE set on\n");
	else 
		printf("RPC_ENABLE set off\n");
	rpc_enable = (on==1);
	/* need to tell eXosip the new setting */
	if (eXosip_set_rpc_mode (rpc_enable)!=0)
		return -1;
	return 0;
}

/* on=1 for RSVP_ENABLE=1...*/
int linphone_core_set_rsvp_mode( int on)
{
	if (on==1)
		printf("RSVP_ENABLE set on\n");
	else 
		printf("RSVP_ENABLE set off\n");
	rsvp_enable = (on==1);
	/* need to tell eXosip the new setting */
	if (eXosip_set_rsvp_mode (rsvp_enable)!=0)
		return -1;
	return 0;
}

/* answer : 1 for yes, 0 for no */
int linphone_core_change_qos( int answer)
{
	char *sdpmesg;
	if (call==NULL){
		return -1;
	}

	if (rsvp_enable && answer==1)
	{
		/* answer is yes, local setting is with qos, so 
		* the user chose to continue with no qos ! */
		/* so switch in normal mode : ring and 180 */
		rsvp_enable = 0; /* no more rsvp */
		eXosip_set_rsvp_mode (rsvp_enable);
		/* send 180 */
		eXosip_lock();
		eXosip_answer_call(call->did,180,NULL);
		eXosip_unlock();
		/* play the ring */
		ms_message("Starting local ring...");
		ringstream=ring_start(sound_conf.local_ring,
			2000,ms_snd_card_manager_get_card(ms_snd_card_manager_get(),sound_conf.ring_sndcard));
	}
	else if (!rsvp_enable && answer==1)
	{
		/* switch to QoS mode on : answer 183 session progress */
		rsvp_enable = 1;
		eXosip_set_rsvp_mode (rsvp_enable);
		/* take the sdp already computed, see osipuacb.c */
		sdpmesg=call->sdpctx->answerstr;
		eXosip_lock();
		eXosip_answer_call_with_body(call->did,183,"application/sdp",sdpmesg);
		eXosip_unlock();
	}
	else
	{
		/* decline offer (603) */
		linphone_core_terminate_call(lc, NULL);
	}
	return 0;
}
#endif

void CLinphoneCore::linphone_core_init_media_streams(){
	audiostream=audio_stream_new(linphone_core_get_audio_port(),linphone_core_ipv6_enabled());
	if (linphone_core_echo_limiter_enabled()){
		const char *type=pConfig->lp_config_get_string("sound","el_type","mic");
		float gain=pConfig->lp_config_get_float("sound","mic_gain",-1);
		if (strcasecmp(type,"mic")==0)
			audio_stream_enable_echo_limiter(audiostream,ELControlMic);
		else if (strcasecmp(type,"speaker")==0)
			audio_stream_enable_echo_limiter(audiostream,ELControlSpeaker);
		if (gain!=-1){
			audio_stream_enable_gain_control(audiostream,TRUE);
		}

	}
	if (linphone_core_echo_cancelation_enabled()){
		int len,delay,framesize;
		len=pConfig->lp_config_get_int("sound","ec_tail_len",0);
		delay=pConfig->lp_config_get_int("sound","ec_delay",0);
		framesize=pConfig->lp_config_get_int("sound","ec_framesize",0);
		audio_stream_set_echo_canceler_params(audiostream,len,delay,framesize);
	}
	audio_stream_enable_automatic_gain_control(audiostream,linphone_core_agc_enabled());
#ifdef VIDEO_ENABLED
	if (video_conf.display || video_conf.capture)
		videostream=video_stream_new(linphone_core_get_video_port(),linphone_core_ipv6_enabled());
#else
	videostream=NULL;
#endif
}

void CLinphoneCore::linphone_core_dtmf_received(RtpSession* s, int dtmf, void* user_data){
	CLinphoneCore* lc = (CLinphoneCore*)user_data;
	CStaticManager::linphone_gtk_dtmf_received(lc, dtmf);
}





void CLinphoneCore::linphone_core_stop_media_streams(){
	if (audiostream!=NULL) {
		audio_stream_stop(audiostream);
		audiostream=NULL;
	}
#ifdef VIDEO_ENABLED
	if (videostream!=NULL){
		if (video_conf.display && video_conf.capture)
			video_stream_stop(videostream);
		else if (video_conf.display)
			video_stream_recv_only_stop(videostream);
		else if (video_conf.capture)
			video_stream_send_only_stop(videostream);
		videostream=NULL;
	}
	if (linphone_core_video_preview_enabled()){
		if (previewstream==NULL){
			previewstream=video_preview_start(video_conf.device, video_conf.vsize,this->m_VHwnd);
		}
	}
#endif
}

int CLinphoneCore::linphone_core_accept_call( const char *url)
{
	char *sdpmesg;
	osip_message_t *msg=NULL;

	int err;
	bool_t offering=FALSE;

	if (call==NULL){
		return -1;
	}

	if (call->state==LCStateAVRunning){
		/*call already accepted*/
		return -1;
	}

	/*stop ringing */
	if (ringstream!=NULL) {
		ms_message("stop ringing");
		ring_stop(ringstream);
		ms_message("ring stopped");
		ringstream=NULL;
	}
	/* sends a 200 OK */
	err=eXosip_call_build_answer(call->tid,200,&msg);
	if (err<0 || msg==NULL){
		OutputDebugString(_T("Fail to build answer for call: err=%i"));
		return -1;
	}
	/*try to be best-effort in giving real local or routable contact address,
	except when the user choosed to override the ipaddress */
	if (linphone_core_get_firewall_policy()!=LINPHONE_POLICY_USE_NAT_ADDRESS)
		fix_contact(msg,call->localip,NULL);
	/*if a sdp answer is computed, send it, else send an offer */
	sdpmesg=call->sdpctx->answerstr;
	if (sdpmesg==NULL){
		offering=TRUE;
		ms_message("generating sdp offer");
		sdpmesg=pSdpHandler->sdp_context_get_offer(call->sdpctx);

		if (sdpmesg==NULL){
			OutputDebugString(_T("fail to generate sdp offer !"));
			return -1;
		}
		linphone_set_sdp(msg,sdpmesg);
		linphone_core_init_media_streams();
	}else{
		linphone_set_sdp(msg,sdpmesg);
	}
	eXosip_lock();
	eXosip_call_send_answer(call->tid,200,msg);
	eXosip_unlock();
	vtable->linphone_gtk_general_state(this,(LinphoneGeneralState *)("Connected."));
	pGeneralState->gstate_new_state(this, GSTATE_CALL_IN_CONNECTED, NULL);

	if (!offering){
		pPhoneCore->linphone_core_start_media_streams(call);
	}
	ms_message("call answered.");
	return 0;
}

int CLinphoneCore::linphone_core_terminate_call( const char *url)
{

	if (call==NULL){
		return -1;
	}
	

	eXosip_lock();
	eXosip_call_terminate(call->cid,call->did);
	eXosip_unlock();

	/*stop ringing*/
	if (ringstream!=NULL) {
		ring_stop(ringstream);
		ringstream=NULL;
	}
	linphone_core_stop_media_streams();
	vtable->linphone_gtk_display_status(this,("Call ended") );
	pGeneralState->gstate_new_state(this, GSTATE_CALL_END, NULL);
	linphone_call_destroy(call);
	return 0;
}

bool_t CLinphoneCore::linphone_core_in_call( ){
	return call!=NULL;
}

int CLinphoneCore::linphone_core_send_publish(
	LinphoneOnlineStatus presence_mode)
{
	const MSList *elem;
	for (elem=pProxy->linphone_core_get_proxy_config_list(this);elem!=NULL;elem=ms_list_next(elem)){
		CProxyConfig *cfg=(CProxyConfig*)elem->data;
		if (cfg->publish) pProxy->linphone_proxy_config_send_publish(cfg,presence_mode);
	}
	return 0;
}

void CLinphoneCore::linphone_core_set_inc_timeout( int seconds){
	sip_conf.inc_timeout=seconds;
}

int CLinphoneCore::linphone_core_get_inc_timeout(){
	return sip_conf.inc_timeout;
}

void CLinphoneCore::linphone_core_set_presence_info(int minutes_away,
													const char *contact,
													LinphoneOnlineStatus presence_mode)
{
	int contactok=-1;
	if (minutes_away>0) minutes_away=minutes_away;
	if (contact!=NULL) {
		osip_from_t *url;
		osip_from_init(&url);
		contactok=osip_from_parse(url,contact);
		if (contactok>=0) {
			ms_message("contact url is correct.");
		}
		osip_from_free(url);

	}
	if (contactok>=0){
		if (alt_contact!=NULL) ms_free(alt_contact);
		alt_contact=ms_strdup(contact);
	}
	if (presence_mode!=presence_mode){
		pPresence->linphone_core_notify_all_friends(this,presence_mode);
		/* 
		Improve the use of all LINPHONE_STATUS available.
		!TODO Do not mix "presence status" with "answer status code"..
		Use correct parameter to follow sip_if_match/sip_etag.
		*/
		linphone_core_send_publish(presence_mode);
	}
	prev_mode=presence_mode;
	presence_mode=presence_mode;

}

LinphoneOnlineStatus CLinphoneCore::linphone_core_get_presence_info(){
	return presence_mode;
}

/* sound functions */
int CLinphoneCore::linphone_core_get_play_level()
{
	return sound_conf.play_lev;
}
int CLinphoneCore::linphone_core_get_ring_level()
{
	return sound_conf.ring_lev;
}
int CLinphoneCore::linphone_core_get_rec_level(){
	return sound_conf.rec_lev;
}
void CLinphoneCore::linphone_core_set_ring_level( int level){
	MSSndCard *sndcard;
	sound_conf.ring_lev=level;
	sndcard=sound_conf.ring_sndcard;
	if (sndcard) ms_snd_card_set_level(sndcard,MS_SND_CARD_PLAYBACK,level);
}

void CLinphoneCore::linphone_core_set_play_level( int level){
	MSSndCard *sndcard;
	sound_conf.play_lev=level;
	sndcard=sound_conf.play_sndcard;
	if (sndcard) ms_snd_card_set_level(sndcard,MS_SND_CARD_PLAYBACK,level);
}

void CLinphoneCore::linphone_core_set_rec_level( int level)
{
	MSSndCard *sndcard;
	sound_conf.rec_lev=level;
	sndcard=sound_conf.capt_sndcard;
	if (sndcard) ms_snd_card_set_level(sndcard,MS_SND_CARD_CAPTURE,level);
}
MSSndCard *CLinphoneCore::get_card_from_string_id(const char *devid, unsigned int cap){
	MSSndCard *sndcard=NULL;
	if (devid!=NULL){
		sndcard=ms_snd_card_manager_get_card(ms_snd_card_manager_get(),devid);
		if (sndcard!=NULL && 
			(ms_snd_card_get_capabilities(sndcard) & cap)==0 ){
				OutputDebugString(_T("%s card does not have the %s capability, ignoring."));
				sndcard=NULL;
		}
	}
	if (sndcard==NULL) {
		/* get a card that has read+write capabilities */
		sndcard=ms_snd_card_manager_get_default_card(ms_snd_card_manager_get());
		/* otherwise refine to the first card having the right capability*/
		if (sndcard==NULL){
			const MSList *elem=ms_snd_card_manager_get_list(ms_snd_card_manager_get());
			for(;elem!=NULL;elem=elem->next){
				sndcard=(MSSndCard*)elem->data;
				if (ms_snd_card_get_capabilities(sndcard) & cap) break;
			}
		}
		if (sndcard==NULL){/*looks like a bug! take the first one !*/
			const MSList *elem=ms_snd_card_manager_get_list(ms_snd_card_manager_get());
			sndcard=(MSSndCard*)elem->data;
		}
	}
	if (sndcard==NULL) OutputDebugString(_T("Could not find a suitable soundcard !"));
	return sndcard;
}

bool_t CLinphoneCore::linphone_core_sound_device_can_capture( const char *devid){
	MSSndCard *sndcard;
	sndcard=ms_snd_card_manager_get_card(ms_snd_card_manager_get(),devid);
	if (sndcard!=NULL && (ms_snd_card_get_capabilities(sndcard) & MS_SND_CARD_CAP_CAPTURE)) return TRUE;
	return FALSE;
}

bool_t CLinphoneCore::linphone_core_sound_device_can_playback( const char *devid){
	MSSndCard *sndcard;
	sndcard=ms_snd_card_manager_get_card(ms_snd_card_manager_get(),devid);
	if (sndcard!=NULL && (ms_snd_card_get_capabilities(sndcard) & MS_SND_CARD_CAP_PLAYBACK)) return TRUE;
	return FALSE;
}

int CLinphoneCore::linphone_core_set_ringer_device( const char * devid){
	MSSndCard *card=get_card_from_string_id(devid,MS_SND_CARD_CAP_PLAYBACK);
	sound_conf.ring_sndcard=card;
	if (card && ready)
		pConfig->lp_config_set_string("sound","ringer_dev_id",ms_snd_card_get_string_id(card));
	return 0;
}

int CLinphoneCore::linphone_core_set_playback_device( const char * devid){
	MSSndCard *card=get_card_from_string_id(devid,MS_SND_CARD_CAP_PLAYBACK);
	sound_conf.play_sndcard=card;
	if (card && ready)
		pConfig->lp_config_set_string("sound","playback_dev_id",ms_snd_card_get_string_id(card));
	return 0;
}

int CLinphoneCore::linphone_core_set_capture_device( const char * devid){
	MSSndCard *card=get_card_from_string_id(devid,MS_SND_CARD_CAP_CAPTURE);
	sound_conf.capt_sndcard=card;
	if (card && ready)
		pConfig->lp_config_set_string("sound","capture_dev_id",ms_snd_card_get_string_id(card));
	return 0;
}

const char * CLinphoneCore::linphone_core_get_ringer_device()
{
	if (sound_conf.ring_sndcard) return ms_snd_card_get_string_id(sound_conf.ring_sndcard);
	return NULL;
}

const char * CLinphoneCore::linphone_core_get_playback_device()
{
	return sound_conf.play_sndcard ? ms_snd_card_get_string_id(sound_conf.play_sndcard) : NULL;
}

const char * CLinphoneCore::linphone_core_get_capture_device()
{
	return sound_conf.capt_sndcard ? ms_snd_card_get_string_id(sound_conf.capt_sndcard) : NULL;
}

/* returns a static array of string describing the sound devices */ 
const char**  CLinphoneCore::linphone_core_get_sound_devices(){
	build_sound_devices_table();
	return sound_conf.cards;
}

const char**  CLinphoneCore::linphone_core_get_video_devices(){
	return video_conf.cams;
}

char CLinphoneCore::linphone_core_get_sound_source()
{
	return sound_conf.source;
}

void CLinphoneCore::linphone_core_set_sound_source( char source)
{
	MSSndCard *sndcard=sound_conf.capt_sndcard;
	sound_conf.source=source;
	if (!sndcard) return;
	switch(source){
		case 'm':
			ms_snd_card_set_capture(sndcard,MS_SND_CARD_MIC);
			break;
		case 'l':
			ms_snd_card_set_capture(sndcard,MS_SND_CARD_LINE);
			break;
	}

}

void CLinphoneCore::linphone_core_set_ring(const char *path){
	if (sound_conf.local_ring!=0){
		ms_free(sound_conf.local_ring);
	}
	sound_conf.local_ring=ms_strdup(path);
	if (ready && sound_conf.local_ring)
		pConfig->lp_config_set_string("sound","local_ring",sound_conf.local_ring);
}

const char *CLinphoneCore::linphone_core_get_ring(){
	return sound_conf.local_ring;
}

void CLinphoneCore::notify_end_of_ring(void *ud ,unsigned int event, void * arg){
	CLinphoneCore *lc=(CLinphoneCore*)ud;
	preview_finished=1;
}

int CLinphoneCore::linphone_core_preview_ring( const char *ring,LinphoneCoreCbFunc func,void * userdata)
{
	if (ringstream!=0){
		OutputDebugString(_T("Cannot start ring now,there's already a ring being played"));
		return -1;
	}
	lc_callback_obj_init(&preview_finished_cb,func,userdata);
	preview_finished=0;
	if (sound_conf.ring_sndcard!=NULL){
		ringstream=ring_start_with_cb(ring,2000,sound_conf.ring_sndcard,CLinphoneCore::notify_end_of_ring,(void *)this);
	}
	return 0;
}


void CLinphoneCore::linphone_core_set_ringback( const char *path){
	if (sound_conf.remote_ring!=0){
		ms_free(sound_conf.remote_ring);
	}
	sound_conf.remote_ring=ms_strdup(path);
}

const char * CLinphoneCore::linphone_core_get_ringback(){
	return sound_conf.remote_ring;
}

void CLinphoneCore::linphone_core_enable_echo_cancelation( bool_t val){
	sound_conf.ec=val;
	if (ready)
		pConfig->lp_config_set_int("sound","echocancelation",val);
}

bool_t CLinphoneCore::linphone_core_echo_cancelation_enabled(){
	return sound_conf.ec;
}

void CLinphoneCore::linphone_core_enable_echo_limiter( bool_t val){
	sound_conf.ea=val;
}

bool_t CLinphoneCore::linphone_core_echo_limiter_enabled(){
	return sound_conf.ea;
}

void CLinphoneCore::linphone_core_enable_agc( bool_t val){
	sound_conf.agc=val;
}

bool_t CLinphoneCore::linphone_core_agc_enabled(){
	return sound_conf.agc;
}


void CLinphoneCore::linphone_core_send_dtmf(char dtmf)
{
	if (linphone_core_get_use_info_for_dtmf()==0){
		/* In Band DTMF */
		if (audiostream!=NULL){
			audio_stream_send_dtmf(audiostream,dtmf);
		}
	}else{
		char dtmf_body[1000];
		char clen[10];
		osip_message_t *msg=NULL;
		/* Out of Band DTMF (use INFO method) */

		if (call==NULL){
			return;
		}
		eXosip_call_build_info(call->did,&msg);
		snprintf(dtmf_body, 999, "Signal=%c\r\nDuration=250\r\n", dtmf);
		osip_message_set_body(msg,dtmf_body,strlen(dtmf_body));
		osip_message_set_content_type(msg,"application/dtmf-relay");
		snprintf(clen,sizeof(clen),"%lu",(unsigned long)strlen(dtmf_body));
		osip_message_set_content_length(msg,clen);

		eXosip_lock();
		eXosip_call_send_request(call->did,msg);
		eXosip_unlock();
	}
}

void CLinphoneCore::linphone_core_set_stun_server( const char *server){
	if (net_conf.stun_server!=NULL)
		ms_free(net_conf.stun_server);
	if (server)
		net_conf.stun_server=ms_strdup(server);
	else net_conf.stun_server=NULL;
	apply_nat_settings=TRUE;
}

const char * CLinphoneCore::linphone_core_get_stun_server(){
	return net_conf.stun_server;
}

const char * CLinphoneCore::linphone_core_get_relay_addr(){
	return net_conf.relay;
}

int CLinphoneCore::linphone_core_set_relay_addr( const char *addr){
	if (net_conf.relay!=NULL){
		ms_free(net_conf.relay);
		net_conf.relay=NULL;
	}
	if (addr){
		net_conf.relay=ms_strdup(addr);
	}
	return 0;
}

/* void CLinphoneCore::apply_nat_settings(){
char *wmsg;
char *tmp=NULL;
int err;
struct addrinfo hints,*res;
const char *addr=net_conf.nat_address;

if (net_conf.firewall_policy==LINPHONE_POLICY_USE_NAT_ADDRESS){
if (addr==NULL || strlen(addr)==0){
vtable->linphone_gtk_display_warning(lc,("No nat/firewall address supplied !"));
linphone_core_set_firewall_policy(lc,LINPHONE_POLICY_NO_FIREWALL);
}

memset(&hints,0,sizeof(struct addrinfo));
if (sip_conf.ipv6_enabled)
hints.ai_family=AF_INET6;
else 
hints.ai_family=AF_INET;
hints.ai_socktype = SOCK_DGRAM;
err=getaddrinfo(addr,NULL,&hints,&res);
if (err!=0){
wmsg=ortp_strdup_printf(("Invalid nat address '%s' : %s"),
addr, gai_strerror(err));
ms_warning(wmsg); // what is this for ?
vtable->linphone_gtk_display_warning(lc, wmsg);
ms_free(wmsg);
linphone_core_set_firewall_policy(lc,LINPHONE_POLICY_NO_FIREWALL);
return;
}

tmp=(char*)ms_malloc0(50);
err=getnameinfo(res->ai_addr,res->ai_addrlen,tmp,50,NULL,0,NI_NUMERICHOST);
if (err!=0){
wmsg=ortp_strdup_printf(("Invalid nat address '%s' : %s"),
addr, gai_strerror(err));
ms_warning(wmsg); // what is this for ?
vtable->linphone_gtk_display_warning(lc, wmsg);
ms_free(wmsg);
ms_free(tmp);
freeaddrinfo(res);
linphone_core_set_firewall_policy(lc,LINPHONE_POLICY_NO_FIREWALL);
return;
}
freeaddrinfo(res);
}

if (net_conf.firewall_policy==LINPHONE_POLICY_USE_NAT_ADDRESS){
if (tmp!=NULL){
if (!net_conf.nat_sdp_only){
eXosip_set_option(EXOSIP_OPT_SET_IPV4_FOR_GATEWAY,tmp);

}
ms_free(tmp);
}
else{
eXosip_set_option(EXOSIP_OPT_SET_IPV4_FOR_GATEWAY,NULL);
eXosip_masquerade_contact("",0);
}
}
else {
eXosip_set_option(EXOSIP_OPT_SET_IPV4_FOR_GATEWAY,NULL);
eXosip_masquerade_contact("",0);	
}
}

*/
void CLinphoneCore::linphone_core_set_nat_address( const char *addr)
{
	if (net_conf.nat_address!=NULL){
		ms_free(net_conf.nat_address);
	}
	if (addr!=NULL) net_conf.nat_address=ms_strdup(addr);
	else net_conf.nat_address=NULL;
	apply_nat_settings=TRUE;
}

const char *CLinphoneCore::linphone_core_get_nat_address()
{
	return net_conf.nat_address;
}

void CLinphoneCore::linphone_core_set_firewall_policy( LinphoneFirewallPolicy pol){
	net_conf.firewall_policy=pol;
	apply_nat_settings=TRUE;
}

LinphoneFirewallPolicy CLinphoneCore::linphone_core_get_firewall_policy(){
	return (LinphoneFirewallPolicy)net_conf.firewall_policy;
}

MSList * CLinphoneCore::linphone_core_get_call_logs(){
	missed_calls=0;
	return call_logs;
}
void CLinphoneCore::toggle_video_preview( bool_t val){
#ifdef VIDEO_ENABLED
	if (videostream==NULL){
		if (val){
			if (previewstream==NULL){
				previewstream=video_preview_start(video_conf.device,
					video_conf.vsize,this->m_VHwnd);
			}
		}else{
			if (previewstream!=NULL){
				video_preview_stop(previewstream);
				previewstream=NULL;
			}
		}
	}
#endif
}

void CLinphoneCore::linphone_core_enable_video( bool_t vcap_enabled, bool_t display_enabled){
#ifndef VIDEO_ENABLED
	if (vcap_enabled || display_enabled)
		OutputDebugString(_T("This version of linphone was built without video support."));
#endif
	video_conf.capture=vcap_enabled;
	video_conf.display=display_enabled;

	if (ready){
		pConfig->lp_config_set_int("video","display",display_enabled);
		pConfig->lp_config_set_int("video","capture",vcap_enabled);
	}

	/* need to re-apply network bandwidth settings*/
	linphone_core_set_download_bandwidth(
		linphone_core_get_download_bandwidth());
	linphone_core_set_upload_bandwidth(
		linphone_core_get_upload_bandwidth());
}

bool_t CLinphoneCore::linphone_core_video_enabled(){
	return (video_conf.display || video_conf.capture);
}

void CLinphoneCore::linphone_core_enable_video_preview( bool_t val){
	video_conf.show_local=val;
	if (ready) pConfig->lp_config_set_int("video","show_local",val);
}

bool_t CLinphoneCore::linphone_core_video_preview_enabled(){
	return video_conf.show_local;
}

void CLinphoneCore::linphone_core_enable_self_view( bool_t val){
	video_conf.selfview=val;
#ifdef VIDEO_ENABLED
	if (videostream){
//		video_stream_enable_self_view(videostream,val);
	}
#endif
}

bool_t CLinphoneCore::linphone_core_self_view_enabled(){
	return video_conf.selfview;
}

int CLinphoneCore::linphone_core_set_video_device( const char *id){
#ifdef VIDEO_ENABLED
	MSWebCam *olddev=video_conf.device;
	const char *vd;
	if (id!=NULL){
		video_conf.device=ms_web_cam_manager_get_cam(ms_web_cam_manager_get(),id);
		if (video_conf.device==NULL){
		//	ms_warning("Could not found video device %s",id);
		}
	}
	if (video_conf.device==NULL)
		video_conf.device=ms_web_cam_manager_get_default_cam(ms_web_cam_manager_get());
	if (olddev!=NULL && olddev!=video_conf.device){
		toggle_video_preview(FALSE);/*restart the video local preview*/
	}
	if (ready && video_conf.device){
		vd=ms_web_cam_get_string_id(video_conf.device);
		if (vd && strstr(vd,"Static picture")!=NULL){
			vd=NULL;
		}
		pConfig->lp_config_set_string("video","device",vd);
	}
#endif
	return 0;
}

const char *CLinphoneCore::linphone_core_get_video_device(){
#ifdef VIDEO_ENABLED
	if (video_conf.device) return ms_web_cam_get_string_id(video_conf.device);
#endif
	return NULL;
}

unsigned long CLinphoneCore::linphone_core_get_native_video_window_id(){
#ifdef VIDEO_ENABLED
	if (videostream)
		return video_stream_get_native_window_id(videostream);
	if (previewstream)
		return video_stream_get_native_window_id(previewstream);
#endif
	return 0;
}



const MSVideoSizeDef *CLinphoneCore::linphone_core_get_supported_video_sizes(){
	return supported_resolutions;
}
MSVideoSize CLinphoneCore::video_size_get_by_name(const char *name){
	MSVideoSizeDef *pdef=supported_resolutions;
	for(;pdef->name!=NULL;pdef++){
		if (strcasecmp(name,pdef->name)==0){
			return pdef->vsize;
		}
	}
	OutputDebugString(_T("Video resolution %s is not supported in linphone."));
	MSVideoSize mms;
	mms.width=0;
	mms.height=0;
	return mms;
}

const char *CLinphoneCore::video_size_get_name(MSVideoSize vsize){
	MSVideoSizeDef *pdef=supported_resolutions;
	for(;pdef->name!=NULL;pdef++){
		if (pdef->vsize.width==vsize.width && pdef->vsize.height==vsize.height){
			return pdef->name;
		}
	}
	return NULL;
}
bool_t CLinphoneCore::video_size_supported(MSVideoSize vsize){
	if (video_size_get_name(vsize)) return TRUE;
	OutputDebugString(_T("Video resolution %ix%i is not supported in linphone."));
	return FALSE;
}


void CLinphoneCore::linphone_core_set_preferred_video_size( MSVideoSize vsize){
	if (video_size_supported(vsize)){
		MSVideoSize oldvsize=video_conf.vsize;
		video_conf.vsize=vsize;
		if (!ms_video_size_equal(oldvsize,vsize) && previewstream!=NULL){
			toggle_video_preview(FALSE);
			toggle_video_preview(TRUE);
		}
		if (ready)
			pConfig->lp_config_set_string("video","size",video_size_get_name(vsize));
	}
}

void CLinphoneCore::linphone_core_set_preferred_video_size_by_name( const char *name){
	MSVideoSize vsize=video_size_get_by_name(name);
	if (vsize.width!=0)	linphone_core_set_preferred_video_size(vsize);
	else{
		MSVideoSize mms =MS_VIDEO_SIZE_CIF;
		linphone_core_set_preferred_video_size(mms);
	}

}

MSVideoSize CLinphoneCore::linphone_core_get_preferred_video_size(){
	return video_conf.vsize;
}

void CLinphoneCore::linphone_core_use_files( bool_t yesno){
	use_files=yesno;
}

void CLinphoneCore::linphone_core_set_play_file( const char *file){
	if (play_file!=NULL){
		ms_free(play_file);
		play_file=NULL;
	}
	if (file!=NULL) {
		play_file=ms_strdup(file);
		if (audiostream)
			audio_stream_play(audiostream,file);
	}
}

void CLinphoneCore::linphone_core_set_record_file( const char *file){
	if (rec_file!=NULL){
		ms_free(rec_file);
		rec_file=NULL;
	}
	if (file!=NULL) {
		rec_file=ms_strdup(file);
		if (audiostream) 
			audio_stream_record(audiostream,file);
	}
}


void *CLinphoneCore::linphone_core_get_user_data(){
	return data;
}

int CLinphoneCore::linphone_core_get_mtu(){
	return net_conf.mtu;
}

void CLinphoneCore::linphone_core_set_mtu( int mtu){
	net_conf.mtu=mtu;
	if (mtu>0){
		if (mtu<500){
			OutputDebugString(_T("MTU too small !"));
			mtu=500;
		}
		ms_set_mtu(mtu);
		ms_message("MTU is supposed to be %i, rtp payload max size will be %i",mtu, ms_get_payload_max_size());
	}else ms_set_mtu(0);//use mediastreamer2 default value
}

void CLinphoneCore::linphone_core_set_waiting_callback( LinphoneWaitingCallback cb, void *user_context){
	wait_cb=cb;
	wait_ctx=user_context;
}

void CLinphoneCore::linphone_core_start_waiting( const char *purpose){
	if (wait_cb){
		wait_ctx=wait_cb(this,wait_ctx,LinphoneWaitingStart,purpose,0);
	}
}

void CLinphoneCore::linphone_core_update_progress( const char *purpose, float progress){
	if (wait_cb){
		wait_ctx=wait_cb(this,wait_ctx,LinphoneWaitingProgress,purpose,progress);
	}else{
#ifdef WIN32
		Sleep(50000);
#else
		usleep(50000);
#endif
	}
}

void CLinphoneCore::linphone_core_stop_waiting(){
	if (wait_cb){
		wait_ctx=wait_cb(this,wait_ctx,LinphoneWaitingFinished,NULL,0);
	}
}

void CLinphoneCore::net_config_uninit()
{
	net_config_t *config=&net_conf;
	pConfig->lp_config_set_int("net","download_bw",config->download_bw);
	pConfig->lp_config_set_int("net","upload_bw",config->upload_bw);

	if (config->stun_server!=NULL)
		pConfig->lp_config_set_string("net","stun_server",config->stun_server);
	if (config->nat_address!=NULL)
		pConfig->lp_config_set_string("net","nat_address",config->nat_address);
	pConfig->lp_config_set_int("net","firewall_policy",config->firewall_policy);
	pConfig->lp_config_set_int("net","mtu",config->mtu);
}


void CLinphoneCore::sip_config_uninit()
{
	MSList *elem;
	int i;
	sip_config_t *config=&sip_conf;
	pConfig->lp_config_set_int("sip","sip_port",config->sip_port);
	pConfig->lp_config_set_int("sip","guess_hostname",config->guess_hostname);
	pConfig->lp_config_set_string("sip","contact",config->contact);
	pConfig->lp_config_set_int("sip","inc_timeout",config->inc_timeout);
	pConfig->lp_config_set_int("sip","use_info",config->use_info);
	pConfig->lp_config_set_int("sip","use_ipv6",config->ipv6_enabled);
	pConfig->lp_config_set_int("sip","register_only_when_network_is_up",config->register_only_when_network_is_up);
	for(elem=config->proxies,i=0;elem!=NULL;elem=ms_list_next(elem),i++){
		CProxyConfig *cfg=(CProxyConfig*)(elem->data);
		pProxy->linphone_proxy_config_write_to_config_file(pConfig,cfg,i);
		pProxy->linphone_proxy_config_edit(cfg);	/* to unregister */
	}

	if (exosip_running)
	{
		int i;
		for (i=0;i<20;i++)
		{
			eXosip_event_t *ev;
			while((ev=eXosip_event_wait(0,0))!=NULL){
				pPhoneCore->linphone_core_process_event(ev);
			}
			eXosip_automatic_action();
#ifndef WIN32
			usleep(100000);
#else
			Sleep(100);
#endif
		}
	}

	pProxy->linphone_proxy_config_write_to_config_file(pConfig,NULL,i);	/*mark the end */

	for(elem=auth_info,i=0;elem!=NULL;elem=ms_list_next(elem),i++){
		LinphoneAuthInfo *ai=(LinphoneAuthInfo*)(elem->data);
		pPrivate->linphone_auth_info_write_config(pConfig,ai,i);
	}
	pPrivate->linphone_auth_info_write_config(pConfig,NULL,i); /* mark the end */
}

void CLinphoneCore::rtp_config_uninit()
{
	rtp_config_t *config=&rtp_conf;
	pConfig->lp_config_set_int("rtp","audio_rtp_port",config->audio_rtp_port);
	pConfig->lp_config_set_int("rtp","video_rtp_port",config->video_rtp_port);
	pConfig->lp_config_set_int("rtp","audio_jitt_comp",config->audio_jitt_comp);
	pConfig->lp_config_set_int("rtp","video_jitt_comp",config->audio_jitt_comp);
	pConfig->lp_config_set_int("rtp","nortp_timeout",config->nortp_timeout);
}

void CLinphoneCore::sound_config_uninit()
{
	sound_config_t *config=&sound_conf;
	ms_free(config->cards);

	pConfig->lp_config_set_string("sound","remote_ring",config->remote_ring);

	if (config->local_ring) ms_free(config->local_ring);
	if (config->remote_ring) ms_free(config->remote_ring);
	ms_snd_card_manager_destroy();
}

void CLinphoneCore::video_config_uninit()
{

}

void CLinphoneCore::codecs_config_uninit()
{
	PayloadType *pt;
	codecs_config_t *config=&codecs_conf;
	MSList *node;
	char key[50];
	int index;
	index=0;
	for(node=config->audio_codecs;node!=NULL;node=ms_list_next(node)){
		pt=(PayloadType*)(node->data);
		sprintf(key,"audio_codec_%i",index);
		pConfig->lp_config_set_string(key,"mime",pt->mime_type);
		pConfig->lp_config_set_int(key,"rate",pt->clock_rate);
		pConfig->lp_config_set_int(key,"enabled",pMisc->payload_type_enabled(pt));
		index++;
	}
	index=0;
	for(node=config->video_codecs;node!=NULL;node=ms_list_next(node)){
		pt=(PayloadType*)(node->data);
		sprintf(key,"video_codec_%i",index);
		pConfig->lp_config_set_string(key,"mime",pt->mime_type);
		pConfig->lp_config_set_int(key,"rate",pt->clock_rate);
		pConfig->lp_config_set_int(key,"enabled",pMisc->payload_type_enabled(pt));
		pConfig->lp_config_set_string(key,"recv_fmtp",pt->recv_fmtp);
		index++;
	}
}

void CLinphoneCore::ui_config_uninit()
{
	if (friends){
		ms_list_for_each(friends,(void (*)(void *))CFriend::linphone_friend_destroy);
		ms_list_free(friends);
		friends=NULL;
	}
}

CConfigure *CLinphoneCore::linphone_core_get_config(){
	return pConfig;

}

void CLinphoneCore::linphone_core_uninit()
{
	if (call){
		int i;
		linphone_core_terminate_call(NULL);
		for(i=0;i<10;++i){
#ifndef WIN32
			usleep(50000);
#else
			Sleep(50);
#endif
		 pPhoneCore->linphone_core_iterate();
		}
	}
	pGeneralState->gstate_new_state(this, GSTATE_POWER_SHUTDOWN, NULL);
#ifdef VIDEO_ENABLED
	if (previewstream!=NULL){
		video_preview_stop(previewstream);
		previewstream=NULL;
	}
#endif
	/* save all config */
	net_config_uninit();
	sip_config_uninit();
	pConfig->lp_config_set_int("sip","default_proxy",pProxy->linphone_core_get_default_proxy(this,NULL));
	rtp_config_uninit();
	sound_config_uninit();
	video_config_uninit();
	codecs_config_uninit();
	ui_config_uninit();
	if (pConfig->lp_config_needs_commit()) pConfig->lp_config_sync();
	pConfig->lp_config_destroy();
	pSipsetup->sip_setup_unregister_all();
#ifdef VIDEO_ENABLED
	//if (payload_type_h264_packetization_mode_1!=NULL)
	//	payload_type_destroy(payload_type_h264_packetization_mode_1);
#endif
	ortp_exit();
	eXosip_quit();
	exosip_running=FALSE;
	pGeneralState->gstate_new_state(this, GSTATE_POWER_OFF, NULL);
}

void CLinphoneCore::linphone_core_destroy(){
	linphone_core_uninit();
	//ms_free(this);
}

void CLinphoneCore::lc_callback_obj_init(LCCallbackObj *obj,LinphoneCoreCbFunc func,void* ud)
{
	obj->_func=func;
	obj->_user_data=ud;
}
void CLinphoneCore::linphone_call_destroy(LinphoneCall *obj)
{
	pPresence->linphone_core_notify_all_friends(obj->core,obj->core->prev_mode);
	pPhoneCore->linphone_call_log_completed(obj->log,obj);
	pMisc->linphone_core_update_allocated_audio_bandwidth(obj->core);
	if (obj->profile!=NULL) rtp_profile_destroy(obj->profile);
	if (obj->sdpctx!=NULL) pSdpHandler->sdp_context_free(obj->sdpctx);
	ms_free(obj);
}
int CLinphoneCore::lc_callback_obj_invoke(LCCallbackObj *obj){
	if (obj->_func!=NULL) obj->_func(this,obj->_user_data);
	return 0;
}



