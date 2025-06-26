#include <scr4.h>
#include "s_html.h"

YYKEYS JHYY_TBL[] = {
    "A",            JH_A,
    "B",            JH_B,
    "BODY",         JH_BODY,
    "FONT",         JH_FONT,
    "H1",           JH_H1,
    "H2",           JH_H2,
    "H3",           JH_H3,
    "H4",           JH_H4,
    "H5",           JH_H5,
    "H6",           JH_H6,
    "HEAD",         JH_HEAD,
    "HR",           JH_HR,
    "HREF",         JH_HREF,
    "HTML",         JH_HTML,
    "IMG",          JH_IMG,
    "META",         JH_META,
    "NAME",         JH_NAME,
    "P",            JH_P,
    "SRC",          JH_SRC,
    "!STARTCMT",    JH_SCMT,
    "!ENDCMT",      JH_ECMT,
    "U",            JH_U,
    "VAR",          JH_VAR,
    "/",            JH_END,
    "=",            JH_EQ
};

#define JH_NBTAGS  (sizeof(JHYY_TBL) / sizeof(YYKEYS))

/*NH*/
#ifndef GENPROTO
/*NH*/
JHYYstrcmp(pyk1, pyk2)
char  *pyk1, *pyk2;
{
    YYKEYS  *yk1 = (YYKEYS *)pyk1,
	    *yk2 = (YYKEYS *)pyk2;

    return(strcmp((char *)yk1->yk_word, (char *)yk2->yk_word));
}
#endif

/*NH*/
#ifdef SCRCPP
/*NH*/
YYFILE *JHYYopen(
HFILE   *hfd
)
#else
YYFILE *JHYYopen(hfd)
HFILE   *hfd;
#endif
{
    static  sorted = 0;
    YYFILE  *yy;

    if(sorted == 0) {
	qsort(JHYY_TBL, sizeof(JHYY_TBL) / sizeof(YYKEYS), sizeof(YYKEYS), JHYYstrcmp);
	sorted = 1;
	}

    yy = YY_open(hfd->h_buf + 1, JHYY_TBL,
		    sizeof(JHYY_TBL) / sizeof(YYKEYS), YY_MEM);
    return(yy);
}


/* ==================================================================
Recherche le mot-clé tag dans le dernier tar HTML lu par JLex() ou
JHRead(). Le mot-clé peut être en majuscules ou minuscules dans tag ou
dans le tag HTML lu. Il n'y a pas de distinction entre majuscules et
minuscules.

&EN hfd : HFILE * retourné par JHopen()
&EN tag = mot-clé à rechercher

Si le tag est trouvé, la valeur suivant le tag= est sauvée dans
l'élément h_val de la structure hfd.

&RT 0 si le mot est trouvé, -1 sinon

&EX
    HFILE   *hfd;

    hfd = JHopen("myfile.htm");
    if(hfd == 0) return(-1);
    ...
    type = JHLex(hfd);
    if(type == JH_A) {
	if(JHYYFind(hfd, "HREF") == 0)
	    printf("Référence = %s\n", hfd->h_val);
	}
    ...
    JHClose(hfd);
&TX

&SA JHOpen(), JHLex(), JHClose()
===================================================================== */

JHYYFind(hfd, tag)
HFILE   *hfd;
U_ch    *tag;
{
    int     type;
    YYFILE  *yy = JHYYopen(hfd);

    if(yy == 0) return(-1);
    while(1) {
	type = YY_read(yy);
	if(type == YY_EOF) {
	    YY_close(yy);
	    return(-1);
	    }
	if(type != YY_WORD) continue;
	if(SCR_cstrcmp(tag, yy->yy_text)) continue;
	YY_read(yy);
	if(yy->yy_text[0] != '=') {
	    YY_unread(yy);
	    continue;
	    }
	YY_read(yy);
	if(hfd->h_val) SCR_free(hfd->h_val);
	hfd->h_val = SCR_stracpy(yy->yy_text);
	YY_close(yy);
	return(0);
	}
}


