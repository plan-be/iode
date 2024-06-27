//#if defined(DOSWIN) || defined(SCRW32)
//#include <windows.h>
//#endif

#include <iode.h>
#include "o_objs.h"
#include <s_mmt.h>

#define MAX_NC 500
#define MAX_NL 500
#define MIN_NC 90
#define MIN_NL 27

int  errno; /* JMP 31-05-00 */

#ifdef DOSW32

/* HTML Help Test */
char *IODE_TYPES[] = {
    "Cmts", "Eqs", "Idts", "Lsts", "Scls", "Tbls", "Vars"
};

int ODE_NC = -1, ODE_NL = -1, ODE_PALETTE_NB = -1;
int ODE_BANNER = 1, ODE_CHECKVERSION = 1;
int ODE_FONTSIZE = 0; // JMP 5/1/2017

char	*ODE_LASTVERS, *ODE_CURVERS;
char	*ODE_keylogfile = 0;
char	*ODE_INIFILE; // JMP 13/9/2015

IodeProperties()
{
    extern HWND hWndDOS;
    char        txt[1024], buf[256];
    int         i;

    SCR_getcwd(0, buf);
    sprintf(txt, "Current Directory:\n\t%s\n\n", buf);
    strcat(txt, "Workspaces:\n");

    for(i = 0 ; i <= K_VAR - COMMENTS ; i++) {
        sprintf(buf, "\t%s :\t%d objects [%s]\n",
                IODE_TYPES[i], KNB(K_WS[COMMENTS + i]), KNAMEPTR(K_WS[COMMENTS + i])); /* JMP 03-06-2015 */
        strcat(txt, buf);
    }

    sprintf(buf, "\nFree Memory:\n\t%ld MBytes", SCR_free_mem() / (1024L * 1024L));
    strcat(txt, buf);
    MessageBox(hWndDOS, txt, "IODE", MB_OK);
    return(0);
}

IodeDftIniFile()
{
	char 	module[1024], file[1024];

    // Get module directory (c:/iode p.ex)
    GetModuleFileName(hInst, module, 1000);
    SCR_split_dir(module, file);
	sprintf(file, "%s\\iode.ini", module);
	ODE_INIFILE = SCR_stracpy(file);
}

