#include <stdio.h>

char    *DebugFilename = 0;

int     DebugActif = 1;     /* BP_M 02-09-1997 */
FILE    *DebugFd = 0;

/*NH /* JMP 02-12-98 */
int DebugOpen()
{
    if(DebugFd) return(0);
    if(!DebugActif) return(-1);
    if(DebugFilename == 0) DebugFilename = "debug.win";
    if(DebugFilename[0] == 0) return(-1);  /* JMP 06-07-99 */
    DebugFd = fopen(DebugFilename, "a+");
    if(DebugFd == 0) return(-1);
    return(0);
}

/*NH JMP 02-12-98 */
int DebugClose()
{
    if(DebugFd == 0) return(0);
    fclose(DebugFd);
    DebugFd = 0;
    return(0);
}

/*NH JMP 02-12-98 */
int DebugPrint(txt, a, b, c, d, e)
char    *txt, *a, *b, *c, *d, *e;
{
    if(txt == 0) return(0);
    if(DebugOpen()) return(-1);
    fprintf(DebugFd, txt, a, b, c, d, e);
    return(0);
}

/* =====================================================================
Cette fonction permet d'accumuler des messages dans un fichier local (en
g‚n‚ral … des fins de test ou de debugging. Elle prend des paramŠtres
comparables … ceux de ~cprintf()~C.

Sa syntaxe est identique … celle de printf. Elle ‚crit par d‚faut dans
le fichier ~cdebug.win~C.

Le fichier est … chaque fois ouvert et ferm‚ pour permettre d'arrˆter le
programme n'importe tout en conservant les r‚sultats dans le fichier.
Cela peut ˆtre un facteur de lenteur dans les applications.

La variable globale
&CO
    int DebugActif
&TX
permet d'annuler temporairement l'output dans un fichier. Il suffit de
fixer sa valeur … 0.

&TI Nom du fichier
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
On peut changer le nom du fichier en fixant la variable
&CO
    char  *DebugFilename;
&TX
Si cette variable est nulle, le fichier est ~cdebug.win~C. Sinon, elle
contient le nom du fichier contenant les messages.

Comme le fichier est … chaque appel ouvert et ferm‚, on peut sans problŠme
changer le nom du fichier … n'importe quel moment.
========================================================*/

int Debug(txt, a, b, c, d, e)
char    *txt, *a, *b, *c, *d, *e;
{
    static  indebug = 0;

    if(indebug) return(0);
    indebug = 1;
    if(DebugPrint(txt, a, b, c, d, e)) {
	indebug = 0; /* JMP 19-12-00 */
	return(-1);
	}
    DebugClose();
    indebug = 0;
    return(0);
}

/* =====================================================================
Cette fonction effectue un Debug() en for‡ant localement DebugActif … 1 et
en remettant … la valeur initiale aprŠs le traitement.
========================================================*/

int DebugForce(txt, a, b, c, d, e)
char    *txt, *a, *b, *c, *d, *e;
{
    int odeb = DebugActif;

    DebugActif = 1;
    Debug(txt, a, b, c, d, e);
    DebugActif = odeb;
    return(0);
}

int DebugCPU  = 0;
int DebugCPUT = -1;

/* =====================================================================
D‚marre une s‚quence DebugB() .. DebugE(). Cette fonction m‚morise le
temps CPU de fa‡on … pouvoir imprimer le temps ‚coul‚ entre DebugB() et
DebugE().

&EX
    DebugB("Impression de %s ...", "toto.txt");
    PrintFile("toto.txt");
    DebugE("temin‚e");

    R‚sultat :
	Impression de toto.txt ...termin‚e [12 ms]

&TX

&SA DebugE(), DebugET(), Debug()
========================================================================*/

int DebugB(txt, a, b, c, d, e)
char    *txt, *a, *b, *c, *d, *e;
{
    char    dt[20], ti[20];

    DebugCPU = WscrGetMS();
    if(DebugCPUT <=0) DebugCPUT = DebugCPU;

    SCR_long_to_fdate(_SCR_current_date(), dt, "dd-mm-yyyy");
    SCR_long_to_ftime(_SCR_current_time(), ti, "hh:mm:ss");
    Debug("%s(%s) : ", dt, ti); /* JMP 02-08-02 */
    return(Debug(txt, a, b, c, d, e));
}

/* =====================================================================
Termine une s‚quence DebugB() .. DebugE(). Cette fonction utilise
l'information m‚moris‚e par DebugB() pour afficher le temps CPU ‚coul‚.

&EX
    DebugB("Impression de %s ...", "toto.txt");
    PrintFile("toto.txt");
    DebugE("temin‚e");

    R‚sultat :
	Impression de toto.txt ...termin‚e [12 ms]

&TX

&SA DebugB(), DebugET(), Debug()
========================================================================*/

int DebugE(txt, a, b, c, d, e)
char    *txt, *a, *b, *c, *d, *e;
{
    int     rc, ncpu;

    rc = Debug(txt, a, b, c, d, e);
    ncpu = WscrGetMS();
    rc = Debug(" [%ld ms]\n", ncpu - DebugCPU);
    DebugCPU = ncpu;
    return(rc);
}

