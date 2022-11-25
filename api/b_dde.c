#include "iode.h"


/**************** WINDOWS : DDE **********************/

#include "scr4w.h"

char    ExcelCellEn[3] = "RC", ExcelCellFr[3] = "LC", ExcelCellNl[3] = "RK";
char    ExcelSheetEn[15] = "Sheet1", ExcelSheetFr[15] = "Feuil1", ExcelSheetNl[15] = "Blad1";
char    *ExcelCellRef = ExcelCellEn, *ExcelSheetRef = ExcelSheetEn;

int IodeDdeLocale(char *buf)
{
    int     i;


    if(buf == 0 || SCR_sDECIMAL == '.') return(0);
    for(i = 0 ; buf[i] ; i++)
        if(buf[i] == '.' && SCR_is_num(buf[i + 1])) buf[i] = SCR_sDECIMAL;

    return(0);
}

int IodeDdeUnLocale(char *buf)
{
    int     i, j;

    if(buf == 0) return(0); /* JMP 29/02/2012 */

    // Skip Thousand separator(s)
    // JMP 27/1/2017 => retirer les thousands AVANT de remplacer les dec
    //                 (sinon les dec peuvent disparaitre si thousands = '.')
    /* JMP 29/02/2012 */
    for(i = j = 0 ; buf[i] ; i++) {
        if(buf[i] != SCR_sTHOUSAND || !SCR_is_num(buf[i + 1]))
            buf[j++] = buf[i];
    }
    buf[j] = 0;

    // Replaces sDecimal by '.' in buf
    for(i = 0 ; buf[i] ; i++)
        if(buf[i] == SCR_sDECIMAL && SCR_is_num(buf[i + 1])) buf[i] = '.';


    // Skip Currency sign
    /* JMP 29/02/2012 */
    for(i = j = 0 ; buf[i] ; i++) {
        if(buf[i] != SCR_sCURRENCY)
            buf[j++] = buf[i];
    }
    buf[j] = 0;

    return(0);
}

int IodeDdeType(char *szTopic)
{
    return(U_pos(SCR_upper_char(szTopic[0]), "CEILSTV"));
}

char *IodeDdeGetWS(char *szItem)
{
    KDB     *kdb;
    char    *res;
    int     i, type;

    SCR_upper(szItem);
    type = IodeDdeType(szItem);
    if(type < 0 || type > 6) return(SCR_stracpy("Error"));

    kdb = K_WS[type];
    if(strcmp(szItem, "SAMPLE") == 0) {
        res = SCR_malloc(40);
        PER_smpltoa(KSMPL(K_WS[K_VAR]), res);
        return(res);
    }
    else if(strcmp(szItem + 1, "LIST") == 0) {
        if(kdb == 0 || KNB(kdb) == 0) return((char *)0);
        res = SCR_malloc((sizeof(ONAME) + 1) * (1 + KNB(kdb))); /* IODE64K */
        for(i = 0 ; i < KNB(kdb) ; i++) {
            strcat(res, KONAME(kdb, i));
            strcat(res, "\t");
        }
        return(res);
    }
    else if(strcmp(szItem + 1, "NAME") == 0) {
        if(kdb == 0) return((char *)0);
        res = SCR_stracpy(KNAMEPTR(kdb)); /* JMP 03-06-2015 */
        return(res);
    }
    else if(strcmp(szItem + 1, "DESCR") == 0) {
        if(kdb == 0) return((char *)0);
        res = SCR_stracpy(KDESC(kdb));
        return(res);
    }
    else if(strcmp(szItem + 1, "NB") == 0) {
        if(kdb == 0) return((char *)0);
        res = SCR_malloc(20);
        sprintf(res, "%d", KNB(kdb));
        return(res);
    }
    return((char *)0);
}

char *IodeDdeCreateSeries(int objnb, int bt)
{
    char    *res, buf[128];
    KDB     *kdb = K_WS[K_VAR];
    int     t;
    double  x;

    res = SCR_malloc(40 * (1 + KSMPL(kdb)->s_nb - bt)); /* JMP 29-06-00 */
    strcpy(res, KONAME(kdb, objnb));
    strcat(res, "\t");
    for(t = bt ; t < KSMPL(kdb)->s_nb ; t++) {
        x = *(KVVAL(kdb, objnb, t));
        if(!L_ISAN(x)) strcpy(buf, "#N/A");
        // else           SCR_fmt_dbl(x, buf, 16, -1); /* JMP 01-02-99 */
        else           IodeFmtVal(buf, x);             /* JMP 01-02-99 */
        strcat(res, buf);
        strcat(res, "\t");
    }

    SCR_sqz(res);
    return(res);
}

