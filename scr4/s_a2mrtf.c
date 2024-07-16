#include "s_a2m.h"
#include <s_ini.h>
#ifndef UNIX
#include <io.h>
#endif

FILE    *A2M_fdrtf, *A2M_fdcnt;
int     A2M_RTF_INTOPIC = 0, A2M_RTF_CURFIG = 1;

/********* A2M.INI DEFAULT VALUES PARMS *****************/

/* Public */
int     A2M_RTF_TCOLOR = 1, A2M_RTF_TBORDER = 1, A2M_RTF_TOC = 0;
U_ch    *A2M_RTF_TITLE, *A2M_RTF_COPYRIGHT;
int     A2M_RTF_TCOL1 = 60; /* mm */
int     A2M_RTF_TCOLN = 15; /* mm */
int     A2M_RTF_TSPLIT = 1, A2M_RTF_TWIDTH = 165;
int     A2M_RTF_HLINES = 0, A2M_RTF_VLINES = 0;
int     A2M_RTF_COMPRESS = 1;
int     A2M_RTF_GIFTOBMP = 0; /* JMP 16-12-97 */
int     A2M_RTF_GIFTOPNG = 0; /* JMP 23-08-2015 */
int     A2M_RTF_OEMTOANSI = 1; /* JMP 12-04-07 */
extern  U_ch  *A2M_PGHEAD, *A2M_PGFOOT; /* JMP 30-08-11 */

/* Private */
int     A2M_RTF_TOPIC = -1, A2M_RTF_HELP = 0, A2M_RTF_LEVEL = 0;
int     A2M_RTF_TOCLEVEL, A2M_RTF_TOCPLEVEL, A2M_RTF_TOCNB;
U_ch    *A2M_RTF_TOCTXT;

/* ================================================================
Interpr�te le contenu d'un fichier a2m et g�n�re un fichier .rtf et
�ventuellement d'autres pour obtenir un input pour la cr�ation d'un
fichier d'aide Win32.

&EN a2mfile = nom du fichier a2m � interpr�ter
&EN outfile = nom du fichier rtf � g�n�rer
&EN wh      = 1 pour g�n�rer un fichier d'aide, 0 pour un fichier
	      rtf normal

&RT La fonction retourne 0 si le processus s'est bien d�roul�, -1 sinon.

La syntaxe des fichiers a2m est d�crite dans un chapitre sp�cifique.

&TI Fichier a2m.ini
���������������
Le fichier a2m.ini (ou un autre) contient des param�tres pour
l'impression et la lecture du fichier a2m. Pour que ces param�tres
soient pris en compte par le programme A2mToRtf(), il faut appeler la
fonction A2mRtfReadIni(filename) avant de lancer la fonction
d'interpr�tation et d'impression.

&CO
    #include <s_a2m.h>

    A2mRtfReadIni("myfile.ini");
    A2mToRtf("myfile", "test.rtf", 1);
&TX

&TI Variables globales
������������������
Les variables globales d�crites dans le fichier .ini peuvent �galement
�tre modifi�es dans le programme.

&IT Variables influen�ant la lecture du fichier
������������������������������������������
Ces variables sont d�finies dans la secion [A2M] du fichier ini.

&EN int A2M_ESCCH = caract�re d'escape (enrichissements et caract�res
    sp�ciaux) : '\'par d�faut
&EN int A2M_CMDCH = caract�re de commande ('.' par d�faut)
    sp�ciaux)
&EN int A2M_DEFCH = caract�re pr�fixant les macros ('&' par d�faut)
&EN int A2M_SEPCH = caract�re de s�paration des cellules ('&' par d�faut)
&EN int A2M_LFON = conserve les linefeed (1) ou non (0)
&EN int A2M_BLON = conserve les blancs (1) ou non (0)
&EN char A2M_CURTAG[41] = paragraphe par d�faut ("par_0")

&IT Variables influen�ant le fichier g�n�r�
���������������������������������������
Ces variables sont d�finies dans la section [RTF] du fichier .ini.

&EN int A2M_FONTSIZE = taille par d�faut des caract�res dans les
    paragraphes (10 pts par d�faut)
&EN int A2M_FONTFAMILY = police de caract�re par d�faut ('H', 'T' ou 'C')
&EN int A2M_FONTINCR = incr�ment de taille de caract�res (2 par d�faut)
&EN int A2M_TFONTSIZE = taille par d�faut des caract�res dans les
    tableaux (8 pts par d�faut)
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

&EN int A2M_RTF_TCOLOR = 1 (dft) pour impression en couleur des
    tableaux, 0 en B/W
&EN int A2M_RTF_TBORDER = �paisseur des cadres des tableaux (1 par dft)
&EN U_ch *A2M_RTF_TITLE = titre du fichier d'aide (si wh == 1)
&EN U_ch *A2M_RTF_COPYRIGHT = texte du Copyright de l'aide (si wh == 1)
&EN int A2M_RTF_TCOL1 = largeur de la colonne 1 des tableaux en mm
	(dft 60)
&EN int A2M_RTF_TCOLN = largeur des autres colonnes des tableaux en mm
	(dft 15)
&EN int A2M_RTF_HLINES = 1 pour des lignes horizontales dans les
    tableaux (dft 0)

&EN int A2M_RTF_VLINES = 1 pour des lignes verticales dans les tableaux
    (dft 0) ;

&NO Certains param�tres ne sont pas g�r�s par le compilateur d'aide
    de Windows 95, notamment dans les attributs des tableaux.

&TI G�n�rer une aide Windows 95
���������������������������
Si on souhaite g�n�rer une aide sous Windows 95, les fichiers suivants
sont g�n�r�s par le programme A2mToRtf() :

&EN .rtf = fichier contenant le texte de l'aide
&EN .cnt = fichier contenant la structure de l'aide (table des mati�res)
&EN .hpj = fichier contenant les param�tres de g�n�ration

Pour construire le fichier d'aide, il faut disposer du compilateur
d'aide de Microsot : hcw.exe. Le processus se d�roule comme suit :

&EN cr�er le ou les fichiers a2m
&EN contruire l'aide :
&CO
    A2mToRtf("myfile.a2m", "myhelp.rtf", 1)
&TX
&EN compiler :
&CO
    hcw /A/C myhelp
&TX
&EN tester :
&CO
    winhelp myhelp
&TX

&RT
&EN 0 en cas de succ�s
&EN -1 si le fichier ne peut �tre ouvert
&EN -2 si les fichiers output n'ont p� �tre cr��s

&SA A2mRtfReadIni(), A2mToRtf(), A2mToMif(), A2mToHtml(), A2mPrintError()
==================================================================== */

