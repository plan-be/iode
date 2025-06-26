#include "scr.h"

/*int SCR_REC_KEYS[SCR_MAX_REC_KEYS + 1]; /* JMP 10-02-96 */
int *SCR_REC_KEYS = 0;
int SCR_NB_REC_KEYS = 0, SCR_NB_ALLOC_KEYS = 0;

/* ====================================================================
Enregistre une touche (caractère ou touche fonction dans le buffer du
clavier. Cette fonction permet donc d'automatiser certaines opérations
en prenant la main sur le clavier. Les touches sont stockées suivant le
principe LIFO (dernier arrivé, premier sorti).

&EN int key : touche à enregister
&EN int enter : si touche = '\n', valeur de remplacement (p.ex SCR_A_N)
&EN int tab : si touche = '\t', valeur de remplacement (p.ex SCR_TAB)

Les macros définissant les touches sont reprises dans le fichier
"scr4_key.h".

Un nombre maximum de SCR_MAX_REC_KEYS (200) touches peuvent être
enregistrées.

&EX
    SCR_record_key(SCR_ENTER);
&TX
&SA SCR_record_text(), SCR_record_key()
======================================================================= */

SCR_record_key_repl(key, enter, tab)
int     key;
{
    int     i;

    if(SCR_NB_REC_KEYS >= SCR_NB_ALLOC_KEYS) {
	SCR_REC_KEYS = (int *)SCR_realloc(SCR_REC_KEYS, sizeof(int), /* JMP 24-09-98 */
			SCR_NB_ALLOC_KEYS, 100 + SCR_NB_ALLOC_KEYS); /* JMP 24-09-98 */
	if(SCR_REC_KEYS == 0) {
	    SCR_NB_REC_KEYS = SCR_NB_ALLOC_KEYS = 0;
	    return(0);  /* BP_MODIF 08-10-98 */
	    }
	SCR_NB_ALLOC_KEYS += 100; /* JMP 24-09-98 */
	}

    if(key == '\n') key = enter;
    else if(key == '\t') key = tab;
    SCR_REC_KEYS[SCR_NB_REC_KEYS] = key;
    SCR_NB_REC_KEYS++;
    return(0);
}

/* ====================================================================
Enregistre une touche (caractère ou touche fonction dans le buffer du
clavier. Cette fonction permet donc d'automatiser certaines opérations
en prenant la main sur le clavier. Les touches sont stockées suivant le
principe LIFO (dernier arrivé, premier sorti).

Les macros définissant les touches sont reprises dans le fichier
"scr4_key.h".

Un nombre maximum de SCR_MAX_REC_KEYS (200) touches peuvent être
enregistrées.

&EX
    SCR_record_key(SCR_ENTER);
&TX
&SA SCR_record_text(), SCR_record_key_repl()
======================================================================= */

SCR_record_key(key)
int     key;
{
    SCR_record_key_repl(key, '\n', '\t');
    return(0);
}

/* ====================================================================
Enregistre un string dans le buffer de
clavier. Cette fonction permet donc d'automatiser certaines opérations
en prenant la main sur le clavier. Les touches sont stockées suivant le
principe LIFO (dernier arrivé, premier sorti).

&EN unsigned char *text: texte à enregister
&EN int enter : valeur de remplacement de '\n' dans text (p.ex SCR_A_N)
&EN int tab : valeur de remplacement  '\t' dans text (p.ex SCR_TAB)

&EX
    SCR_record_text("SCR4/AL1");
&TX
&SA SCR_record_key(), SCR_record_text()
======================================================================= */

SCR_record_text_repl(text, enter, tab)
unsigned char   *text;
int             enter, tab;
{
    int     i;

    for(i = strlen(text) - 1 ; i >= 0 ; i--)
	SCR_record_key_repl(text[i], enter, tab);

    return(0);
}

/* ====================================================================
Enregistre un string dans le buffer de
clavier. Cette fonction permet donc d'automatiser certaines opérations
en prenant la main sur le clavier. Les touches sont stockées suivant le
principe LIFO (dernier arrivé, premier sorti).

Un nombre maximum de SCR_MAX_REC_KEYS (200) caractères peuvent être
enregistrées.

&EX
    SCR_record_text("SCR4/AL1");
&TX
&SA SCR_record_key()
======================================================================= */

SCR_record_text(text)
unsigned char   *text;
{
    int     i;

    for(i = strlen(text) - 1 ; i >= 0 ; i--)
	SCR_record_key(text[i]);

    return(0);
}

