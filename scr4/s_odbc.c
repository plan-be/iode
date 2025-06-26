#if defined(DOSW32) || defined(WINDOWS)

#include <scr4w.h>
#include "s_odbc.h"
#include "sqlvers.h"

/*
#include "sql.h"
#include "sqlext.h"
#include "odbcinst.h"
*/

SQLHENV     OHenv = 0;
char        OCharBuf[256];
SQLSMALLINT OShortBuf;
SQLINTEGER  OLongBuf;
SQLUSMALLINT ORowStatusArray[2];
//extern int  ODebugDetail;
int     ODebugDetail = 0;   // JMP 08/12/2021 : moved from s_dbbvars.c
                            // JMP 15/03/2023 : remis dans s_dbvars.c sinon s_dbc ne le connait pas 

/* ====================================================================
Initialise une session ODBC. Cette fonction doit être appelée avant
toute utilisation des autres fonctions du groupe ODBC.
Elle peut être appelée plusieurs fois consécutivement.

&RT 0 en cas de succès, -1 en cas d'erreur.
&SA OEnd()
=======================================================================*/

OInit()
{
    // Allocate an Environment Handle
    if(OHenv) return(0);
    DebugB("ODBC environment initialisation...");
    if(SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE, &OHenv) != SQL_SUCCESS)
	goto err;

    if(SQLSetEnvAttr(OHenv, SQL_ATTR_ODBC_VERSION,
	    (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER) == SQL_SUCCESS) {
	DebugE("Ok");
	return(0);
	}

    // Free up all ODBC related resources
    SQLFreeHandle(SQL_HANDLE_ENV, OHenv);
    OHenv = 0;
err:
    DebugE("Failed");
    return(-1);
}


/* ====================================================================
Termine une session ODBC. Cette fonction doit être appelée pour clôturer
proprement l'utilisation d'ODBC.
Elle peut être appelée plusieurs fois consécutivement.

&RT 0 en cas de succès, -1 en cas d'erreur.
&SA OInit()
=======================================================================*/

OEnd()
{
   // Free up all ODBC related resources
   if(OHenv == 0) return(0);
   if(SQLFreeHandle(SQL_HANDLE_ENV,OHenv) == SQL_SUCCESS) {
	OHenv = 0;
	return(0);
	}
   return(-1);
}

/* ====================================================================
Ouvre une Data Source. Le nom de l'utilisateur et son mot de passe doivent
être passés s'ils sont définis. S'ils ne sont pas définis sur la
source szDBName, ces valeurs ne sont pas utilisées.

&EN char *szDBName : nom de la source (définie dans le gestionnaire ODBC)
&EN char *szUserName : login name
&EN char *szPassword : password

&RT Pointeur vers une structure ODSN qui contient les
    informations nécessaires à l'exploitation du DSN ou pointeur nul
    en cas d'erreur.

&EX
    ODSN    *odsn = 0;
    OCSR    *ocsr = 0;

    if(OInit()) {
	printf("Cannot initialize ODBC\n");
	return(-1);
	}
    odsn = OOpenDSN(dsname, user, pwd);
    if(odsn == 0) {
	printf("Error : Cannot open %s\n", ARG_VALUES[pos + 1]);
	}
    else {
	ocsr = OQuery(odsn, "SELECT * FROM Clients");
	if(ocsr == 0)
	    printf("Query failed : %s\n", OError());
	else {
	    while(ONext(ocsr) == 0) {
	    OPrintRow(ocsr);
	    }
	}
    OFreeOCSR(ocsr);
    OCloseDSN(odsn);
    OEnd();
&TX

&SA OCloseDSN(), OQuery(), OSql()
=======================================================================*/

ODSN *OOpenDSN(char *szDBName, char *szUserName, char *szPassword)
{
    SQLRETURN   nResult;                // Result code
    ODSN        *odsn;

    odsn = (ODSN *) SCR_malloc(sizeof(ODSN));

    // Allocate a new SQLHDBC. If successful then call SQLConnect using the
    // allocated hdbc and supplied connection information.
    if((nResult = SQLAllocHandle(SQL_HANDLE_DBC, OHenv, (SQLHDBC FAR *)&(odsn->hdbc))) != SQL_SUCCESS) {
	SCR_free(odsn);
	Debug("*** ODBC handle NOT created\n");
	return((ODSN *)0);
	}

    nResult = SQLConnect(odsn->hdbc, (LPSTR)szDBName, SQL_NTS,
				     (LPSTR)szUserName, SQL_NTS,
				     (LPSTR)szPassword, SQL_NTS);

    // if failed to connect, free the allocated hdbc before return
    if(nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO) {
	OSetErrorDb("SQLConnect: ", odsn->hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, odsn->hdbc);
	odsn->hdbc = 0;
	OCloseDSN(odsn);
	Debug("*** ODBC connection for %s failed\n", szDBName);
	return((ODSN *)0);
	}
    OGetInfos(odsn);
    odsn->dsname    = SCR_stracpy(szDBName);
    odsn->user      = SCR_stracpy(szUserName);
    odsn->passwd    = SCR_stracpy(szPassword);
    Debug("Connected to %s : OK\n", szDBName);
    return(odsn);
}

/* ====================================================================
Ferme une Data Source. odsn peut être nul.

&EN ODSN *odsn : pointeur vers une structure allouée par OOpenDSN().

&RT Pointeur vers une structure ODSN qui contient les
    informations nécessaires à l'exploitation du DSN ou pointeur nul
    en cas d'erreur.

&EX
    voir OOpenDSN()
&TX

&SA OOpenDSN()
=======================================================================*/

OCloseDSN(ODSN *odsn)
{
    int     i;

    if(odsn == 0 || odsn->hdbc == 0) return(0);
    if(odsn->hstmt) SQLFreeHandle(SQL_HANDLE_STMT, (SQLHSTMT)odsn->hstmt);
    for(i = 0 ; i < odsn->ncsrs; i++)
	if(odsn->hcsrs[i]) SQLFreeHandle(SQL_HANDLE_STMT, odsn->hcsrs[i]);
    SCR_free(odsn->hcsrs);

    if(odsn->hdbc) {
	SQLDisconnect(odsn->hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, odsn->hdbc);
	}

    Debug("DSN %s closed\n", odsn->dsname);
    SCR_free(odsn->dsname);
    SCR_free(odsn->user);
    SCR_free(odsn->passwd);
    SCR_free(odsn->dbms);
    SCR_free(odsn->dbmsvers);
    SCR_free(odsn->filename);
    SCR_free(odsn);
    return(0);
}

/* ====================================================================
Construit une structure contenant la définition d'une table.

&EN ODSN *odsn : pointeur vers un DSN retourné par OOpenDSN()
&EN char *tblname : nom de la table

&RT Pointeur vers une structure OTBL qui contient les
    informations sur les colonnes de la table tblname.
    Pointeur nul en cas d'erreur.

&EX
    ODSN    *odsn = 0;
    OTBL    *otbl = 0;
    int     k;

    if(OInit()) {
	printf("Cannot initialize ODBC\n");
	return(-1);
	}
    odsn = OOpenDSN(dsname, user, pwd);
    if(odsn == 0) {
	printf("Error : Cannot open %s\n", ARG_VALUES[pos + 1]);
	}
    else {
	otbl = OGetTableInfo(odsn, "MyTable");
	if(otbl == 0)
	    printf("Cannot read table MyTable: %s\n", OError());
	else {
	    for(k = 0 ; k < otbl->ncols ; k++)
		printf("%d : %s[%d]\n",
		    k, otbl->cols[k].typename,
		    otbl->cols[k].len);
	    OFreeOTBL(otbl);
	    }
	}
    OCloseDSN(odsn);
    OEnd();
&TX

&SA OCloseDSN(), OInit(), OEnd(), ODisplayTable()
=======================================================================*/