A2mToRtf(a2mfile, outfile, wh)
char    *a2mfile, *outfile;
int     wh;
{
    A2MFILE *af;
    A2MOBJ  *ao;
    int     rc;

    A2mReadTopics(a2mfile);  /* JMP 17-05-99 */
    af = A2mOpen(a2mfile, 0);
    if(af == 0) {
	A2mFreeTopics();  /* JMP 24-05-99 */
	return(-1);
	}

    if(A2mRtfInit(outfile, wh)) {
	A2mClose(af);
	A2mFreeTopics();  /* JMP 24-05-99 */
	return(-2);
	}

    while(1) {
	ao = A2mRead(af);
	if(ao == 0) break;
	A2mRtfPrintObj(ao);
	}

    A2mFreeTopics();  /* JMP 24-05-99 */
    rc = A2mRtfEnd(outfile);
    A2mClose(af); /* JMP 12-04-07 : d�plac� pour avoir le catalogue sinon free avant dump */
    return(rc);
}

/*NH*/
A2mRtfInit(outfile, wh)
char    *outfile;
int     wh;
{
    char        buf[256];
    extern int  A2M_CUROBJ;

    A2M_RTF_HELP = wh;
    A2M_RTF_TOCPLEVEL = A2M_RTF_TOCLEVEL = A2M_RTF_TOCNB = -1;
    A2M_RTF_TOCTXT = 0;
    A2M_CUROBJ = 1;
    A2M_RTF_CURFIG = 1;
    A2M_RTF_INTOPIC = 0;

    SCR_change_ext(buf, outfile, "tmp");
    A2M_fdrtf = fopen(buf, "w+");
    if(A2M_fdrtf == 0) return(-2);

    if(A2M_RTF_HELP == 0) return(0);
    SCR_change_ext(buf, outfile, "cnt");
    A2M_fdcnt = fopen(buf, "w+");
    if(A2M_fdcnt == 0) {
	fclose(A2M_fdrtf);
	return(-2);
	}
    SCR_change_ext(buf, outfile, "hlp");
    fprintf(A2M_fdcnt, ":Base %s\n", buf);
    if(A2M_RTF_TITLE) fprintf(A2M_fdcnt, ":Title %s\n", A2M_RTF_TITLE);
    return(0);
}

/*NH*/
A2mRtfEnd(outfile)
char    *outfile;
{
    char    buf[256];

    fclose(A2M_fdrtf);
    A2M_fdrtf = fopen(outfile, "w+");
    if(A2M_fdrtf == 0) return(-2);

    A2mRtfHeader();
    A2mRtfStyles();
    A2mFrCatFile(A2M_fdrtf, outfile, "tmp");
    fprintf(A2M_fdrtf, "\n}\n");
    fclose(A2M_fdrtf);
    A2M_fdrtf = 0;
    SCR_change_ext(buf, outfile, "tmp");
#ifdef __GNUC__
    unlink(buf);
#else
    _unlink(buf);
#endif

    if(A2M_RTF_HELP) {
	A2mRtfPrintTopicCnt((A2MTOP *)0);
	fclose(A2M_fdcnt);
	A2M_fdcnt = 0;
	A2mRtfHpj(outfile);
	}
    return(0);
}

/*NH JMP 30-08-11 */
void A2mRtfPrintPgHeadFoot(U_ch * txt, int type)
{
    char    buf[1024];
    if(txt) {
	if(strlen(txt) > 512) txt[512] = 0;
	strcpy(buf, txt);
	SCR_replace(buf, "%d", "{\\field{\\fldinst page}}");

	if(type) fprintf(A2M_fdrtf, "{\\footer{\\pard\\plain \\s1\\sb60\\sa60\\f2\\fs18\\qc %s \\par}}\n", buf);
	else     fprintf(A2M_fdrtf, "{\\header{\\pard\\plain \\s2\\sb60\\sa60\\f2\\fs18\\qc %s \\par}}\n", buf);
	}
    SCR_free(txt);
}


