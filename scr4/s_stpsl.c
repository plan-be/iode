#include <stdio.h>
#include "s_strs.h"

extern FILE *PS_IN_FD, *PS_OUT_FD;
extern int  PS_LINE_NB, PS_PAGE_H, PS_PAGE_W;
extern float PS_MARGV, PS_MARGH, PS_FONTSIZE, PS_SPACING;
int  PS_NVL, PS_NHL, PS_V, PS_H;

/* ======================================================================
Transforme un fichier ASCII PC-8 définissant des étiquettes en Postscript.
Chaque étiquette commence par un signe < et se termine par un signe >.
Les caractères en dehors des étiquettes sont ignorés.

Les paramètres sont semblables à ceux de PS_text() :

&EN ifl : input filename. Si nul, stdin.
&EN ofl : output filename. Si nul, stdout.
&EN font : police de caractère. Si nul, "Helvetica"
&EN size : taille en points. Si nul, vaut 10.0.
&EN margv : marge verticale en points. Si nul, vaut 30.0
&EN margh : marge horizontale en points. Si nul, vaut 20.0
&EN spacing : interligne en points. Si nul, vaut 2.0

En plus :

&EN nvl : nombre d'étiquettes verticalement
&EN nhl : nombre d'étiquettes horizontalement

Le papier est divisé en rectangles correspondant à ces paramètres.

L'interligne est calculé comme size + spacing.

&EX
    PS_labels("tmp", "lpt1", 8, 2, "Courier", 12.0, 30.0, 20.0, 2.0);
&TX
&RT -1 en cas d'erreur (fichier non trouvé), 0 sinon.
&SA PS_text()
------------------------------------------------------------------------- */

PS_labels(ifl, ofl, nvl, nhl, font, size, margv, margh, spacing)
char    *ifl, *ofl, *font;
int     nvl, nhl;
double  size, margv, margh, spacing;
{
    if(PS_init(ifl, ofl, font, size, margv, margh, spacing, 0)) return(-1);

    PS_V = PS_H = -1;
    PS_NHL = nhl;
    PS_NVL = nvl;

    while(1) {
	if(PS_next_label()) break;
	PS_new_label();
	PS_eat_label();
	}
    fprintf(PS_OUT_FD, "showpage\n");
    PS_close();
    return(0);
}

/*NH*/
PS_next_label()
{
    int     ch;

    while(1) {
	ch = getc(PS_IN_FD);
	if(ch == EOF) return(-1);
	if(ch == '<') return(0);
	}
}

/*NH*/
int PS_eat_label()
{
    int     ch;

    while(1) {
	PS_lab_nl();
	while(1) {
	    ch = getc(PS_IN_FD);
	    if(ch == '>' || ch == '\n' || ch == EOF) break;
	    PS_mif_char(ch);
	    }
	fprintf(PS_OUT_FD, ")show\n");
	if(ch == '>' || ch == EOF) return(0);
	}
    return(0);
}

/*NH*/
int PS_lab_nl()
{
    fprintf(PS_OUT_FD, "%g %g moveto (",
	PS_MARGH + (PS_H * PS_PAGE_W) / PS_NHL,
	((PS_NVL - PS_V) * PS_PAGE_H) / PS_NVL - PS_MARGV - PS_LINE_NB * (PS_FONTSIZE + PS_SPACING));

    PS_LINE_NB++;
    return(0);
}

/*NH*/
int PS_new_label()
{
    PS_LINE_NB = 0;
    if(PS_H < 0) PS_H = PS_V = 0;
    else {
	PS_H = (PS_H + 1) % PS_NHL;
	if(PS_H == 0) {
	    PS_V = (PS_V + 1) % PS_NVL;
	    if(PS_V == 0) fprintf(PS_OUT_FD, "showpage\n");
	    }
	}
    return(0);
}





