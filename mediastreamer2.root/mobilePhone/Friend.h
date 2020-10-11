#pragma once
#define key_compare(key, word) strcmp((key),(word))
class CLinphoneCore;
class CConfigure;
typedef struct _LinphoneFriend LinphoneFriend;
typedef struct osip_message osip_message_t;
 #include "linphoneCore.h"
class CFriend
{
public:
	CFriend(void);
	~CFriend(void);
public:

	const char *linphone_online_status_to_string(LinphoneOnlineStatus ss);
	static int friend_data_compare(const void * a, const void * b, void * data);
	static int friend_compare(const void * a, const void * b);

	MSList *find_friend(MSList *fl, const osip_from_t *friend1, LinphoneFriend **lf);
	LinphoneFriend *linphone_find_friend_by_nid(MSList *l, int nid);
	LinphoneFriend *linphone_find_friend_by_sid(MSList *l, int sid);
	void __linphone_friend_do_subscribe(LinphoneFriend *fr);
	LinphoneFriend * linphone_friend_new();
	LinphoneFriend *linphone_friend_new_with_addr(const char *addr);

	void linphone_core_interpret_friend_uri(CLinphoneCore *lc, const char *uri, char **result);
	int linphone_friend_set_sip_addr(LinphoneFriend *lf, const char *addr);
	int linphone_friend_set_name(LinphoneFriend *lf, const char *name);
	int linphone_friend_send_subscribe(LinphoneFriend *fr, bool_t val);
	int linphone_friend_set_inc_subscribe_policy(LinphoneFriend *fr, LinphoneSubscribePolicy pol);
	int linphone_friend_set_proxy(LinphoneFriend *fr, CProxyConfig *cfg);
	void linphone_friend_set_sid(LinphoneFriend *lf, int sid);
	void linphone_friend_set_nid(LinphoneFriend *lf, int nid);
static	void add_presence_body(osip_message_t *notify, LinphoneOnlineStatus online_status);
static	void linphone_friend_notify(LinphoneFriend *lf, int ss, LinphoneOnlineStatus os);
static	void linphone_friend_unsubscribe(LinphoneFriend *lf);

static	void linphone_friend_destroy(LinphoneFriend *lf);
	void linphone_friend_check_for_removed_proxy(LinphoneFriend *lf, CProxyConfig *cfg);
	char *linphone_friend_get_addr(LinphoneFriend *lf);
	char *linphone_friend_get_name(LinphoneFriend *lf);
	char * linphone_friend_get_url(LinphoneFriend *lf);
	bool_t linphone_friend_get_send_subscribe(const LinphoneFriend *lf);

	LinphoneSubscribePolicy linphone_friend_get_inc_subscribe_policy(const LinphoneFriend *lf);

	LinphoneOnlineStatus linphone_friend_get_status(const LinphoneFriend *lf);
	void linphone_friend_apply(LinphoneFriend *fr, CLinphoneCore *lc);

	void linphone_friend_edit(LinphoneFriend *fr);
	void linphone_friend_done(LinphoneFriend *fr);
	void linphone_core_add_friend(CLinphoneCore *lc, LinphoneFriend *lf);
	void linphone_core_remove_friend(CLinphoneCore *lc, LinphoneFriend* fl);


	LinphoneSubscribePolicy __policy_str_to_enum(const char* pol);
	CProxyConfig *__index_to_proxy(CLinphoneCore *lc, int index);

	LinphoneFriend * linphone_friend_new_from_config_file(CLinphoneCore *lc, int index);

	const char *__policy_enum_to_str(LinphoneSubscribePolicy pol);

	void linphone_friend_write_to_config_file(CConfigure *config, LinphoneFriend *lf, int index);

	void linphone_core_write_friends_config(CLinphoneCore* lc);

};
