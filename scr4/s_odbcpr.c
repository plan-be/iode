
#if defined(DOSW32) || defined(WINDOWS)

#include <scr4w.h>
#include "s_odbc.h"
#include "sqlvers.h"

char    *OMyEngine = 0, *OMyCharSet = 0, *OMyDbName = 0; /* JMP 05-02-11 */

/* ====================================================================
Imprime dans le fichier décrit par fd la ligne courante d'un dataset.
Le format est identique à l'output de scr4 -dump, soit un ligne avec
les champs séparés par des '|'.

&EN OCSR *ocsr : pointeur vers un curseur retourné par OQuery()

&EX
    voir OQuery().
&TX

&SA OQuery(), ONext(), OBindCOl()
=======================================================================*/

OPrintRowFd(FILE *fd, OCSR *ocsr)
{
    int     i;
    short   *sh;
    long    val = 0;
    double  dbl = 0;
    char    *ptr;
    char    buf[10240];

    if(fd == 0) return(0); /* JMP 06-01-99 */
    for(i = 0 ; i < ocsr->otbl->ncols ; i++) {
	if(i > 0) fprintf(fd, "|");
	ptr = OGetCol(ocsr, i);
	if(ptr == 0) {
	    //fprintf(fd, "''");
	    continue; /* JMP 25-10-99 */
	    }
	switch(ocsr->otbl->cols[i].Ctype) {
	    case SQL_C_CHAR :
		if(ptr) fprintf(fd, "%s", ptr); break;
	    case SQL_C_TINYINT :
		val = ptr[0];
		fprintf(fd, "%ld", val);
		break;
	    case SQL_C_SHORT :
		val = *(short *)ptr;
		fprintf(fd, "%ld", val);
		break;
	    case SQL_C_LONG :
		val = *(long *)ptr;
		fprintf(fd, "%ld", val);
		break;
	    case SQL_C_FLOAT :
		dbl = *(float *)ptr;
		fprintf(fd, "%.8e", dbl);
		break;
	    case SQL_C_DOUBLE :
		dbl = *(double *)ptr;
		fprintf(fd, "%.16le", dbl);
		break;

	    case SQL_C_TYPE_TIMESTAMP:
	    case SQL_C_TYPE_DATE:
		sh = (short *)ptr;
		if(sh[0] == 1899 && sh[1] == 12 && sh[2] == 30) // Time Only
		    fprintf(fd, "%02d%02d%02d", sh[3], sh[4], sh[5]);
		else {  /* BP_M 18-01-2006 */
		    fprintf(fd, "%04d%02d%02d", sh[0], sh[1], sh[2]);
		    if(sh[3] != 0 || sh[4] != 0 || sh[5] != 0)
			fprintf(fd, " %02d%02d%02d", sh[3], sh[4], sh[5]);
		}
		break;

	    case SQL_C_TYPE_TIME:
		sh = (short *)ptr;
		fprintf(fd, "%02d%02d%02d", sh[0], sh[1], sh[2]);
		break;

	    default :
		fprintf(fd, "Unknown type");
		break;
	    }
	}
    fprintf(fd, "\n");
}

/* ====================================================================
Imprime dans le fichier décrit par fd la ligne courante d'un dataset sous
forme d'une instruction INSERT INTO.

&EN FILE *fd : descripteur du fichier output
&EN OCSR *ocsr : pointeur vers un curseur retourné par OQuery()
&EN int synt : 0 pour SQL standard, 1 pour MyQql, 2 pour Access

&EX
    voir OQuery().
&TX

&SA OQuery(), ONext(), OBindCOl()
=======================================================================*/