/*NH*/
A2mRtfPrintObj(ao)
A2MOBJ  *ao;
{
    char        msg[80];
    extern int  A2M_CUROBJ;

    sprintf(msg, "Printing Object %d", A2M_CUROBJ++);
    A2mMessage(msg);
    A2mRtfPrintPgHeadFoot(A2M_PGHEAD, 0);
    A2mRtfPrintPgHeadFoot(A2M_PGFOOT, 1);
    A2M_PGHEAD = A2M_PGFOOT = 0;
    
    if(ao->ao_type == A2M_PAR) {
        A2mRtfPrintPar((A2MPAR *)ao->ao_ptr, 0);
        A2mFreeObj(ao);
        return(0);
	}

    if(ao->ao_type == A2M_TOPIC) {
        A2mRtfPrintTopic((A2MTOP *)ao->ao_ptr);
        A2mFreeObj(ao);
        return(0);
	}

    if(ao->ao_type == A2M_TBL) {
        A2mRtfPrintTbl((A2MTBL *)ao->ao_ptr);
        A2mFreeObj(ao);
        return(0);
	}

    if(ao->ao_type == A2M_GRF) {
        A2mRtfPrintGrf(ao);
#if !defined(SCRW32) && !defined(DOSW32) && !defined(SCRGNOME)
        A2mFreeObj(ao);
#endif
        return(0);
	}

    if(ao->ao_type == A2M_PAGE) {
        A2mRtfPrintPage((A2MPAGE *)ao->ao_ptr, 0);
        A2mFreeObj(ao);
        return(0);
	}
    return(0);
}


/*NH*/
int A2mRtfHpj(char* outfile)
{
    char    buf[256];

    SCR_change_ext(buf, outfile, "hpj");
    A2M_fdrtf = fopen(buf, "w+");
    if(A2M_fdrtf == 0) return(0);

    fprintf(A2M_fdrtf, "[OPTIONS]\n");
    if(A2M_RTF_TITLE) fprintf(A2M_fdrtf, "TITLE=%s\n", A2M_RTF_TITLE);
    fprintf(A2M_fdrtf, "COMPRESS=%s\n", A2M_RTF_COMPRESS ? "True" : "False");
    SCR_change_ext(buf, outfile, "cnt");
    fprintf(A2M_fdrtf, "CNT=%s\n", buf);
    if(A2M_RTF_COPYRIGHT)
	fprintf(A2M_fdrtf, "COPYRIGHT=%s\n", A2M_RTF_COPYRIGHT);
    fprintf(A2M_fdrtf, "CONTENTS=T0000\n");
    fprintf(A2M_fdrtf, "CITATION=Created by SCR4/AL1 Help Tools\n");
    fprintf(A2M_fdrtf, "ERRORLOG=tmp.log\n");
    fprintf(A2M_fdrtf, ";FTS=0x1F\n");
    fprintf(A2M_fdrtf, "OLDKEYPHRASE=No\n");
    fprintf(A2M_fdrtf, "WARNING=1\n");
    fprintf(A2M_fdrtf, "\n[FILES]\n");
    fprintf(A2M_fdrtf, "%s\n", outfile);

    fprintf(A2M_fdrtf, "\n[BITMAPS]\n\n");

    fprintf(A2M_fdrtf, "\n[CONFIG]\n");
    fprintf(A2M_fdrtf, "BrowseButtons()\n");

    fclose(A2M_fdrtf);
    return(0);
}

/*NH*/
A2mRtfPrintTopic(atp)
A2MTOP  *atp;
{
    if(A2M_RTF_HELP == 0 || atp == 0) return(0);

    if(A2M_RTF_INTOPIC) fprintf(A2M_fdrtf, "\\page\n");
    A2M_RTF_INTOPIC = 1;

    fprintf(A2M_fdrtf, "#{\\footnote T%04d}\n", atp->atp_nb);
    fprintf(A2M_fdrtf, "${\\footnote ");
    A2mRtfPrintTxt(A2M_fdrtf, atp->atp_keyws);
    fprintf(A2M_fdrtf, "}\n");
    fprintf(A2M_fdrtf, "K{\\footnote ");
    A2mRtfPrintTxt(A2M_fdrtf, atp->atp_keyws);
    fprintf(A2M_fdrtf, "}\n");
    fprintf(A2M_fdrtf, "+{\\footnote T%04d}\n", atp->atp_nb + 1);

    A2mRtfPrintTopicCnt(atp);
    return(0);
}

/*NH
A2mRtfPrintTopicCnt(atp)
A2MTOP  *atp;
{
    int     clevel = 0;

    if(atp) clevel = atp->atp_level;

    if(A2M_RTF_TOCLEVEL >= 0) {
	if(clevel <= A2M_RTF_TOCLEVEL &&
	   A2M_RTF_TOCLEVEL >= A2M_RTF_TOCPLEVEL)  {
	    fprintf(A2M_fdcnt, "%d %s=T%04d\n", A2M_RTF_TOCLEVEL + 1,
				A2M_RTF_TOCTXT, A2M_RTF_TOCNB);
	    }
	else {
	    fprintf(A2M_fdcnt, "%d %s\n", A2M_RTF_TOCLEVEL + 1,
					  A2M_RTF_TOCTXT);
	    fprintf(A2M_fdcnt, "%d %s=T%04d\n", A2M_RTF_TOCLEVEL + 2,
				A2M_RTF_TOCTXT, A2M_RTF_TOCNB);
	    clevel++;
	    }
	}

    SCR_free(A2M_RTF_TOCTXT);
    A2M_RTF_TOCTXT = 0;
    if(atp) {
	A2M_RTF_TOCTXT   = SCR_stracpy(atp->atp_keyws);
	SCR_OemToAnsi(A2M_RTF_TOCTXT, A2M_RTF_TOCTXT);
	A2M_RTF_TOCPLEVEL = A2M_RTF_TOCLEVEL;
	A2M_RTF_TOCLEVEL = clevel;
	A2M_RTF_TOCNB    = atp->atp_nb;
	}

    return(0);
}
*/