/* =====================================================================
Termine une s‚quence DebugB() .. DebugE[n](). Cette fonction utilise
l'information m‚moris‚e par DebugB() pour afficher le temps CPU ‚coul‚.

A la diff‚rence de DebugE(), il n'y a pas de \n en fin de ligne.

&EX
    DebugB("Impression de %s ...", "toto.txt");
    PrintFile("toto.txt");
    DebugE("temin‚e\n");

    R‚sultat :
	Impression de toto.txt ...termin‚e [12 ms]

&TX

&SA DebugB(), DebugE(), DebugET(), Debug()
========================================================================*/

int DebugEn(txt, a, b, c, d, e)
char    *txt, *a, *b, *c, *d, *e;
{
    int     rc, ncpu;

    rc = Debug(txt, a, b, c, d, e);
    ncpu = WscrGetMS();
    rc = Debug(" [%ld ms]", ncpu - DebugCPU);
    DebugCPU = ncpu;
    return(rc);
}

/* =====================================================================
Termine une s‚quence DebugB() .. DebugET(). Cette fonction utilise
l'information m‚moris‚e par DebugB() pour afficher le temps CPU ‚coul‚ et
le temps total depuis le premier appel … DebugB().

&EX
    DebugB("Impression de %s ...", "toto.txt");
    PrintFile("toto.txt");
    DebugE("temin‚e");

    R‚sultat :
	Impression de toto.txt ...termin‚e [12/1200 ms]

&TX

&SA DebugB(), DebugET(), Debug()
========================================================================*/

int DebugET(txt, a, b, c, d, e)
char    *txt, *a, *b, *c, *d, *e;
{
    int     rc, ncpu;

    rc = Debug(txt, a, b, c, d, e);
    ncpu = WscrGetMS();
    rc = Debug(" [%ld/%ld ms]\n", ncpu - DebugCPU, ncpu - DebugCPUT);
    DebugCPU = ncpu;
    return(rc);
}

/* =====================================================================
Cette fonction imprime en hexad‚cimal dans le fichier debug (voir Debug())
le contenu du string str de longueur lg.

&EN char *title : texte … placer avant la valeur hexa
&EN unsigned char *str : string … imprimer (ou pointeur vers une variable)
&EN int lg : longueur de str

La variable
&CO
    int DebugActif
&TX
permet d'annuler temporairement l'output dans un fichier. Il suffit de
fixer sa valeur … 0.

&TI Nom du fichier
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
On peut changer le nom du fichier en fixant la variable
&CO
    char  *DebugFilename;
&TX
Si cette variable est nulle, le fichier est ~cdebug.win~C. Sinon, elle
contient le nom du fichier contenant les messages.

Comme le fichier est … chaque appel ouvert et ferm‚, on peut sans problŠme
changer le nom du fichier … n'importe quel moment.

&EX
    DebugHexa("Valeur du double : ", &dbl, sizeof(double));

    R‚sultat :
	Valeur du double : 12eaf2df267e129a

&TX

&SA DebugB(), DebugET(), Debug()
========================================================*/

int DebugHexa(char *title, unsigned char *str, int lg)
{
    int     j;

    Debug("%s", title);
    for(j = 0 ; j < lg ; j++)
	Debug("%02.2x", 0xFF & str[j]);

    return(0);
}

/* =====================================================================
Cette fonction imprime dans le fichier filename
le contenu d‚fini par le format txt et les paramŠtres a, b, c,  d et e.

&EN char *filename : nom du fichier r‚sultat
&EN char *txt : format d'impression (fprintf)
&EN char *a, *b, *c, *d, *e : paramŠtres optionnels de txt

La variable
&CO
    int DebugActif
&TX
permet d'annuler temporairement l'output dans un fichier. Il suffit de
fixer sa valeur … 0.

&EX
    DebugFile("dump.txt", "Valeur du double : %lf\n", &dbl);
&TX

&SA Debug(), DebugB(), DebugET(), DebugHexa()
========================================================================*/

int DebugFile(filename, txt, a, b, c, d, e)
char    *filename, *txt, *a, *b, *c, *d, *e;
{
    char *ofile = DebugFilename;
    int     rc, t_actif = DebugActif;

    DebugFilename = filename;
    DebugActif = 1;
    rc = Debug(txt, a, b, c, d, e);
    DebugActif = t_actif;
    DebugFilename = ofile;
    return(rc);
}

/* =====================================================================
Cette fonction imprime en hexad‚cimal dans le fichier filename
le contenu du string str de longueur lg.

&EN char *filename : nom du fichier r‚sultat
&EN char *title : texte … placer avant la valeur hexa
&EN unsigned char *str : string … imprimer (ou pointeur vers une variable)
&EN int lg : longueur de str

La variable
&CO
    int DebugActif
&TX
permet d'annuler temporairement l'output dans un fichier. Il suffit de
fixer sa valeur … 0.

&EX
    DebugFileHexa("dump.txt", "Valeur du double : %lf\n", &dbl);
&TX

&SA Debug(), DebugB(), DebugET(), DebugHexa()
========================================================================*/

int DebugFileHexa(char *filename, char *title, unsigned char *str, int lg)
{
    char *ofile = DebugFilename;
    int     rc;

    DebugFilename = filename;
    rc = DebugHexa(title, str, lg);
    DebugFilename = ofile;
    return(rc);
}


/****************** DEBUGGING GNOME ********************/


Gdbg(text, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd)
char    *text, *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8, *c9, *ca, *cb, *cc, *cd;
{
    extern int GDEBUG;

#ifdef LINUX
    if(GDEBUG)
	fprintf(stderr, text, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd);
#else
    if(GDEBUG)
	Debug(stderr, text, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd);
#endif
}









