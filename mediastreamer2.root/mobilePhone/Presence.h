#pragma once
#include "linphonecore.h"

#include <eXosip2/eXosip.h>
#include <osipparser2/osip_message.h>
class CPresence
{
public:
	CPresence(void);
	~CPresence(void);
public:
	void linphone_core_add_subscriber(CLinphoneCore *lc, const char *subscriber, int did, int nid);

void linphone_core_reject_subscriber(CLinphoneCore *lc, LinphoneFriend *lf);
 static void __do_notify(void * data, void * user_data);

void __linphone_core_notify_all_friends(CLinphoneCore *lc, int ss, int os);
void linphone_core_notify_all_friends(CLinphoneCore *lc, LinphoneOnlineStatus os);
bool_t linphone_core_check_presence(CLinphoneCore *lc);

void linphone_subscription_new(CLinphoneCore *lc, eXosip_event_t *ev);
void linphone_notify_recv(CLinphoneCore *lc, eXosip_event_t *ev);
void linphone_subscription_answered(CLinphoneCore *lc, eXosip_event_t *ev);
void linphone_subscription_closed(CLinphoneCore *lc,eXosip_event_t *ev);
};