OPrintInsertRowFd(FILE *fd, OCSR *ocsr, char *tblname, int strip, int synt)
{
    int     i, j, ch;
    short   *sh;
    long    val = 0;
    double  dbl = 0;
    U_ch    *ptr, buf[80];

    if(fd == 0) return(0); /* JMP 06-01-99 */
    switch(synt) {
	case 1 : fprintf(fd, "INSERT INTO ");
		 if(OMyDbName) fprintf(fd, "`%s`.", OMyDbName);
		 fprintf(fd, "`%s` VALUES (", tblname);
		 break;
	case 2 : fprintf(fd, "INSERT INTO [%s] VALUES (", tblname); break;
	default : fprintf(fd, "INSERT INTO %s VALUES (", tblname); break;
	}

    for(i = 0 ; i < ocsr->otbl->ncols ; i++) {
	if(i > 0) fprintf(fd, ",");
	ptr = OGetCol(ocsr, i);
	switch(ocsr->otbl->cols[i].Ctype) {
	    case SQL_C_CHAR :
		fprintf(fd, "'");
		if(ptr) {
		    if(strip) SCR_strip(ptr);
		    for(j = 0 ; ptr[j] ; j++) {
			ch = ptr[j];
			if(ch == '\'') fprintf(fd, "''");
			else if(synt == 1 && ch == '\\') fprintf(fd, "\\\\"); /* JMP 03-01-06 */
			else if(synt == 2 && (ch < ' ' || ch == '=')) {
			    buf[0] = '=';
			    buf[1] = "0123456789ABCDEF"[ch / 16];
			    buf[2] = "0123456789ABCDEF"[ch % 16];
			    buf[3] = 0;
			    fprintf(fd, "%s", buf);
			    }
			/* JMP 05-10-2003 */
			else
			    putc(ch, fd);
			}

		    }
		 fprintf(fd, "'");
		 break;

	    case SQL_C_TINYINT :
		if(ptr == 0) {fprintf(fd, "0"); break;}
		val = ptr[0];
		fprintf(fd, "%ld", val);
		break;
	    case SQL_C_SHORT :
		if(ptr == 0) {fprintf(fd, "0"); break;}
		val = *(short *)ptr;
		fprintf(fd, "%ld", val);
		break;
	    case SQL_C_LONG :
		if(ptr == 0) {fprintf(fd, "0"); break;}
		val = *(long *)ptr;
		fprintf(fd, "%ld", val);
		break;
	    case SQL_C_FLOAT :
		if(ptr == 0) {fprintf(fd, "0"); break;}
		dbl = *(float *)ptr;
		fprintf(fd, "%.8e", dbl);
		break;
	    case SQL_C_DOUBLE :
		if(ptr == 0) {fprintf(fd, "0"); break;}
		dbl = *(double *)ptr;
		fprintf(fd, "%.16le", dbl);
		break;

	    case SQL_C_TYPE_TIMESTAMP:
	    case SQL_C_TYPE_DATE:
		if(ptr == 0) {fprintf(fd, "0"); break;}
		sh = (short *)ptr;
		fprintf(fd, "%04d%02d%02d", sh[0], sh[1], sh[2]);
		if(sh[3] != 0 || sh[4] != 0 || sh[5] != 0)  /* BP_M 18-01-2006 */
		    fprintf(fd, " %02d%02d%02d", sh[3], sh[4], sh[5]);
		break;

	    case SQL_C_TYPE_TIME:
		if(ptr == 0) {fprintf(fd, "0"); break;}
		sh = (short *)ptr;
		fprintf(fd, "%02d%02d%02d", sh[0], sh[1], sh[2]);
		break;

	    default :
		fprintf(fd, "Unknown type");
		break;
	    }
	}
    fprintf(fd, ");\n");
}

/* ====================================================================
Imprime dans le standard output la ligne courante d'un dataset.
Le format est identique à l'output de scr4 -dump, soit un ligne avec
les champs séparés par des '|'.

&EN OCSR *ocsr : pointeur vers un curseur retourné par OQuery()

&EX
    voir OQuery().
&TX

&SA OQuery(), ONext(), OBindCOl()
=======================================================================*/

OPrintRow(OCSR *ocsr)
{
    OPrintRowFd(stdout, ocsr);
}

