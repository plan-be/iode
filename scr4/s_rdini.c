#include <scr4.h>
#include "s_ini.h"

/* ====================================================================
Ouvre un fichier au format .ini et retourne un pointeur vers
une structure INIFILE allouée.

&EN filename indique le nom du fichier à ouvrir.
&EN usection vaut 1 si les titres des sections doivent être
    automatiquement transposés en majuscules.
&EN uparam indique la même chose pour les lignes
    de paramètres internes aux sections.

&RT pointeur vers une structure INIFILE

&PROTO
    #include <s_ini.h>

&EX
    DwReadRules(filename)
    char    *filename;
    {
	INIFILE *inif;

	vtclose();
	inif = IniOpen(filename, 1, 0);
	if(inif == 0) return(-1);

	while(1) {
	    switch(IniRead(inif)) {
		case INI_SECTION :
		    if(memcmp(inif->if_section, "RULES", 5) == 0)
			DwReadRules_1(inif);
		    break;

		case -1:
		    IniClose(inif);
		    return(0);

		default :
		    break;
		}
	    }
    }

&TX
&SA IniClose(), UniRead(), IniUnRead(), IniOpenSection()
=======================================================================*/

#ifdef __cplusplus
INIFILE *IniOpen(
char    *filename,
int     usection,
int     uparam
)
#else
INIFILE *IniOpen(filename, usection, uparam)
char    *filename;
int     usection;
int     uparam;
#endif
{
    INIFILE    *inif = 0;
    FILE        *fd;

    fd = ISC_fopen(filename, "r");  /* JMP 11-06-02 */
    if(fd == 0) return(inif);
    inif = (INIFILE *)SCR_malloc(sizeof(INIFILE));
    inif->if_fname = SCR_stracpy(filename);
    inif->if_fd = fd;
    inif->if_usection = usection;
    inif->if_uparam = uparam;
    return(inif);
}

/* ====================================================================
Ferme un fichier au format .ini ouvert par IniOpen() ou IniOpenSection().
Libère la structure INIFILE allouée.

&EX
    DwReadRules(filename)
    char    *filename;
    {
	INIFILE *inif;

	vtclose();
	inif = IniOpen(filename, 1, 0);
	if(inif == 0) return(-1);

	while(1) {
	    switch(IniRead(inif)) {
		case INI_SECTION :
		    if(memcmp(inif->if_section, "RULES", 5) == 0)
			DwReadRules_1(inif);
		    break;

		case -1:
		    IniClose(inif);
		    return(0);

		default :
		    break;
		}
	    }
    }

&TX
&SA IniOpen(), UniRead(), IniUnRead(), IniOpenSection()
=======================================================================*/

int IniClose(inif)
INIFILE     *inif;
{
    if(inif == 0) return(0);
    if(inif->if_fd) ISC_fclose(inif->if_fd); /* JMP 11-06-02 */
    SCR_free(inif->if_fname);
    SCR_free(inif->if_section);
    SCR_free(inif->if_param);
    SCR_free(inif);
    return(0);
}

/* ====================================================================
Retient le dernier élément lu par IniRead() pour le prochain appel
à IniRead().

&SA IniOpen(), UniRead(), IniClose(), IniOpenSection()
=======================================================================*/

int IniUnRead(inif)
INIFILE    *inif;
{
    if(inif == 0) return(0);
    inif->if_unread = 1;
    return(0);
}

/* ====================================================================
Lit la ligne suivante dans le fichier .ini décrit par la structure
INIFILE inif. Retourne le type d'élément lu :

&EN INI_SECTION : lignes du type [.....]
&EN INI_PARAM  : autres lignes

Les lignes vides ou commençant par un ; sont sautées. Les blancs
en début ou en fin de lignes sont ignorés (SQZ).

&SA IniOpen(), UniClose(), IniUnRead(), IniOpenSection()
=======================================================================*/

