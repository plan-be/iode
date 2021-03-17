//#include "iode.h"

int     first_page = 0;
char    STATIC_BUF[512];        /* JMP 10-01-11 */
int     STATIC_BUF_LG = 511;    /* JMP 10-01-11 */
int     ODE_NOEXPANDSTAR; /* JMP 10-03-98 */

K_supmsg()
{
    extern int SCR_vers_done, SCR_sn_done;

    SCR_vers_done = SCR_sn_done = first_page = 1;
    return(0);
}

/*
static  ADEF    adef[] = {
    { "-ws",        "",              "Automatic load ws.*"          , 0, 0, 0,    NULL,       NULL,       NULL},
    { "-rep",       "",              "Execution of report"          , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-erep",      "",              "Edit of report"               , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-l",         "",              "Load an object file"          , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-p",         "",              "Print an object file"         , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-msg",       "",              "suppress version message"     , 0, 0, 0,    NULL,       NULL,       NULL},
    { "-nc",        "nc",            "screen size (#cols)"          , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-nl",        "nl",            "screen size (#lines)"         , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-fontsize",  "nPts",          "fontsize"                     , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-fontname",  "fontname",      "fontname"                     , 0, 1, 1,    NULL,       NULL,       NULL},
	{ "-palette",   "nb",            "palette nb"                   , 0, 1, 1,    NULL,       NULL,       NULL},
	{ "-inifile",   "filename",      "iode ini file"                                , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-pbf",       "filename",      "play back file"               , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-pbs",       "string",        "play back string"             , 0, 1, 1,    NULL,       NULL,       NULL},
    { "-rec",           "filename",      "record keys in a playback file", 0, 1, 1,    NULL,       NULL,       NULL},
	{ "-h",         "",              "print this message and exit"  , 0, 0, 0,    NULL,       NULL,       NULL},
    { "-seg",           "16384",         "allocation segment size"      , 0, 1, 1,    NULL,       NULL,       NULL},

    { NULL,     0,                    0                             , 0, 0, 0,    NULL,       NULL,       NULL}
};
*/

ODE_usage()
{
    char buf[4192];

    sprintf(buf, "%s\n\n", IODE_VERSION);
    strcat(buf, "Syntax : iode [-parm arg]... \n\n");
    strcat(buf, "       -l filename : load an object file\n");
    strcat(buf, "       -ws : load ws.*\n");
    strcat(buf, "       -rep filename : execute a report file\n");
    strcat(buf, "       -erep filename : start editing a report file\n");
    strcat(buf, "       -p filename : print an object file\n");
//    strcat(buf, "       -msg : suppress version message\n");
    strcat(buf, "       -seg segsize : set the allocation segment size (dft 16384)\n");
    strcat(buf, "       -nc number : set the number of columns of the iode window\n");
    strcat(buf, "       -nl number : set the number of lines of the iode window\n");
    strcat(buf, "       -fontsize nPts : set the character size (in points : [3-50]\n");
    strcat(buf, "       -fontname fontname : set the font (default Courier New)\n");
    //strcat(buf, "       -palette nb : change dft screen colors to palette nb (0-4)\n");
    strcat(buf, "       -inifile filename : file containing various iode parms\n");
    strcat(buf, "       -pbf filename : execute a 'play back' file\n");
    strcat(buf, "       -pbs string : execute a 'play back' string\n");
    strcat(buf, "       -rec filename : record keys in a playback file\n");
    strcat(buf, "       -h : display this message and exit\n");
    SCR_OemToAnsi(buf, buf);
    MessageBox(0L, buf, "Iode Syntax", MB_TOPMOST | MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);
    exit(0);
}


extern char SCR_NAME_SCR[];
extern char SCR_CMT_FILE[];