WscrDOSUserInit(argc, argv)  /* JMP 14-12-97 */
int     argc;
char    *argv[];
{
	int     i, palette_nb;
    extern int  (*WSCR_PROPSFN)(void);
    extern char *WSCR_FONTNAME;
    extern int  WSCR_FONTSIZE;
	extern int	SCR_NB_PALETTES;
    extern int  (*WSCR_DOSCLOSEFN)(); /* JMP 13-01-11 */
    extern int  ODE_quit();

    WSCR_ICON = 1; /* JMP 08-01-99 */
    WSCR_PROPSFN = IodeProperties;
	
    WSCR_FONTNAME = "Courier New";
    WSCR_SEMIGRAPHIC_CHARS = 1;     /* JMP 09-01-11 */
    WSCR_SET_CADRES  = 0;           /* JMP 09-01-11 */
    SCR_map_box_2();                /* JMP 20-10-2003 */
    SCR_PAGE_SIZE[0] = 35;          /* JMP 03-10-15 */
    SCR_PAGE_SIZE[1] = 95 ;         /* JMP 03-10-15 */
    DebugActif = 0;                 // JMP 29/09/2020

    IodeDftIniFile(); // JMP 13-09-2015

    if(A_find(argv, "-h") >= 0) ODE_usage();

    // Traitement des arguments graphiques, -h
    for(i = 1 ; i < argc ; i++) {
        if(SCR_cstrcmp(argv[i], "-nc") == 0 && argc > i + 1) {
            ODE_NC = atoi(argv[i + 1]);
            ODE_NC = max(MIN_NC, ODE_NC);
            ODE_NC = min(MAX_NC, ODE_NC);
            i++;
        }
        else if(SCR_cstrcmp(argv[i], "-nl") == 0 && argc > i + 1) {
            ODE_NL = atoi(argv[i + 1]);
            ODE_NL = max(MIN_NL, ODE_NL);
            ODE_NL = min(MAX_NL, ODE_NL);
            i++;
        }
        else if(SCR_cstrcmp(argv[i], "-fontsize") == 0 && argc > i + 1) {
            WSCR_FONTSIZE = atoi(argv[i + 1]);
            WSCR_FONTSIZE = max(6, WSCR_FONTSIZE);                                  /* JMP 28-03-11 */
            WSCR_FONTSIZE = min(100, WSCR_FONTSIZE);
            ODE_FONTSIZE = WSCR_FONTSIZE;
            i++;
        }
/*        else if(SCR_cstrcmp(argv[i], "-palette") == 0 && argc > i + 1) {
			palette_nb = atoi(argv[i + 1]);
			if(palette_nb < SCR_NB_PALETTES && palette_nb >= 0) {
				ODE_PALETTE_NB = palette_nb;
			}	
            i++;
        }
*/
        else if(SCR_cstrcmp(argv[i], "-inifile") == 0 && argc > i + 1) {
			ODE_INIFILE = argv[i + 1];
            i++;
        }
        else if(SCR_cstrcmp(argv[i], "-fontname") == 0 && argc > i + 1) {
            WSCR_FONTNAME = argv[i + 1];
            i++;
        }
        else if(SCR_cstrcmp(argv[i], "-pbf") == 0 && argc > i + 1) {
            SCR_PB_FILENAME = argv[i + 1];
            i++;
        }
        else if(SCR_cstrcmp(argv[i], "-pbs") == 0 && argc > i + 1) {
            SCR_PB_STRING = argv[i + 1];
            i++;
        }
        else if(SCR_cstrcmp(argv[i], "-rec") == 0 && argc > i + 1) {
            ODE_keylogfile = argv[i + 1];
            i++;
        }
    }

	ODE_ReadIni();
	if(ODE_NL > 0)          SCR_PAGE_SIZE[0] = ODE_NL;
	if(ODE_NC > 0)          SCR_PAGE_SIZE[1] = ODE_NC;
    if(ODE_FONTSIZE > 0)    WSCR_FONTSIZE = ODE_FONTSIZE; // JMP 5/1/2017 
	ODE_check_page_size();
	WSCR_DOSCLOSEFN = ODE_quit;       /* JMP 13-01-11 */
    /*    WSCR_NOWINDOW = 1; */
	
    return(0);
}
#endif

ODE_ReadIni()
{
    
	ScrIniRead(ODE_INIFILE, 1); // JMP 13/9/2015
	ODE_BANNER = ScrIniReadYN(ODE_INIFILE, "GENERAL", "BANNER");
	ODE_CHECKVERSION = ScrIniReadYN(ODE_INIFILE, "GENERAL", "CHECKVERSION");
	ScrIniReadInt(ODE_INIFILE, "SCREEN", "NL", &ODE_NL, 0);
	ScrIniReadInt(ODE_INIFILE, "SCREEN", "NC", &ODE_NC, 0);
}

ODE_DisplayBanner()
{
	static int firstpass = 0;
	char	buf[512];
	
	// Banner
	if(ODE_BANNER) PG_edit_window(odep_info);
	ODE_BANNER = 1; // A partir de la deuxi�me fois, on affiche d'office (car vient du menu)
	
	// Message pour nouvelle version
	if(ODE_CHECKVERSION && firstpass == 0 && SCR_cstrcmp(ODE_LASTVERS, ODE_CURVERS)) {
		PG_display_error("A new IODE version is available.\nYour version = %s\nLast version = %s\n\nCheck http://iode.plan.be", ODE_CURVERS, ODE_LASTVERS);
		//sprintf(buf, "A new IODE version is available.\nYour version = %s\nLatest version = %s\n\nCheck http://iode.plan.be", ODE_CURVERS, ODE_LASTVERS);
		//MessageBox(WscrGetMainWnd(), buf, "IODE", MB_TOPMOST | MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);
	}	
	firstpass = 1;
	return(0);
}

kwrprt()
{
    char    buf[10];

    if(B_dest) strcpy(buf, "File");
    else       strcpy(buf, "Printer");

    SCR_pad(buf, 8);
    SCR_set_attr(SCR_REVERSE);
    SCR_write_text(SCR_PAGE_SIZE[0] - 1, SCR_PAGE_SIZE[1] - 16, buf, 7);  /* JMP 03-12-10 */
}


/************************** NEW RESIZING FUNCTIONS (2011) *******************/

