#include "scr.h"

int PR_FILE_BUF, PR_FILE_POS;
extern U_ch *PR_REPL_FROM , *PR_REPL_TO;

/*NH*/
#ifdef SCRCPP
/*NH*/
FILE *PR_file_open(
char    *filename
)
#else
FILE *PR_file_open(filename)
char    *filename;
#endif
{
    FILE            *fd = 0;
    unsigned char   fld_name[65];

    PR_FILE_BUF = PR_FILE_POS = 0;
    if(filename == 0) return(fd);
    strcpy(fld_name, filename);
    SCR_strip(fld_name);
    return(fopen(fld_name, "r"));
}


int     PR_FILE_EMPTY;


/*NH*/
PR_is_eq(fd, ch)
FILE    *fd;
int     ch;
{
    int     ch1;

    ch1 = PR_file_getc(fd);
    if(ch == ch1) return(1);
    PR_file_ungetc(ch1);
    return(0);
}

/*NH*/
PR_read_keyw(fd, keyw)
FILE    *fd;
char    *keyw;
{
    int     ch, i = 0, flag = 0;

    ch = PR_file_getc(fd);
    if(ch == '.') flag = 1;
    else          PR_file_ungetc(ch);

    while(1) {
	ch = PR_file_getc(fd);
	if(SCR_is_anum(ch) || ch == '_') {
	    if(i < 50)  keyw[i++] = ch;
	    }
	else {
	    keyw[i] = 0;
	    PR_file_ungetc(ch);
	    return(flag);
	    }
	}
}

/*NH*/
PR_file_ungetc(ch)
int     ch;
{
    PR_FILE_BUF = ch;
    return(0);
}

/*NH*/
PR_file_getc(fd)
FILE    *fd;
{
    int     ch;

    if(PR_FILE_BUF) {
	ch = PR_FILE_BUF;
	PR_FILE_BUF = 0;
	return(ch);
	}
    ch = getc(fd);
    if(ch == '\n' || ch == '\f') PR_FILE_POS = 0;
    else PR_FILE_POS++;
    return(ch);
}

/*NH*/
PR_file_eol(fd)
FILE    *fd;
{
    int     ch;

    while(1) {
	ch = PR_file_getc(fd);
	if(ch == EOF || ch == '\n') return(0);
	}
}

long    PR_NB, PR_NR;
/*NH*/
PR_print_var(fld_name)
char    *fld_name;
{
    char    buf[30];

    SCR_upper(fld_name);

    if(strcmp(fld_name, "DATE") == 0)
	PR_string(SCR_long_to_fdate(SCR_current_date(), buf, "dd-mm-yy"));
    else if(strcmp(fld_name, "LDATE") == 0)
	PR_string(SCR_long_to_fdate(SCR_current_date(), buf, "dd-mm-yyyy"));
    else if(strcmp(fld_name, "TIME") == 0)
	PR_string(SCR_long_to_ftime(SCR_current_time(), buf, "hh:mm"));
    else if(strcmp(fld_name, "PG") == 0)
	PR_string("%d", PR_CURRENT->pr_pgnb);
    else if(strcmp(fld_name, "NB") == 0)
	PR_string("%ld", PR_NB);
    else if(strcmp(fld_name, "NR") == 0)
	PR_string("%ld", PR_NR);
    else if(strcmp(fld_name, "SUITE") == 0) {
	if(PR_NB > 0) PR_string("(suite)");
	else return(0);
	}
    else if(strcmp(fld_name, "VERVOLG") == 0) {
	if(PR_NB > 0) PR_string("(vervolg)");
	else return(0);
	}
    else if(strcmp(fld_name, "CNT") == 0) {
	if(PR_NB > 0) PR_string("(cnt)");
	else return(0);
	}
    else PR_string(fld_name);

    return(0);
}