OTBL *OGetTableInfo(ODSN *odsn, char *tblname)
{
    SQLRETURN   retcode;
    OTBL        *otbl;
    int         i, pos;

    if(odsn == 0) return((OTBL *)0);
    DebugB("OGetTableInfo %s ...", tblname);
    // Allocate a new Statement Handle on the scanned hdbc
    if(odsn->hstmt == 0) {
	if((SQLAllocHandle(SQL_HANDLE_STMT, odsn->hdbc, &(odsn->hstmt))) != SQL_SUCCESS) {
	    DebugE("failed");
	    return((OTBL *)0);
	    }
	}

    retcode = SQLColumns(odsn->hstmt,
		     NULL, 0,              /* All catalogs */
		     NULL, 0,              /* All schemas     */
		     tblname, SQL_NTS,     /* tblname table */
		     NULL, 0);             /* All columns    */

    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
	OSetError("SQLColumns", odsn->hstmt);
	DebugE("SQLColumns failed");
	return((OTBL *)0);
	}

    otbl = (OTBL *) SCR_malloc(sizeof(OTBL));
    otbl->tbl_name = SCR_stracpy(tblname);
    otbl->odsn = odsn;

    /* Fetch Result Set */
    i = 0;
    while(1) {
	retcode = SQLFetch(odsn->hstmt);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) break;
	otbl->cols = (OCOL *) SCR_realloc(otbl->cols, sizeof(OCOL),
					  otbl->ncols, otbl->ncols + 1);

	SQLGetData(odsn->hstmt, 4, SQL_C_CHAR, OCharBuf, 100, &OLongBuf);
	otbl->cols[i].name = SCR_stracpy(OCharBuf);
	SQLGetData(odsn->hstmt, 5, SQL_C_SSHORT, &OShortBuf, sizeof(OShortBuf), NULL);
	otbl->cols[i].sqltype = OShortBuf;
	SQLGetData(odsn->hstmt, 6, SQL_C_CHAR, OCharBuf, 100, NULL);
	otbl->cols[i].typename = SCR_stracpy(OCharBuf);
	SQLGetData(odsn->hstmt, 7, SQL_C_SLONG, &OLongBuf,  sizeof(OLongBuf), NULL);
	otbl->cols[i].nc = OLongBuf;
	SQLGetData(odsn->hstmt, 8, SQL_C_SLONG, &OLongBuf,  sizeof(OLongBuf), NULL);
	otbl->cols[i].len = OLongBuf;
	SQLGetData(odsn->hstmt, 11, SQL_C_SSHORT, &OShortBuf,  sizeof(OShortBuf), NULL);
	otbl->cols[i].nullable = OShortBuf;
	SQLGetData(odsn->hstmt, 12, SQL_C_CHAR, OCharBuf, 100, NULL);
	otbl->cols[i].descr = SCR_stracpy(OCharBuf);
//        SQLGetData(odsn->hstmt, 13, SQL_C_CHAR, OCharBuf, 100, NULL); /* Plante avec MySQL */
//        otbl->cols[i].dft = SCR_stracpy(OCharBuf);
//        SQLGetData(odsn->hstmt, 18, SQL_C_CHAR, OCharBuf, 100, NULL); /* Plante avec MySQL */
//        otbl->cols[i].isnull = SCR_stracpy(OCharBuf);
	otbl->ncols++;
	OConvertTypeSQL2SCR(otbl->cols + i);
	otbl->ptrlen += otbl->cols[i].Clen;
	i++;
	}

    SQLFreeHandle(SQL_HANDLE_STMT, odsn->hstmt);
    odsn->hstmt = 0;

    if(otbl->ncols == 0) {
	OFreeOTBL(otbl);
	return((OTBL *)0);
	}

    /* Prepare les pointeurs directs vers les colonnes pour OWrite() */
    otbl->ptr = SCR_malloc(otbl->ptrlen + 1);
    for(i = pos = 0; i < otbl->ncols; i++) {
	otbl->cols[i].Cptr = otbl->ptr + pos;
	pos += otbl->cols[i].Clen;
	}
    DebugE("OK");
    return(otbl);
}

OIDX *OGetTableIndices(ODSN *odsn, char *tblname)
{
    SQLRETURN   retcode;
    OIDX        *oidx = 0;
    int         i, j, pos, nidx;

    if(odsn == 0) return((OTBL *)0);
    DebugB("OGetTableIndices %s ...", tblname);
    // Allocate a new Statement Handle on the scanned hdbc
    if(odsn->hstmt == 0) {
	if((SQLAllocHandle(SQL_HANDLE_STMT, odsn->hdbc, &(odsn->hstmt))) != SQL_SUCCESS) {
	    DebugE("failed");
	    return((OTBL **)0);
	    }
	}

    retcode = SQLStatistics(odsn->hstmt,
		     NULL, 0,              /* All catalogs */
		     NULL, 0,              /* All schemas     */
		     tblname, SQL_NTS,     /* tblname table */
		     SQL_INDEX_ALL, 0);    /* All indices */

    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
	OSetError("SQLStatistics", odsn->hstmt);
	DebugE("SQLStatistics failed");
	return((OTBL *)0);
	}

    /* Fetch Result Set - First Row describes next rows, thus skipped */
    retcode = SQLFetch(odsn->hstmt);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) goto fin;

    i = -1;
    j = 0;
    while(1) {
	retcode = SQLFetch(odsn->hstmt);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) break;
	SQLGetData(odsn->hstmt, 6, SQL_C_CHAR, OCharBuf, 100, &OLongBuf);
	if(i < 0 || strcmp(oidx[i].name, OCharBuf)) {
	    i++;
	    oidx = (OIDX *) SCR_realloc(oidx, sizeof(OIDX), i, i + 1);
	    oidx[i].ncols = j = 0;
	    oidx[i].name = SCR_stracpy(OCharBuf);
	    SQLGetData(odsn->hstmt, 4, SQL_C_SSHORT, &OShortBuf, sizeof(OShortBuf), NULL);
	    oidx[i].unique = OShortBuf;
	    }

	SQLGetData(odsn->hstmt, 9, SQL_C_CHAR, OCharBuf, 100, &OLongBuf);
	oidx[i].oflds[j].name = SCR_stracpy(OCharBuf);
	SQLGetData(odsn->hstmt, 10, SQL_C_CHAR, OCharBuf, 100, &OLongBuf);
	oidx[i].oflds[j].asc = OCharBuf[0];
	oidx[i].ncols++;
	j++;
	}

    if(i >= 0)
	oidx = (OIDX *) SCR_realloc(oidx, sizeof(OIDX), i + 1, i + 2);
fin:
    SQLFreeHandle(SQL_HANDLE_STMT, odsn->hstmt);
    odsn->hstmt = 0;

    DebugE("OK");
    return(oidx);
}

/* ====================================================================
Libère l'espace alloué pour la définition d'une table par OGetTableInfo().

&EN OTBL *otbl : pointeur vers un OTBL retourné par OGetTableInfo()

&RT 0

&EX
    voir OGetTableInfo()
&TX

&SA OGetTableInfo()
=======================================================================*/
OFreeOTBL(OTBL *otbl)
{
    int     i;

    if(otbl == 0) return(0);
    for(i = 0 ; i < otbl->ncols ; i++) {
	SCR_free(otbl->cols[i].name);
	SCR_free(otbl->cols[i].typename);
	SCR_free(otbl->cols[i].descr);
	SCR_free(otbl->cols[i].dft);
	SCR_free(otbl->cols[i].isnull);
	SCR_free(otbl->cols[i].Cvptr);
	}
    SCR_free(otbl->tbl_name);
    SCR_free(otbl->cols);
    if(otbl->whstmt) SQLFreeHandle(SQL_HANDLE_STMT, (SQLHSTMT)otbl->whstmt);
    SCR_free(otbl->ptr);
    SCR_free(otbl);
    return(0);
}

/* ====================================================================
Libère l'espace alloué pour la définition des index d'une table par OGetTableIndices().

&EN OIDX *oidx : pointeur vers un OIDX retourné par OGetTableIndices()

&RT 0

&EX
    voir OGetTableIndices()
&TX

&SA OGetTableInfo()
=======================================================================*/
OFreeOIDX(OIDX *oidx)
{
    int     i, j;

    if(oidx == 0) return(0);
    for(i = 0 ; oidx[i].name ; i++) {
	SCR_free(oidx[i].name);
	for(j = 0 ; j < oidx[i].ncols ; j++)
	    SCR_free(oidx[i].oflds[j].name);
	}
    SCR_free(oidx);
    return(0);
}

/* ====================================================================
Fonction identique à OQuery(), mais retourne optionnellement les noms des
champs de la requête dans la structure retournée (ocsr->otbl->ocol[i].name).

&EN ODSN *odsn : pointeur vers un DSN retourné par OOpenDSN()
&EN char *qury : requête SQL
&EN int withnames : 1 pour lire les noms des colonnes, 0 sinon (comme OQuery())

&RT Pointeur vers une structure OCSR qui contient les
    informations sur les colonnes de la table résultant de le
    requête, y compris le nom des colonnes. Pointeur nul en cas d'erreur.

&SA ONext(), OPrevious(), OSql(), OBindCol(), OGetCol(), OQuery()
=======================================================================*/


