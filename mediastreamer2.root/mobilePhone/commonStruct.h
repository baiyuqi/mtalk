
#include <osipparser2/osip_message.h>
#include <ortp/ortp.h>
#include <ortp/payloadtype.h>
#include <mediastreamer2/mscommon.h>
#include <mediastreamer2/msvideo.h>
class CLinphoneCore;
class CProxyConfig;
//typedef struct _LpConfig LpConfig;


static const char *liblinphone_version="1.0";


/* relative path where is stored local ring*/
#define LOCAL_RING "oldphone.wav"
/* same for remote ring (ringback)*/
#define REMOTE_RING "ringback.wav"


class  CStaticManager;

#define LINPHONE_IPADDR_SIZE 64
#define LINPHONE_HOSTNAME_SIZE 128


struct _MSSndCard;
struct _LinphoneCore;


void payload_type_set_enable(struct _PayloadType *pt,int value);
const char *payload_type_get_description(struct _PayloadType *pt);
int payload_type_get_bitrate(PayloadType *pt);
const char *payload_type_get_mime(PayloadType *pt);
int payload_type_get_rate(PayloadType *pt);


struct _LpConfig;

typedef struct sip_config
{
	char *contact;
	char *guessed_contact;
	int sip_port;
	MSList *proxies;
	MSList *deleted_proxies;
	int inc_timeout;	/*timeout after an un-answered incoming call is rejected*/
	bool_t use_info;
	bool_t guess_hostname;
	bool_t loopback_only;
	bool_t ipv6_enabled;
	bool_t sdp_200_ack;
	bool_t only_one_codec; /*in SDP answers*/
	bool_t register_only_when_network_is_up;
} sip_config_t;

typedef struct rtp_config
{
	int audio_rtp_port;
	int video_rtp_port;
	int audio_jitt_comp;  /*jitter compensation*/
	int video_jitt_comp;  /*jitter compensation*/
	int nortp_timeout;
}rtp_config_t;



typedef struct net_config
{
	char *nat_address;
	char *stun_server;
	char *relay;
	int download_bw;
	int upload_bw;
	int firewall_policy;
	int mtu;
	bool_t nat_sdp_only;
}net_config_t;


typedef struct sound_config
{
	struct _MSSndCard * ring_sndcard;	/* the playback sndcard currently used */
	struct _MSSndCard * play_sndcard;	/* the playback sndcard currently used */
	struct _MSSndCard * capt_sndcard; /* the capture sndcard currently used */
	const char **cards;
	int latency;	/* latency in samples of the current used sound device */
	char rec_lev;
	char play_lev;
	char ring_lev;
	char source;
	char *local_ring;
	char *remote_ring;
	bool_t ec;
	bool_t ea;
	bool_t agc;
} sound_config_t;

typedef struct codecs_config
{
	MSList *audio_codecs;  /* list of audio codecs in order of preference*/
	MSList *video_codecs;	/* for later use*/
}codecs_config_t;

typedef struct video_config{
	struct _MSWebCam *device;
	const char **cams;
	MSVideoSize vsize;
	bool_t capture;
	bool_t show_local;
	bool_t display;
	bool_t selfview; /*during calls*/
}video_config_t;

typedef struct ui_config
{
	int is_daemon;
	int is_applet;
	unsigned int timer_id;  /* the timer id for registration */
}ui_config_t;



typedef struct autoreplier_config
{
	int enabled;
	int after_seconds;		/* accept the call after x seconds*/
	int max_users;			/* maximum number of user that can call simultaneously */
	int max_rec_time;  	/* the max time of incoming voice recorded */
	int max_rec_msg;		/* maximum number of recorded messages */
	const char *message;		/* the path of the file to be played */
}autoreplier_config_t;


struct _LinphoneCore;
struct _sdp_context;
struct _SipSetupContext;
	
typedef struct _StreamParams
{
	int initialized;
	int line;
	int localport;
	int remoteport;
	int remotertcpport;
	int pt;
	char *relay_session_id;
	int natd_port;
	char remoteaddr[LINPHONE_HOSTNAME_SIZE];
	char natd_addr[LINPHONE_HOSTNAME_SIZE];
} StreamParams;

typedef enum _LCState{
	LCStateInit,
	LCStateRinging,
	LCStateAVRunning
}LCState;

typedef enum _LinphoneCallDir {LinphoneCallOutgoing, LinphoneCallIncoming} LinphoneCallDir;


