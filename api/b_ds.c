#include "iode.h"

//#if defined(DOSW32) || defined(SCRW32)
//#include "scr4w.h"


char    *B_DSPeriod2Date(PERIOD *per, char *date, char *freq)
{
    char  year[8];

    if(per->p_y < 2000) sprintf(year, "%ld", per->p_y - 1900L);
    else {
        if(per->p_y < 2010) sprintf(year, "0%ld", per->p_y - 2000L);
        else                sprintf(year, "%ld", per->p_y - 2000L);
    }

    *freq = per->p_p;
    switch(per->p_p) {
        case 'Y':
            sprintf(date, "1/1/%s", year);
            break;
        case 'Q':
            sprintf(date, "1/%ld/%s", 3 * per->p_s, year);
            break;
        case 'M':
            sprintf(date, "1/%ld/%s", per->p_s, year);
            break;
    }

    return(date);
}

int B_DSLog(char *log)
{
    B_DSCode(log);
    Debug(log);
    Debug("\n");
    return(0);
}

int B_DSTimedOut(char *name)
{

    char    buf[81];

    sprintf(buf, "_DSTIMEO %s", name);
    B_DataAppend(buf, K_LST);
    return(0);
}

long B_DSa2y(year)
char    *year;
{
    long    y;

    y = atol(year);
    if(y < 50) y += 2000;
    // else       y += 1900;
    if(y < 1900) y += 1900;  /* JMP 26-04-02 */
    return(y);
}

int B_DSDate2Period(PERIOD *per, char *date, char freq)
{
    char    **tbl;
    extern  long    atol();

    per->p_p = freq;
    per->p_s = 1L;
    per->p_y = 1970L;

    if(U_is_in('/', date)) { /* DD/MM/YY */
        tbl = SCR_vtom(date, '/');
        per->p_y = B_DSa2y(tbl[2]);
        switch(freq) {
            case 'Y' :
                break;
            case 'Q' :
                per->p_s = atol(tbl[1])/4;
                break;
            case 'M' :
                per->p_s = atol(tbl[1]);
                break;
        }
        SCR_free_tbl(tbl);
        return(0);
    }

    if(U_is_in('Q', date)) {    /* "Q1 70" */
        tbl = SCR_vtom(date, ' ');
        per->p_y = B_DSa2y(tbl[1]);
        switch(freq) {
            case 'Y' :
                break;
            case 'Q' :
                per->p_s = atol(tbl[0] + 1);
                break;
            case 'M' :
                per->p_s = atol(tbl[0] + 1)* 3;
                break;
        }
        SCR_free_tbl(tbl);
        return(0);
    }

    per->p_y = B_DSa2y(date);
    return(0);
}

int B_DSCode(char *name)
{
    int i, pfct;

    pfct = U_pos('(', name);
    if(pfct > 0) {
        for(i = 0, pfct++; name[pfct] && name[pfct] != ')'; i++, pfct++)
            name[i] = name[pfct];
        name[i] = '\0';
    }

    for(i = 1 ; name[i] ; i++)
        if(!SCR_is_anum(name[i])) name[i] = '_';
    return(0);
}

int B_DSUpdateCmt(char *name, char *val)
{
    int     rc = -1;
    char    *update;

    update = SCR_malloc(K_MAX_NAME + 3 + (int)strlen(val)); /* IODE64K */
    if(update) {
        sprintf(update, "%s %s", name, val);
        rc = B_DataUpdate(update, K_CMT);
        SCR_free(update);
    }
    return(rc);
}

int B_DSUpdateVar(char *name, PERIOD *per, char *val)
{
    int     rc = -1;
    char    *update, period[11];

    update = SCR_malloc(K_MAX_NAME + 23 + (int)strlen(val)); /* IODE64K */
    if(update) {
        SCR_replace(val, ",", ".");
        SCR_replace(val, "#n/a", "na"); // Modif na au lieu de -- 27/08/2009

        sprintf(update, "%s %s %s", name, PER_pertoa(per, period), val);
        rc = B_DataUpdate(update, K_VAR);
        SCR_free(update);
    }
    return(rc);
}

int B_DSInterpret(char *code, char freq, char *contents)
{
    char    **line = NULL, **tbl = NULL;
    PERIOD  stp;
    int     i, nl, rc = 0;

    line = SCR_vtom(contents, '\n');
    nl = SCR_tbl_size(line);
    if(nl < 4) rc = -1;

    for(i = 0; i < nl - 1 && !rc; i++) {
        tbl = SCR_vtom(line[i], '"');
        switch(i) {
            case 0: /* Start */
                if(strcmp(tbl[0], "Start")) rc = -1;
                else B_DSDate2Period(&stp, tbl[2], freq);
                break;
            case 1: /* End */
                if(strcmp(tbl[0], "End")) rc = -1;
                break;
            case 2: /* Freq */
                if(strcmp(tbl[0], "Frequency")
                        || tbl[2][0] != freq) rc = -1;
                break;
            default: /* Cmt */
                B_DSCode(code);
                if(B_DSUpdateCmt(code, tbl[0]) < 0 ||
                        B_DSUpdateVar(code, &stp, tbl[1]) < 0) rc = -1;
                break;

        }

        SCR_free_tbl(tbl);
        tbl = NULL;
    }

    SCR_free_tbl(line);
    SCR_free_tbl(tbl);

    if(rc < 0) B_DSLog(contents);
    return(rc);
}

/* DATA stream interface */
int B_DSImportDb_1(char *arg, SAMPLE *smpl)
{
    char            freq;
    unsigned char   *ptr,
             request[81],
             st_date[20], nd_date[20];

    B_DSPeriod2Date(&(smpl->s_p1), st_date, &freq);
    B_DSPeriod2Date(&(smpl->s_p2), nd_date, &freq);

    sprintf(request, "TRH>%s;;%s;%s;%c", arg, st_date, nd_date, freq);

    kmsg("DSDDE :%s ...", request);
    //if(SCR_hit_key() != 0) SCR_get_key();
    if(khitkey() != 0) kgetkey();               // JMP 11/12/2021
    
    ptr = WscrDdeGet("DSDDE", "TS", request);
    if(ptr != NULL) {
        B_DSInterpret(arg, freq, ptr);
        SCR_free(ptr);
    }
    else B_DSTimedOut(arg);

    return(0);
}

int B_DSImportDb(char *arg)
{
    SAMPLE  *smpl;

    if(KSMPL(K_WS[K_VAR])->s_nb == 0) return(-1);

    WscrDdeSetTimeOut(10000, 3);
    smpl = KSMPL(K_WS[K_VAR]);
    return(B_ainit_loop(arg, B_DSImportDb_1, (char *)smpl));
}
//#endif