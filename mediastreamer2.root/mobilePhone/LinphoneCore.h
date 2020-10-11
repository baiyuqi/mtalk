#pragma once
class CStaticManager;
class CConfigure;
class CMisc;
class CProxy;
class CPrivate;
class CFriend;
class CSipsetup;
class CGeneralState;
class CSdpHandler;
class CPhoneCore;
class CPresence;
class CProxyConfig;
#include "commonStruct.h"
#include "mediastreamer2/mediastream.h"
#include "mediastreamer2/msvolume.h"
#include "mediastreamer2/msequalizer.h"
#define PAYLOAD_TYPE_ENABLED	PAYLOAD_TYPE_USER_FLAG_0
static	bool_t preview_finished;
class CLinphoneCore
{
public:
	CLinphoneCore(CPhoneCore * phoneCore);
	CLinphoneCore(void);
	~CLinphoneCore(void);

public:
	CPhoneCore * pPhoneCore;
	CConfigure *pConfig;
	CMisc *pMisc;
	CProxy *pProxy;
	CPrivate *pPrivate;
	CFriend *pFriend;
	CSipsetup *pSipsetup;
	CSdpHandler *pSdpHandler;
	CPresence *pPresence;
	CGeneralState *pGeneralState;
	CStaticManager *vtable;
public:
	
	
	net_config_t net_conf;
	sip_config_t sip_conf;
	rtp_config_t rtp_conf;
	sound_config_t sound_conf;
	video_config_t video_conf;
	codecs_config_t codecs_conf;
	ui_config_t ui_conf;
	autoreplier_config_t autoreplier_conf;
	CProxyConfig *default_proxy;
	MSList *friends;
	MSList *auth_info;
	struct _RingStream *ringstream;
	LCCallbackObj preview_finished_cb;

	LinphoneCall *call;   /* the current call, in the future it will be a list of calls (conferencing)*/
	int rid; /*registration id*/
	MSList *queued_calls;	/* used by the autoreplier */
	MSList *call_logs;
	MSList *chatrooms;
	int max_call_logs;
	int missed_calls;
	struct _AudioStream *audiostream;  /**/
	struct _VideoStream *videostream;
	struct _VideoStream *previewstream;
	struct _RtpProfile *local_profile;
	MSList *subscribers;	/* unknown subscribers */
	int minutes_away;
	LinphoneOnlineStatus presence_mode;
	LinphoneOnlineStatus prev_mode;
	char *alt_contact;
	void *data;
	ms_mutex_t lock;
	char *play_file;
	char *rec_file;
	time_t prevtime;
	int dw_audio_bw;
	int up_audio_bw;
	int dw_video_bw;
	int up_video_bw;
	int audio_bw;
	int automatic_action;
	gstate_t gstate_power;
	gstate_t gstate_reg;
	gstate_t gstate_call;
	LinphoneWaitingCallback wait_cb;
	void *wait_ctx;
	bool_t use_files;
	bool_t apply_nat_settings;
	bool_t ready;
	HWND m_VHwnd;
public:
	bool_t exosip_running;
	char _ua_name[64];
	char _ua_version[64];
public:
	void discover_mtu(const char *remote);

	size_t my_strftime(char *s, size_t max, const char  *fmt,  LPSYSTEMTIME tm);
	void net_config_read ();
   void build_sound_devices_table();
	void sound_config_read();
	void sip_config_read();
	void rtp_config_read();
	PayloadType * get_codec(CConfigure *config, char* type,int index);
	void codecs_config_read();


	void ui_config_read();
	void autoreplier_config_init();

	void linphone_core_set_download_bandwidth( int bw);
	void linphone_core_set_upload_bandwidth( int bw);

	int linphone_core_get_download_bandwidth();

	int linphone_core_get_upload_bandwidth();
	const char * linphone_core_get_version(void);
	const MSList *linphone_core_get_audio_codecs();

	const MSList *linphone_core_get_video_codecs();
	int linphone_core_set_primary_contact( const char *contact);

	void linphone_core_get_local_ip( const char *dest, char *result);
	const char *linphone_core_get_primary_contact();

	void linphone_core_set_guess_hostname( bool_t val);
	bool_t linphone_core_get_guess_hostname();

	osip_from_t *linphone_core_get_primary_contact_parsed();

	int linphone_core_set_audio_codecs( MSList *codecs);
	int linphone_core_set_video_codecs( MSList *codecs);

	const MSList * linphone_core_get_friend_list();

	int linphone_core_get_audio_jittcomp();
	int linphone_core_get_audio_port();

	int linphone_core_get_video_port();

	int linphone_core_get_nortp_timeout();

	void linphone_core_set_audio_jittcomp( int value);
	void linphone_core_set_audio_port( int port);

	void linphone_core_set_video_port( int port);

	void linphone_core_set_nortp_timeout( int nortp_timeout);

	bool_t linphone_core_get_use_info_for_dtmf();

	void linphone_core_set_use_info_for_dtmf(bool_t use_info);

	int linphone_core_get_sip_port();


	 void apply_user_agent(void);

	void linphone_core_set_user_agent(const char *name, const char *ver);

	void linphone_core_set_sip_port(int port);
	bool_t linphone_core_ipv6_enabled();
	void linphone_core_enable_ipv6( bool_t val);
	void display_bandwidth(RtpSession *as, RtpSession *vs);
	void linphone_core_disconnected();
	//proxy改变，重新注册
	void proxy_update( time_t curtime);

	bool_t linphone_core_is_in_main_thread();
	osip_to_t *osip_to_create(const char *to);
	char *guess_route_if_any( osip_to_t *parsed_url);
	bool_t linphone_core_interpret_url( const char *url, char **real_url, osip_to_t **real_parsed_url, char **route);
	const char * linphone_core_get_identity();