OCSR *OQueryWithNames(ODSN *odsn, char *qury, int withnames)
{
    OCSR        *ocsr;
    OTBL        *otbl;
    SQLRETURN   nReturn;
    SWORD       i, swColType, swColScale, swColNull, swColLength;
    UDWORD      udwColDef;
    long        dwColDef;
    char        szBuffer[128];
    int         len, pos;
    short       ncols;

    ocsr = (OCSR *) SCR_malloc(sizeof(OCSR));
    DebugB("\tQuery [%08ld] : %s ...", ocsr, qury);
    ocsr->odsn = odsn;
    ocsr->currec = 0;

    // Allocate a new Statement Handle on the scanned hdbc
    if((SQLAllocHandle(SQL_HANDLE_STMT, odsn->hdbc, &(ocsr->hstmt))) != SQL_SUCCESS) {
	SCR_free(ocsr);
	return((OCSR *)0);
	}
//    nReturn = SQLSetStmtAttr(ocsr->hstmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_STATIC, SQL_IS_INTEGER);
    nReturn = SQLSetStmtAttr(ocsr->hstmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC, SQL_IS_INTEGER);
//    nReturn = SQLSetStmtAttr(ocsr->hstmt, SQL_ATTR_CURSOR_SENSITIVITY, (SQLPOINTER)SQL_SENSITIVE, SQL_IS_INTEGER);
//    nReturn = SQLSetStmtAttr(ocsr->hstmt, SQL_ATTR_CURSOR_SCROLLABLE, (SQLPOINTER)SQL_SCROLLABLE, SQL_IS_INTEGER);
    if(nReturn) OSetError("SQLSetStmtAttr", ocsr->hstmt);
//    nReturn = SQLSetStmtAttr(ocsr->hstmt, SQL_ATTR_ROW_BIND_TYPE, sizeof(ORDERINFO), 0);
    nReturn = SQLSetStmtAttr(ocsr->hstmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)1, 0);
    nReturn = SQLSetStmtAttr(ocsr->hstmt, SQL_ATTR_ROW_STATUS_PTR, (SQLPOINTER)ORowStatusArray, 0);


    nReturn = SQLExecDirect(ocsr->hstmt, (SQLCHAR *)qury, SQL_NTS);
    if(nReturn != SQL_SUCCESS && nReturn != SQL_SUCCESS_WITH_INFO) {
	OSetError("Query", ocsr->hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, ocsr->hstmt);
	SCR_free(ocsr);
	DebugE("Failed : %s", OError()); /* JMP 23-11-99 */
	return((OCSR *)0);
	}

    SQLNumResultCols(ocsr->hstmt, &ncols);
    if(ncols == 0) {
	OSetError("Query empty", ocsr->hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, ocsr->hstmt);
	SCR_free(ocsr);
	DebugE("Empty");
	return((OCSR *)0);
	}
    otbl = ocsr->otbl = (OTBL *) SCR_malloc(sizeof(OTBL));
    otbl->cols = (OCOL *) SCR_malloc(sizeof(OCOL) * ncols);
    otbl->ncols = ncols;
    for(len = 0, i = 0; i < ncols; i++) {
	SQLDescribeCol(ocsr->hstmt, i + 1, szBuffer, 80,
		     &swColLength, &swColType, &udwColDef,
		     &swColScale, &swColNull);
	otbl->cols[i].len = udwColDef;
//        memcpy(&dwColDef, &udwColDef, sizeof(long));
	otbl->cols[i].sqltype = swColType;
	if(withnames) otbl->cols[i].name = SCR_stracpy(szBuffer); /* JMP 22-04-05 */
	OConvertTypeSQL2SCR(otbl->cols + i);
	len += otbl->cols[i].Clen;
	}

    otbl->ptrlen = len;
    ocsr->ptr = SCR_malloc(len + 1);

    for(i = pos = 0; i < ncols; i++) {
	otbl->cols[i].Cptr = ocsr->ptr + pos;
	if(OBindCol(ocsr, i, ocsr->ptr + pos)) {
	    OFreeOCSR(ocsr);
	    DebugE("Bind Error");
	    return((OCSR *)0);
	    }
	pos += otbl->cols[i].Clen;
	}

    DebugE("Ok");
    return(ocsr);
}

/* ====================================================================
Effectue une recherche (SELECT) et crée un "curseur" sur le
dataset résultat. Bien que n'importe quelle requête puisse être
exécutée par cette fonction, il est préférable de ne l'utiliser
que pour les SELECT. La fonction OSql() permet d'exécuter une
requête quelconque.

Le résultat est obtenu sous forme d'un dataset dans lequel on
peut se déplacer à l'aide de ONext() ou OPrevious() (selon le
driver, cette dernière fonction peut ne pas fonctionner).

Ce dataset est static. Il ne connaît donc pas les modifications
apportées par un autre utilisateur aux records de la base de
données.

Pour obtenir le pointeur sur la valeur d'une colonne, il suffit
d'appeler la fonction OGetCol(). Si on ignore le type de la
colonne, celui-ci peut être obtenu en consultant la structure
ocsr->otbl.

Aucun enregistrement n'est lu par la fonction OQuery(). Il faut
appeler ONext() pour que le premier enregistrement (s'il y en a
un) soit lu.

&EN ODSN *odsn : pointeur vers un DSN retourné par OOpenDSN()
&EN char *qury : requête SQL

&RT Pointeur vers une structure OCSR qui contient les
    informations sur les colonnes de la table résultant de le
    requête. Pointeur nul en cas d'erreur.

&EX
    ODSN    *odsn = 0;
    OCSR    *ocsr = 0;

    odsn = OOpenDSN(dsname, user, pwd);
    if(odsn == 0) {
	printf("Error : Cannot open %s\n", dsname);
	}
    else {
	ocsr = OQuery(odsn, "SELECT * from clients");
	if(ocsr == 0)
	    printf("Query failed : %s\n", OError());
	    else {
		while(ONext(ocsr) == 0) {
		    OPrintRow(ocsr);
		    }
		OFreeOCSR(ocsr);
		}
	    }
	OCloseDSN(odsn);
	}
&TX

&SA ONext(), OPrevious(), OSql(), OBindCol(), OGetCol()
=======================================================================*/


OCSR *OQuery(ODSN *odsn, char *qury)
{
    return(OQueryWithNames(odsn, qury, 0));
}

/* ====================================================================
Détermine l'adresse à laquelle les fonctions ONext() et
OPrevious() vont placer les valeurs des colonnes d'un dataset
résultant de OQuery().
Ces valeurs sont également utilisées par les fonctions OWrite()
et ORewrite().

&EN OCSR *ocsr : pointeur retourné par OQuery()
&EN int colnb : numéro de la colonne (indice 0) à modifier
&EN char *ptr : adresse où stocker la valeur

Une valeur par défaut est fixée par la fonction OQuery().

L'appel suivant à OGetCol() fournira le nouveau pointeur.

Dans le cas de champs de longueur variable (p.ex. LONGVARTEXT),
une valeur interne est conservée pour permettre une allocation à
chaque lecture du buffer. OBindCol() est donc inutile dans ce
cas.

&NO Le pointeur ptr doit être statique ou alloué. Il ne faut pas
    utiliser une variable locale. De plus, pour les champs de
    texte, l'espace doit être suffisant (y compris un caractère nul
    en fin de string).

&RT 0 en cas de succès, -1 en cas d'erreur. La fonction OError()
    peut être appelée pour obtenir un message d'erreur complet.

&EX
    static char szClientName[50];
    OCSR        *ocsr;

    ocsr = OQuery(odsn, "SELECT * from client");
    if(ocsr == 0) {
       printf("Bind:%s\n", OError());
       return(-1);
       }
    if(OBindCol(ocsr, 2, szClientName)) {
       printf("Bind:%s\n", OError());
       return(-1);
       }
    return(0);
&TX

&SA OQuery(), ONext(), OPrevious(), OGetCol()
=======================================================================*/

OBindCol(OCSR *ocsr, int colnb, char *ptr)
{
    SQLRETURN   nReturn;

    nReturn = SQLBindCol(ocsr->hstmt,
		    (UWORD)(colnb + 1),
		    ocsr->otbl->cols[colnb].Ctype,
		    ptr,
		    ocsr->otbl->cols[colnb].Clen,
		    &(ocsr->otbl->cols[colnb].datalen));
    if(nReturn == SQL_ERROR) {
	OSetError("Bind", ocsr->hstmt);
	return(-1);
	}
    return(0);
}

