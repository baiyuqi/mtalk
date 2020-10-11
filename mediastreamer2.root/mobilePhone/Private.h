#pragma once
typedef struct _LinphoneAuthInfo LinphoneAuthInfo;
typedef struct _LinphoneChatRoom LinphoneChatRoom;
 typedef struct eXosip_event eXosip_event_t;
class CLinphoneCore;
class CConfigure;
#include "linphoneCore.h"

class CPrivate
{
public:
	CPrivate(void);
	~CPrivate(void);
public:
	LinphoneAuthInfo *linphone_auth_info_new(const char *username, const char *userid,
		const char *passwd, const char *ha1,const char *realm);
	void linphone_auth_info_set_passwd(LinphoneAuthInfo *info, const char *passwd);
	void linphone_auth_info_set_username(LinphoneAuthInfo *info, const char *username);
	void linphone_auth_info_destroy(LinphoneAuthInfo *obj);
	void linphone_auth_info_write_config(CConfigure *config, LinphoneAuthInfo *obj, int pos);
	LinphoneAuthInfo *linphone_auth_info_new_from_config_file(CConfigure * config, int pos);
static	bool_t key_match(const char *tmp1, const char *tmp2);
static	char * remove_quotes(char * input);
static	int realm_match(const char *realm1, const char *realm2);
	static int auth_info_compare(const void *pinfo,const void *pref);
static	int auth_info_compare_only_realm(const void *pinfo,const void *pref);
	LinphoneAuthInfo *linphone_core_find_auth_info(CLinphoneCore *lc, const char *realm, const char *username);
	void linphone_core_add_auth_info(CLinphoneCore *lc, LinphoneAuthInfo *info);
	void linphone_core_abort_authentication(CLinphoneCore *lc,  LinphoneAuthInfo *info);
	void linphone_core_remove_auth_info(CLinphoneCore *lc, LinphoneAuthInfo *info);
	const MSList *linphone_core_get_auth_info_list(const CLinphoneCore *lc);
	void linphone_core_clear_all_auth_info(CLinphoneCore *lc);
	void linphone_authentication_ok(CLinphoneCore *lc, eXosip_event_t *ev);
	void linphone_core_find_or_ask_for_auth_info(CLinphoneCore *lc,const char *username,const char* realm, int tid);
	void linphone_process_authentication(CLinphoneCore *lc, eXosip_event_t *ev);

	LinphoneChatRoom * linphone_core_create_chat_room(CLinphoneCore *lc, const char *to);
	void linphone_chat_room_destroy(LinphoneChatRoom *cr);
	void linphone_chat_room_send_message(LinphoneChatRoom *cr, const char *msg);
	bool_t linphone_chat_room_matches(LinphoneChatRoom *cr, osip_from_t *from);
	void linphone_chat_room_text_received(LinphoneChatRoom *cr, CLinphoneCore *lc, const char *from, const char *msg);
	void linphone_core_text_received(CLinphoneCore *lc, eXosip_event_t *ev);
	void linphone_chat_room_set_user_data(LinphoneChatRoom *cr, void * ud);
	void * linphone_chat_room_get_user_data(LinphoneChatRoom *cr);

	static inline int get_min_bandwidth(int dbw, int ubw){
	if (dbw<0) return ubw;
	if (ubw<0) return dbw;
	return MIN(dbw,ubw);
}

static inline bool_t bandwidth_is_greater(int bw1, int bw2){
	if (bw1<0) return TRUE;
	else if (bw2<0) return FALSE;
	else return bw1>=bw2;
}

static inline void set_string(char **dest, const char *src){
	if (*dest){
		ms_free(*dest);
		*dest=NULL;
	}
	if (src)
		*dest=ms_strdup(src);
}

public:
	bool_t is_enum(const char *sipaddress, char **enum_domain);
int enum_lookup(const char *enum_domain, enum_lookup_res_t **res);
void enum_lookup_res_free(enum_lookup_res_t *res);
bool_t is_a_number(const char *str);

char *create_enum_domain(const char *number);
};