/* ==================================================================
Ouvre un fichier HTML pour interprétation à l'aide des fonctions JH*().

&EN filename : nom du fichier HTML

&EX
    HFILE   *hfd;

    hfd = JHopen("myfile.htm");
    if(hfd == 0) return(-1);
    ...
    while(1) {
	type = JHLex(hfd);
	if(type == EOF) break;
	...
	}
    ...
    JHClose(hfd);
&TX

&SA JHYYFind(), JHLex(), JHRead(), JHClose()
===================================================================== */

#ifdef SCRCPP
HFILE *JHOpen(
char    *filename
)
#else
HFILE *JHOpen(filename)
char    *filename;
#endif
{
    FILE    *fd;
    HFILE   *hfd = 0;

    fd = fopen(filename, "r");
    if(fd == 0) return(hfd);
    hfd = (HFILE *)SCR_malloc(sizeof(HFILE));
    if(hfd == 0) {
	fclose(fd);
	return(hfd);
	}
    hfd->h_filename = SCR_stracpy(filename);
    hfd->h_fd = fd;
    return(hfd);
}

/* ==================================================================
Ferme un fichier HTML ouvert à l'aide de JHOpen(). Libère l'espace
alloué par les fonctions de lecture.

&EN hfd : HFILE * retourné par JHOpen()

&EX
    HFILE   *hfd;

    hfd = JHopen("myfile.htm");
    if(hfd == 0) return(-1);
    ...
    while(1) {
	type = JHLex(hfd);
	if(type == EOF) break;
	...
	}
    ...
    JHClose(hfd);
&TX

&SA JHYYFind(), JHLex(), JHRead(), JHOpen()
===================================================================== */

int JHClose(hfd)
HFILE   *hfd;
{
    if(hfd == 0) return(0);
    if(hfd->h_fd) fclose(hfd->h_fd);
    SCR_free(hfd->h_filename);
    SCR_free(hfd->h_buf);
    SCR_free(hfd->h_val);
    SCR_free(hfd);
    return(0);
}

/* ==================================================================
Lit l'entrée suivante dans le fichier HTML ouvert par JHOpen() à l'aide
de la fonction JHRead(). S'il ne s'agit pas d'un tag, retourne 0. Sinon,
interprète ensuite le tag et retourne une valeur l'identifiant.

&EN hfd : HFILE * retourné par JHopen()

&RT
&EN 0 pour du texte simple
&EN 1 pour un tag HTML (<....>)
&EN -1 pour la fin du fichier
&EN pour A         :  JH_A,
&EN pour B         :  JH_B,
&EN pour BODY      :  JH_BODY,
&EN pour FONT      :  JH_FONT,
&EN pour H1        :  JH_H1,
&EN pour H2        :  JH_H2,
&EN pour H3        :  JH_H3,
&EN pour H4        :  JH_H4,
&EN pour H5        :  JH_H5,
&EN pour H6        :  JH_H6,
&EN pour HEAD      :  JH_HEAD,
&EN pour HR        :  JH_HR,
&EN pour HREF      :  JH_HREF,
&EN pour HTML      :  JH_HTML,
&EN pour IMG       :  JH_IMG,
&EN pour META      :  JH_META,
&EN pour NAME      :  JH_NAME,
&EN pour P         :  JH_P,
&EN pour SRC       :  JH_SRC,
&EN pour !STARTCMT :  JH_SCMT,
&EN pour !ENDCMT   :  JH_ECMT,
&EN pour U         :  JH_U,
&EN pour VAR       :  JH_VAR,

Les mêmes valeurs en négatif sont retournées pour un tag de fin:
&EN pour /A  :  -JH_A,
&EN pour /B  :  -JH_B,

Ces constantes sont définies dans s_html.h.

Le texte lu ou le tag HTML se trouvent en sortie dans le buffer
hfd->h_buf.

&EX
    HFILE   *hfd;

    hfd = JHopen("myfile.htm");
    if(hfd == 0) return(-1);
    ...
    while(1) {
	type = JHLex(hfd);
	switch(type) {
	    case EOF : goto fin;
	    case JH_H1 : ...
	    case -JH_H1 : ...
	    ...
	    }
	}
    ...
    JHClose(hfd);
&TX

&SA JHOpen(), JHRead(), JHUnread(), JHClose()
===================================================================== */