/* ====================================================================
Imprime dans le standard output la ligne courante d'un dataset sous forme d'une instruction INSERT INTO.

&EN OCSR *ocsr : pointeur vers un curseur retourné par OQuery()

&EX
    voir OQuery().
&TX

&SA OQuery(), ONext(), OBindCOl()
=======================================================================*/

OPrintInsertRow(OCSR *ocsr, char *tblname, int strip)
{
    OPrintInsertRowFd(stdout, ocsr, tblname, strip, 0);
}


/*NH*/
ODisplayCreateFieldFd(FILE *fd, OCOL *ocol, int synt)
{
    short   *sh;
    long    val;
    double  dbl;
    char    *ptr;

    switch(synt) {
	case 1  : fprintf(fd, "`%s` ", ocol->name); break;
	case 2  : fprintf(fd, "[%s] ", ocol->name); break;
	default : fprintf(fd, "%s ", ocol->name); break;
	}
    if(synt == 1 && strcmp(ocol->typename, "COUNTER") == 0)
	fprintf(fd, "INTEGER "); // Mysql ne connait pas COUNTER
    else if(strcmp(ocol->typename, "LONGCHAR") == 0) {
	if(synt == 2) fprintf(fd, "MEMO");
	else          fprintf(fd, "TEXT");
	}
    else if(strcmp(ocol->typename, "VARCHAR") == 0) {
	if(synt == 2 && ocol->nc > 1) fprintf(fd, "VARCHAR"); /* ACCESS JMP 16-02-07 */
	else if(ocol->nc == 1 && synt != 1) /* MYSQL JMP 03-01-06 */
	    fprintf(fd, "TINYINT");
	else
	    fprintf(fd, "CHAR(%d)", ocol->nc);
	}
    else if(strcmp(ocol->typename, "CHAR") == 0) {
	fprintf(fd, "CHAR(%d)", ocol->nc);
	}
    else
	fprintf(fd, "%s", ocol->typename);
}

/* ====================================================================
Imprime dans le standard output la commande pour dropper une table
La syntaxe peut varier en fonction
de la valeur de synt (MySQL, ...).

&EN FILE *fd : file descriptor ouvert en écriture
&EN char *table : nom de la table ou NULL. Si nul, toutes les
    tables sont imprimées
&EN int synt : 0 pour syntaxe std, 1 pour MySql

&RT 0 en cas de succès, -1 en cas d'erreur
=======================================================================*/

ODisplayDropTableFd(FILE *fd, char *table, int synt)
{
    fprintf(fd, "DROP TABLE ");
    switch(synt) {
	case 1  : fprintf(fd, "IF EXISTS ");
		  if(OMyDbName) fprintf(fd, "`%s`.", OMyDbName);
		  fprintf(fd, "`%s`", table);
		  break;
	case 2  : fprintf(fd, "[%s] ", table); break;
	default : fprintf(fd, "%s ", table); break;
	}
    fprintf(fd, ";\n");
}

/* ====================================================================
Imprime dans le standard output la définition d'une ou de toutes
les tables d'un DSN dans le format SQL. La syntaxe peut varier en fonction
de la valeur de synt (MySQL, ...).

&EN FILE *fd : file descriptor ouvert en écriture
&EN ODSN *odsn : Data Source ouverte
&EN char *table : nom de la table ou NULL. Si nul, toutes les
    tables sont imprimées
&EN int synt : 0 pour syntaxe std, 1 pour MySql

&RT 0 en cas de succès, -1 en cas d'erreur

&EX
    odsn = OOpenDSN("testdsn", "jmp", "123");
    ODisplayCreateTable(odsn, NULL);
&TX
=======================================================================*/