char *IodeDdeCreatePer(int bt)
{
    char    *res;
    KDB     *kdb = K_WS[K_VAR];
    PERIOD  *per;
    int     t;

    res = SCR_malloc(11 * (1 + KSMPL(kdb)->s_nb - bt));
    for(t = bt ; t < KSMPL(kdb)->s_nb ; t++) {
        per = PER_addper(&(KSMPL(kdb)->s_p1), t);
        strcat(res, PER_pertoa(per, 0L));
        strcat(res, "\t");
    }

    SCR_sqz(res);
    return(res);
}

char    *ToBase26(int num)
{
    static  char    base26[10];
    int             i, pos;


    pos = 0;
    while(num > 25) {
        i = num %26;
        num = num/26;
        base26[pos] = (char)(64 + i);
        pos ++;
    }
    base26[pos] = (char)(64 + num);

    return(base26);
}

char *IodeDdeXlsCell(char *offset, int i, int j, int lg, int hg)
{
    int     line, col, pos;
    static char buf[40];

    if(offset == 0 || offset[0] == 0) {
        line = col = 1;
    }
    else {
        line = atoi(offset + 1);
        pos = U_pos('C', offset);
        if(pos > 0) col = atoi(offset + pos + 1);
        else        col = 1;
    }

    i+= line;
    j+= col;
    //sprintf(buf, "R%dC%d:R%dC%d", i, j, i + hg - 1, j + lg - 1);
    sprintf(buf, "%c%d%c%d:%c%d%c%d",
            ExcelCellRef[0], i, ExcelCellRef[1], j,
            ExcelCellRef[0], i + hg - 1, ExcelCellRef[1], j + lg - 1);
    //sprintf(buf, "%s%d:%s%d", ToBase26(i), j, ToBase26(i + hg - 1), j + lg - 1);
    return(buf);
}

extern  char **KT_names;
extern  int  KT_nbnames, KT_mode[MAX_MODE];
#define DDECELLSIZE 512
char    *IodeTblCell(TCELL *cell, COL *cl, int nbdec)
{
    static char    buf[DDECELLSIZE + 1]; /* JMP 4/4/2016 (PS) !!! */
    char    *ptr = NULL;
    char    *COL_text();

    buf[0] = 0;
    if(cell->tc_type != 0) {
        if(cl == NULL || cell->tc_type == KT_STRING) {
            ptr = (char *) COL_text(cl, cell->tc_val, KT_nbnames);
            SCR_strlcpy(buf, ptr, DDECELLSIZE);
            buf[DDECELLSIZE] = 0;
            SW_nfree(ptr);
        }
        else {
            if(!L_ISAN(cl->cl_res)) strcpy(buf, "#N/A");
            else {
                sprintf(buf, "%80.20lf", cl->cl_res); /* JMP 18-10-07 */
                SCR_sqz(buf);                         /* JMP 18-10-07 */
                SCR_pstrip(buf, "0");                 /* JMP 18-10-07 */
            }
            // T_fmt_val(buf, cl->cl_res, 20, nbdec); /* JMP 18-10-07 */
        }
    }

    return(buf);
}