IniRead(inif)
INIFILE    *inif;
{
    U_ch    buf[4096];   /* BP_M 26-09-2008 11:20 */

    if(inif == 0) return(-1);
    if(inif->if_unread) {
	inif->if_unread = 0;
	return(inif->if_type);
	}

ag:
    if(ISC_read_line(inif->if_fd, buf, 4095) == -1) return(-1);
    inif->if_cline++;
    U_ljust_text(buf);
    SCR_strip(buf);
    if(buf[0] == ';' || buf[0] == 0) goto ag;
    if(buf[0] == '[') {
	if(buf[(int)strlen(buf) - 1] == ']') buf[(int)strlen(buf) - 1] = 0;
	IniSetSection(inif, buf + 1);
	return(inif->if_type = INI_SECTION);
	}
    IniSetParam(inif, buf);
    return(inif->if_type = INI_PARAM);
}
/*NH*/
int IniSetData(adr, txt, ucase)
U_ch    **adr;
U_ch    *txt;
int     ucase;
{
    SCR_free(*adr);
    *adr = SCR_stracpy(txt);
    if(ucase) SCR_upper(*adr);
    return(0);
}

/*NH*/
int IniSetParam(inif, txt)
INIFILE *inif;
U_ch     *txt;
{
    IniSetData(&(inif->if_param), txt, inif->if_uparam);
    return(0);
}
/*NH*/
int IniSetSection(inif, txt)
INIFILE *inif;
U_ch     *txt;
{
    IniSetData(&(inif->if_section), txt, inif->if_usection);
    return(0);
}

/* ====================================================================
Ouvre le fichier filename du type .ini et se positionne sur la première
section dont le nom est section.

&EN filename indique le nom du fichier à ouvrir.
&EN section est le nom de la section à rechercher
&EN usection vaut 1 si les titres des sections doivent être
    automatiquement transposés en majuscules (dans ce cas section doit
    être en majuscules)
&EN uparam indique la même chose pour les lignes
    de paramètres internes aux sections.

&PROTO
    #include <s_ini.h>

&EX
    DwReadConfig(filename)
    char    *filename;
    {
	INIFILE *inif;
	int     pos;

	vtclose();
	inif = IniOpenSection(filename, "CONFIGURATION", 1, 0);
	if(inif == 0) return(-1);

	while(IniRead(inif) == INI_PARAM) {
	    pos = U_pos('=', inif->if_param);
	    if(pos < 0) {
		DwPrintf("%s : syntax error (%s)\n",
				IniError(inif), inif->if_param);
		continue;
		}
	    inif->if_param[pos] = 0;
	    SCR_upper(inif->if_param);
	    if(strcmp(inif->if_param, "DBDDIR") == 0)
		DW_DBDDIR = SCR_stracpy(inif->if_param + pos + 1);
	    else if(strcmp(inif->if_param, "HOMEDIR") == 0)
		DW_HOMEDIR = SCR_stracpy(inif->if_param + pos + 1);
	    else DwPrintf("%s : syntax error (%s)\n",
				IniError(inif), inif->if_param);
	    }

	IniClose(inif);
	return(0);
    }
&TX
&SA IniOpen(), UniClose(), IniRead(), IniUnRead(), IniReadSection()
=======================================================================*/

#ifdef __cplusplus
INIFILE *IniOpenSection(
U_ch    *filename,
U_ch    *section,
int     usection,
int     uparam
)
#else
INIFILE *IniOpenSection(filename, section, usection, uparam)
U_ch    *filename;
U_ch    *section;
int     usection, uparam;
#endif

{
    INIFILE *inif;
    int     lg = (int)strlen(section);

    inif = IniOpen(filename, usection, uparam);
    if(inif == 0) return(inif);

    while(1) {
	switch(IniRead(inif)) {
	    case INI_SECTION :
		if(memcmp(inif->if_section, section, lg) == 0)
		    return(inif);
		break;

	    case -1:
		IniClose(inif);
		return((INIFILE *)0);

	    default :
		break;
	    }
	}
}

