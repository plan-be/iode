/**
 *  @header4iode
 * 
 *  IODE report @-functions for ODBC interface
 *  ------------------------------------------
 *  See b_rep_fns.c for a detailed explanation on the @-functions usage.
 *  Each function in this group has the same syntax:
 *  
 *      U_ch* <function_name>(U_ch** arg)
 *  
 *      @param [in] U_ch**   arg    List of arguments of the function (sep = ',')
 *      @return     U_ch*           allocated string containing the computed value of the function
 *                                  NULL on error
 *
 *  List of functions
 *  -----------------
 *      U_ch *RPS_Open(U_ch** args)    | @SqlOpen(dsn[,user,password])
 *      U_ch *RPS_Field(U_ch** args)   | @SqlField(field_number)
 *      U_ch *RPS_NbFlds(U_ch** args)  | @SqlNbFlds() 
 *      U_ch *RPS_Record(U_ch** args)  | @SqlRecord([fld_nb1[,fld_nb2]]) : all fields between fld_nb1 and fld_nb2
 *      U_ch *RPS_Query(U_ch** args)   | @SqlQuery(sql_select)
 *      U_ch *RPS_Next(U_ch** args)    | @SqlNext()
 *      U_ch *RPS_Close(U_ch** args)   | @SqlClose()
 *  
 *  TODO: rename b_sql.c in b_rep_sql.c
 */
 
#include "api/report/undoc/undoc.h"

// These functions are NOT implemented (yet) under Linux or in VC64
#if defined(UNIX) || defined(_MSC_VER) 
// for IODE64 MSC => link with odbc32.$(L) odbccp32.$(L) 

U_ch    *RPS_Open(U_ch** args)
{
    return(SCR_stracpy("0"));
}


U_ch  *RPS_Field(U_ch** args)
{
    return(SCR_stracpy("0"));
}

U_ch  *RPS_NbFlds(U_ch** args)
{
    return(SCR_stracpy("0"));
}

U_ch  *RPS_Record(U_ch** args)
{
    return(SCR_stracpy("0"));
}


U_ch    *RPS_Query(U_ch** args)
{
    return(SCR_stracpy("0"));
}

U_ch    *RPS_Sql(U_ch** args)
{
    return(SCR_stracpy("0"));
}

U_ch    *RPS_Next(U_ch** args)
{
    return(SCR_stracpy("0"));
}

U_ch    *RPS_Close(U_ch** args)
{
    return(SCR_stracpy("1"));
}

//KDB  *K_load_odbc(int ftype, char *odbc, int no, char **objs)
//{
//    KDB     *kdb = NULL;
//    return(kdb);
//}


#else
// Embarcadero 32 bits
// ------------------- 

#include <s_odbc.h>
//#include <sqltypes.h>
#include "sql.h"
#include "sqlext.h"

ODSN    *odsn = NULL;       // Global ODBC data set name
OCSR    *ocsr = NULL;       // Global ODBC cursor

// @SqlOpen(dsn[,user,password])
U_ch    *RPS_Open(U_ch** args)
{
    int     nbargs;
    U_ch    dsn[80], user[80], pwd[80];

    dsn[0] = user[0] = pwd[0] = 0;
    if(args != NULL) {
        nbargs = SCR_tbl_size(args);
        if(nbargs > 0) strcpy(dsn, args[0]);
        if(nbargs > 1) strcpy(user, args[1]);
        if(nbargs > 2) strcpy(pwd, args[2]);
    }

    if(dsn[0] == 0) goto err;

    OInit();
    odsn = OOpenDSN(dsn, user, pwd);
    if(odsn == NULL) goto err;
    else return(SCR_stracpy("1"));

err:
    return(SCR_stracpy("0"));
}

/**
 *  Copy the value of the current record (after a query) into the buffer buf according to its type.
 *  Sub-function of RPS_Field().
 *  
 *  @param [out] buf     char*  output buffer
 *  @param [in]  ptr     char*  pointer to the field in the read record
 *  @param [in]  type    int    SQL type of the field
 *  @return              int     0
 */
