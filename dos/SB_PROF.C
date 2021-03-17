#include <iodedos.h>

SB_ProfileLoad()
{
    PG_edit_window(vkp_profload);
    return(0);
}

C_ProfileLoad()
{
    if(B_ProfileLoad(vkp_profload_NAME)) {
	B_display_last_error();
	return(-1);
	}
    else {
	ODE_error(OM_PROF_LOADED);
	return(0);
	}
}

SB_ProfileSave()
{
    PG_edit_window(vkp_profsave);
    return(0);
}

C_ProfileSave()
{
    if(B_ProfileSave(vkp_profsave_NAME)) {
	B_display_last_error();
	return(-1);
	}
    else {
	ODE_error(OM_PROF_SAVED);
	return(0);
	}
}

SB_ProfileReset()
{
    if(ODE_ask(OM_SURE)) return(0);
    return(B_ProfileReset());
}