/* ====================================================================
Ouvre le fichier filename du type .ini et se positionne sur la première
section dont le nom est section. Le nom de la section doit être
fourni en majuscules.

Tous les paramètres de la section sont lus et stockés dans un tableau
de pointeur alloué qui est retourné par la fonction. Ce tableau est
terminé par un pointeur nul.

Le fichier est fermé par la fonction.

&EN filename indique le nom du fichier à ouvrir.
&EN section est le nom de la section à rechercher

En cas d'erreur (fichier non trouvé ou section non présente), un
pointeur nul est retourné par la fonction.

&PROTO
    #include <s_ini.h>

&EX
    U_ch  **DW_CONFIG = 0;

    DwReadConfig(filename)
    char    *filename;
    {
	U_ch    **cnf;

	cnf = IniReadSection(filename, "CONFIGURATION");
	if(cnf == 0) {
	    printf("Section [Configuration] not found in %s\n", filename);
	    return(-1);
	    }
	else {
	    for(i = 0 ; cnf[i]; i++)
		printf("%s\n", cnf[i]);
	    SCR_free_tbl(cnf);
	    return(0);
	    }
    }
&TX
&SA IniOpen(), UniClose(), IniRead(), IniUnRead(), IniReadCurSection()
=======================================================================*/

U_ch  **IniReadSection(filename, section)
char    *filename, *section;
{
    INIFILE *inif;
    U_ch    **lines = 0;

    inif = IniOpenSection(filename, section, 1, 0);
    if(inif == 0) return(lines);
    lines = IniReadCurSection(inif);
    IniClose(inif);
    return(lines);
}

/* ====================================================================
Lit la section courante dans le fichier ouvert décrit par la structure
INIFILE inif. Les lignes de paramètres sont retournées dans un tableau
de pointeurs alloué par la fonction et terminé par un pointeur nul.

&PROTO
    #include <s_ini.h>

&EX
    DwReadRules(filename)
    char    *filename;
    {
	INIFILE *inif;

	inif = IniOpen(filename, 1, 0);
	if(inif == 0) return(-1);

	while(1) {
	    switch(IniRead(inif)) {
		case INI_SECTION :
		    if(memcmp(inif->if_section, "RULES", 5) == 0) {
			lines = IniReadCurSection(inif);
			DwAddRules(lines);
			}
		    break;

		case -1:
		    IniClose(inif);
		    return(0);

		default :
		    break;
		}
	    }
    }

&TX
&SA IniOpen(), UniClose(), IniRead(), IniUnRead(), IniReadSection()
=======================================================================*/


U_ch  **IniReadCurSection(inif)
INIFILE *inif;
{
    U_ch    **lines = 0;
    int     nl = 0;

    if(inif == 0) return(lines);
    while(1) {
	switch(IniRead(inif)) {
	    case -1 :
		SCR_add_ptr(&lines, &nl, 0L);
		return(lines);

	    case INI_SECTION :
		SCR_add_ptr(&lines, &nl, 0L);
		IniUnRead(inif);
		return(lines);

	    case INI_PARAM :
		SCR_add_ptr(&lines, &nl, inif->if_param);
		break;
	    }
	}
}

/* ====================================================================
Retourne le nom du fichier .ini ouvert défini par la structure INIFILE
et la ligne courante dans ce fichier.
Le pointeur retourné est une valeur statique (non modifiable).

&PROTO
    #include <s_ini.h>

&EX
    print("%s : syntax error (%s)\n", IniError(inif), inif->if_param);
&TX
&SA IniOpen(), UniClose(), IniRead(), IniUnRead(), IniReadSection()
=======================================================================*/

U_ch  *IniError(inif)
INIFILE *inif;
{
    static U_ch     buf[80];

    strcpy(buf, "File not open");
    if(inif == 0) return(buf);
    sprintf(buf, "%s[%d]", inif->if_fname, inif->if_cline);
    return(buf);
}

