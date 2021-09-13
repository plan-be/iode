#include <scr4.h>
/* ====================================================================
Fonction d'initialisation d'une page HTML. G‚n‚re les sections
<HEAD> et <TITLE>.

&EN html = FILE *
&EN title = titre de la page

&EX
    MyHtmlForm()
    {
	FILE    *html = stdout;
	U_ch    **mtype = SCR_vtom("Type1,Type2,Type3", ',');
	U_ch    **mlang = SCR_vtom("Fran‡ais,N‚erlandais,Anglais", ',');

	HtmlInit(html, "Automatic form generation");
	fprintf(html, "<Body>\n");
	HtmlFormInit(html, "/htbin/dw_update");
	HtmlFormText(html, "crdate", 10, "10/06/96");
	HtmlFormText(html, "docname", 64, "c:/usr/doc/test.dwt");
	HtmlFormMenu(html, "ftype", mtype, 2);
	HtmlFormRadio(html,"lang", mlang, 2);
	HtmlFormTextArea(html, "abstract",
			"This is a test\nThis is a test\n", 3, 64);
	HtmlFormEnd(html, "OK", "Clear");
	fprintf(html, "</Body>\n");
	HtmlEnd(html, "Last updated 09/05/06");
	SCR_free_tbl(mtype);
	SCR_free_tbl(mlang);
    }
&TX
&SA Html*()
=======================================================================*/

int HtmlInit(html, title)
FILE    *html;
char    *title;
{
    fprintf(html, "<HTML> <HEAD> <TITLE> %s </TITLE> </HEAD>", title);
    return(0);
}

