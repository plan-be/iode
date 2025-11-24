/**
 *  @header4iode
 * 
 *  This file contains 2 groups of functions: 
 *      - implementation of an IODE DDE server, allowing access to IODE functions from inside any program able to 
 *        interact with a DDE server, like Excel or Word
 *      - functions to access the Excel DDE server. Some of IODE report commands line $ExcelGet are implemented here.
 *  
 *  List of functions 
 *  -----------------
 *      int IodeDdeLocale(char *buf)
 *      int IodeDdeUnLocale(char *buf)
 *      int IodeDdeType(char *szTopic)
 *      char *IodeDdeGetWS(char *szItem)
 *      char *IodeDdeCreateSeries(int objnb, int bt)
 *      char *IodeDdeCreatePer(int bt)
 *      char *ToBase26(int num)
 *      char *IodeDdeXlsCell(char *offset, int i, int j, int lg, int hg)
 *      char *IodeTblCell(TableCell *cell, COL *cl, int nbdec)
 *      char *IodeDdeCreateTbl(int objnb, char *ismpl, int *nc, int *nl, int nbdec)
 *      char *IodeDdeCreateObj(int objnb, int type, int *nc, int *nl)
 *      char *IodeDdeGetReportRC(char *szItem)
 *      char *IodeDdeGetXObj(char *szItem, int type)
 *      char *IodeDdeGetItem(char *szTopic, char *szItem)
 *      int IodeDdeSetWS(char *szItem, char *szBuffer)
 *      int IodeDdePlay(char *szItem, char *szBuffer)
 *      int DdeTsfKey(char *key)
 *      int IodeDdeSetItem(char *szTopic, char *szItem, char *szBuffer)
 *  
 *      char *B_ExcelGetItem(char *arg)
 *      int B_ExcelSetItem(char *ddeitem, char *ptr, int nc, int nl)
 *      int B_ExcelDecimal(char *arg, int unused)
 *      int B_ExcelThousand(char *arg, int unused)
 *      int B_ExcelCurrency(char *arg, int unused)
 *      int B_ExcelLang(char *arg, int unused)
 *      int B_ExcelGet(char *arg, int type)
 *      int B_ExcelSet(char *arg, int type)
 *      int B_ExcelExecute(char *arg, int unused)
 *      int B_ExcelCmd(char *cmd, char *arg)
 *      int B_DDEGet(char *arg, int unused)
 *      int B_ExcelWrite(char *ptr, int unused)
 *      int B_DDEGet(char *arg, int unused)
 *      char *B_ExcelGetItem(char *arg)
 *      int B_ExcelGet(char *arg, int type)
 *      int B_ExcelSet(char *arg, int type)
 *      int B_ExcelExecute(char *arg, int unused)
 *      int B_ExcelCmd(char *cmd, char *arg)
 *      int B_ExcelWrite(char *ptr, int unused)
 *      int B_ExcelOpen(char *arg, int unused)
 *      int B_ExcelClose(char *arg, int unused)
 *      int B_ExcelPrint(char *arg, int unused)
 *      int B_ExcelSave(char *arg, int unused)
 *      int B_ExcelSaveAs(char *arg, int unused)
 *      int B_ExcelNew(char *arg, int unused)
 *      int IodeFmtVal(char *buf, double val)
 */
#include "scr4/scr4w.h"

#include "api/constants.h"
#include "api/b_args.h"
#include "api/k_lang.h"
#include "api/k_super.h"
#include "api/b_errors.h"
#include "api/print/print.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/grep.h"
#include "api/objs/comments.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"
#include "api/print/print.h"

#include "api/report/commands/commands.h"
#include "api/report/engine/engine.h"
#include "api/report/undoc/undoc.h"


/**************** WINDOWS : DDE **********************/

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
    return(U_pos(SCR_upper_char(szTopic[0]), (unsigned char*) "CEILSTV"));
}