/* ====================================================================
Détermine l'adresse de la valeur de la colonne colnb du dataset
défini par ocsr. Les fonctions ONext() et OPrevious() vont placer
les valeurs des colonnes dans ces adresses. Ces adresses peuvent
être modifiées soit par la fonction OBindCol(), soit par le fait
qu'elles soient de longueur variable, auquel cas ONext() et
OPrevious() réallouent le buffer.

Dans le cas de colonnes de longueur variable, la valeur peut être
nulle.

&EN OCSR *ocsr : pointeur retourné par OQuery()
&EN int colnb : numéro de la colonne (indice 0) à modifier

&RT pointeur vers la valeur.

&EX
    static char szClientName[50];
    OCSR        *ocsr;

    ocsr = OQuery(odsn, "SELECT * from client");
    if(ocsr == 0) {
       printf("Bind:%s\n", OError());
       return(-1);
       }
    while(1) {
	if(ONext()) break;
	printf("Nom du client : %s\n", OGetCol(ocsr, 2));
	}
    OFreeOCSR(ocsr);
    return(0);
&TX

&SA OQuery(), ONext(), OPrevious(), OBindCol()
=======================================================================*/

char *OGetCol(OCSR *ocsr, int colnb)
{
    if(ocsr->otbl->ncols <= colnb || /* JMP 30-03-00 */
       ocsr->otbl->cols[colnb].datalen <= 0)
	    return((char *)0); /* JMP 25-10-99 */

    if(ocsr->otbl->cols[colnb].Cvar)
	return(ocsr->otbl->cols[colnb].Cvptr);
    else
	return(ocsr->otbl->cols[colnb].Cptr);
}

/* BP_M 19-10-2009 20:40 */
/* ==================================================================
    retourne 1 si le champs est de type MEMO
	     0 sinon
=======================================================================*/
OGetColVar(OCSR *ocsr, int colnb)
{
    if(ocsr->otbl->ncols <= colnb) return(0);
    return(ocsr->otbl->cols[colnb].Cvar);
}


/* BP_M 19-10-2009 20:40 */
/* ==================================================================
    Attribue un nouveau pointeur à un champs MEMO

=======================================================================*/
OSetPtrVCol(OCSR *ocsr, int colnb, char *ptr)
{
    if(ocsr->otbl->ncols <= colnb || /* JMP 30-03-00 */
       ocsr->otbl->cols[colnb].datalen <= 0)
	    return(0);

    if(ocsr->otbl->cols[colnb].Cvar) {
	SCR_free(ocsr->otbl->cols[colnb].Cvptr);
	ocsr->otbl->cols[colnb].Cvptr = ptr;
    }
}

OGetColLen(OCSR *ocsr, int colnb)
{
    if(ocsr->otbl->ncols <= colnb ||
       ocsr->otbl->cols[colnb].datalen <= 0)
	    return(0);

    return(ocsr->otbl->cols[colnb].datalen);
}

/*NH*/
OGetVCols(OCSR *ocsr)
{
    int         i;
    OCOL        *ocol;

    /* Alloue et lit les champs de longueur variable */
    for(i = 0, ocol = ocsr->otbl->cols ; i < ocsr->otbl->ncols ; i++, ocol++) {
	if(ocol->Cvar == 0) continue;
	SCR_free(ocol->Cvptr);
	ocol->Cvptr = 0;
	if(ocol->datalen > 0) {
	    ocol->Cvptr = SCR_malloc(ocol->datalen + 1);
	    SQLGetData(ocsr->hstmt, i + 1, ocol->Ctype,
			    ocol->Cvptr, ocol->datalen + 1, &OLongBuf);
	    }
	}

    return(0);
}

/* ====================================================================
Lit l'enregistrement suivant dans le dataset créé par la fonction
OQuery(). Les valeurs des colonnes sont adaptées au nouvel
enregistrement.

Pour obtenir le pointeur sur la valeur d'une colonne, il suffit
d'appeler la fonction OGetCol(). Si on ignore le type de la
colonne, celui-ci peut être obtenu en consultant la structure
ocsr->otbl.

&EN OCSR *ocsr : pointeur vers un OCSR retourné par OQuery()

&RT 0 en cas de succès, -1 en cas d'erreur. La fonction OError()
    peut être appelée pour obtenir un message d'erreur complet.

&EX
    voir OQuery().
&TX

&SA OQuery(), OPrevious(), OSql(), OBindCol(), OGetCol()
=======================================================================*/

ONext(OCSR *ocsr)
{
    return(ONext_(ocsr, 1));
}

/* ====================================================================
Lit l'enregistrement précédent dans le dataset créé par la fonction
OQuery(). Les valeurs des colonnes sont adaptées au nouvel
enregistrement.

Pour obtenir le pointeur sur la valeur d'une colonne, il suffit
d'appeler la fonction OGetCol(). Si on ignore le type de la
colonne, celui-ci peut être obtenu en consultant la structure
ocsr->otbl.

&EN OCSR *ocsr : pointeur vers un OCSR retourné par OQuery()

&RT 0 en cas de succès, -1 en cas d'erreur. La fonction OError()
    peut être appelée pour obtenir un message d'erreur complet.

&EX
    voir OQuery().
&TX

&SA OQuery(), ONext(), OSql(), OBindCol(), OGetCol()
=======================================================================*/

OPrevious(OCSR *ocsr)
{
    return(OPrevious_(ocsr, 1));
}


/* ====================================================================
Libère le "curseur" alloué par OQuery().

&EN OCSR *ocsr : pointeur vers un OCSR retourné par OQuery()

&RT 0 en cas de succès, -1 si ocstr est nul.

&EX
    voir OQuery().
&TX

&SA OQuery(), ONext(), OPrevious()
=======================================================================*/

OFreeOCSR(OCSR *ocsr)
{
    int     i;

    if(ocsr == 0) return(-1);
    if(ODebugDetail > 1) Debug("\tQuery [%08ld] : closed\n", ocsr);
    if(ocsr->hstmt) {
	SQLCloseCursor(ocsr->hstmt); /* JMP 08-10-99 */
	for(i = 0 ; i < ocsr->otbl->ncols ; i++)
	    OBindCol(ocsr, i, (char *)0);
	/*nReturn = SQLBindCol(ocsr->hstmt,
		    (UWORD)(colnb + 1),
		    ocsr->otbl->cols[colnb].Ctype,
		    ptr,
		    ocsr->otbl->cols[colnb].Clen,
		    &(ocsr->otbl->cols[colnb].datalen));
	*/

	//SQLFreeStmt(ocsr->hstmt, SQL_CLOSE);
	//SQLFreeStmt(ocsr->hstmt, SQL_UNBIND);
	//SQLFreeStmt(ocsr->hstmt, SQL_RESET_PARAMS);
	SQLFreeHandle(SQL_HANDLE_STMT, ocsr->hstmt);
	}
    SCR_free(ocsr->ptr);
    OFreeOTBL(ocsr->otbl);
    SCR_free(ocsr);
    return(0);
}

/* ====================================================================
Exécute la commande SQL sql.

&EN ODSN *odsn : pointeur vers un Data Source ouvert par OOpenDSN()
&EN char *sql : commande sql

&RT 0 en cas de succès, -1 en cas d'erreur. La fonction OError()
    peut être appelée pour obtenir un message d'erreur complet.

&EX
    ODSN    *odsn = 0;

    odsn = OOpenDSN(dsname, user, pwd);
    if(odsn == 0) {
	printf("Error : Cannot open %s\n", dsname);
	}
    else {
	if(OSql(odsn, "CREATE TABLE test (Topic CHAR(12),TopicId INTEGER(5))"))
	    printf("OSql : %s\n", OError());
	else
	    printf("OSql : Ok\n");
	OCloseDSN(odsn);
	}
&TX

&SA OQuery()
=======================================================================*/

OSql(ODSN *odsn, char *sql)
{
    SQLRETURN   nReturn;
    SQLHSTMT    hstmt;

    DebugB("\tSQLDirect :        %s ...", sql);
    // Allocate a new Statement Handle on the scanned hdbc
    if((SQLAllocHandle(SQL_HANDLE_STMT, odsn->hdbc, &hstmt)) != SQL_SUCCESS) {
	DebugE("Failed");
	return(-1);
	}

    nReturn = SQLExecDirect(hstmt, (SQLCHAR *)sql, SQL_NTS);
    if(nReturn != SQL_SUCCESS && nReturn != SQL_SUCCESS_WITH_INFO) {
	OSetError("OSql", hstmt);
	DebugE("Failed");
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return(-1);
	}
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    DebugE("Ok");
    return(0);
}