char    *IodeDdeCreateTbl(int objnb, char *ismpl, int *nc, int *nl, int nbdec)
{
    int     dim, i, j, d, rc = 0, nli = 0,
                          nf = 0, nm = 0;
    char    gsmpl[128], **l = NULL, *buf, *res = NULL; /* JMP 30-04-98 */

    TBL     *tbl = KTVAL(K_WS[K_TBL], objnb);
    COLS    *cls;
    TLINE   *line;
    SAMPLE  *smpl = (SAMPLE *) KDATA(KV_WS);

    /* date */
    long    SCR_current_date();
    char    date[11];
    /* mode */
    extern char *KLG_OPERS_TEXTS[][3];

    if(ismpl == NULL) {
        sprintf(gsmpl, "%s:%d",
                PER_pertoa(&(smpl->s_p1), NULL), smpl->s_nb);
    }
    else
        sprintf(gsmpl, "%s", ismpl);

    dim = T_prep_cls(tbl, gsmpl, &cls);
    if(dim < 0) return(SCR_stracpy("Error in Tbl or Smpl"));

    KT_names = T_find_files(cls);
    KT_nbnames = SCR_tbl_size(KT_names);
    if(KT_nbnames == 0) return(SCR_stracpy("Error in Tbl or Smpl"));
    COL_find_mode(cls, KT_mode, 2);

    *nc = dim + 1;
    *nl = 1;

    buf = SCR_malloc(256 + dim * 128);
    for(i = 0; rc == 0 && i < T_NL(tbl); i++) {
        buf[0] = 0;
        line = T_L(tbl) + i;

        switch(line->tl_type) {
            case KT_LINE  :
                break;
            case KT_DATE  :
                strcat(buf, SCR_long_to_fdate(SCR_current_date(), date, "dd/mm/yy"));
                break;
            case KT_MODE  :
                for(j = 0; j < MAX_MODE; j++) {
                    if(KT_mode[j] == 0) continue;
                    sprintf(date, "(%s) ", COL_OPERS[j + 1]);
                    strcat(buf, date);
                    //strcat(buf, KLG_OPERS_TEXTS[j + 1][B_LANG]);
                    strcat(buf, KLG_OPERS_TEXTS[j + 1][K_LANG]); // JMP 18-04-2022
                    strcat(buf, "\n");
                    nm ++;
                }
                break;
            case KT_FILES :
                for(j = 0; KT_names[j]; j++) {
                    strcat(buf, KT_names[j]);
                    strcat(buf, "\n");
                    nf ++;
                }
                break;
            case KT_TITLE :
                strcat(buf, IodeTblCell((TCELL *) line->tl_val, NULL, nbdec));
                //strcat(buf,"\x01\x02\03"); // JMP 13/7/2022
                break;
            case KT_CELL  :
                COL_clear(cls);
                if(COL_exec(tbl, i, cls) < 0)
                    strcat(buf, "Error in calc");
                else
                    for(j = 0; j < cls->cl_nb; j++) {
                        d = j % T_NC(tbl);
                        if(tbl->t_free == 0 && d == 0 && j != 0) continue;
                        strcat(buf, IodeTblCell((TCELL *) line->tl_val + d, cls->cl_cols + j, nbdec));
                        strcat(buf, "\t");
                    }
                break;
        }

        if(buf[0]) {
            SCR_add_ptr(&l, &nli, buf);
            (*nl)++;
        }
    }
    SCR_add_ptr(&l, &nli, NULL);
    *nl += nf + nm;
    res = SCR_mtov(l, '\n');

    COL_free_cols(cls);
    SCR_free_tbl(l);
    SCR_free(buf);

    SCR_free_tbl(KT_names);
    KT_names = NULL;
    KT_nbnames = 0;

    return(res);
}

char *IodeDdeCreateObj(int objnb, int type, int *nc, int *nl)
{
    char    *obj, *res;
    KDB     *kdb = K_WS[type];

    if(type != K_TBL) {
        *nc = 2;
        *nl = 1;
        switch(type) {
            case K_CMT :
                obj = (char *)KCVAL(kdb, objnb);
                break;
            case K_EQS :
                obj = (char *)KELEC(kdb, objnb);
                break;
            case K_IDT :
                obj = (char *)KILEC(kdb, objnb);
                break;
            case K_LST :
                obj = (char *)KLVAL(kdb, objnb);
                break;
            default    :
                obj = SCR_stracpy("Not yet implemented") ;
                break;
        }

        if(obj == 0) obj = " ";
        res = SCR_malloc((int)sizeof(ONAME) + 10 + (int)strlen(obj));
        strcpy(res, KONAME(kdb, objnb));
        strcat(res, "\t");
        strcat(res, obj);
        /* strcat(res, "\t");*/
    }
    else
        res = IodeDdeCreateTbl(objnb, NULL, nc, nl, -1);

    SCR_OemToAnsi(res, res);
    return(res);
}

char *IodeDdeGetReportRC(char *szItem)
{
    char    *res;
    int     rc;

    rc = B_ReportLine(szItem);
    res = SCR_malloc(30);
    sprintf(res, "%d", rc);
    return(res);
}


#if defined(DOSW32) || defined(SCRW32)