main(argc, argv)
int     argc;
char    *argv[];
{
    char    **args;
    int     ws = 0, pos, type, loaded = -1;
    extern int SCR_tabenter;
    extern int ODE_PALETTE_NB;
    extern long     *SCR_PALETTES[];

#ifdef X11
    X11UserInit(argc, argv);
#endif
    YY_NO_COMMENT2 = 1;

    A_SEPS = " ,;\n\t\r";
    ODE_NOEXPANDSTAR = 0;
    // args = A_initv(argv);        /* JMP 13-01-11 */
    args = argv;                    /* JMP 13-01-11 */
    ODE_NOEXPANDSTAR = 1;
    SCR_HLP_NOPGNAME = 1;

    if(ODE_PALETTE_NB >= 0)  // JMP 6/9/2015
        WscrSetPalette(SCR_PALETTES[ODE_PALETTE_NB]);

//    if(A_help(args, ODE_usage)) exit(0); /* JMP 27-01-11 */
//    if(A_check(args, adef) < 0) exit(1); /* JMP 27-01-11 */

    // Gestion des arguments aprŠs lancement de la fenˆtre
    // Arguments valables pour tous les os

    // Set SW_SEG_SIZE
    pos = A_find(args, "-seg");                         /* JMP 27-11-07 */
    if(pos >= 0) SW_SEG_SIZE = atol(argv[pos + 1]);     /* JMP 27-11-07 */
    if(SW_SEG_SIZE < 16384) SW_SEG_SIZE = 16384;      /* JMP 27-11-07 */

    if(A_find(args, "-msg") >= 0 ||
            A_find(args, "-rep") >= 0 ||
            A_find(args, "-erep") >= 0 ||
            A_find(args, "-p") >= 0 ||
            A_find(args, "-l") >= 0) K_supmsg();

    if(ODE_init(argv[0]) < 0) exit(1);

    vkiap_appl->ap_nl = SCR_PAGE_SIZE[0] - 2;
    vkiap_appl->ap_nc = SCR_PAGE_SIZE[1];
    AP_display(vkiap_appl);
    ODE_adapt_size(0, 0);

    // -ws : charge les WS ws.*
    if(A_find(args, "-ws") >= 0) ws = 1;
    K_init_ws(ws);

    /* Nouvelle syntaxe iode filename.var filename.eqs ... */
    // 17/1/2019 :  filename.rep => exec report 
    
    for(pos = 1 ; pos < argc ; pos++) {
        if(args[pos][0] == '-') {
            pos++;
            continue;
        }
        else {
            type = X_findtype(args[pos]);
            if(type >= 10 && type <= 16) type -= 10; // ac .. av === cmt => var
            if(type >= 0  && type <= 6) {
                B_WsLoad(args[pos], type);
                loaded = type;
            }
            else if(type == 22) {  // REP // JMP 16/1/2019 (pas propre)
                B_ReportExec(args[pos]);
                W_close();
            }
        }
    }

    if((pos = A_find(args, "-l")) >= 0) {
        type = X_findtype(args[pos + 1]);
        if(type > 9 && type < 18) type -= 10;
        if(type > -1 && type < 8) {
            B_WsLoad(args[pos + 1], type);
            loaded = type;
        }
    }
    if(loaded >= 0) {
        K_supmsg();   /* JMP 09-04-98 */
        kwrprt();
        //AP_display(vkiap_appl);
        ODE_adapt_size(0, 0);
        SB_DataEditScroll(0L, loaded); /* JMP 10-03-98 */
    }

    if((pos = A_find(args, "-p")) >= 0) {
        type = X_findtype(args[pos + 1]);
        if(type > 9 && type < 18) type -= 10;
        if(type > -1 && type < 8) {
            B_WsLoad(args[pos + 1], type);
            if(!C_PrintDestDecLang(0, -1, 0)) B_PrintObjDef(0L, type);
            W_close();

            RP_quitode(0L);
        }
    }

    if((pos = A_find(args, "-rep")) >= 0) {
        B_ReportExec(args[pos + 1]);
        W_close();
    }

    if((pos = A_find(args, "-erep")) >= 0) {
        SB_FileEdit(args[pos + 1]);
    }

    kwrprt();
    AP_edit(vkiap_appl);
    AP_undisplay(vkiap_appl);
#ifdef UNIX
    system("clear");
#endif
#ifdef WATCOM
    system("cls");
#endif
    exit(0);
}

SWHDL ODE_OLD_SCREEN;
extern char    *SCR_DOS_SCREEN,
       *SCR_OLD_SCREEN;

