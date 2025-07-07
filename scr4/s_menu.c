#include "scr.h"
/*
extern int MN_add_set_option_text(MENU *, char *, ... );
extern int MN_set_option_text(MENU *,int , char *, ... );
*/
int SCR_mn_color = -1;

/*NH*/
/* =========MN_INIT ====================================
     Initialize a MENU.
     mn : IO : pointer to the menu
     return : 0 or SCR_NO_MENU
   ============================================================== */

MN_init(mn)
MENU    *mn;
{
    mn->mn_box      = 1;
    mn->mn_shadow   = 1;    /* BP_M 25-10-95 */
    mn->mn_opt_mem  = 1;
    mn->mn_back     = (SCR_mn_color < 0) ? SCR_DEFAULT : SCR_mn_color; /* JMP41 22-11-92 */
    mn->mn_cmt_pos  = -1;
    mn->mn_opt_mem  = 1; /* JMP 24-01-99 */
    return(0);
}
/*NH*/
/* =+=======MN_INIT_OPTION ====================================
     Initialize an option
     mn : IO : pointer to the option
     return : 0 or SCR_NO_OPTION
   ============================================================== */

MN_init_option(opt)
OPTION    *opt;
{
    memset(opt, 0, sizeof(OPTION)); /*4.04*/
    opt->opt_enable = 1;
    opt->opt_cmt    = -1;
    return(0);
}

/* =========MN_CREATE ============================================
    Crée un nouveau menu en allouant l'espace nécessaire et en
    initialisant les valeurs par défaut : BOX 1, BACK DEFAULT.

    Retourne un pointeur vers la structure allouée.
&EX
    mn = MN_create();
&TX
&SA MN_auto_create(), MN_free()
================================================================= */

MENU *MN_create()
{
    MENU    *mn;

    mn = (MENU *)SCR_malloc(sizeof(MENU));
    MN_init(mn);
    return(mn);
}
/*NH*/
/* =+====== MN_FREE_OPTION ========================================
     Frees the allocated space for an option.

     opt : IO : pointer to the struct OPTION
   ============================================================== */

MN_free_option(opt)
OPTION      *opt;
{
    if(opt == 0) return(0);

    SCR_free(opt->opt_text);
    opt->opt_text = 0;
    return(0);
}

/* =+====== MN_FREE =============================================
    Libère l'espace alloué pour un menu créé à l'aide d'une des
    fonctions de création MN_create(), MN_auto_create(), ...

    Les options, titre et footnote du MENU sont également libérés.

&NO Cette fonction ne peut jamais être utilisée sur un MENU défini comme
objet SCR dans un fichier source.

&SA MN_create()
================================================================= */

MN_free(mn)
MENU    *mn;
{
    int     i;

    /* free the OPTIONs */
    if(mn == 0) return(0);

    if(mn->mn_opts != 0)
	for(i = 0; i < mn->mn_nb_opts ; i++)
	    MN_free_option(mn->mn_opts + i);

    /* free the OPTION definitions */
    SCR_free(mn->mn_opts);

    /* free the title */
    if(SCR_MN_FREE_TITLE)       /* BP_M 22-02-2000 */
	SCR_free(mn->mn_title);

    /* free the footnote */
    if(SCR_MN_FREE_FOOT)        /* BP_M 22-02-2000 */
	SCR_free(mn->mn_foot);

    /* free the MENU itself */
    SCR_free(mn);

    return(0);
}


/* =+====== MN_ADD_OPTION ============================================
    Ajoute une option à un MENU créé à l'aide d'une des fonctions
    d'allocation de MENU MN_create(), MN_auto_create(), etc.

    La valeur retournée est un pointeur vers la nouvelle option.

&NO Cette fonction peut être utilisée en cours d'édition du MENU, mais
jamais sur un MENU défini comme objet SCR dans un fichier source.


&SA MN_auto_create(), MN_set_option_text()
====================================================================== */

