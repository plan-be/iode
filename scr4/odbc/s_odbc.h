#ifndef _S_ODBC_H_
#define _S_ODBC_H_

typedef void* SQLHSTMT;
typedef void* SQLHDBC;
typedef void* SQLHENV;
typedef void* SQLHANDLE;

typedef struct _odsn_ {
    SQLHDBC     hdbc;    /* Handle sur la database */
    SQLHSTMT    hstmt;   /* Handle sur le statement de base */
    int         ncsrs;   /* Nombre de curseurs ouverts */
    SQLHSTMT    *hcsrs;  /* Curseurs ouverts */
    char        *dsname; /* Nom ODBC de la source */
    char        *user;   /* Nom du USER */
    char        *passwd; /* Password */
    char        *dbms;   /* DMBS */
    char        *dbmsvers; /* DMBS Version */
    char        *filename; /* Nom du fichier (si défini) */
    int         prev[2];   /* IS_prev possible(static, dynamic)  */
} ODSN;

typedef struct _ocol_ {
    char        *name;       /* Nom de la colonne */
    char        *typename;   /* Type de la colonne */
    char        *descr;      /* Description de la colonne */
    char        *dft;        /* Default value */
    char        *isnull;     /* Is Nullable */
    int         sqltype;     /* Type SQL de la colonne */
    int         scrtype;     /* Type SCR de la colonne */
    int         Ctype;       /* Type C de la colonne */
    int         Clen;        /* Longueur C de la colonne */
    int         Cvar;        /* 1 si champ variable (Cptr alloué) */
    char        *Cptr;       /* Pointeur vers la valeur */
    char        *Cvptr;      /* Pointeur vers la valeur (si Cvar) */
    int         len;         /* Buffer len */
    int         nc;          /* Char len (display) */
    int         nullable;    /* Nuls acceptés */
    long        datalen;     /* Taille de la valeur de retour (bind) */
} OCOL;

typedef struct _otbl_ {
    ODSN        *odsn;     /* Definition de la database */
    char        *tbl_name; /* Nom de la table */
    int         ncols;     /* Nombre de colonnes */
    OCOL        *cols;     /* table de colonnes */
    char        *ptr;      /* Pointeur sur le "record" */
    int         ptrlen;    /* Longueur d'un record */
    SQLHSTMT    whstmt;    /* Handle sur le statement de base */
} OTBL;

typedef struct _oidxfld_ {
    char        *name;     /* Nom du champ */
    int         asc;       /* Index ascendant */
} OIDXFLD;

typedef struct _oidx_ {
    char        *name;     /* Nom de l'index */
    int         unique;    /* Index unique */
    int         ncols;     /* Nombre de champs */
    OIDXFLD     oflds[10]; /* Description des champs */
} OIDX;

typedef struct _ocsr_ {
    ODSN        *odsn;   /* Definition de la database */
    OTBL        *otbl;   /* Definition de la table résultat du Query */
    SQLHSTMT    hstmt;   /* Handle sur le statement de base */
    char        *ptr;    /* Pointeur sur le "record" */
    long        currec; /* numéro du record courant */
//    int         tmp;     /* Buffer pour le param de bind */
} OCSR;

/* VARS */
extern SQLHENV OHenv;

/* FNS */
extern int     OInit();
extern int     OEnd();
extern char ** OListDSNs();
extern ODSN *  OOpenDSN(char *dsn, char *user, char *passwd);
extern int     OCloseDSN(ODSN *);
extern char ** OListTables(ODSN *);
extern OTBL *  OGetTableInfo(ODSN *, char *tblname);
extern int     OSql(ODSN *, char *sqlcmd);
extern OCSR *  OQuery(ODSN *, char *sqlcmd);
extern char *  OError();
extern char *  OSetError(char *msg, SQLHANDLE hstmt);
extern char *  OSetErrorDb(char *msg, SQLHANDLE hstmt);
extern char *  OGetCol(OCSR *, int);
extern char *  OGetOtblCol(OTBL *, int);
extern long    OGetCount(ODSN *odsn, char *tblname);
extern double  OQueryDouble(ODSN *odsn, char *query);

#endif /* _S_ODBC_H_ */
