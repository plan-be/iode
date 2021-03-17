#include "scr.h"

/* ====================================================================
Lit un fichier de d‚finition des imprimantes et cr‚e un tableau de
structures exploitables pour d‚finir efficacement les PRINTER de SCR/AL1
dans diff‚rents sites.

&RT tableau de pointeurs vers des structures PRTDEV.
&EX
    D‚finir une MENU avec les imprimantes d‚finies sur le site
    ----------------------------------------------------------

    PRTDEV  **GA_PRTS = 0;

    MyApplSetMenuPrts()
    {
	int     i;
	MENU    *mn = gm_prts;

	SCR_FreePrts(GA_PRTS);
	GA_PRTS = SCR_ReadPrts("ga4.ini");
	if(GA_PRTS == 0) return(-1);
	for(i = 0 ; i < mn->mn_nb_opts; i++)
	    SCR_free(mn->mn_opts[i].opt_text);
	SCR_free(mn->mn_opts);
	mn->mn_nb_opts = 0;
	mn->mn_opts = 0;

	for(i = 0 ; GA_PRTS[i] ; i++)
	    MN_add_set_option_text(mn, "%s (%s)", GA_PRTS[i]->prd_name,
						  GA_PRTS[i]->prd_descr);
	MN_end(mn);
    }

    Exploiter pour d‚finir les structures PR_DEV
    --------------------------------------------
    MyApplSetAttr(prattr, prdattr)
    char    **prattr, *prdattr;
    {
	if(prdattr == 0) return;
	if(*prattr && strcmp(*prattr, prdattr) == 0) return;
	*prattr = SCR_stracpy(prdattr);
    }

    MyApplPrOpen(pr)
    PR_DEF  *pr;
    {
	PRTDEV  *prd;
	int     ininb, i;

	pr = MyApplPr0;
	prd = GA_PRTS[0];

	// Exploitation des infos de ga4.ini
	pr->pr_type = prd->prd_mode;
	MyApplSetAttr(&(pr->pr_linefeed), prd->prd_eol);
	MyApplSetAttr(&(pr->pr_formfeed), prd->prd_eop);
	MyApplSetAttr(&(pr->pr_end), prd->prd_eof);
	PR_DOCNAME = "Gestion syndicale";

    #ifndef DOS
	if(pr->pr_type == 'D') pr->pr_type = 'P';
    #endif
	if(pr->pr_type == 'F') {
	    PG_edit_window(gp_prtfile); // Demande le nom du fichier
	    PR_set_name(pr, gp_prtfile_FILE);
	    }

	else PR_set_name(pr, prd->prd_name);

	PR_select(pr);
    }
&TX
&SA SCR_FreePrts()
=======================================================================*/

PRTDEV **SCR_ReadPrts(filename)
char *filename;
{
    PRTDEV  **prtdevs = 0;
    U_ch    **prts;
    int     i, nbprts;

    prts = IniReadSection(filename, "PRINTERS");
    nbprts = SCR_tbl_size(prts);
    if(nbprts == 0) return(prtdevs);
    prtdevs = (PRTDEV **)SCR_malloc((nbprts + 1) * sizeof(PRTDEV *));
    for(i = 0 ; prts[i] ; i++) {
	prtdevs[i] = (PRTDEV *) SCR_malloc(sizeof(PRTDEV));
	SCR_Read1Prt(filename, prts[i], prtdevs[i]);
       }

    SCR_free_tbl(prts);
    return(prtdevs);
}

/*NH*/
char *SCR_ReadPrtStr(buf)
char    *buf;
{
    char    *str;

    SCR_lower(buf);
    if(strcmp(buf, "ff") == 0)        str = "\f";
    else if(strcmp(buf, "lf") == 0)   str = "\n";
    else if(strcmp(buf, "crlf") == 0) str = "\r\n";
    else if(strcmp(buf, "cr") == 0)   str = "\r";
    else if(strcmp(buf, "eof") == 0)  str = "\026";
    else                              str = "";

    return(SCR_stracpy(str));
}

/*NH*/
SCR_Read1Prt(filename, name, prtdev)
char    *filename;
char    *name;
PRTDEV  *prtdev;
{
    char    buf[256];

    SCR_upper(name);
    if(IniReadTxtParm(filename, name, "DESCRIPTION", buf, 255) == 0)
	prtdev->prd_descr = SCR_stracpy(buf);

    if(IniReadTxtParm(filename, name, "NAME", buf, 255) == 0)
	prtdev->prd_name = SCR_stracpy(buf);

    if(IniReadTxtParm(filename, name, "COMMAND", buf, 255) == 0)
	prtdev->prd_command = SCR_stracpy(buf);

    if(IniReadTxtParm(filename, name, "EOL", buf, 255) == 0)
	prtdev->prd_eol = SCR_ReadPrtStr(buf);

    if(IniReadTxtParm(filename, name, "EOP", buf, 255) == 0)
	prtdev->prd_eop = SCR_ReadPrtStr(buf);

    if(IniReadTxtParm(filename, name, "EOF", buf, 255) == 0)
	prtdev->prd_eof = SCR_ReadPrtStr(buf);

    if(IniReadTxtParm(filename, name, "MODE", buf, 255) == 0) {
	SCR_lower(buf);
	if(strcmp(buf, "raw") == 0)         prtdev->prd_mode = 'D';
	else if(strcmp(buf, "file") == 0)   prtdev->prd_mode = 'F';
	else                                prtdev->prd_mode = 'P';
	}

    if(IniReadTxtParm(filename, name, "CHARSET", buf, 255) == 0) {
	SCR_lower(buf);
	if(strcmp(buf, "ansi") == 0)   prtdev->prd_charset = 1;
	else                           prtdev->prd_charset = 0;
	}

    if(IniReadTxtParm(filename, name, "GRAPHIC", buf, 255) == 0) {
	SCR_lower(buf);
	if(strcmp(buf, "yes") == 0)   prtdev->prd_graph = 1;
	else                          prtdev->prd_graph = 0;
	}
    return(0);
}

/* ====================================================================
LibŠre l'espace allou‚ par la fonction SCR_ReadPrts().

&SA SCR_ReadPrts()
=======================================================================*/

SCR_FreePrts(prts)
PRTDEV  **prts;
{
    int     i;

    if(prts == 0) return;
    for(i = 0 ; prts[i] ; i++) {
	SCR_free(prts[i]->prd_name);
	SCR_free(prts[i]->prd_descr);
	SCR_free(prts[i]->prd_command);
	SCR_free(prts[i]->prd_eol);
	SCR_free(prts[i]->prd_eop);
	SCR_free(prts[i]->prd_eof);
	SCR_free(prts[i]);
	}
    SCR_free(prts);
}

