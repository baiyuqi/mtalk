#pragma once
#include "linphoneCore.h"
class CGeneralState
{
public:
	CGeneralState(void);
	~CGeneralState(void);
public:
	void gstate_initialize(CLinphoneCore *lc) ;
	gstate_t linphone_core_get_state(const CLinphoneCore *lc, gstate_group_t group);
	void linphone_core_set_state(CLinphoneCore *lc, gstate_group_t group, gstate_t new_state);
	void gstate_new_state(CLinphoneCore *lc,gstate_t new_state,const char *message) ;
};
