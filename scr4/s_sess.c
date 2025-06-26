#include "scr4.h"

#ifdef DOS
char *WSCR_SESSION_DIR = ".\\";
#else
char *WSCR_SESSION_DIR = "/tmp/";
#endif

/*NH*/
unsigned char **WscrSessionReadFile(filename)
char    *filename;
{
    FILE            *fd;
    unsigned char   **txt = 0,
		    buffer[10240];
    int             len = 0, ch,
		    nb_line = 0,
		    i;

    /* OPEN INFO FILE */
    fd = fopen(filename, "r");
    if(fd == NULL) return(txt);

    /* READ INFO */
    while(1) {
	ch = getc(fd);
	switch(ch) {
	    case EOF:
		buffer[len] = 0;
		SCR_add_ptr(&txt, &nb_line, buffer);
		SCR_add_ptr(&txt, &nb_line, NULL);
		fclose(fd);
		return(txt);
	    case '\n':
		buffer[len] = 0;
		SCR_add_ptr(&txt, &nb_line, buffer);
		len = 0;
		break;
	    case '\t':
		for(i = len ; i < len + 8 - len%8 ; i++)
		    buffer[i] = ' ';
		len = i - 1;
		break;
	    default:
		if(len < sizeof(buffer) - 1) buffer[len++] = ch;
		break;
	    }
	}
}

/*NH*/

WscrSessionSaveFile(filename, ptr)
char    *filename,
	**ptr;
{
    FILE    *fd;

    fd = fopen(filename, "w");
    if(fd == 0) {
	Debug("WscrSessionSaveFile(%s, ...); Pas de fichier\n", filename);
	return(-1);
    }
    while(*ptr != 0) {
	fprintf(fd, "%s\n", *ptr);
	ptr++;
	}

    fclose(fd);
    return(0);
}



/******************************************************************************
Crée une nouvelle session. Cette session reste active pendant secs secondes.

La variable WSCR_SESSION_DIR contient un pointeur vers le préfixe du nom des fichiers
servant de support aux sessions. Si cette variable est vide, la session
est créée dans le répertoire courant.



&EX
    long    sessnb;

    WSCR_SESSION_DIR = "c:\\tmp\\";
    sessnb = WscrSessionCreate(300); // La session est valable pour 5 minutes
    switch(WscrSessionCheck(sessnb)) {
	case 0  : printf("Session OK\n"); break;
	case -1 : printf("Session inexistante\n"); break;
	case -2 : printf("Délai expiré\n"); break;
	}

&TX

&SA WscrSessionDelete(),
    WscrSessionCheck()
    WscrSessionSetCookie()
    WscrSessionGetCookie()
    WscrSessionDelCookie()
*******************************************************************************/

WscrSessionCreate(long secs)
{
    long    i;
    char    buf[256];
    FILE    *fd;
    long    asec;

    srand(time(NULL));
    while(1) {
	i = 65535U * rand() + rand();
	i = 100000L + i % (900000L);
	sprintf(buf, "%sSS%ld.ses", WSCR_SESSION_DIR, i);
	if(access(buf, 0) < 0) break;
	}

    fd = fopen(buf, "w+");
    asec = DT_current_sec();
    fprintf(fd, "%10ld,%10ld,%10ld,%10ld",
	SCR_current_date(), SCR_current_time(), asec, secs);
    fclose(fd);
    return(i);
/*
    if(IS_open_files(scr_sess, 0L)) return(-1);
    scr_sess_CRDATE = SCR_current_date();
    scr_sess_CRTIME = SCR_current_time();
    scr_sess_ASEC  = DT_current_sec();
    scr_sess_DELAY  = secs;
    scr_sess_COOKIE[0] = 0;
    if(IS_write(scr_sess)) return(-1);
    IS_close(scr_sess);
    return(scr_sess_NUM);
*/
}

/******************************************************************************
Détruit un session.

La variable WSCR_SESSION_DIR contient un pointeur vers le préfixe du nom des fichiers
servant de support aux sessions.


&EX
    long    sessnb;

    WSCR_SESSION_DIR = "c:\\tmp\\";
    sessnb = WscrSessionCreate(300); // La session est valable pour 5 minutes
    switch(WscrSessionCheck(sessnb)) {
	case 0  : printf("Session OK\n"); break;
	case -1 : printf("Session inexistante\n"); break;
	case -2 : printf("Délai expiré\n"); break;
	}
    WscrSessionDelete(sessnb);
&TX

&SA WscrSessionCreate(),
    WscrSessionCheck()
    WscrSessionSetCookie()
    WscrSessionGetCookie()
    WscrSessionDelCookie()
*******************************************************************************/