static char  OLASTERROR[SQL_SQLSTATE_SIZE+SQL_MAX_MESSAGE_LENGTH+50];

/* ====================================================================
Retourne l'adresse d'un buffer statique contenant le dernier
message d'erreur ODBC.

&RT adresse du buffer des messages d'erreur.

&EX
    voir OQuery().
&TX

&SA OQuery()
=======================================================================*/

char *OError()
{
    return(OLASTERROR);
}

/*NH*/
char *OSetErrorEx(char *msg, SQLHANDLE hstmt, SQLSMALLINT type)
{
    UCHAR        szErrState[SQL_SQLSTATE_SIZE+1];        // SQL Error State string
    UCHAR        szErrText[SQL_MAX_MESSAGE_LENGTH+1];    // SQL Error Text string
    SWORD        wErrMsgLen;             // Error message length
    UDWORD       dwErrCode;              // Native Error code
    SQLRETURN    nErrResult;             // Return Code from SQLGetDiagRec

    strcpy(OLASTERROR, msg);

    nErrResult = SQLGetDiagRec(type, hstmt, 1,
			      szErrState, &dwErrCode,
			      szErrText,  SQL_MAX_MESSAGE_LENGTH-1,
			      &wErrMsgLen);

    if(nErrResult == SQL_ERROR || nErrResult == SQL_INVALID_HANDLE) {
	sprintf(OLASTERROR, "%s : Empty Error Information", msg);
	return(OLASTERROR);
	}

    if(nErrResult == SQL_NO_DATA) {
	sprintf(OLASTERROR, "%s : Record not found", msg);
	return(OLASTERROR);
	}

/*   sprintf(OLASTERROR, "%s:  SQL Error State:%s, Native Error Code: %lX, ODBC Error: %s",
	msg, (LPSTR)szErrState, dwErrCode, (LPSTR)szErrText);
*/
    SCR_AnsiToOem(szErrText, szErrText);
    sprintf(OLASTERROR, "%s:  %s [Error State:%s]",
	    msg, (LPSTR)szErrText, (LPSTR)szErrState);
    return(OLASTERROR);
}

/*NH*/
char *OSetError(char *msg, SQLHANDLE hstmt)
{
    return(OSetErrorEx(msg, hstmt, SQL_HANDLE_STMT));
}

/*NH*/
char *OSetErrorDb(char *msg, SQLHANDLE hstmt)
{
    return(OSetErrorEx(msg, hstmt, SQL_HANDLE_DBC));
}

/*NH*/
OConvertTypeSQL2SCR(OCOL *ocol)
{
    switch(ocol->sqltype) {
	case SQL_WCHAR:
	case SQL_CHAR:
	    ocol->scrtype = SCR_STRING;
	    ocol->Ctype = SQL_C_CHAR;
	    ocol->Clen = ocol->len + 1;
	    break;
	case SQL_WVARCHAR:
	case SQL_VARCHAR:
	    ocol->scrtype = SCR_ZSTRING;
	    ocol->Ctype = SQL_C_CHAR;
	    if(ocol->len > 100000L) goto undef; /* JMP 29-10-01 */
	    ocol->Clen = ocol->len + 1;
	    break;
	case SQL_DECIMAL:
	case SQL_NUMERIC:
	    ocol->scrtype = SCR_ZSTRING;
	    ocol->Ctype = SQL_C_DOUBLE;
	    ocol->Clen = 8;
	    break;
	case SQL_SMALLINT:
	    ocol->scrtype = SCR_SHORT;
	    ocol->Ctype = SQL_C_SHORT;
	    ocol->Clen = 2;
	    break;
	case SQL_BIGINT:        /* JMP 21-09-10 tentative pour docuweb badges */
	case SQL_INTEGER:
	    ocol->scrtype = SCR_LONG;
	    ocol->Ctype = SQL_C_LONG;
	    ocol->Clen = 4;
	    break;
	case SQL_FLOAT:
	case SQL_REAL:
	    ocol->scrtype = SCR_FLOAT;
	    ocol->Ctype = SQL_C_FLOAT;
	    ocol->Clen = 4;
	    break;
	case SQL_DOUBLE:
	    ocol->scrtype = SCR_DOUBLE;
	    ocol->Ctype = SQL_C_DOUBLE;
	    ocol->Clen = 8;
	    break;
	case SQL_BIT:
	case SQL_TINYINT:
	    ocol->scrtype = SCR_CHAR;
	    ocol->Ctype = SQL_C_TINYINT;
	    ocol->Clen = 1;
	    break;
	case SQL_TIME:
	    ocol->scrtype = SCR_TIME;
	    ocol->Ctype = SQL_C_CHAR;
	    ocol->Clen = 12;
	    break;
	case SQL_DATE:
	    ocol->scrtype = SCR_DATE;
	    ocol->Ctype = SQL_C_CHAR;
	    ocol->Clen = 12;
	    break;

	case SQL_LONGVARCHAR:
undef: /* JMP 29-10-01 */
	    ocol->scrtype = SCR_EDITOR;
	    ocol->Ctype = SQL_C_CHAR;
	    ocol->Clen = 30;
	    ocol->Cvar = 1;
	    break;

	case SQL_TIMESTAMP:
	case SQL_TYPE_TIMESTAMP:
	    ocol->scrtype = SCR_DATE;
	    ocol->Ctype = SQL_C_TYPE_TIMESTAMP;
	    ocol->Clen = 16;
	    break;

	case SQL_TYPE_DATE:
	    ocol->scrtype = SCR_DATE;
	    ocol->Ctype = SQL_C_TYPE_DATE;
	    ocol->Clen = 6;
	    break;

	case SQL_TYPE_TIME:
	    ocol->scrtype = SCR_TIME;
	    ocol->Ctype = SQL_C_TYPE_TIME;
	    ocol->Clen = 6;
	    break;

	/* TYPE NON SUPPORTES */
	case SQL_INTERVAL_DAY :
	case SQL_INTERVAL_DAY_TO_MINUTE   :
	case SQL_INTERVAL_HOUR            :
	case SQL_INTERVAL_DAY_TO_SECOND   :
	case SQL_INTERVAL_MINUTE          :
	case SQL_INTERVAL_HOUR_TO_MINUTE  :
	case SQL_INTERVAL_SECOND          :
	case SQL_INTERVAL_HOUR_TO_SECOND  :
	case SQL_INTERVAL_DAY_TO_HOUR     :
	case SQL_INTERVAL_MINUTE_TO_SECOND:
	case SQL_WLONGVARCHAR:
	case SQL_BINARY:        /* NOT SUPPORTED */
	case SQL_VARBINARY:     /* NOT SUPPORTED */
	case SQL_LONGVARBINARY: /* NOT SUPPORTED */
//        case SQL_BIGINT:        /* NOT SUPPORTED */
	default :
	    ocol->scrtype = SCR_ZSTRING;
	    ocol->Ctype = SQL_C_DEFAULT;
	    ocol->Clen = 30;
	    break;
	}
    return(0);
}

/* ====================================================================
Exécute les commandes SQL regroupées dans un fichier ASCII.
Une commande peut être spécifiée par ligne.
Si la ligne commence par le caractère '+', il s'agit d'un SELECT et
le résultat de la recherche est affichée dans le standard output.

&EN char *filename : nom du fichier de commandes
&EN char *dsn : nom de la Data Source
&EN char *user : login name
&EN char *pwd : mot de passe éventuel
&EN int Verbose : 1 pour un affichage des messages pour chaque ligne


&RT 0 en cas de succès pour toutes les commandes, -1 en cas d'erreur

&EX
    voir OQuery().
&TX

&SA OQuery()
=======================================================================*/