typedef struct _LinphoneCall
{
	CLinphoneCore *core;
	StreamParams audio_params;
	StreamParams video_params;
	LinphoneCallDir dir;
	struct _RtpProfile *profile;	/*points to the local_profile or to the remote "guessed" profile*/
	struct _LinphoneCallLog *log;
	int cid; /*call id */
	int did; /*dialog id */
	int tid; /*last transaction id*/
	char localip[LINPHONE_IPADDR_SIZE]; /* our best guess for local ipaddress for this call */
	struct _sdp_context *sdpctx;
	time_t start_time; /*time at which the call was initiated*/
	time_t media_start_time; /*time at which it was accepted, media streams established*/
	LCState	state;
	bool_t auth_pending;
} LinphoneCall;

#define linphone_call_set_state(lcall,st)	(lcall)->state=(st)

typedef enum _LinphoneCallStatus { 
	LinphoneCallSuccess,
	LinphoneCallAborted,
	LinphoneCallMissed
} LinphoneCallStatus;

typedef struct _LinphoneCallLog{
	LinphoneCallDir dir;
	LinphoneCallStatus status;
	char *from;
	char *to;
	char start_date[128];
	int duration;
	
} LinphoneCallLog;

typedef enum{
	LinphoneSPWait,
	LinphoneSPDeny,
	LinphoneSPAccept
}LinphoneSubscribePolicy;


#define MAX_ENUM_LOOKUP_RESULTS 10

typedef struct enum_lookup_res{
	char *sip_address[MAX_ENUM_LOOKUP_RESULTS];
}enum_lookup_res_t;

typedef enum _LinphoneOnlineStatus{
	LINPHONE_STATUS_UNKNOWN,
	LINPHONE_STATUS_ONLINE,
	LINPHONE_STATUS_BUSY,
	LINPHONE_STATUS_BERIGHTBACK,
	LINPHONE_STATUS_AWAY,
	LINPHONE_STATUS_ONTHEPHONE,
	LINPHONE_STATUS_OUTTOLUNCH,
	LINPHONE_STATUS_NOT_DISTURB,
	LINPHONE_STATUS_MOVED,
	LINPHONE_STATUS_ALT_SERVICE,
	LINPHONE_STATUS_OFFLINE,
	LINPHONE_STATUS_PENDING,
	LINPHONE_STATUS_CLOSED,
	LINPHONE_STATUS_END
}LinphoneOnlineStatus;

const char *linphone_online_status_to_string(LinphoneOnlineStatus ss);

typedef struct _LinphoneFriend{
	osip_from_t *url;
	int in_did;
	int out_did;
	int sid;
	int nid;
	LinphoneSubscribePolicy pol;
	LinphoneOnlineStatus status;
	CProxyConfig *proxy;
	CLinphoneCore *lc;
	bool_t subscribe;
	bool_t inc_subscribe_pending;
}LinphoneFriend;	


#define linphone_friend_in_list(lf)	((lf)->lc!=NULL)

#define linphone_friend_url(lf) ((lf)->url)

/*
typedef struct _LinphoneProxyConfig
{
	 CLinphoneCore *lc;
	char *reg_proxy;
	char *reg_identity;
	char *reg_route;
	char *realm;
	int expires;
	int reg_time;
	int rid;
	char *type;
	struct _SipSetupContext *ssctx;
	int auth_failures;
	char *contact_addr;
	int contact_port; 
	bool_t commit;
	bool_t reg_sendregister;
	bool_t registered;
	bool_t publish;
} LinphoneProxyConfig;*/


#define linphone_proxy_config_enableregister linphone_proxy_config_enable_register

#define linphone_proxy_config_get_route(obj)  ((obj)->reg_route)
#define linphone_proxy_config_get_identity(obj)	((obj)->reg_identity)
#define linphone_proxy_config_publish_enabled(obj) ((obj)->publish)
#define linphone_proxy_config_get_addr(obj) ((obj)->reg_proxy)
#define linphone_proxy_config_get_expires(obj)	((obj)->expires)
#define linphone_proxy_config_register_enabled(obj) ((obj)->reg_sendregister)
#define linphone_proxy_config_get_core(obj) ((obj)->lc)


typedef struct _LinphoneAccountCreator{
	CLinphoneCore *lc;
	struct _SipSetupContext *ssctx;
	char *username;
	char *password;
	char *domain;
	bool_t succeeded;
}LinphoneAccountCreator;




typedef struct _LinphoneAuthInfo
{
	char *username;
	char *realm;
	char *userid;
	char *passwd;
	char *ha1;
	bool_t works;
	bool_t first_time;
}LinphoneAuthInfo;



struct _LinphoneChatRoom{
	CLinphoneCore *lc;
	char  *peer;
	char *route;
	osip_from_t *peer_url;
	void * user_data;
};
typedef struct _LinphoneChatRoom LinphoneChatRoom;



