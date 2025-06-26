#include "scr4.h"
#ifndef PRWINDOWS

/*NH*/
A2mToGdiEMF()
{
    return(-1);
}
#else

#include <scr4w.h>
#include "s_a2m.h"

extern int  A2M_GDI_W, A2M_GDI_H;
extern int  A2M_GDI_MARGX, A2M_GDI_MARGY;

/*NH*/
A2mGdiEMFInit(filename, w, h)
U_ch    *filename;
int     w, h;
{
    if(WprEMFInit(filename, w, h, 0L)) return(-1);
    A2mGdiAllInit();
    A2M_GDI_MARGX = A2M_GDI_MARGY = 0; /* JMP 24-02-98 */
    A2M_GDI_W = WprHORZRES;            /* JMP 24-02-98 */
    A2M_GDI_H = WprVERTRES;            /* JMP 24-02-98 */
    return(0);
}

/*NH*/
A2mGdiEMFEnd()
{
    WprEMFEnd();
    A2mGdiAllEnd();
    return(0);
}

/*NH*/
A2mGdiWMFEnd()
{
    WprWMFEnd();
    A2mGdiAllEnd();
    return(0);
}

/*NH*/
A2mGdiWMFInit(filename, w, h)
U_ch    *filename;
int     w, h;
{
    if(WprWMFInit(filename, w, h)) return(-1);
    A2mGdiAllInit();
    A2M_GDI_MARGX = A2M_GDI_MARGY = 0; /* JMP 24-02-98 */
    A2M_GDI_W = WprHORZRES;            /* JMP 24-02-98 */
    A2M_GDI_H = WprVERTRES;            /* JMP 24-02-98 */
    return(0);
}


/* ================================================================
Interprète le contenu d'un fichier a2m et génère un fichier au format
Enhanced MetaFile de Windows.

&EN a2mfile = nom du fichier a2m à imprimer
&EN outfile = nom du fichier EMF résultat
&EN w = largeur en mm du dessin
&EN h = hauteur en mm du dessin

&RT La fonction retourne 0 si le processus s'est bien déroulé, -1 sinon.

La syntaxe des fichiers a2m est décrite dans un chapitre spécifique.

&TI Fichier a2m.ini
───────────────
Le fichier a2m.ini (ou un autre) contient des paramètres pour
l'impression et la lecture du fichier a2m. Pour que ces paramètres
soient pris en compte par le programme A2mToGdiEMF(), il faut appeler la
fonction A2mGdiReadIni(filename) avant de lancer la fonction
d'interprétation et d'impression.

&CO
    #include <s_a2m.h>

    A2mGdiReadIni("myfile.ini");
    A2mToGdiEMF("myfile.a2m", "myfile.emf", 100, 100);
&TX

&TI Variables globales
──────────────────
Les variables globales décrites dans le fichier .ini peuvent également
être modifiées dans le programme.

&IT Variables influençant la lecture du fichier
──────────────────────────────────────────
Ces variables sont définies dans la section [A2M] du fichier ini.

&EN int A2M_ESCCH = caractère d'escape (enrichissements et caractères
    spéciaux) : '\'par défaut
&EN int A2M_CMDCH = caractère de commande ('.' par défaut)
    spéciaux)
&EN int A2M_DEFCH = caractère préfixant les macros ('&' par défaut)
&EN int A2M_SEPCH = caractère de séparation des cellules ('&' par défaut)
&EN int A2M_LFON = conserve les linefeed (1) ou non (0)
&EN int A2M_BLON = conserve les blancs (1) ou non (0)
&EN char A2M_CURTAG[41] = paragraphe par défaut ("par_0")

&IT Variables influençant l'impression
──────────────────────────────────
Ces variables sont définies dans la section [GDI] du fichier .ini.

&EN int A2M_FONTSIZE = taille par défaut des caractères dans les
    paragraphes (10 pts par défaut)
&EN int A2M_FONTINCR = incrément de taille de caractères (2 par défaut)
&EN int A2M_TFONTSIZE = taille par défaut des caractères dans les
    tableaux (8 pts par défaut)
&EN U_ch *A2M_PGHEAD = titre de page ("" par défaut)
&EN U_ch *A2M_PGFOOT = pied de page ("" par défaut)
&EN int A2M_TSHADING_COL[2] = couleurs de la brosse de hachurage des
    titres([0]) et corps([1]) des tableaux
&EN2 0 = noir
&EN2 1 = blanc
&EN2 2 = rouge
&EN2 3 = vert
&EN2 4 = bleu
&EN2 5 = cyan
&EN2 6 = magenta
&EN2 7 = jaune

&EN int A2M_TSHADING_BRUSH[2] = brosse de hachurage des titres([0]) et
    corps([1]) des tableaux
&EN2 7 = 50%
&EN2 6 = 25%
&EN2 5 = 12%
&EN2 4 = 6%
&EN2 3 = 3%
&EN2 0-2 = 0%

&EN int A2M_GDI_COLOR = 1 (dft) pour impression en couleur, 0 en B/W
&EN int A2M_GDI_TBORDER = épaisseur des cadres des tableaux (2 par dft)
&EN int A2M_GDI_LMARG = marge de gauche de la page (12 pts par dft)
&EN int A2M_GDI_RMARG = marge de droite de la page (12 pts par dft)
&EN int A2M_GDI_TMARG = marge de haut de la page (12 pts par dft)
&EN int A2M_GDI_BMARG = marge de bas de la page (12 pts par dft)

&RT
&EN 0 en cas de succès
&EN -1 si le fichier ne peut être ouvert
&EN -2 si le fichier EMF ne peut être créé

&SA A2mGdiReadIni(), A2mToGdiPrinter(), A2mToRtf(), A2mToMif(),
    A2mToHtml(), A2mPrintError()
==================================================================== */

