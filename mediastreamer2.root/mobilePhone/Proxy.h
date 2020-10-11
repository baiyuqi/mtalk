#pragma once
#include <ortp/ortp.h>
#include <ortp/payloadtype.h>
class CLinphoneCore;

typedef struct _LinphoneAccountCreator LinphoneAccountCreator;
typedef struct _MSList MSList;
typedef struct eXosip_event eXosip_event_t;
class CConfigure;
typedef struct _SipSetupContext SipSetupContext;
typedef enum _LinphoneOnlineStatus LinphoneOnlineStatus;
typedef struct osip_message osip_message_t;
class CProxyConfig;
class CProxy
{
public:
	CProxy(void);
	~CProxy(void);
public:
	
void linphone_proxy_config_write_all_to_config_file(MSList *proxies);
void linphone_proxy_config_init(CProxyConfig *obj);
CProxyConfig *linphone_proxy_config_new();
void linphone_proxy_config_destroy(CProxyConfig *obj);
bool_t linphone_proxy_config_is_registered( CProxyConfig *obj);
void linphone_proxy_config_get_contact(CProxyConfig *cfg, const char **ip, int *port);
void update_contact(CProxyConfig *cfg, const char *ip, const char *port);
bool_t linphone_proxy_config_register_again_with_updated_contact(CProxyConfig *obj, osip_message_t *orig_request, osip_message_t *last_answer);
static int linphone_proxy_config_set_server_addr(CProxyConfig *obj, const char *server_addr);
void linphone_proxy_config_set_identity(CProxyConfig *obj, const char *identity);
const char *linphone_proxy_config_get_domain( CProxyConfig *cfg);
void linphone_proxy_config_set_route(CProxyConfig *obj, const char *route);

bool_t linphone_proxy_config_check(CLinphoneCore *lc, CProxyConfig *obj);
void linphone_proxy_config_enableregister(CProxyConfig *obj, bool_t val);
void linphone_proxy_config_expires(CProxyConfig *obj, int val);

void linphone_proxy_config_enable_publish(CProxyConfig *obj, bool_t val);
void linphone_proxy_config_edit(CProxyConfig *obj);
void linphone_proxy_config_apply(CProxyConfig *obj,CLinphoneCore *lc);

static void linphone_proxy_config_register(CProxyConfig *obj);
int linphone_proxy_config_done(CProxyConfig *obj);

void linphone_proxy_config_set_realm(CProxyConfig *cfg, const char *realm);
int linphone_proxy_config_send_publish(CProxyConfig *proxy,
			       LinphoneOnlineStatus presence_mode);
int linphone_core_add_proxy_config(CLinphoneCore *lc, CProxyConfig *cfg);

//extern void linphone_friend_check_for_removed_proxy(LinphoneFriend *lf, CProxyConfig *cfg);

void linphone_core_remove_proxy_config(CLinphoneCore *lc, CProxyConfig *cfg);

void linphone_core_set_default_proxy(CLinphoneCore *lc, CProxyConfig *config);
void linphone_core_set_default_proxy_index(CLinphoneCore *lc, int index);
int linphone_core_get_default_proxy(CLinphoneCore *lc, CProxyConfig **config);
static int rid_compare(const void *pcfg,const void *prid);
CProxyConfig *linphone_core_get_proxy_config_from_rid(CLinphoneCore *lc, int rid);
const MSList *linphone_core_get_proxy_config_list(const CLinphoneCore *lc);
void linphone_proxy_config_process_authentication_failure(CLinphoneCore *lc, eXosip_event_t *ev);
void linphone_proxy_config_write_to_config_file(CConfigure *config, CProxyConfig *obj, int index);
CProxyConfig *linphone_proxy_config_new_from_config_file(CConfigure *config, int index);

static void linphone_proxy_config_activate_sip_setup(CProxyConfig *cfg);
static void linphone_proxy_config_update(CProxyConfig *cfg);
void linphone_proxy_config_set_sip_setup(CProxyConfig *cfg, const char *type);
SipSetupContext *linphone_proxy_config_get_sip_setup_context(CProxyConfig *cfg);
LinphoneAccountCreator *linphone_account_creator_new(CLinphoneCore *core, const char *type);
void linphone_account_creator_set_username(LinphoneAccountCreator *obj, const char *username);
void linphone_account_creator_set_password(LinphoneAccountCreator *obj, const char *password);
void linphone_account_creator_set_domain(LinphoneAccountCreator *obj, const char *domain);
const char * linphone_account_creator_get_username(LinphoneAccountCreator *obj);
const char * linphone_account_creator_get_domain(LinphoneAccountCreator *obj);
int linphone_account_creator_test_existence(LinphoneAccountCreator *obj);
CProxyConfig * linphone_account_creator_validate(LinphoneAccountCreator *obj);
void linphone_account_creator_destroy(LinphoneAccountCreator *obj);


//void linphone_proxy_config_enableregister(CProxyConfig *obj, bool_t val);
//void linphone_proxy_config_enableregister(CProxyConfig *obj, bool_t val);


};