static int RPS_GetFld(char* buf, char* ptr, int type)
{
    short   *sh;
    long    val;
    double  dbl;

    switch(type) {
        case SQL_C_CHAR :
            if(ptr) SCR_strlcpy(buf, ptr, 4095);  /* JMP 13-02-2013 */
            break;
        case SQL_C_TINYINT :
            val = ptr[0];
            sprintf(buf, "%ld", val);
            break;
        case SQL_C_SHORT :
            val = *(short *)ptr;
            sprintf(buf, "%ld", val);
            break;
        case SQL_C_LONG :
            val = *(long *)ptr;
            sprintf(buf, "%ld", val);
            break;
        case SQL_C_FLOAT :
            dbl = *(float *)ptr;
            sprintf(buf, "%lf", dbl);
            break;
        case SQL_C_DOUBLE :
            dbl = *(double *)ptr;
            sprintf(buf, "%lf", dbl);
            break;

        case SQL_C_TYPE_TIMESTAMP:
        case SQL_C_TYPE_DATE:
            sh = (short *)ptr;
            sprintf(buf, "%04d%02d%02d", sh[0], sh[1], sh[2]);
            break;

        case SQL_C_TYPE_TIME:
            sh = (short *)ptr;
            sprintf(buf, "%02d%02d%02d", sh[0], sh[1], sh[2]);
            break;

        default :
            sprintf(buf, "Unknown type");
            return(-1);
            break;
    }
    return(0);
}


// @SqlField(field_number)
U_ch  *RPS_Field(U_ch** args)
{
    int     fldnb = 0;
    U_ch    *res, buf[4096];
    char    *ptr;

    if(ocsr == NULL) return(SCR_stracpy("0"));

    if(args != NULL && args[0] != NULL) fldnb = atoi(args[0]);
    if(fldnb < 0 || fldnb > ocsr->otbl->ncols) return(SCR_stracpy("0"));

    ptr = OGetCol(ocsr, fldnb);
    RPS_GetFld(buf, ptr, ocsr->otbl->cols[fldnb].Ctype);
    res = SCR_stracpy(buf);
    return(res);
}


// @SqlNbFlds() 
U_ch  *RPS_NbFlds(U_ch** args)
{
    int     fldnb = 0;
    U_ch    *res, buf[4096];

    fldnb = ocsr->otbl->ncols;
    sprintf(buf, "%d", fldnb);
    res = SCR_stracpy(buf);
    return(res);
}

// @SqlRecord() : all fields 
// @SqlRecord(fld_nb) : all fields as from fld_nb
// @SqlRecord(fld_nb1,fld_nb2) : all fields between fld_nb1 and fld_nb2
U_ch  *RPS_Record(U_ch** args)
{
    char    *ptr;
    int     i, ntbl = 0, bt, et, nbargs;
    U_ch    *res, **tbl = NULL, buf[4096];

    if(ocsr == NULL) return(SCR_stracpy("0"));

    bt = 0;
    et = ocsr->otbl->ncols;

    if(args != NULL) {
        nbargs = SCR_tbl_size(args);
        if(nbargs > 0) bt = atoi(args[0]);
        if(nbargs > 1) et = atoi(args[1]);
    }

    et = min(et, ocsr->otbl->ncols);
    bt = min(bt, ocsr->otbl->ncols);

    for(i = bt ; i <  et; i++) {
        ptr = OGetCol(ocsr, i);
        RPS_GetFld(buf, ptr, ocsr->otbl->cols[i].Ctype);
        SCR_add_ptr(&tbl, &ntbl, buf);
    }

    SCR_add_ptr(&tbl, &ntbl, NULL);
    res = SCR_mtov(tbl, ' ');
    SCR_free_tbl(tbl);
    return(res);
}

// @SqlQuery(sql_select)
U_ch *RPS_Query(U_ch** args)
{
    U_ch    *arg;

    if(odsn == NULL) return(SCR_stracpy("0"));

    arg = SCR_mtov(args, ' ');

    if(ocsr != NULL) OFreeOCSR(ocsr);
    ocsr = OQuery(odsn, arg);
    SCR_free(arg);

    if(ocsr == NULL) return(SCR_stracpy("0"));
    else return(RPS_NbFlds(NULL));
}


// U_ch *RPS_Sql(U_ch** args)
// {
//     int     rc;
//     U_ch    *arg;
// 
//     if(odsn == NULL) return(SCR_stracpy("0"));
// 
//     arg = SCR_mtov(args, ' ');
//     rc = OSql(odsn, arg);
//     SCR_free(arg);
// 
//     if(rc < 0) return(SCR_stracpy("0"));
//     else return(SCR_stracpy("1"));
// }