/* ====================================================================
Lit dans le fichier filename la valeur du paramètre parm de la section
section. La définition du paramètre est stocké dans le buffer val dont
la longueur est au minimum de lg caractères.

&EN filename = nom du fichier .ini
&EN section = section (en majuscules) dont le paramètre dit être lu
&EN parm = paramètre à lire
&EN val = pointeur destiné à contenir la valeur du paramètre
&EN lg = longueur maximum du paramètre

&RT 0 si le paramètre est trouvé, -1 sinon

&PROTO
    #include <s_ini.h>

&EX
    Fichier a2m.ini
    ---------------
    [HTML]
    ...
    FontSize=12
    ...
    [MAKER]
    ...
    FontSize=10

    Source
    ------
    char    buf[81];

    if(IniReadTxtParm("a2m.ini", "HTML", "FONTSIZE", buf, 80) == 0)
	printf("HTML Font size=%s\n", buf);
    else
	printf("HTML Font size non présent dans a2m.ini\n");
&TX

&SA IniOpen(), IniClose(), IniReadNumParm(), IniReadSection()
=======================================================================*/

IniReadTxtParm(filename, section, parm, val, lg)
U_ch    *filename, *section, *parm, *val;
int     lg;
{
    INIFILE *inif;
    int     pos;

    inif = IniOpenSection(filename, section, 1, 0);

    if(inif == 0) return(-1);
    while(1) {
	switch(IniRead(inif)) {
	    case -1 :
	    case INI_SECTION :
		IniClose(inif);
		val[0] = 0;
		return(-1);

	    case INI_PARAM :
		pos = U_pos('=', inif->if_param);
		if(pos < 0) break;
		inif->if_param[pos] = 0;
		SCR_upper(inif->if_param);
		SCR_strip(inif->if_param); // JMP 13-9-2015
		if(strcmp(inif->if_param, parm) == 0) {
		    SCR_strlcpy(val, inif->if_param + pos + 1, lg);
		    IniClose(inif);
		    return(0);
		    }
		break;
	    }
	}

}

/* ====================================================================
Lit dans le fichier filename la valeur du paramètre parm de la section
section. La définition du paramètre est stocké dans l'entier dont le
pointeur est passé dans val.

&EN filename = nom du fichier .ini
&EN section = section (en majuscules) dont le paramètre dit être lu
&EN parm = paramètre à lire
&EN val = pointeur vers un entier destiné à contenir la valeur lue

&RT 0 si le paramètre est trouvé, -1 sinon

&PROTO
    #include <s_ini.h>

&EX
    Fichier a2m.ini
    ---------------
    [HTML]
    ...
    FontSize=12
    ...
    [MAKER]
    ...
    FontSize=10

    Source
    ------
    int     fontsize;

    if(IniReadNumParm("a2m.ini", "HTML", "FONTSIZE", &fontsize) == 0)
	printf("HTML Font size=%d\n", fontsize);
    else
	printf("HTML Font size non présent dans a2m.ini\n");
&TX

&SA IniOpen(), IniClose(), IniReadTxtParm(), IniReadSection()
=======================================================================*/

int IniReadNumParm(filename, section, parm, num)
U_ch    *filename, *section, *parm;
int     *num;
{
    char    val[32];

    if(IniReadTxtParm(filename, section, parm, val, 30) == 0)
	*num = atoi(val);
    else
	return(-1);

    return(0);
}