ODisplayCreateTableFd(FILE *fd, ODSN *odsn, char *table, int synt)
{
    int     i, k;
    OTBL    *otbl;
    char    **tbls;

    if(table == 0) {
	tbls = OListTables(odsn);
	if(tbls == 0) {
	    printf("Cannot read tables ; %s\n", OError());
	    return(-1);
	    }
	}
    else
	tbls = (char **)SCR_vtom(table, ',');

    for(i = 0; tbls[i] ; i++) {
	if(synt == 1) {
	    fprintf(fd, "CREATE TABLE ");
	    if(OMyDbName) fprintf(fd, "`%s`.", OMyDbName);
	    fprintf(fd, "`%s` (", tbls[i]);
	    }
	else
	    fprintf(fd, "CREATE TABLE %s (", tbls[i]);

	otbl = OGetTableInfo(odsn, tbls[i]);
	if(otbl == 0)
	    printf("    Cannot read COLS\n");
	else {
	    for(k = 0 ; k < otbl->ncols ; k++) {
		if(k) fprintf(fd, ", ");
		ODisplayCreateFieldFd(fd, otbl->cols + k, synt);
		}
	    OFreeOTBL(otbl);
	    }

	fprintf(fd, ")");
	if(synt == 1) {
	    if(OMyEngine)  fprintf(fd, " ENGINE=%s", OMyEngine);
	    if(OMyCharSet) fprintf(fd, " CHARSET=%s", OMyCharSet);
	    }
	fprintf(fd, ";\n");
	}

    SCR_free_tbl(tbls);
    return(0);
}

/* ====================================================================
Imprime dans le standard output la définition d'une ou de toutes
les tables d'un DSN dans le format SQL.

&EN ODSN *odsn : Data Source ouverte
&EN char *table : nom de la table ou NULL. Si nul, toutes les
    tables sont imprimées

&RT 0 en cas de succès, -1 en cas d'erreur

&EX
    odsn = OOpenDSN("testdsn", "jmp", "123");
    ODisplayCreateTable(odsn, NULL);
&TX
=======================================================================*/

ODisplayCreateTable(ODSN *odsn, char *table)
{
    return(ODisplayCreateTableFd(stdout, odsn, table, 0));
}


ODisplayCreateIndexFieldFd(FILE *fd, OIDXFLD *ofld, int synt)
{
    switch(synt) {
	case 1  : fprintf(fd, "`%s` ", ofld->name); break;
	case 2  : fprintf(fd, "[%s] ", ofld->name); break;
	default : fprintf(fd, "%s ", ofld->name); break;
	}

    if(ofld->asc == 'D') fprintf(fd, "DESC");
    else                 fprintf(fd, "ASC");
}

/* ====================================================================
Imprime dans le standard output la définition des index d'une table
d'un DSN dans le format SQL.

&EN FILE *fd : file descriptor ouvert en écriture
&EN ODSN *odsn : Data Source ouverte
&EN char *table : nom de la table ou NULL. Si nul, tous les
    index de toutes les tables sont imprimés

&RT 0 en cas de succès, -1 en cas d'erreur

&EX
    odsn = OOpenDSN("testdsn", "jmp", "123");
    ODisplayCreateIndicesFd(odsn, NULL);
&TX
=======================================================================*/