/*NH*/
A2mRtfPrintCntTxt(txt)
U_ch    *txt;
{
    int     i;

    for(i = 0 ; txt[i] ; i++) {
	if(txt[i] == '=') fprintf(A2M_fdcnt, "\\=");
	else              fprintf(A2M_fdcnt, "%c", txt[i]);
	}
    return(0);
}

/*NH*/
A2mRtfPrintTopicCnt(atp)
A2MTOP  *atp;
{
    int     clevel = 0;

    if(atp) clevel = atp->atp_level;

    if(A2M_RTF_TOCLEVEL >= 0) {
	if(clevel <= A2M_RTF_TOCLEVEL &&
	   A2M_RTF_TOCLEVEL >= A2M_RTF_TOCPLEVEL)  {
	    fprintf(A2M_fdcnt, "%d ", A2M_RTF_TOCLEVEL + 1);
	    A2mRtfPrintCntTxt(A2M_RTF_TOCTXT);
	    fprintf(A2M_fdcnt, "=T%04d\n", A2M_RTF_TOCNB);
	    }
	else {
	    fprintf(A2M_fdcnt, "%d ", A2M_RTF_TOCLEVEL + 1);
	    A2mRtfPrintCntTxt(A2M_RTF_TOCTXT);
	    fprintf(A2M_fdcnt, "\n");
	    fprintf(A2M_fdcnt, "%d", A2M_RTF_TOCLEVEL + 2);
	    A2mRtfPrintCntTxt(A2M_RTF_TOCTXT);
	    fprintf(A2M_fdcnt, "=T%04d\n", A2M_RTF_TOCNB);
	    A2M_RTF_TOCLEVEL++;
	    }
	}

    SCR_free(A2M_RTF_TOCTXT);
    A2M_RTF_TOCTXT = 0;
    if(atp) {
	A2M_RTF_TOCTXT   = SCR_stracpy(atp->atp_keyws);
	SCR_OemToAnsi(A2M_RTF_TOCTXT, A2M_RTF_TOCTXT);
	A2M_RTF_TOCPLEVEL = A2M_RTF_TOCLEVEL;
	A2M_RTF_TOCLEVEL = clevel;
	A2M_RTF_TOCNB    = atp->atp_nb;
	}
    return(0);
}

/*NH*/
A2mRtfPrintPar(ap, tbl)
A2MPAR  *ap;
int     tbl;
{
    int     i, curimg;

    if(ap == 0 || ap->ap_strs == 0) return(0);
    A2mRtfSetParProps(ap, tbl);

    curimg = A2M_RTF_CURFIG;
    for(i = 0 ; ap->ap_strs[i] ; i++) A2mRtfPrintStr(ap, i);
    A2M_RTF_CURFIG = curimg;
    for(i = 0 ; ap->ap_strs[i] ; i++) A2mRtfPrintStrImage(ap, i);

    if(A2M_RTF_HELP && A2M_RTF_TOPIC >= 0) {
	fprintf(A2M_fdrtf, "}{\\v T%04d}", A2M_RTF_TOPIC);
	A2M_RTF_TOPIC = -1;
	}

    if(tbl == 0) fprintf(A2M_fdrtf, "\\par\n");
    return(0);
}

/*NH JMP 30-08-11 */
int A2mRtfPrintPage(A2MPAGE* ap, int tbl)
{
    if(!A2M_RTF_HELP) {
	fprintf(A2M_fdrtf, "{\\page}\n");
	}

    return(0);
}

/*NH*/
A2mRtfSetParProps(ap, tbl)
A2MPAR  *ap;
{
    A2MPPR  *pp = &(ap->ap_ppr);
    int     parnb;

    A2mSetParProps(ap->ap_tag, pp);

    parnb = 1 + A2mFindPpr(ap->ap_tag);
    if(parnb <= 0) parnb = 1;
    fprintf(A2M_fdrtf, "\\pard\\plain\\s%d ", parnb);
    if(tbl) {
	fprintf(A2M_fdrtf, "\\intbl ");
	if(A2M_RTF_HELP) {
	    if(A2M_RTF_HLINES &&A2M_RTF_VLINES)
		fprintf(A2M_fdrtf, "\\brdrl\\brdrr\\brdrt\\brdrb ");
	    }
	else {
	    if(A2M_RTF_HLINES) fprintf(A2M_fdrtf, "\\clbrdrl\\clbrdrr ");
	    }
	}
    if(A2M_RTF_HELP == 0 && pp->pp_keepwn) fprintf(A2M_fdrtf, "\\keepn ");
    if(A2M_RTF_HELP == 0 && pp->pp_newpage) fprintf(A2M_fdrtf, "\\pagebb ");
    if(A2M_RTF_INTOPIC == 1) {
	fprintf(A2M_fdrtf, "\\keepn ");
	A2M_RTF_INTOPIC = 2;
	}

    if(pp->pp_html)    fprintf(A2M_fdrtf, "\\level%d ", pp->pp_html);
    if(pp->pp_just)    fprintf(A2M_fdrtf, "\\q%c ", "lcrrj"[pp->pp_just]);
    if(pp->pp_spaceb)  fprintf(A2M_fdrtf, "\\sb%d ", 20 * pp->pp_spaceb);
    if(pp->pp_spacea)  fprintf(A2M_fdrtf, "\\sa%d ", 20 * pp->pp_spacea);
    if(pp->pp_lmarg)   fprintf(A2M_fdrtf, "\\li%d ", 20 * pp->pp_lmarg);
    if(pp->pp_fnt.af_family == 'C') fprintf(A2M_fdrtf, "\\keep ");
    if(pp->pp_bullet)  {
	if(A2M_RTF_HELP)     fprintf(A2M_fdrtf, "\\fi-160\\tx%d \\{bmc onestep.bmp\\}\\tab ", 20 * pp->pp_lmarg);
	else if(A2M_NUMBERS) fprintf(A2M_fdrtf, "\\fi-160\\tx%d -\\tab ", 20 * pp->pp_lmarg);
	}

    return(0);
}