/* ====================================================================
Lit dans le fichier filename la valeur du paramètre parm de la section
section. La définition du paramètre est stocké dans le double dont le
pointeur est passé dans val.

&EN filename = nom du fichier .ini
&EN section = section (en majuscules) dont le paramètre dit être lu
&EN parm = paramètre à lire
&EN val = pointeur vers un double destiné à contenir la valeur lue

&RT 0 si le paramètre est trouvé, -1 sinon

&PROTO
    #include <s_ini.h>

&EX
    Fichier a2m.ini
    ---------------
    [HTML]
    ...
    ColumnWidth=12.25
    ...
    [MAKER]
    ...
    ColumnWidth=12.5

    Source
    ------
    double  width;

    if(IniReadRealParm("a2m.ini", "HTML", "COLUMNWIDTH", &width) == 0)
	printf("HTML Col Width=%lf\n", width);
    else
	printf("HTML Col Width non présent dans a2m.ini\n");
&TX

&SA IniOpen(), IniClose(), IniReadTxtParm(), IniReadSection(), IniReadNumParm()
=======================================================================*/

int IniReadRealParm(filename, section, parm, num)
U_ch    *filename, *section, *parm;
double  *num;
{
    char    val[32];

    if(IniReadTxtParm(filename, section, parm, val, 30) == 0)
	*num = atof(val);
    else
	return(-1);

    return(0);
}

/* ====================================================================
Lit dans le fichier filename la valeur du paramètre parm de la section
section. Cette valeur est interprétée comme Yes ou No. La valeur
calculée est retournée par la fonction.

&EN filename = nom du fichier .ini
&EN section = section (en majuscules) dont le paramètre dit être lu
&EN parm = paramètre à lire

&RT 1 si la valeur du paramètre est yes, oui, ja ou 1 (en minuscules)
    ou majuscules et 0 sinon

&PROTO
    #include <s_ini.h>

&SA IniOpen(), IniClose(), IniReadTxtParm(), IniReadSection()
=======================================================================*/

int IniReadYNParm(char* filename, char* section, char* parm)
{
    char    buf[256];

    if(IniReadTxtParm(filename, section, parm, buf, 255) == 0) {
	SCR_lower(buf);
	SCR_sqz(buf);
	if(U_is_in(buf[0], "yj1o"))      return(1);
	else                             return(0);
	}
    else
	return(-1);
}

/*NH*/
IniIsSection(buf, section)
char    *buf, *section;
{
    char    buf2[512];

    strcpy(buf2, buf);  /* JMP 14-11-2004 */
    U_ljust_text(buf2);
    SCR_strip(buf2);
    if(buf2[0] == '[' && buf2[(int)strlen(buf2) - 1] == ']') {
	if(section == 0) return(1);
	buf2[(int)strlen(buf2) - 1] = 0;
	if(SCR_cstrcmp(buf2 + 1, section) == 0) return(1);
	}
    return(0);
}

/*NH*/
IniIsParm(buf, parm)
char    *buf, *parm;
{
    int     pos;
    char    buf2[512];

    strcpy(buf2, buf);  /* JMP 14-11-2004 */
    U_ljust_text(buf2);
    SCR_strip(buf2);
    pos = U_pos('=', buf2);
    if(pos <= 0) return(0);
    buf2[pos] = 0;
    if(SCR_cstrcmp(buf2, parm) == 0) return(1);
    return(0);
}

/* ====================================================================
Remplace dans un fichier .ini la valeur d'un paramètre.

&EN filename = nom du fichier .ini
&EN section = section (en majuscules) dont le paramètre dit être lu
&EN parm = paramètre à lire
&EN val = nouvelle valeur

&RT
&EN 0 en cas de remplacement
&EN -1 en cas d'erreur à la lecture ou à la création du fichier
&EN -2 en cas de non remplacement (paramètre non trouvé)


&PROTO
    #include <s_ini.h>

&SA IniOpen(), IniClose(), IniReadTxtParm(), IniReadSection()
=======================================================================*/

