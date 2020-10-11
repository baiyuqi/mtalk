#pragma once
#include <osipparser2/sdp_message.h>
//#include "linphonecore.h"

typedef struct _sdp_payload
{
	int line;	/* the index of the m= line */
	int pt;		/*payload type */
	int localport;
	int remoteport;
	int b_as_bandwidth;	/* application specific bandwidth */
	char *proto;
	char *c_nettype;
	char *c_addrtype;
	char *c_addr;
	char *c_addr_multicast_ttl;
	char *c_addr_multicast_int;
	char *a_rtpmap;
	char *a_fmtp;
	char *relay_host;
	int relay_port;
	char *relay_session_id;
	int a_ptime;
} sdp_payload_t;

typedef struct _sdp_context sdp_context_t;

typedef int (*sdp_handler_read_codec_func_t) (struct _sdp_context *,
											sdp_payload_t *);
typedef int (*sdp_handler_write_codec_func_t) (struct _sdp_context *);

typedef struct _sdp_handler
{
	sdp_handler_read_codec_func_t accept_audio_codecs;   /*from remote sdp */
	sdp_handler_read_codec_func_t accept_video_codecs;   /*from remote sdp */
	sdp_handler_write_codec_func_t set_audio_codecs;	/*to local sdp */
	sdp_handler_write_codec_func_t set_video_codecs;	/*to local sdp */
	sdp_handler_read_codec_func_t get_audio_codecs;	/*from incoming answer  */
	sdp_handler_read_codec_func_t get_video_codecs;	/*from incoming answer  */
} sdp_handler_t;


typedef enum _sdp_context_state
{
	SDP_CONTEXT_STATE_INIT,
	SDP_CONTEXT_STATE_NEGOCIATION_OPENED,
	SDP_CONTEXT_STATE_NEGOCIATION_CLOSED
} sdp_context_state_t;

struct _sdp_context
{
	sdp_handler_t *handler;
	char *localip;
	char *username;
	void *reference;
	sdp_message_t *offer;		/* the local sdp to be used for outgoing request */
	char *offerstr;
	sdp_message_t *answer;		/* the local sdp generated from an inc request */
	char *answerstr;
	char *relay;
	char *relay_session_id;
	int negoc_status;	/* in sip code */
	int incb;
	sdp_context_state_t state;
};
class CSdpHandler
{
public:
	CSdpHandler(void);
	~CSdpHandler(void);
public:
sdp_context_t *sdp_handler_create_context(sdp_handler_t *handler, const char *localip, const char *username, const char *relay);
void sdp_context_set_user_pointer(sdp_context_t * ctx, void* up);
void *sdp_context_get_user_pointer(sdp_context_t * ctx);
void sdp_context_add_audio_payload( sdp_context_t * ctx, sdp_payload_t * payload);
void sdp_context_add_video_payload( sdp_context_t * ctx, sdp_payload_t * payload);
char * sdp_context_get_offer(sdp_context_t *ctx);
char * sdp_context_get_answer(sdp_context_t* ctx, sdp_message_t *remote_offer);
int sdp_context_get_status(sdp_context_t* ctx);
void sdp_context_read_answer(sdp_context_t *ctx, sdp_message_t *remote_answer);
void sdp_context_free(sdp_context_t *ctx);

int sdp_payload_init (sdp_payload_t * payload);

void sdp_context_add_payload (sdp_context_t * ctx, sdp_payload_t * payload, char *media);
sdp_message_t * sdp_context_generate_template (sdp_context_t * ctx);
static char * int_2char(int a);
char *sdp_message_a_attr_value_get_with_pt(sdp_message_t *sdp,int pos,int pt,const char *field);
char *sdp_message_a_attr_value_get(sdp_message_t *sdp,int pos,const char *field);

public:
	static char * parse_relay_addr(char *addr, int *port);
	static void refuse_mline(sdp_message_t *answer,char *mtype,char *proto, int mline);
	static void add_relay_info(sdp_message_t *sdp, int mline, const char *relay, const char *relay_session_id);
    static char *make_relay_session_id(const char *username, const char *relay);
    static int CSdpHandler::_sdp_message_get_a_ptime(sdp_message_t *sdp, int mline);
};