ODisplayCreateIndicesFd(FILE *fd, ODSN *odsn, char *table, int synt)
{
    int     i, j, k, flag_pri;
    OIDX    *oidx;
    char    **tbls;

    if(table == 0) {
	tbls = OListTables(odsn);
	if(tbls == 0) {
	    printf("Cannot read tables ; %s\n", OError());
	    return(-1);
	    }
	}
    else
	tbls = (char **)SCR_vtom(table, ',');

    for(i = 0; tbls[i] ; i++) {
	oidx = OGetTableIndices(odsn, tbls[i]);
	if(oidx == 0) continue;
	flag_pri = 1;
	for(j = 0 ; oidx[j].name ; j++) {
	    fprintf(fd, "CREATE ");
	    if(oidx[j].unique == 0) fprintf(fd, "UNIQUE ");
	    switch(synt) {
		case 1  :
		    fprintf(fd, "INDEX `%s` ON ", oidx[j].name);
		    if(OMyDbName) fprintf(fd, "`%s`.", OMyDbName);
		    fprintf(fd, "`%s` (", tbls[i]);
		    break;
		case 2  :
		    fprintf(fd, "INDEX [%s] ON [%s] (", oidx[j].name, tbls[i]);
		    break;
		default :
		    fprintf(fd, "INDEX %s ON %s (", oidx[j].name, tbls[i]);
		    break;
		}

	    for(k = 0 ; k < oidx[j].ncols ; k++) {
		if(k) fprintf(fd, ", ");
		ODisplayCreateIndexFieldFd(fd, oidx[j].oflds + k, synt);
		}
	    fprintf(fd, ")");
	    if(flag_pri && oidx[j].unique == 0) {
		// fprintf(fd, " WITH PRIMARY"); /* JMP 05-10-2003 */
		flag_pri = 0;
		}
	    fprintf(fd, ";\n");
	    }
	OFreeOIDX(oidx);
	}

    SCR_free_tbl(tbls);
    return(0);
}

/* ====================================================================
Imprime dans le standard output la définition d'une ou de toutes
les tables d'un DSN dans le format SQL.

&EN ODSN *odsn : Data Source ouverte
&EN char *table : nom de la table ou NULL. Si nul, toutes les
    tables sont imprimées

&RT 0 en cas de succès, -1 en cas d'erreur

&EX
    odsn = OOpenDSN("testdsn", "jmp", "123");
    ODisplayCreateTable(odsn, NULL);
&TX
=======================================================================*/

ODisplayCreateIndices(ODSN *odsn, char *table)
{
    return(ODisplayCreateIndicesFd(stdout, odsn, table, 0));
}
/* ====================================================================
Imprime dans le standard output la définition d'une ou de toutes
les tables d'un DSN dans le format des ISAM de SCR/AL1. Le DSN
est ouvert et fermé par cette fonction.

&EN char *dsn : nom de la Data Source
&EN char *user : login name
&EN char *passwd : mot de passe
&EN char *table : nom de la table ou NULL. Si nul, toutes les
    tables sont imprimées

&RT 0 en cas de succès, -1 en cas d'erreur

&EX
    ODisplayTable("test", "", "", NULL);
&TX
=======================================================================*/

ODisplayTable(char *dsn, char *user, char *passwd, char *table)
{
    return(ODisplayTableEx(dsn, user, passwd, table, 0));
}

/*NH*/
ODisplayTableEx(char *dsn, char *user, char *passwd, char *table, int names)
{
    int     i, k;
    ODSN    *odsn;
    OTBL    *otbl;
    char    **tbls;

    if(OInit()) {
	printf("ODBC error: Cannot initialize\n");
	return(-1);
	}
    odsn = OOpenDSN(dsn, user, passwd);
    if(odsn == 0) {
	printf("DSN '%s' : cannot Open (%s)\n", dsn, OError());
	return(-1);
	}

    if(table == 0) {
	tbls = OListTables(odsn);
	if(tbls == 0) {
	    printf("DSN %s : cannot read tables (%s)\n", dsn, OError());
	    OCloseDSN(odsn);
	    return(-1);
	    }
	}
    else
	tbls = (char **)SCR_vtom(table, ',');

    printf("/* DSN \"%s\" */\n", dsn);
    for(i = 0; tbls[i] ; i++) {
	if(names == 0) {
	    otbl = OGetTableInfo(odsn, tbls[i]);
	    if(otbl == 0)
		printf("    Cannot read COLS\n");
	    else {
		printf("  ISAM \"%s\" {\n", SCR_sqz(tbls[i]));
		for(k = 0 ; k < otbl->ncols ; k++)
		    ODisplayFieldSCR(otbl->cols + k, k);
		OFreeOTBL(otbl);
		printf("  }\n", tbls[i]);
		}
	    }
	else
	    printf("%s\n", tbls[i]);
	}

    SCR_free_tbl(tbls);
    OCloseDSN(odsn);
    return(0);
}