/* ====================================================================
Fonction semblable à PR_file() mais généralisée de telle sorte que
l'utilisateur ait le choix de la fonction à exécuter pour obtenir
le texte à imprimer pour chaque champ rencontré dans le fichier filename.
Dans le cas de PR_file(), ce texte était celui du champ de la page
de référence.

Le contenu d'un fichier est imprimé en remplaçant les champs indiqués
dans le fichier par des $nom ou ##nom par le texte retourné par la
fonction utilisateur, fonction qui reçoit le nom du champ comme
paramètre.


&EN filename est un fichier Ascii contenant le texte à imprimer et
    des séquences spéciales décrites plus bas.

&EN fn est un pointeur vers fonction. Cette fonction reçoit comme
    argument le nom du champ rencontré dans le fichier et retourne
    le texte à imprimer à la place du champ. Sa syntaxe est la suivante :
&CO
    unsigned char *fn(unsigned char *fld_name);

    Par exemple :

    unsigned char *PR_file_txt(fld_name)
    unsigned char    *fld_name;
    {
	int          fld_nb;
	static U_ch  tmp[30];

	fld_nb = SearchFieldNumber(fld_name);
	if(fld_nb << 0) return((U_ch  *)0);
	sprintf(tmp, "Champ %d", fld_nb);
	return(tmp);
    }
&TX

&EN col qui indique en quelle colonne le texte défini par le fichier
    doit être imprimé (valable pour les étiquettes)

&EN nl qui indique le nombre de lignes maximum à imprimer (si ce nombre
    vaut -1, il n'y a pas de limite). A nouveau, ceci est surtout utile
    pour des étiquettes

&EN first_ch permet de sélectionner les lignes à imprimer dans le fichier
    filename. Seules les lignes qui commencent par ce caractère sont
    imprimées (majuscules ou minuscules). Si ce caractère est nul, toutes
    les lignes sont sélectionnées.

Dans le cas où first_ch est spécifié, les lignes dont le premier
caractère est identique à first_ch et est majuscule sont de toute
façon imprimées. Celle dont le premier caractère est identique, mais
en minuscule, ne sont imprimées que dans 2 cas : soit si aucun champ
n'est présent dans cette ligne, soit si au moins un champ de la ligne
est non vide.

Lorsque un des caractères verticaux semi-graphiques (simple ou double) ou
le caractère de hachurage de mt, ou le caractère pipe (|) apparaissent
dans une ligne, le pointeur d'impression est remplacé à cet endroit
quelque soit la longueur des champs précédents.

Le caractère ampersand a le même effet, mais n'est pas imprimé sauf s'il
est doublé.

&RT Si le fichier ne peut être ouvert, PR_mail_col retourne -1. Sinon
    la fonction retourne 0

&EX
    PR_select(pr_deskjet);
    ...
	SCR_isam_to_page(pg_client, is_client);
	PR_mail_col("etiq.txt", myfn, 40, 9, 0);
    ...
    PR_send();
&TX

&SA SCR_isam_to_page(), PR_select(), PR_send(), PR_file(), PR_listing(),
    PR_mail()
======================================================================= */

#ifdef SCRPROTO
PR_mail_col(
unsigned char   *filename,
unsigned char   *(*fn)(unsigned char *),
int             col,
int             nl,
int             first_ch
)
#else
PR_mail_col(filename, fn, col, nl, first_ch)
unsigned char    *filename;
unsigned char    *(*fn)();
int     col, nl, first_ch;
#endif
{
    return(PR_mail_col_repl(filename, fn, col, nl, first_ch, 0, 0));
}

#ifdef SCRPROTO
PR_mail_col_repl(
unsigned char   *filename,
unsigned char   *(*fn)(unsigned char *),
int             col,
int             nl,
int             first_ch,
unsigned char   *repl,
unsigned char   *by
)
#else
PR_mail_col_repl(filename, fn, col, nl, first_ch, repl, by)
unsigned char    *filename;
unsigned char    *(*fn)();
int     col, nl, first_ch;
unsigned char   *repl, *by;
#endif