int ODE_minmax(int *minnl, int *minnc, int *maxnl, int *maxnc) /* JMP 08-01-11 */
{
    int curx, cury, nl, nc;


    // Eviter de resize dans MMT
    if(MMT_FILES) {
        *maxnc = SCR_PAGE_SIZE[1];
        *maxnl = SCR_PAGE_SIZE[0];
        return(0);
    }

    WscrDOSScreenInfos(&curx, &cury, &nl, &nc);
    *minnc = MIN_NC;
    *minnl = MIN_NL;
    *maxnc = nc;
    *maxnl = nl;

    return(0);
}

int ODE_check_page_size()
{
    SCR_PAGE_SIZE[0]= min(MAX_NL, SCR_PAGE_SIZE[0]);
    SCR_PAGE_SIZE[0]= max(MIN_NL, SCR_PAGE_SIZE[0]);
    SCR_PAGE_SIZE[1]= min(MAX_NC, SCR_PAGE_SIZE[1]);
    SCR_PAGE_SIZE[1]= max(MIN_NC, SCR_PAGE_SIZE[1]);
}

// Fonctions appel�es lors du resize de la fen�tre DOSWIN
int ODE_adapt_ncnl(int nl, int nc) /* JMP 08-01-11 */
{
    ODE_adapt_size(nl - SCR_PAGE_SIZE[0], nc - SCR_PAGE_SIZE[1]); /* JMP 03-12-10 */
    return(0);
}