ODE_init(argv0)
char    *argv0;
{
    int     kclmsg(), size;

    SW_MIN_MEM = 120 * 1024L;
    SW_MIN_SEGS = 2;
    SW_init(1);

    ODE_init_argv0(argv0); /* JMP_M 4.21 01-09-95 */

#ifdef DOS
    if(SCR_OLD_SCREEN) {
        size = 2 * SCR_PAGE_SIZE[0] * SCR_PAGE_SIZE[1];
        ODE_OLD_SCREEN = SW_alloc(size);
        memcpy(SW_getptr(ODE_OLD_SCREEN), SCR_OLD_SCREEN, size);
        SCR_free(SCR_OLD_SCREEN);
        SCR_OLD_SCREEN = 0;
    }
#if !defined(WATCOM) && !defined(DOSWIN) && !defined(SCRW32) /* JMP 09-02-97 */
    {
        extern int SCR_harderror(); /* JMP41 20-11-92 */
        harderr(SCR_harderror);     /* JMP41 20-11-92 */
    }
#endif
#endif

    SCR_prof_load("iode"); /* JMP 15-06-96 */
    //ODE_adapt_ncnl(global_WNL, global_WNC); /* JMP 10-03-11 */
    ODE_adapt_ncnl(ODE_NL, ODE_NC); /* JMP 6-13-15 */

    SCR_LOCK_TIME = 0; /* JMP 24-01-99 */
    SCR_LOCK_TXT = "I.O.D.E. (c) Planning Office";
    K_SCR = 1;
    B_LANG = 0; /* JMP38 01-10-92 */

    C_FilePrintSetup();

    OSCRL_init();                  /* Open scrolling facilities */
    TSCRL_init();                  /* Open scrolling facilities */
    VT_init();

    B_NBDEC = -1;
    IodeStartDde(); /* JMP 17-06-97 */
    ODE_settitle();
    return(0);
}


ODE_settitle()
{
#ifndef UNIX
    U_ch    buf[512], dirname[256], instance[20], vers[10];  /* JMP 4-10-2015 */
    int     count;

    
    // Pb reading the web site : SCR_HttpGetFile sur ovh ne fonctionne plus
    if(0) {
        
        if(ODE_CHECKVERSION) ODE_LASTVERS = K_LastVersion();
        else 				 ODE_LASTVERS = K_CurrentVersion();
    }
    else {    
        ODE_LASTVERS = K_CurrentVersion(); 
    }
    
    ODE_CURVERS  = K_CurrentVersion();
    //Debug("ODE_CHECKVERSION %d\n", ODE_CHECKVERSION);
    //Debug("ODE_LASTVERS %s\n", ODE_LASTVERS);
    //Debug("ODE_CURVERS %s\n", ODE_CURVERS);

    SCR_getcwd(0, buf);
    count = WscrGetCurrentProcessCount();
    if(count > 1) {
        if(WscrRegisterGetApplData("IODE", "Instance", instance) == 0)
            count = 1 + atoi(instance);

        sprintf(dirname, ":%d - %s", ODE_CURVERS, count, buf);
    }
    else {
        if(ODE_CHECKVERSION == 0 || SCR_cstrcmp(ODE_LASTVERS, ODE_CURVERS) == 0)
            sprintf(dirname, "%s - %s", ODE_CURVERS, buf);
        else
            sprintf(dirname, "%s (last version => %s) - %s", ODE_CURVERS, ODE_LASTVERS, buf );
    }

    sprintf(instance, "%d", count);
    WscrRegisterSetApplData("IODE", "Instance", instance);

#ifndef SCRW32
    WscrDOSSetWindowText(dirname);
#endif
#endif //UNIX
}

