#include "scr4.h"
#include "s_gzutil.h"

/* ������������������������������������������������������������������
Fonction de compactage de string bas�e sur l'algorithme de PkZip.

&EN instr est le string � compacter. instr peut
    contenir des z�ros.
&EN inlen est la longueur de instr
&EN outstr est un pointeur vers le string r�sultat
    allou� par la fonction.
&EN outlen est un pointeur vers sa longueur.

Le string r�sultat contient :
&EN la longueur du string d'origine (long)
&EN le string compact� (char *)

&RT 0 en cas de succ�s, -4 en cas de manque de m�moire.
&SA GzipDecodeStr()
������������������������������������������������������������������ */
int GzipEncodeStr(unsigned char* instr, unsigned long inlen, unsigned char** outstr, unsigned long* outlen)
{
    int             rc;

    *outlen = 14 + inlen * 1.1;
    *outstr = SCR_malloc(sizeof(long) + *outlen);
    rc = compress2((*outstr) + sizeof(long), outlen, instr, inlen, SCR_GZIP_LEVEL);
    if(rc) {
	SCR_free(*outstr);
	*outstr = 0;
	*outlen = 0;
	return(rc);
	}
    *outlen += sizeof(long);
    memcpy(*outstr, &inlen, sizeof(long));
    return(0);
}

/* ������������������������������������������������������������������
Fonction de d�compactage de string compact� � laide de GzipEncodeStr.

&EN instr est le string input g�n�r� par GzipEncodeStr().
&EN inlen est la longueur de string compact�
&EN outstr est un pointeur vers le string r�sultat qui doit �tre
    suffisamment grand (*(long *)instr contient la taille input)

&EN outlen est un pointeur vers la longueur du r�sultat

&RT 0 en cas de succ�s, < 0 en cas d'erreur
&SA GZipEncodeStrNA(), GzipDecodeStr()
������������������������������������������������������������������ */
GzipDecodeStrNA(instr, inlen, outstr)
unsigned char   *instr, *outstr;
unsigned long   inlen;
{
    int             rc;
    long            outlen;

    outlen = ((long *)instr)[0];
    rc = uncompress(outstr, &outlen, instr + sizeof(long), inlen - sizeof(long));
    return(rc);
}

/* ������������������������������������������������������������������
Fonction de d�compactage de string compact� � laide de GzipEncodeStr.

&EN instr est le string input g�n�r� par GzipEncodeStr().
&EN inlen est la longueur de string compact�
&EN outstr est un pointeur
vers le string r�sultat qui est allou� via SCR_malloc().

&EN outlen est un pointeur ves la longueur du r�sultat

&RT 0 en cas de succ�s, < 0 en cas d'erreur
&SA GZipEncodeStr()
������������������������������������������������������������������ */
int GzipDecodeStr(unsigned char* instr, unsigned long inlen, unsigned char** outstr, unsigned long* outlen)
{
    int             rc;

    *outlen = ((long *)instr)[0];
    *outstr = SCR_malloc(*outlen);
    rc = uncompress(*outstr, outlen, instr + sizeof(long), inlen - sizeof(long));
    if(rc) {
	SCR_free(*outstr);
	*outlen = 0;
	}
    return(rc);
}


/* ������������������������������������������������������������������
Fonction de cr�ation d'un fichier compact� par l'algorithme de PkZip.
Les appels suivants � GzipWrite() �criront dans le fichier en compactant.

SCR_GZIP_LEVEL d�termine le niveau de compression (1-9).

&EN filename : nom du fichier

&RT GZFILE * cas de succ�s, NULL en cas d'erreur
&SA GzipOpen(), GzipRead(), GzipWrite(), GzipSeek()
������������������������������������������������������������������ */
GZIPFILE *GzipCreate(filename)
unsigned char   *filename;
{
    char        buf[10];
    gzFile      gzf;

#ifdef UNIX
    sprintf(buf, "w%d", SCR_GZIP_LEVEL);
#else
    sprintf(buf, "wb%d", SCR_GZIP_LEVEL);
#endif
    gzf = gzopen(filename, buf);
    return((GZIPFILE *) gzf);
}