/*NH*/
A2mRtfPrintTxt(fd, txt)
FILE    *fd;
U_ch    *txt;
{
    int     i;

    if(txt == 0) return(0);
    for(i = 0 ; txt[i] ; i++) A2mRtfPrintChar(fd, txt[i]);
    return(0);
}

/*NH*/
A2mRtfPrintStr(ap, n)
A2MPAR  *ap;
int     n;
{
    A2MSTR  *as = ap->ap_strs[n];
    char    buf[65];

    if(A2M_NUMBERS && n == 0 && ap->ap_ppr.pp_number >= 1) {
	A2mCalcNumbering(ap->ap_ppr.pp_number - 1, buf);
	fprintf(A2M_fdrtf, "{");
	A2mRtfPrintAttr(ap, n);
	A2mRtfPrintTxt(A2M_fdrtf, buf);
	fprintf(A2M_fdrtf, "}");
	}

    switch(as->as_type) {
	case A2M_TEXT   :
		fprintf(A2M_fdrtf, "{");
		A2mRtfPrintAttr(ap, n);
		A2mRtfPrintTxt(A2M_fdrtf, as->as_txt);
		fprintf(A2M_fdrtf, "}");
		break;
	case A2M_IMAGEFR :
	case A2M_IMAGER :
		fprintf(A2M_fdrtf, "{");
		sprintf(buf, "Figure %d", A2M_RTF_CURFIG);
		A2mRtfPrintAttr(ap, n);
		A2mRtfPrintTxt(A2M_fdrtf, buf);
		fprintf(A2M_fdrtf, "}");
		break;
	case A2M_IMAGEF :
	case A2M_IMAGE  :
		A2M_RTF_CURFIG++;
		break;
	case A2M_NEWLINE: fprintf(A2M_fdrtf, "\\line "); break;
	case A2M_TAB    : fprintf(A2M_fdrtf, "\\tab "); break;
	case A2M_BHREF  : fprintf(A2M_fdrtf, "%s", as->as_txt); return(0);
	case A2M_TOPIC  :
			if(A2M_RTF_HELP) {
			    fprintf(A2M_fdrtf, "{\\uldb ");
			    A2M_RTF_TOPIC = atoi(as->as_txt);
			    }
			else
			    fprintf(A2M_fdrtf, "\\ul ");
			break;
	case A2M_ETOPIC :
			if(A2M_RTF_HELP) {
			    fprintf(A2M_fdrtf, "}{\\v T%04d}", A2M_RTF_TOPIC);
			    A2M_RTF_TOPIC = -1;
			    }
			else
			    fprintf(A2M_fdrtf, "\\ul0 ");
			break;
	}
    return(0);
}

/*NH*/
A2mRtfPrintStrImage(ap, n)
A2MPAR  *ap;
int     n;
{
    A2MSTR  *as = ap->ap_strs[n];

    switch(as->as_type) {
	case A2M_IMAGEFR :
	case A2M_IMAGER :
	case A2M_IMAGEF :
	case A2M_IMAGE  :
		if(as->as_type == A2M_IMAGER || as->as_type == A2M_IMAGEFR) /* JMP 17-04-00 */
		    fprintf(A2M_fdrtf, "{\\line {\\f1\\fs24\\b Figure %d}\\line} ", A2M_RTF_CURFIG);
		A2mRtfAFrame(as->as_txt, as->as_type);
		A2M_RTF_CURFIG++;
		break;
	}
    return(0);
}

/*NH*/
A2mRtfPrintAttr(ap, n)
A2MPAR  *ap;
int     n;
{
    A2MFNT  fnt;

    A2mSetFont(ap, ap->ap_strs[n], &fnt);

    fprintf(A2M_fdrtf, "\\fs%d", fnt.af_size * 2);
    fprintf(A2M_fdrtf, "\\cf%d", fnt.af_color);
    if(fnt.af_underline) fprintf(A2M_fdrtf, "\\ul");
    if(fnt.af_bold)      fprintf(A2M_fdrtf, "\\b");
    if(fnt.af_italic)    fprintf(A2M_fdrtf, "\\i");
    if(fnt.af_strike)    fprintf(A2M_fdrtf, "\\strike");
    if(fnt.af_family == 'C') fprintf(A2M_fdrtf, "\\f0");
    else if(fnt.af_family == 'T') fprintf(A2M_fdrtf, "\\f1");
    else                          fprintf(A2M_fdrtf, "\\f2");
    fprintf(A2M_fdrtf, " ");
    return(0);
}

