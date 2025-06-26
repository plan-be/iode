#include <string.h>
#include <s_strs.h>
#include "s_args.h"

#define A_MAX_ARG   128

char    *A_SEPS = " ,\n\r\f\t";
char    **A_VAL;
int     A_NB_VAL, A_NB_OPEN, A_D = 0;


/* =======================================================================
    Initialise une liste d'arguments : le string présenté est décomposé
    en une suite de "mots" et placé dans le tableau A_VAL alloué.

    Les séparateurs entre les mots sont par défaut les blancs, les tabs,
    les virgules et les returns (\n). La variable char *A_SEPS est un
    pointeur vers la liste des séparateurs et peut être modifiée par
    l'utilisateur.

    Lorsque @filename est rencontré, le fichier filename est utilisé
    pour poursuivre la liste des arguments. A la fin du fichier, la
    lecture de la suite du string continue.

    Lorsque $nom est rencontré, la fonction A_expand() à définir par
    l'utilisateur est appelé et son output est utilisé pour la suite des
    arguments. La lecture du string reprend ensuite. La fonction
    utilisateur A_iexpand() est appelée en début de processus pour
    permettre l'initialisation du processus de traduction des macros. Si
    ces deux fonctions ne sont pas définies par l'utilisateur, le
    programme utilise deux fonctions par défaut.

    La fonction alloue un tableau de pointeur A_VAL de type char **. Ce
    tableau est terminé par un pointeur NULL. Le nombre d'arguments est
    indiqué par la variable A_NB_VAL.

    Le tableau A_VAL est retourné par la fonction, permettant à
    l'utilisateur d'effectuer plusieurs fois A_init dans le même
    programme avec des arguments différents. Au prochain appel, A_VAL
    est réinitialisé pour la nouvelle interprétation, mais pas détruit
    (pas de free).

&EX
    Appel :

	A_init("-iv file1, *.c file2");

    Résultat :

	A_VAL === {
	    "-iv",
	    "file1",
	    "ex.c",
	    "ex2.c",
	    "file3",
	    NULL
	    }

	A_NB_VAL === 5
&TX
&SA A_initv()
==================================================================== */

char **A_init(string)
char    *string;
{
    char    *argv[12];

    argv[0] = string;
    argv[1] = 0L;
    return(A_initv(argv));
}

/* ====================================================================
    Initialise une liste d'arguments. L'objet et le fonctionnement
    de la fonction est semblable à A_init(), mais au lieu de recevoir
    un seul string, cette fonction reçoit un tableau de strings du
    type des arguments de la fonction main(). Ce tableau doit être
    terminé par un pointeur NULL.

&EX
	#include <s_args.h>

	main(argc, argv)
	int     argc;
	char    *argv[];
	{
	    int     i;

	    A_initv(argv);
	    for(i = 0 ; i < A_NB_VAL ; i++)
		printf("%s\n", A_VAL[i]);
	    A_free(A_VAL);
	}
&TX
&SA A_init(), A_free()
================================================================== */

char **A_initv(argv)
char    **argv;
{
    int     i;
    char    **argv2;

    A_initv1(argv);
    if(A_D == 0) return(A_VAL);

    argv2 = A_VAL;
    A_iexpand(argv2);
    A_initv1(argv2);
    if(A_D == 0) {
	A_free((U_ch **)argv2);
	return(A_VAL);
    }

    for(i = 0; i < A_NB_VAL; i++)
	if(A_VAL[i][0] == '$')
	    A_cerror("A_init : %s : cannot be expanded", A_VAL[i]);

    A_free((U_ch **)argv2);
    A_free((U_ch **)A_VAL);
    return(NULL);
}

/*NH*/
/* ============================================================
    Passes the arguments and performs the expand of $ and @
   ============================================================ */
