/**
 *  @header4iode
 * 
 *  IODE report @-functions for ODBC interface
 *  ------------------------------------------
 *  See b_rep_fns.c for a detailed explanation on the @-functions usage.
 *  Each function in this group has the same syntax:
 *  
 *      std::string <function_name>(const std::vector<std::string>& args)
 *
 *      @param [in] const std::vector<std::string>&  args   List of arguments of the function (sep = ',')
 *      @return     std::string                             computed value of the function
 *
 *  List of functions
 *  -----------------
 *      std::string RPS_Open(const std::vector<std::string>& args)    | @SqlOpen(dsn[,user,password])
 *      std::string RPS_Field(const std::vector<std::string>& args)   | @SqlField(field_number)
 *      std::string RPS_NbFlds(const std::vector<std::string>& args)  | @SqlNbFlds() 
 *      std::string RPS_Record(const std::vector<std::string>& args)  | @SqlRecord([fld_nb1[,fld_nb2]]) : all fields between fld_nb1 and fld_nb2
 *      std::string RPS_Query(const std::vector<std::string>& args)   | @SqlQuery(sql_select)
 *      std::string RPS_Next(const std::vector<std::string>& args)    | @SqlNext()
 *      std::string RPS_Close(const std::vector<std::string>& args)   | @SqlClose()
 *  
 *  TODO: rename b_sql.c in b_rep_sql.c
 */

#include "api/pch.h"
#include "api/report/undoc/undoc.h"

// These functions are NOT implemented (yet) under Linux or in VC64
#if defined(UNIX) || defined(_MSC_VER) 
// for IODE64 MSC => link with odbc32.$(L) odbccp32.$(L) 

std::string RPS_Open(const std::vector<std::string>& args)
{
    return "0";
}


std::string RPS_Field(const std::vector<std::string>& args)
{
    return "0";
}

std::string RPS_NbFlds(const std::vector<std::string>& args)
{
    return "0";
}

std::string RPS_Record(const std::vector<std::string>& args)
{
    return "0";
}


std::string RPS_Query(const std::vector<std::string>& args)
{
    return "0";
}

std::string RPS_Sql(const std::vector<std::string>& args)
{
    return "0";
}

std::string RPS_Next(const std::vector<std::string>& args)
{
    return "0";
}

std::string RPS_Close(const std::vector<std::string>& args)
{
    return "1";
}

//KDB  *K_load_odbc(int ftype, char *odbc, int no, char **objs)
//{
//    KDB     *kdb = NULL;
//    return(kdb);
//}


#else
// Embarcadero 32 bits
// ------------------- 

#include "api/iode_scr4.h"
#include "scr4/odbc/s_odbc.h"
//#include <sqltypes.h>
#include "sql.h"
#include "sqlext.h"

ODSN    *odsn = NULL;       // Global ODBC data set name
OCSR    *ocsr = NULL;       // Global ODBC cursor

// @SqlOpen(dsn[,user,password])
std::string RPS_Open(const std::vector<std::string>& args)
{
    std::string dsn  = (args.size() > 0) ? args[0] : "";
    std::string user = (args.size() > 1) ? args[1] : "";
    std::string pwd  = (args.size() > 2) ? args[2] : "";

    if(dsn.empty())
        return "0";

    OInit();
    odsn = OOpenDSN(dsn.data(), user.data(), pwd.data());
    return (odsn == NULL) ? "0" : "1";
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
            return -1;
            break;
    }
    return 0;
}


// @SqlField(field_number)
std::string RPS_Field(const std::vector<std::string>& args)
{
    int     fldnb = 0;
    char    buf[4096];
    char    *ptr;

    if(ocsr == NULL) return "0";

    if(args.size() > 0) fldnb = atoi(args[0].c_str());
    if(fldnb < 0 || fldnb > ocsr->otbl->ncols) return "0";

    buf[0] = 0;
    ptr = OGetCol(ocsr, fldnb);
    RPS_GetFld(buf, ptr, ocsr->otbl->cols[fldnb].Ctype);
    return std::string(buf);
}


// @SqlNbFlds() 
std::string RPS_NbFlds(const std::vector<std::string>& args)
{
    return std::to_string(ocsr->otbl->ncols);
}

// @SqlRecord() : all fields 
// @SqlRecord(fld_nb) : all fields as from fld_nb
// @SqlRecord(fld_nb1,fld_nb2) : all fields between fld_nb1 and fld_nb2
std::string RPS_Record(const std::vector<std::string>& args)
{
    char    *ptr;
    int     i, bt, et;
    char    buf[4096];
    std::vector<std::string> tbl;

    if(ocsr == NULL)
        return "0";

    bt = 0;
    et = ocsr->otbl->ncols;

    if(args.size() > 0) bt = atoi(args[0].c_str());
    if(args.size() > 1) et = atoi(args[1].c_str());

    et = min(et, ocsr->otbl->ncols);
    bt = min(bt, ocsr->otbl->ncols);

    for(i = bt ; i <  et; i++) 
    {
        buf[0] = 0;
        ptr = OGetCol(ocsr, i);
        RPS_GetFld(buf, ptr, ocsr->otbl->cols[i].Ctype);
        tbl.push_back(std::string((char*) buf));
    }

    return join(tbl, " ");
}

// @SqlQuery(sql_select)
std::string RPS_Query(const std::vector<std::string>& args)
{
    if(odsn == NULL) return "0";

    std::string arg = join(args, " ");

    if(ocsr != NULL) OFreeOCSR(ocsr);
    ocsr = OQuery(odsn, arg.data());

    if(ocsr == NULL) return "0";
    else return RPS_NbFlds({});
}


// std::string RPS_Sql(const std::vector<std::string>& args)
// {
//     int     rc;
//     U_ch    *arg;
// 
//     if(odsn == NULL) return "0";
// 
//     arg = SCR_mtov(args, ' ');
//     rc = OSql(odsn, arg);
//     SCR_free(arg);
// 
//     if(rc < 0) return "0";
//     else return "1";
// }

// @SqlNext()
std::string RPS_Next(const std::vector<std::string>& args)
{
    int rc;

    if(ocsr == NULL) return "0";

    rc = ONext(ocsr);
    if(rc == 0) return "1";
    else return "0";
}


// @SqlClose()
std::string RPS_Close(const std::vector<std::string>& args)
{
    if(ocsr != NULL) OFreeOCSR(ocsr);
    if(odsn != NULL) OCloseDSN(odsn);

    ocsr = NULL;
    odsn = NULL;

    return "1";
}

#endif // UNIX