/* describes the different groups of states */
typedef enum _gstate_group {
  GSTATE_GROUP_POWER,
  GSTATE_GROUP_REG,
  GSTATE_GROUP_CALL
} gstate_group_t;

typedef enum _gstate {
  /* states for GSTATE_GROUP_POWER */
  GSTATE_POWER_OFF = 0,        /* initial state */
  GSTATE_POWER_STARTUP,
  GSTATE_POWER_ON,
  GSTATE_POWER_SHUTDOWN,
  /* states for GSTATE_GROUP_REG */
  GSTATE_REG_NONE = 10,       /* initial state */
  GSTATE_REG_OK,
  GSTATE_REG_FAILED,
  /* states for GSTATE_GROUP_CALL */
  GSTATE_CALL_IDLE = 20,      /* initial state */
  GSTATE_CALL_OUT_INVITE,
  GSTATE_CALL_OUT_CONNECTED,
  GSTATE_CALL_IN_INVITE,
  GSTATE_CALL_IN_CONNECTED,
  GSTATE_CALL_END,
  GSTATE_CALL_ERROR,
  GSTATE_INVALID
} gstate_t;

struct _LinphoneGeneralState {
  gstate_t old_state;
  gstate_t new_state;
  gstate_group_t group;
  const char *message;
};
typedef struct _LinphoneGeneralState LinphoneGeneralState;


typedef void (*ShowInterfaceCb)(CLinphoneCore *lc);
typedef void (*InviteReceivedCb)(CLinphoneCore *lc, const char *from);
typedef void (*ByeReceivedCb)(CLinphoneCore *lc, const char *from);
typedef void (*DisplayStatusCb)(CLinphoneCore *lc, const char *message);
typedef void (*DisplayMessageCb)(CLinphoneCore *lc, const char *message);
typedef void (*DisplayUrlCb)(CLinphoneCore *lc, const char *message, const char *url);
typedef void (*DisplayQuestionCb)(CLinphoneCore *lc, const char *message);
typedef void (*LinphoneCoreCbFunc)(CLinphoneCore *lc,void * user_data);
typedef void (*NotifyReceivedCb)(CLinphoneCore *lc, LinphoneFriend * fid, const char *url, const char *status, const char *img);
typedef void (*NewUnknownSubscriberCb)(CLinphoneCore *lc, LinphoneFriend *lf, const char *url);
typedef void (*AuthInfoRequested)(CLinphoneCore *lc, const char *realm, const char *username);
typedef void (*CallLogUpdated)(CLinphoneCore *lc, struct _LinphoneCallLog *newcl);
typedef void (*TextMessageReceived)(CLinphoneCore *lc, LinphoneChatRoom *room, const char *from, const char *message);
typedef void (*GeneralStateChange)(CLinphoneCore *lc, LinphoneGeneralState *gstate);
typedef void (*DtmfReceived)(CLinphoneCore* lc, int dtmf);
typedef void (*ReferReceived)(CLinphoneCore *lc, const char *refer_to);



typedef struct _LCCallbackObj
{
  LinphoneCoreCbFunc _func;
  void * _user_data;
}LCCallbackObj;



typedef enum _LinphoneFirewallPolicy{
	LINPHONE_POLICY_NO_FIREWALL,
	LINPHONE_POLICY_USE_NAT_ADDRESS,
	LINPHONE_POLICY_USE_STUN
} LinphoneFirewallPolicy;

typedef enum _LinphoneWaitingState{
	LinphoneWaitingStart,
	LinphoneWaitingProgress,
	LinphoneWaitingFinished
} LinphoneWaitingState;
typedef void * (*LinphoneWaitingCallback)(CLinphoneCore *lc, void *context, LinphoneWaitingState ws, const char *purpose, float progress);



typedef struct MSVideoSizeDef{
	MSVideoSize vsize;
	const char *name;
}MSVideoSizeDef;


/*internal use only */
#define linphone_core_lock(lc)	ms_mutex_lock(&(lc)->lock)
#define linphone_core_unlock(lc)	ms_mutex_unlock((&lc)->lock)

static MSVideoSizeDef supported_resolutions[]={
	{	MS_VIDEO_SIZE_SVGA	,	"svga"	},
	{	MS_VIDEO_SIZE_4CIF	,	"4cif"	},
	{	MS_VIDEO_SIZE_VGA	,	"vga"	},
	{	MS_VIDEO_SIZE_CIF	,	"cif"	},
	{	MS_VIDEO_SIZE_QVGA	,	"qvga"	},
	{	MS_VIDEO_SIZE_QCIF	,	"qcif"	},
	{	{0,0}			,	NULL	}
};

