#include <iodedos.h>

SB_IdtExecute()
{
    PG_edit_window(vkip_exec);
    return(0);
}

C_IdtExecute()
{
    char    *buf;
    int     rc;

    if(C_PrintDestDecLang(0, 2, (int) vkip_exec_LANG)) /* GB 08-97 */
	return(-1);
    B_IdtExecuteVarFiles(vkip_exec_VARFILES);
    B_IdtExecuteSclFiles(vkip_exec_SCLFILES);
    B_IdtExecuteTrace((vkip_exec_TRACE == 0) ? "N" : "Y");

    if(vkip_exec_IDTS != NULL) {
	buf = SW_nalloc(strlen(vkip_exec_IDTS) + 25);
	if(buf == NULL) return(-1);
	sprintf(buf, "%s %s %s", vkip_exec_FROM, vkip_exec_TO, vkip_exec_IDTS);
    }
    else {
	buf = SW_nalloc(25);
	if(buf == NULL) return(-1);
	sprintf(buf, "%s %s", vkip_exec_FROM, vkip_exec_TO);
    }

    if(B_IdtExecute(buf)) {
	B_display_last_error();
	rc = -1;
	}
    else {
	ODE_error(OM_EXEC_ACH);
	rc = 0;
	}

    SW_nfree(buf);
    W_close();
    return(rc);
}

/*
SB_IdtExecuteCnf()
{
    char    *buf;

    buf = SCR_mtov(KEXEC_VFILES, ' ');
    SCR_strfacpy(&(vkip_exec_VARFILES), buf);
    SCR_free(buf);

    buf = SCR_mtov(KEXEC_SFILES, ' ');
    SCR_strfacpy(&(vkip_exec_SCLFILES), buf);
    SCR_free(buf);

    PG_edit_window(vkip_exec);
    return(0);
}
*/