char *IodeDdeGetXObj(char *szItem, int type)
{
    U_ch    **tbl, **lst;
    char    *res = 0, *sheet, *item,
             *plst =  NULL, *K_expand();
    KDB     *kdb;
    int     objnb, i, l, h;
    HCONV   hConv;

    kdb = K_WS[type];
    tbl = SCR_vtom(szItem, '!');
    if(SCR_tbl_size(tbl) < 1) lst = SCR_vtom("", ',');
    else {
        plst = K_expand(type, NULL, tbl[0], '*');
        lst = B_ainit_chk(plst, NULL, 0);
    }

    if(SCR_tbl_size(tbl) < 2) sheet = "Sheet1";
    else                      sheet = tbl[1];
    if(SCR_tbl_size(tbl) < 3) item = "R1C1";
    else                      item = tbl[2];

    hConv = WscrDdeConnect("EXCEL", sheet);
    if(hConv == 0) return(SCR_stracpy("Error"));

    switch(type) {
        case K_VAR:
            if(SCR_tbl_size(lst) == 0) {
                res = IodeDdeCreatePer(0);
                WscrDdeSetItem(hConv,
                               IodeDdeXlsCell(item, 0, 1,  KSMPL(kdb)->s_nb, 1), res);
                SCR_free(res);
            }

            if(SCR_tbl_size(lst) == 0) {
                for(i = 0 ; i < KNB(kdb) ; i++) {
                    res = IodeDdeCreateSeries(i, 0);
                    WscrDdeSetItem(hConv,
                                   IodeDdeXlsCell(item, i + 1, 0, 1 + KSMPL(kdb)->s_nb, 1), res);
                    SCR_free(res);
                }
            }

            else {
                for(i = 0 ; lst[i] ; i++) {
                    if(strcmp(lst[i], "t") == 0) {
                        res = IodeDdeCreatePer(0);
                        WscrDdeSetItem(hConv,
                                       IodeDdeXlsCell(item, i, 1, 1 + KSMPL(kdb)->s_nb, 1), res);
                        SCR_free(res);
                    }
                    else {
                        objnb = K_find(kdb, lst[i]);
                        if(objnb < 0) continue;

                        res = IodeDdeCreateSeries(objnb, 0);
                        WscrDdeSetItem(hConv,
                                       IodeDdeXlsCell(item, i, 0, 1 + KSMPL(kdb)->s_nb, 1), res);
                        SCR_free(res);
                    }
                }
            }
            break;

        default :
            if(SCR_tbl_size(lst) == 0) {
                for(i = 0 ; i < KNB(kdb) ; i++) {
                    res = IodeDdeCreateObj(i, type, &l, &h);
                    WscrDdeSetItem(hConv,
                                   IodeDdeXlsCell(item, i, 0, l, h), res);
                    SCR_free(res);
                }
            }

            else {
                for(i = 0 ; lst[i] ; i++) {
                    objnb = K_find(kdb, lst[i]);
                    if(objnb < 0) continue;
                    res = IodeDdeCreateObj(objnb, type, &l, &h);
                    WscrDdeSetItem(hConv,
                                   IodeDdeXlsCell(item, i, 0, l, h), res);
                    SCR_free(res);
                }
            }
    }

    SCR_free_tbl(lst);
    SCR_free(plst);
    WscrDdeEnd(hConv) ;
    SCR_free_tbl(tbl);
    return(SCR_stracpy("Ok"));
}

char *IodeDdeGetItem(char *szTopic, char *szItem)
{
    int     objnb, t, type;
    KDB     *kdb;
    double  x;
    char    buf[80], *res;
    SCL     *scl;

    SCR_vtime = 1;
    SCR_upper(szTopic);
    kmsg("Dispatching DDE-call: %s!%s", szTopic, szItem);

    if(strcmp(szTopic, "WS") == 0)   return(IodeDdeGetWS(szItem));
    if(strcmp(szTopic, "XVAR") == 0) return(IodeDdeGetXObj(szItem, K_VAR));
    if(strcmp(szTopic, "XCMT") == 0) return(IodeDdeGetXObj(szItem, K_CMT));
    if(strcmp(szTopic, "XLST") == 0) return(IodeDdeGetXObj(szItem, K_LST));
    if(strcmp(szTopic, "XIDT") == 0) return(IodeDdeGetXObj(szItem, K_IDT));
    if(strcmp(szTopic, "XTBL") == 0) return(IodeDdeGetXObj(szItem, K_TBL));
    if(strcmp(szTopic, "XREP") == 0) return(IodeDdeGetReportRC(szItem));

    type = IodeDdeType(szTopic);
    if(type < 0 || type > 6) return(SCR_stracpy("Error"));
    kdb = K_WS[type];
    if(type == K_SCL) SCR_lower(szItem);
    objnb = K_find(kdb, szItem);
    if(objnb < 0) return((char *)0);

    switch(type) {
        case K_EQS :
        case K_IDT :
        case K_TBL :
            return((char *)0);

        case K_CMT :
        case K_LST :
            res = SCR_stracpy((char *)KCVAL(kdb, objnb));
            SCR_replace(res, "\t", " ");
            SCR_replace(res, "\n", " ");
            return(res);

        case K_VAR :
            res = SCR_malloc(40 * (1 + KSMPL(kdb)->s_nb)); /* JMP 29-06-00 */
            for(t = 0 ; t < KSMPL(kdb)->s_nb ; t++) {
                x = *(KVVAL(kdb, objnb, t));
                if(!L_ISAN(x)) strcpy(buf, "0");
                //else           SCR_fmt_dbl(x, buf, 16, -1);  /* JMP 01-02-99 */
                else           IodeFmtVal(buf, x);             /* JMP 01-02-99 */
                strcat(res, buf);
                strcat(res, "\t");
            }
            return(res);

        case K_SCL :
            res = SCR_malloc(40);
            scl = KSVAL(kdb, objnb);
            if(!L_ISAN(scl->val)) strcpy(res, "0");
            //else                  SCR_fmt_dbl(scl->val, res, 16, -1); /* JMP 01-02-99 */
            else                  IodeFmtVal(res, scl->val);    /* JMP 01-02-99 */
            return(res);
        default:
            return((char *)0);
    }
}

