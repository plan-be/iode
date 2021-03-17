#include "s_strs.h"
#include <stdio.h>

extern FILE     *PC_FD;

/* ====================================================================
Recherche dans le fichier filename toutes les lignes de d‚finition et
retourne un tableau de pointeurs vers les champs fld_nb de chaque
ligne de d‚finition.

Les paramŠtres fld_select et fld_vals permettent de s‚lectionner un
nombre d'entr‚es limit‚es : fld_select est le num‚ro du champ sur
lequel s'effectue la s‚lection et fld_vals est un string contenant les
valeurs que peut avoir la premiŠre lettre de ce champ. Si fld_val est
NULL ou vide, ou fld_select est < 0, toutes les lignes sont s‚lectionn‚es.

&RT un tableau contenant les champs num‚ro fld_nb de chaque ligne de
    d‚finition. Ce tableau est nul en cas d'erreur (fichier non trouv‚,
    pas de ligne de d‚finition trouv‚e)
&EX

    PrintFile(filename)
    char *filename;
    {
	char    **descr;

	descr = PC_list("printcap", 2, -1, NULL);
	if(descr == 0) return(-1);
	mn = MN_auto_create(descr, 5, 15, 10, 1, SCR_REVERSE);
	SCR_free_tbl(descr);

	prnb = MN_edit(mn) - 1;
	if(prnb < 0) return(-1);
	descr = PC_list("printcap", 0, -1, NULL);
	PR_exec("printcap", descr[prnb], filename, NULL, NULL);
	SCR_free_tbl(descr);
	return(0);
    }
&TX
&SA PC_exec(), PC_infos(), SCR_free_tbl();

&TI Description du fichier
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le fichier contient trois types de lignes :

&EN lignes de description commen‡ant par un + et contenant un ensemble
    de champs s‚par‚s par des :
&EN lignes de commandes suivant la ligne de d‚finition et contenant
    ‚ventuellement des paramŠtres %1, %2 et %3
&EN lignes de commentaires commen‡ant par un ##

Par exemple, un fichier de d‚finition des imprimantes install‚es sur le
systŠme peut ˆtre du type suivant :

&CO
    +ps402:PS:1:Agfa P400PS laserprinter 2th floor
	net use lpt3: /d
	net use lpt3: suncalc:ps402
	net print %1 lpt3
	net use lpt3: /d
    +lw3:PS:1:Apple Laserwriter 3th floor
	net use lpt3: /d
	net use lpt3: suncalc:lw3
	net print %1 lpt3
	net use lpt3: /d
    +lpt1:HPDJ:1:HP Deskjet 500 on lpt1:
	copy %1 lpt1
    +psfile:PS:2:Postscript file
	copy %1 %2
    ##+lpt1:FASCII:1:Standard DOS Printer on lpt1:
    ##    copy %1 lpt1
    ##+lpt2:FASCII:1:Standard DOS Printer on lpt2:
    ##    copy %1 lpt1
&TX
Les lignes de d‚finition sont ici d‚coup‚es en 4 champs :

&EN le nom logique de l'imprimante
&EN le langage de l'imprimante (PS=Postscript, etc)
&EN le nombre de paramŠtres requis
&EN la description en langage clair de l'imprimante

Les lignes qui suivent sont les commandes … ex‚cuter pour imprimer
un fichier.

Le caractŠre + de d‚but de ligne peut ˆtre modifi‚ par l'utilisateur
en rempla‡ant le pointeur PC_BEG par un string au choix.

De mˆme, le s‚parateur peut ˆtre red‚fini en pla‡ant dans la
variable PC_SEPS un pointeur vers la liste des caractŠres accept‚s comme
s‚parateurs de champ.
=======================================================================*/

char **PC_list(filename, fld_nb, fld_select, fld_vals, maxparms)
char    *filename;
int     fld_nb, fld_select, maxparms;
char    *fld_vals;
{
    U_ch    **tbl = 0, **def, buf[132];
    int     n = 0, type, size;

    if(PC_open(filename)) return((char **)tbl);

    while(1) {
	type = PC_getline("", buf);
	if(type < 0) break;
	if(type == 0) {
	    def = SCR_vtoms(buf + strlen(PC_BEG), PC_SEPS);
	    size = SCR_tbl_size(def);
	    if(fld_select >= 0 && fld_vals && fld_vals[0]) {
		if(fld_select >= size) goto next;
		if(!U_is_in(def[fld_select][0], fld_vals)) goto next;
		}
	    if(fld_nb >= size)
		SCR_add_ptr(&tbl, &n, "");
	    else
		SCR_add_ptr(&tbl, &n, def[fld_nb]);
	next:
	    SCR_free_tbl(def);
	    }
	}
    SCR_add_ptr(&tbl, &n, NULL);
    fclose(PC_FD);
    return((char **)tbl);
}

