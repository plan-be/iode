/**
 *  @header4iode
 *
 *  Implementation of the DATA stream interface.
 * 
 *  @see GB for details.
 *  
 *  List of functions 
 *  -----------------
 *      char *B_DSPeriod2Date(Period *per, char *date, char *freq)
 *      int B_DSLog(char *log)
 *      int B_DSTimedOut(char *name)
 *      long B_DSa2y(year)
 *      int B_DSDate2Period(Period *per, char *date, char freq)
 *      int B_DSCode(char *name)
 *      int B_DSUpdateCmt(char *name, char *val)
 *      int B_DSUpdateVar(char *name, Period *per, char *val)
 *      int B_DSInterpret(char *code, char freq, char *contents)
 *      int B_DSImportDb_1(char *arg, Sample *smpl)
 *      int B_DSImportDb(char *arg, int unused)
 */ 
#include "scr4/s_a2m.h"
#include "scr4/s_args.h"
#ifdef _MSC_VER
    #include "scr4/s_prowin.h"
#endif

#include "api/constants.h"
#include "api/b_args.h"
#include "api/k_super.h"
#include "api/objs/variables.h"

#include "api/report/commands/commands.h"
#include "api/report/undoc/undoc.h"


char    *B_DSPeriod2Date(Period *per, char *date, char *freq)
{
    char  year[8];

    if(per->year < 2000) sprintf(year, "%ld", per->year - 1900L);
    else {
        if(per->year < 2010) sprintf(year, "0%ld", per->year - 2000L);
        else                sprintf(year, "%ld", per->year - 2000L);
    }

    *freq = per->periodicity;
    switch(per->periodicity) {
        case 'Y':
            sprintf(date, "1/1/%s", year);
            break;
        case 'Q':
            sprintf(date, "1/%ld/%s", 3 * per->step, year);
            break;
        case 'M':
            sprintf(date, "1/%ld/%s", per->step, year);
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
    B_DataAppend(buf, LISTS);
    return(0);
}

long B_DSa2y(char* year)
{
    long    y;

    y = atol(year);
    if(y < 50) y += 2000;
    // else       y += 1900;
    if(y < 1900) y += 1900;  /* JMP 26-04-02 */
    return(y);
}

int B_DSDate2Period(Period *per, char *date, char freq)
{
    char    **tbl;

    per->periodicity = freq;
    per->step = 1L;
    per->year = 1970L;

    if(U_is_in('/', date)) { /* DD/MM/YY */
        tbl = (char**) SCR_vtom((unsigned char*) date, (int) '/');
        per->year = B_DSa2y(tbl[2]);
        switch(freq) {
            case 'Y' :
                break;
            case 'Q' :
                per->step = atol(tbl[1]) / 4;
                break;
            case 'M' :
                per->step = atol(tbl[1]);
                break;
        }
        SCR_free_tbl((unsigned char**) tbl);
        return(0);
    }

    if(U_is_in('Q', date)) {    /* "Q1 70" */
        tbl = (char**) SCR_vtom((unsigned char*) date, (int) ' ');
        per->year = B_DSa2y(tbl[1]);
        switch(freq) {
            case 'Y' :
                break;
            case 'Q' :
                per->step = atol(tbl[0] + 1);
                break;
            case 'M' :
                per->step = atol(tbl[0] + 1) * 3;
                break;
        }
        SCR_free_tbl((unsigned char**) tbl);
        return(0);
    }

    per->year = B_DSa2y(date);
    return(0);
}

int B_DSCode(char *name)
{
    int i, pfct;

    pfct = U_pos('(', (unsigned char*) name);
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
        rc = B_DataUpdate(update, COMMENTS);
        SCR_free(update);
    }
    return(rc);
}

int B_DSUpdateVar(char *name, Period *per, char *val)
{
    int     rc = -1;
    char    *update;

    update = SCR_malloc(K_MAX_NAME + 23 + (int)strlen(val)); /* IODE64K */
    if(update) {
        SCR_replace((unsigned char*) val, (unsigned char*) ",", (unsigned char*) ".");
        SCR_replace((unsigned char*) val, (unsigned char*) "#n/a", (unsigned char*) "na"); // Modif na au lieu de -- 27/08/2009

        sprintf(update, "%s %s %s", name, (char*) per->to_string().c_str(), val);
        rc = B_DataUpdate(update, VARIABLES);
        SCR_free(update);
    }
    return(rc);
}

int B_DSInterpret(char *code, char freq, char *contents)
{
    char    **line = NULL, **tbl = NULL;
    Period  stp;
    int     i, nl, rc = 0;

    line = (char**) SCR_vtom((unsigned char*) contents, (int) '\n');
    nl = SCR_tbl_size((unsigned char**) line);
    if(nl < 4) rc = -1;

    for(i = 0; i < nl - 1 && !rc; i++) {
        tbl = (char**) SCR_vtom((unsigned char*) line[i], (int) '"');
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

        SCR_free_tbl((unsigned char**) tbl);
        tbl = NULL;
    }

    SCR_free_tbl((unsigned char**) line);
    SCR_free_tbl((unsigned char**) tbl);

    if(rc < 0) B_DSLog(contents);
    return(rc);
}

/* DATA stream interface */
int B_DSImportDb_1(char *arg, Sample *smpl)
{
#ifdef WIN32
    char            freq;
    unsigned char   *ptr,
             request[81],
             st_date[20], nd_date[20];

    B_DSPeriod2Date(&(smpl->start_period), (char*) st_date, &freq);
    B_DSPeriod2Date(&(smpl->end_period), (char*) nd_date, &freq);

    sprintf((char*) request, "TRH>%s;;%s;%s;%c", arg, st_date, nd_date, freq);

    kmsg("DSDDE :%s ...", request);
    //if(SCR_hit_key() != 0) SCR_get_key();
    if(khitkey() != 0) kgetkey();               // JMP 11/12/2021
    
    ptr = WscrDdeGet("DSDDE", "TS", (char*) request);
    if(ptr != NULL) {
        B_DSInterpret(arg, freq, (char*) ptr);
        SCR_free(ptr);
    }
    else B_DSTimedOut(arg);

    return(0);
#else
    return(-1);
#endif
}

int wrapper_B_DSImportDb_1(char *arg, void *smpl)
{
    return B_DSImportDb_1(arg, (Sample*) smpl);
}

int B_DSImportDb(char *arg, int unused)
{
#ifdef WIN32
    Sample  *smpl;

    if(KV_WS->sample->nb_periods == 0) 
        return(-1);

    WscrDdeSetTimeOut(10000, 3);
    smpl = KV_WS->sample;
    return(B_ainit_loop(arg, wrapper_B_DSImportDb_1, (char *) smpl));
#else
    return(-1);
#endif
}