int IodeDdeSetWS(char *szItem, char *szBuffer)
{
    KDB     *kdb;
    int     type, rc = 0;

    kmsg("Dispatching DDE-Poke: %s!%s", szItem, szBuffer);
    SCR_upper(szItem);
    type = IodeDdeType(szItem);
    if(type < 0 || type > 6) return(-1);

    kdb = K_WS[type];
    if(strcmp(szItem, "SAMPLE") == 0) rc = B_WsSample(szBuffer);
    else if(strcmp(szItem + 1, "NAME") == 0)  rc = B_WsName(szBuffer, type);
    else if(strcmp(szItem + 1, "DESCR") == 0) rc = B_WsDescr(szBuffer, type);
    else if(strcmp(szItem + 1, "CLEAR") == 0) rc = B_WsClear("", type);
    else if(strcmp(szItem + 1, "LOAD") == 0)  rc = B_WsLoad(szBuffer, type);
    else if(strcmp(szItem + 1, "SAVE") == 0)  rc = B_WsSave(szBuffer, type);
    return(rc);
}


int IodeDdePlay(char *szItem, char *szBuffer)
{
    U_ch    **keys;
    int     key, i, n;

    SCR_upper(szItem);
    if(strcmp(szItem, "TEXT") == 0) {
        krecordtext(szBuffer);
        return(0);
    }
    else {
        keys = SCR_vtoms(szBuffer, ", ;");
        n = SCR_tbl_size(keys);
        for(i = n - 1 ; i >= 0 ; i--) {
            key = DdeTsfKey(keys[i]);
            if(key) krecordkey(key);
        }
        SCR_free_tbl(keys);
    }
    return(0);
}

int DdeTsfKey(char *key)
{
    if(strlen(key) == 1) return(key[0]);
    SCR_upper(key);
    if(strcmp(key, "HOME") == 0)     return(SCR_CSR_HOME);
    if(strcmp(key, "END") == 0)      return(SCR_CSR_END);
    if(strcmp(key, "UP") == 0)       return(SCR_CSR_UP);
    if(strcmp(key, "DOWN") == 0)     return(SCR_CSR_DOWN);
    if(strcmp(key, "PGUP") == 0)     return(SCR_CSR_PG_UP);
    if(strcmp(key, "PGDN") == 0)     return(SCR_CSR_PG_DN);
    if(strcmp(key, "ESC") == 0)      return(SCR_ESCAPE);
    if(strcmp(key, "ESCAPE") == 0)   return(SCR_ESCAPE);
    if(strcmp(key, "LEFT") == 0)     return(SCR_CSR_LEFT);
    if(strcmp(key, "RIGHT") == 0)    return(SCR_CSR_RIGHT);
    if(strcmp(key, "ENTER") == 0)    return(SCR_ENTER);
    if(strcmp(key, "BACK") == 0)     return(SCR_BACKSPACE);
    if(strcmp(key, "BACKSPACE") == 0) return(SCR_BACKSPACE);
    if(strcmp(key, "DEL") == 0)      return(SCR_DELETE);
    if(strcmp(key, "DELETE") == 0)   return(SCR_DELETE);
    if(strcmp(key, "TAB") == 0)      return(SCR_TAB);

    if(strlen(key) == 2) {
        if(key[0] == 'A') return(SCR_A_A + key[1] - 'A');
        if(key[0] == 'C') return(SCR_C_A + key[1] - 'A');
    }
    if(memcmp(key, "CF", 2) == 0) return(SCR_C_F1 + atoi(key + 2) - 1);
    if(memcmp(key, "AF", 2) == 0) return(SCR_A_F1 + atoi(key + 2) - 1);
    if(memcmp(key, "SF", 2) == 0) return(SCR_S_F1 + atoi(key + 2) - 1);

    return(0);
}

int IodeDdeSetItem(char *szTopic, char *szItem, char *szBuffer)
{
    char    *tmp, buf[30];
    KDB     *kdb;
    int     type;

    kmsg("Dispatching DDE-Poke: %s!%s", szTopic, szItem);
    SCR_vtime = 1;
    SCR_upper(szTopic);
    if(strcmp(szTopic, "WS") == 0)   return(IodeDdeSetWS(szItem, szBuffer));
    if(strcmp(szTopic, "PLAY") == 0) return(IodeDdePlay(szItem, szBuffer));
    if(strcmp(szTopic, "REP") == 0)  return(B_ReportLine(szBuffer));

    type = IodeDdeType(szTopic);
    if(type < 0 || type > 6) return(-1);

    kdb = K_WS[type];

    tmp = SCR_malloc((int)strlen(szBuffer) + 30);
    if(type == K_VAR) {
        sprintf(tmp, "%s %s %s", szItem,
                PER_pertoa(&(KSMPL(kdb)->s_p1), buf), szBuffer);
    }
    else {
        sprintf(tmp, "%s %s", szItem, szBuffer);
    }

    SCR_replace(tmp, "\t", " ");
    B_DataUpdate(tmp, type);
    SCR_free(tmp);
    return(0);
}