/* ������������������������������������������������������������������
Fonction d'ouverture d'un fichier compact� cr�� par la fonction GzipCreate() et
des appels successifs � GzipWrite() et/ou GzipPutc().

Les appels suivants � GzipRead() et GzipGetc() liront dans le fichier compact�.

Si le fichier ouvert n'est pas compact�, la lecture se fait comme avec
fopen() et fread().

&EN filename : nom du fichier

&RT GZFILE * cas de succ�s, NULL en cas d'erreur
&SA GzipOpen(), GzipRead(), GzipWrite(), GzipSeek()
������������������������������������������������������������������ */
GZIPFILE *GzipOpen(filename)
unsigned char   *filename;
{
    char        buf[10];
    gzFile      gzf;

#ifdef UNIX
    sprintf(buf, "r");
#else
    sprintf(buf, "rb");
#endif

    gzf = gzopen(filename, buf);
    return((GZIPFILE *) gzf);
}

/* ������������������������������������������������������������������
Fonction de fermeture d'un fichier ouvert par la fonction GzipCreate() ou GzipOpen().

&EN GZIPFILE * : file descriptor cr�� par un appel � GzipCreate() ou GzipOpen()

&RT 0
&SA GzipOpen(), GzipCreate()
������������������������������������������������������������������ */
GzipClose(gzf)
GZIPFILE    *gzf;
{
    if(gzf) gzclose(gzf);
    return(0);
}


/* ������������������������������������������������������������������
Fonction d'�criture dans un fichier cr�� par la fonction GzipCreate().

&EN GZIPFILE * : file descriptor cr�� par un appel � GzipCreate()
&EN buf  : pointeur vers le buffer � �crire (comprim�) dans le fichier
&EN len : longueur de bytes du buffer (non comprim�)

&RT Nombre de bytes �crits (non compact�s).
&SA GzipCreate(), GzipPutc()
������������������������������������������������������������������ */
GzipWrite(gzf, buf, len)
GZIPFILE        *gzf;
const voidp     buf;
unsigned int    len;
{
    return(gzwrite((gzFile) gzf, (const voidp) buf, len));
}

/* ������������������������������������������������������������������
Fonction de lecture dans un fichier ouvert par la fonction GzipOpen().

&EN GZIPFILE * : file descriptor cr�� par un appel � GzipOpen()
&EN buf  : pointeur vers le buffer destin� � recevoir len bytes
&EN len : nombre de bytes � lire du buffer (non comprim�)

&RT Nombre de bytes lus (non compact�s).
&SA GzipOpen(), GzipGetc(), GzipSeek()
������������������������������������������������������������������ */
GzipRead(gzf, buf, len)
GZIPFILE        *gzf;
U_ch            *buf;
unsigned int    len;
{
    return(gzread((gzFile) gzf, (void *) buf, len));
}

/* ������������������������������������������������������������������
Fonction de positionnement dans un fichier ouvert par la fonction GzipOpen()
(et pas GzipCreate()).

&EN GZIPFILE * : file descriptor cr�� par un appel � GzipOpen()
&EN offset : position relative (voir whence) (non comprim�)
&EN whence : 0 : relative au d�but du fichier, 1: relative au caract�re courant

Les op�rations ont lieu comme si on travaillait sur un fichier non comprim�.
Cette fonction peut �tre tr�s lente.

&RT -1 en cas d'erreur, la position nouvelle depuis le d�but du fichier si Ok
&SA GzipOpen(), GzipGetc(), GzipRead()
������������������������������������������������������������������ */
long GzipSeek(gzf, offset, whence)
GZIPFILE        *gzf;
long            offset;
int             whence;
{
    return(gzseek((gzFile) gzf, (z_off_t)offset, whence));
}

/* ������������������������������������������������������������������
Fonction de lecture dans un fichier ouvert par la fonction GzipOpen().
Cette fonction lit le byte suivant dans le fichier (non comprim�).

&EN GZIPFILE * : file descriptor cr�� par un appel � GzipOpen()

&RT Caract�re lu ou -1 en cas de fin de fichier ou d'erreur
&SA GzipOpen(), GzipRead(), GzipSeek()
������������������������������������������������������������������ */
GzipGetc(gzf)
GZIPFILE    *gzf;
{
    return(gzgetc((gzFile) gzf));
}

/* ������������������������������������������������������������������
Fonction d'�criture d'un byte dans un fichier cr�� par la fonction GzipCreate().

&EN GZIPFILE * : file descriptor cr�� par un appel � GzipCreate()
&EN ch : caract�re � placer dans le fichier

&RT Le caract�re �crit ou -1 en cas d'erreur
&SA GzipCreate(), GzipWrite()
������������������������������������������������������������������ */
GzipPutc(gzf, ch)
GZIPFILE    *gzf;
int         ch;
{
    return(gzputc((gzFile) gzf, ch));
}



