#include "s_ydvlp.h"


/*NH*/
/*#ifndef GENPROTO */
int YY_strcmp(pyk1, pyk2)
const char *pyk1;
const char *pyk2;
{
    YYKEYS  *yk1 = (YYKEYS *)pyk1,
	    *yk2 = (YYKEYS *)pyk2;

    return(strcmp((char *)yk1->yk_word, (char *)yk2->yk_word));
}

/*NH=+======= YY_FIND_OPER =======================================
     Find an operator in a table
	table : IN : table of KEY_TABLE structures term by 0
	yy    : IN : current YYFILE with first char of op read
	return   : the pos of the operator found or -1
   ============================================================== */

YY_find_oper(yy, yykeys, nbkeys)
YYFILE      *yy;
YYKEYS      *yykeys;
int         nbkeys;
{
    int     i, ch, pos;
    YYKEYS  *yk, yk1;

    yk1.yk_word = yy->yy_text;
    yk = (YYKEYS *) bsearch(&yk1, yykeys, nbkeys, sizeof(YYKEYS), YY_strcmp);
    if(yk == 0) return(-1);
    pos = (int)(yk - yykeys);
    if(pos == nbkeys -1 || yk[1].yk_word[0] != yy->yy_text[0])
	return(yk->yk_def);
    YY_skip_spaces(yy);
    ch = YY_getc(yy);
    for(i = 1 ; pos + i < nbkeys && yk[i].yk_word[0] == yy->yy_text[0] ; i++)
	if(ch == yk[i].yk_word[1]) return(yk[i].yk_def);
    YY_ungetc(ch, yy);
    return(yk->yk_def);
}
/*NH*/
/* =+======= YY_FIND_STRING ============================================
     Find a string in a YYKEYS table
	table : IN : table of KEY_TABLE structures term by 0
	txt   : IN : string
	return   : the pos of the string found or -1
   ============================================================== */

YY_find_string(yy, yykeys, nbkeys)
YYFILE      *yy;
YYKEYS      *yykeys;
int         nbkeys;
{
    YYKEYS  *yk, yk1;

    yk1.yk_word = yy->yy_text;
    yk = (YYKEYS *)bsearch(&yk1, yykeys, nbkeys, sizeof(YYKEYS), YY_strcmp);
    if(yk != 0)
	return(yk->yk_def);
    return(-1);
}

/* ====================================================================
Fonction presque identique … YY_lex(). Cette fonction permet cependant
de remplacer la table de mots-cl‚s d‚finie … l'ouverture du texte
(YY_open()) par une table alternative, valable uniquement pour l'appel
courant.

&NO
    La table des mots-cl‚s doit ˆtre class‚e par l'utilisateur par ordre
    alphab‚tique.

&RT le type de l'‚l‚ment suivant du texte ou la valeur de l'entier
correspondant au mot-cl‚ lu dans la table des YYKEYS.
&SA YY_read(), YY_unread(), YY_lex()
=======================================================================*/

YY_lex2(yy, yykeys, nbkeys)
YYFILE      *yy;
YYKEYS      *yykeys;
int         nbkeys;
{
    int     type,
	    found;

again :
    type = YY_read(yy);
    switch(type) {
	case YY_COMMENT :
	    goto again;

	case YY_WORD :
	    if(YY_CASE_SENSITIVE == 0) YY_upper(yy->yy_text);
	    if(nbkeys == 0) return(YY_WORD);
	    found = YY_find_string(yy, yykeys, nbkeys);
	    return((found < 0) ? YY_WORD : found);

	case YY_SPECIAL :
	    if(nbkeys == 0) return(YY_SPECIAL);
	    found = YY_find_oper(yy, yykeys, nbkeys);
	    return((found < 0) ? YY_SPECIAL : found);

	default :
	    return(type);
	}
}

/* ====================================================================
Lit le prochain ‚l‚ment du texte. Le type est retourn‚ et la valeur
est sauv‚e dans la structure YYFILE. La liste suivante reprend les types
possibles et l'‚l‚ment de la structure YYFILE qui les contient.

Cette fonction appelle d'abord YY_read(). Trois types retourn‚s par
YY_read() sont trait‚s par YY_lex().

Si le type retourn‚ par YY_read() est YY_COMMENT, YY_lex() lit l'‚l‚ment
suivant sans retourner de valeur.

Si le type est YY_WORD, YY_lex() commence par mettre le mot en majuscule
si YY_CASE_SENSITIVE vaut 1. Ensuite, YY_lex() compare le mot lu avec
les mots d‚finis dans la table de YYKEYS pass‚es … l'ouverture du texte
(voir YY_open()). Si le mot est trouv‚, la valeur de l'entier
correspondant … ce mot est retourn‚. Sinon, la valeur YY_WORD est
retourn‚e.

Enfin, si le type est YY_SPECIAL, YY_lex() tente de retrouver le
caractŠre dans la table des YYKEYS et retourne le cas ‚ch‚ant la valeur
de l'entier correspondant. Les suites de caractŠres sp‚ciaux d‚finis
dans la table des YYKEYS sont trait‚es de la mˆme fa‡on : YY_lex() lit
tant qu'il trouve un d‚but de s‚quence correcte dans la table. Ainsi,
"??" ou "{" peuvent ˆtre des mots-cl‚s.

&NO
    La table des mots-cl‚s doit ˆtre class‚e par l'utilisateur par ordre
    alphab‚tique.

&RT le type de l'‚l‚ment suivant du texte ou la valeur de l'entier
correspondant au mot-cl‚ lu dans la table des YYKEYS.
&SA YY_read(), YY_unread(), YY_lex2()
=======================================================================*/

YY_lex(yy)
YYFILE      *yy;
{
    return(YY_lex2(yy, yy->yy_keys, yy->yy_nb_keys));
}
