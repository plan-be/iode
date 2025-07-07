#include "s_a2m.h"

/***************************************************************/

U_ch    A2M_CURTAG1[41], A2M_CURCTAG[41], A2M_GALIGN;
A2MPPR  *A2M_PPRS;
int     A2M_NB_PPRS;
int     A2M_CUROBJ = 0;
A2MTOP  **A2M_TOPICS = 0;
int     A2M_NB_TOPICS = 0;
int     A2M_CURTOPIC = -1;

/*********** MODIFIABLES PAR PROGRAMME *****************/
U_ch    A2M_CURTAG[41] = "par_0";
U_ch    *A2M_PGHEAD = 0, *A2M_PGFOOT = 0;
U_ch    *A2M_IMGPATH = 0; /* JMP 24-09-10 */
int     A2M_CMDCH = '.', A2M_ESCCH = '\\', A2M_DEFCH = '&',
        A2M_SEPCH = '&';
U_ch    *A2M_IGNORECHARS = 0;
int     A2M_LFON = 0, A2M_BLON = 0,
        A2M_TBREAK = 1, A2M_TPAGE = 0, A2M_GPAGE = 0; /* JMP 08-09-97 */
int     A2M_TSHADING_COL[2] = {4, 4}, A2M_TSHADING_BRUSH[2] = {7, 5};
double  *A2M_TWIDTHS = 0;
int     A2M_NB_TWIDTHS = 0;
int     A2M_TOPS_CASE = 1; /* JMP 21-05-99 */
int     A2M_TOPS_AUTO = 1; /* JMP 21-05-99 */
int     A2M_TOPS_PARTIAL = 0; /* JMP 21-05-99 */
int     A2M_TOPS_TITLE = 0; /* JMP 24-05-99 */

/*NH*/
A2MFILE *A2mOpen(char    *filename, int     type)
{
    A2MFILE *af = 0;
    CPPFILE *cpp;
    //int     i;

    A2M_CURTAG1[0] = 0;
    A2M_CURCTAG[0] = 0;
    A2M_PPRS = 0;
    A2M_NB_PPRS = 0;
    A2M_CURTOPIC = -1;
    /*    for(i = 0 ; i < 2 ; i++) {
    	A2M_TSHADING_COL[i] = 4;
    	A2M_TSHADING_BRUSH[i] = 7 - 2 * i;
    	}
    /* JMP 08-09-97 */

    CPP_DIRCH = A2M_CMDCH;
    CPP_DEFCH = A2M_DEFCH;

    cpp = CppOpen(filename, type);
    if(cpp == 0) return(af);
    af = (A2MFILE *) SCR_malloc(sizeof(A2MFILE));
    af->af_cpp = cpp;
    A2mCreateParCatalog(af);
    return(af);
}

/*NH*/
int A2mClose(af)
A2MFILE *af;
{
    if(af == 0) return(0);
    CppClose(af->af_cpp);
    SCR_free(af->af_outfile);
    SCR_free(af);
    SCR_free(A2M_PGHEAD);
    SCR_free(A2M_PGFOOT);
    A2M_PGFOOT = A2M_PGHEAD = 0;
    SCR_free(A2M_PPRS);
    A2M_PPRS = 0;
    A2M_NB_PPRS = 0;
    return(0);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MPAR *A2mNewPar(
    char    *tag
)
#else
A2MPAR *A2mNewPar(tag)
char    *tag;
#endif
{
    A2MPAR  *ap;

    ap = (A2MPAR *) SCR_malloc(sizeof(A2MPAR));

    if(tag) A2mSetParTag(ap, tag);
    else {
        if(A2M_CURTAG1[0]) {
            A2mSetParTag(ap, A2M_CURTAG1);
            A2M_CURTAG1[0] = 0;
        }
        else if(A2M_CURTAG[0]) A2mSetParTag(ap, A2M_CURTAG);
        else                   A2mSetParTag(ap, "par_0");
    }

    return(ap);
}

/*NH*/
int A2mSetParTag(ap, tag)
A2MPAR  *ap;
U_ch    *tag;
{
    if(ap == 0) return(0);
    SCR_free(ap->ap_tag);
    ap->ap_tag = SCR_stracpy(tag);
    return(0);
}

/*NH*/
int A2mFreePar(ap)
A2MPAR  *ap;
{
    if(ap == 0) return(0);
    A2mFreeStrs(ap->ap_strs);
    A2mFreeStrs(ap->ap_as2);
    SCR_free(ap->ap_tag);
    SCR_free(ap);
    return(0);
}

/*NH*/
int A2mFreeStrs(as)
A2MSTR  **as;
{
    int     i;

    if(as == 0) return(0);
    for(i = 0 ; as[i] ; i++) {
        SCR_free(as[i]->as_txt);
        SCR_free(as[i]->as_ctag);
        SCR_free(as[i]);
    }
    SCR_free(as);
    return(0);
}

/*NH*/
A2MTBL *A2mNewTbl()
{
    A2MTBL  *at;

    at = (A2MTBL *) SCR_malloc(sizeof(A2MTBL));

    at->at_border = -1;
    at->at_tspaceb = -10000; /* JMP 15-03-2004 */
    at->at_tspacea = -10000; /* JMP 15-03-2004 */
    return(at);
}

/*NH*/
A2MTOP *A2mNewTopic()
{
    A2MTOP  *atp;

    atp = (A2MTOP *) SCR_malloc(sizeof(A2MTOP));
    atp->atp_linkcase = A2M_TOPS_CASE;
    atp->atp_linkpart = A2M_TOPS_PARTIAL;
    atp->atp_linkauto = A2M_TOPS_AUTO;
    atp->atp_linktitle  = A2M_TOPS_TITLE;

    return(atp);
}

/*NH*/
int A2mFreeTopic(atp)
A2MTOP  *atp;
{
    if(atp == 0) return(0);
    SCR_free(atp->atp_keyws);
    SCR_free(atp->atp_keywsu);
    SCR_free(atp);
    return(0);
}

/*NH*/
A2MPAGE *A2mNewPage()
{
    A2MPAGE  *apg;

    apg = (A2MPAGE *) SCR_malloc(sizeof(A2MPAGE));

    return(apg);
}

/*NH*/
int A2mFreePage(apg)
A2MPAGE  *apg;
{
    SCR_free(apg);
    return(0);
}

/*NH*/
A2MLINE *A2mNewLine()
{
    A2MLINE *al;

    al = (A2MLINE *) SCR_malloc(sizeof(A2MLINE));

    return(al);
}

/*NH*/
A2MGOTO *A2mNewGoto()
{
    A2MGOTO *ag;

    ag = (A2MGOTO *) SCR_malloc(sizeof(A2MGOTO));

    return(ag);
}

/*NH*/
A2MMARGINS *A2mNewMargins()
{
    A2MMARGINS *am;

    am = (A2MMARGINS *) SCR_malloc(sizeof(A2MMARGINS));

    return(am);
}

/*NH*/
int A2mFreeLine(al)
A2MLINE  *al;
{
    SCR_free(al);
    return(0);
}

/*NH*/
int A2mAddTblLine(at, type, pos)
A2MTBL  *at;
int     type, pos;
{
    at->at_tls = (A2MTL *) SCR_realloc(at->at_tls, sizeof(A2MTL),
                                       at->at_nl, at->at_nl + 1);

    if(type == 0)
        at->at_tls[at->at_nl].atl_tcs =
            (A2MTC *) SCR_malloc(at->at_nc * sizeof(A2MTC));

    at->at_tls[at->at_nl].atl_type = type;
    at->at_tls[at->at_nl].atl_hbf = pos;
    at->at_nl++;
    return(0);
}

/*NH*/
int A2mSetTblNc(at, nc)
A2MTBL  *at;
int     nc;
{
    int     i;

    if(nc <= at->at_nc) return(0);
    for(i = 0 ; i < at->at_nl ; i++)
        at->at_tls[i].atl_tcs =
            (A2MTC *) SCR_realloc(at->at_tls[i].atl_tcs, sizeof(A2MTC),
                                  at->at_nc, nc);

    at->at_nc = nc;
    return(0);
}


/*NH*/
int A2mFreeTbl(at)
A2MTBL  *at;
{
    int     i, j;
    A2MTL   *tl;

    if(at == 0) return(0);

    for(i = 0 ; i < at->at_nl; i++) {
        tl = at->at_tls + i;
        if(tl->atl_type) continue;
        for(j = 0 ; j < at->at_nc; j++)
            A2mFreePar(tl->atl_tcs[j].atc_par);
        SCR_free(tl->atl_tcs);
    }

    A2mFreePar(at->at_title);
    A2mFreePar(at->at_footnote);
    SCR_free(at->at_tls);
    SCR_free(at->at_tag);
    SCR_free(at->at_widths);
    SCR_free(at->at_ws);
    SCR_free(at);
    return(0);
}

/*NH*/
A2MGRF *A2mNewGrf()
{
    A2MGRF  *ag;
    int     i;

    ag = (A2MGRF *) SCR_malloc(sizeof(A2MGRF));

    for(i = 0 ; i < 3 ; i++)
        ag->ag_mins[i] = ag->ag_maxs[i] = SCR_NOT_AVAIL;

    return(ag);
}


/*NH*/
int A2mFreeGrids(grids)
A2MGRID **grids;
{
    SCR_free_tbl((U_ch **)grids);
    return(0);
}

/*NH*/
int A2mFreeLabs(labs)
A2MLAB  **labs;
{
    int     i;

    if(labs == 0) return(0);
    for(i = 0 ; labs[i] ; i++) A2mFreePar(labs[i]->al_par);

    SCR_free_tbl((U_ch **)labs);
    return(0);
}

/*NH*/
int A2mFreeGXY(gxy)
A2MGXY  **gxy;
{
    int     i;

    if(gxy == 0) return(0);
    for(i = 0 ; gxy[i] ; i++) {
        A2mFreePar(gxy[i]->xy_title);
        SCR_free(gxy[i]->xy_vals);
        SCR_free(gxy[i]);
    }

    SCR_free(gxy);
    return(0);
}

/*NH*/
int A2mFreeGrf(ag)
A2MGRF  *ag;
{
    int     i;

    if(ag == 0) return(0);
    A2mFreePar(ag->ag_title);
    A2mFreePar(ag->ag_footnote);
    A2mFreePar(ag->ag_header);
    A2mFreePar(ag->ag_footer);
    A2mFreeGXY(ag->ag_gxy);

    for(i = 0 ; i < 3 ; i++) {
        A2mFreePar(ag->ag_legends[i]);
        A2mFreeGrids(ag->ag_grids[i]);
        A2mFreeLabs(ag->ag_labs[i]);
    }

    SCR_free(ag);
    return(0);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mNewObj(
    int     type
)
#else
A2MOBJ *A2mNewObj(type)
int     type;
#endif
{
    A2MOBJ  *ao = (A2MOBJ *) SCR_malloc(sizeof(A2MOBJ));

    ao->ao_type = type;
    switch(type) {
    case A2M_PAR   :
        ao->ao_ptr = (char *)A2mNewPar(0L);
        break;
    case A2M_TBL   :
        ao->ao_ptr = (char *)A2mNewTbl();
        break;
    case A2M_GRF   :
        ao->ao_ptr = (char *)A2mNewGrf();
        break;
    case A2M_TOPIC :
    case A2M_TOPICALIAS:
        ao->ao_ptr = (char *)A2mNewTopic();
        break;
    case A2M_PAGE  :
        ao->ao_ptr = (char *)A2mNewPage();
        break;
    case A2M_LINE  :
        ao->ao_ptr = (char *)A2mNewLine();
        break;
    case A2M_GOTO  :
        ao->ao_ptr = (char *)A2mNewGoto();
        break;
    case A2M_MARGINS :
        ao->ao_ptr = (char *)A2mNewMargins();
        break;
    default :
        break;
    }
    return(ao);
}

/*NH*/
int A2mFreeObj(ao)
A2MOBJ  *ao;
{
    if(ao == 0) return(0);
    switch(ao->ao_type) {
    case A2M_PAR :
        A2mFreePar((A2MPAR *)ao->ao_ptr);
        break;
    case A2M_TBL :
        A2mFreeTbl((A2MTBL *)ao->ao_ptr);
        break;
    case A2M_GRF :
        A2mFreeGrf((A2MGRF *)ao->ao_ptr);
        break;
    case A2M_TOPIC :
    case A2M_TOPICALIAS :
        A2mFreeTopic((A2MTOP *)ao->ao_ptr);
        break;
    case A2M_PAGE :
        A2mFreePage((A2MPAGE *)ao->ao_ptr);
        break;
    case A2M_LINE :
        A2mFreeLine((A2MLINE *)ao->ao_ptr);
        break;
    default :
        break;
    }
    SCR_free(ao);
    return(0);
}

/*NH*/
int A2mReadVar(af, var)
A2MFILE *af;
U_ch    **var;
{
    SCR_free(*var);
    CppSkipSpaces(af->af_cpp);
    *var = CppReadToChars(af->af_cpp, "\n");
    CppGetc(af->af_cpp); /* read \n */
    return(0);
}

/*NH*/
int A2mReadInt(af)
A2MFILE *af;
{
    int     i;

    i = CppReadLong(af->af_cpp);
    if(i == EOF) return(-1);
    CppSkipToEol(af->af_cpp);
    return(i);
}

/*NH*/
int A2mDefCh(af, escch)
A2MFILE *af;
int     *escch;
{
    int     ch;

    CppSkipSpaces(af->af_cpp);
    ch = CppGetc(af->af_cpp);
    if(ch == EOF) return(0);
    if(ch == '\n') return(0);
    *escch = ch;
    CppSkipToEol(af->af_cpp);
    return(0);
}

/*NH*/
int A2mDefPar(af, parname)
A2MFILE *af;
char    *parname;
{
    char    str[51];

    CppSkipSpaces(af->af_cpp);
    CppReadWord(af->af_cpp, str, 50);
    SCR_strlcpy(parname, str, 40);
    CppSkipToEol(af->af_cpp);
    return(0);
}

/*NH*/

#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mReadLine(
    A2MFILE *af,
    int     type
)
#else
A2MOBJ *A2mReadLine(af, type)
A2MFILE *af;
int     type;
#endif

{
    A2MOBJ  *ao;
    A2MLINE *al;

    ao = A2mNewObj(A2M_LINE);
    al = (A2MLINE *)ao->ao_ptr;

    al->al_type = type;
    if(type == 0) al->al_type = (short)CppReadLong(af->af_cpp);
    CppSkipToEol(af->af_cpp);
    return(ao);
}

#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mReadGoto(
    A2MFILE *af
)
#else
A2MOBJ *A2mReadGoto(af)
A2MFILE *af;
#endif