int IniWriteParm(char* filename, char* section, char* parm, char* val)
{
    FILE    *fd, *fdo;
    char    old[128], buf[512], bufo[512];
    int     ok = -2;

    SCR_change_ext(old, filename, "old");
    ISC_unlink(old); /* JMP 11-06-02 */
    if(ISC_rename(filename, old)) return(-1); /* JMP 11-06-02 */
    fd = ISC_fopen(old, "r");
    if(fd == 0) return(-1);
    fdo = ISC_fopen(filename, "w+");
    if(fdo == 0) {
	ISC_fclose(fd);
	return(-1);
	}

    while(1) {
	if(ISC_read_line(fd, buf, 510) == -1) break;
	strcpy(bufo, buf);
      next:
	if(IniIsSection(buf, section)) {
	    // C'est la bonne section
	    ISC_fwrite(bufo, (int)strlen(bufo), 1, fdo); /* JMP 11-06-02 */
	    ISC_fwrite("\n", 1, 1, fdo);            /* JMP 11-06-02 */
	    while(1) {
		if(ISC_read_line(fd, buf, 510) == -1) {
		    sprintf(buf, "%s=%s\n", parm, val);
		    ISC_fwrite(buf, (int)strlen(buf), 1, fdo);
		    ok = 0;
		    break;
		    }
		strcpy(bufo, buf);
		if(IniIsSection(buf, 0L)) {
		    /* C'était la bonne, c'est une nouvelle section qui commence : on ajoute une ligne */
		    if(ok != 0) {
			sprintf(buf, "%s=%s\n", parm, val);         /* JMP 03-10-02 */
			ISC_fwrite(buf, (int)strlen(buf), 1, fdo);       /* JMP 03-10-02 */
			ok = 0;
			}
		    goto next;
		    }
		if(IniIsParm(buf, parm)) {
		    /* C'est le bon paramètre : on le remplace */
		    /* ISC_fprintf(fdo, "%s=%s\n", parm, val); */
		    sprintf(buf, "%s=%s\n", parm, val);
		    ISC_fwrite(buf, (int)strlen(buf), 1, fdo);
		    ok = 0;
		    break;
		    }
		else {
		    /* C'est un autre paramètre : on continue */
		    /* ISC_fprintf(fdo, "%s\n", bufo);       */
		    sprintf(buf, "%s\n", bufo);
		    ISC_fwrite(buf, (int)strlen(buf), 1, fdo);
		    }
		}
	    }
	else {
	    /* ISC_fprintf(fdo, "%s\n", bufo); */
	    sprintf(buf, "%s\n", bufo);
	    ISC_fwrite(buf, (int)strlen(buf), 1, fdo);
	    }
	}
    if(ok == -2) {
	/* On n'a pas trouvé la section /* JMP 03-10-02 */
	sprintf(buf, "\n[%s]\n", section);
	ISC_fwrite(buf, (int)strlen(buf), 1, fdo);
	sprintf(buf, "%s=%s\n", parm, val);
	ISC_fwrite(buf, (int)strlen(buf), 1, fdo);
	ok = 0;
	}
    ISC_fclose(fd);
    ISC_fclose(fdo);
    return(ok);
}

/* ====================================================================
Lit le nom de toutes les sections du fichier filename du type .ini.
Retourne un tableau de pointeurs vers les noms des sections.

&EN filename indique le nom du fichier à ouvrir.

&PROTO
    #include <s_ini.h>

&SA IniOpen(), UniClose(), IniRead(), IniUnRead(), IniReadSection()
=======================================================================*/

#ifdef __cplusplus
U_ch **IniReadAllSections(
U_ch    *filename
)
#else
U_ch **IniReadAllSections(filename)
U_ch    *filename;
#endif
{
    INIFILE *inif;
    U_ch    **sects = 0, **sect;
    int     nbsects = 0;

    inif = IniOpen(filename, 1, 1);
    if(inif == 0) return(sects);

    while(1) {
	switch(IniRead(inif)) {
	    case INI_SECTION :
		SCR_add_ptr(&sects, &nbsects, inif->if_section);
		sect = IniReadCurSection(inif);
		SCR_free_tbl(sect);
		break;

	    case -1:
		IniClose(inif);
		SCR_add_ptr(&sects, &nbsects, 0L);
		return(sects);

	    default :
		break;
	    }
	}
}