/*NH*/
A2mRtfPrintChar(fd, ch)
FILE    *fd;
int     ch;
{

    if(A2M_RTF_OEMTOANSI) /* JMP 12-04-07 */
	ch = SCR_OemToAnsiChar(ch);

    if(ch == '\\' || ch == '{' || ch == '}' || ch >= 128 || ch < ' ')
	fprintf(fd, "\\'%02x", ch);
    else
	fprintf(fd, "%c", ch);

    return(0);
}

/*NH*/
A2mRtfPrintTbl(at)
A2MTBL  *at;
{
    if(at->at_title) A2mRtfPrintPar(at->at_title, 0);

    A2mRtfPrintTblLines(at, 0);
    A2mRtfPrintTblLines(at, 1);
    A2mRtfPrintTblLines(at, 2);

    if(at->at_footnote) A2mRtfPrintPar(at->at_footnote, 0);

    return(0);
}

/*NH*/
A2mRtfPrintTblLines(at, hbf)
A2MTBL  *at;
int     hbf;
{
    A2MTC   *tc;
    int     i, j, line = 0;

    for(i = 0 ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_hbf != hbf) continue;
	if(at->at_tls[i].atl_type) {
	    line = 1;
	    continue;
	    }
	fprintf(A2M_fdrtf, "\\trowd\\trgaph0");
	if(A2M_RTF_HELP) fprintf(A2M_fdrtf, "\\trqc");
	A2mRtfCellxs(at, i, hbf, line);
	if(line) fprintf(A2M_fdrtf, "\\trbrdrb ");
	for(j = 0 ; j < at->at_nc ; j++) {
	    tc = at->at_tls[i].atl_tcs + j;
	    A2mRtfPrintPar(tc->atc_par, 1);
	    if(line && A2M_RTF_HELP) fprintf(A2M_fdrtf, "\\brdrt");
	    if(tc->atc_center == 5)                           /* JMP 28-01-99 */
		tc->atc_center = tc->atc_par->ap_ppr.pp_just; /* JMP 28-01-99 */
	    fprintf(A2M_fdrtf, "\\q%c\\cell\n", "lcrrl"[tc->atc_center % 6]);
	    j += tc->atc_ncells - 1;
	    }
	fprintf(A2M_fdrtf, "\\row\n");
	line = 0;
	}

    return(0);
}

/*NH*/
A2mRtfCellxs(at, line, hbf, hline)
A2MTBL  *at;
int     line, hbf, hline;
{
    A2MTC   *tc;
    int     i, k, fl, ll;
    long    pos, nc;

    tc = at->at_tls[line].atl_tcs;
    nc = tc->atc_ncells;
    if(at->at_widths) {
	pos = 1440.0 * at->at_widths[0] / 2.54;
	for(k = 1 ; k < nc ; k++)
	    pos += (long) (1440.0 * at->at_widths[k] / 2.54);
	}
    else {
	pos = (14400L * (long)A2M_RTF_TCOL1) / 254L;
	pos += (long) (((nc - 1) * 14400L * (long)A2M_RTF_TCOLN) / 254L);
	}
    fl = A2mRtfCalcFirstLine(at, hbf);
    ll = A2mRtfCalcLastLine(at, hbf);

    if(A2M_RTF_HELP == 0) {
	fprintf(A2M_fdrtf, "\\brdrth\\clbrdrl");
	if(nc >= at->at_nc) fprintf(A2M_fdrtf, "\\clbrdrr");
	if(hline || fl == line)      fprintf(A2M_fdrtf, "\\clbrdrt");
	if(ll == line)      fprintf(A2M_fdrtf, "\\clbrdrb");
	}
    fprintf(A2M_fdrtf, "\\cellx%ld", pos);

    for(i = nc ; i < at->at_nc ; i++) {
	tc = at->at_tls[line].atl_tcs + i;
	nc = tc->atc_ncells;
	if(at->at_widths)
	    for(k = 0 ; k < nc ; k++)
		pos += (long) (1440.0 * at->at_widths[k + i] / 2.54);
	else
	    pos += (nc * 14400L * (long)A2M_RTF_TCOLN) / 254L;

	if(A2M_RTF_HELP == 0) {
	    if(i + nc >= at->at_nc) fprintf(A2M_fdrtf, "\\brdrth\\clbrdrr\\clbrdrl");  /* JMP 05-01-02 */
	    if(hline || fl == line)          fprintf(A2M_fdrtf, "\\clbrdrt");
	    if(ll == line)          fprintf(A2M_fdrtf, "\\clbrdrb");
	    }
	fprintf(A2M_fdrtf, "\\cellx%d", pos);
	i += nc - 1;
	}

    fprintf(A2M_fdrtf, "\n");
    return(0);
}

/*NH*/
A2mRtfCalcFirstLine(at, hbf)
A2MTBL  *at;
int     hbf;
{
    int     i;

    for(i = 0 ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_hbf != hbf) continue;
	if(at->at_tls[i].atl_type) continue;
	return(i);
	}
    return(-1);
}