char *IodeDdeGetWS(char *szItem)
{
    KDB     *kdb;
    char    *res;
    int     i, type;

    SCR_upper((unsigned char*) szItem);
    type = IodeDdeType(szItem);
    if(type < 0 || type > 6) 
        return((char*) SCR_stracpy((unsigned char*) "Error"));

    kdb = get_global_db(type);
    if(strcmp(szItem, "Sample") == 0) 
    {
        res = (char*) global_ws_var->sample->to_string().c_str();
        return(res);
    }
    else if(strcmp(szItem + 1, "LIST") == 0) 
    {
        if(kdb == 0 || kdb->size() == 0) 
            return((char *)0);
        res = SCR_malloc((sizeof(ONAME) + 1) * (1 + kdb->size())); /* IODE64K */
        for(i = 0 ; i < kdb->size() ; i++) 
        {
            strcat(res, kdb->get_name(i).c_str());
            strcat(res, "\t");
        }
        return(res);
    }
    else if(strcmp(szItem + 1, "NAME") == 0) 
    {
        if(kdb == 0) 
            return((char*) 0);
        res = (char*) kdb->filepath.c_str();
        return(res);
    }
    else if(strcmp(szItem + 1, "DESCR") == 0) 
    {
        if(kdb == 0) 
            return((char*) 0);
        res = (char*) kdb->description.c_str();
        return(res);
    }
    else if(strcmp(szItem + 1, "NB") == 0) 
    {
        if(kdb == 0) 
            return((char*) 0);
        res = SCR_malloc(20);
        sprintf(res, "%d", kdb->size());
        return(res);
    }
    return((char*) 0);
}

char *IodeDdeCreateSeries(int objnb, int bt)
{
    char    *res, buf[128];
    KDB     *kdb = global_ws_var.get();
    int     t;
    double  x;

    std::string name = kdb->get_name(objnb);
    res = SCR_malloc(40 * (1 + kdb->sample->nb_periods - bt)); /* JMP 29-06-00 */
    strcpy(res, name.c_str());
    strcat(res, "\t");
    for(t = bt ; t < kdb->sample->nb_periods ; t++) 
    {
        x = *(KVVAL(kdb, name, t));
        if(!IODE_IS_A_NUMBER(x)) 
            strcpy(buf, "#N/A");
        else 
            IodeFmtVal(buf, x);
        strcat(res, buf);
        strcat(res, "\t");
    }

    SCR_sqz((unsigned char*) res);
    return(res);
}