// @SqlNext()
U_ch *RPS_Next(U_ch** args)
{
    int rc;

    if(ocsr == NULL) return(SCR_stracpy("0"));

    rc = ONext(ocsr);
    if(rc == 0) return(SCR_stracpy("1"));
    else return(SCR_stracpy("0"));
}


// @SqlClose()
U_ch *RPS_Close(U_ch** args)
{
    if(ocsr != NULL) OFreeOCSR(ocsr);
    if(odsn != NULL) OCloseDSN(odsn);

    ocsr = NULL;
    odsn = NULL;

    return(SCR_stracpy("1"));
}

/*
// OLD FUNCTIONS FOR A SPECIFIC PROJECT 

static int Str2Vec(double* vec, char* buf, int nb)
{
    int     i, nbtbl;
    char    **tbl;

    tbl = SCR_vtom(buf, ' ');

    nbtbl = SCR_tbl_size(tbl);
    for(i = 0; i < nb && i < nbtbl; i++) vec[i] = (double) atof(tbl[i]);
    for(; i < nb; i++) vec[i] = IODE_NAN;
    SCR_free_tbl(tbl);
}


KDB  *K_load_odbc(int ftype, char *odbc, int no, char **objs)
{
    int     i, nb, pos;
    char    *res, buf[1025],
            **tbl = NULL,
              *name, *values;
    KDB     *kdb = NULL, *tkdb = NULL;
    SAMPLE  *smpl = NULL;
    double    *vec = NULL;
    ODSN    *odsn = NULL;
    OCSR    *ocsr = NULL;
    extern  int ODebugDetail;


    ODebugDetail = 2;
    OInit();
    tbl = SCR_vtom(odbc, '!');
    if(tbl == NULL || SCR_tbl_size(tbl) < 2) goto err;

    odsn = OOpenDSN(tbl[0], "", "");
    if(odsn == NULL) goto err;

    kdb = K_create(ftype, UPPER_CASE);
    // Sample 
    sprintf(buf, "SELECT DISTINCT Begin, End FROM %s WHERE Project='RSZ'", tbl[1]);
    ocsr = OQuery(odsn, buf);
    if(ocsr == NULL) goto err;
    if(ONext(ocsr)) goto err;

    smpl = PER_atosmpl(OGetCol(ocsr, 0), OGetCol(ocsr, 1));
    if(smpl == 0 || smpl->s_nb <= 0) goto err; // JMP 25-05-92 
    if(KV_sample(kdb, smpl) < 0) goto err;
    memcpy((SAMPLE *) KDATA(kdb), smpl, sizeof(SAMPLE));
    nb = smpl->s_nb;
    vec = (double *) SW_nalloc(nb * sizeof(double));

    if(no == 0) {
        sprintf(buf, "SELECT Name, Values FROM %s WHERE Project='RSZ' AND Name Like '%%'", tbl[1]);
        ocsr = OQuery(odsn, buf);
        if(ocsr == NULL) goto err;

        while(1) {
            if(ONext(ocsr)) break;

            name = OGetCol(ocsr, 0);
            values = OGetCol(ocsr, 1);
            Str2Vec(vec, values, nb);
            pos = K_add(kdb, name, vec, &nb);
        }
    }

    else {
        // get data of the specified objects 
        no = SCR_tbl_size(objs);
        for(i = 0; i < no; i++) {
            sprintf(buf, "SELECT Name, Values FROM %s WHERE Project='RSZ' AND Name Like '%s'", tbl[1], objs[i]);
            ocsr = OQuery(odsn, buf);
            if(ocsr == NULL) continue;
            if(ONext(ocsr)) continue;

            Str2Vec(vec, OGetCol(ocsr, 1), nb);
            pos = K_add(kdb, OGetCol(ocsr, 0), vec, &nb);
            OFreeOCSR(ocsr);
            ocsr = NULL;
        }
    }

    if(ocsr != NULL) OFreeOCSR(ocsr);
    if(odsn != NULL) OCloseDSN(odsn);

err:
    SCR_free_tbl(tbl);
    SCR_free(smpl);
    SCR_free(vec);
    return(kdb);

}
*/
#endif // UNIX