ODE_adapt_size(int incrnl, int incrnc) /* JMP 03-12-10 */
{
    IMAGE *newscreen, *oldscreen, *im;
    int     onl, onc;
	char    *txt, buf[120];
    static int inuse = 0;

    // if no change, return
    //if(incrnl == 0 && incrnc == 0) return(0);

    // Eviter la r�entrance
    if(inuse) return(0);
    else inuse = 1;

    // Eviter de resize dans MMT
//    if(MMT_CURRENT) return(0);

    // Ne faire de changements qui si on a un incr�ment non nul
    if(1 || incrnl != 0 || incrnc != 0) {
        // Si  MMT en cours, undisplay
        //ODE_ResizeMMTs0();

        // Set new value of screen size
        onl = SCR_PAGE_SIZE[0];
        onc = SCR_PAGE_SIZE[1];
        SCR_PAGE_SIZE[0] += incrnl;
        SCR_PAGE_SIZE[1] += incrnc;

        // Ne rien faire si minimize
        if(SCR_PAGE_SIZE[0] <= 0 || SCR_PAGE_SIZE[1] <=0) {
            SCR_PAGE_SIZE[0] = onl;
            SCR_PAGE_SIZE[1] = onc;
            inuse = 0;
            return(0);
        }

        ODE_check_page_size();

        /* Alloc new screen, copy and clear old one  */
        newscreen = SCR_alloc_image(SCR_PAGE_SIZE[0], SCR_PAGE_SIZE[1]);
        oldscreen = SCR_SCREEN;
        SCR_SCREEN = newscreen;

        // Tout l'�cran NEW en bleu
        //SCR_set_attr(SCR_BLUE);
		SCR_set_attr(SCR_BLACK);
        SCR_fill(' ', 0, 0, SCR_PAGE_SIZE[0], SCR_PAGE_SIZE[1]);

        // Ligne 0 et page_size - 2 en rouge
        SCR_set_attr(SCR_RED);
        SCR_fill(' ', 0, 0, 1, SCR_PAGE_SIZE[1]);
        SCR_write_center(0, 0, IODE_VERSION, SCR_PAGE_SIZE[1]);
        SCR_fill(' ', SCR_PAGE_SIZE[0] - 2, 0, 1, SCR_PAGE_SIZE[1]);

        // Ligne 1 et page_size - 1 en reverse
        SCR_set_attr(SCR_REVERSE);
        SCR_fill(' ', 1, 0, 1, SCR_PAGE_SIZE[1]);
        SCR_fill(' ', SCR_PAGE_SIZE[0] - 1, 0, 1, SCR_PAGE_SIZE[1]);
        SCR_write(SCR_PAGE_SIZE[0] - 1, SCR_PAGE_SIZE[1] - 9, "�", 1);
        SCR_write(SCR_PAGE_SIZE[0] - 1, SCR_PAGE_SIZE[1] - 17, "�", 1);
        //SCR_write(SCR_PAGE_SIZE[0] - 1, SCR_PAGE_SIZE[1] - 28, "�", 1);

        // Recopier OLD DANS NEW
        // Ligne de menu (sans INPUT)
        im = SCR_extract_image(oldscreen, 1, 0, 1, onc - 7); // -7 pour �viter INPUT
        SCR_im_to_im(newscreen, im, 0, 0);
        SCR_free_image(im);

        // Centre de l'�cran
        im = SCR_extract_image(oldscreen, 2, 0, min(onl, SCR_PAGE_SIZE[0]) - 4, onc);
        SCR_im_to_im(newscreen, im, 0, 0);
        SCR_free_image(im);

        // Ligne de cmt
        im = SCR_extract_image(oldscreen, onl - 2, 0, 1, onc);
        SCR_im_to_im(newscreen, im, incrnl, 0);
        SCR_free_image(im);

        // Ligne de MSG  (sans clock ...)
        /*im = SCR_extract_image(oldscreen, onl - 1, 0, 1, onc - 37);
        SCR_im_to_im(newscreen, im, incrnl, 0);
        SCR_free_image(im);
        */

        // Free ancien �cran
        SCR_free_image(oldscreen);

        /* Reset infos positions on screen */
        SCR_CLOCK_POS[0] = SCR_PAGE_SIZE[0] - 1;  // 29;
        SCR_CLOCK_POS[1] = SCR_PAGE_SIZE[1] - 8; // 53;

        /*      SCR_MEM_POS[0] = SCR_PAGE_SIZE[0] - 1;    // 29;
        	SCR_MEM_POS[1] = SCR_PAGE_SIZE[1] - 10;   // 70;
        */
        SCR_KEY_POS[0] = 1;                       // 1;
        SCR_KEY_POS[1] = SCR_PAGE_SIZE[1] - 6;    // 84;

        SCR_CMT.c_line   = SCR_PAGE_SIZE[0] - 1;  // 29;
        SCR_CMT.c_col    = 0;                     // 0;
        SCR_CMT.c_nc     = SCR_PAGE_SIZE[1];      // 90;
        SCR_cmt_alloc();

        // Affichage des infos variables
        SCR_set_clock();
//        SCR_set_mem();
        SCR_set_date();
        kwrprt(); // Printer ou file

        // Si SCROLL en cours, resize
        ODE_ResizeScrolls(); /* JMP 04-02-11 */

        // Si MMT actif, effacer et recr�er l'image
        //ODE_ResizeMMTs1();

        //DebugActif = 1;
        //Debug("[%d, %d]\n", SCR_PAGE_SIZE[0], SCR_PAGE_SIZE[1]);

        /* Refresh */
        //SCR_refresh();

        // Recentre les pages
        ODE_recenter_pages();
    }

    // Force le resize
    WscrDOSResize();
    inuse = 0;
    return(0);
}

ODE_recenter_pages()
{
    int         i, lc;
    FILE_TYPE   *flt;
    PAGE        *pg;

    return(0); /* JMP 13-01-11 */

    for(i = 1 ; i < SCR_READ_FILES.tbl_nb_fl ; i++) {
        flt = SCR_READ_FILES.tbl_fl[i];
        if(TYPE16(flt->fl_type) == SCR_TYPE_PAGE) {
            pg = (PAGE *)flt->fl_ptr;   /* BP_M 23-01-97 */
            if(pg->pg_center) {
                lc = (SCR_PAGE_SIZE[0] - pg->pg_nb_line)/2;
                pg->pg_line = max(0, lc);
                lc = (SCR_PAGE_SIZE[1] - pg->pg_nb_column)/2;
                pg->pg_column = max(0, lc);
            }
        }
    }
}