/* Interface to EXCEL via DDE */
/* fn rapport
    $ExcelSetVar VARNAME [Sheet] [R1C1]
    $ExcelGetVar VARNAME [PERIODE] [Sheet] [R1C1]

    $ExcelExecute Open("c:\usr\iode\test.xls")
    $ExcelExecute Close

*/
char    *B_ExcelGetItem(char *arg)
{
    char    *ddeptr = NULL,
             **args = NULL;
    int     nb_args;

    args = SCR_vtom(arg, '!');
    nb_args = SCR_tbl_size(args);

    if(nb_args == 2) ddeptr = WscrDdeGet("Excel", args[0], args[1]);
    IodeDdeUnLocale(ddeptr);

    SCR_free_tbl(args);
    return(ddeptr);
}

int B_ExcelSetItem(char *ddeitem, char *ptr, int nc, int nl)
{
    char    **args = NULL, *topic, *item;
    int     nb_args, rc = -1;

    args = SCR_vtom(ddeitem, '!');
    nb_args = SCR_tbl_size(args);

    if(nb_args == 2) {
        topic = args[0];
        item = args[1];
    }
    else {
        topic = "Sheet1";
        if(nb_args == 1) item = args[0];
        else item = "R1C1";
    }

    rc = WscrDdeSet("Excel", topic,
                    IodeDdeXlsCell(item, 0, 0, nc, nl), ptr);
    SCR_free_tbl(args);
    return(rc);
}

int B_ExcelDecimal(char *arg)
{
    char    **args;
    int     nb_args, rc= 0;

    args = SCR_vtoms(arg, B_SEPS);
    nb_args = SCR_tbl_size(args);

    if(nb_args == 1) {
        switch(args[0][0]) {
            case 'c': /* comma */
            case 'C':
                SCR_sDECIMAL = ',';
                break;
            default:
                SCR_sDECIMAL = '.';
                break;
        }
    }
    else SCR_sDECIMAL = '.';

    return(rc);
}

/**
Tells IODE which thousand sep Excel uses (for $ExcelGetVar).
$ExcelSetThousand => no sep
$ExcelSetThousand Comma => skip commas when reading from excel sheet
$ExcelSetThousand Period => skip periods when reading from excel sheet (! decimals !)
  - comma or Comma : ,
  - p, P, d, D : dot (period)
  - n, N : no separator
  - no arg => no sep
*/

int B_ExcelThousand(char *arg)
{
    char    **args;
    int     nb_args, rc = 0;

    args = SCR_vtoms(arg, B_SEPS);
    nb_args = SCR_tbl_size(args);

    if(nb_args == 1) {
        switch(args[0][0]) {
            case 'c': /* comma */
            case 'C':
                SCR_sTHOUSAND = ',';
                break;

            case 'p': /* period */
            case 'P':
            case 'd' :
            case 'D' :
                SCR_sTHOUSAND = '.';
                break;

            case 's': /* space */
            case 'S':
                SCR_sTHOUSAND = ' ';
                break;

            case 'n': /* none */
            case 'N':
                SCR_sTHOUSAND = 0;
                break;

            default:
                SCR_sTHOUSAND = args[0][0];
                break;
        }
    }
    else SCR_sTHOUSAND = 0;
    return(rc);
}


/**
Tells IODE to skip the currency sign when readinf Excel data (for $ExcelGetVar).
$ExcelSetCurrency => no sep
$ExcelSetCurrency Dollar => skip $
$ExcelSetCurrency Pound => skip �
$ExcelSetCurrency Euro => skip �
$ExcelSetCurrency c => skip c (any c)
$ExcelSetCurrency [no arg] => no sep
*/

int B_ExcelCurrency(char *arg)
{
    char    **args;
    int     nb_args, rc = 0;

    args = SCR_vtoms(arg, B_SEPS);
    nb_args = SCR_tbl_size(args);

    if(nb_args == 1) {
        switch(args[0][0]) {
            case 'd': /* dollar */
            case 'D':
                SCR_sCURRENCY = '$';
                break;

            case 'e': /* EUR */
            case 'E':
                SCR_sCURRENCY = '�';
                break;

            case 'p': /* Pound */
            case 'P':
                SCR_sCURRENCY = '�';
                break;

            default:
                SCR_sCURRENCY = args[0][0];
                break;
        }
    }
    else SCR_sCURRENCY = 0;

    return(rc);
}


