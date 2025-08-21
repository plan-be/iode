#ifndef UNIX
#include <io.h>
#endif

#include "s_file.h"
#include "s_filein.h"

int     FI_nmagic = 0;        /* number of valid magic[]s  */
struct  FI_magic *FI_magic;   /* array of magic entries    */
int     FI_next = 0;          /* number of valid ext[]s     */
struct  FI_ext *FI_ext;       /* array of extension entries */
int     FI_cmtlg = 0;

/* ====================================================================
Analyse le fichier inname et retourne son type en fonction des
paramètres passés dans tbl. Un commentaire de longueur maximum lg peut
être retourné dans cmt si cette variable n'est pas nulle.

Le tableau passé comme premier paramètre indique les différents
formats de fichiers reconnus. Le format de chaque ligne du tableau
est déterminé par le premier caractère:

&EN s'il s'agit d'un chiffre, il indique que le format est déterminé
par le contenu du fichier. Le chiffre est un déplacement par rapport au
début du fichier. Après le chiffre se trouve l'entrée que l'on trouve
à cette position dans le fichier: string, short, long, ... Ensuite vient
la valeur de l'entrée. Si cette valeur correspond à ce qui est lu,
le type de fichier est trouvé.

&EN s'il s'agit du caractère >> suivi d'un nombre, cela signifie que
le type déjà trouvé précédemment est complété par une information qui
se trouve nn caractères plus loin dans le fichier (voir IODE).

&EN s'il s'agit d'un ., cela signifie que l'extension du fichier
détermine son type. L'extension est indiquée après le point.

Si le type ne peut être déterminé par les données de tbl, la fonction
recherche elle-même des indications dans le fichier. Dans ce cas,
une valeur négative est retournée par la fonction, valeur correspondant
à des define du fichier s_file.h.

&RT numéro de l'entrée dans tbl si une entrée correspond ou valeur
    prédéfinie sinon

&IT Valeurs prédéfinies
───────────────────
&CO
    #define FI_DIR         -1
    #define FI_DATA        -2
    #define FI_EMPTY       -3
    #define FI_A2M         -4
    #define FI_FORTRAN     -5
    #define FI_ASCII       -6

    #define FI_C           -10
    #define FI_FORT        -11
    #define FI_MAKE        -12
    #define FI_PLI         -13
    #define FI_MACH        -14
    #define FI_ENG         -15
    #define FI_PAS         -16
    #define FI_MAIL        -17
    #define FI_NEWS        -18

    #define FI_UNKNOWN      -100
    #define FI_UNFOUND      -101
&TX

&EX
    char **MAGIC_DATA =  {
       "1 string MakerFile FrameMaker document   ",
       "1 string MIFFile  FrameMaker MIF file    ",
       "1 string MakerDic FrameMaker Dictionary  ",
       "1 string  Book FrameMaker Book file      ",
       "0 string MZ DOS executable (EXE)         ",
       "0 string  \037\213 gzip compressed data  ",
       "0 string  \037\235        compressed data",
       "0 string  %!  PostScript document        ",
       "1 string  HTML HTML document             ",
       "1 string  html HTML document             ",
       "2 string  rtf  RTF document              ",
       "0 string KOBJS IODE                      ",
       ">19  short  0  CMT file                  ",
       ">19  short  1  EQS file                  ",
       ">19  short  2  IDT file                  ",
       ">19  short  3  LST file                  ",
       ">19  short  4  Scalar file                  ",
       ">19  short  5  TBL file                  ",
       ">19  short  6  VAR file                  ",
       ".obj Object Module                       ",
       ".lib Library                             ",
       ".xls Excel file                          ",
       ".doc Word File                           ",
       ".mdb Access Database                     ",
       ".scr SCR/AL1 objects                     ",
       ".cmt SCR/AL1 data                        ",
       ".f SCR/AL1 source                        ",
	0
	};

    CalcFileType(fname)
    char    *fname;
    {
	return(FI_file(MAGIC_DATA, fname, 0L, 0));
    }
&TX

&NO
En Unix, de nombreuses définitions de formats peuvent être trouvées
dans le fichier /etc/magic.
=======================================================================*/

int FI_file(tbl, inname, cmt, lg)
char    **tbl;
char    *inname, *cmt;
int     lg;
{
#if !defined(INTAU) && !defined(XENIX)        /* BP_M 28-01-97 */
    int     fd = 0, type = FI_UNKNOWN;
    static  char stdname[] = "standard input";
    unsigned char   buf[HOWMANY+1]; /* one extra for terminating '\0' */
    struct utimbuf  utbuf;
    struct stat     sb;
    int nbytes = 0;                 /* number of bytes read from a datafile */

    if(FI_magic == 0 && FI_ext == 0) FI_apprentice(tbl);
    if(cmt) cmt[0] = '\0';
    FI_cmtlg = lg;

    if (strcmp("-", inname) == 0) inname = stdname;

    if (inname != stdname) {
	/*
	 * first try judging the file based on its filesystem status
	 */
	type = FI_fsmagic(inname, cmt, &sb);
	if (type != FI_UNKNOWN) return(type);

#ifndef DOS
	if ((fd = open(inname, O_RDONLY)) < 0) {
#else
	if ((fd = open(inname, O_RDONLY | O_BINARY)) < 0) {
#endif
		/* We can't open it, but we were able to stat it. */
		if (sb.st_mode & 0002) FI_strcat(cmt, "writeable, ");
		if (sb.st_mode & 0111) FI_strcat(cmt, "executable, ");
		type = FI_UNKNOWN;
		return(type);
	}
    }


    /*
     * try looking at the first HOWMANY bytes
     */
    if ((nbytes = read(fd, (char *)buf, HOWMANY)) == -1) {
	    fprintf(stderr, "read failed (errno=%d).\n", errno);
	    /*NOTREACHED*/
    }

    if (nbytes == 0) {
	FI_strcat(cmt, "empty");
	return(type = FI_EMPTY);
    }
    else {
	buf[nbytes++] = '\0';   /* null-terminate it */
	type = FI_tryit(inname, cmt, buf, nbytes);
    }

    if (inname != stdname) {
	/*
	 * Try to restore access, modification times if read it.
	 */
	utbuf.actime = sb.st_atime;
	utbuf.modtime = sb.st_mtime;
	(void) utime(inname, &utbuf); /* don't care if loses */
	(void) close(fd);
    }
/*
    free(FI_magic);
    free(FI_ext);
    FI_magic = 0; FI_nmagic = 0;
    FI_ext = 0; FI_next = 0;
*/
    return(type);
#endif          /* BP_M 28-01-97 */
}

/*NH*/
int FI_tryit(inname, cmt, buf, nb)
char    *inname, *cmt;
unsigned char *buf;
int nb;
{
    int type;

    type = FI_softmagic(cmt, buf, nb);
    if(type == FI_UNKNOWN)
	type = FI_extmagic(inname, cmt);
    if(type == FI_DATA)
	type = FI_ascmagic(cmt, buf, nb);

    if(type == FI_DATA) FI_strcat(cmt, "DATA");
    return(type);
}

/*NH*/
int FI_strcat(cmt, string)
char    *cmt, *string;
{
    int     lenstr, lencmt;

    if(cmt == NULL) return(0);
    lenstr = strlen(string);
    lencmt = strlen(cmt);
    if(lenstr > FI_cmtlg - lencmt) lenstr = FI_cmtlg - lencmt;
    strncat(cmt, string, lenstr);
    return(1);
}