{
#ifdef SCRPROTO
    FILE *PR_file_open(char *);
#else
    FILE *PR_file_open();
#endif
    FILE            *fd;
    unsigned char   fld_name[65], *val;
    int             ch, fld_nb, nb_lines = 0, first = 1, i;


    if(PR_CURRENT == 0) return(-1);
    fd = PR_file_open(filename);
    if(fd == 0) return(-2);

    PR_REPL_FROM = repl;  /* JMP 26-03-02 */
    PR_REPL_TO   = by;    /* JMP 26-03-02 */
    PR_CURRENT->pr_ccol = col; /* JMP 27-12-91 */

    while(1) {
	ch = PR_file_getc(fd);
	if(ch == EOF) break;
	if(first_ch && first) {
	    if(SCR_upper_char(ch) != first_ch) {
		if(ch != '\n') PR_file_eol(fd); /* JMP 12-10-2004 */
		continue;
		}
	    else {
		PR_FILE_POS = 0;
		PR_FILE_EMPTY = SCR_is_lower(ch);
		ch = PR_file_getc(fd);
		}
	    }
	first = 0;
	switch(ch) {
	    case '&' :
		if(PR_is_eq(fd, ch)) {
		    PR_char(ch);
		    break;
		    }

		PR_goto(-1, col + PR_FILE_POS - 1);
		PR_clear_line(PR_CURRENT->pr_cline, PR_CURRENT->pr_ccol);
		break;

	    case 179 : /* '│' */
	    case 186 : /* '║' */
	    case 177 : /* '▒' */
	    case 124 : /* '|' */
		PR_goto(-1, col + PR_FILE_POS - 1);
		PR_clear_line(PR_CURRENT->pr_cline, PR_CURRENT->pr_ccol);
		PR_char(ch);
		break;

	    case '\\' :
		if(PR_is_eq(fd, ch)) {
		    PR_char(ch);
		    break;
		    }
		ch = PR_file_getc(fd);
		switch(SCR_upper_char(ch)) {
		    case 'B' : PR_bold(); break;
		    case 'C' : PR_condensed(); break;
		    case 'E' : PR_enlarged(); break;
		    case 'U' : PR_underline(); break;
		    case 'N' : PR_normal(); break;
		    case 'F' : PR_formfeed(); break;
		    }
		break;

	    case '$' :
	    case '#' :
		if(PR_is_eq(fd, ch)) {
		    PR_char(ch);
		    break;
		    }
		if(PR_read_keyw(fd, fld_name)) {
		    PR_print_var(fld_name);
		    break;
		    }
		if(fn == 0) {
		    PR_string("%c%s", ch, fld_name);
		    break;
		    }
		val = (*fn)(fld_name);
		if(val == 0) PR_string("%s", fld_name);
		else {
		    if(!PR_is_empty(val)) PR_FILE_EMPTY = 0;
		    if(ch == '#') SCR_strip(val);
		    PR_string("%s", val);
		    }
		break;

	    default  :
		if(ch == '\n') {
		    if(PR_FILE_EMPTY == 0) {
			nb_lines++;
			if(nb_lines >= nl && nl > 0) goto fin;
			PR_linefeed();
			}
		    else
			PR_clear_line(PR_CURRENT->pr_cline, col);

		    PR_goto(-1, col);
		    first = 1;
		    }
		else PR_char(ch);
		break;
	    }
	}
fin:
    fclose(fd);
    PR_REPL_FROM = 0;     /* JMP 26-03-02 */
    PR_REPL_TO   = 0;     /* JMP 26-03-02 */
    return(0);
}


/* ====================================================================
Cette fonction est un cas particulier de PR_mail_col() : l'impression
commence toujours en col 0, il n'y a pas de caractère de début de ligne
pour limiter l'impression, et le nombre de lignes n'est pas limité.

La fonction
&CO
	PR_mail(filename, fnptr)
&TX
vaut simplement
&CO
	PR_mail_col(filename, fnptr, 0, -1, 0)
&TX

&RT Si le fichier ne peut être ouvert, PR_mail retourne -1. Sinon
    la fonction retourne 0

&EX
    PR_select(pr_deskjet);
    ...
	PR_mail("etiq.txt", myfn);
    ...
    PR_send();
&TX

&SA SCR_isam_to_page(), PR_select(), PR_send(), PR_file(), PR_listing(),
    PR_mail_col()
======================================================================= */

#ifdef SCRPROTO
PR_mail(
unsigned char *filename,
unsigned char *(*fn)(unsigned char *)
)
#else
PR_mail(filename, fn)
unsigned char    *filename, *(*fn)();
#endif
{
    return(PR_mail_col(filename, fn, 0, -1, 0));
}

/*NH*/
PR_is_empty(ptr)
U_ch    *ptr;
{
    int     i;

    for(i = 0 ; ptr[i] ; i++)
	if(ptr[i] != ' ' && ptr[i] != '0') return(0);

    return(1);
}


