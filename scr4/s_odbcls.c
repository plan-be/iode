
#if defined(DOSW32) || defined(WINDOWS)

#include <scr4w.h>
#include "s_odbc.h"
#include "sqlvers.h"

extern char        OCharBuf[256];
extern SQLSMALLINT OShortBuf;
extern SQLINTEGER  OLongBuf;

/* ====================================================================
Retourne la liste de toutes les Data Sources disponibles sur le
système.

&RT tableau de pointeurs vers le nom des DSN ou NULL en cas d'erreur.

&EX
    char    **dsns;
    int     i;

    dsns = OListDSNs();
    if(dsns) {
	for(i = 0; dsns[i] ; i++) printf("%s\n", dsns[i]);
	SCR_free_tbl(dsns);
	}
    else {
	printf("Error reading DSN list\n");
	}
&TX

&SA OListTables()
=======================================================================*/

char **OListDSNs()
{
   SWORD     nDirection, nDSNlg, nSDlg;
   char      szDSN[256];
   char      szDescr[256];
   SQLRETURN nResult;
   char      **tbl = 0;
   int       nb = 0;

    //Call SQLDataSources, first with SQL_FETCH_FIRST and from then on
    //with SQL_FETCH_NEXT until it returns SQL_NO_DATA.
    nDirection = SQL_FETCH_FIRST;
    while(1) {
	nResult = SQLDataSources(OHenv, nDirection, szDSN, 255,
				  &nDSNlg, szDescr, 255,
				  &nSDlg);
	if(nResult == SQL_NO_DATA || nResult == SQL_ERROR) break;
	SCR_add_ptr(&tbl, &nb, szDSN);
	nDirection = SQL_FETCH_NEXT;
	}

    SCR_add_ptr(&tbl, &nb, 0L);
    return(tbl);
}

/* ====================================================================
Retourne la liste de toutes les tables d'une Data Source ouverte.

&RT tableau de pointeurs vers le nom des tables ou NULL en cas d'erreur.

&EX
    ODSN    *odsn;
    char    **tbls;
    int     i;

    ...
    tbls = OListTables(odsn);
    if(tbls) {
	for(i = 0; tbls[i] ; i++) printf("%s\n", tbls[i]);
	SCR_free_tbl(tbls);
	}
    else {
	printf("Error reading table list\n");
	}
&TX

&SA OListDSNs(), OOpenDSN(), OCloseDSN()
=======================================================================*/

char **OListTables(ODSN *odsn)
{
    SQLRETURN   retcode;
    char        **tbl = 0;
    int         nb = 0;

    // Allocate a new Statement Handle on the scanned hdbc
    if(odsn->hstmt == 0) {
	if((SQLAllocHandle(SQL_HANDLE_STMT, odsn->hdbc, &(odsn->hstmt))) != SQL_SUCCESS)
	    return((char **)0);
	}

    retcode = SQLTables(odsn->hstmt,
		     NULL, 0, /* All catalogs */
		     NULL, 0, /* All schemas     */
		     NULL, 0, /* All names */
		     "TABLE", SQL_NTS);             /* Only Tabbles */

    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) return((char **)0);

    /* Bind columns in result set to buffers */
//    SQLBindCol(odsn->hstmt, 3, SQL_C_CHAR, OCharBuf, 127, &OShortBuf);

    /* Fetch Result Set */
    while(1) {
	retcode = SQLFetch(odsn->hstmt);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) break;
	SQLGetData(odsn->hstmt, 3, SQL_C_CHAR, OCharBuf, 100, &OShortBuf);
	SCR_add_ptr(&tbl, &nb, OCharBuf);
	}

    SCR_add_ptr(&tbl, &nb, 0L);
//    SQLFreeStmt(odsn->hstmt, SQL_UNBIND);
//    SQLFreeStmt(odsn->hstmt, SQL_RESET_PARAMS);
    SQLFreeHandle(SQL_HANDLE_STMT, odsn->hstmt);
    odsn->hstmt = 0;
    return(tbl);
}



#endif