/* ====================================================================
Fermeture d'une page HTML. G‚n‚re la section <Address>.

&EN html = FILE *
&EN footer = contenu de la section <Address>

&EX
&TX
    Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlEnd(html, footer)
FILE    *html;
char    *footer;
{
    fprintf(html, "<ADDRESS> %s </ADDRESS>\n</HTML>", footer);
    return(0);
}

/* ====================================================================
Initialisation d'un FORM. La m‚thode utilis‚e pour ex‚cuter le FORM est
"POST".

&EN html = FILE *
&EN action = nom du CGI

&EX
&TX
    Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlFormInit(html, action)
FILE    *html;
char    *action;
{
    fprintf(html, "<FORM METHOD=POST ACTION=\"%s\">\n", action);
    return(0);
}

/* ====================================================================
Fin d'un FORM dans une page HTML.

&EN html = FILE *
&EN submit = texte du bouton destin‚ … ex‚cuter le FORM
&EN reset = texte du bouton destin‚ … remettre les valeurs par d‚faut
dans le FORM

&EX
&TX
    Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlFormEnd(html, submit, reset)
FILE    *html;
char    *submit, *reset;
{
    fprintf(html, "<INPUT TYPE=\"submit\" VALUE=\"%s\">\n", submit);
    fprintf(html, "<INPUT TYPE=\"reset\"  VALUE=\"%s\">\n", reset);
    fprintf(html, "</FORM>\n");
    return(0);
}

/* ====================================================================
Bouton suppl‚mentaire dans une page HTML. Cette fonction doit ˆtre
pr‚c‚d‚e d'un HtmlFormInit().

&EN html = FILE *
&EN name = nom du bouton dans le FORM
&EN text = texte du bouton

&EX
&TX
    Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlFormButton(html, name, text)
FILE    *html;
U_ch    *text, *name;
{
    fprintf(html, "<B><INPUT TYPE=\"submit\" VALUE=\"%s\"", text);
    if(name) fprintf(html, " NAME=\"%s\"", name);
    fprintf(html, "></B>\n");
    return(0);
}

/* ====================================================================
Champ hidden dans un FORM d'une page HTML. Cette fonction doit ˆtre
pr‚c‚d‚e d'un HtmlFormInit().

&EN html = FILE *
&EN name = nom du champ dans le FORM
&EN text = texte du champ

&EX
&TX
    Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlFormHidden(html, name, text)
FILE    *html;
U_ch    *name, *text;
{
    fprintf(html, "<INPUT TYPE=\"hidden\" NAME=\"%s\" VALUE=\"%s\">\n", name, text);
    return(0);
}

/* ====================================================================
Champ mot de passe dans un FORM d'une page HTML. Cette fonction doit
ˆtre pr‚c‚d‚e d'un HtmlFormInit(). La longueur est fix‚e … 15 caractŠres.

&EN html = FILE *
&EN name = nom du champ

&EX
&TX
    Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlFormPassword(html, name)
FILE    *html;
U_ch    *name;
{
    fprintf(html, "<INPUT TYPE=\"password\" SIZE=15 NAME=\"%s\">\n", name);
    return(0);
}

/* ====================================================================
Champ texte d'une ligne dans un FORM d'une page HTML. Cette fonction
doit ˆtre pr‚c‚d‚e d'un HtmlFormInit().

&EN html = FILE *
&EN name = nom du champ
&EN lg = longueur maximum du champ
&EN value = texte par d‚faut

Si le champ d‚passe 64 caractŠres, la partie visible est limit‚e … 64
caractŠres.

&EX
&TX
Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlFormText(html, name, lg, value)
FILE    *html;
char    *name, *value;
int     lg;
{
    int     vlg = lg;

    if(vlg > 64) vlg = 64;
    fprintf(html,
	"<INPUT TYPE=\"text\" NAME=\"%s\" MAXLENGTH=%d SIZE=%d VALUE=\"%s\">\n",
	name, lg, vlg, value);
    return(0);
}

/* ====================================================================
Champ de type Radio button dans un FORM d'une page HTML. Cette fonction
doit ˆtre pr‚c‚d‚e d'un HtmlFormInit().

&EN html = FILE *
&EN name = nom du champ
&EN options = tableaux de pointeurs vers le texte des options
&EN choice = choix par d‚faut

&EX
&TX
Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlFormRadio(html, name, options, choice)
FILE    *html;
char    *name, **options;
int     choice;
{
    int     i;

    for(i = 0; options[i]; i++) {
	if (i == choice)
	    fprintf(html,
		"<INPUT TYPE=\"radio\" NAME=\"%s\" VALUE=\"%s\" CHECKED> %s\n",
		name, options[i], options[i]);
	else
	    fprintf(html,
		"<INPUT TYPE=\"radio\" NAME=\"%s\" VALUE=\"%s\"> %s\n",
		name, options[i], options[i]);

    }
    return(0);
}

/* ====================================================================
Champ de type Menu dans un FORM d'une page HTML. Cette fonction
doit ˆtre pr‚c‚d‚e d'un HtmlFormInit().

&EN html = FILE *
&EN name = nom du champ
&EN options = tableaux de pointeurs vers le texte des options
&EN choice = choix par d‚faut

&EX
&TX
Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlFormMenu(html, name, options, choice)
FILE    *html;
char    *name, **options;
int     choice;
{
    int     i;

    fprintf(html, "<SELECT NAME=\"%s\">\n", name);
    for(i = 0; options[i]; i++) {
	if (i == choice)
	    fprintf(html, "<OPTION SELECTED> %s\n", options[i]);
	else
	    fprintf(html, "<OPTION> %s\n", options[i]);
    }
    fprintf(html, "</SELECT>\n" );
    return(0);
}

/* ====================================================================
Champ texte de plusieurs lignes dans un FORM d'une page HTML. Cette
fonction doit ˆtre pr‚c‚d‚e d'un HtmlFormInit().

&EN html = FILE *
&EN name = nom du champ
&EN value = texte par d‚faut
&EN nl = nombre de lignes visibles
&EN nc = nombre de colonnes visibles

Le champ est normalement accompagn‚ de scrollbar horizontaux et verticaux.
Si longueur n'est pas limit‚e.

&EX
&TX
Voir HtmlInit().

&SA Html*()
=======================================================================*/

int HtmlFormTextArea(html, name, value, nl, nc)
FILE    *html;
char    *name, *value;
int     nl, nc;
{
    fprintf(html, "<TEXTAREA NAME=\"%s\" ROWS=%d COLS=%d>\n", name, nl, nc);
    fprintf(html, "%s\n", value);
    fprintf(html, "</TEXTAREA>\n");
    return(0);
}