int A_initv1(argv)
char    **argv;
{
    int     i;

    A_NB_VAL = 0;
    A_D = 0;
    A_VAL = 0;
    A_NB_OPEN = 0;

    for(i = 0; argv[i] != 0 ; i++) A_add(argv[i]);
    A_add(0L);
    return(0);
}


/*NH*/
/* ===============================================================
    Add an argument in the table A_VAL and increment A_NB_VAL.
	arg : IN : ptr to the argument
   =============================================================== */

int A_add(arg)
char    *arg;
{
    char    *tmp, *exp, word[A_MAX_ARG + 1];
    FILE    *fd;

    if(arg == 0) {
	A_add1(arg);
	return(0);
    }

    switch(arg[0]) {
	case '@':
	    if(A_NB_OPEN >= 10) {
		A_cerror("A_init : Maximum 10 levels of nesting");
		return(-1);
	    }
	    tmp = arg;
	    A_read(0, &tmp, word, A_MAX_ARG);
	    fd = fopen(word + 1, "r");
	    if(fd == 0) {
		A_cerror("A_init : Cannot open '%s'", word + 1);
		return(-1);
	    }
	    A_NB_OPEN++;

	    while(A_read(1, (char **)fd, word, A_MAX_ARG) == 0) if(word[0] != 0) A_add(word);
	    fclose(fd);
	    A_NB_OPEN--;
	    break;

	case '$':
	    if(A_NB_OPEN >= 10) {
		A_cerror("A_init : Maximum 10 levels of nesting");
		return(-1);
	    }
	    tmp = arg;
	    A_read(0, &tmp, word, A_MAX_ARG);
	    exp = A_expand(word + 1);
	    if(exp == 0) {
		A_D = -1;
		A_add1(word);
		return(0);
	    }
	    A_NB_OPEN++;

	    while(A_read(0, &exp, word, A_MAX_ARG) == 0)
		if(word[0] != 0) A_add(word);
	    A_NB_OPEN--;
	    break;

	default:
#ifdef DOS
	    if(!A_to_be_exp(arg)) {
		tmp = arg;
		break;
		}
	    else {
		if(A_expandstar(arg) == 0) tmp = "";
		else                       tmp = arg;
		break;
		}
#else
	    tmp = arg;
	    break;
#endif
	}

    while(A_read(0, &tmp, word, A_MAX_ARG) == 0) {
	if(word[0] != 0) {
	    if(word[0] == '@' || word[0] == '$') A_add(word);
	    else A_add1(word);
	}
    }
    return(0);
}

#ifdef DOS
/* ====================================================================
Cette fonction vérifie simplement qu'un string (normalement un nom de
fichier) contient des "wild cards", c'est-à-dire des caractères
permettant de reprendre plusieurs fichiers sous un même nom (*.c par
exemple).

&RT 1 si le string contient * ou ? et 0 sinon.
&EX
    A_to_be_exp("*.c")      vaut 1
    A_to_be_exp("args.c")   vaut 0
&TX
=======================================================================*/

A_to_be_exp(str)
char    *str;
{
    int     i = 0;

    while(str[i] != 0) {
	if(str[i] == '*' || str[i] == '?') return(1);
	i++;
	}
    return(0);
}
#endif

/*NH*/
/* =============================================================
    Add an argument in the table A_VAL and increment A_NB_VAL.
   ============================================================= */

int A_add1(arg)
char    *arg;
{

    SCR_add_ptr((U_ch ***)&A_VAL, &A_NB_VAL, arg);
    if(arg == 0) A_NB_VAL--;
    return(0);
}

/*NH*/
/* ========================
    Read next word in file.
=========================== */