int B_ExcelLang(char *arg)
{
    char    **args;
    int     nb_args, rc = 0;

    args = SCR_vtoms(arg, B_SEPS);
    nb_args = SCR_tbl_size(args);

    if(nb_args == 1) {
        switch(args[0][0]) {
            case 'f':
            case 'F':
                ExcelCellRef = ExcelCellFr;
                ExcelSheetRef = ExcelSheetFr;
                break;
            case 'n':
            case 'N':
                ExcelCellRef = ExcelCellNl;
                ExcelSheetRef = ExcelSheetNl;
                break;
            default:
                ExcelCellRef = ExcelCellEn;
                ExcelSheetRef = ExcelSheetEn;
        }
    }
    else {
        ExcelCellRef = ExcelCellEn;
        ExcelSheetRef = ExcelSheetEn;
    }
    return(rc);
}

int B_ExcelGet(char *arg, int type)
{
    char    *ddeptr = NULL, *argptr = NULL, /* GB 26/01/98 */
             **args = NULL, **tbl = NULL;
    int     nb_args, ntbl = 0,
                     i, rc = -1;

    args = SCR_vtoms(arg, B_SEPS);
    nb_args = SCR_tbl_size(args);

    ddeptr = B_ExcelGetItem(args[nb_args - 1]);
    if(ddeptr == NULL) goto the_end;

    for(i = 0; i < nb_args - 1; i++) SCR_add_ptr(&tbl, &ntbl, args[i]);
    SCR_add_ptr(&tbl, &ntbl, ddeptr);
    SCR_add_ptr(&tbl, &ntbl, NULL);

    argptr = SCR_mtov(tbl, ' ');
    rc = B_DataUpdate(argptr, type);

the_end:
    SCR_free_tbl(args);
    SCR_free_tbl(tbl);
    SCR_free(argptr);
    SCR_free(ddeptr);
    return(rc);
}

int B_ExcelSet(char *arg, int type)
{
    int         pos, shift, rc = -1, 
                nb_args, nbr = 1, nc = 1, nl = 1;
    KDB         *kdb = K_WS[type];
    SCL         *scl;
    PERIOD      *per = NULL;
    IODE_REAL   d;
    char        **args = NULL,
                *ptr = NULL,
                *item, *smpl;

    args = SCR_vtoms(arg, B_SEPS);
    nb_args = SCR_tbl_size(args);

    pos = K_find(kdb, args[0]);
    if(pos < 0) goto the_end;

    item = args[nb_args - 1];
    switch(type) {
        case K_CMT :
            ptr = SCR_stracpy(KCVAL(kdb, pos));
            break;
        case K_IDT :
            ptr = SCR_stracpy(KILEC(kdb, pos));
            break;
        case K_LST :
            ptr = SCR_stracpy(KLVAL(kdb, pos));
            break;
        case K_EQS :
            ptr = SCR_stracpy(KELEC(kdb, pos));
            break;
        case K_SCL :
            scl = KSVAL(kdb, pos); /* JMP 10-08-00 */
            d = scl->val;          /* JMP 10-08-00 */
            ptr = SCR_malloc(80);  /* JMP 10-08-00 */
            IodeFmtVal(ptr, d);    /* JMP 01-02-99 */
            break;                 /* JMP 10-08-00 */

        case K_TBL :
            if(nb_args == 3) smpl = args[1];
            else smpl = NULL;

            ptr = IodeDdeCreateTbl(pos, smpl, &nc, &nl, -1);
            break;

        case K_VAR : /* Name Period nVal */
            per = PER_atoper(args[1]);
            if(per == NULL
                    || (shift = PER_diff_per(per, &(KSMPL(kdb)->s_p1))) < 0) shift = 0;
            else item = args[2];

            ptr = IodeDdeCreateSeries(pos, shift);
            nc = 1 + KSMPL(kdb)->s_nb - shift; /* JMP 04-10-99 */
            break;
    }

    SCR_OemToAnsi(ptr, ptr);
    IodeDdeLocale(ptr);
    rc = B_ExcelSetItem(item, ptr, nc, nl);

the_end:
    if(pos < 0) B_seterror(B_msg(98), args[0]);  /* JMP 10-08-00 */

    SCR_free_tbl(args);
    SCR_free(per);
    SCR_free(ptr);

    if(rc >= 0) rc = 0;

    return(rc);
}

int B_ExcelExecute(char *arg)
{
    int     rc;
    char    *cmd;

    SCR_strip(arg);
    cmd = SCR_malloc((int)strlen(arg)+2);
    sprintf(cmd, "[%s]", arg);
    rc = WscrDdeExecute("Excel", "System", cmd);

    SCR_free(cmd);
    return(rc);
}