OExecSQLFile(char *filename, char *dsn, char *user, char *pwd, int Verbose)
{
    FILE    *fd;
    ODSN    *odsn;
    OCSR    *ocsr;
    char    buf[10240], *stmt = 0;
    int     rc = 0, i;
    long    linenb = 0;

    fd = fopen(filename, "r");
    if(fd == 0) return(-1);

    odsn = OOpenDSN(dsn, user, pwd);
    if(odsn == 0) return(-2);

    while(1) {
	SCR_free(stmt);
	stmt = 0;
	while(1) {
	    if(SCR_read_line(fd, buf, 10239) < 0) goto fin;
	    linenb++;
	    //SCR_replace(buf, "\t", " ");
	    //U_ljust_text(buf);
	    SCR_strip(buf);
	    for(i = 0 ; buf[i] ; i++)
		if(buf[i] != ' ') break;
	    if(buf[i] == 0 || buf[i] == '#') continue;
	    if(stmt) stmt = SCR_strafcat(stmt, "\n");
	    stmt = SCR_strafcat(stmt, buf);
	    if(stmt[strlen(stmt) - 1] == ';') break;
	    }
	if(Verbose) Wprintf("%s[%ld]: %s ", filename, linenb, stmt);

	if(stmt[0] == '+') {
	    ocsr = OQuery(odsn, stmt + 1);
	    if(ocsr == 0) {
		rc = -1;
		OFreeOCSR(ocsr);
		if(Verbose) Wprintf("\n ***** %s\n\n", OError());
		}
	    else {
		if(Verbose) Wprintf("\n---------------------------------------------\n");
		while(ONext(ocsr) == 0) {
		    OPrintRow(ocsr);
		    }
		if(Verbose) Wprintf("---------------------------------------------\n");
		OFreeOCSR(ocsr);
		}
	    }
	else {
	    if(OSql(odsn, stmt)) {
		if(Verbose) Wprintf("\n ***** %s\n\n", OError());
		rc = -1;
		}
	    else
		if(Verbose) Wprintf(" Ok\n");
	    }
	}
fin:
    OCloseDSN(odsn);
    fclose(fd);
    return(rc);
}

/*NH*/
OGetInfos(ODSN *odsn)
{
    short           lg;
    char            buf[256];
    unsigned long   bits;

    if(odsn == 0) return(-1);
    if(SQLGetInfo(odsn->hdbc, SQL_DBMS_NAME, buf, 255, &lg) == 0)
	odsn->dbms = SCR_stracpy(buf);

    if(SQLGetInfo(odsn->hdbc, SQL_DBMS_VER, buf, 255, &lg) == 0)
	odsn->dbmsvers = SCR_stracpy(buf);

    if(SQLGetInfo(odsn->hdbc, SQL_DATABASE_NAME, buf, 255, &lg) == 0)
	odsn->filename = SCR_stracpy(buf);

    if(SQLGetInfo(odsn->hdbc, SQL_STATIC_CURSOR_ATTRIBUTES1, &bits, 4, &lg) == 0)
	odsn->prev[0] = bits & SQL_CA1_NEXT;

    if(SQLGetInfo(odsn->hdbc, SQL_DYNAMIC_CURSOR_ATTRIBUTES1, &bits, 4, &lg) == 0)
	odsn->prev[1] = bits & SQL_CA1_NEXT;

    return(0);
}

/* ====================================================================
Retourne la commande SQL tranformée par le driver avant l'envoi
au serveur. Cette fonction ne doit pas être utilisée avant
l'exécution d'une commande SQL. Elle sert uniquement à des fins
de debugging.

&RT 0 en cas de succès, -1 en cas d'erreur
=======================================================================*/

OSqlNative(ODSN *odsn, char *sqlin, char *sqlout, int maxlg)
{
    SQLINTEGER  lg;

    if(SQLNativeSql(odsn->hdbc, sqlin, strlen(sqlin), sqlout, maxlg, &lg))
	return(-1);
    return(0);
}


/* ====================================================================
Ajoute un DSN système dans le gestionnaire ODBC pour un fichier
ACCESS.

&EN char *dsn : nom du DSN système
&EN char *descr : description du DSN
&EN char *file : fichier mdb définissant le DSN

&CO
    //Fixer le nom du DSN ODBC
    strcpy(SCR_db_name, "my_db");
    // Tentative d'ouverture
    if(IS_init_database()) {
	// Ouverture my_db impossible, donc essai d'installation
	OAddMsAccessDSN("my_db",
			"Base de données test",
			"c:\\usr\\my_app\\my_db.mdb");
	// Retest d'ouverture
	if(IS_init_database()) {
	    // Abandon, l'obstacle est trop haut
	    Debug("Impossible d'ouvrir et/ou d'installer my_db");
	    SCR_end();
	    exit(-1);
	    }
	}
&TX

&RT -1 en cas d'erreur, 0 en cas de succès
    Avant 4.56 : le code de retour est toujours 1 (code d'erreur) bien que la fonction
    fonctionne correctement
=======================================================================*/

OAddMSAccessDSN(char *dsn, char *descr, char *file)
{
    char    txt[512];
    int     rc, i;

    sprintf(txt, "DSN=%s|DBQ=%s|Description=%s|", dsn, file, descr);
    for(i = 0 ; txt[i] ; i++) if(txt[i] == '|') txt[i] = 0;
    rc = SQLConfigDataSource((HWND)0, ODBC_ADD_SYS_DSN, "Microsoft Access Driver (*.mdb)", txt);
    if(rc == FALSE) return(-1);
    return(0);
}


/*NH*/
OWrite(OTBL *otbl)
{
    SQLRETURN   retcode;
    char        *buf = 0;
    int         i;

    if(otbl->whstmt == 0) {
	if((SQLAllocHandle(SQL_HANDLE_STMT, otbl->odsn->hdbc,
			    &(otbl->whstmt))) != SQL_SUCCESS) {
	    return(-1);
	    }

	buf = SCR_malloc(100 + otbl->ncols * 3);
	sprintf(buf, "INSERT INTO %s VALUES (?", otbl->tbl_name);
	for(i = 1 ; i < otbl->ncols ; i++) strcat(buf, ", ?");
	strcat(buf, ")");

	retcode = SQLPrepare(otbl->whstmt, buf, SQL_NTS);
	SCR_free(buf);
	if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
	    SQLFreeHandle(SQL_HANDLE_STMT, (SQLHSTMT)otbl->whstmt);
	    otbl->whstmt = 0;
	    return(-1);
	    }

	for(i = 0 ; i < otbl->ncols ; i++) {
	    otbl->cols[i].datalen = otbl->cols[i].len;
	    retcode = SQLBindParameter(otbl->whstmt, i + 1,
				       SQL_PARAM_INPUT,
				       otbl->cols[i].Ctype,
				       otbl->cols[i].sqltype,
				       otbl->cols[i].len,
				       0,
				       otbl->cols[i].Cptr,
				       0,
				       &(otbl->cols[i].datalen));
	    if(retcode == SQL_ERROR) {
		OSetError("Bind(OWrite)", otbl->whstmt);
		return(-2);
		}
	    }
	}

    retcode = SQLExecute(otbl->whstmt);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
	OSetError("SQLExecute", otbl->whstmt);
	return(-1);
	}
    return(0);
}


/*NH*/
char *OGetOtblCol(OTBL *otbl, int colnb)
{
    if(otbl->cols[colnb].Cvar)
	return(otbl->cols[colnb].Cvptr);
    else
	return(otbl->cols[colnb].Cptr);
}

/* ====================================================================
Retourne la valeur maximum du champ fldname de type entier de la table
tblname satisfaisant à la WHERE CLAUSE where.

&EN ODSN *odsn : Data Source
&EN char *tblname : nom de la table
&EN char *fldname : nom du champ (entier long)
&EN char *where : WHERE CLAUSE

Cette fonction utilise SELECT MAX() pour obtenir l'information. Tous les
systèmes ne supportent pas cette fonction scalaire.

&EX
    printf("Nombre de clients avec impayé : %ld\n",
	    OGetCountWhere(MyDSN, "mytable", "impaye > 0"));
&TX

&RT le nombre d'enregistrements ou -1L en cas d'erreur
=======================================================================*/