WscrSessionDelete(long session)
{
    char    buf[256];

    sprintf(buf, "%sSS%ld.ses", WSCR_SESSION_DIR, session);
    return(unlink(buf));

/*
    if(IS_open_files(scr_sess, 0L)) return(-1);
    scr_sess_NUM = session;
    if(OBJ_search(scr_sess_num_i, SCR_EQ)) return(-1);
    IS_delete(scr_sess);
    retur(0);

*/
}

/******************************************************************************
Vérifie la validité d'une session.

La variable WSCR_SESSION_DIR contient un pointeur vers le préfixe du nom des fichiers
servant de support aux sessions.

&RT
&EN 0 : 0k
&EN -1 : session non trouvée
&EN -2 : session expirée


&EX
    long    sessnb;

    WSCR_SESSION_DIR = "c:\\tmp\\";
    sessnb = WscrSessionCreate(300); // La session est valable pour 5 minutes
    switch(WscrSessionCheck(sessnb)) {
	case 0  : printf("Session OK\n"); break;
	case -1 : printf("Session inexistante\n"); break;
	case -2 : printf("Délai expiré\n"); break;
	}

&TX

&SA WscrSessionDelete(),
    WscrSessionCreate()
    WscrSessionSetCookie()
    WscrSessionGetCookie()
    WscrSessionDelCookie()
*******************************************************************************/

WscrSessionCheck(long session)
{
    char    buf[256];
    FILE    *fd;
    long    crdate, crtime, csec, asec, secs;

/*
    if(IS_open_files(scr_sess, 0L)) return(-1);
    scr_sess_NUM = session;
    if(OBJ_search(scr_sess_num_i, SCR_EQ)) return(-1);
    csec = DT_current_sec();
    if(csec > asec + secs) {
	fclose(fd);
	return(-2); // Délai dépassé
	}
    scr_sess_ASEC = csec;
    if(IS_rewrite(scr_sess)) return(-3);
    return(0);
*/

    sprintf(buf, "%sSS%ld.ses", WSCR_SESSION_DIR, session);
    fd = fopen(buf, "r+");
    if(fd == 0) {
	Debug("WscrSessionCheck(%ld); fichier de session non trouvé\n", session);
	return(-1);
    }
    fscanf(fd, "%10ld,%10ld,%10ld,%10ld", &crdate, &crtime, &asec, &secs);

    csec = DT_current_sec();
    if(csec > asec + secs) {
	fclose(fd);
	return(-2); // Délai dépassé
	}

    fseek(fd, 0L, 0);
    asec = DT_current_sec();
    fprintf(fd, "%10ld,%10ld,%10ld,%10ld", crdate, crtime, csec, secs);
    fclose(fd);
    return(0);
}

/******************************************************************************
Ajoute une variable à une session. Si cette variable existe, elle est remplacée.
La variable ne peut dépasser 7500 bytes. Celle-ci est codée en base 64 pour
permettre le stockage sur n'importe quel support dans le futur (SQL p.exe).

Le nom de la variable est case insensitive.

La variable WSCR_SESSION_DIR contient un pointeur vers le préfixe du nom des fichiers
servant de support aux sessions.

&NO Dans le cas des strings, il est préférable de donner la longueur de toute la
variable, y compris le zéro de fin. Sinon, la valeur lue par la suite ne contiendra
pas le zéro final.

&RT 0 en cas de succès, -1 en cas d'erreur (session non trouvée ou incorrecte)

&EX
    long    sessnb;

    WSCR_SESSION_DIR = "c:\\tmp\\";
    sessnb = WscrSessionCreate(300); // La session est valable pour 5 minutes
    switch(WscrSessionCheck(sessnb)) {
	case 0  : printf("Session OK\n"); break;
	case -1 : printf("Session inexistante\n"); break;
	case -2 : printf("Délai expiré\n"); break;
	}
    WscrSessionSetCookie(sessnb, "NOM", "Jean-Marc Paul", 15);
    WscrSessionDelete(sessnb);
&TX

&SA WscrSessionCreate(),
    WscrSessionCheck()
    WscrSessionSetCookie()
    WscrSessionGetCookie()
    WscrSessionDelCookie()
*******************************************************************************/