ODE_end(disp)
int     disp;
{
    int     size;

    global_WNL = SCR_PAGE_SIZE[0]; /* JMP 10-03-11 */
    global_WNC = SCR_PAGE_SIZE[1]; /* JMP 10-03-11 */
    SCR_prof_save("iode");  /* JMP 04-02-96 */
#ifdef DOS
    size = 2 * SCR_PAGE_SIZE[0] * SCR_PAGE_SIZE[1];
    if(disp && ODE_OLD_SCREEN && SCR_DOS_SCREEN) {
        if(first_page) {
            Sys_clear();
            printf("Thank you for using IODE\n");
        }
        else {
            memcpy(SCR_DOS_SCREEN, SW_getptr(ODE_OLD_SCREEN), size);
            SW_free(ODE_OLD_SCREEN);
        }
    }
#endif
    SCR_end();
    SW_end();
    W_close();
    IodeEndDde(); /* JMP 17-06-97 */
}

ODE_quit()
{
    char    *buts[4];

    buts[0] = "Quit";
    buts[1] = "Save and Quit";
    buts[2] = "Cancel";
    buts[3] = 0;

    switch(SCR_MessageBox("I.O.D.E", "Quit IODE ?", buts)) {
    case 1:
        if(SB_WsSave()) return(1); // Pb = Event trait‚ (ne pas sortir)
        // Si save ok, terminer sur case 0
    case 0:
        SCR_APP_ABORT = ACT_APPL_QUIT;
        K_end_ws(0);
        ODE_end(1);
        //return(0);
        exit(0);
    default:
        return(1); // Event trait‚ par le programme (ne pas sortir)
    }
}


Sys_clear()
{
#if defined(DOS) && !defined(DOSWIN) && !defined(SCRW32)
    system("cls");
#endif
#ifdef UNIX
    system("clear");
#endif
}


A_expandstar(arg) /* JMP 10-03-98 */
char    *arg;
{
    if(ODE_NOEXPANDSTAR) return(-1);
    return(A_expandstar_dft(arg));
}

/* SUPPRESSION DE FNS */

PG_display_file()       {}
PG_display_slide()      {}
SCR_dump_pg()           {}
PG_display_infos()      {}


/* YY DEF et YY INCL */

YY_include()            {}
YY_unsave_define()      {}
YY_ifdef()              {}
YY_define()             {}
YY_undef()              {}
YY_find_end_else()      {}
YY_save_define()        {}
YY_find_def()           {}


A_error(txt)
char    *txt;
{
    B_seterror("%.80s", txt);
}

char    *A_expand(name)
char    *name;
{
    int     pos;

    pos = K_find(KL_WS, name);
    if (pos < 0) return(NULL);
    return((char *)KLVAL(KL_WS, pos));
}

char    *L_expand(name)
char    *name;
{
    return(A_expand(name));
}

/*********** DDE ********************/

#if defined(DOSW32) || defined(SCRW32)

IodeStartDde()
{
    extern char *IodeDdeGetItem();
    extern int  IodeDdeSetItem();
    char        **topic;

    topic = SCR_vtoms("VAR,SCL,LST,EQS,TBL,CMT,IDT,WS,PLAY,REP,XVAR,XLST,XIDT,XCMT,XTBL,XREP", ",");
    WscrDdeServer("IODE", topic, IodeDdeGetItem, IodeDdeSetItem);
    SCR_free_tbl(topic);
}

IodeEndDde()
{
    WscrDdeServerEnd();
}

#else
IodeStartDde() {}
IodeEndDde() {}
#endif

#ifdef X11
X11UserInit(argc, argv)
int     argc;
char    *argv[];
{
    int     nl = 30, nc = 90, i, fontnb = -1;
    char    *getenv(), *ptr;
    extern int X_wait_for_next_event;
    X_wait_for_next_event = 1;
    if(ptr = getenv("MTNL")) nl = atoi(ptr);     /* JMP 30-09-98 */
    if(ptr = getenv("MTNC")) nc = atoi(ptr);     /* JMP 30-09-98 */
    for(i = 1; i < argc ; i++) {
        if(strcmp(argv[i], "-nl") == 0) nl = atoi(argv[i + 1]);
        if(strcmp(argv[i], "-nc") == 0) nc = atoi(argv[i + 1]);
    }
    if(nl < 5) nl = 5;
    if(nl > 80) nl = 80;
    if(nc < 15) nc = 15;
    if(nc > 140) nc = 140;
    SCR_PAGE_SIZE[0] = nl;
    SCR_PAGE_SIZE[1] = nc;
    return(0);
}
#endif