char *IodeDdeCreatePer(int bt)
{
    char    *res;
    KDB     *kdb = global_ws_var.get();
    int     t;

    res = SCR_malloc(11 * (1 + kdb->sample->nb_periods - bt));
    for(t = bt ; t < kdb->sample->nb_periods ; t++) 
    {
        Period per = kdb->sample->start_period.shift(t);
        strcat(res, (char*) per.to_string().c_str());
        strcat(res, "\t");
    }

    SCR_sqz((unsigned char*) res);
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
        pos = U_pos('C', (unsigned char*) offset);
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

#define DDECELLSIZE 512

char* IodeTblCell(TableCell *cell, COL *cl, int nbdec)
{
    static char    buf[DDECELLSIZE + 1]; /* JMP 4/4/2016 (PS) !!! */
    char    *ptr = NULL;

    buf[0] = 0;

    if(cell->is_null()) 
        return(buf);

    if(cl == NULL || cell->get_type() == TABLE_CELL_STRING) 
    {
        std::string text = cell->get_content();
        ptr = (char *) COL_text(cl, (char*) text.c_str(), KT_nbnames);
        SCR_strlcpy((unsigned char*) buf, (unsigned char*) ptr, DDECELLSIZE);
        buf[DDECELLSIZE] = 0;
        SW_nfree(ptr);
    }
    else 
    {
        if(!IODE_IS_A_NUMBER(cl->cl_res)) 
            strcpy(buf, "#N/A");
        else 
        {
            sprintf(buf, "%80.20lf", cl->cl_res); /* JMP 18-10-07 */
            SCR_sqz((unsigned char*) buf);                         /* JMP 18-10-07 */
            SCR_pstrip((unsigned char*) buf, (unsigned char*) "0");                 /* JMP 18-10-07 */
        }
        // T_fmt_val(buf, cl->cl_res, 20, nbdec); /* JMP 18-10-07 */
    }

    return(buf);
}


/**
 *  Compute the table objnb on the GSample ismpl and return a string containing the result.
 */

char* IodeDdeCreateTbl(int objnb, char *ismpl, int *nc, int *nl, int nbdec)
{
    int     dim, i, j, d, rc = 0, nli = 0,
                          nf = 0, nm = 0;
    char    gsmpl[128], **l = NULL, *buf, *res = NULL; /* JMP 30-04-98 */
    COLS    *cls;

    std::string name = global_ws_tbl->get_name(objnb);
    Table* tbl = KTVAL(global_ws_tbl.get(), name);
    Sample* smpl = global_ws_var->sample;

    /* date */
    char date[11];

    if(smpl == nullptr || smpl->nb_periods == 0) 
        return (char*) "";

    /* mode */
    if(!ismpl)
        sprintf(gsmpl, "%s:%d", (char*) smpl->start_period.to_string().c_str(), smpl->nb_periods);
    else
        sprintf(gsmpl, "%s", ismpl);

    dim = T_prep_cls(tbl, gsmpl, &cls);
    if(dim < 0) 
        return((char*) SCR_stracpy((unsigned char*) "Error in Tbl or Smpl"));

    KT_names = T_find_files(cls);
    KT_nbnames = SCR_tbl_size((unsigned char**) KT_names);
    if(KT_nbnames == 0) 
        return((char*) SCR_stracpy((unsigned char*) "Error in Tbl or Smpl"));
    COL_find_mode(cls, KT_mode, 2);

    *nc = dim + 1;
    *nl = 1;

    TableLine* line;
    buf = SCR_malloc(256 + dim * 128);
    for(i = 0; rc == 0 && i < T_NL(tbl); i++) 
    {
        buf[0] = 0;
        line = &tbl->lines[i];

        switch(line->get_type()) 
        {
            case TABLE_LINE_SEP   :
                break;
            case TABLE_LINE_DATE  :
                strcat(buf, SCR_long_to_fdate(SCR_current_date(), date, "dd/mm/yy"));
                break;
            case TABLE_LINE_MODE  :
                for(j = 0; j < MAX_MODE; j++) 
                {
                    if(KT_mode[j] == 0) 
                        continue;
                    sprintf(date, "(%s) ", COL_OPERS[j + 1]);
                    strcat(buf, date);
                    //strcat(buf, KLG_OPERS_TEXTS[j + 1][B_LANG]);
                    strcat(buf, KLG_OPERS_TEXTS[j + 1][K_LANG]); // JMP 18-04-2022
                    strcat(buf, "\n");
                    nm ++;
                }
                break;
            case TABLE_LINE_FILES :
                for(j = 0; KT_names[j]; j++) 
                {
                    strcat(buf, KT_names[j]);
                    strcat(buf, "\n");
                    nf ++;
                }
                break;
            case TABLE_LINE_TITLE :
                strcat(buf, IodeTblCell(&(line->cells[0]), NULL, nbdec));
                //strcat(buf,"\x01\x02\03"); // JMP 13/7/2022
                break;
            case TABLE_LINE_CELL  :
                COL_clear(cls);
                if(COL_exec(tbl, i, cls) < 0)
                    strcat(buf, "Error in calc");
                else
                    for(j = 0; j < cls->cl_nb; j++) 
                    {
                        d = j % T_NC(tbl);
                        if(tbl->repeat_columns == 0 && d == 0 && j != 0) 
                            continue;
                        strcat(buf, IodeTblCell(&(line->cells[d]), cls->cl_cols + j, nbdec));
                        strcat(buf, "\t");
                    }
                break;
        }

        if(buf[0]) 
        {
            SCR_add_ptr((unsigned char***) &l, &nli, (unsigned char*) buf);
            (*nl)++;
        }
    }
    
    SCR_add_ptr((unsigned char***) &l, &nli, NULL);
    *nl += nf + nm;
    res = (char*) SCR_mtov((unsigned char**) l, '\n');

    COL_free_cols(cls);
    SCR_free_tbl((unsigned char**) l);
    SCR_free(buf);

    SCR_free_tbl((unsigned char**) KT_names);
    KT_names = NULL;
    KT_nbnames = 0;

    return(res);
}

char *IodeDdeCreateObj(int objnb, int type, int *nc, int *nl)
{
    KDB* kdb = get_global_db(type);
    if(objnb < 0 || objnb >= kdb->size())
        return (char*) 0;

    std::string name = kdb->get_name(objnb);
    
    char *obj, *res;
    std::string lec;
    if(type != TABLES) 
    {
        *nc = 2;
        *nl = 1;
        switch(type) 
        {
            case COMMENTS :
                obj = (char*) KCVAL(kdb, name);
                break;
            case EQUATIONS :
                lec = KELEC(kdb, name);
                obj = (char*) lec.c_str();
                break;
            case IDENTITIES :
                obj = (char*) KILEC(kdb, name);
                break;
            case LISTS :
                obj = (char*) KLVAL(kdb, name);
                break;
            default    :
                obj = (char*) SCR_stracpy((unsigned char*) "Not yet implemented") ;
                break;
        }

        if(obj == 0) obj = " ";
        res = SCR_malloc((int)sizeof(ONAME) + 10 + (int)strlen(obj));
        strcpy(res, name.c_str());
        strcat(res, "\t");
        strcat(res, obj);
    }
    else
        res = IodeDdeCreateTbl(objnb, NULL, nc, nl, -1);

    SCR_OemToAnsi((unsigned char*) res, (unsigned char*) res);
    return(res);
}

char *IodeDdeGetReportRC(char *szItem)
{
    char    *res;
    int     rc;

    rc = B_ReportLine(szItem, 1);
    res = SCR_malloc(30);
    sprintf(res, "%d", rc);
    return(res);
}


#if defined(DOSW32) || defined(SCRW32)

char *IodeDdeGetXObj(char *szItem, int type)
{
    U_ch    **tbl, **lst;
    char    *res = 0, *sheet, *item,
             *plst =  NULL;
    KDB     *kdb;
    int     objnb, i, l, h;
    HCONV   hConv;
    std::string name;

    kdb = get_global_db(type);
    tbl = SCR_vtom((unsigned char*) szItem, (int) '!');
    if(SCR_tbl_size(tbl) < 1) lst = SCR_vtom((unsigned char*) "", (int) ',');
    else {
        plst = K_expand(type, NULL, (char*) tbl[0], '*');
        lst = (unsigned char**) B_ainit_chk(plst, NULL, 0);
    }

    if(SCR_tbl_size(tbl) < 2) sheet = "Sheet1";
    else                      sheet = (char*) tbl[1];
    if(SCR_tbl_size(tbl) < 3) item = "R1C1";
    else                      item = (char*) tbl[2];

    hConv = WscrDdeConnect("EXCEL", sheet);
    if(hConv == 0) return((char*) SCR_stracpy((unsigned char*) "Error"));

    switch(type) {
        case VARIABLES:
            if(SCR_tbl_size(lst) == 0) {
                res = IodeDdeCreatePer(0);
                WscrDdeSetItem(hConv, IodeDdeXlsCell(item, 0, 1,  kdb->sample->nb_periods, 1), 
                               (unsigned char*) res);
                SCR_free(res);
            }

            if(SCR_tbl_size(lst) == 0) {
                for(i = 0 ; i < kdb->size() ; i++) {
                    res = IodeDdeCreateSeries(i, 0);
                    WscrDdeSetItem(hConv, IodeDdeXlsCell(item, i + 1, 0, 1 + kdb->sample->nb_periods, 1), 
                                   (unsigned char*) res);
                    SCR_free(res);
                }
            }

            else {
                for(i = 0 ; lst[i] ; i++) 
                {
                    name = std::string((char*) lst[i]);
                    if(strcmp((char*) lst[i], "t") == 0) {
                        res = IodeDdeCreatePer(0);
                        WscrDdeSetItem(hConv, IodeDdeXlsCell(item, i, 1, 1 + kdb->sample->nb_periods, 1), 
                                       (unsigned char*) res);
                        SCR_free(res);
                    }
                    else 
                    {
                        objnb = kdb->index_of(name);
                        if(objnb < 0) 
                            continue;

                        res = IodeDdeCreateSeries(objnb, 0);
                        WscrDdeSetItem(hConv, IodeDdeXlsCell(item, i, 0, 1 + kdb->sample->nb_periods, 1), 
                                       (unsigned char*) res);
                        SCR_free(res);
                    }
                }
            }
            break;

        default :
            if(SCR_tbl_size(lst) == 0) {
                for(i = 0 ; i < kdb->size() ; i++) {
                    res = IodeDdeCreateObj(i, type, &l, &h);
                    WscrDdeSetItem(hConv, IodeDdeXlsCell(item, i, 0, l, h), 
                                   (unsigned char*) res);
                    SCR_free(res);
                }
            }
            else 
            {
                for(i = 0 ; lst[i] ; i++) 
                {
                    name = std::string((char*) lst[i]);
                    objnb = kdb->index_of((char*) lst[i]);
                    if(objnb < 0) 
                        continue;
                    
                    res = IodeDdeCreateObj(objnb, type, &l, &h);
                    WscrDdeSetItem(hConv, IodeDdeXlsCell(item, i, 0, l, h), 
                                   (unsigned char*) res);
                    SCR_free(res);
                }
            }
    }

    SCR_free_tbl(lst);
    SCR_free(plst);
    WscrDdeEnd(hConv) ;
    SCR_free_tbl(tbl);
    return((char*) SCR_stracpy((unsigned char*) "Ok"));
}

char *IodeDdeGetItem(char *szTopic, char *szItem)
{
    int     t, type;
    KDB     *kdb;
    double  x;
    char    buf[80], *res;
    Scalar  *scl;
    std::string name;

    SCR_vtime = 1;
    SCR_upper((unsigned char*) szTopic);
    kmsg("Dispatching DDE-call: %s!%s", szTopic, szItem);

    if(strcmp(szTopic, "WS") == 0)   return(IodeDdeGetWS(szItem));
    if(strcmp(szTopic, "XVAR") == 0) return(IodeDdeGetXObj(szItem, VARIABLES));
    if(strcmp(szTopic, "XCMT") == 0) return(IodeDdeGetXObj(szItem, COMMENTS));
    if(strcmp(szTopic, "XLST") == 0) return(IodeDdeGetXObj(szItem, LISTS));
    if(strcmp(szTopic, "XIDT") == 0) return(IodeDdeGetXObj(szItem, IDENTITIES));
    if(strcmp(szTopic, "XTable") == 0) return(IodeDdeGetXObj(szItem, TABLES));
    if(strcmp(szTopic, "XREP") == 0) return(IodeDdeGetReportRC(szItem));

    type = IodeDdeType(szTopic);
    if(type < 0 || type > 6) 
        return((char*) SCR_stracpy((unsigned char*) "Error"));
    
    kdb = get_global_db(type);
    if(type == SCALARS) 
        SCR_lower((unsigned char*) szItem);
    
    name = std::string((char*) szItem);
    if(!kdb->contains(name)) 
        return((char *)0);

    switch(type) 
    {
        case EQUATIONS :
        case IDENTITIES :
        case TABLES :
            return((char *)0);

        case COMMENTS :
        case LISTS :
            res = (char*) SCR_stracpy((unsigned char*) KCVAL(kdb, name));
            SCR_replace((unsigned char*) res, (unsigned char*) "\t", (unsigned char*) " ");
            SCR_replace((unsigned char*) res, (unsigned char*) "\n", (unsigned char*) " ");
            return(res);

        case VARIABLES :
            res = SCR_malloc(40 * (1 + kdb->sample->nb_periods));
            for(t = 0 ; t < kdb->sample->nb_periods ; t++) 
            {
                x = *(KVVAL(kdb, name, t));
                if(!IODE_IS_A_NUMBER(x)) 
                    strcpy(buf, "0");
                else 
                    IodeFmtVal(buf, x);
                strcat(res, buf);
                strcat(res, "\t");
            }
            return(res);

        case SCALARS :
            res = SCR_malloc(40);
            scl = KSVAL(kdb, name);
            if(!IODE_IS_A_NUMBER(scl->value)) strcpy(res, "0");
            //else                  SCR_fmt_dbl(scl->value, res, 16, -1); /* JMP 01-02-99 */
            else                  IodeFmtVal(res, scl->value);    /* JMP 01-02-99 */
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
    SCR_upper((unsigned char*) szItem);
    type = IodeDdeType(szItem);
    if(type < 0 || type > 6) return(-1);

    kdb = get_global_db(type);
    if(strcmp(szItem, "Sample") == 0) rc = B_WsSample(szBuffer);
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

    SCR_upper((unsigned char*) szItem);
    if(strcmp(szItem, "TEXT") == 0) {
        krecordtext((unsigned char*) szBuffer);
        return(0);
    }
    else {
        keys = SCR_vtoms((unsigned char*) szBuffer, (unsigned char*) ", ;");
        n = SCR_tbl_size(keys);
        for(i = n - 1 ; i >= 0 ; i--) {
            key = DdeTsfKey((char*) keys[i]);
            if(key) krecordkey(key);
        }
        SCR_free_tbl(keys);
    }
    return(0);
}

int DdeTsfKey(char *key)
{
    if(strlen(key) == 1) return(key[0]);
    SCR_upper((unsigned char*) key);
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
    char    *tmp;
    KDB     *kdb;
    int     type;

    kmsg("Dispatching DDE-Poke: %s!%s", szTopic, szItem);
    SCR_vtime = 1;
    SCR_upper((unsigned char*) szTopic);
    if(strcmp(szTopic, "WS") == 0)   return(IodeDdeSetWS(szItem, szBuffer));
    if(strcmp(szTopic, "PLAY") == 0) return(IodeDdePlay(szItem, szBuffer));
    if(strcmp(szTopic, "REP") == 0)  return(B_ReportLine(szBuffer, 1));

    type = IodeDdeType(szTopic);
    if(type < 0 || type > 6) return(-1);

    kdb = get_global_db(type);

    tmp = SCR_malloc((int)strlen(szBuffer) + 30);
    if(type == VARIABLES) 
    {
        sprintf(tmp, "%s %s %s", szItem,
                (char*) kdb->sample->start_period.to_string().c_str(), szBuffer);
    }
    else
        sprintf(tmp, "%s %s", szItem, szBuffer);

    SCR_replace((unsigned char*) tmp, (unsigned char*) "\t", (unsigned char*) " ");
    B_DataUpdate(tmp, type);
    SCR_free(tmp);
    return(0);
}

/* Interface to EXCEL via DDE */
/* fn rapport
    $ExcelSetVar VARNAME [Sheet] [R1C1]
    $ExcelGetVar VARNAME [PeriodE] [Sheet] [R1C1]

    $ExcelExecute Open("c:\usr\iode\test.xls")
    $ExcelExecute Close

*/
char    *B_ExcelGetItem(char *arg)
{
    char    *ddeptr = NULL,
             **args = NULL;
    int     nb_args;

    args = (char**) SCR_vtom((unsigned char*) arg, (int) '!');
    nb_args = SCR_tbl_size((unsigned char**) args);

    if(nb_args == 2) ddeptr = (char*) WscrDdeGet("Excel", args[0], args[1]);
    IodeDdeUnLocale(ddeptr);

    SCR_free_tbl((unsigned char**) args);
    return(ddeptr);
}

int B_ExcelSetItem(char *ddeitem, char *ptr, int nc, int nl)
{
    char    **args = NULL, *topic, *item;
    int     nb_args, rc = -1;

    args = (char**) SCR_vtom((unsigned char*) ddeitem, (int) '!');
    nb_args = SCR_tbl_size((unsigned char**) args);

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
                    IodeDdeXlsCell(item, 0, 0, nc, nl), (unsigned char*) ptr);
    SCR_free_tbl((unsigned char**) args);
    return(rc);
}

int B_ExcelDecimal(char *arg, int unused)
{
    char    **args;
    int     nb_args, rc= 0;

    args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
    nb_args = SCR_tbl_size((unsigned char**) args);

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

int B_ExcelThousand(char *arg, int unused)
{
    char    **args;
    int     nb_args, rc = 0;

    args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
    nb_args = SCR_tbl_size((unsigned char**) args);

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
$ExcelSetCurrency Pound => skip £
$ExcelSetCurrency Euro => skip €
$ExcelSetCurrency c => skip c (any c)
$ExcelSetCurrency [no arg] => no sep
*/

int B_ExcelCurrency(char *arg, int unused)
{
    char    **args;
    int     nb_args, rc = 0;

    args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
    nb_args = SCR_tbl_size((unsigned char**) args);

    if(nb_args == 1) {
        switch(args[0][0]) {
            case 'd': /* dollar */
            case 'D':
                SCR_sCURRENCY = '$';
                break;

            case 'e': /* EUR */
            case 'E':
                SCR_sCURRENCY = '€';
                break;

            case 'p': /* Pound */
            case 'P':
                SCR_sCURRENCY = '£';
                break;

            default:
                SCR_sCURRENCY = args[0][0];
                break;
        }
    }
    else SCR_sCURRENCY = 0;

    return(rc);
}


int B_ExcelLang(char *arg, int unused)
{
    char    **args;
    int     nb_args, rc = 0;

    args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
    nb_args = SCR_tbl_size((unsigned char**) args);

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

    args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
    nb_args = SCR_tbl_size((unsigned char**) args);

    ddeptr = B_ExcelGetItem(args[nb_args - 1]);
    if(ddeptr == NULL) goto the_end;

    for(i = 0; i < nb_args - 1; i++) 
        SCR_add_ptr((unsigned char***) &tbl, &ntbl, (unsigned char*) args[i]);
    SCR_add_ptr((unsigned char***) &tbl, &ntbl, (unsigned char*) ddeptr);
    SCR_add_ptr((unsigned char***) &tbl, &ntbl, NULL);

    argptr = (char*) SCR_mtov((unsigned char**) tbl, (int) ' ');
    rc = B_DataUpdate(argptr, type);

the_end:
    SCR_free_tbl((unsigned char**) args);
    SCR_free_tbl((unsigned char**) tbl);
    SCR_free(argptr);
    SCR_free(ddeptr);
    return(rc);
}

int B_ExcelSet(char *arg, int type)
{
    int         pos, shift, rc = -1, 
                nb_args, nbr = 1, nc = 1, nl = 1;
    KDB         *kdb = get_global_db(type);
    Scalar         *scl;
    Period      *per = NULL;
    double      d;
    char        **args = NULL,
                *ptr = NULL,
                *item, *smpl;
    std::string name;
    std::string lec;

    args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
    nb_args = SCR_tbl_size((unsigned char**) args);

    name = std::string((char*) args[0]);
    if(name.empty()) 
        goto the_end;
    
    pos = kdb->index_of(name);

    item = args[nb_args - 1];
    switch(type) 
    {
        case COMMENTS :
            ptr = (char*) SCR_stracpy((unsigned char*) KCVAL(kdb, name));
            break;
        case IDENTITIES :
            ptr = (char*) SCR_stracpy((unsigned char*) KILEC(kdb, name));
            break;
        case LISTS :
            ptr = (char*) SCR_stracpy((unsigned char*) KLVAL(kdb, name));
            break;
        case EQUATIONS :
            lec = KELEC(kdb, name);
            ptr = (char*) SCR_stracpy((unsigned char*) lec.c_str());
            break;
        case SCALARS :
            scl = KSVAL(kdb, name);
            d = scl->value;        
            ptr = SCR_malloc(80);  
            IodeFmtVal(ptr, d);    
            break;                 

        case TABLES :
            if(nb_args == 3) smpl = args[1];
            else smpl = NULL;

            ptr = IodeDdeCreateTbl(pos, smpl, &nc, &nl, -1);
            break;

        case VARIABLES : /* Name Period nVal */
            per = new Period(std::string(args[1]));
            shift = per->difference(kdb->sample->start_period);
            if(per == NULL || shift< 0) 
                shift = 0;
            else 
                item = args[2];

            ptr = IodeDdeCreateSeries(pos, shift);
            nc = 1 + kdb->sample->nb_periods - shift; /* JMP 04-10-99 */
            break;
    }

    SCR_OemToAnsi((unsigned char*) ptr, (unsigned char*) ptr);
    IodeDdeLocale(ptr);
    rc = B_ExcelSetItem(item, ptr, nc, nl);

the_end:
    if(pos < 0)
        error_manager.append_error(std::string(args[0]) + " : not found");  /* JMP 10-08-00 */

    SCR_free_tbl((unsigned char**) args);
    delete per;
    per = nullptr;
    SCR_free(ptr);

    if(rc >= 0) 
        rc = 0;

    return(rc);
}

int B_ExcelExecute(char *arg, int unused)
{
    int     rc;
    char    *cmd;

    SCR_strip((unsigned char*) arg);
    cmd = SCR_malloc((int)strlen(arg)+2);
    sprintf(cmd, "[%s]", arg);
    rc = WscrDdeExecute("Excel", "System", (unsigned char*) cmd);

    SCR_free(cmd);
    return(rc);
}

int B_ExcelCmd(char *cmd, char *arg)
{
    int     rc;
    char    *xlscmd;

    SCR_strip((unsigned char*) cmd);
    if(arg) SCR_strip((unsigned char*) arg);
    if(arg != NULL && arg[0] != '\0') {
        xlscmd = SCR_malloc((int)strlen(cmd) + (int)strlen(arg) + 10);

        if(U_is_in('"', arg)) sprintf(xlscmd, "[%s(%s)]", cmd, arg);
        else                  sprintf(xlscmd, "[%s(\"%s\")]", cmd, arg);
    }
    else {
        xlscmd = SCR_malloc((int)strlen(cmd) + 2);
        sprintf(xlscmd, "[%s]", cmd);
    }

    rc = WscrDdeExecute("Excel", "System", (unsigned char*) xlscmd);

    SCR_free(xlscmd);
    return(rc);
}

int B_DDEGet(char *arg, int unused)
{
    U_ch    **tbl, *ddeptr;

    tbl = SCR_vtom((unsigned char*) arg, (int) ' ');
    if(SCR_tbl_size(tbl) < 3) return(-1);
//    rc = WscrDdeExecute(tbl[0], tbl[1], tbl[2]);
    ddeptr = WscrDdeGet((char*) tbl[0], (char*) tbl[1], (char*) tbl[2]);

    SCR_free_tbl(tbl);
    SCR_free(ddeptr);
    return(0);
}

/* JMP 03-08-2004 */
int B_ExcelWrite(char *ptr, int unused)
{
    char    **args = NULL, *topic, *item, *val, **tbl, **tbl2;
    int     i, nb_args, rc = -1, pos, nl = 1, nc = 1, nc1;


    U_ljust_text((unsigned char*) ptr);
    pos = U_pos(' ', (unsigned char*) ptr);
    if(pos < 0)
        val = "";
    else {
        ptr[pos] = 0;
        val = ptr + pos + 1;
        U_ljust_text((unsigned char*) val);
        SCR_replace((unsigned char*) val, (unsigned char*) "\\t", (unsigned char*) "\t");
        SCR_replace((unsigned char*) val, (unsigned char*) "\\n", (unsigned char*) "\n");
        tbl = (char**) SCR_vtom((unsigned char*) val, (int) '\n');
        nl = SCR_tbl_size((unsigned char**) tbl);
        for(i = 0 ; i < nl ; i++) {
            tbl2 = (char**) SCR_vtom((unsigned char*) tbl[i], (int) '\t');
            nc1 = SCR_tbl_size((unsigned char**) tbl2);
            if(nc1 > nc) nc = nc1;
            SCR_free_tbl((unsigned char**) tbl2);
        }
        SCR_free_tbl((unsigned char**) tbl);
    }

    args = (char**) SCR_vtom((unsigned char*) ptr, (int) '!');
    nb_args = SCR_tbl_size((unsigned char**) args);

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
                    IodeDdeXlsCell(item, 0, 0, nc, nl), (unsigned char*) val);
    SCR_free_tbl((unsigned char**) args);
    return(rc);
}

#else
int B_DDEGet(char *arg, int unused)
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

int B_ExcelExecute(char *arg, int unused)
{
    return(-1);
}

int B_ExcelCmd(char *cmd, char *arg)
{
    return(-1);
}

int B_ExcelWrite(char *ptr, int unused)
{
    return(-1);
}

#endif

int B_ExcelOpen(char *arg, int unused)
{
    return(B_ExcelCmd("OPEN", arg));
}

int B_ExcelClose(char *arg, int unused)
{
    return(B_ExcelCmd("CLOSE", NULL));
}

int B_ExcelPrint(char *arg, int unused)
{
    return(B_ExcelCmd("PRINT", NULL));
}

int B_ExcelSave(char *arg, int unused)
{
    return(B_ExcelCmd("SAVE", NULL));
}

int B_ExcelSaveAs(char *arg, int unused)
{
    return(B_ExcelCmd("SAVE.AS", arg));
}

int B_ExcelNew(char *arg, int unused)
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


int IodeFmtVal(char *buf, double val)
{
    if(IODE_IS_A_NUMBER(val)) {
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