WscrSessionSetCookie(long session, char *name, U_ch *value, int length)
{
    char    filename[256];
    U_ch    *buf;
    U_ch    **tbl;
    int     i, nb, lg;

    sprintf(filename, "%sSS%ld.ses", WSCR_SESSION_DIR, session);
    tbl = WscrSessionReadFile(filename);
    nb = SCR_tbl_size(tbl);
    if(nb < 1) {
	Debug("Pas de fichier '%s'\n", filename);
	return(-1);
    }
    if(tbl[nb - 1][0] == 0) {
	nb--;
	SCR_free(tbl[nb]);
	tbl[nb] = 0;
	}

    buf = SCR_malloc(length * 2);
    sprintf(buf, "%s-%d=", name, length);
    SCR_code64(value, length, buf + strlen(buf));

    lg = strlen(name);
    for(i = 1 ; i < nb ; i++) {
	if(SCR_cstrncmp(tbl[i], name, lg) == 0 && tbl[i][lg] == '-') {
	    SCR_free(tbl[i]);
	    break;
	    }
	}

    tbl[i] = SCR_stracpy(buf);
    if(i == nb) {
	nb++;
	SCR_add_ptr(&tbl, &nb, 0L);
	}

    WscrSessionSaveFile(filename, tbl);
    SCR_free_tbl(tbl);
    SCR_free(buf);
    return(0);
}

/******************************************************************************
Lit une variable et sa longueur dans une session. Le nom est case insensitive.

La variable WSCR_SESSION_DIR contient un pointeur vers le préfixe du nom des fichiers
servant de support aux sessions.

&RT 0 en cas de succès, -1 en cas d'erreur (session non trouvée ou
incorrecte, ou variable non trouvée)

&EX
    long    sessnb;
    int     lg;
    char    buf[128];

    WSCR_SESSION_DIR = "c:\\tmp\\";
    sessnb = WscrSessionCreate(300); // La session est valable pour 5 minutes
    switch(WscrSessionCheck(sessnb)) {
	case 0  : printf("Session OK\n"); break;
	case -1 : printf("Session inexistante\n"); break;
	case -2 : printf("Délai expiré\n"); break;
	}
    WscrSessionSetCookie(sessnb, "NOM", "Jean-Marc Paul", 15);
    WscrSessionGetCookie(sessnb, "NOM", buf, &lg);
    WscrSessionDelete(sessnb);
&TX

&SA WscrSessionCreate(),
    WscrSessionCheck()
    WscrSessionSetCookie()
    WscrSessionGetCookie()
    WscrSessionDelCookie()
*******************************************************************************/

WscrSessionGetCookie(long session, char *name, char *value, int *length)
{
    char    filename[256], buf[4096];
    U_ch    **tbl;
    int     i, nb, lg, pos = -1;

    sprintf(filename, "%sSS%ld.ses", WSCR_SESSION_DIR, session);
    tbl = WscrSessionReadFile(filename);
    nb = SCR_tbl_size(tbl);
    if(nb < 1) {
	Debug("WscrSessionGetCookie(%ld, %s); fichier de session vide\n", session, name);
	return(-1);
    }
    lg = strlen(name);
    for(i = 1 ; i < nb ; i++) {
	if(SCR_cstrncmp(tbl[i], name, lg) == 0 && tbl[i][lg] == '-') {
	    *length = atol(tbl[i] + lg + 1);
	    pos = U_pos('=', tbl[i]);
	    if(*length == 0 || pos < 0) break;
	    SCR_decode64(tbl[i] + pos + 1, *length, value);
	    break;
	    }
	}

    SCR_free_tbl(tbl);
    if(i == nb || pos < 0) {
	Debug("WscrSessionGetCookie(%ld, %s); page non trouvée dans le fichier de session\n", session, name);
	return(-1);
    }
    return(0);
}

/******************************************************************************
Détruit une variable dans une session. Le nom est case insensitive.

La variable WSCR_SESSION_DIR contient un pointeur vers le préfixe du nom des fichiers
servant de support aux sessions.

&RT 0 en cas de succès, -1 en cas d'erreur (session non trouvée ou
incorrecte, ou variable non trouvée)

&EX
    long    sessnb;
    int     lg;
    char    buf[128];

    WSCR_SESSION_DIR = "c:\\tmp\\";
    sessnb = WscrSessionCreate(300); // La session est valable pour 5 minutes
    switch(WscrSessionCheck(sessnb)) {
	case 0  : printf("Session OK\n"); break;
	case -1 : printf("Session inexistante\n"); break;
	case -2 : printf("Délai expiré\n"); break;
	}
    WscrSessionSetCookie(sessnb, "NOM", "Jean-Marc Paul", 15);
    WscrSessionGetCookie(sessnb, "NOM", buf, &lg);
    WscrSessionDetCookie(sessnb, "NOM");
    WscrSessionDelete(sessnb);
&TX

&SA WscrSessionCreate(),
    WscrSessionCheck()
    WscrSessionSetCookie()
    WscrSessionGetCookie()
    WscrSessionDelCookie()
*******************************************************************************/