A2mToGdiEMF(a2mfile, outfile, w, h)
U_ch    *a2mfile, *outfile;
int     w, h;
{
    A2MFILE *af;
    A2MOBJ  *ao;

    af = A2mOpen(a2mfile, 0);
    if(af == 0) return(-1);

    if(A2mGdiEMFInit(outfile, w, h)) {
	A2mClose(af);
	return(-2);
	}

    while(1) {
	ao = A2mRead(af);
	if(ao == 0) break;
	A2mGdiPrintObj(ao);
	}

    A2mGdiPrintObj((A2MOBJ *)0);
    A2mGdiEMFEnd();
    A2mClose(af);
    return(0);
}

/* ========================================================
Cette fonction traduit le graphique défini dans le fichier a2mfile
en un fichier EMF outfile.

&EN a2mfile = fichier a2m ne contenant qu'un graphique.
&EN outfile = fichier EMF résultat

===========================================================*/

A2mGdiEMFGrf(char *a2mfile, char *outfile)
{
    A2MFILE *af;
    A2MOBJ  *ao;
    A2MGRF  *ag;
    int     w, h;

    af = A2mOpen(a2mfile, 0);
    if(af == 0) return(-1);
    af->af_outfile = SCR_stracpy(outfile);
    af->af_dest = A2M_DESTGDIEMF;
    ao = A2mRead(af);
    if(ao) {
	if(ao->ao_type != A2M_GRF) goto fin;
	ag = ao->ao_ptr;

	if(A2mGdiEMFInit(outfile, 120, 90)) goto fin;
	A2mCalcObjSize(ao, A2M_GDI_W);
	A2mGdiWriteObj(ao);
	w = 2 * A2M_GDI_MARGX + ao->ao_w;
	h = 2 * A2M_GDI_MARGY + ao->ao_h;
	w = (w * 254) / (10 * WprLOGX);
	h = (h * 254) / (10 * WprLOGY);
	A2mGdiWriteObj(ao);
	A2mGdiEMFEnd();

	if(A2mGdiEMFInit(outfile, w, h)) goto fin;
	A2mGdiPrintObj(ao);
	A2mGdiPrintObj((A2MOBJ *)0);
	A2mGdiEMFEnd();
	}

fin:
    A2mClose(af);
    return(0);
}


#endif