	const char * linphone_core_get_route();

	void linphone_set_sdp(osip_message_t *sip, const char *sdpmesg);

	CProxyConfig * linphone_core_lookup_known_proxy( const char *uri);

	void fix_contact(osip_message_t *msg, const char *localip,CProxyConfig *dest_proxy);

	int linphone_core_invite( const char *url);
	int linphone_core_refer( const char *url);
	bool_t linphone_core_inc_invite_pending();
	void linphone_core_init_media_streams();
static	void linphone_core_dtmf_received(RtpSession* s, int dtmf, void* user_data);
	void linphone_core_stop_media_streams();

	int linphone_core_accept_call( const char *url);

	int linphone_core_terminate_call( const char *url);

	bool_t linphone_core_in_call();

	int linphone_core_send_publish(LinphoneOnlineStatus presence_mode);

	void linphone_core_set_inc_timeout( int seconds);

	int linphone_core_get_inc_timeout();
	void linphone_core_set_presence_info(int minutes_away,const char *contact,LinphoneOnlineStatus presence_mode);
	LinphoneOnlineStatus linphone_core_get_presence_info();
	
	int linphone_core_get_play_level();
	int linphone_core_get_ring_level();
	int linphone_core_get_rec_level();
	void linphone_core_set_ring_level( int level);
	void linphone_core_set_play_level( int level);
	void linphone_core_set_rec_level( int level);
	MSSndCard *get_card_from_string_id(const char *devid, unsigned int cap);
	bool_t linphone_core_sound_device_can_capture( const char *devid);

	bool_t linphone_core_sound_device_can_playback( const char *devid);

	int linphone_core_set_ringer_device( const char * devid);

	int linphone_core_set_playback_device( const char * devid);

	int linphone_core_set_capture_device( const char * devid);

	const char * linphone_core_get_ringer_device();
	const char * linphone_core_get_playback_device();

	const char * linphone_core_get_capture_device();
	
	const char**  linphone_core_get_sound_devices();

	const char**  linphone_core_get_video_devices();
	char linphone_core_get_sound_source();

	void linphone_core_set_sound_source( char source);
	void linphone_core_set_ring(const char *path);
	const char *linphone_core_get_ring();

static 	void notify_end_of_ring(void *ud ,unsigned int event, void * arg);

	int linphone_core_preview_ring( const char *ring,LinphoneCoreCbFunc func,void * userdata);


	void linphone_core_set_ringback( const char *path);

	const char * linphone_core_get_ringback();

	void linphone_core_enable_echo_cancelation( bool_t val);

	bool_t linphone_core_echo_cancelation_enabled();

	void linphone_core_enable_echo_limiter( bool_t val);

	bool_t linphone_core_echo_limiter_enabled();

	void linphone_core_enable_agc( bool_t val);

	bool_t linphone_core_agc_enabled();


	void linphone_core_send_dtmf(char dtmf);

	void linphone_core_set_stun_server( const char *server);

	const char * linphone_core_get_stun_server();

	const char * linphone_core_get_relay_addr();

	int linphone_core_set_relay_addr( const char *addr);

	//	void apply_nat_settings();

	void linphone_core_set_nat_address( const char *addr);

	const char *linphone_core_get_nat_address();
	void linphone_core_set_firewall_policy( LinphoneFirewallPolicy pol);
	LinphoneFirewallPolicy linphone_core_get_firewall_policy();

	MSList * linphone_core_get_call_logs();
	void toggle_video_preview( bool_t val);
	void linphone_core_enable_video( bool_t vcap_enabled, bool_t display_enabled);

	bool_t linphone_core_video_enabled();
	void linphone_core_enable_video_preview( bool_t val);

	bool_t linphone_core_video_preview_enabled();
	void linphone_core_enable_self_view( bool_t val);

	bool_t linphone_core_self_view_enabled();

	int linphone_core_set_video_device( const char *id);

	const char *linphone_core_get_video_device();

	unsigned long linphone_core_get_native_video_window_id();

	const MSVideoSizeDef *linphone_core_get_supported_video_sizes();
	MSVideoSize video_size_get_by_name(const char *name);
	const char *video_size_get_name(MSVideoSize vsize);
	bool_t video_size_supported(MSVideoSize vsize);
	void linphone_core_set_preferred_video_size( MSVideoSize vsize);
	void linphone_core_set_preferred_video_size_by_name( const char *name);
	MSVideoSize linphone_core_get_preferred_video_size();
	void linphone_core_use_files( bool_t yesno);
	void linphone_core_set_play_file( const char *file);
	void linphone_core_set_record_file( const char *file);
	void *linphone_core_get_user_data();

	int linphone_core_get_mtu();

	void linphone_core_set_mtu( int mtu);

	void linphone_core_set_waiting_callback( LinphoneWaitingCallback cb, void *user_context);

	void linphone_core_start_waiting( const char *purpose);

	void linphone_core_update_progress( const char *purpose, float progress);

	void linphone_core_stop_waiting();

	void net_config_uninit();
	void sip_config_uninit();
	void rtp_config_uninit();

	void sound_config_uninit();
	void video_config_uninit();
	void codecs_config_uninit();

	void ui_config_uninit();
	CConfigure *linphone_core_get_config();
	void linphone_core_destroy();
	void linphone_core_uninit();
void lc_callback_obj_init(LCCallbackObj *obj,LinphoneCoreCbFunc func,void* ud);
void linphone_call_destroy(LinphoneCall *obj);
int lc_callback_obj_invoke(LCCallbackObj *obj);
//LinphoneCall * linphone_call_new_outgoing(const osip_from_t *from, const osip_to_t *to);
	};