WscrSessionDelCookie(long session, char *name)
{
    char    filename[256];
    U_ch    **tbl;
    int     i, nb, lg, pos = -1;

    sprintf(filename, "%sSS%ld.ses", WSCR_SESSION_DIR, session);
    tbl = WscrSessionReadFile(filename);
    nb = SCR_tbl_size(tbl);
    if(nb < 1) return(-1);

    lg = strlen(name);
    for(i = 1 ; i < nb ; i++) {
	if(memcmp(tbl[i], name, lg) == 0 && tbl[i][lg] == '-') {
	    SCR_free(tbl[i]);
	    tbl[i] = SCR_stracpy("");
	    break;
	    }
	}

    WscrSessionSaveFile(filename, tbl);
    SCR_free_tbl(tbl);
    if(i == nb) return(-1);
    return(0);
}



/******************************************************************************
Ajoute une PAGE (le record) une session. Si cette PAGE existe das la
session, elle est remplacée. La taille du record de la page ne peut dépasser
7500 bytes. Celle-ci est codée en base 64 pour permettre le stockage sur
n'importe quel support dans le futur (SQL p.exe).

Le nom de la PAGE est case insensitive.

La variable WSCR_SESSION_DIR contient un pointeur vers le préfixe du nom des fichiers
servant de support aux sessions.

&RT 0 en cas de succès, -1 en cas d'erreur (session non trouvée ou incorrecte)

&EX
    long    sessnb;

    WSCR_SESSION_DIR = "c:\\tmp\\";
    sessnb = WscrSessionCreate(300); // La session est valable pour 5 minutes
    switch(WscrSessionCheck(sessnb)) {
	case 0  : printf("Session OK\n"); break;
	case -1 : printf("Session inexistante\n"); break;
	case -2 : printf("Délai expiré\n"); break;
	}
    WscrSessionSavePage(sessnb, mypage);
    WscrSessionDelete(sessnb);
&TX

&SA WscrSessionCreate(),
    WscrSessionCheck()
    WscrSessionSetCookie()
    WscrSessionGetCookie()
    WscrSessionDelCookie()
    WscrSessionLoadPage()
*******************************************************************************/

WscrSessionSavePage(long session, PAGE *pg)
{
    char    *name;

    if(session <= 0) {
	Debug("WscrSessionSavePage(); Pas de numéro de session\n");
	return(-1);
    }
    name = SCR_find_ptr_name(pg);
    if(name == 0) {
	Debug("WscrSessionSavePage(); page *pg: non trouvée\n");
	return(-1);
    }
    return(WscrSessionSetCookie(session, name,
				   pg->pg_rec,
				   pg->pg_rec_len));
}

/******************************************************************************
Lit une PAGE (le record) d'une session.
Le nom de la PAGE est case insensitive.

La variable WSCR_SESSION_DIR contient un pointeur vers le préfixe du nom des fichiers
servant de support aux sessions.

&RT 0 en cas de succès, -1 en cas d'erreur (session non trouvée ou incorrecte)

&EX
    WSCR_SESSION_DIR = "c:\\tmp\\";
    WscrSessionLoadPage(sessnb, mypage);
&TX

&SA WscrSessionCreate(),
    WscrSessionCheck()
    WscrSessionSetCookie()
    WscrSessionGetCookie()
    WscrSessionDelCookie()
    WscrSessionSavePage()
*******************************************************************************/

WscrSessionLoadPage(long session, PAGE *pg)
{
    char    *name;
    int     lg, rc;
    if(session <= 0) {
	Debug("WscrSessionLoadPage(%ld, pg); mauvais numéro de session\n", session);
	return(-1);
    }
    name = SCR_find_ptr_name(pg);
    if(name == 0) {
	Debug("WscrSessionLoadPage(%ld, pg); Page non référencée\n", session);
	return(-1);
    }
    rc = WscrSessionGetCookie(session, name, pg->pg_rec, &lg);
    return(rc);
}























