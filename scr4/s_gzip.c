#include "scr4.h"
#include "s_gzutil.h"

/* ══════════════════════════════════════════════════════════════════
Fonction de compactage de string basée sur l'algorithme de PkZip.

&EN instr est le string à compacter. instr peut
    contenir des zéros.
&EN inlen est la longueur de instr
&EN outstr est un pointeur vers le string résultat
    alloué par la fonction.
&EN outlen est un pointeur vers sa longueur.

Le string résultat contient :
&EN la longueur du string d'origine (long)
&EN le string compacté (char *)

&RT 0 en cas de succès, -4 en cas de manque de mémoire.
&SA GzipDecodeStr()
────────────────────────────────────────────────────────────────── */
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

/* ══════════════════════════════════════════════════════════════════
Fonction de décompactage de string compacté à laide de GzipEncodeStr.

&EN instr est le string input généré par GzipEncodeStr().
&EN inlen est la longueur de string compacté
&EN outstr est un pointeur vers le string résultat qui doit être
    suffisamment grand (*(long *)instr contient la taille input)

&EN outlen est un pointeur vers la longueur du résultat

&RT 0 en cas de succès, < 0 en cas d'erreur
&SA GZipEncodeStrNA(), GzipDecodeStr()
────────────────────────────────────────────────────────────────── */
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

/* ══════════════════════════════════════════════════════════════════
Fonction de décompactage de string compacté à laide de GzipEncodeStr.

&EN instr est le string input généré par GzipEncodeStr().
&EN inlen est la longueur de string compacté
&EN outstr est un pointeur
vers le string résultat qui est alloué via SCR_malloc().

&EN outlen est un pointeur ves la longueur du résultat

&RT 0 en cas de succès, < 0 en cas d'erreur
&SA GZipEncodeStr()
────────────────────────────────────────────────────────────────── */
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


/* ══════════════════════════════════════════════════════════════════
Fonction de création d'un fichier compacté par l'algorithme de PkZip.
Les appels suivants à GzipWrite() écriront dans le fichier en compactant.

SCR_GZIP_LEVEL détermine le niveau de compression (1-9).

&EN filename : nom du fichier

&RT GZFILE * cas de succès, NULL en cas d'erreur
&SA GzipOpen(), GzipRead(), GzipWrite(), GzipSeek()
────────────────────────────────────────────────────────────────── */
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

/* ══════════════════════════════════════════════════════════════════
Fonction d'ouverture d'un fichier compacté créé par la fonction GzipCreate() et
des appels successifs à GzipWrite() et/ou GzipPutc().

Les appels suivants à GzipRead() et GzipGetc() liront dans le fichier compacté.

Si le fichier ouvert n'est pas compacté, la lecture se fait comme avec
fopen() et fread().

&EN filename : nom du fichier

&RT GZFILE * cas de succès, NULL en cas d'erreur
&SA GzipOpen(), GzipRead(), GzipWrite(), GzipSeek()
────────────────────────────────────────────────────────────────── */
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

/* ══════════════════════════════════════════════════════════════════
Fonction de fermeture d'un fichier ouvert par la fonction GzipCreate() ou GzipOpen().

&EN GZIPFILE * : file descriptor créé par un appel à GzipCreate() ou GzipOpen()

&RT 0
&SA GzipOpen(), GzipCreate()
────────────────────────────────────────────────────────────────── */
GzipClose(gzf)
GZIPFILE    *gzf;
{
    if(gzf) gzclose(gzf);
    return(0);
}


/* ══════════════════════════════════════════════════════════════════
Fonction d'écriture dans un fichier créé par la fonction GzipCreate().

&EN GZIPFILE * : file descriptor créé par un appel à GzipCreate()
&EN buf  : pointeur vers le buffer à écrire (comprimé) dans le fichier
&EN len : longueur de bytes du buffer (non comprimé)

&RT Nombre de bytes écrits (non compactés).
&SA GzipCreate(), GzipPutc()
────────────────────────────────────────────────────────────────── */
GzipWrite(gzf, buf, len)
GZIPFILE        *gzf;
const voidp     buf;
unsigned int    len;
{
    return(gzwrite((gzFile) gzf, (const voidp) buf, len));
}

/* ══════════════════════════════════════════════════════════════════
Fonction de lecture dans un fichier ouvert par la fonction GzipOpen().

&EN GZIPFILE * : file descriptor créé par un appel à GzipOpen()
&EN buf  : pointeur vers le buffer destiné à recevoir len bytes
&EN len : nombre de bytes à lire du buffer (non comprimé)

&RT Nombre de bytes lus (non compactés).
&SA GzipOpen(), GzipGetc(), GzipSeek()
────────────────────────────────────────────────────────────────── */
GzipRead(gzf, buf, len)
GZIPFILE        *gzf;
U_ch            *buf;
unsigned int    len;
{
    return(gzread((gzFile) gzf, (void *) buf, len));
}

/* ══════════════════════════════════════════════════════════════════
Fonction de positionnement dans un fichier ouvert par la fonction GzipOpen()
(et pas GzipCreate()).

&EN GZIPFILE * : file descriptor créé par un appel à GzipOpen()
&EN offset : position relative (voir whence) (non comprimé)
&EN whence : 0 : relative au début du fichier, 1: relative au caractère courant

Les opérations ont lieu comme si on travaillait sur un fichier non comprimé.
Cette fonction peut être très lente.

&RT -1 en cas d'erreur, la position nouvelle depuis le début du fichier si Ok
&SA GzipOpen(), GzipGetc(), GzipRead()
────────────────────────────────────────────────────────────────── */
long GzipSeek(gzf, offset, whence)
GZIPFILE        *gzf;
long            offset;
int             whence;
{
    return(gzseek((gzFile) gzf, (z_off_t)offset, whence));
}

/* ══════════════════════════════════════════════════════════════════
Fonction de lecture dans un fichier ouvert par la fonction GzipOpen().
Cette fonction lit le byte suivant dans le fichier (non comprimé).

&EN GZIPFILE * : file descriptor créé par un appel à GzipOpen()

&RT Caractère lu ou -1 en cas de fin de fichier ou d'erreur
&SA GzipOpen(), GzipRead(), GzipSeek()
────────────────────────────────────────────────────────────────── */
GzipGetc(gzf)
GZIPFILE    *gzf;
{
    return(gzgetc((gzFile) gzf));
}

/* ══════════════════════════════════════════════════════════════════
Fonction d'écriture d'un byte dans un fichier créé par la fonction GzipCreate().

&EN GZIPFILE * : file descriptor créé par un appel à GzipCreate()
&EN ch : caractère à placer dans le fichier

&RT Le caractère écrit ou -1 en cas d'erreur
&SA GzipCreate(), GzipWrite()
────────────────────────────────────────────────────────────────── */
GzipPutc(gzf, ch)
GZIPFILE    *gzf;
int         ch;
{
    return(gzputc((gzFile) gzf, ch));
}