ODE_init_argv0(argv0)
char    *argv0;
{
    int     kclmsg(), nc, nl;
    extern int     (*WSCR_DOSSIZEFN)(int nl, int nc); /* JMP 08-01-11 */
    extern int     (*WSCR_DOSMINMAXFN)(int *minnl, int *minnc, int *maxnl, int *maxnc); /* JMP 08-01-11 */

    MMT_HLPF10 = "iode.hlp";
    SCR_FN_KEY = kclmsg;
    CNF_REVERSE_TITLE = 0;

//    strcpy(SCR_NAME_ERR, "iode.msg");

/* BEGIN Version sans signature 
    SCR_strlcpy(SCR_CMT_FILE, argv0, sizeof(SCR_CMT_FILE));

    SCR_CMT_FILE_NB = 1;
    strcpy(SCR_NAME_ERR, argv0);
    SCR_ERR_FILE_NB = 2;
    strcpy(SCR_NAME_SCR, argv0);
    SCR_SCR_FILE_NB = 3;
/* END Version sans signature */

/* BEGIN Version avec signature  */
    //SCR_strlcpy(SCR_CMT_FILE, argv0, sizeof(SCR_CMT_FILE));
    
    SCR_change_ext(SCR_CMT_FILE, argv0, "scr");
    SCR_CMT_FILE_NB = 1;
Debug("SCR_CMT_FILE = '%s'", SCR_CMT_FILE);

    strcpy(SCR_NAME_ERR, SCR_CMT_FILE);
    SCR_ERR_FILE_NB = 2;

    strcpy(SCR_NAME_SCR, SCR_CMT_FILE);
    SCR_SCR_FILE_NB = 3;
/* END Version avec signature */

    SCR_init();
    WSCR_DOSSIZEFN = ODE_adapt_ncnl;  /* JMP 08-01-11 */
    WSCR_DOSMINMAXFN = ODE_minmax;    /* JMP 08-01-11 */

    if(ODE_NL < 0) ODE_NL = SCR_PAGE_SIZE[0];
    if(ODE_NC < 0) ODE_NC = SCR_PAGE_SIZE[1];
    ODE_adapt_ncnl(ODE_NL, ODE_NC);  /* JMP 08-01-11 */


//    DebugActif = 1;
//    Debug("SW_SEG_SIZE=%d\n", SW_SEG_SIZE);
}

U_ch  *T_mmt_edit(U_ch *txt, int line, int col, int nc, int lim)
{
    MMT     *mmt;
    LST     *lst;
    int     key;
    extern MMTFN T_LINE_FNS[];

    //MMT_HLPF10 = HLP_NAME; //"iode.hlp"; // JMP 5/9/2015
    mmt = MMT_create(line, col, 1, nc, SCR_RED, SCR_GREEN,
                     0, 0, 0L, 0, T_LINE_FNS, 0L, 0);
    if(lim) mmt->lim[0] = 1;
    lst = LST_vtol(txt, "\n");
    MMT_set_lst(mmt, lst);
    key = MMT_edit_window(mmt);
    if(key == SCR_ENTER || key == SCR_CSR_UP ||
            key == SCR_CSR_DOWN || key == SCR_CSR_LEFT)
        txt = LST_ltov(lst, " ");
    else
        txt = 0;
    LST_free_text(lst);
    MMT_free(mmt);
    return(txt);
}


#ifdef VC /* JMP 21-01-98 */

SCR_closedir() {
    return(0);
}
SCRSTAT *SCR_readdir() {
    return(0);
}
SCRDIR *SCR_opendir() {
    return(0);
}
SCR_is_dir () {
    return(0);
}

#endif /* VC */


#include "ode.c"    /* JMP 12-02-97 */


int IS_init_database() {
    return(0);
}
int ISC_init_start_replicas() {
    return(0);
}


// Resize de la fen�tre MMT

#include <s_mmt.h>

ODE_ResizeMMTs0()
{
    if(MMT_CURRENT) {
        ED_end_im(); // avant resize
    }
}

ODE_ResizeMMTs1()
{

    if(MMT_CURRENT) {
        ED_init_im(); // apr�s resize
        ED_set_pos();
        ED_display();
        ED_set_cursor();
    }
}


ODE_set_hlp(int type)
{
    switch(type) {
    /*case 1 :                          // JMP 30/9/2020 : winhelp ne fonctionne plus
        strcpy(HLP_NAME, "iode.hlp"); 
        break;
    */    
    case 2 :
        strcpy(HLP_NAME, "iode.chm");
        break;
    }
}

ODE_readme() 
{
	extern char *HLP_FILENAME;

	HLP_filename("readme.htm");
	WscrShellExec(HLP_FILENAME);
}