A_read(type, buf, word, max_lg)
int     type;
char    **buf;
char    *word;
int     max_lg;
{
    int     lg = 0, c, q = 0;

    while(1) {
	/* Gets next char */
	if(type == 0) {
	    c = **buf;
	    if(c == 0) c = EOF;
	    else (*buf)++;
	    }
	else c = getc((FILE *)buf);

	/* Checks EOF or EOString */
	if(c == EOF) {
	    word[lg] = 0;
	    return((lg == 0) ? -1 : 0);
	    }

	if(q == 0 && A_is_sep(c) >= 0) {
	    if(lg > 0) {
		word[lg] = 0;
		return(0);
		}
	    }

	else if(lg < max_lg) {
	    if(c == '"') {
		if(lg == 0) q = 1;
		else if(q == 1) {
		    word[lg] = 0;
		    return(0);
		    }
		}

	    else word[lg++] = c;
	    }
	}
}

/*NH*/

A_is_sep(ch)
int     ch;
{
    int     i;

    for(i = 0 ; A_SEPS[i] ; i++) if(A_SEPS[i] == ch) return(i);
    return(-1);
}

/* =====================================================================
    Libère l'espace alloué pour le stockage des arguments par A_init()
    ou A_initv().

&EX
    char    **args;

    args = A_initv(argv);
    ...
    A_free(args);
&TX
======================================================================= */

A_free(args)
U_ch    **args;
{
    SCR_free_tbl(args);
    return(0);
}

/* ======================================================================
    Recherche un string dans une table d'arguments généré par A_init()
    ou A_initv(). La position du string dans cette table est retournée si
    le string est présent. La fonction retourne -1 sinon.

&EX
    args = A_initv(argv);
    if(A_find(args, "-h") >= 0) {
	printf("Usage : ...\n");
	exit(0);
	}
&TX
======================================================================== */

A_find(args, string)
char    **args;
char    *string;
{
    int     i;

    if(string == NULL) return(-1);

    for(i = 0; args[i] != 0 ; i++)
	if(strcmp(args[i], string) == 0) return(i);

    return(-1);
}

/*======================================================================
    Recherche dans la liste des arguments générée par A_init() ou
    A_initv() le nombre de paramètres à partir d'une position donnée
    (calculée par exemple à l'aide de A_find()),
    c'est-à-dire le nombre de strings ne commençant pas par '-'.

    Si la position est hors des la liste des arguments, retourne -1.

&EX
	#include <s_args.h>

	main(argc, argv)
	int     argc;
	char    *argv[];
	{
	    int     i, nb, pos;
	    char    **args;

	    args = A_initv(argv);
	    pos = A_find(args, "-f");
	    nb  = A_nb(args, pos);
	    if(nb >= 0) {
		printf("Arguments de -f\n");
		for(i = 0 ; i < nb ; i++)
		    printf("%s\n", A_VAL[pos + i + 1]);
		}
	    else
		printf("Pas d'arguments de -f\n");
	}
&TX
&SA A_init(), A_find()
====================================================================== */

A_nb(args, pos)
char    **args;
int     pos;
{
    int     i;

    if(pos == -1 || args[pos] == 0) return(-1);

    for(i = pos + 1; args[i] != 0 ; i++)
	if(args[i][0] == '-' && args[i][1] != 0) break;

    return(i - pos - 1);
}

/* =======================================================================
    Recherche l'argument -h dans la liste créée à l'aide de A_init() ou
    A_initv() et exécute la fonction passée comme argument si -h est
    présent.

    Si -h est présent, retourne -1, sinon retourne 0.

&EX
	#include <s_args.h>

	Usage()
	{
	    printf("Usage : ...\n");
	}

	main(argc, argv)
	int     argc;
	char    *argv[];
	{
	    char    **args;

	    args = A_initv(argv);
	    if(A_help(args, Usage)) exit(0);
	    ...
	}
&TX
&SA A_initv(), A_find()
======================================================================== */

#ifdef SCRPROTO
A_help(
char    **args,
int     (*fn)(void)
)
#else
A_help(args, fn)
char    **args;
int     (*fn)();
#endif
{
    if(A_find(args, "-h") < 0) return(0);
    (*fn)();
    return(-1);
}