long OGetMaxValue(ODSN *odsn, char *tblname, char *fldname, char *where)
{
    OCSR    *ocsr;
    char    buf[1024];
    long    total = -1L;
    char    *ptr;

    if(where == 0) where = "";
    sprintf(buf, "SELECT MAX(%s) from %s %s", fldname, tblname, where);
    ocsr = OQuery(odsn, buf);
    if(ocsr == 0) return(-1L);
    if(ONext(ocsr) == 0) {
	ptr = OGetCol(ocsr, 0);
	if(ptr)
	    memcpy(&total, ptr, sizeof(long));
	}

    OFreeOCSR(ocsr);
    return(total);
}
/* ====================================================================
Retourne le nombre de records satisfaisant à la WHERE CLAUSE where dans
la table tblname.

&EN ODSN *odsn : Data Source
&EN char *tblname : nom de la table
&EN char *where : WHERE CLAUSE

Cette fonction utilise SELECT COUNT(*) pour obtenir
l'information. Tous les systèmes ne supportent pas cette fonction
scalaire.

&EX
    printf("Nombre de clients avec impayé : %ld\n",
	    OGetCountWhere(MyDSN, "mytable", "impaye > 0"));
&TX

&RT le nombre d'enregistrements ou -1L en cas d'erreur
=======================================================================*/
/*
long OGetCountWhere(ODSN *odsn, char *tblname, char *where)
{
    OCSR    *ocsr;
    char    buf[1024];
    long    total = -1L;
    char    *ptr;

    if(where == 0) where = "";
    sprintf(buf, "SELECT COUNT(*) from %s %s", tblname, where);
    Debug("%s\n", buf);
    ocsr = OQuery(odsn, buf);
    if(ocsr == 0) return(-1L);
    if(ONext(ocsr) == 0) {
	ptr = OGetCol(ocsr, 0);
	if(ptr)
	    memcpy(&total, ptr, sizeof(long));
	}
    OFreeOCSR(ocsr);
    return(total);
}
OLD */

long OGetCountWhere(ODSN *odsn, char *tblname, char *where)
{
    OCSR    *ocsr;
    char    buf[1024];
    long    total = -1L;
    char    *ptr, from[10];

    if(where == 0) where = "";
    if(tblname == 0 || tblname[0] <= ' ') {
	from[0] = 0;
	tblname = ""; /* JMP 03-10-08 */
	}
    else
	strcpy(from, "FROM");

    sprintf(buf, "SELECT COUNT(*) %s %s %s", from, tblname, where);
    Debug("%s\n", buf);
    ocsr = OQuery(odsn, buf);
    if(ocsr == 0) return(-1L);
    if(ONext(ocsr) == 0) {
	ptr = OGetCol(ocsr, 0);
	if(ptr)
	    memcpy(&total, ptr, sizeof(long));
	}
    OFreeOCSR(ocsr);
    return(total);
}

/* ====================================================================
Retourne un double, seul champ de la requête query.

&EN ODSN *odsn : Data Source
&EN char *query : requête retournant un seul champ double


&EX
    printf("Total TVA des ventes : %lf\n",
	    OQueryDouble(MyDSN, "Select sum(total * 0.21) from mytable");
&TX

&RT le résultat de la requête ou SCR_NAN en cas d'erreur
=======================================================================*/

double OQueryDouble(ODSN *odsn, char *query)
{
    OCSR    *ocsr;
    double  res = SCR_NAN;
    char    *ptr;

    ocsr = OQuery(odsn, query);
    if(ocsr == 0) return(res);
    if(ONext(ocsr) == 0) {
	ptr = OGetCol(ocsr, 0);
	if(ptr)
	    memcpy(&res, ptr, sizeof(double));
	}
    OFreeOCSR(ocsr);
    return(res);
}


/* ====================================================================
Retourne un long, seul champ de la requête query.

&EN ODSN *odsn : Data Source
&EN char *query : requête retournant un seul champ long


&EX
    printf("Nbre de ventes : %lf\n",
	    OQueryLong(MyDSN, "Select count(*) from mytable");
&TX

&RT le résultat de la requête ou SCR_NAN en cas d'erreur
=======================================================================*/

long OQueryLong(ODSN *odsn, char *query)
{
    OCSR    *ocsr;
    long    res = -1;       /* JMP 09-06-05 */
    char    *ptr;

    ocsr = OQuery(odsn, query);
    if(ocsr == 0) return(res);
    if(ONext(ocsr) == 0) {
	ptr = OGetCol(ocsr, 0);
	if(ptr)
	    memcpy(&res, ptr, sizeof(long));
	}
    OFreeOCSR(ocsr);
    return(res);
}


/* ====================================================================
Retourne un int, seul champ de la requête query.

&EN ODSN *odsn : Data Source
&EN char *query : requête retournant un seul champ long

&EX
    printf("Nbre de ventes : %lf\n",
	    OQueryShort(MyDSN, "Select count(*) from mytable");
&TX

&RT le résultat de la requête ou -1 en cas d'erreur
=======================================================================*/

OQueryShort(ODSN *odsn, char *query)
{
    OCSR    *ocsr;
    short   res = -1;
    char    *ptr;

    ocsr = OQuery(odsn, query);
    if(ocsr == 0) return(res);
    if(ONext(ocsr) == 0) {
	ptr = OGetCol(ocsr, 0);
	if(ptr)
	    memcpy(&res, ptr, sizeof(short));
	}
    OFreeOCSR(ocsr);
    return(res);
}


/* ====================================================================
Retourne le nombre de records dans la table tblname.

&EN ODSN *odsn : Data Source
&EN char *tblname : nom de la table

Cette fonction utilise SELECT COUNT(*) pour obtenir
l'information. Tous les systèmes ne supportent pas cette fonction
scalaire.

&RT le nombre d'enregistrements ou -1L en cas d'erreur
=======================================================================*/

long OGetCount(ODSN *odsn, char *tblname)
{
/*
    OCSR    *ocsr;
    char    buf[256];
    long    total = -1L;

    sprintf(buf, "SELECT COUNT(*) from %s", tblname);
    ocsr = OQuery(odsn, buf);
    if(ocsr == 0) return(-1L);
    if(ONext(ocsr) == 0)
	memcpy(&total, OGetCol(ocsr, 0), sizeof(long));
    OFreeOCSR(ocsr);
    return(total);
*/
    return(OGetCountWhere(odsn, tblname, (char *)0));
}
/* BP_M 22-12-2011 18:33 */
long OGetCountFree(ODSN *odsn, char *sql)
{
    OCSR    *ocsr;
    char    buf[256], *ptr;
    long    val = -1;
    double  d;

    ocsr = OQuery(odsn, sql);
    if(ocsr == 0) return(-1L);
    if(ONext(ocsr) == 0) {
	ptr = OGetCol(ocsr, 0);
	if(!ptr) goto ended;
	switch(ocsr->otbl->cols[0].Ctype) { /* BP_M 22-12-2011 18:33 */
	    case SQL_C_SHORT :
		val = *(short *)ptr;
		break;
	    case SQL_C_LONG :
		val = *(long *)ptr;
		break;
	    case SQL_C_FLOAT :
		d = *(float *)ptr;
		val = (long)d;
		break;
	    case SQL_C_DOUBLE :
		d = *(double *)ptr;
		val = (long)d;
		break;
	}
    }
ended:
    OFreeOCSR(ocsr);
    return(val);
}
/* BP_M 22-12-2011 18:33 */

/*NH*/
ONext_(OCSR *ocsr, int prev)
{
    SQLRETURN   retcode;
    int         i;
    OCOL        *ocol;
    extern FILE *DebugFd;

    if(ODebugDetail) DebugB("\t Next [%08ld] : ", ocsr);
    if(ocsr == 0) return(-1);
ag:
    retcode = SQLFetchScroll(ocsr->hstmt, SQL_FETCH_NEXT, 0);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO &&
       !(retcode == -1 && ORowStatusArray[0] == SQL_ROW_ERROR)  /* JMP 14-02-2004 */
	) {
	if(ODebugDetail) DebugE("Failed(rc=%d)", retcode);    /* JMP 10-07-03 */
	if(prev) OSetError("Next", ocsr->hstmt);
	if(prev && retcode == SQL_NO_DATA) OPrevious_(ocsr, 0);
	return(-1);
	}
    if(ORowStatusArray[0] ==  SQL_ROW_DELETED ||  /* JMP 06-10-99 */
       ORowStatusArray[0] ==  SQL_ROW_NOROW ||    /* JMP 06-10-99 */
       ORowStatusArray[0] ==  SQL_ROW_ERROR) {    /* JMP 06-10-99 */
	if(ODebugDetail) Debug("Status %d...", ORowStatusArray[0]);
	ORowStatusArray[0] = 0;
	goto ag;
	}

    ocsr->currec++;
    OGetVCols(ocsr);
    if(ODebugDetail > 1) {
	DebugOpen();
	OPrintRowFd(DebugFd, ocsr);
	DebugClose();
	}
    else if(ODebugDetail == 1) DebugE("Ok");

    return(0);
}