#ifdef __cplusplus
OPTION *MN_add_option(
MENU    *mn
)
#else
OPTION *MN_add_option(mn)
MENU    *mn;
#endif
{
    OPTION   *opt;

    if(mn->mn_nb_opts % 10 ==  0) {   /*4.04*/
	if(mn->mn_nb_opts == 0)
	    opt = malloc(sizeof(OPTION) * 10);
	else
	    opt = realloc(mn->mn_opts, sizeof(OPTION) * (mn->mn_nb_opts + 10));
	if(opt == 0) return((OPTION *)0);
	mn->mn_opts = opt;
	}
/*    mn->mn_opts = (OPTION *) SCR_realloc (mn->mn_opts,
					  sizeof(OPTION),
					  SCR_alloc_chunck(mn->mn_nb_opts - 1),
					  SCR_alloc_chunck(mn->mn_nb_opts));
*/
    opt = mn->mn_opts + mn->mn_nb_opts;
    MN_init_option(opt);
    mn->mn_nb_opts += 1;
    return(opt);
}
/* =+=======MN_END ==================================================
Termine la définition d'un MENU créé à l'aide d'une des fonctions
d'allocation de MENU MN_create(), MN_auto_create(), etc.

Le nombre de colonnes est ajustée sur les textes des OPTIONS, de la
footnote et du titre. Le nombre de lignes visibles est limité à la
taille de l'écran et au nombre d'options.

La lettre qui permettra de se positionner ou de déclencher l'action
d'une option est fixée comme étant la première non blanche de l'option
si cette lettre n'a pas encore été fixée.

&EX
    EditProducts(prod, prices)
    char    **prod;
    long    *prices;
    {
	MENU    *mn;
	int     i, choice;

	mn = MN_create();
	mn->mn_line   = 5;
	mn->mn_column = 15;
	for(i = 0 ; prod[i] != 0 ; i++)
	    MN_add_set_option_text(mn, "%s : %ld FB",
					prod[i], prices[i]);

	MN_end(mn);
	choice = MN_edit(mn);
	MN_free(mn);
	return(choice);
    }
&TX
===================================================================== */

MN_end(mn)
MENU    *mn;
{
    int     nc = 0,
	    i, j, sel = 0;
    OPTION  *opt;

    for(i = 0 ; i < mn->mn_nb_opts ; i++) {
	opt = mn->mn_opts + i;
	if(opt->opt_sel > 0) sel = 1;
	if(opt->opt_letter == 0)
	    for(j = 0 ; opt->opt_text[j] != 0 ; j++)
		if(opt->opt_text[j] !=  ' ') {
		    opt->opt_letter = opt->opt_text[j];
		    break;
		}
	nc = max(nc, strlen(opt->opt_text));
	}


    if(mn->mn_title != 0) nc = max(strlen(mn->mn_title), nc);
    if(mn->mn_foot  != 0) nc = max(strlen(mn->mn_foot),  nc);

    mn->mn_nb_column = nc + sel;

    if(mn->mn_nb_line == 0)
	mn->mn_nb_line = min(mn->mn_nb_opts, SCR_PAGE_SIZE[0] - 2);

    return(0);
}
/*NH*/
/* =+=======MN_OPTION_END ============================================
     Fix the options for a OPTION.
	mn     : IO : pointer to the MENU
	opt_nb : IN : number of the OPTION in the MENU
	return   : error code or 0
   ============================================================== */

MN_option_end()
{
    return(0);
}

/* ====================================================================
Crée un MENU dynamique et fixe directement la ligne, la colonne, la
couleur et le nombre de lignes visibles.

&EX
    EditProducts(prod, prices)
    char    **prod;
    long    *prices;
    {
	MENU    *mn;
	int     i, choice;

	mn = MN_create_default(5, 15, SCR_REVERSE, 5);
	for(i = 0 ; prod[i] != 0 ; i++)
	    MN_add_set_option_text(mn, "%s : %ld FB",
					prod[i], prices[i]);

	MN_end(mn);
	choice = MN_edit(mn);
	MN_free(mn);
	return(choice);
    }
&TX
&SA MN_create(), MN_free(), MN_auto_create(), MN_add_set_option_text()
======================================================================= */

