#include <iodedos.h>

int     B_dest;
char    *B_outfile = NULL;

/* */
void SB_FileOpen()
{
}

void C_FileOpen()
{
}

/* */
void SB_FileNew()
{
}

void C_FileNew()
{
}

/* */
void SB_FileSave()
{
}

void C_FileSave()
{
}

/* */
SB_FilePrintSetup()
{
    PG_edit_window(p_fprintsetup);
    return(0);
}

void C_FilePrintSetup()
{
    B_dest = p_fprintsetup_DEST;

    kwrprt();
    SCR_free(B_outfile);
    B_outfile = SCR_stracpy("");
}

void C_FilePrintFile()
{
    char    format;

    if(B_dest) {
	SCR_free(B_outfile);
	B_outfile = SCR_malloc(strlen(vkp_fprintfile_FILE) + 5); // JMP 6/10/2015
	switch(vkp_fprintfile_FORMAT) {
	    case 1:  format = 'R'; break;
	    case 2:  format = 'H'; break;
	    case 3:  format = 'M'; break;
	    case 4:  format = 'C'; break;
	    default: format = 'A'; break;
	}

	sprintf(B_outfile, "%s %c", vkp_fprintfile_FILE, format);
    }
}

/* */
void SB_FilePreferences()
{
}

void SB_PrintPreferences()    /* JMP 19-02-98 */
{
    PG_edit_window(tp_pref);
    B_A2mGetGdiParms();
    B_A2mGetGnlParms();
    B_A2mGetRtfParms();
    B_A2mGetGIFParms();
    B_A2mGetHtmlParms();
    B_A2mGetMifParms();
    B_A2mGetCsvParms();
}

void C_FilePreferences()
{
}