/*NH*/
OPrevious_(OCSR *ocsr, int next)
{
    SQLRETURN   retcode;
    extern FILE *DebugFd;

    if(ODebugDetail) DebugB("\t Prev [%08ld] : ", ocsr);
    if(ocsr == 0) return(-1);
ag:
    retcode = SQLFetchScroll(ocsr->hstmt, SQL_FETCH_PRIOR, 0);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO &&
       !(retcode == -1 && ORowStatusArray[0] == SQL_ROW_ERROR)  /* JMP 14-02-2004 */
       ) {
	if(ODebugDetail) DebugE("Failed (rc=%d)", retcode);
	if(next) OSetError("Previous", ocsr->hstmt);
	if(next && retcode == SQL_NO_DATA) ONext_(ocsr, 0);
	return(-1);
	}
    if(ORowStatusArray[0] ==  SQL_ROW_DELETED ||  /* JMP 06-10-99 */
       ORowStatusArray[0] ==  SQL_ROW_NOROW ||    /* JMP 06-10-99 */
       ORowStatusArray[0] ==  SQL_ROW_ERROR) {    /* JMP 06-10-99 */
	if(ODebugDetail) Debug("Status %d...", ORowStatusArray[0]);
	ORowStatusArray[0] = 0;
	goto ag;
	}

    ocsr->currec--;
    OGetVCols(ocsr);
    if(ODebugDetail > 1) {
	DebugOpen();
	OPrintRowFd(DebugFd, ocsr);
	DebugClose();
	}
    else if(ODebugDetail == 1) DebugE("Ok");
    return(0);
}

/*NH*/
ODelete(OCSR *ocsr)
{
    SQLRETURN   retcode;
    char        *buf = 0;
    int         i;

    DebugB("\t Del. [%08ld] ...", ocsr);
    retcode = SQLSetPos(ocsr->hstmt, (SQLUINTEGER)1, SQL_DELETE, SQL_LOCK_NO_CHANGE);
    if(retcode != 0) {
	OSetError("ODelete", ocsr->hstmt);
	DebugE("SetPos Failed - %s", OLASTERROR);   /* JMP 26-11-10 */
	return(-1);
	}
    DebugE("Ok");
    return(0);
}

/*NH*/
ORefresh(OCSR *ocsr)
{
    SQLRETURN   retcode;
    char        *buf = 0;
    int         i;

    retcode = SQLSetPos(ocsr->hstmt, 0, SQL_REFRESH, SQL_LOCK_NO_CHANGE);
    if(retcode != 0) {
	OSetError("ORefresh", ocsr->hstmt);         /* JMP 26-11-10 */
	DebugE("SetPos Failed - %s", OLASTERROR);   /* JMP 26-11-10 */
	return(-1);
	}
    return(0);
}

/*NH*/
OFormatFld(char *buf, OCSR *ocsr, int fldnb)
{
    short   *sh;
    long    val;
    double  dbl;
    char    *ptr;

    ptr = OGetCol(ocsr, fldnb);
    switch(ocsr->otbl->cols[fldnb].Ctype) {
	case SQL_C_CHAR :
	    if(ptr) sprintf(buf, "%s", ptr); break;
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
	    break;
    }
}


/* ====================================================================
Ajoute ou remplace les records définis dans le fichier csv csvfile.

&EN char *filename : nom du fichier csv. Première ligne = nom des fields
&EN char *dsn : nom de la Data Source
&EN char *table : nom de la table à modifier
&EN char *user : login name
&EN char *pwd : mot de passe éventuel
&EN int Verbose : 1 pour un affichage des messages pour chaque ligne


&RT 0 en cas de succès pour toutes les commandes, -1 en cas d'erreur

&EX
    voir OQuery().
&TX

&SA OQuery()
=======================================================================*/

OInsertRecords(char *filename, char *dsn, char *table, char *user, char *pwd, int Verbose, int insrepl)
{
    FILE    *fd;
    ODSN    *odsn;
    OCSR    *ocsr;
    char    buf[10240], sql[10240], values[10240], where[1024], fldval[4096],
	    **fldnames, *fldlist, **tbl, set[10240];
    int     rc = 0, i, j, ch, synt = 0;
    long    linenb = 0, nbupd = 0, nbins = 0;

    fd = fopen(filename, "r");
    if(fd == 0) return(-1);

    odsn = OOpenDSN(dsn, user, pwd);
    if(odsn == 0) return(-2);

    // Lit le nom des champs
    if(SCR_read_line(fd, buf, 10239) < 0) {
	rc = -1;
	goto fin;
	}
    fldnames = SCR_vtoms3(buf, ";", 0);
    fldlist = SCR_mtovs(fldnames, ", ");

    // Recherche les types
    sprintf(buf, "Select %s from %s", fldlist, table);
    ocsr = OQuery(odsn, buf);
    if(ocsr == 0) {
	rc = -1;
	OFreeOCSR(ocsr);
	if(Verbose) Wprintf("\n ***** %s\n\n", OError());
	goto fin;
	}

    while(1) {
	// Lit la ligne
	if(SCR_read_line(fd, buf, 10239) < 0) goto fin;
	linenb++;

	// Saute les lignes vides ou en commentaire
	SCR_strip(buf);
	for(i = 0 ; buf[i] ; i++)
	    if(buf[i] != ' ') break;
	if(buf[i] == 0 || buf[i] == '#') continue;

	// Créée la VALUES clause
	tbl = SCR_vtoms3(buf, ";", 0);
	if(SCR_tbl_size(tbl) != SCR_tbl_size(fldnames)) {
	    if(Verbose) Wprintf("\n ***** Line %d : incorrect # of fields \n\n", linenb);
	    goto fin;
	    }
	values[0] = 0;
	set[0] = 0;
	for(i = 0 ; i < ocsr->otbl->ncols ; i++) {
	    if(i > 0) {
		strcat(values, ", ");
		strcat(set, ", ");
		}
	    fldval[0] = 0;
	    switch(ocsr->otbl->cols[i].Ctype) {
		case SQL_C_CHAR :
		    strcat(fldval, "'");
		    SCR_strip(tbl[i]);
		    for(j = 0 ; tbl[i][j] ; j++) {
			ch = tbl[i][j];
			if(ch == '\'') strcat(fldval, "''");
			else if(synt == 1 && ch == '\\') strcat(fldval, "\\\\");
			else {
			    strcat(fldval, " ");
			    fldval[strlen(fldval) - 1] = ch;
			    }
			}

		 strcat(fldval, "'");
		 break;

	    case SQL_C_TINYINT :
	    case SQL_C_SHORT :
	    case SQL_C_LONG :
	    case SQL_C_FLOAT :
	    case SQL_C_DOUBLE :
	    case SQL_C_TYPE_TIMESTAMP:
	    case SQL_C_TYPE_DATE:
	    case SQL_C_TYPE_TIME:
		strcat(fldval, tbl[i]);
		break;

	    default :
		if(Verbose) Wprintf("Unknown type");
		break;
	    }
	    if(i == 0) strcpy(where, fldval);
	    strcat(values, fldval);
	    sprintf(set + strlen(set), " %s=%s", fldnames[i], fldval);
	}

	// Essaie de créer l'enregistrement
	switch(insrepl) {
	    case 0 : // Insertion seule
		sprintf(sql, "INSERT INTO %s (%s) VALUES (%s)", table, fldlist, values);
		if(OSql(odsn, sql)) {
		    Wprintf("Statement '%s' failed: %s\n", sql, OError());
		    rc = -1;
		    goto fin;
		    }
		else nbins++;
		break;

	    case 1 : // remplacement seul
		sprintf(sql, "UPDATE %s SET %s WHERE %s=%s",
		    table, set, fldnames[0], where);
		if(OSql(odsn, sql)) {
		    Wprintf("Statement '%s' failed: %s\n", sql, OError());
		    rc = -1;
		    goto fin;
		    }
		else nbupd++;
		break;
	    default : // Insertion ou remplacement
		sprintf(sql, "INSERT INTO %s (%s) VALUES (%s)", table, fldlist, values);
		if(OSql(odsn, sql)) {
		    sprintf(sql, "UPDATE %s SET %s WHERE %s=%s",
			table, set, fldnames[0], where);
		    if(OSql(odsn, sql)) {
			Wprintf("Statement '%s' failed: %s\n", sql, OError());
			rc = -1;
			goto fin;
			}
		    else nbupd++;
		     }
		else nbins++;
		break;
	    }

	SCR_free_tbl(tbl);
	}
fin:
    OCloseDSN(odsn);
    SCR_free_tbl(fldnames);
    SCR_free(fldlist);
    fclose(fd);
    if(Verbose)
	Wprintf("Read record(s) : %ld - Inserts : %ld - Updates : %ld\n", linenb, nbins, nbupd);
    return(rc);
}


#endif







