#include <iodedos.h>

/* Object Definitions */

SB_PrintObjDef()
{
    PG_edit_window(vkp_probjs);
    return(0);
}

C_PrintObjDef()
{
    int        rc;
    extern int B_TBL_TITLE, B_EQS_INFOS, B_EQS_LEC;

    if(C_PrintDestDecLang(0, (int)vkp_probjs_NDEC, (int)vkp_probjs_LANG)) /* GB 08-97 */
	return(-1);


    B_TBL_TITLE = vkp_probjs_TITLE;         /* JMP 12-11-93 */
    B_EQS_INFOS = vkp_probjs_EINFOS;        /* JMP 12-11-93 */
    B_EQS_LEC   = vkp_probjs_ELEC;          /* JMP 12-11-93 */
    if(B_PrintObjDefArgs(vkp_probjs_OBJS, (int)vkp_probjs_TYPE)) { /* JMP 12-11-93 */
	B_display_last_error();
	rc = -1;
    }
    else {
	ODE_error(OM_OBJS_PRINTED);
	rc = 0;
    }

    W_close();
    return(rc);
}


SB_A2MPrint()
{
    PG_edit_window(vkp_prtfile);
    return(0);
}


C_A2MPrint()
{
	char    name[K_MAX_FILE + 1];

    strcpy(name, vkp_prtfile_NAME);
    SCR_strip(name);
    K_set_ext(name, name, K_A2M);
    if(B_FilePrint(name, 1) == 0) return(0);
    B_display_last_error();
    return(-1);
}






