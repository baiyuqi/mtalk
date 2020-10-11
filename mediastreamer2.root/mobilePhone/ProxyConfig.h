#pragma once
typedef unsigned char bool_t;
class CLinphoneCore;
class CProxyConfig
{
public:
	CProxyConfig(void);
	~CProxyConfig(void);
public:
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
	char *contact_addr; /* our IP address as seen by the proxy, read from via 's received= parameter*/
	int contact_port; /*our IP port as seen by the proxy, read from via's rport= parameter */
	bool_t commit;
	bool_t reg_sendregister;
	bool_t registered;
	bool_t publish;
};