/*NH*/
A2mRtfCalcLastLine(at, hbf)
A2MTBL  *at;
int     hbf;
{
    int     i, last = -1;

    for(i = 0 ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_hbf != hbf) continue;
	if(at->at_tls[i].atl_type) continue;
	last = i;
	}
    return(last);
}

/*N
A2mRtfPrintGrf(ao)
A2MOBJ  *ao;
{
#if defined(SCRW32) || defined (DOSW32)
    A2MGRF  *ag = (A2MGRF *)ao->ao_ptr;
    int     w, h, wgoal, hgoal;

    w = ag->ag_size[0] * 10;
    h = ag->ag_size[1] * 10;
    if(A2mGdiWMFInit("_scr4.wmf", w, h)) return(-1);

    wgoal = (ag->ag_size[0] * 1440.0) / 2.54;
    hgoal = (ag->ag_size[1] * 1440.0) / 2.54;
    w = ag->ag_size[0] * 1000.0;
    h = ag->ag_size[1] * 1000.0;

    A2mGdiPrintObj(ao);
    A2mGdiPrintObj((A2MOBJ *)0);
    A2mGdiWMFEnd();
//    fprintf(A2M_fdrtf, "\n{{\\pict\\wmetafile8\\picw%d\\pich%d\n", w, h);
    fprintf(A2M_fdrtf, "\n{{\\pict\\emfblip\\picw%d\\pich%d\\picwgoal%d\\pichgoal%d\n", w, h, wgoal, hgoal);
    SCR_dumphexfd("test.emf", A2M_fdrtf, 1);
    fprintf(A2M_fdrtf, "}\n\\par}\n");
//    unlink("_scr4.wmf");
#endif
    return(0);
}

*/

/*NH*/
A2mRtfAFrame(filename, type)
char    *filename;
int     type;
{
    char    buf[128];

    strcpy(buf, filename);
    if(A2M_RTF_GIFTOBMP && U_index(buf, ".gif") > 0)
	SCR_change_ext(buf, filename, "bmp");
    if(A2M_RTF_GIFTOPNG && U_index(buf, ".gif") > 0) /* JMP 23-08-2015 */
	SCR_change_ext(buf, filename, "png");        /* JMP 23-08-2015 */

    SCR_std_filename(buf);              /* JMP 25-05-99 */
    SCR_replace(buf, "\\", "\\\\");     /* JMP 25-05-99 */
    SCR_replace(buf, "\\", "\\\\");     /* JMP 25-05-99 */
    if(A2M_RTF_HELP == 0)
	fprintf(A2M_fdrtf, "{\\field{\\*\\fldinst {INCLUDEPICTURE \"%s\" \\\\* MERGEFORMAT \\\\d }}}\n", buf); /* JMP 23-10-97 */
    else
	fprintf(A2M_fdrtf, "{\\{bmc %s\\}}", buf); /* JMP 23-10-97 */

    return(0);
}

/*NH*/
A2mRtfHeader()
{
    fprintf(A2M_fdrtf, "{\\rtf1\\ansi\n");
    fprintf(A2M_fdrtf, "{\\info{\\comment Generated by SCR4_A2R (SCR4/AL1)}}\n");
    fprintf(A2M_fdrtf, "\\deff1\n");
    fprintf(A2M_fdrtf, "{\\fonttbl\n");
    fprintf(A2M_fdrtf, "{\\f0\\fmodern Courier New;}\n");
    fprintf(A2M_fdrtf, "{\\f1\\froman Times New Roman;}\n");
    fprintf(A2M_fdrtf, "{\\f2\\fswiss Arial;}\n");
    fprintf(A2M_fdrtf, "{\\f3\\fdecor Symbol;}\n");
    fprintf(A2M_fdrtf, "}\n");
    fprintf(A2M_fdrtf, "{\\colortbl ;\n");
    fprintf(A2M_fdrtf, "\\red255\\green255\\blue255;\n");
    fprintf(A2M_fdrtf, "\\red255\\green0\\blue0;\n");
    fprintf(A2M_fdrtf, "\\red0\\green127\\blue0;\n");
    fprintf(A2M_fdrtf, "\\red0\\green0\\blue255;\n");
    fprintf(A2M_fdrtf, "\\red0\\green255\\blue255;\n");
    fprintf(A2M_fdrtf, "\\red255\\green0\\blue255;\n");
    fprintf(A2M_fdrtf, "\\red255\\green255\\blue0;\n");
    fprintf(A2M_fdrtf, "}\n");
    return(0);
}

