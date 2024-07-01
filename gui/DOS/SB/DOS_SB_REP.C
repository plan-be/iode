#include <iodedos.h>

SB_FileEdit(filename)
char    *filename;
{
    int     oldcursor = WSCR_CURSORCOLOR;

    SCR_comment("F1=Help Alt-Q=Quit Alt-W=Save");
    LST_NO_TABS = 1;                    
    MMT_BACK = SCR_WHITE;              
    MMT_REVERSE = SCR_REVERSE;         
    MMT_COMMENT = SCR_RED;             
    MMT_BOXES = SCR_WHITE;             
    WSCR_CURSORCOLOR = SCR_BLACK;      

    MMT_edit_file(filename, 2, 0, SCR_PAGE_SIZE[0] - 4, SCR_PAGE_SIZE[1]);
    WSCR_CURSORCOLOR = oldcursor;      
    return(0);
}

SB_ReportEdit()
{
    PG_edit_window(vkp_edrep);
    return(0);
}


C_ReportEdit()
{
    char    *name = vkp_edrep_NAME;

    K_set_ext(name, name, FILE_REP);
	if(SB_FileEdit(name) == 0) return(0); 
    B_display_last_error();
    return(-1);
}

SB_ReportPrint()
{
    PG_edit_window(vkp_prtrep);
    return(0);
}


C_ReportPrint()
{
    char    *name = vkp_prtrep_NAME;

    K_set_ext(name, name, FILE_REP);

    if(B_FilePrint(name, 0) == 0) return(0);
//    if(B_FilePrintGdi(name) == 0) return(0);
    B_display_last_error();
    return(-1);
}


SB_ReportExec()
{
    PG_edit_window(vkp_execrep);
    return(0);
}

C_ReportExec()
{
    int     rc = -1, lga = 0;           /* JMP 24-06-96 */
    char    *name = vkp_execrep_NAME;
    char    *buf, *arg;

    if(C_PrintDestDecLang(1, (int)vkp_execrep_NDEC, (int)vkp_execrep_LANG)) /* JMP 02-12-97 */
	return(-1);

    rc = B_PrintDest("dummy D");

    K_set_ext(name, name, FILE_REP);

    arg = vkp_execrep_PARMS;
    if(arg) lga = strlen(arg);                  /* JMP 24-06-96 */
    buf = SW_nalloc(lga + strlen(name) + 2);    /* JMP 24-06-96 */
    if(arg) sprintf(buf, "%s %s", name, arg);   /* JMP 24-06-96 */
    else    strcpy(buf, name);

    if(B_ReportExec(buf) == 0) rc = 0;
    else rc = -1;

    if(rc < 0) B_display_last_error();
    SW_nfree(buf);
    W_close();
    return(rc);
}
/*
SB_ReportEditOutput()
{
    if(W_filename[0]) {
	SCR_record_key(SCR_CSR_PG_UP);
	SCR_record_key(SCR_C_CSR_END);
	return(SB_FileEdit(W_filename));
	}
    ODE_error(OM_NO_OUTPUT);
    return(-1);
}
*/
SB_ReportLine()
{
    PG_edit_window(vkp_repline);
    return(0);
}

C_ReportLine()
{
    if(C_PrintDestDecLang(0, (int)vkp_repline_NDEC, (int)vkp_repline_LANG)) /* GB 08-97 */
	return(-1);

    if(B_ReportLine(vkp_repline_CMD)) {
	B_display_last_error();
	return(-1);
	}
    W_close();
    return(0);
}

SB_ReportPrompt(arg, parm)
char    *arg;
int     parm;
{
    int     lg;
    ONAME   macro;
    char    *prompt, *gmacro;
    extern  char    *RP_gmacro();

    lg = B_get_arg0(macro, arg, K_MAX_NAME);
    prompt = arg + lg + 1;
    U_ljust_text(prompt);
    SCR_strip(prompt);
    lg = strlen(prompt) + 1;

    strncpy(vkp_prompt_MACRO, macro, K_MAX_NAME);
    strncpy(vkp_prompt_PROMPT, prompt, 80);
    gmacro = RP_gmacro(macro);

    if(gmacro) {
	B_clear_last_error();
	strncpy(vkp_prompt_DEFINE, gmacro, 120);
    }
    else vkp_prompt_DEFINE[0] = '\0';

    PG_edit_window(vkp_prompt);
    return(0);
}

C_ReportPrompt()
{
    char    *buf;
    int     rc = 0;

    buf = SW_nalloc(strlen(vkp_prompt_MACRO) + strlen(vkp_prompt_DEFINE) + 10); /* JMP 11-09-96 */
    sprintf(buf, "%s %s", vkp_prompt_MACRO, vkp_prompt_DEFINE);

    if(B_ReportPrompt(buf)) {
	B_display_last_error();
	rc = -1;
    }

    SW_nfree(buf);
    return(rc);
}















