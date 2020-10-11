#include "StdAfx.h"
#include "ProxyConfig.h"

CProxyConfig::CProxyConfig(void)
{
	 lc = NULL;
	 reg_proxy= NULL;
	 reg_identity= NULL;
	 reg_route= NULL;
	 realm= NULL;
	 type= NULL;
	 ssctx= NULL;
	 auth_failures= NULL;
	 contact_addr= NULL;
}

CProxyConfig::~CProxyConfig(void)
{
}
