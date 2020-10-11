#include "StdAfx.h"
#include "GeneralState.h"
#include "staticManager.h"

CGeneralState::CGeneralState(void)
{
}

CGeneralState::~CGeneralState(void)
{
}

/* set the initial states */
void CGeneralState::gstate_initialize(CLinphoneCore *lc) {
  lc->gstate_power = GSTATE_POWER_OFF;
  lc->gstate_reg   = GSTATE_REG_NONE;
  lc->gstate_call  = GSTATE_CALL_IDLE;
}

gstate_t CGeneralState::linphone_core_get_state(const CLinphoneCore *lc, gstate_group_t group){
	switch(group){
		case GSTATE_GROUP_POWER:
			return lc->gstate_power;
		case GSTATE_GROUP_REG:
			return lc->gstate_reg;
		case GSTATE_GROUP_CALL:
			return lc->gstate_call;
	}
	return GSTATE_INVALID;
}

void CGeneralState::linphone_core_set_state(CLinphoneCore *lc, gstate_group_t group, gstate_t new_state){
	switch(group){
		case GSTATE_GROUP_POWER:
			lc->gstate_power=new_state;
			break;
		case GSTATE_GROUP_REG:
			lc->gstate_reg=new_state;
			break;
		case GSTATE_GROUP_CALL:
			lc->gstate_call=new_state;
			break;
	}
}

void CGeneralState::gstate_new_state(CLinphoneCore *lc,
                      gstate_t new_state,
                      const char *message) {
  LinphoneGeneralState states_arg;
  
  /* determine the affected group */
  if (new_state < GSTATE_REG_NONE)
    states_arg.group = GSTATE_GROUP_POWER;
  else if (new_state < GSTATE_CALL_IDLE)
    states_arg.group = GSTATE_GROUP_REG;
  else
    states_arg.group = GSTATE_GROUP_CALL;
  
  /* store the new state while remembering the old one */
  states_arg.new_state = new_state;
  states_arg.old_state = linphone_core_get_state(lc,states_arg.group);
  linphone_core_set_state(lc, states_arg.group,new_state);
  states_arg.message = message;
  
  /*printf("gstate_new_state: %s\t-> %s\t(%s)\n",
         _gstates_text[states_arg.old_state],
         _gstates_text[states_arg.new_state],
         message);*/

  /* call the virtual method */

	  lc->vtable->linphone_gtk_general_state(lc, &states_arg);
  
  /* immediately proceed to idle state */
  if (new_state == GSTATE_CALL_END ||
      new_state == GSTATE_CALL_ERROR)
    gstate_new_state(lc, GSTATE_CALL_IDLE, NULL);
}