int B_ExcelCmd(char *cmd, char *arg)
{
    int     rc;
    char    *xlscmd;

    SCR_strip(cmd);
    if(arg) SCR_strip(arg);
    if(arg != NULL && arg[0] != '\0') {
        xlscmd = SCR_malloc((int)strlen(cmd) + (int)strlen(arg) + 10);

        if(U_is_in('"', arg)) sprintf(xlscmd, "[%s(%s)]", cmd, arg);
        else                  sprintf(xlscmd, "[%s(\"%s\")]", cmd, arg);
    }
    else {
        xlscmd = SCR_malloc((int)strlen(cmd) + 2);
        sprintf(xlscmd, "[%s]", cmd);
    }

    rc = WscrDdeExecute("Excel", "System", xlscmd);

    SCR_free(xlscmd);
    return(rc);
}

int B_DDEGet(char *arg)
{
    U_ch    **tbl, *ddeptr;

    tbl = SCR_vtom(arg, ' ');
    if(SCR_tbl_size(tbl) < 3) return(-1);
//    rc = WscrDdeExecute(tbl[0], tbl[1], tbl[2]);
    ddeptr = WscrDdeGet(tbl[0], tbl[1], tbl[2]);

    SCR_free_tbl(tbl);
    SCR_free(ddeptr);
    return(0);
}

/* JMP 03-08-2004 */
int B_ExcelWrite(char *ptr)
{
    char    **args = NULL, *topic, *item, *val, **tbl, **tbl2;
    int     i, nb_args, rc = -1, pos, nl = 1, nc = 1, nc1;


    U_ljust_text(ptr);
    pos = U_pos(' ', ptr);
    if(pos < 0)
        val = "";
    else {
        ptr[pos] = 0;
        val = ptr + pos + 1;
        U_ljust_text(val);
        SCR_replace(val, "\\t", "\t");
        SCR_replace(val, "\\n", "\n");
        tbl = SCR_vtom(val, '\n');
        nl = SCR_tbl_size(tbl);
        for(i = 0 ; i < nl ; i++) {
            tbl2 = SCR_vtom(tbl[i], '\t');
            nc1 = SCR_tbl_size(tbl2);
            if(nc1 > nc) nc = nc1;
            SCR_free_tbl(tbl2);
        }
        SCR_free_tbl(tbl);
    }

    args = SCR_vtom(ptr, '!');
    nb_args = SCR_tbl_size(args);

    if(nb_args == 2) {
        topic = args[0];
        item = args[1];
    }
    else {
        topic = "Sheet1";
        if(nb_args == 1) item = args[0];
        else item = "R1C1";
    }

    rc = WscrDdeSet("Excel", topic,
                    IodeDdeXlsCell(item, 0, 0, nc, nl), val);
    SCR_free_tbl(args);
    return(rc);
}

#else
int B_DDEGet(char *arg)
{
    return(-1);
}

char    *B_ExcelGetItem(char *arg)
{
    return(0);
}
int B_ExcelGet(char *arg, int type)
{
    return(-1);
}

int B_ExcelSet(char *arg, int type)
{
    return(-1);
}

int B_ExcelExecute(char *arg)
{
    return(-1);
}

int B_ExcelCmd(char *cmd, char *arg)
{
    return(-1);
}

int B_ExcelWrite(char *ptr)
{
    return(-1);
}

#endif

int B_ExcelOpen(char *arg)
{
    return(B_ExcelCmd("OPEN", arg));
}

int B_ExcelClose(char *arg)
{
    return(B_ExcelCmd("CLOSE", NULL));
}

int B_ExcelPrint(char *arg)
{
    return(B_ExcelCmd("PRINT", NULL));
}

int B_ExcelSave(char *arg)
{
    return(B_ExcelCmd("SAVE", NULL));
}

int B_ExcelSaveAs(char *arg)
{
    return(B_ExcelCmd("SAVE.AS", arg));
}

int B_ExcelNew(char *arg)
{
    return(B_ExcelCmd("NEW(5)", NULL));
}

/*
int B_ExcelSendKeys(char *arg)
{
    char    buffer[81];

    B_ExcelCmd("APP.MAXIMIZE()", NULL);

    sprintf(buffer, "SEND.KEYS(\"%s\", TRUE)", arg);
    return(B_ExcelCmd(buffer, NULL));
}
*/


int IodeFmtVal(char *buf, IODE_REAL val)
{
    if(L_ISAN(val)) {
        if(fabs(val) < 1e-12) val =0.0;
#ifdef REALD
        sprintf(buf, "%.15lg ", (double)(val));
#else
        sprintf(buf, "%.8lg ", (double)(val));
#endif
    }
    else
        sprintf(buf, "na ");
    return(0);
}