JHLex(hfd)
HFILE   *hfd;
{
    int     type, fin = 0, i, len;
    YYFILE  *yy;
    char    buf[50];

    type = JHRead(hfd);
    hfd->h_type = type;
    if(type <= 0) return(type);
    SCR_strlcpy(buf, hfd->h_buf, 30);
    U_ljust_text(buf + 1);
    SCR_upper(buf);
    if(buf[1] == '/') fin = 1;
    U_ljust_text(buf + fin + 1);
    for(i = 0 ; i < JH_NBTAGS ; i++) {
	len = strlen(JHYY_TBL[i].yk_word);
	if(strlen(buf + 1 + fin) < len) continue;
	if(memcmp(buf + 1 + fin, JHYY_TBL[i].yk_word, len)) continue;
	if(SCR_is_anum(buf[len + 1 + fin])) continue;
	type = JHYY_TBL[i].yk_def;
	if(fin) return(type = -type);
	else    return(type);
	}
    return(type);
}

/* ==================================================================
Lit l'entrée suivante dans le fichier HTML ouvert par JHOpen().
Deux éléments sont distingués : texte et tag HTML.

&EN hfd : HFILE * retourné par JHOpen()

&RT
&EN 0 pour du texte simple
&EN 1 pour un tag HTML (<....>)
&EN -1 pour la fin du fichier

Le texte lu ou le tag HTML se trouvent en sortie dans le buffer
hfd->h_buf.

&SA JHOpen(), JHLex(), JHClose()
===================================================================== */

JHRead(hfd)
HFILE   *hfd;
{
    int     ch;

    if(hfd->h_reread) {
	hfd->h_reread = 0;
	return(hfd->h_type);
	}
    JHReset(hfd);
    ch = getc(hfd->h_fd);
    if(ch == EOF) return(hfd->h_type = -1);
    JHAdd(hfd, ch);
    if(ch == '<') {
	while(1) {
	    ch = getc(hfd->h_fd);
	    if(ch == EOF) {
		JHAdd(hfd, 0);
		return(hfd->h_type = 1);
		}
	    JHAdd(hfd, ch);
	    if(ch == '>') {
		JHAdd(hfd, 0);
		return(hfd->h_type = 1);
		}
	    }
	}
    else {
	 while(1) {
	    ch = getc(hfd->h_fd);
	    if(ch == EOF) {
		JHAdd(hfd, 0);
		return(hfd->h_type = 0);
		}
	    if(ch == '<') {
		ungetc(ch, hfd->h_fd);
		JHAdd(hfd, 0);
		return(hfd->h_type = 0);
		}
	    JHAdd(hfd, ch);
	    if(ch == '\n') {
		JHAdd(hfd, 0);
		return(hfd->h_type = 0);
		}
	    }
	}
}

/* ==================================================================
Retient la dernière valeur lue de façon à pouvoir la retourner lors du
prochain appel de JHRead() ou JHLex().

&EN hfd : HFILE * retourné par JHOpen()

&SA JHOpen(), JHLex(), JHRead(), JHClose()
===================================================================== */


int JHUnread(hfd)
HFILE   *hfd;
{
    hfd->h_reread = 1;
    return(0);
}

/*NH*/
int JHReset(hfd)
HFILE   *hfd;
{
    if(hfd->h_maxbufsize == 0) return(0);
    hfd->h_buf[0] = 0;
    hfd->h_bufsize = 0;
    return(0);
}

/*NH*/
int JHAdd(hfd, ch)
HFILE   *hfd;
int     ch;
{
    if(hfd->h_maxbufsize <= hfd->h_bufsize) {
	hfd->h_buf = SCR_realloc(hfd->h_buf, 1, hfd->h_maxbufsize, hfd->h_maxbufsize + 128);
	if(hfd->h_buf == 0) SCR_panic();
	hfd->h_maxbufsize += 128;
	}

    hfd->h_buf[hfd->h_bufsize] = ch;
    hfd->h_bufsize++;
    return(0);
}