/*NH*/
ODisplayFieldSCR(OCOL *ocol, int k)
{
    if(ocol->Cvar)    printf("    VFIELD /*%d*/ ", k);
    else              printf("    FIELD /*%d*/ ", k);
    switch(ocol->scrtype) {
	case SCR_EDITOR  : printf("EDITOR     "); break;
	case SCR_STRING  : printf("STRING %d  ", ocol->Clen); break;
	case SCR_ZSTRING : printf("ZSTRING %d ", ocol->Clen); break;
	case SCR_SHORT   : printf("SHORT      "); break;
	case SCR_LONG    : printf("LONG       "); break;
	case SCR_FLOAT   : printf("FLOAT      "); break;
	case SCR_DOUBLE  : printf("DOUBLE     "); break;
	case SCR_TIME    : printf("TIME       "); break;
	case SCR_DATE    : printf("DATE       "); break;
	case SCR_CHAR    : printf("CHAR       "); break;
	default          : printf("/*UNKNOWN*/"); break;
	}
    if(ocol->name && ocol->name[0])
	printf("\tNAME \"%s\" ", SCR_sqz(ocol->name));
    if(ocol->descr && ocol->descr[0])
	printf("COMMENT \"%s\" ", ocol->descr);

    printf(" /* ODBC:type=%s[%d] len=%d nullable=%d isnull=%s */\n",
	ocol->typename,
	ocol->sqltype,
	ocol->len,
	ocol->nullable,
	ocol->isnull);
}


/* ====================================================================
Imprime dans le standard output les informations de version
d'un DSN donné.
Le DSN est ouvert et fermé par cette fonction.

&EN char *dsn : nom de la Data Source
&EN char *user : login name
&EN char *passwd : mot de passe

&RT 0 en cas de succès, -1 en cas d'erreur

&EX
    ODisplayDSNInfos("test", "", "", NULL);
&TX
=======================================================================*/

ODisplayDSNInfos(char *dsn, char *user, char *passwd)
{
    ODSN            *odsn;
    unsigned long   bits;
    short           lg;

    if(OInit()) {
	printf("ODBC error: Cannot initialize\n");
	return(-1);
	}
    odsn = OOpenDSN(dsn, user, passwd);
    if(odsn == 0) {
	printf("DSN '%s' : cannot Open (%s)\n", dsn, OError());
	return(-1);
	}
    printf("DSN %s\n", dsn);
    printf("  DBMS: %s\n", odsn->dbms);
    printf("  DBMS Version: %s\n", odsn->dbmsvers);
    printf("  DBMS File: %s\n", odsn->filename);

    if(SQLGetInfo(odsn->hdbc, SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1, &bits, 4, &lg) == 0) {
	printf("  FORW NEXT : %d\n", bits & SQL_CA1_NEXT);
	printf("  FORW ABSOLUTE : %d\n", bits & SQL_CA1_ABSOLUTE);
	printf("  FORW BULK_ADD : %d\n", bits & SQL_CA1_BULK_ADD);
	printf("  FORW BULK_UPDATE_BY_BOOKMARK : %d\n", bits & SQL_CA1_BULK_UPDATE_BY_BOOKMARK);
	}

    if(SQLGetInfo(odsn->hdbc, SQL_STATIC_CURSOR_ATTRIBUTES1, &bits, 4, &lg) == 0) {
	printf("  STATIC NEXT : %d\n", bits & SQL_CA1_NEXT);
	printf("  STATIC ABSOLUTE : %d\n", bits & SQL_CA1_ABSOLUTE);
	printf("  STATIC BULK_ADD : %d\n", bits & SQL_CA1_BULK_ADD);
	printf("  STATIC BULK_UPDATE_BY_BOOKMARK : %d\n", bits & SQL_CA1_BULK_UPDATE_BY_BOOKMARK);
	}

    if(SQLGetInfo(odsn->hdbc, SQL_DYNAMIC_CURSOR_ATTRIBUTES1, &bits, 4, &lg) == 0) {
	printf("  DYNAMIC NEXT : %d\n", bits & SQL_CA1_NEXT);
	printf("  DYNAMIC PREV : %d\n", bits & SQL_CA1_RELATIVE);
	printf("  DYNAMIC ABSOLUTE : %d\n", bits & SQL_CA1_ABSOLUTE);
	printf("  DYNAMIC BULK_ADD : %d\n", bits & SQL_CA1_BULK_ADD);
	printf("  DYNAMIC BULK_UPDATE_BY_BOOKMARK : %d\n", bits & SQL_CA1_BULK_UPDATE_BY_BOOKMARK);
	}

    if(SQLGetInfo(odsn->hdbc, SQL_KEYSET_CURSOR_ATTRIBUTES1, &bits, 4, &lg) == 0) {
	printf("  KEYSET NEXT : %d\n", bits & SQL_CA1_NEXT);
	printf("  KEYSET PREV : %d\n", bits & SQL_CA1_RELATIVE);
	printf("  KEYSET ABSOLUTE : %d\n", bits & SQL_CA1_ABSOLUTE);
	printf("  KEYSET BULK_ADD : %d\n", bits & SQL_CA1_BULK_ADD);
	printf("  KEYSET DYNAMIC BULK_UPDATE_BY_BOOKMARK : %d\n", bits & SQL_CA1_BULK_UPDATE_BY_BOOKMARK);
	}

    OCloseDSN(odsn);
    return(0);
}