{
    A2MOBJ  *ao;
    A2MGOTO *ag;

    ao = A2mNewObj(A2M_GOTO);
    ag = (A2MGOTO *)ao->ao_ptr;

    ag->ag_x = CppReadDbl(af->af_cpp);
    ag->ag_y = CppReadDbl(af->af_cpp);
    CppSkipToEol(af->af_cpp);
    return(ao);
}

#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mReadMargins(
    A2MFILE *af
)
#else
A2MOBJ *A2mReadMargins(af)
A2MFILE *af;
#endif

{
    A2MOBJ      *ao;
    A2MMARGINS  *am;

    ao = A2mNewObj(A2M_MARGINS);
    am = (A2MMARGINS *)ao->ao_ptr;

    am->am_left     = CppReadDbl(af->af_cpp);
    am->am_right    = CppReadDbl(af->af_cpp);
    am->am_top      = CppReadDbl(af->af_cpp);
    am->am_bottom   = CppReadDbl(af->af_cpp);
    CppSkipToEol(af->af_cpp);
    return(ao);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mReadPage(
    A2MFILE *af
)
#else
A2MOBJ *A2mReadPage(af)
A2MFILE *af;
#endif
{
    A2MOBJ  *ao;
    A2MPAGE *apg;

    ao = A2mNewObj(A2M_PAGE);
    apg = (A2MPAGE *)ao->ao_ptr;

    CppSkipToEol(af->af_cpp);
    return(ao);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mReadTopic(
    A2MFILE *af
)
#else
A2MOBJ *A2mReadTopic(af)
A2MFILE *af;
#endif
{
    A2MOBJ  *ao;
    A2MTOP  *atp;

    ao = A2mNewObj(A2M_TOPIC);
    atp = (A2MTOP *)ao->ao_ptr;

    A2M_CURTOPIC = atp->atp_nb = (short)CppReadLong(af->af_cpp);
    atp->atp_level = (short)CppReadLong(af->af_cpp);
    CppSkipSpaces(af->af_cpp);
    atp->atp_keyws = CppReadToChars(af->af_cpp, "\n");
    if(atp->atp_keyws == 0) atp->atp_keyws = SCR_stracpy("");
    SCR_strip(atp->atp_keyws);
    atp->atp_keywsu = SCR_stracpy(atp->atp_keyws);
    SCR_upper(atp->atp_keywsu);
    CppSkipToEol(af->af_cpp);
    atp->atp_lg = (short)strlen(atp->atp_keyws);
    return(ao);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mReadTopicAlias(
    A2MFILE *af
)
#else
A2MOBJ *A2mReadTopicAlias(af)
A2MFILE *af;
#endif
{
    A2MOBJ  *ao;
    A2MTOP  *atp;

    ao = A2mNewObj(A2M_TOPICALIAS);
    atp = (A2MTOP *)ao->ao_ptr;

    atp->atp_nb = A2M_CURTOPIC;
    CppSkipSpaces(af->af_cpp);
    atp->atp_keyws = CppReadToChars(af->af_cpp, "\n");
    SCR_strip(atp->atp_keyws);
    atp->atp_keywsu = SCR_stracpy(atp->atp_keyws);
    SCR_upper(atp->atp_keywsu);
    CppSkipToEol(af->af_cpp);
    atp->atp_lg = (short)strlen(atp->atp_keyws);
    return(ao);
}

/*NH*/
int A2mReadTopicLink(af, atp)
A2MFILE *af;
A2MTOP  *atp;
{
    char    buf[65];
    int     ch, val;

    while(1) {
        CppSkipSpaces(af->af_cpp);
        ch = CppGetc(af->af_cpp);
        if(ch == '\n' || ch == EOF) return(0);
        CppUngetc(af->af_cpp, ch);
        CppReadWord(af->af_cpp, buf, 64);
        SCR_lower(buf);
        if(strcmp(buf, "auto") == 0) {
            val = A2mReadParChars(af, "NY", 1);
            if(atp) atp->atp_linkauto = val;
        }
        else if(strcmp(buf, "case") == 0) {
            val = A2mReadParChars(af, "NY", 1);
            if(atp) atp->atp_linkcase = val;
        }
        else if(strcmp(buf, "partial") == 0) {
            val = A2mReadParChars(af, "NY", 1);
            if(atp) atp->atp_linkpart = val;
        }
        else if(strcmp(buf, "title") == 0) {
            val = A2mReadParChars(af, "NY", 1);
            if(atp) atp->atp_linktitle = val;
        }
        else {
            A2mPrintError(af, "Syntax error");
            CppSkipToEol(af->af_cpp);
            return(0);
        }
    }
}

/*NH*/
int A2mReadTopicsLink(af)
A2MFILE *af;
{
    char    buf[65];
    int     ch;

    while(1) {
        CppSkipSpaces(af->af_cpp);
        ch = CppGetc(af->af_cpp);
        if(ch == '\n' || ch == EOF) return(0);
        CppUngetc(af->af_cpp, ch);
        CppReadWord(af->af_cpp, buf, 64);
        SCR_lower(buf);
        if(strcmp(buf, "auto") == 0)         A2M_TOPS_AUTO = A2mReadParChars(af, "NY", 1);
        else if(strcmp(buf, "case") == 0)    A2M_TOPS_CASE = A2mReadParChars(af, "NY", 1);
        else if(strcmp(buf, "partial") == 0) A2M_TOPS_PARTIAL = A2mReadParChars(af, "NY", 1);
        else if(strcmp(buf, "title") == 0)   A2M_TOPS_TITLE = A2mReadParChars(af, "NY", 1);
        else {
            A2mPrintError(af, "Syntax error");
            CppSkipToEol(af->af_cpp);
            return(0);
        }
    }
}


/*NH*/
#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mReadPar(
    A2MFILE *af
)
#else
A2MOBJ *A2mReadPar(af)
A2MFILE *af;
#endif
{
    int     ch, i;
    A2MOBJ  *ao;
    A2MPAR  *ap;
    U_ch    *line;
    U_ch    ignorechars[256]; /* JMP 24-07-2014 */

    ao = A2mNewObj(A2M_PAR);
    ap = (A2MPAR *)ao->ao_ptr;

    ap->ap_lfon = A2M_LFON;
    while(1) {
        ch = CppGetc(af->af_cpp);
        if(ch != EOF) CppUngetc(af->af_cpp, ch);
        if(ch == A2M_CMDCH || ch == EOF) break;
        line = CppReadLine(af->af_cpp);
        if(line == 0) {
            if(A2M_LFON == 0) break;
            else line = SCR_stracpy(" ");  /* JMP 15-05-99 */
        }
        if(line[0] == 0 && A2M_LFON == 0) { /* JMP 15-05-99 */
            SCR_free(line);
            break;
        }

        if(A2M_LFON == 0) {
            strcpy(ignorechars, " \t\304\315\227"); /* JMP 24-07-2014 */
            if(A2M_IGNORECHARS)
                SCR_strlcpy(ignorechars + strlen(ignorechars), A2M_IGNORECHARS, 200);   /* JMP 24-07-2014 */

            for(i = 0 ; line[i] ; i++)
                if(!U_is_in(line[i], ignorechars)) break; /* JMP 24-07-2014 */
            if(line[i] == 0) {
                SCR_free(line);
                break;
            }
        }

        if(ap->ap_strs) {
            if(A2M_LFON) ap->ap_strs = A2mAddParLine(ap->ap_strs, 0L);
            else         ap->ap_strs = A2mAddParLine(ap->ap_strs, " ");
        }

        if(A2M_BLON == 0) A2mSupdbl(line, 1);
        ap->ap_strs = A2mAddParLine(ap->ap_strs, line);
        SCR_free(line);
    }

    if(ap->ap_strs == 0) {
        A2mFreeObj(ao);
        ao = 0;
    }

    return(ao);
}

/*NH*/
int A2mSupdbl(txt, ljust)
U_ch    *txt;
int     ljust;
{
    int     i = 0, j = 0, prev = 1;

    if(ljust == 0)
        for(; txt[i] ; i++, j++) if(txt[i] !=  ' ') break;

    for(; txt[i] ; i++) {
        if(txt[i] != ' ' || prev == 0) {
            txt[j++] = txt[i];
            if(txt[i] == ' ') prev = 1;
            else              prev = 0;
        }
    }

    txt[j] = 0;
    if(j > 0 && txt[j - 1] == ' ') {
        if(ljust || j > 1) txt[j - 1] = 0;
    }
    return(0);
}

/************ TABLES ***************************/

char *A2M_TBLTAGS[] = {
    "Left", "Center", "Right", "Decimal", "Just", ""
};

char *A2M_TBLHBF[] = {
    "Header", "Cell", "Footer"
};

/*NH*/
int A2mReadTblLine(af, at, pos)
A2MFILE *af;
A2MTBL  *at;
int     pos;
{
    U_ch    chars[3], *cell, str[51];
    A2MTC   *tc;
    A2MPAR  *ap;
    int     ch, col = 0, tag;

    sprintf(chars, "\n%c", A2M_SEPCH);
    A2mAddTblLine(at, 0, pos);

    while(1) {
        if(col >= at->at_nc) {
            A2mSetTblNc(at, col + 1);
            /*   A2mPrintError(af, "Too many columns");
               CppSkipToEol(af->af_cpp);
               return;
            */
        }
        tc = at->at_tls[at->at_nl - 1].atl_tcs + col;

        ch = CppGetc(af->af_cpp);

        if(ch == ' ') {         // JMP 6/1/2022 pour accepter les tableaux | 21|... sans avoir 21 colonnes
            tc->atc_ncells = 0; // Si le char 1 == ' ' on le skippe et on ne prend pas l'entier qui
        }                        // suit comme le #cols à regrouper (span)
        else {
            CppUngetc(af->af_cpp, ch);
            tc->atc_ncells = (short)CppReadLong(af->af_cpp);
        }

        if(tc->atc_ncells == 0) {
            tc->atc_ncells = 1;
            tc->atc_center = (pos == 1) ? 0 : 1;
        }
        else {
            ch = CppGetc(af->af_cpp);
            tc->atc_center = U_pos(ch, "LCRDJN");     /* JMP 17-03-00 */
            if(tc->atc_center < 0) {
                tc->atc_center = 0;
                CppUngetc(af->af_cpp, ch);
            }
            if(tc->atc_center >= 5) tc->atc_center++; /* JMP 17-03-00 */
        }
        ch = CppGetc(af->af_cpp);
        tag = 0;
        if(ch == '<') {
            CppSkipSpaces(af->af_cpp);
            CppReadWord(af->af_cpp, str, 50);
            CppSkipSpaces(af->af_cpp);
            ch = CppGetc(af->af_cpp);
            if(ch != '>') {
                CppUngetc(af->af_cpp, ch);
                A2mPrintError(af, "Syntax error");
            }
            else tag = 1;
            tc->atc_center = 5; /* JMP 28-01-99 */
        }
        else CppUngetc(af->af_cpp, ch);

        cell = CppReadToChars(af->af_cpp, chars);
        if(cell == 0) cell = SCR_stracpy(" ");
        if(tag) ap = A2mNewPar(str);
        else {
            sprintf(str, "%s%s",
                    A2M_TBLHBF[pos], A2M_TBLTAGS[min(4,tc->atc_center)]); // CORR JMP 31/3/2017 atc_center = 6 !!
            ap = A2mNewPar(str);
        }
        if(A2M_BLON == 0) A2mSupdbl(cell, col);
        ap->ap_strs = A2mAddParLine(ap->ap_strs, cell);
        tc->atc_par = ap;
        SCR_free(cell);

        ch = CppGetc(af->af_cpp);
        if(ch == '\n' || ch == EOF) return(0);
        col += tc->atc_ncells;
    }

}

/*NH*/
int A2mReadTblWidth(af, at)
A2MFILE *af;
A2MTBL  *at;
{
    U_ch    str[51];
    int     j, i = 0;
    /*
    #ifndef WATCOM
        double  atof();
    #endif
    cft stdlib.h
    /* JMP 04-01-98 */

    while(1) {
        CppSkipSpaces(af->af_cpp);
        CppReadString(af->af_cpp, str, 255);
        if(str[0] == 0) break;
        if(at->at_widths == 0)
            at->at_widths = (double *) SCR_malloc(at->at_nc * sizeof(double));

        at->at_widths[i] = atof((char *)str);
        if(at->at_widths[i] == 0) at->at_widths[i] = 1.0;
        i++;
        if(i >= at->at_nc) break;
    }

    if(i == 0) {                   /* JMP 18-02-99 */
        SCR_free(at->at_widths);   /* JMP 18-02-99 */
        at->at_widths = 0;         /* JMP 18-02-99 */
    }
    else {
        for(j = i; j < at->at_nc ; j++)
            at->at_widths[j] = at->at_widths[i - 1];
    }

    CppSkipToEol(af->af_cpp); /* \n */
    return(0);
}

/*NH*/
int A2mReadTblWidths(af)
A2MFILE *af;
{
    U_ch    str[51];
    int     i = 0;
    /*
    #ifndef WATCOM
        double  atof();
    #endif
    cft stdlib.h
    /* JMP 04-01-98 */

    if(A2M_TWIDTHS) {
        SCR_free(A2M_TWIDTHS);
        A2M_TWIDTHS = 0;
        A2M_NB_TWIDTHS = 0;
    }

    while(1) {
        CppSkipSpaces(af->af_cpp);
        CppReadString(af->af_cpp, str, 255);
        if(str[0] == 0) break;
        if(A2M_NB_TWIDTHS % 10 == 0)
            A2M_TWIDTHS = (double *) SCR_realloc(A2M_TWIDTHS,
                                                 sizeof(double),
                                                 A2M_NB_TWIDTHS,
                                                 A2M_NB_TWIDTHS + 10);

        A2M_TWIDTHS[A2M_NB_TWIDTHS] = atof((char *)str);
        if(A2M_TWIDTHS[A2M_NB_TWIDTHS] == 0) A2M_TWIDTHS[A2M_NB_TWIDTHS] = 1.0;
        A2M_NB_TWIDTHS++;
    }

    CppSkipToEol(af->af_cpp); /* \n */
    return(0);
}

/*NH*/
int A2mReadParLine(af, ap)
A2MFILE *af;
A2MPAR  **ap;
{
    U_ch    *txt;

    CppSkipSpaces(af->af_cpp);
    txt = CppReadToChars(af->af_cpp, "\n");
    if(txt && txt[0]) {
        if(*ap == 0) *ap = A2mNewPar(0L);
        else (*ap)->ap_strs = A2mAddParLine((*ap)->ap_strs, 0L);
        (*ap)->ap_strs = A2mAddParLine((*ap)->ap_strs, txt);
    }
    SCR_free(txt);
    CppGetc(af->af_cpp); /* read \n */
    return(0);
}

/*NH*/
A2MOBJ *A2mReadTbl(A2MFILE *af)
{
    A2MOBJ  *ao = A2mNewObj(A2M_TBL);
    A2MTBL  *at = (A2MTBL *)ao->ao_ptr;
    U_ch    str[51];
    int     ch, pos = 1, i;

    /* .tb 12 */
    at->at_nc = (short)CppReadLong(af->af_cpp);
    if(at->at_nc == 0) at->at_nc = 1;
    CppSkipSpaces(af->af_cpp);
    at->at_tag = CppReadToChars(af->af_cpp, "\n");
    CppSkipToEol(af->af_cpp);

    while(1) {
        ch = CppGetc(af->af_cpp);
        if(ch == EOF) goto fin;

        /* .tl ou .sep ou .te */
        if(ch == A2M_CMDCH) {
            CppReadWord(af->af_cpp, str, 50);
            SCR_lower(str);
            if(A2mReadCmdNoObj(af, str)) ; /* No action */
            else if(strcmp("twidth", str) == 0) A2mReadTblWidth(af, at);

            else if(strcmp("ttitle", str) == 0)     A2mReadParLine(af, &(at->at_title));
            else if(strcmp("tfootnote", str) == 0)  A2mReadParLine(af, &(at->at_footnote));

            else if(strcmp("tborder", str) == 0) at->at_border = A2mReadInt(af);
            else if(strcmp("tbody", str) == 0)   {
                pos = 1;
                CppSkipToEol(af->af_cpp);
            }
            else if(strcmp("theader", str) == 0) {
                pos = 0;
                CppSkipToEol(af->af_cpp);
            }
            else if(strcmp("tfooter", str) == 0) {
                pos = 2;
                CppSkipToEol(af->af_cpp);
            }
            else if(strcmp("tspaceb", str) == 0) {
                at->at_tspaceb = A2mReadInt(af);   /* JMP 15-03-2004 */
            }
            else if(strcmp("tspacea", str) == 0) {
                at->at_tspacea = A2mReadInt(af);   /* JMP 15-03-2004 */
            }

            else if(strcmp("tl", str) == 0) {
                A2mAddTblLine(at, 1, pos);
                CppSkipToEol(af->af_cpp);
            }

            else if(strcmp("te", str) == 0) {
                CppSkipToEol(af->af_cpp);
                goto fin;
            }
            else {
                CppSkipToEol(af->af_cpp);
                A2mPrintError(af, "Syntax error");
            }
        }

        /* &nC....&.... */
        else if(ch == A2M_SEPCH)
            A2mReadTblLine(af, at, pos);

        else if(ch == '\n') ; /* Skip empty lines */

        else {
            A2mPrintError(af, "Syntax error");
            CppSkipToEol(af->af_cpp);
        }
    }
fin:
    A2mSetParTag(at->at_title, "TableTitle");
    A2mSetParTag(at->at_footnote, "TableFootnote");
    for(i = 0 ; i < 2 ; i++) {
        at->at_shadcol[i] = A2M_TSHADING_COL[i];
        at->at_shadbrush[i] = A2M_TSHADING_BRUSH[i];
    }
    at->at_break = A2M_TBREAK;
    at->at_newpage = A2M_TPAGE;
    if(at->at_widths == 0 && A2M_TWIDTHS) {  /* JMP 18-02-99 */
        at->at_widths = (double *) SCR_malloc(at->at_nc * sizeof(double));
        for(i = 0; i < at->at_nc && i < A2M_NB_TWIDTHS ; i++)
            at->at_widths[i] = A2M_TWIDTHS[i];
        if(i > 0) {
            for(; i < at->at_nc ; i++)
                at->at_widths[i] = at->at_widths[i - 1];
        }
    }

    A2mEndTbl(at);
    return(ao);
}

/*NH*/
int A2mEndTbl(A2MTBL* at)
{
    int     i, j;
    A2MTC   *tc;

    for(i = 0 ; i < at->at_nl ; i++) {
        if(at->at_tls[i].atl_type) continue;
        for(j = 0 ; j < at->at_nc ; j++) {
            tc = at->at_tls[i].atl_tcs + j;
            if(tc->atc_ncells == 0) tc->atc_ncells = 1;
            j += tc->atc_ncells - 1;
        }
    }
    return(0);
}

/*NH*/
/********** GRAPHICS **********************/

/*NH*/

int A2mReadGrTxt(af, txt)
A2MFILE *af;
U_ch    **txt;
{
    CppSkipSpaces(af->af_cpp);
    *txt = CppReadToChars(af->af_cpp, "\n");
    CppGetc(af->af_cpp); /* read \n */
    return(0);
}

/*NH*/
double A2mRead1Dbl(af)
A2MFILE *af;
{
    double  val;

    val = CppReadDbl(af->af_cpp);
    CppSkipToEol(af->af_cpp);
    return(val);
}

/*NH*/

/*NH*/
int A2mReadGrGrids(af, grds)
A2MFILE *af;
A2MGRID ***grds;
{
    A2MGRID grd;
    U_ch    str[256];
    int     nl = SCR_tbl_size((U_ch **)*grds);
    /*
    #ifndef WATCOM
        double  atof();
    #endif
    stdarg.h /* JMP 04-01-98 */

    grd.ag_type = 0;
    while(1) {
        CppSkipSpaces(af->af_cpp);
        CppReadString(af->af_cpp, str, 255);
        if(str[0] == 0) break;
        if(!U_is_in(str[0], "0123456789.-+")) {
            grd.ag_type = str[0];
            continue;
        }

        grd.ag_val = atof((char *)str);

        SCR_add_ptr((U_ch ***)grds, &nl, 0L);
        (*grds)[nl - 1] = (A2MGRID *)SCR_malloc(sizeof(A2MGRID));
        memcpy((*grds)[nl - 1], &grd, sizeof(A2MGRID));
    }

    CppGetc(af->af_cpp); /* \n */
    SCR_add_ptr((U_ch ***)grds, &nl, 0L);
    return(0);
}

/*NH*/
int A2mReadGrLabs(af, labs)
A2MFILE *af;
A2MLAB  ***labs;
{
    A2MLAB  lab;
    U_ch    *txt;
    int     ch, nl = SCR_tbl_size((U_ch **)*labs);
    /*
    #ifndef WATCOM
        double  atof();
    #endif
    stdarg.h /* JMP 04-01-98 */

    while(1) {
        A2mSkipBrackets(af);
        ch = CppGetc(af->af_cpp);
        if(U_is_in(ch, "0123456789.-+")) {
            CppUngetc(af->af_cpp, ch);
            lab.al_val = CppReadDbl(af->af_cpp);
            continue;
        }
        if(ch == '\n' || ch == EOF) break;
        if(ch != '"') continue;

        txt = CppReadQuotedString(af->af_cpp);
        lab.al_par = A2mNewPar("GraphLabel");
        lab.al_par->ap_strs = A2mAddParLine(0L, txt);
        SCR_free(txt);

        SCR_add_ptr((U_ch ***)labs, &nl, 0L);
        (*labs)[nl - 1] = (A2MLAB *)SCR_malloc(sizeof(A2MLAB));
        memcpy((*labs)[nl - 1], &lab, sizeof(A2MLAB));
    }

    SCR_add_ptr((U_ch ***)labs, &nl, 0L);
    return(0);
}

/*NH*/
int A2mReadGrGrid(af, grid)
A2MFILE *af;
short   *grid;
{
    int     i;
    U_ch    str[256];

    grid[0] = grid[1] = grid[2] = 'M';
    CppSkipSpaces(af->af_cpp);
    CppReadString(af->af_cpp, str, 255);
    CppSkipToChars(af->af_cpp, "\n");

    if(strlen(str) > 3) {
        SCR_upper(str);
        if(memcmp(str, "MI", 2) == 0)
            grid[0] = grid[1] = grid[2] = 'm';
        else if(memcmp(str, "NO", 2) == 0)
            grid[0] = grid[1] = grid[2] = 'N';
        return(0);
    }

    for(i = 0; i < (int)strlen(str); i++)
        if(U_is_in(str[i], "MmNnTt") != 0) grid[i] = str[i];

    return(0);
}


/*NH*/
int A2mReadGrXY(af, gxys, axis)
A2MFILE *af;
A2MGXY  ***gxys;
int     axis;
{
    A2MGXY  *gxy;
    U_ch    str[256], ch, *txt;
    int     nl = SCR_tbl_size((U_ch **)*gxys), nxy = 0;
    double  xy[2];
    /*
    #ifndef WATCOM
        double  atof();
    #endif
    stdarg.h /* JMP 04-01-98 */

    SCR_add_ptr((U_ch ***)gxys, &nl, 0L);
    gxy = (*gxys)[nl - 1] = (A2MGXY *)SCR_malloc(sizeof(A2MGXY));
    gxy->xy_axis = axis;

    CppSkipSpaces(af->af_cpp);
    CppReadString(af->af_cpp, str, 255);
    gxy->xy_type = str[0];
    CppSkipSpaces(af->af_cpp);
    ch = CppGetc(af->af_cpp);
    if(ch == '"') {
        txt = CppReadQuotedString(af->af_cpp);
        gxy->xy_title = A2mNewPar("GraphHeader");
        gxy->xy_title->ap_strs = A2mAddParLine(0L, txt);
        SCR_free(txt);
    }
    else CppUngetc(af->af_cpp, ch);

    while(1) {
        A2mSkipBrackets(af);
        CppReadString(af->af_cpp, str, 255);
        if(str[0] == 0) break;
        if(U_is_in(str[0], "0123456789.-+")) {
            if(strncmp(str, "-.-", 3) == 0) xy[nxy] = SCR_NAN;
            else xy[nxy] = atof((char *)str);
        }

        if(nxy == 0) {
            nxy = 1;
            continue;
        }

        gxy->xy_vals = (double *) SCR_realloc(gxy->xy_vals, 2 * sizeof(double),
                                              gxy->xy_nobs, gxy->xy_nobs + 1);
        gxy->xy_vals[2 * gxy->xy_nobs]     = xy[0];
        gxy->xy_vals[2 * gxy->xy_nobs + 1] = xy[1];
        gxy->xy_nobs += 1;
        nxy = 0;
    }

    CppGetc(af->af_cpp); /* \n */
    SCR_add_ptr((U_ch ***)gxys, &nl, 0L);
    return(0);
}

/*NH*/
int A2mReadAlign(af)
A2MFILE *af;
{
    U_ch    ch;

    CppSkipSpaces(af->af_cpp);
    ch = CppGetc(af->af_cpp);
    if(!U_is_in(ch,  "LMR")) {
        ch = 'L';
        CppUngetc(af->af_cpp, ch);
    }
    else CppGetc(af->af_cpp); /* \n */
    return((int) ch);
}

/*NH*/
int A2mReadGrTY(af, gxys, axis)
A2MFILE *af;
A2MGXY  ***gxys;
int     axis;
{
    A2MGXY  *gxy;
    U_ch    str[256], ch, *txt;
    int     nl = SCR_tbl_size((U_ch **)*gxys), nxy = 0;
    double  xy[2],
            align, period, skip;
    /*
    #ifndef WATCOM
        double  atof();
    #endif
    /* JMP 04-01-98 */

    SCR_add_ptr((U_ch ***)gxys, &nl, 0L);
    gxy = (*gxys)[nl - 1] = (A2MGXY *)SCR_malloc(sizeof(A2MGXY));
    gxy->xy_axis = axis;

    CppSkipSpaces(af->af_cpp);
    CppReadString(af->af_cpp, str, 255);
    gxy->xy_type = str[0];
    CppSkipSpaces(af->af_cpp);
    ch = CppGetc(af->af_cpp);
    if(ch == '"') {
        txt = CppReadQuotedString(af->af_cpp);
        gxy->xy_title = A2mNewPar("GraphHeader");
        gxy->xy_title->ap_strs = A2mAddParLine(0L, txt);
        SCR_free(txt);
    }
    else CppUngetc(af->af_cpp, ch);

    CppSkipSpaces(af->af_cpp);
    CppReadString(af->af_cpp, str, 255);
    A2mAnalysePeriod(str, &period, &skip);

    /*
        CppSkipSpaces(af->af_cpp);
        ch = CppGetc(af->af_cpp);
    */
    switch(A2M_GALIGN) {
    case 'L' :
        align = 0.0;
        break;
    case 'M' :
        align = skip/2;
        break;
    case 'R' :
        align = skip;
        break;

    default  :
        align = 0.0;
        CppUngetc(af->af_cpp, ch);
    }

    while(1) {
        A2mSkipBrackets(af);
        CppReadString(af->af_cpp, str, 255);
        if(str[0] == 0) break;

        xy[0] = period + align;
        if(U_is_in(str[0], "0123456789.-+")) {
            if(strncmp(str, "-.-", 3) == 0) xy[1] = SCR_NAN;
            else xy[1] = atof((char *)str);
        }
        gxy->xy_vals = (double *) SCR_realloc(gxy->xy_vals, 2 * sizeof(double),
                                              gxy->xy_nobs, gxy->xy_nobs + 1);
        gxy->xy_vals[2 * gxy->xy_nobs]     = xy[0];
        gxy->xy_vals[2 * gxy->xy_nobs + 1] = xy[1];
        gxy->xy_nobs += 1;
        period += skip;
    }

    CppGetc(af->af_cpp); /* \n */
    SCR_add_ptr((U_ch ***)gxys, &nl, 0L);
    return(0);
}

/*NH JMP 20-02-98 */
int A2mReadGrBox(af, ag)
A2MFILE *af;
A2MGRF  *ag;
{
    char    str[256];
    int     br, pct;

    ag->ag_box = (short)CppReadLong(af->af_cpp);
    CppSkipSpaces(af->af_cpp);
    CppReadWord(af->af_cpp, str, 255);
    ag->ag_fillcolor = U_pos(str[0], "Bwrgbcmy");
    if(ag->ag_fillcolor < 0) {
        A2mPrintError(af, "Undefined Color");
        ag->ag_fillcolor = 0;
    }

    pct = CppReadLong(af->af_cpp);
    if(pct >= 50)       br = 8;
    else if(pct >= 25)  br = 7;
    else if(pct >= 12)  br = 6;
    else if(pct >= 6)   br = 5;
    else if(pct >= 3)   br = 4;
    else if(pct >  0)   br = 3;
    else                br = 0;
    ag->ag_fillbrush = br;

    CppSkipToEol(af->af_cpp);
    return(0);
}


/*NH*/
int A2mSkipBrackets(af)
A2MFILE *af;
{
    int     ch;

    while(1) {
        CppSkipSpaces(af->af_cpp);
        ch = CppGetc(af->af_cpp);
        if(U_is_in(ch, ",;()[]{}")) continue;
        CppUngetc(af->af_cpp, ch);
        return(0);
    }
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mReadGraph(
    A2MFILE *af
)
#else
A2MOBJ *A2mReadGraph(af)
A2MFILE *af;
#endif

{
    A2MOBJ  *ao = A2mNewObj(A2M_GRF);
    A2MGRF  *ag = (A2MGRF *)ao->ao_ptr;
    U_ch    str[51];
    int     ch, i;
    double  d;

    /* Syntax
    .gb 10 10
    .gbox 2 b 50 -> cadre, colour, pct
    .gheader title1 (mindoc)
    .gfooter footer2 (minidoc)
    .gxtitle title
    .gytitle title
    .gztitle title
    .gxMin val
    .gxMax val
    .gyMin val
    .gyMax val
    .gzMin val
    .gzMax val
    .ggrid Major|Minor|None
    .gxgrid 12.5 3 6 9
    .gygrid 12.5 3 6 9
    .gzgrid 12.5 3 6 9
    .gxlabels (1.5 "Jan'90") ...
    .gylabels (1.5 "Jan'90") ...
    .gzlabels (1.5 "Jan'90") ...
    .gxy L|B|F|S "Title" (0.1 12.5) (2.5 -4.2) ...
    .gxz L|B|F|S "Title" (0.1 12.5) (2.5 -4.2) ...
    .gty L|B|F|S "Title" 1960M1 [ML] 12.6 4.8 7.9
    .gtz L|B|F|S "Title" 1960M1 [ML] 12.6 4.8 7.9

    .gpie "lab1" val1 "lab2" val2 ...
    */

    /* .gb width height */
    A2M_GALIGN = 'L';
    ag->ag_size[0] = CppReadDbl(af->af_cpp);
    ag->ag_size[1] = CppReadDbl(af->af_cpp);
    if(ag->ag_size[0] == 0) ag->ag_size[0] = 10.0;
    if(ag->ag_size[1] == 0) ag->ag_size[1] = 10.0;
    CppSkipToEol(af->af_cpp);

    while(1) {
        ch = CppGetc(af->af_cpp);
        if(ch == EOF) goto fin;
        if(ch == '\r' || ch == '\n') continue;

        /* .gx ou .go ou .ge */
        if(ch == A2M_CMDCH) {
            CppReadWord(af->af_cpp, str, 50);
            SCR_lower(str);
            if(A2mReadCmdNoObj(af, str)) ; /* No action */
            else if(strcmp("gtitle", str) == 0)
                A2mReadParLine(af, &(ag->ag_title));
            else if(strcmp("gfootnote", str) == 0)
                A2mReadParLine(af, &(ag->ag_footnote));
            else if(strcmp("gheader", str) == 0)
                A2mReadParLine(af, &(ag->ag_header));
            else if(strcmp("gfooter", str) == 0)
                A2mReadParLine(af, &(ag->ag_footer));
            else if(strcmp("galign", str) == 0)
                A2M_GALIGN = A2mReadAlign(af);
            else if(strcmp("gxlegend", str) == 0)
                A2mReadParLine(af, &(ag->ag_legends[0]));
            else if(strcmp("gylegend", str) == 0)
                A2mReadParLine(af, &(ag->ag_legends[1]));
            else if(strcmp("gzlegend", str) == 0)
                A2mReadParLine(af, &(ag->ag_legends[2]));

            else if(strcmp("gxmin", str) == 0)
                ag->ag_mins[0] = A2mRead1Dbl(af);
            else if(strcmp("gxmax", str) == 0)
                ag->ag_maxs[0] = A2mRead1Dbl(af);
            else if(strcmp("gymin", str) == 0)
                ag->ag_mins[1] = A2mRead1Dbl(af);
            else if(strcmp("gymax", str) == 0)
                ag->ag_maxs[1] = A2mRead1Dbl(af);
            else if(strcmp("gzmin", str) == 0)
                ag->ag_mins[2] = A2mRead1Dbl(af);
            else if(strcmp("gzmax", str) == 0)
                ag->ag_maxs[2] = A2mRead1Dbl(af);

            else if(strcmp("ggrid", str) == 0)
                //A2mReadGrGrid(af, &(ag->ag_grid));
                A2mReadGrGrid(af, ag->ag_grid);

            else if(strcmp("gxgrids", str) == 0)
                A2mReadGrGrids(af, &(ag->ag_grids[0]));
            else if(strcmp("gygrids", str) == 0)
                A2mReadGrGrids(af, &(ag->ag_grids[1]));
            else if(strcmp("gzgrids", str) == 0)
                A2mReadGrGrids(af, &(ag->ag_grids[2]));

            else if(strcmp("gxlabs", str) == 0)
                A2mReadGrLabs(af, &(ag->ag_labs[0]));
            else if(strcmp("gylabs", str) == 0)
                A2mReadGrLabs(af, &(ag->ag_labs[1]));
            else if(strcmp("gzlabs", str) == 0)
                A2mReadGrLabs(af, &(ag->ag_labs[2]));

            else if(strcmp("gxy", str) == 0)
                A2mReadGrXY(af, &(ag->ag_gxy), 0);
            else if(strcmp("gxz", str) == 0)
                A2mReadGrXY(af, &(ag->ag_gxy), 1);

            else if(strcmp("gty", str) == 0)
                A2mReadGrTY(af, &(ag->ag_gxy), 0);
            else if(strcmp("gtz", str) == 0)
                A2mReadGrTY(af, &(ag->ag_gxy), 1);

            else if(strcmp("gbox", str) == 0)
                A2mReadGrBox(af, ag);

            else if(strcmp("ge", str) == 0) {
                CppSkipToEol(af->af_cpp);
                goto fin;
            }
            else {
                CppSkipToEol(af->af_cpp);
                A2mPrintError(af, "Syntax error");
            }
        }
        else {
            CppSkipToEol(af->af_cpp);
            A2mPrintError(af, "Syntax error");
        }
    }
fin:
    A2mSetParTag(ag->ag_title, "GraphTitle");
    A2mSetParTag(ag->ag_footnote, "GraphFootnote");
    A2mSetParTag(ag->ag_header, "GraphHeader");
    A2mSetParTag(ag->ag_footer, "GraphFooter");
    for(i = 0 ; i < 3 ; i++) {
        A2mSetParTag(ag->ag_legends[i], "GraphLegend");
        d = ag->ag_mins[i];
        if(d > ag->ag_maxs[i]) {
            ag->ag_mins[i] = ag->ag_maxs[i];
            ag->ag_maxs[i] = d;
        }
    }

    ag->ag_newpage = A2M_GPAGE;
    return(ao);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mReadCmd(
    A2MFILE *af
)
#else
A2MOBJ *A2mReadCmd(af)
A2MFILE *af;
#endif
{
    char    str[51];
    A2MOBJ  *ao = 0;

    CppReadWord(af->af_cpp, str, 50);

    SCR_lower(str);
    if(A2mReadCmdNoObj(af, str)) return(ao);
    else if(strcmp(str, "page")  == 0) {
        return(A2mReadPage(af));
    }
    else if(strcmp(str, "line")  == 0) {
        return(A2mReadLine(af, 0));
    }
    else if(strcmp(str, "line1") == 0) {
        return(A2mReadLine(af, 1));
    }
    else if(strcmp(str, "line2") == 0) {
        return(A2mReadLine(af, 2));
    }
    else if(strcmp(str, "tb")    == 0) {
        return(A2mReadTbl(af));
    }
    else if(strcmp(str, "gb")    == 0) {
        return(A2mReadGraph(af));
    }
    else if(strcmp(str, "topic") == 0) {
        return(A2mReadTopic(af));
    }
    else if(strcmp(str, "topicalias") == 0) {
        return(A2mReadTopicAlias(af));
    }
    else if(strcmp(str, "goto") == 0) {
        return(A2mReadGoto(af));
    }
    else if(strcmp(str, "margins") == 0) {
        return(A2mReadMargins(af));
    }
    else {
        A2mPrintError(af, "Unrecognized command");
        return(ao);
    }
}

/*NH*/
int A2mReadCmdNoObj(af, str)
A2MFILE *af;
char    str[51];
{
    /* Lignes ..... ===> comment */ /* JMP 21-05-99 */
    if(str[0] == 0) {               /* JMP 21-05-99 */
        CppSkipToEol(af->af_cpp);   /* JMP 21-05-99 */
        return(1);                  /* JMP 21-05-99 */
    }

    if(strcmp(str, "cmd") == 0)         {
        A2mDefCh(af, &A2M_CMDCH);
        CPP_DIRCH = A2M_CMDCH;
        return(1);
    }
    else if(strcmp(str, "esc")    == 0) {
        A2mDefCh(af, &A2M_ESCCH);
        return(1);
    }
    else if(strcmp(str, "def")    == 0) {
        A2mDefCh(af, &A2M_DEFCH);    /* JMP 08-05-97 */
        CPP_DEFCH = A2M_DEFCH;
        return(1);
    }
    else if(strcmp(str, "sep")    == 0) {
        A2mDefCh(af, &A2M_SEPCH);
        return(1);
    }
    else if(strcmp(str, "ignore") == 0) {
        A2mReadVar(af, &A2M_IGNORECHARS);
        return(1);
    }
    else if(strcmp(str, "par")    == 0) {
        A2mDefPar(af, A2M_CURTAG);
        return(1);
    }
    else if(strcmp(str, "font")   == 0) {
        A2mDefPar(af, A2M_CURCTAG);
        return(1);
    }
    else if(strcmp(str, "par1")   == 0) {
        A2mDefPar(af, A2M_CURTAG1);
        return(1);
    }
    else if(strcmp(str, "lf_on")  == 0) {
        A2M_LFON = 1;
        return(1);
    }
    else if(strcmp(str, "lf_off") == 0) {
        A2M_LFON = 0;
        return(1);
    }
    else if(strcmp(str, "bl_on")  == 0) {
        A2M_BLON = 1;
        return(1);
    }
    else if(strcmp(str, "bl_off") == 0) {
        A2M_BLON = 0;
        return(1);
    }
    else if(strcmp(str, "pghead") == 0) {
        A2mReadVar(af, &A2M_PGHEAD);
        return(1);
    }
    else if(strcmp(str, "pgfoot") == 0) {
        A2mReadVar(af, &A2M_PGFOOT);
        return(1);
    }
    else if(strcmp(str, "tshading") == 0) {
        A2mReadTShading(af);
        return(1);
    }
    else if(strcmp(str, "tbreak") == 0) {
        A2M_TBREAK = A2mReadInt(af);
        return(1);
    }
    else if(strcmp(str, "tpage") == 0)  {
        A2M_TPAGE = A2mReadInt(af);
        return(1);
    }
    else if(strcmp(str, "twidth") == 0) {
        A2mReadTblWidths(af);
        return(1);
    }
    else if(strcmp(str, "gpage") == 0)  {
        A2M_GPAGE = A2mReadInt(af);
        return(1);
    }
    else if(strcmp(str, "pardef") == 0)  {
        A2mReadParDef(af);
        return(1);
    }
    else if(strcmp(str, "topiclink") == 0)  {
        A2mReadTopicLink(af, (A2MTOP *)0);
        return(1);
    }
    else if(strcmp(str, "topicslink") == 0) {
        A2mReadTopicsLink(af);
        return(1);
    }
    else if(strcmp(str, "bminidoc")    == 0) {
        A2mReadAnchor(af, 0);
        return(1);
    }
    else if(strcmp(str, "imgpath")    == 0) {
        A2mReadVar(af, &A2M_IMGPATH);    /* JMP 24-09-10 */
        return(1);
    }
    else return(0);
}

/*NH*/
int A2mReadTShading(af)
A2MFILE *af;
{
    char    str[256];
    int     br, pct, col, i;

    for(i = 0 ; i < 2 ; i++) {
        CppSkipSpaces(af->af_cpp);
        CppReadWord(af->af_cpp, str, 255);
        col = U_pos(str[0], "Bwrgbcmy");
        if(col < 0) {
            A2mPrintError(af, "Undefined Color");
            col = 0;
        }

        pct = CppReadLong(af->af_cpp);
        if(pct >= 50)       br = 8;
        else if(pct >= 25)  br = 7;
        else if(pct >= 12)  br = 6;
        else if(pct >= 6)   br = 5;
        else if(pct >= 3)   br = 4;
        else if(pct >  0)   br = 3;
        else                br = 0;
        A2M_TSHADING_COL[i] = col;
        A2M_TSHADING_BRUSH[i] = br;
    }

    CppSkipToEol(af->af_cpp);
    return(0);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MOBJ *A2mRead(
    A2MFILE *af
)
#else
A2MOBJ *A2mRead(af)
A2MFILE *af;
#endif
{
    int     ch;
    A2MOBJ  *ao = 0;

    while(1) {
        ch = CppGetc(af->af_cpp);
        if(ch == EOF) return(ao);
        if(ch == A2M_CMDCH) {
            ao = A2mReadCmd(af);
            if(ao) return(ao);
        }
        else if(ch != '\n') {
            CppUngetc(af->af_cpp, ch);
            ao = A2mReadPar(af);
            if(ao) return(ao);
        }
    }
}

/*NH*/
int A2mAddParStrAlloc(as, nl)
A2MSTR  ***as;
int     *nl;
{
    *as = (A2MSTR **) SCR_realloc(*as, sizeof(A2MSTR *), *nl, 1 + *nl);
    (*as)[*nl] = (A2MSTR *) SCR_malloc(sizeof(A2MSTR));
    (*nl)++;
    return(0);
}

/*NH*/
int A2mAddParStr(as, nl, as_proto)
A2MSTR  ***as, *as_proto;
int     *nl;
{
    *as = (A2MSTR **) SCR_realloc(*as, sizeof(A2MSTR *), *nl, 1 + *nl);
    (*as)[*nl] = 0;
    if(as_proto) {
        (*as)[*nl] = (A2MSTR *) SCR_malloc(sizeof(A2MSTR));
        memcpy((*as)[*nl], as_proto, sizeof(A2MSTR));
        (*as)[*nl]->as_type = 0;
    }
    (*nl)++;
    return(0);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MSTR **A2mAddParLine(
    A2MSTR  **as,
    U_ch    *txt
)
#else
A2MSTR **A2mAddParLine(as, txt)
A2MSTR  **as;
U_ch    *txt;
#endif
{
    A2MSTR  as_proto;
    int     i, pos, nl = 0, len, chattr = 0, ch;
    char    filename[256], tmp[512];
    U_ch    *A2mHHTopic(U_ch *); /* JMP 18-05-99 */
    U_ch    *txt1 = 0;

    memset(&as_proto, 0, sizeof(A2MSTR));
    if(as) {
        nl = SCR_tbl_size((U_ch**)as);
        if(nl) {
            memcpy(&as_proto, as[nl - 1], sizeof(A2MSTR));
            if(as_proto.as_type == A2M_SEP) {
                nl--;
                SCR_free(as[nl]);
            }
        }
    }

    else {
        memset(&as_proto, 0, sizeof(A2MSTR));
    }

    if(txt == 0) { /* Hardreturn */
        A2mAddParStr(&as, &nl, &as_proto);
        as[nl - 1]->as_type = A2M_NEWLINE;
        as[nl - 1]->as_txt = 0;
        A2mAddParStr(&as, &nl, 0L);
        return(as);
    }

    txt1 = txt = A2mHHTopic(txt); /* JMP 18-05-99 */
    while(1) {
        pos = U_pos(A2M_ESCCH, txt);
        if(pos < 0) pos = (int)strlen(txt);
        if(pos > 0) {
            A2mAddParStr(&as, &nl, &as_proto);
            as[nl - 1]->as_txt = SCR_malloc(pos + 1);
            memcpy(as[nl - 1]->as_txt, txt, pos);
            chattr = 0;
        }

        if(txt[pos] == 0) {
            if(chattr) {
                A2mAddParStr(&as, &nl, &as_proto);
                as[nl - 1]->as_type = A2M_SEP;
                as[nl - 1]->as_txt = 0;
            }
            break;
        }

        chattr = 1;
        switch(txt[pos + 1]) {
        case 'b' :
            as_proto.as_fnt.af_bold        = 1;
            break;
        case 'B' :
            as_proto.as_fnt.af_bold        = 0;
            break;
        case 'c' :
            as_proto.as_fnt.af_family      = 'C';
            break;
        case 'C' :
            as_proto.as_fnt.af_family      = 0;
            break;    /* JMP 26-08-99 */
        case 'g' :
            as_proto.as_fnt.af_family      = 'G';
            break;  /* JMP 26-08-99 */
        case 'G' :
            as_proto.as_fnt.af_family      = 0;
            break;
        case 'i' :
            as_proto.as_fnt.af_italic      = 1;
            break;
        case 'I' :
            as_proto.as_fnt.af_italic      = 0;
            break;
        case 'u' :
            as_proto.as_fnt.af_underline   = 1;
            break;
        case 'U' :
            as_proto.as_fnt.af_underline   = 0;
            break;
        case 'o' :
            as_proto.as_fnt.af_strike      = 1;
            break;
        case 'O' :
            as_proto.as_fnt.af_strike      = 0;
            break;

        case 'k' :
            pos++;
            i = U_pos(txt[pos + 1], "Bwrgbcmy");
            if(i < 0) break;
            as_proto.as_fnt.af_color = i;
            break;

        case 'e' :
            as_proto.as_fnt.af_bold        = 0;
            as_proto.as_fnt.af_italic      = 0;
            as_proto.as_fnt.af_underline   = 0;
            as_proto.as_fnt.af_strike      = 0;
            break;

        case 's' :
            as_proto.as_fnt.af_asize       -= 2;
            break;
        case 'S' :
            as_proto.as_fnt.af_asize       += 2;
            break;

        case '-' :
            as_proto.as_fnt.af_pos         = -1;
            as_proto.as_fnt.af_asize       -= 2;
            break;
        case '+' :
            as_proto.as_fnt.af_pos         = 1;
            as_proto.as_fnt.af_asize       -= 2;
            break;
        case '=' :
            as_proto.as_fnt.af_pos         = 0;
            as_proto.as_fnt.af_asize       = 0;
            break;

        case 'a' :
        case 'A' :
            ch = txt[pos + 2];                   /* JMP 03-09-99 */
            //if(!U_is_in(ch, "<'\"")) break;    /* JMP 03-09-99 *
            if(!U_is_in(ch, "<'\"|")) break;     /* JMP 18-05-11 */
            if(ch == '<') ch = '>';
            len = U_pos(ch, txt + pos + 3);
            if(len < 0) break;
            memcpy(tmp, txt + pos + 3, len);
            tmp[len] = 0;

            // Add image path à filename si défini                  /* JMP 24-09-10 */
            if(A2M_IMGPATH && A2M_IMGPATH[0]) {                     /* JMP 24-09-10 */
                if(A2M_IMGPATH[strlen(A2M_IMGPATH) - 1] != '\\')    /* JMP 24-09-10 */
                    strcat(A2M_IMGPATH, "\\");                      /* JMP 24-09-10 */
                sprintf(filename, "%s%s", A2M_IMGPATH, tmp);        /* JMP 24-09-10 */
            }                                                   /* JMP 24-09-10 */
            else                                                    /* JMP 24-09-10 */
                strcpy(filename, tmp);                              /* JMP 24-09-10 */

            A2mAddParStr(&as, &nl, &as_proto);

            switch(ch) {
            case '"' :
                if(txt[pos + 1] == 'a')
                    as[nl - 1]->as_type = A2M_IMAGEF;  /* JMP 14-12-97 */
                else
                    as[nl - 1]->as_type = A2M_IMAGEFR; /* JMP 14-12-97 */
                break;
            case '\'' :
                if(txt[pos + 1] == 'a')
                    as[nl - 1]->as_type = A2M_IMAGE;   /* JMP 14-12-97 */
                else
                    as[nl - 1]->as_type = A2M_IMAGER;  /* JMP 14-12-97 */
                break;
            case '|' :                                 /* JMP 18-05-11 */
            case '>' :
                as[nl - 1]->as_type = A2M_ANCHOR;      /* JMP 03-09-99 */
                break;
            }

#ifdef DOS
            SCR_replace(filename, "/", "\\");
#else
            SCR_replace(filename, "\\", "/");
#endif

            as[nl - 1]->as_txt = SCR_stracpy(filename);
            pos += len + 2;
            break;

        case 'f' :
        case 'F' :
            ch = txt[pos + 2];                  /* JMP 03-09-99 */
            if(!U_is_in(ch, "<'\"")) break;     /* JMP 03-09-99 */
            if(ch == '<') ch = '>';
            len = U_pos(ch, txt + pos + 3);
            if(len < 0) break;
            memcpy(filename, txt + pos + 3, len);
            filename[len] = 0;
            A2mAddParStr(&as, &nl, &as_proto);
            as[nl - 1]->as_type = A2M_FNOTE;      /* JMP 03-09-99 */
            as[nl - 1]->as_txt = SCR_stracpy(filename);
            pos += len + 2;
            break;

        case 'h' :
            if(txt[pos + 2] != '<') break;
            len = U_pos('>', txt + pos + 3);
            if(len < 0) break;
            memcpy(filename, txt + pos + 3, len);
            filename[len] = 0;
            A2mAddParStr(&as, &nl, &as_proto);
            as[nl - 1]->as_type = A2M_TOPIC;
            as[nl - 1]->as_txt = SCR_stracpy(filename);
            pos += len + 2;
            break;

        case 'H' :
            A2mAddParStr(&as, &nl, &as_proto);
            as[nl - 1]->as_type = A2M_ETOPIC;
            as[nl - 1]->as_txt = 0;
            break;

        case 'l' :                                     /* JMP 29-04-99 */
            if(!U_is_in(txt[pos + 2], "'\"")) break;
            len = U_pos(txt[pos + 2], txt + pos + 3);
            if(len < 0) break;
            memcpy(filename, txt + pos + 3, len);
            filename[len] = 0;
            A2mAddParStr(&as, &nl, &as_proto);
            as[nl - 1]->as_type = A2M_BHREF;
            as[nl - 1]->as_txt = SCR_stracpy(filename);
            pos += len + 2;
            break;

        case 'L' : /* JMP 29-04-99 */
            A2mAddParStr(&as, &nl, &as_proto);
            as[nl - 1]->as_type = A2M_EHREF;
            as[nl - 1]->as_txt = 0;
            break;

        case 'p' :
            A2mAddParStr(&as, &nl, &as_proto);
            as[nl - 1]->as_type = A2M_NEWLINE;
            as[nl - 1]->as_txt = 0;
            break;

        case 't' :
            A2mAddParStr(&as, &nl, &as_proto);
            as[nl - 1]->as_type = A2M_TAB;
            as[nl - 1]->as_txt = 0;
            break;

        case 0   :
            pos--;
            break;

        default  :
            if(txt[pos + 1] == A2M_ESCCH) {
                A2mAddParStr(&as, &nl, &as_proto);
                as[nl - 1]->as_txt = SCR_malloc(2);
                as[nl - 1]->as_txt[0] = A2M_ESCCH;
                break;
            }
            pos--;
            break;
        }

        txt += pos + 2;
    }

    SCR_free(txt1); /* JMP 18-05-99 */
    A2mAddParStr(&as, &nl, 0L);
    return(as);
}

/* =====================================================================
Cette fonction construit dans un buffer statique un message indiquant le
fichier et la ligne et la colonne dans ce fichier où en est arrivée la
lecture du fichier a2m ouvert.

&EN af  = descripteur du fichier a2m ouvert

&EX
    #include <scr4.h>
    #include <s_a2m.h>

    A2mPrintError(af, txt)
    A2MFILE *af;
    U_ch    *txt;
    {
	PG_display_error("%s : %s", A2mError(af), txt);
    }
&TX
&SA A2mPrintError(), A2mToGdi(), A2mToHtml(), A2mToRtf(), A2mToMif()
======================================================================== */

U_ch *A2mError(af)
A2MFILE *af;
{
    static char msg[128];

    sprintf(msg, "%s[%d;%d]", af->af_cpp->filename, af->af_cpp->cline + 1, /* JMP 11-09-99 */
            af->af_cpp->ccol + 1); /* JMP 11-09-99 */
    return(msg);
}

/*NH*/
/***************** Catalogue de paragraphes *********************/

/*NH*/
A2mFindPpr(partag)
char    *partag;
{
    int     i;

    for(i = 0 ; i < A2M_NB_PPRS ; i++)
        if(strcmp(A2M_PPRS[i].pp_tag, partag) == 0)
            return(i);

    return(-1);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MPPR *A2mCreatePpr(
    char    *partag
)
#else
A2MPPR *A2mCreatePpr(partag)
char    *partag;
#endif
{
    int     i = A2mFindPpr(partag);

    if(i >= 0) return(A2M_PPRS + i);

    A2M_PPRS = (A2MPPR *) SCR_realloc(A2M_PPRS, sizeof(A2MPPR), A2M_NB_PPRS, A2M_NB_PPRS + 1);
    SCR_strlcpy(A2M_PPRS[A2M_NB_PPRS].pp_tag, partag, 31);
    A2M_PPRS[A2M_NB_PPRS].pp_spaceb = 4;
    A2M_PPRS[A2M_NB_PPRS].pp_spacea = 2;
    A2M_PPRS[A2M_NB_PPRS].pp_fnt.af_family = 'A';
    A2M_PPRS[A2M_NB_PPRS].pp_fnt.af_size = 10;

    A2M_NB_PPRS++;
    return(A2M_PPRS + A2M_NB_PPRS - 1);
}

/*NH*/
int A2mReadParString(af, str)
A2MFILE *af;
U_ch    *str;
{
    int     ch;

    CppSkipSpaces(af->af_cpp);
    ch = CppGetc(af->af_cpp);
    if(ch != '=') {
        CppUngetc(af->af_cpp, ch);
        A2mPrintError(af, "Syntax error");
        return(-1);
    }
    CppSkipSpaces(af->af_cpp);
    CppReadWord(af->af_cpp, str, 64);
    return(0);
}

/*NH*/
int A2mReadParVal(af)
A2MFILE *af;
{
    char    str[65];

    if(A2mReadParString(af, str) < 0) return(0);
    return(atoi(str));
}

/*NH*/
int A2mReadParChars(af, chars, upper)
A2MFILE *af;
char    *chars;
int     upper;
{
    char    str[65];
    int     pos;

    if(A2mReadParString(af, str) < 0) return(0);
    if(upper) SCR_upper(str);
    pos = U_pos(str[0], chars);
    if(pos < 0) pos = 0;
    return(pos);
}

/*NH*/
int A2mReadParChar(af)
A2MFILE *af;
{
    char    str[65];

    if(A2mReadParString(af, str) < 0) return(0);
    return(SCR_upper_char(str[0]));
}


/*NH*/
int A2mReadParAlias(af, pp)
A2MFILE *af;
A2MPPR  *pp;
{
    char    str[65], partag[65];
    int     pos;

    if(A2mReadParString(af, str) < 0) return(0);
    pos = A2mFindPpr(str);
    if(pos < 0) return(0);
    strcpy(partag, pp->pp_tag);
    memcpy(pp, A2M_PPRS + pos, sizeof(A2MPPR));
    SCR_strlcpy(pp->pp_tag, partag, 31);
    return(0);
}

/*NH*/
int A2mReadParDef(af)
A2MFILE *af;
{
    char    buf[65];
    A2MPPR  *pp;
    int     ch;

    CppSkipSpaces(af->af_cpp);
    CppReadWord(af->af_cpp, buf, 64);
    if(buf[0] == 0) return(0);
    pp = A2mCreatePpr(buf);

    while(1) {
        CppSkipSpaces(af->af_cpp);
        ch = CppGetc(af->af_cpp);
        if(ch == '\n' || ch == EOF) return(0);
        CppUngetc(af->af_cpp, ch);
        CppReadWord(af->af_cpp, buf, 64);
        SCR_lower(buf);
        if(strcmp(buf, "lmargin") == 0)      pp->pp_lmarg  =  A2mReadParVal(af);
        else if(strcmp(buf, "spacea") == 0)  pp->pp_spacea =  A2mReadParVal(af);
        else if(strcmp(buf, "spaceb") == 0)  pp->pp_spaceb =  A2mReadParVal(af);
        else if(strcmp(buf, "number") == 0)  pp->pp_number =  A2mReadParVal(af);
        else if(strcmp(buf, "html") == 0)    pp->pp_html   =  A2mReadParVal(af);
        else if(strcmp(buf, "justify") == 0) pp->pp_just   =  A2mReadParChars(af, "LCRDJ", 1);
        else if(strcmp(buf, "keepwn") == 0)  pp->pp_keepwn =  A2mReadParChars(af, "NY", 1);
        else if(strcmp(buf, "newpage") == 0) pp->pp_newpage=  A2mReadParChars(af, "NY", 1);
        else if(strcmp(buf, "bullet") == 0)  pp->pp_bullet =  A2mReadParChars(af, "NY", 1);
        else if(strcmp(buf, "color") == 0)   pp->pp_fnt.af_color = A2mReadParChars(af, "Bwrgbcmy", 0);
        else if(strcmp(buf, "family") == 0)  pp->pp_fnt.af_family =  A2mReadParChar(af);
        else if(strcmp(buf, "size") == 0)    pp->pp_fnt.af_size =  A2mReadParVal(af);
        else if(strcmp(buf, "italic") == 0)  pp->pp_fnt.af_italic =  A2mReadParChars(af, "NY", 1);
        else if(strcmp(buf, "bold") == 0)    pp->pp_fnt.af_bold =  A2mReadParChars(af, "NY", 1);
        else if(strcmp(buf, "nowrap") == 0)  pp->pp_nowrap =  A2mReadParChars(af, "NY", 1); /* JMP 24-01-2004 */
        else if(strcmp(buf, "underline") == 0)pp->pp_fnt.af_underline =  A2mReadParChars(af, "NY", 1);
        else if(strcmp(buf, "copy") == 0)    A2mReadParAlias(af, pp);
        else {
            A2mPrintError(af, "Syntax error");
            CppSkipToEol(af->af_cpp);
            return(0);
        }
    }
}

/*NH*/
#ifdef __cplusplus
/*NH*/
A2MPAR *A2mTxt2Par(
    char    *tag,
    char    *txt
)
#else
A2MPAR *A2mTxt2Par(tag, txt)
char    *tag, *txt;
#endif
{
    A2MPAR *par;

    par = (A2MPAR *) A2mNewPar(tag);
    par->ap_strs = A2mAddParLine(0L, txt);
    return(par);
}


/*NH*/
int A2mReadA2mIni(filename)
char    *filename;
{
    U_ch    buf[256];

    if(IniReadTxtParm(filename, "A2M", "ESC", buf, 255) == 0)
        if(buf[0]) A2M_ESCCH = buf[0];
    if(IniReadTxtParm(filename, "A2M", "CMD", buf, 255) == 0)
        if(buf[0]) A2M_CMDCH = buf[0];
    if(IniReadTxtParm(filename, "A2M", "DEFCH", buf, 255) == 0)
        if(buf[0]) A2M_DEFCH = buf[0];
    if(IniReadTxtParm(filename, "A2M", "SEP", buf, 255) == 0)
        if(buf[0]) A2M_SEPCH = buf[0];
    if(IniReadTxtParm(filename, "A2M", "PAR", buf, 40) == 0)
        strcpy(A2M_CURTAG, buf);

    IniReadNumParm(filename, "A2M", "LF_ON",       &A2M_LFON);
    IniReadNumParm(filename, "A2M", "BL_ON",       &A2M_BLON);
    IniReadNumParm(filename, "A2M", "TBREAK",      &A2M_TBREAK);
    IniReadNumParm(filename, "A2M", "TPAGE",       &A2M_TPAGE);
    IniReadNumParm(filename, "A2M", "GPAGE",       &A2M_GPAGE);
    return(0);
}


char    A2M_PER_CH[] = "YSQMWD";
int     A2M_PER_NB[] = {1, 2, 4, 12, 52, 365};

/*NH*/
int A2mAnalysePeriod(str, period, skip)
char    *str;
double  *period, *skip;
{
    int     i, j, nb_per, year, sub;

    *period = *skip = 0.0;
    for(i = 0; i < 4; i++)
        if(!SCR_is_num(str[i])) break;
    if(i == 0) goto fin;

    nb_per = U_pos(toupper(str[i]), A2M_PER_CH);
    if(nb_per < 0) goto fin;

    str[i] = 0;
    year = atoi(str);
    str[i] = A2M_PER_CH[nb_per];

    i++;
    j = i;
    while(SCR_is_num(str[i])) i++;
    str[i] = 0;
    sub = atoi(str + j);
    if(A2M_PER_NB[nb_per] < sub) goto fin;

    *skip = 1.0/A2M_PER_NB[nb_per];
    *period = (double) year + *skip * (sub - 1);

fin:
    return(0);
}

/*NH*/
/*------------- READ TOPICS --------------------*/
int A2mTopicSort(const void* c1, const void* c2)
{
    A2MTOP *t1 = (A2MTOP *)c1;
    A2MTOP *t2 = (A2MTOP *)c2;

    if(t1->atp_lg < t2->atp_lg) return(1);
    return(-1);
}


/*NH*/
int A2mAdd1Topic(A2MOBJ *ao)
{
    A2MTOP  *atp;

    atp = (A2MTOP *)ao->ao_ptr;
    if(A2M_NB_TOPICS % 100 == 0)
        A2M_TOPICS = (A2MTOP **) SCR_realloc(A2M_TOPICS,
                                             sizeof(A2MTOP *), A2M_NB_TOPICS, A2M_NB_TOPICS + 100);
    A2M_TOPICS[A2M_NB_TOPICS] = atp;
    A2M_NB_TOPICS++;
    ao->ao_ptr = 0;
    return(0);
}



/*NH*/
int A2mReadAllTopics(A2MFILE* af)
{
    int     ch;
    A2MOBJ  *ao;
    A2MTOP  *atp;
    char    str[51];

    while(1) {
        ch = CppGetc(af->af_cpp);
        if(ch == EOF) break;
        else if(ch == '\n') continue;
        else if(ch == A2M_CMDCH) {
            CppReadWord(af->af_cpp, str, 50);
            SCR_lower(str);
            if(strcmp(str, "cmd") == 0) {
                A2mDefCh(af, &A2M_CMDCH);
                CPP_DIRCH = A2M_CMDCH;
            }
            else if(strcmp(str, "def") == 0) {
                A2mDefCh(af, &A2M_DEFCH);
                CPP_DEFCH = A2M_DEFCH;
            }
            else if(strcmp(str, "topiclink") == 0) {
                atp = 0;
                if(A2M_NB_TOPICS) atp = A2M_TOPICS[A2M_NB_TOPICS - 1];
                A2mReadTopicLink(af, atp);
            }
            else if(strcmp(str, "topicslink") == 0) {
                A2mReadTopicsLink(af);
            }
            else if(strcmp(str, "topic") == 0) {
                ao = A2mReadTopic(af);
                A2mAdd1Topic(ao);
                A2mFreeObj(ao);
            }
            else if(strcmp(str, "topicalias") == 0) {
                ao = A2mReadTopicAlias(af);
                A2mAdd1Topic(ao);
                A2mFreeObj(ao);
            }
            else if(strcmp(str, "bminidoc") == 0) {
                A2mReadAnchor(af, 1);
            }
            else
                CppSkipToEol(af->af_cpp);
        }
        else
            CppSkipToEol(af->af_cpp);
    }

    return(0);
}

/*NH*/
A2mReadTopics(a2mfile)
char    *a2mfile;
{
    A2MFILE *af;
    CPPFILE *cpp;
    int     oldcmdch = A2M_CMDCH;
    int     olddefch = A2M_DEFCH;
    int     tcase    = A2M_TOPS_CASE;
    int     tauto    = A2M_TOPS_AUTO;
    int     ttitle   = A2M_TOPS_TITLE;
    int     tpartial = A2M_TOPS_PARTIAL;

    CPP_DIRCH = A2M_CMDCH;
    CPP_DEFCH = A2M_DEFCH;

    cpp = CppOpen(a2mfile, 0);
    if(cpp == 0) return(-1);
    af = (A2MFILE *) SCR_malloc(sizeof(A2MFILE));
    af->af_cpp = cpp;

    A2mFreeTopics();
    A2mReadAllTopics(af);
    qsort(A2M_TOPICS, A2M_NB_TOPICS, sizeof(A2MTOP *), A2mTopicSort);

    CppClose(af->af_cpp);
    SCR_free(af->af_outfile);
    SCR_free(af);

    A2M_CMDCH = oldcmdch;
    A2M_DEFCH = olddefch;
    A2M_TOPS_CASE = tcase;
    A2M_TOPS_AUTO = tauto;
    A2M_TOPS_TITLE = ttitle;
    A2M_TOPS_PARTIAL = tpartial;

    return(0);
}
/*NH*/
int A2mFreeTopics()
{
    int     i;

    for(i = 0 ; i < A2M_NB_TOPICS ; i++)
        A2mFreeTopic(A2M_TOPICS[i]);

    SCR_free(A2M_TOPICS);
    A2M_TOPICS = 0;
    A2M_NB_TOPICS = 0;
    A2mFreeAnchors();
    return(0);
}

/*NH*/
U_ch *A2mHHTopic(line)
U_ch    *line;
{
    A2MTOP  *atp;
    int     i;
    U_ch    txt[2048], txtu[2048], rtxt[2048];

    if(strlen(line) > 2000 || A2M_HTML_AUTOLINK == 0) /* JMP 20-10-01 */
        return(SCR_stracpy(line));
    strcpy(txt, line);
    strcpy(txtu, txt);
    SCR_upper(txtu);
    for(i = 0 ; i < A2M_NB_TOPICS ; i++) {
        atp = A2M_TOPICS[i];
        if(atp->atp_nb == A2M_CURTOPIC) continue;
        if(atp->atp_linkauto == 0) continue;

//        if(A2mHHKeepKw(A2M_TOPICS[i]) == 0) continue;
        if(A2mHHReplace1(rtxt, txt, txtu, atp)) {
            strcpy(txt, rtxt);
            strcpy(txtu, txt);
            SCR_upper(txtu);
        }
    }
    return(SCR_stracpy(txt));
}


/*NH*/
A2mHHKeepKw(keyw)
U_ch    *keyw;
{
    int     i;

    if(keyw[0] == 0) return(0);
    for(i = 0 ; keyw[i] ; i++)
        if(keyw[i] != '_' && !SCR_is_anum(keyw[i])) return(0);
    return(1);
}

#define lettre(x)   (SCR_is_anum(x) || (x) == '_')
/*NH*/
A2mHHCalcPosH(txt, pos, posH)
U_ch    *txt;
int     pos, *posH;
{
    int     i;

    if(pos == 0) return(-1);
    for(i = pos - 1 ; i >= 0 ; i--) {
        if(txt[i] != A2M_ESCCH) continue;
        if(U_is_in(txt[i + 1], "LH")) return(-1);
        if(!U_is_in(txt[i + 1], "hl")) continue;
        for(i = i + 2 ; txt[i] ; i++) {
            if(txt[i] != A2M_ESCCH || !U_is_in(txt[i + 1], "LH")) continue;
            *posH = i + 2;
            return(0);
        }
    }
    return(-1);
}

/*NH*/
int A2mHHCalcPosA(txt, shift, pos, posH)
U_ch    *txt;
int     shift, pos, *posH;
{
    int     i;

    if(shift + pos == 0) return(-1);
    for(i = shift + pos - 1 ; i >= shift ; i--) {
        if(txt[i] != A2M_ESCCH) continue;
        if(!U_is_in(txt[i + 1], "aA")) continue;
        for(i = i + 3 ; txt[i] ; i++) {
            if(!U_is_in(txt[i], "'\"")) continue;
            *posH = i + 1;
            return(0);
        }
    }
    return(-1);
}
/*NH*/
int A2mHHReplace1(U_ch* res, U_ch* txt, U_ch* txtu, A2MTOP* atp)
{
    U_ch    *keyw = atp->atp_keyws, *line = txt;
    int     ch, pos, shift = 0, posH, rc = 0;
    int     lgkw = (int)strlen(keyw);
    U_ch    esch[3], escH[3], buf[128];

    strcpy(esch, " h");
    strcpy(escH, " H");
    escH[0] = esch[0] = A2M_ESCCH;

    if(atp->atp_linkcase == 0) {
        line = txtu;
        keyw = atp->atp_keywsu;
    }

    res[0] = 0;
    while(1) {
        pos = U_index(line + shift, keyw);
        if(pos < 0) {
            strcat(res, txt + shift);
            return(rc);
        }

        /* Check partiel */
        if(atp->atp_linkpart == 0) {
            if(((shift + pos > 0 && lettre(txt[shift + pos - 1])) &&
                    !(shift + pos > 1 && txt[shift + pos - 2] == A2M_ESCCH)) ||
                    (lettre(txt[shift + pos + lgkw]))) {
                ch = txt[shift + pos + lgkw];
                txt[shift + pos + lgkw] = 0;
                strcat(res, txt + shift);
                txt[shift + pos + lgkw] = ch;
                shift += pos + lgkw;
                rc = 1;
                continue;
            }
        }

        if(A2mHHCalcPosH(txt, shift + pos, &posH) == 0) {
            ch = txt[posH];
            txt[posH] = 0;
            strcat(res, txt + shift);
            txt[posH] = ch;
            shift = posH;
            rc = 1;
            continue;
        }

        if(A2mHHCalcPosA(txt, shift, pos, &posH) == 0) {
            ch = txt[posH];
            txt[posH] = 0;
            strcat(res, txt + shift);
            txt[posH] = ch;
            shift = posH;
            rc = 1;
            continue;
        }

        ch = txt[shift + pos];
        txt[shift + pos] = 0;
        strcat(res, txt + shift);
        txt[shift + pos] = ch;
        ch = txt[shift + pos + lgkw];
        txt[shift + pos + lgkw] = 0;
        strcat(res, esch);
        sprintf(buf, "<%d>", atp->atp_nb);
        strcat(res, buf);
        strcat(res, txt + shift + pos);
        strcat(res, escH);
        txt[shift + pos + lgkw] = ch;
        shift += pos + lgkw;
        rc = 1;
    }
}


/****************** ANCHORS *****************************/

int         A2M_NB_ANCHORS = 0, A2M_IN_ANCHOR = 0;
A2MANCHOR   **A2M_ANCHORS = 0;

/*NH*/
int A2mReadAnchor(A2MFILE *af, int save)
{
    A2MANCHOR   *aa;
    int         ch, nl = 0;
    U_ch        str[128];
    A2MOBJ      *ao;

    if(save) {
        if(A2M_NB_ANCHORS % 100 == 0)
            A2M_ANCHORS = (A2MANCHOR **) SCR_realloc(A2M_ANCHORS,
                          sizeof(A2MANCHOR *), A2M_NB_ANCHORS, A2M_NB_ANCHORS + 100);
        aa = A2M_ANCHORS[A2M_NB_ANCHORS] = (A2MANCHOR *) SCR_malloc(sizeof(A2MANCHOR));
        A2M_NB_ANCHORS++;
    }

    CppSkipSpaces(af->af_cpp);
    CppReadWord(af->af_cpp, str, 19);
    if(save) strcpy(aa->aa_name, str);
    CppSkipToEol(af->af_cpp);
    A2M_IN_ANCHOR++;
    while(1) {
        if(save)
            aa->aa_aos = (A2MOBJ **) SCR_realloc(aa->aa_aos, sizeof(A2MOBJ),
                                                 aa->aa_nb_aos, aa->aa_nb_aos + 1);
        ao = A2mRead(af);
        if(save) aa->aa_aos[aa->aa_nb_aos++] = ao;
        else     A2mFreeObj(ao);

        while(1) {
            ch = CppGetc(af->af_cpp);
            if(!SCR_is_space(ch)) break; /* JMP 29-03-00 */
        }
        if(ch == EOF) break;
        if(ch == A2M_CMDCH) {
            CppReadWord(af->af_cpp, str, 120);
            if(strcmp(str, "eminidoc") != 0) {
                CppRecord(af->af_cpp, str);
                CppUngetc(af->af_cpp, A2M_CMDCH);
            }
            else {
                CppSkipToEol(af->af_cpp);
                break;
            }
        }

        if(ao == 0) break;
    }
    A2M_IN_ANCHOR--;
    return(0);
}

/*NH*/
int A2mFreeAnchors()
{
    int     i, j;

    for(i = 0 ; i < A2M_NB_ANCHORS ; i++)
        for(j = 0 ; j < A2M_ANCHORS[i]->aa_nb_aos ; j++)
            A2mFreeObj(A2M_ANCHORS[i]->aa_aos[j]);

    SCR_free(A2M_ANCHORS);
    A2M_ANCHORS = 0;
    A2M_NB_ANCHORS = 0;
    return(0);
}

/*NH*/
int A2mFindAnchor(char *ref)
{
    int     i;

    for(i = 0 ; i < A2M_NB_ANCHORS ; i++)
        if(strcmp(A2M_ANCHORS[i]->aa_name, ref) == 0) return(i);

    return(-1);
}


int     A2M_NB_FNOTES = 0;
char    **A2M_FNOTES = 0;

/*NH*/
int A2mFreeFNotes()
{
    if(A2M_NB_FNOTES == 0) return(0);

    SCR_add_ptr((U_ch***)&A2M_FNOTES, &A2M_NB_FNOTES, 0L);
    SCR_free_tbl((U_ch**)A2M_FNOTES);

    A2M_NB_FNOTES = 0;
    A2M_FNOTES = 0;
    return(0);
}

/*NH*/
int A2mAddFNote(char* ref)
{
    SCR_add_ptr((U_ch***)&A2M_FNOTES, &A2M_NB_FNOTES, ref);
    return(0);
}