/*NH*/
A2mRtfStyles()
{
    A2MPPR  *pp;
    A2MFNT  *fnt;
    int     i, f;

    fprintf(A2M_fdrtf, "{\\stylesheet\n");
    fprintf(A2M_fdrtf, "{\\s244 \\f2\\fs16\\up6\\lang1036 \\sbasedon0\\snext0 footnote reference;}\n");
    fprintf(A2M_fdrtf, "{\\s245 \\f2\\fs20\\lang1036 \\sbasedon0\\snext245 footnote text;}\n");
    fprintf(A2M_fdrtf, "{\\s254\\sb240 \\b\\f2\\ul\\lang1036 \\sbasedon0\\snext0 heading 1;}\n");
    fprintf(A2M_fdrtf, "{\\f2\\fs20\\lang1036 \\snext0 Normal;}\n");

    for(i = 0; i < A2M_NB_PPRS ; i++) {
	pp = A2M_PPRS + i;
	fnt = &(pp->pp_fnt);
	fprintf(A2M_fdrtf, "{\\s%d", i + 1);
	switch(fnt->af_family) {
	    case 'A' : f = 2; break;
	    case 'H' : f = 2; break;
	    case 'T' : f = 1; break;
	    default  : f = 0; break;
	    }
	fprintf(A2M_fdrtf, "\\f%d\\fs%d", f, 2 * fnt->af_size);
	fprintf(A2M_fdrtf, "\\li%d\\sb%d\\sa%d\\sbasedon0",
		pp->pp_lmarg * 20, pp->pp_spaceb * 20, pp->pp_spacea * 20);
	if(pp->pp_bullet)  fprintf(A2M_fdrtf, "\\fi-160\\tx%d", 20 * pp->pp_lmarg);
	if(pp->pp_html)    fprintf(A2M_fdrtf, "\\level%d ", pp->pp_html);
	if(pp->pp_just)    fprintf(A2M_fdrtf, "\\q%c ", "lcrrj"[pp->pp_just]);
	if(pp->pp_keepwn)  fprintf(A2M_fdrtf, "\\keepn ");
	if(pp->pp_newpage) fprintf(A2M_fdrtf, "\\pagebb ");

	if(fnt->af_color) fprintf(A2M_fdrtf, "\\cf%d", fnt->af_color);
	if(fnt->af_bold)  fprintf(A2M_fdrtf, "\\b ");
	if(fnt->af_underline) fprintf(A2M_fdrtf, "\\ul ");
	if(fnt->af_italic) fprintf(A2M_fdrtf, "\\i ");

	fprintf(A2M_fdrtf, "\\snext%d %s;}\n", i, pp->pp_tag);
	}
    fprintf(A2M_fdrtf, "}\n");
    return(0);
}

/* ================================================================
Fixe la valeur des variables globales avant l'interp�tation d'un fichier
a2m et la g�n�ration d'un fichier rtf en se basant sur les d�finitions du
fichier .ini associ�.

Les sections [A2M] et [RTF] du fichier .ini sont interpr�t�es.

&EN filename = nom du fichier .ini

La syntaxe des fichiers a2m est d�crite dans un chapitre sp�cifique.

&TI Fichier a2m.ini
���������������
Le fichier a2m.ini (ou un autre) contient des param�tres pour
la lecture du fichier a2m etla g�nration du fichier rtf. Pour que ces
param�tres soient pris en compte par le programme A2mToRtf(), il faut
appeler la fonction A2mRtfReadIni(filename) avant de lancer la fonction
d'interpr�tation et d'impression.

&CO
    #include <s_a2m.h>

    A2mRtfReadIni("myfile.ini");
    A2mToRtf("myfile.a2m", "myfile.rtf", 0);
&TX

&SA A2mToGdi(), A2mToRtf(), A2mToMif(), A2mToHtml(), A2mPrintError()
==================================================================== */

A2mRtfReadIni(filename)
char    *filename;
{
    U_ch    buf[256];

    A2mReadA2mIni(filename);
    if(IniReadTxtParm(filename, "RTF", "TITLE", buf, 255) == 0) {
	A2M_RTF_TITLE = SCR_stracpy(buf);
	SCR_OemToAnsi(A2M_RTF_TITLE, A2M_RTF_TITLE);
	}
    if(IniReadTxtParm(filename, "RTF", "COPYRIGHT", buf, 255) == 0) {
	A2M_RTF_COPYRIGHT = SCR_stracpy(buf);
	SCR_OemToAnsi(A2M_RTF_COPYRIGHT, A2M_RTF_COPYRIGHT);
	}

    if(IniReadTxtParm(filename, "RTF", "FONTFAMILY", buf, 255) == 0)
	A2M_FONTFAMILY = buf[0];

    IniReadNumParm(filename, "RTF", "FONTSIZE",       &A2M_FONTSIZE);
    IniReadNumParm(filename, "RTF", "FONTINCR",       &A2M_FONTINCR);
    IniReadNumParm(filename, "RTF", "TABLEFONTSIZE",  &A2M_TFONTSIZE);
    IniReadNumParm(filename, "RTF", "NUMBERING",      &A2M_NUMBERS);
    IniReadNumParm(filename, "RTF", "TABLECOLOR",     &A2M_RTF_TCOLOR);
    IniReadNumParm(filename, "RTF", "TABLECOL1",      &A2M_RTF_TCOL1);
    IniReadNumParm(filename, "RTF", "TABLECOLN",      &A2M_RTF_TCOLN);
    IniReadNumParm(filename, "RTF", "TABLESPLIT",     &A2M_RTF_TSPLIT);
    IniReadNumParm(filename, "RTF", "TABLEWIDTH",     &A2M_RTF_TWIDTH);
    IniReadNumParm(filename, "RTF", "TABLEBORDER",    &A2M_RTF_TBORDER);
    IniReadNumParm(filename, "RTF", "TABLEHLINES",    &A2M_RTF_HLINES);
    IniReadNumParm(filename, "RTF", "TABLEVLINES",    &A2M_RTF_VLINES);
    IniReadNumParm(filename, "RTF", "TOC",            &A2M_RTF_TOC);
    IniReadNumParm(filename, "RTF", "COMPRESS",       &A2M_RTF_COMPRESS);
    return(0);
}