/*NH*/
JHCompare(hfd, tag)
HFILE   *hfd;
char    *tag;
{
    unsigned char   buf[32];

    if(strlen(tag) >= strlen(hfd->h_buf)) return(-1);
    SCR_strlcpy(buf, hfd->h_buf, 30);
    SCR_sqz(buf);
    SCR_upper(buf);
    return(memcmp(tag, buf + 1, strlen(tag)));
}

/* ============ GESTION DES FICHIERS HREF =========== */

HREF    **JHRefs = 0;
int     JHRefsNb = 0;

/*NH*/
JHRefFind(filename)
char    *filename;
{
    int     i;

    for(i = 0 ; i < JHRefsNb ; i++)
	if(strcmp(JHRefs[i]->hr_filename, filename) == 0) return(i);
    return(-1);
}

/*NH*/
int JHRefFree()
{
    int     i;

    for(i = 0 ; i < JHRefsNb - 1 ; i++)
	SCR_free(JHRefs[i]->hr_filename);

    SCR_free_tbl((U_ch **)JHRefs);
    JHRefs   = 0;
    JHRefsNb = 0;
    return(0);
}

/*NH*/
int JHRefAdd(filename, type, exist)
char    *filename;
int     type, exist;
{
    int     i;

    i = JHRefFind(filename);
    if(i >= 0) {
	JHRefs[i]->hr_type  = type;
	JHRefs[i]->hr_exist = exist;
	return(0);
	}

    SCR_add_ptr((U_ch ***)&JHRefs, &JHRefsNb, 0L);
    JHRefs[JHRefsNb - 1] = (HREF *) SCR_malloc(sizeof(HREF));
    JHRefs[JHRefsNb - 1]->hr_filename = SCR_stracpy(filename);
    JHRefs[JHRefsNb - 1]->hr_type     = type;
    JHRefs[JHRefsNb - 1]->hr_exist    = exist;
    return(0);
}

/*NH*/
JHRefIsProto(filename)
char    *filename;
{
    int     pos;

    if(U_pos(':', filename) >= 0) return(1);
    return(0);
}

/*NH*/
U_ch *JHRefFullPath(res, current, name)
char    *current, *name, *res;
{
    int     nvcdir, i, pos;
    U_ch    **vcdir, **vname, *tmp, disk[5];
    /* **SCR_vtoms(), *SCR_mtov() /* JMP 04-01-98 */

    if(U_pos(':', name) >= 0) {
	strcpy(res, name);
	SCR_std_filename(res);
	return(res);
	}

    memset(disk , 0, 3);
    pos = U_pos(':', current);
    if(pos > 0) {
	disk[0] = current[0];
	disk[1] = ':';
	current += pos + 1;
	}

    if(name[0] == '#') {  /* JMP 30-12-96 */
	res[0] = 0;
	return(res);
	}

    vname = SCR_vtoms(name, "\\/");
    if(name[0] == '/' || name[0] == '\\')
	vcdir = SCR_vtoms("", "/");
    else
	vcdir = SCR_vtoms(current, "\\/");

    nvcdir = SCR_tbl_size(vcdir);
    if(nvcdir > 0) {
	nvcdir--;
	SCR_free(vcdir[nvcdir]);
	}

    for(i = 0; vname[i] ; i++) {
	if(strcmp(vname[i], ".") == 0) continue;
	if(strcmp(vname[i], "..") == 0) {
	    if(nvcdir > 0) {
		nvcdir--;
		SCR_free(vcdir[nvcdir]);
		}
	    continue;
	    }
	SCR_add_ptr(&vcdir, &nvcdir, vname[i]);
	}

    SCR_add_ptr(&vcdir, &nvcdir, 0L);
    tmp = SCR_mtov(vcdir, '/');
    strcpy(res, disk);
    strcat(res, "/");
    strcat(res, tmp);
    i = U_pos('#', res);
    if(i >= 0) res[i] = 0;
    SCR_free(tmp);
    SCR_free_tbl(vcdir);
    SCR_free_tbl(vname);

    SCR_std_filename(res);
    return(res);
}










