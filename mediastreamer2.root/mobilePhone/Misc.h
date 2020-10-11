#pragma once
typedef struct _PayloadType PayloadType;
#include "linphoneCore.h"
class CMisc
{
public:
	CMisc(void);
	~CMisc(void);
public:

	char *int2str(int number);
	void check_sound_device(CLinphoneCore *lc);

	const char *payload_type_get_description(PayloadType *pt);
	void payload_type_set_enable(PayloadType *pt,int value) ;
	bool_t payload_type_enabled(PayloadType *pt) ;
	int payload_type_get_bitrate(PayloadType *pt);
	const char *payload_type_get_mime(PayloadType *pt);
	int payload_type_get_rate(PayloadType *pt);
	double get_audio_payload_bandwidth(const PayloadType *pt);

	void linphone_core_update_allocated_audio_bandwidth_in_call(CLinphoneCore *lc, const PayloadType *pt);
	void linphone_core_update_allocated_audio_bandwidth(CLinphoneCore *lc);
	/* return TRUE if codec can be used with bandwidth, FALSE else*/
	bool_t linphone_core_check_payload_type_usability(CLinphoneCore *lc, PayloadType *pt);
	PayloadType * find_payload(RtpProfile *prof, PayloadType *pt /*from config*/);

	bool_t check_h264_packmode(PayloadType *payload, MSFilterDesc *desc);
	MSList *fix_codec_list(RtpProfile *prof, MSList *conflist);


	void linphone_core_setup_local_rtp_profile(CLinphoneCore *lc);

	int from_2char_without_params(osip_from_t *from,char **str);
	static bool_t lp_spawn_command_line_sync(const char *command, char **result,int *command_ret);


//	bool_t host_has_ipv6_network();
	ortp_socket_t create_socket(int local_port);
	int sendStunRequest(int sock, const struct sockaddr *server, socklen_t addrlen, int id, bool_t changeAddr);

	int parse_stun_server_addr(const char *server, struct sockaddr_storage *ss, socklen_t *socklen);
	int recvStunResponse(ortp_socket_t sock, char *ipaddr, int *port, int *id);
	void linphone_core_run_stun_tests(CLinphoneCore *lc, LinphoneCall *call);
	int extract_sip_port(const char *config);
	int linphone_core_wake_up_possible_already_running_instance(const char * config_file, const char * addr_to_call);
	int linphone_core_get_local_ip_for(const char *dest, char *result);
};
