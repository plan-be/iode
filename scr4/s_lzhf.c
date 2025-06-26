#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "s_strs.h"
#include "s_lzh.h"

char    LZH_MAGIC[] = "SCR4_LZH 1.01\032";
int     LZH_DIRECTION;


/* ====== Interface fichier ======== */

FILE    *LZH_infile, *LZH_outfile;

/*NH*/
LzhGetFile()
{
    return(fgetc(LZH_infile));
}

/*NH*/
LzhPutFile(c)
int     c;
{
    int     rc;

    rc = fputc(c, LZH_outfile);
    if((rc & c) != rc) return(-1);
/*        printf("\nCode=%d - Retour=%d - errno=%d\n", c, rc, errno); */

    return(0);
}

/*NH*/
long LzhSizFile()
{
    long    pos;

    fseek(LZH_infile, 0L, 2);
    pos = ftell(LZH_infile);
    fseek(LZH_infile, 0L, 0);
    return(pos);
}

/*NH*/
LzhOpenFiles(filein, fileoutt, op, pos)
char    *filein, *fileoutt;
int     op, pos;
{
    char    buf[80], fileout[80];
    int     lostch = '!';
/*    extern long SCR_get_app_pos(); /* JMP 04-01-98 */

#ifdef DOS
    LZH_infile = fopen(filein, "rb");
#else
    LZH_infile = fopen(filein, "r");
#endif
    if(LZH_infile == 0) return(-1);
    setvbuf(LZH_infile, NULL, _IOFBF, 8192);
    if(pos && SCR_get_app_pos(LZH_infile, pos) < 0) return(-1);
    if(op == 'd') {
	fread(buf, strlen(LZH_MAGIC), 1, LZH_infile);
	if(memcmp(buf, LZH_MAGIC, strlen(LZH_MAGIC))) goto err;
	lostch = LzhGetFile();
	}
    if(fileoutt) strcpy(fileout, fileoutt);
    else {
	strcpy(fileout, filein);
	fileout[strlen(fileout) - 1] = lostch;
	}
#ifdef DOS
    LZH_outfile = fopen(fileout, "wb+");
#else
    LZH_outfile = fopen(fileout, "w+");
#endif
    if(LZH_outfile == 0) goto err;
    setvbuf(LZH_outfile, NULL, _IOFBF, 8192);
    if(op == 'e') {
	fwrite(LZH_MAGIC, strlen(LZH_MAGIC), 1, LZH_outfile);
	LzhPutFile(filein[strlen(filein) - 1]);
	}

    return(0);
err:
    if(LZH_infile)  fclose(LZH_infile);
    if(LZH_outfile) fclose(LZH_outfile);
    return(-1);
}

/* ══════════════════════════════════════════════════════════════════
Fonction de compactage de fichier basée sur l'algorithme de
Lempel-Zev et sur la méthode de Huffmann.

Le nom du fichier à compacter est passé comme argument : filein. Le
fichier résultat porte le même nom sauf en ce qui concerne le dernier
caractère qui est remplacé par un point d'exclamation. Si fileout n'est
pas NULL, le fichier output porte ce nom.

&RT 0 en cas de succès, -1 en cas d'erreur
&SA LzhDecodeFile(), LzhEncodeStr(), LzhEncodeGnl()
────────────────────────────────────────────────────────────────── */

LzhEncodeFile(filein, fileout)
char    *filein, *fileout;
{
    int     rc;

    if(LzhOpenFiles(filein, fileout, 'e', 0)) return(-1);
    rc = LzhEncodeGnl(LzhGetFile, LzhPutFile, LzhSizFile);
    fclose(LZH_infile);
    fclose(LZH_outfile);
    return(rc);
}

/* ══════════════════════════════════════════════════════════════════
Fonction de décompactage de fichier basée sur l'algorithme de Lempel-Zev
et sur la méthode de Huffmann. Le fichier traité doit être le résultat
de la fonction LzhEncodeFile().

Le nom du fichier compacté est passé comme argument : filein. Le fichier
décompacté porte le même nom sauf en ce qui concerne le dernier
caractère qui reprend sa valeur d'avant le compactage. Si fileout n'est
pas NULL, le fichier output porte ce nom.

&RT 0 en cas de succès, -1 en cas d'erreur
&SA LzhEncodeFile(), LzhDecodeStr(), LzhDecodeGnl()
────────────────────────────────────────────────────────────────── */
LzhDecodeFile(filein, fileout)
char    *filein, *fileout;
{
    return(LzhDecodeAppFile(filein, fileout, 0)); /* JMP 29-11-97 */
}

/* ══════════════════════════════════════════════════════════════════
Fonction de décompactage de fichier basée sur l'algorithme de Lempel-Zev
et sur la méthode de Huffmann. Le fichier traité doit être le résultat
de la fonction LzhEncodeFile(). Si le paramètre pos est positif,
il indique que le fichier à décompacté est concaténé au fichier
filein à l'aide de scr4_app à la position pos à partir de la fin
du fichier infile.

Le nom du fichier compacté est passé comme argument : filein. Le fichier
décompacté porte le même nom sauf en ce qui concerne le dernier
caractère qui reprend sa valeur d'avant le compactage. Si fileout n'est
pas NULL, le fichier output porte ce nom.

&RT 0 en cas de succès, -1 en cas d'erreur
&SA LzhEncodeFile(), LzhDecodeStr(), LzhDecodeGnl(), LzhDecodeFile()
────────────────────────────────────────────────────────────────── */
LzhDecodeAppFile(filein, fileout, pos)
char    *filein, *fileout;
int     pos;
{
    int     rc;      /* JMP 29-11-97 */

    if(LzhOpenFiles(filein, fileout, 'd', pos)) return(-1);
    rc = LzhDecodeGnl(LzhGetFile, LzhPutFile);
    fclose(LZH_infile);
    fclose(LZH_outfile);
    return(rc);
}