#ifdef __cplusplus
MENU *MN_create_default(
int     line,
int     col,
int     back,
int     nb_line
)
#else
MENU *MN_create_default(line, col, back, nb_line)
int     line, col, back, nb_line;
#endif
{
    MENU    *mn;

    mn = MN_create();
    if(mn == 0) return((MENU *)0);
    mn->mn_line   = line;
    mn->mn_column = col;
    mn->mn_back   = back;
    mn->mn_nb_line= nb_line;

    return(mn);
}

/* ====================================================================
Recherche le numéro de l'option correspondant au texte txt dans le MENU
mn. Le texte de chaque option est localement strippé (les blancs de fin
sont supprimés) et justifié à gauche. De même en ce qui concerne le
texte passé comme argument à la fonction.

La fonction retourne -1 si le texte n'est pas trouvé dans le MENU. Elle
retourne le numéro de l'option sinon (première option = 0).

&EX
    MN_search_option(mn, " Impression sur LPT1:");
&TX
======================================================================= */

MN_search_option(mn, txt)
MENU    *mn;
unsigned char    *txt;
{
    return(SCR_search_menu(mn, txt));
}

/*NH*/
SCR_search_menu(mn, txt)
MENU    *mn;
unsigned char    *txt;
{
    int     i;
    unsigned char    buf[SCR_MAX_FIELD_LENGTH + 1];
    unsigned char    buf2[SCR_MAX_FIELD_LENGTH + 1];

    strcpy(buf, txt);
    U_ljust_text(buf);
    SCR_strip(buf);
    for(i = 0 ; i < mn->mn_nb_opts ; i++) {
	strcpy(buf2, mn->mn_opts[i].opt_text);
	SCR_strip(buf2);
	U_ljust_text(buf2);
	if(strcmp(buf2, buf) == 0)
	    return(i);
    }
    return(-1);
}

/* ====================================================================
Crée un MENU dynamique et fixe directement la ligne, la colonne, la
couleur, le nombre de lignes visibles, l'épaisseur du cadre (BOX) et les
options du menu.

text est un tableau de pointeurs vers le texte des options. Le dernier
élément du tableau doit être un pointeur nul ou un pointeur vers un
string vide ("").

La fonction MN_end() est appelée en fin de fonction pour ajuster le MENU.

&EX
    EditProducts(prod)
    char    **prod;
    {
	MENU    *mn;
	int     choice;

	mn = MN_auto_create(prod, 5, 15, 10, 1, SCR_REVERSE);
	choice = MN_edit(mn);
	MN_free(mn);
	return(choice);
    }
&TX
&SA MN_create(), MN_free(), MN_end(), MN_add_set_option_text(),
MN_edit()
======================================================================= */

#ifdef __cplusplus
MENU *MN_auto_create(
char    **text,
int     line,
int     col,
int     nlv,
int     box,
int     back
)
#else
MENU *MN_auto_create(text, line, col, nlv, box, back)
char    **text;
int     line, col, nlv, box, back;
#endif
{
    MENU    *mn;
    int     i;

    mn = MN_create_default(line, col, back, nlv);
    if(mn == 0) return((MENU *)0);
    mn->mn_box      = box;

    for(i = 0 ; text[i] != 0 && text[i][0] != 0 ; i++)
	MN_add_set_option_text(mn, "%s", text[i], 0L, 0L, 0L, 0L, 0L);

    MN_end(mn);
    return(mn);
}

/*
MN_set_option_text(mn, opt_nb, txt, ...)
MENU    *mn;
int     opt_nb;
char    *txt;
{
    OPTION  *opt;
    //char    buf[256];
    char    buf[1024];
    va_list args;

    opt = mn->mn_opts + opt_nb;
    SCR_free(opt->opt_text);
    va_start(args, txt);
    vsprintf(buf, txt, args);
    va_end(args);
    opt->opt_text = (unsigned char *)SCR_stracpy(buf);
    return(0);
}

MN_add_set_option_text(mn, txt, ...)
MENU    *mn;
char    *txt;
{
    va_list args;

    va_start(args, txt);
    MN_add_option(mn);
    MN_set_option_text(mn, mn->mn_nb_opts - 1, txt, args);
    va_end(args);
    return(0);
}

*/