OSqlQP2Ascii(U_ch *in, U_ch *out)
{
    int     i, j;

    for(i = j = 0 ; in[i] ; i++) {
	if(in[i] == '=') {
	    out[j++] = U_pos(in[i + 1], "0123456789ABCDEF") * 16 +
		       U_pos(in[i + 2], "0123456789ABCDEF");
	    i += 2;
	    }
	else
	    out[j++] = in[i];
	}
    out[j] = 0;
}

/* ====================================================================
Exécute les commandes SQL regroupées dans un fichier ASCII.
Une commande peut être spécifiée par ligne.

Chaque ligne du fichier est décodée Quoted-Printable.
&EN ODSN *odsn : DSN ouvert
&EN char *filename : nom du fichier de commandes
&EN int Verbose : 1 pour un affichage des messages pour chaque ligne
&EN int maxerr : nbre d'erreurs max après quoi il faut arrêter ou -1
    pour ne jamais stopper.

Les lignes sont limitées à 10239 caractères.

&RT 0 en cas de succès pour toutes les commandes, -1 en cas d'erreur

&EX
    voir OQuery().
&TX

&SA OQuery()
=======================================================================*/

OExecSQLFileQP(ODSN *odsn, char *filename, int Verbose, int maxerr)
{
    FILE    *fd;
    char    buf[10240];
    int     rc = 0, odebug;
    long    linenb = 0;

    fd = fopen(filename, "r");
    if(fd == 0) return(-1);

    while(1) {
	if(SCR_read_line(fd, buf, 10239) < 0) break;
	OSqlQP2Ascii(buf, buf);
	linenb++;
	if(Verbose) Wprintf("%s[%ld]:", filename, linenb);

	if(OSql(odsn, buf)) {
	    odebug = DebugActif;
	    DebugActif = 1;
	    if(Verbose) Wprintf("%s ---> %s\n", buf, OError());
	    else Debug("%s[%ld]:%s\n", filename, linenb, OError());
	    DebugActif = odebug;
	    rc++;
	    }
	else
	    if(Verbose) Wprintf(":Ok\n");
	if(maxerr >= 0 && rc > maxerr) break;
	}

    fclose(fd);
    return(rc);
}



#endif



