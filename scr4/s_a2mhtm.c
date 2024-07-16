#include "s_a2m.h"
#include "s_ini.h"

/********************* Parameters *************************/

/* Public */
int     A2M_HTML_TCOLOR = 0;  /* JMP 12-02-99 */
int     A2M_HTML_TBORDER = 1, A2M_HTML_TOC = 0;
char    *A2M_HTML_BODY = 0;
int     A2M_HTML_IS_GIF = 1;
/* = "TEXT=\"#000000\" BGCOLOR=\"#FFFFC0\" LINK=\"#0000FF\" ALINK=\"#FF00FF\" VLINK=\"#800080\"";
*/
U_ch    *A2M_HTML_TITLE = 0;
int     A2M_HTML_GIFTOBMP = 0; /* JMP 16-12-97 */
int     A2M_HTML_GIFTOPNG = 0; /* JMP 23-08-2015 */
char    *A2M_HTML_STYLE = 0; /* JMP 09-05-99 */
char    *A2M_HTML_RELSTYLE = 0; /* JMP 29-01-2004 */
char    *A2M_HTML_TARGET = 0; /* JMP 28-02-2004 */
int     A2M_HTML_AUTOLINK = 1; /* JMP 20-10-01 */

int     A2M_HTML_IMGWIDTH = 0;    /* JMP 29-06-02 */
int     A2M_HTML_IMGBORDER = -1;  /* JMP 29-06-02 */
int     A2M_HTML_OEMTOANSI = 1;     /* JMP 16-08-10 */
int     A2M_HTML_NOINLINESTYLE = 0;    /* JMP 25-09-10 */

char    *A2M_HTML_TABLECLASS = 0; //  0 = D�faut class, "" = no class, "classname" = user class
char    *A2M_HTML_TRCLASS = 0; //  0 = D�faut class, "" = no class, "classname" = user class
char    *A2M_HTML_THCLASS = 0; //  0 = D�faut class, "" = no class, "classname" = user class
char    *A2M_HTML_TDCLASS = 0; //  0 = D�faut class, "" = no class, "classname" = user class

/* Private */
FILE    *A2M_fdhtm, *A2M_fdhhc, *A2M_fdhhk, *A2M_fdtoc;
A2MFNT  A2M_CURFONT;
int     A2M_LEVEL = 0, A2M_HTMLREF = 0;
int     A2M_GIF_CNT = 0;
int     A2M_TABS[10], A2M_NB_TABS;
int     A2M_HTML_LTGT = 0;
int     A2M_HTML_STRIP = 0;
int     A2M_HTML_TTITLE= 1; /* JMP 12-02-99 */

/* Private */
int     A2M_HTML_TOPIC = -1, A2M_HTML_HELP = 0, A2M_HTML_LEVEL = 0;
int     A2M_HTML_TOCLEVEL, A2M_HTML_TOCPLEVEL, A2M_HTML_TOCNB;
U_ch    *A2M_HTML_TOCTXT;
int     A2M_HTML_INTOPIC = 0, A2M_HTML_GREEK = 0;



/* ================================================================
Interpr�te le contenu d'un fichier a2m et g�n�re un fichier .htm.

&EN a2mfile = nom du fichier a2m � interpr�ter
&EN outfile = nom du fichier htm � g�n�rer
&EN wh = 0 pour un fichier HTML simple, 1 pour un fichier pour
    Html Help

&RT La fonction retourne 0 si le processus s'est bien d�roul�, -1 sinon.

La syntaxe des fichiers a2m est d�crite dans un chapitre sp�cifique.

&TI Fichier a2m.ini
���������������
Le fichier a2m.ini (ou un autre) contient des param�tres pour
l'impression et la lecture du fichier a2m. Pour que ces param�tres
soient pris en compte par le programme A2mToHtml(), il faut appeler la
fonction A2mHtmlReadIni(filename) avant de lancer la fonction
d'interpr�tation et d'impression.

&CO
    #include <s_a2m.h>

    A2mHtmlReadIni("myfile.ini");
    A2mToHtml("myfile", "test.htm", 0);
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
Ces variables sont d�finies dans la section [HTML] du fichier .ini.

&EN int A2M_FONTSIZE = taille par d�faut des caract�res dans les
    paragraphes (10 pts par d�faut)
&EN int A2M_FONTINCR = incr�ment de taille de caract�res (2 par d�faut)
&EN int A2M_TFONTSIZE = taille par d�faut des caract�res dans les
    tableaux (8 pts par d�faut)
&EN int A2M_FONTFAMILY = police de caract�re par d�faut ('H', 'T' ou 'C')
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

&EN int A2M_HTML_TCOLOR = 1 (dft) pour impression en couleur des
    tableaux, 0 en B/W
&EN int A2M_HTML_TBORDER = �paisseur des cadres des tableaux (1 par dft)
&EN U_ch *A2M_HTML_TITLE = titre du fichier html
&EN U_ch *A2M_HTML_BODY = contenu du tag <BODY> du fichier HTML
&EN int A2M_HTML_LTGT = 0 si les caract�res < et > doivent appara�tre
    comme tel en HTML (&lt;) et non interpr�t�s comme tag (on mettra 0
    dans ce cas).
&EN int A2M_HTML_TTITLE = 1 pour inclure le titre des tableaux dans le tableau
&EN int A2M_HTML_AUTOLINK = 0 pour supprimer la cr�ation automatique de liens
    par l'analyse des sujets d'aide (acc�l�re le traitement)

&RT
&EN 0 en cas de succ�s
&EN -1 si le fichier ne peut �tre ouvert
&EN -2 si les fichiers output n'ont pu �tre cr��s


&SA A2mHtmlReadIni(), A2mToRtf(), A2mToMif(), A2mToMif(), A2mPrintError()
==================================================================== */

A2mToHtml(a2mfile, outfile, wh)
char    *a2mfile, *outfile;
int     wh;
{
    return(A2mToHtmlStrip(a2mfile, outfile, A2M_HTML_STRIP, wh));
}

/*
    Fonction identique � A2mToHtml() mais sans inclure de sections
    HEAD et BODY dans le fichier r�sultat.
*/

int A2mToHtmlStrip(char* a2mfile, char* outfile, int strp, int wh)
{
    A2MFILE *af;
    A2MOBJ  *ao;
    int     ostrp = A2M_HTML_STRIP;

    A2mReadTopics(a2mfile);  /* JMP 17-05-99 */
    af = A2mOpen(a2mfile, 0);
    if(af == 0) {
        A2mFreeTopics();  /* JMP 24-05-99 */
        return(-1);
    }

    A2M_HTML_STRIP = strp;
    if(A2mHtmlInit(outfile, wh)) {
        A2mClose(af);
        A2M_HTML_STRIP = ostrp;
        A2mFreeTopics();  /* JMP 24-05-99 */
        return(-2);
    }

    while(1) {
        ao = A2mRead(af);
        if(ao == 0) break;
        A2mHtmlPrintObj(ao, 1);
    }

    A2mHtmlEnd();
    A2mClose(af);
    A2mFreeTopics();  /* JMP 24-05-99 */
    A2M_HTML_STRIP = ostrp;
    return(0);
}
/*NH*/
int A2mHtmlPre(fd, base)
FILE    *fd;
char    *base;
{
    //int     i;
    extern  int A2M_SWF_NOGIF;
    extern U_ch    SWF_JSPATH[256],
           SWF_JSEXPORTPATH[256];
    extern int     SWF_EXPORTENABLED;


    fprintf(fd, "<HTML>\n<HEAD>\n");
    fprintf(fd, "<TITLE>");
    if(A2M_HTML_TOCTXT) fprintf(fd, "%s", A2M_HTML_TOCTXT);
//   else if(A2M_HTML_TITLE) fprintf(fd, "%s", A2M_HTML_TITLE);
//       for(i = 0 ; A2M_HTML_TITLE[i] ; i++)
//       A2mHtmlPrintChar(fd, A2M_HTML_TITLE[i]);
    fprintf(fd, "\n</TITLE>\n");

    if(A2M_HTML_STYLE) {
        // fprintf(fd, "<STYLE>\n<!--\n"); /* JMP 21-10-01 */
        fprintf(fd, "<!STARTSTYLE>\n");  /* JMP 14-03-11 */
        SCR_dumpf_file(fd, A2M_HTML_STYLE);
        fprintf(fd, "<!ENDSTYLE>\n");  /* JMP 14-03-11 */
        // fprintf(fd, "\n-->\n</STYLE>\n"); /* JMP 21-10-01 */
    }

    if(A2M_HTML_RELSTYLE) { /* JMP 29-01-2004 */
        fprintf(fd, "<!STARTSTYLE>\n");  /* JMP 14-03-11 */
        fprintf(fd, "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\">\n", A2M_HTML_RELSTYLE); /* JMP 29-01-2004 */
        fprintf(fd, "<!ENDSTYLE>\n");  /* JMP 14-03-11 */
    }
    if(base) /* JMP 01-02-2004 */
        fprintf(fd, "<base target=\"%s\">\n", base);

    if(A2M_SWF_NOGIF) {
        A2mSWFIni();
        fprintf(fd, "<script language=\"JavaScript\" src=\"%s\"></script>", SWF_JSPATH);
        if(SWF_JSEXPORTPATH[0] != 0) {
            fprintf(fd, "<script language=\"JavaScript\" src=\"%s\"></script>", SWF_JSEXPORTPATH);
            SWF_EXPORTENABLED = 1;
        }
        else SWF_EXPORTENABLED = 0;
    }

    fprintf(fd, "\n<META NAME=\"GENERATOR\" CONTENT=\"SCR4_A2H\">\n");
    fprintf(fd, "</HEAD>\n");
    if(A2M_HTML_BODY)
        fprintf(fd, "<BODY %s>\n", A2M_HTML_BODY);
    else {
        if(A2M_HTML_STYLE == 0 && A2M_HTML_RELSTYLE == 0)
            fprintf(fd, "<BODY TEXT=\"#000000\" BGCOLOR=\"#FFFFC0\" LINK=\"#0000FF\" ALINK=\"#FF00FF\" VLINK=\"#800080\">\n"); /* JMP 27-04-99 */
        else
            fprintf(fd, "<BODY>\n");
    }

    fprintf(fd, "<!STARTHEADER>\n");
    fprintf(fd, "<!ENDHEADER>\n");
    return(0);
}

/*NH*/
int A2mHtmlPost(FILE* fd)
{
//    A2mHtmlPrintFNotes();
//    A2mFreeFNotes();
    fprintf(fd, "<!STARTFOOTER>\n");
    fprintf(fd, "<!ENDFOOTER>\n");
    fprintf(fd, "</BODY>\n");
    fprintf(fd, "</HTML>\n");
    return(0);
}

/*NH*/
A2mHtmlInit(outfile, wh)
char    *outfile;
int     wh;
{
    char        buf[256];
    extern int  A2M_CUROBJ;

    A2M_HTML_HELP = wh;
    A2M_CUROBJ = 1;
    A2M_GIF_CNT = 0;
    A2M_LEVEL = A2M_HTMLREF = 0;

    A2M_HTML_TOCPLEVEL = A2M_HTML_TOCLEVEL = A2M_HTML_TOCNB = -1;
    A2M_HTML_TOCTXT = 0;
//    A2M_HTML_CURFIG = 1;
    A2M_HTML_INTOPIC = 0;

    A2M_fdhtm = fopen(outfile, "w+");
    if(A2M_fdhtm == 0) return(-2);
    if(A2M_HTML_TITLE && A2M_HTML_OEMTOANSI) SCR_OemToAnsi(A2M_HTML_TITLE, A2M_HTML_TITLE); /* JMP 16-08-10 */
    if(A2M_HTML_STRIP == 0) A2mHtmlPre(A2M_fdhtm, A2M_HTML_TARGET);
    A2mFreeFNotes(); /* JMP 05-09-99 */

    if(A2M_HTML_HELP == 0) return(0);

    SCR_change_ext(buf, outfile, "hhc");
    A2M_fdhhc = fopen(buf, "w+");
    if(A2M_fdhhc == 0) {
        fclose(A2M_fdhtm);
        return(-2);
    }
    A2mHtmlPre(A2M_fdhhc, 0);
    fprintf(A2M_fdhhc, "<UL>\n");

    SCR_change_ext(buf, outfile, "hhk");
    A2M_fdhhk = fopen(buf, "w+");
    if(A2M_fdhhk == 0) {
        fclose(A2M_fdhtm);
        fclose(A2M_fdhhc);
        return(-2);
    }
    A2mHtmlPre(A2M_fdhhk, 0);
    fprintf(A2M_fdhhk, "<UL>\n");

    A2mHtmlHhp(outfile);


    /* JMP 25-01-2004 : TOC */
    SCR_change_ext(buf, outfile, "toc.htm");
    A2M_fdtoc = fopen(buf, "w+");
    if(A2M_fdtoc == 0) {
        fclose(A2M_fdhtm);
        fclose(A2M_fdhhc);
        fclose(A2M_fdhhk);
        return(-2);
    }
    A2mHtmlPre(A2M_fdtoc, "contents");
    fprintf(A2M_fdtoc, "<UL>\n");

    return(0);
}

/*NH*/
int A2mHtmlHhp(char* outfile)
{
    FILE    *fd;
    char    buf[256], cnt[256], key[256];

    SCR_change_ext(buf, outfile, "hhp");
    fd = fopen(buf, "w+");
    if(fd == 0) return(0);

    fprintf(fd, "[OPTIONS]\n");
    fprintf(fd, "Compatibility=1.1 or later\n");

    SCR_change_ext(buf, outfile, "chm");
    fprintf(fd, "Compiled file=%s\n", buf);

    SCR_change_ext(cnt, outfile, "hhc");
    fprintf(fd, "Contents file=%s\n", cnt);

    fprintf(fd, "Default Window=xxx\n");
    fprintf(fd, "Default topic=t0000.htm\n");
    fprintf(fd, "Display compile progress=No\n");
    fprintf(fd, "Full-text search=Yes\n");

    SCR_change_ext(key, outfile, "hhk");
    fprintf(fd, "Index file=%s\n", key);
    fprintf(fd, "Language=0x80c French (Belgian)\n");
    if(A2M_HTML_TITLE) fprintf(fd, "Title=%s\n", A2M_HTML_TITLE);

    fprintf(fd, "[WINDOWS]\n");
    fprintf(fd, "xxx=,\"%s\",\"%s\",\"t0000.htm\",,,,,,0x63521,,0x380e,,0x80000,,,,,,0\n",
            cnt, key);

    fprintf(fd, "[INFOTYPES]\n");
    fprintf(fd, "[FILES]\n");
    fprintf(fd, "listing.gif\n");
    fprintf(fd, "title.gif\n");
    fprintf(fd, "style.css\n");
    fclose(fd);
    return(0);
}

/*NH*/
int A2mHtmlEnd()
{
    int     i;

    for(i = 0 ; i < A2M_LEVEL ; i++) fprintf(A2M_fdhtm, "</UL>\n");
    A2M_LEVEL = 0;
    A2mHtmlPrintFNotes();
    A2mFreeFNotes();
    if(A2M_HTML_STRIP == 0) A2mHtmlPost(A2M_fdhtm);
    fclose(A2M_fdhtm);
    A2M_fdhtm = 0;

    if(A2M_HTML_HELP) {
        A2mHtmlPrintTopicCnt((A2MTOP *)0);
        A2mHtmlPost(A2M_fdhhc);
        fclose(A2M_fdhhc);
        A2M_fdhhc = 0;

        fprintf(A2M_fdhhk, "</UL>\n");
        A2mHtmlPost(A2M_fdhhk);
        fclose(A2M_fdhhk);
        A2M_fdhhk = 0;

        fprintf(A2M_fdtoc, "</UL>\n");
        A2mHtmlPost(A2M_fdtoc);
        fclose(A2M_fdtoc);
        A2M_fdtoc = 0;
    }

    if(A2M_HTML_TITLE && A2M_HTML_OEMTOANSI) SCR_AnsiToOem(A2M_HTML_TITLE, A2M_HTML_TITLE); /* JMP 30-04-99 */
    return(0);
}

/*NH*/
A2mHtmlPrintObj(ao, del)
A2MOBJ  *ao;
int     del;
{
    char        msg[80];
    extern int  A2M_CUROBJ;

    sprintf(msg, "Printing Object %d", A2M_CUROBJ++);
    A2mMessage(msg);
    if(ao->ao_type == A2M_PAR)
        A2mHtmlPrintPar((A2MPAR *)ao->ao_ptr, 0, 0);

    if(ao->ao_type == A2M_TOPIC)
        A2mHtmlPrintTopic((A2MTOP *)ao->ao_ptr, 0);

    if(ao->ao_type == A2M_TOPICALIAS) /* JMP 09-09-99 */
        A2mHtmlPrintTopic((A2MTOP *)ao->ao_ptr, 1);

    if(ao->ao_type == A2M_TBL)
        A2mHtmlPrintTbl((A2MTBL *)ao->ao_ptr);

    if(ao->ao_type == A2M_GRF)
        A2mHtmlPrintGrf((A2MGRF *)ao->ao_ptr);

    if(del) A2mFreeObj(ao);
    return(0);
}

/*NH*/
int A2mHtmlPrintTopic(A2MTOP* atp, int alias)
{
    char    buf[256];
    int     i;

    if(A2M_HTML_HELP == 0 || atp == 0) return(0);

    if(alias == 0) A2mHtmlPrintTopicCnt(atp);
    A2mHtmlPrintTopicKey(atp);

    if(A2M_fdhtm != NULL) {
        for(i = 0 ; i < A2M_LEVEL ; i++) fprintf(A2M_fdhtm, "</UL>\n");
        A2M_LEVEL = 0;
        A2mHtmlPrintFNotes();
        A2mFreeFNotes();
        A2mHtmlPost(A2M_fdhtm);
        fclose(A2M_fdhtm);
        A2M_fdhtm = NULL;
    }

    sprintf(buf, "T%04d.htm", atp->atp_nb);
    A2M_fdhtm = fopen(buf, "w+");
    A2mHtmlPre(A2M_fdhtm, 0);


    return(0);
}

/*NH*/
A2mHtmlCalcBlc(ap)
A2MPAR  *ap;
{
    int     i;
    U_ch    *txt;

    if(ap->ap_strs == 0 || ap->ap_strs[0] == 0) return(0);

    txt = ap->ap_strs[0]->as_txt;
    if(txt == 0) return(0);
    for(i = 0 ; txt[i] == ' ' ; i++);
    if(txt[i] == 0) return(0);
    /*!!!*/
    return(i); /* SUPPR JMP 07-04-08 */

    U_ljust_text(ap->ap_strs[0]->as_txt);
    SCR_strip(ap->ap_strs[0]->as_txt);
    return(i);
}

/*NH*/
A2mHtmlOpenTab(blc)
int     blc;
{
    int     i;

    if(blc <= 0) return(0);
    for(i = 0 ; i < A2M_NB_TABS ; i++) {
        if(blc <= A2M_TABS[i]) break;
    }

    if(i == A2M_NB_TABS && A2M_NB_TABS <= 10) {
        A2M_TABS[i] = blc;
        A2M_NB_TABS++;
    }

    /*    for( ; i >= 0 ; i--) fprintf(A2M_fdhtm, "<UL>"); /* JMP 14-12-98 */
    for( ; i >= 0 ; i--) fprintf(A2M_fdhtm, "&nbsp;"); /* JMP 14-12-98 */

    return(0);
}

/*NH
A2mHtmlCloseTab(blc)
int     blc;
{
    int     i;

    if(blc <= 0) return;
    for(i = 0 ; i < A2M_NB_TABS ; i++)
	if(A2M_TABS[i] == blc) break;

    if(i >= A2M_NB_TABS) return;

    for( ; i >= 0 ; i--) fprintf(A2M_fdhtm, "</UL>");
}
*/

/*NH*/
A2mHtmlPrintPar(ap, tbl, tcol)
A2MPAR  *ap;
int     tbl, tcol;
{
    int     i, level, blc;
    char    tag[32], *just;
    A2MFNT  fntd;

    if(ap == 0 || ap->ap_strs == 0) {
        if(tbl) fprintf(A2M_fdhtm, "&nbsp;"); /* JMP 31-12-98 */
        return(0);
    }
    A2mSetParProps(ap->ap_tag, &(ap->ap_ppr));
    A2mHtmlFindTag(&(ap->ap_ppr), tag);

    level = ap->ap_ppr.pp_lmarg - 16;
    if(level < 0) level = 0;
    else level = level / 8;

    if(level > 6) level = 6; /* JMP 11-09-99 */

    if(level < A2M_LEVEL)
        for(i = level ; i < A2M_LEVEL ; i++) fprintf(A2M_fdhtm, "</UL>\n");
    else if(level > A2M_LEVEL)
        for(i = A2M_LEVEL ; i < level ; i++) fprintf(A2M_fdhtm, "<UL>\n");

    A2M_LEVEL = level;

    /* if(tbl && tcol == 0) {
    blc = A2mHtmlCalcBlc(ap);
    if(blc) A2mHtmlOpenTab(blc);
    }
    GB 29/04/2004 */
    switch(ap->ap_ppr.pp_just) {
    case 1 :
        just = "CENTER";
        break;
    case 2 :
    case 3 :
        just = "RIGHT";
        break;    /* JMP 12-02-99 */
    case 4 :
        just = "JUSTIFY";
        break;  /* JMP 12-02-99 */
    default :
        just = 0;
        break;
    }

    if(tbl == 0 || strcmp(tag, "P") || ap->ap_tag) {
        if(tag[0] == 'H') fprintf(A2M_fdhtm, "<A NAME=\"REF%d\"></A>", ++A2M_HTMLREF); /* JMP 30-04-00 */
        fprintf(A2M_fdhtm, "<%s", tag);
        if(just && A2M_HTML_STYLE == 0 && A2M_HTML_RELSTYLE == 0) /* JMP 29-01-2004 */
            fprintf(A2M_fdhtm, " ALIGN=%s", just);
        if(ap->ap_tag) fprintf(A2M_fdhtm, " class=\"%s\"", ap->ap_tag);/* JMP 25-01-2004 */
        if(tcol == 0) {
            blc = A2mHtmlCalcBlc(ap);
            if(blc && A2M_HTML_NOINLINESTYLE == 0)
                fprintf(A2M_fdhtm, " STYLE=\"margin-left:%dpt\" ", blc*2); /* JMP 25-09-10 */
        }
        fprintf(A2M_fdhtm, ">"); /* JMP 17-05-99 */
        /* if(tag[0] == 'H') fprintf(A2M_fdhtm, "</A>"); /* JMP 30-04-00 */
    }

//    if(tbl == 0 && just) fprintf(A2M_fdhtm, "<%s>", just); /* JMP 12-02-99 */

    A2mHtmlCalcTagFnt(tag, &fntd);
    memset(&A2M_CURFONT, 0, sizeof(A2MFNT));

    for(i = 0 ; ap->ap_strs[i] ; i++) A2mHtmlPrintStr(ap, i, &fntd, tbl);

//    if(tbl && (i == 0 || (i == 1 && ap->ap_strs[0]->as_txt[0] == 0)))
//        fprintf(A2M_fdhtm, "&nbsp;"); /* JMP 31-12-98 */
    A2mHtmlResetFont();

//    if(tbl == 0 && just) fprintf(A2M_fdhtm, "</%s>", just); /* JMP 12-02-99 */
    if(tbl == 0 || strcmp(tag, "P")) fprintf(A2M_fdhtm, "</%s>\n", tag);
    /*    if(tbl && tcol == 0 && blc) A2mHtmlCloseTab(blc); */

    return(0);
}

/*NH*/
A2mHtmlFindTag(pp, tag)
A2MPPR  *pp;
char    *tag;
{
    A2M_HTML_GREEK = 0;
    if(pp->pp_html)                      sprintf(tag, "H%d", pp->pp_html);
    else if(pp->pp_bullet)               strcpy(tag, "LI");
    else if(pp->pp_fnt.af_family == 'C') strcpy(tag, "PRE");
    else if(pp->pp_fnt.af_family == 'G') A2M_HTML_GREEK = 1;
    else strcpy(tag, "P");

    return(0);
}


/*NH*/
A2mHtmlCalcTagFnt(tag, fnt)
char    *tag;
A2MFNT  *fnt;
{
    memset(fnt, 0, sizeof(A2MFNT));

    fnt->af_size = 12;
    fnt->af_family = 'T';

    if(tag[0] == 'H') {
        fnt->af_size = 20 - 2 * atoi(tag + 1);
        fnt->af_bold = 1;
    }

    if(strcmp(tag, "PRE") == 0) {
        fnt->af_size = 10;
        fnt->af_family = 'C';
    }

    return(0);
}

/*NH*/
A2mHtmlPrintStr(ap, n, fntt, tbl)
A2MPAR  *ap;
int     n;
A2MFNT  *fntt;
int     tbl;
{
    A2MSTR  *as = ap->ap_strs[n];
    int     i;
    char    buf[65];

    A2mHtmlPrintAttr(ap, n, fntt);
    if(A2M_NUMBERS && n == 0 && ap->ap_ppr.pp_number >= 1) {
        A2mCalcNumbering(ap->ap_ppr.pp_number - 1, buf);
        for(i = 0 ; buf[i] ; i++) A2mHtmlPrintChar(A2M_fdhtm, buf[i]);
    }

    switch(as->as_type) {
    case A2M_TEXT :
        for(i = 0 ; as->as_txt[i] ; i++)
            A2mHtmlPrintChar(A2M_fdhtm, as->as_txt[i]);
        return(0);

    case A2M_IMAGEFR:
    case A2M_IMAGER:
    case A2M_IMAGEF:
    case A2M_IMAGE :
        A2mHtmlAFrame(as->as_txt);
        return(0);
    case A2M_ANCHOR :
        A2mHtmlAnchor(as->as_txt);
        return(0);
    case A2M_FNOTE :
        A2mHtmlFNote(as->as_txt);
        return(0);
    case A2M_BHREF:
        fprintf(A2M_fdhtm, "<A HREF=\"%s\">", as->as_txt);
        return(0);
    case A2M_EHREF:
        fprintf(A2M_fdhtm, "</A>");
        return(0);
    case A2M_NEWLINE : //if(tbl == 0) fprintf(A2M_fdhtm, "<BR>");
        //else fprintf(A2M_fdhtm, "</P><P>");
        fprintf(A2M_fdhtm, "<BR>");
        return(0);

    case A2M_TAB :
        fprintf(A2M_fdhtm, " ");
        return(0);
    case A2M_TOPIC  :                              /* JMP 29-04-99 */
        if(A2M_HTML_HELP) {
            A2M_HTML_TOPIC = atoi(as->as_txt);
            fprintf(A2M_fdhtm, "<A HREF=\"T%04d.htm\">", A2M_HTML_TOPIC);
        }
        else
            fprintf(A2M_fdhtm, "");
        break;
    case A2M_ETOPIC :                              /* JMP 29-04-99 */
        if(A2M_HTML_HELP) {
            fprintf(A2M_fdhtm, "</A>");
            A2M_HTML_TOPIC = -1;
        }
        else
            fprintf(A2M_fdhtm, "");
        break;
    }

    return(0);
}

/*NH*/
A2mHtmlResetFont()
{
    int     i;


    if(A2M_CURFONT.af_pos == 1) fprintf(A2M_fdhtm, "</SUP>");
    if(A2M_CURFONT.af_pos == -1) fprintf(A2M_fdhtm, "</SUB>");
    if(A2M_CURFONT.af_italic)   fprintf(A2M_fdhtm, "</I>");
    if(A2M_CURFONT.af_bold)     fprintf(A2M_fdhtm, "</B>");
    if(A2M_CURFONT.af_underline)fprintf(A2M_fdhtm, "</U>");
    if(A2M_CURFONT.af_strike)   fprintf(A2M_fdhtm, "</STRIKE>");
    if(A2M_CURFONT.af_family)   fprintf(A2M_fdhtm, "</TT>");

    for(i = 0; i < A2M_CURFONT.af_color + A2M_CURFONT.af_size ; i++)
        fprintf(A2M_fdhtm, "</FONT>");

    return(0);
}
char    *A2M_HTMLCOLORS[] = {
    "000000",
    "FFFFFF",
    "FF0000",
    "008000",
    "0000FF",
    "00FFFF",
    "FF00FF",
    "FFFF00"
};

/*NH*/
A2mHtmlPrintAttr(ap, n, fntt)
A2MPAR  *ap;
int     n;
A2MFNT  *fntt;
{
    A2MFNT  fnt, fntp;

    A2mSetFont(ap, ap->ap_strs[n], &fnt);

    if(n) A2mSetFont(ap, ap->ap_strs[n - 1], &fntp);
    else memcpy(&fntp, fntt, sizeof(A2MFNT));

    /* RESET */
    if(fnt.af_family != 'C' && fntp.af_family == 'C')
    {
        fprintf(A2M_fdhtm, "</TT>");
        A2M_CURFONT.af_family = 0;
    }

    if(fnt.af_family != 'G' && fntp.af_family == 'G')
    {
        A2M_HTML_GREEK = 0;
    }

    if(!fnt.af_italic && fntp.af_italic)
    {
        fprintf(A2M_fdhtm, "</I>");
        A2M_CURFONT.af_italic = 0;
    }

    if(fnt.af_pos == 0 && fntp.af_pos == -1)
    {
        fprintf(A2M_fdhtm, "</SUB>");
        A2M_CURFONT.af_pos = 0;
    }

    if(fnt.af_pos == 0 && fntp.af_pos == 1)
    {
        fprintf(A2M_fdhtm, "</SUP>");
        A2M_CURFONT.af_pos = 0;
    }

    if(!fnt.af_bold && fntp.af_bold)
    {
        fprintf(A2M_fdhtm, "</B>");
        A2M_CURFONT.af_bold = 0;
    }

    if(!fnt.af_underline && fntp.af_underline)
    {
        fprintf(A2M_fdhtm, "</U>");
        A2M_CURFONT.af_underline = 0;
    }

    if(!fnt.af_strike && fntp.af_strike)
    {
        fprintf(A2M_fdhtm, "</STRIKE>");
        A2M_CURFONT.af_strike = 0;
    }

    if(A2M_HTML_STYLE == 0 && A2M_HTML_RELSTYLE == 0 && fnt.af_size != fntp.af_size) { /* JMP 29-01-2004 */
        if(fnt.af_size <= 7)       fprintf(A2M_fdhtm, "<FONT SIZE=-2>");
        else if(fnt.af_size <= 10) fprintf(A2M_fdhtm, "<FONT SIZE=-1>");
        else if(fnt.af_size <= 12) fprintf(A2M_fdhtm, "<FONT SIZE=+0>");
        else if(fnt.af_size <= 14) fprintf(A2M_fdhtm, "<FONT SIZE=+1>");
        else if(fnt.af_size <= 16) fprintf(A2M_fdhtm, "<FONT SIZE=+2>");
        else if(fnt.af_size <= 18) fprintf(A2M_fdhtm, "<FONT SIZE=+3>");
        else fprintf(A2M_fdhtm, "<FONT SIZE=+4>");
        A2M_CURFONT.af_size++;
    }

    if(fnt.af_color != fntp.af_color) {
        fprintf(A2M_fdhtm, "<FONT COLOR=\"#%s\">",
                A2M_HTMLCOLORS[fnt.af_color]);
        A2M_CURFONT.af_color++;
    }

    if(A2M_HTML_STYLE == 0 && A2M_HTML_RELSTYLE == 0 &&
            fnt.af_family == 'H' && fntp.af_family != 'H') /* JMP 29-01-2004 */
    {
        fprintf(A2M_fdhtm, "<FONT FACE=\"Arial\">");
        A2M_CURFONT.af_color++;
    }
    if(A2M_HTML_STYLE == 0 && A2M_HTML_RELSTYLE == 0 &&
            fnt.af_family == 'V' && fntp.af_family != 'V') /* JMP 09-05-99 */
    {
        fprintf(A2M_fdhtm, "<FONT FACE=\"Verdana\">");
        A2M_CURFONT.af_color++;
    }


    /* SET */
    if(fnt.af_strike && !fntp.af_strike)
    {
        fprintf(A2M_fdhtm, "<STRIKE>");
        A2M_CURFONT.af_strike = 1;
    }

    if(fnt.af_underline && !fntp.af_underline)
    {
        fprintf(A2M_fdhtm, "<U>");
        A2M_CURFONT.af_underline = 1;
    }

    if(fnt.af_bold && !fntp.af_bold)
    {
        fprintf(A2M_fdhtm, "<B>");
        A2M_CURFONT.af_bold = 1;
    }

    if(fnt.af_italic && !fntp.af_italic)
    {
        fprintf(A2M_fdhtm, "<I>");
        A2M_CURFONT.af_italic = 1;
    }

    if(fnt.af_pos == 1 && fntp.af_pos != 1)
    {
        fprintf(A2M_fdhtm, "<SUP>");
        A2M_CURFONT.af_pos = 1;
    }

    if(fnt.af_pos == -1 && fntp.af_pos != -1)
    {
        fprintf(A2M_fdhtm, "<SUB>");
        A2M_CURFONT.af_pos = -1;
    }

    if(fnt.af_family == 'C' && fntp.af_family != 'C')
    {
        fprintf(A2M_fdhtm, "<TT>");
        A2M_CURFONT.af_family = 1;
    }

    if(fnt.af_family == 'G' && fntp.af_family != 'G')
    {
        A2M_HTML_GREEK = 1;
    }

    return(0);
}

/*NH*/
A2mHtmlPrintChar(fd, ch)
FILE    *fd;
int     ch;
{
    char    *txt;


    if(A2M_HTML_GREEK) {
        switch(ch) {
        case 'a'    :
            txt = "&alpha;";
            break;
        case 'A'    :
            txt = "&Alpha;";
            break;
        case 'b'    :
            txt = "&beta;";
            break;
        case 'B'    :
            txt = "&Beta;";
            break;
        case 'g'    :
            txt = "&gamma;";
            break;
        case 'G'    :
            txt = "&Gamma;";
            break;
        case 'd'    :
            txt = "&delta;";
            break;
        case 'D'    :
            txt = "&Delta;";
            break;
        case 'e'    :
            txt = "&epsilon;";
            break;
        case 'E'    :
            txt = "&Epsilon;";
            break;
        case 'z'    :
            txt = "&zeta;";
            break;
        case 'Z'    :
            txt = "&Zeta;";
            break;
        case 'n'    :
            txt = "&eta;";
            break;
        case 'N'    :
            txt = "&Eta;";
            break;
        case 'h'    :
            txt = "&eta;";
            break;
        case 'H'    :
            txt = "&Eta;";
            break;
        case 'i'    :
            txt = "&iota;";
            break;
        case 'I'    :
            txt = "&Iota;";
            break;
        case 'k'    :
            txt = "&kappa;";
            break;
        case 'K'    :
            txt = "&Kappa;";
            break;
        case 'l'    :
            txt = "&lambda;";
            break;
        case 'L'    :
            txt = "&Lambda;";
            break;
        case 'm'    :
            txt = "&mu;";
            break;
        case 'M'    :
            txt = "&Mu;";
            break;
        case 'v'    :
            txt = "&nu;";
            break;
        case 'V'    :
            txt = "&Nu;";
            break;
        case 'o'    :
            txt = "&omicron;";
            break;
        case 'O'    :
            txt = "&Omicron;";
            break;
        case 'x'    :
            txt = "&xi;";
            break;
        case 'X'    :
            txt = "&Xi;";
            break;
        case 'p'    :
            txt = "&pi;";
            break;
        case 'P'    :
            txt = "&Pi;";
            break;
        case 'r'    :
            txt = "&rho;";
            break;
        case 'R'    :
            txt = "&Rho;";
            break;
        case 's'    :
            txt = "&sigma;";
            break;
        case 'S'    :
            txt = "&Sigma;";
            break;
        case 't'    :
            txt = "&tau;";
            break;
        case 'T'    :
            txt = "&Tau;";
            break;
        case 'u'    :
            txt = "&upsilon;";
            break;
        case 'U'    :
            txt = "&Upsilon;";
            break;
        case 'j'    :
        case 'f'    :
            txt = "&phi;";
            break;
        case 'J'    :
        case 'F'    :
            txt = "&Phi;";
            break;
        case 'c'    :
            txt = "&chi;";
            break;
        case 'C'    :
            txt = "&Chi;";
            break;
        case 'y'    :
            txt = "&psi;";
            break;
        case 'Y'    :
            txt = "&Psi;";
            break;
        case 'w'    :
            txt = "&omega;";
            break;
        case 'W'    :
            txt = "&Omega;";
            break;
        default     :
            fprintf(fd, "%c", ch);
            return(0);
        }
    }
    else {
        /* if(A2M_HTML_OEMTOANSI) ch = SCR_OemToAnsiChar(ch); // JMP 16-08-10
        switch(ch) {
          case 129    : txt = "&uuml;";   break; // u trema
          case 130    : txt = "&eacute;"; break; // e aigu
          case 131    : txt = "&acirc;";  break; // a circomflexe
          case 132    : txt = "&auml;";   break; // a trema
          case 133    : txt = "&agrave;"; break; // a grave
          case 135    : txt = "&#231;";   break; // c cedille
          case 136    : txt = "&ecirc;";  break; // e circomflexe
          case 137    : txt = "&euml;";   break; // e trema
          case 138    : txt = "&egrave;"; break; // e grave
          case 139    : txt = "&iuml;";   break; // i trema
          case 140    : txt = "&icirc;";  break; // i circomflexe
          case 147    : txt = "&ocirc;";  break; // o circomfl
          case 148    : txt = "&ouml;";   break; // o trema
          case 150    : txt = "&ucirc;";  break; // u circomflexe
          case 151    : txt = "&ugrave;"; break; // u grave
          case 225    : txt = "&szlig;";  break; // ss allemand
          case 167    : txt = "&#176;";   break; // degree
          case 238    : txt = "'";        break; // single quote
          case 239    :
          case 240    : txt = "\"";       break; // double quote
          case 248    : txt = "&#176;";   break; // degree
          case '�'    : txt = "&#176;";   break; // degree
          default     :
        if(A2M_HTML_LTGT == 0 && ch == '<') txt = "&lt;";
        else if(A2M_HTML_LTGT == 0 && ch == '>') txt = "&gt;";
        else if(A2M_HTML_LTGT == 0 && ch == '&') txt = "&amp;";
        else {fprintf(fd, "%c", ch); return(0);}
        break;
          }
          */
        if(A2M_HTML_OEMTOANSI) ch = SCR_OemToAnsiChar(ch); // JMP 16-08-10
        switch(ch) {
        case 0xE1 :
            txt = "&aacute;";
            break; // a grave
        case 0xE0 :
            txt = "&agrave;";
            break; // a grave
        case 0xE2 :
            txt = "&acirc;";
            break; // a circomflexe
        case 0xE4 :
            txt = "&auml;";
            break; // a trema
        case 0xC7 :
            txt = "&Ccedil;";
            break; // c cedille
        case 0xE7 :
            txt = "&ccedil;";
            break; // c cedille
        case 0xE9 :
            txt = "&eacute;";
            break; // e aigu
        case 0xE8 :
            txt = "&egrave;";
            break; // e grave
        case 0xEA :
            txt = "&ecirc;";
            break; // e circomflexe
        case 0xEB :
            txt = "&euml;";
            break; // e trema
        case 0xED :
            txt = "&iacute;";
            break; // i aigu
        case 0xEC :
            txt = "&igrave;";
            break; // i grave
        case 0xEE :
            txt = "&icirc;";
            break; // i circomflexe
        case 0xEF :
            txt = "&iuml;";
            break; // i trema
        case 0xF3 :
            txt = "&oacute;";
            break; // o aigu
        case 0xF2 :
            txt = "&ograve;";
            break; // o grave
        case 0xF4 :
            txt = "&ocirc;";
            break; // o circomfl
        case 0xF6 :
            txt = "&ouml;";
            break; // o trema
        case 0xFA :
            txt = "&uacute;";
            break; // u grave
        case 0xF9 :
            txt = "&ugrave;";
            break; // u grave
        case 0xFB :
            txt = "&ucirc;";
            break; // u circomflexe
        case 0xFC :
            txt = "&uuml;";
            break; // u trema
        case 0xDF :
            txt = "&szlig;";
            break; // ss allemand
        case 0xB0 :
            txt = "&#176;";
            break; // degree
        case '\''   :
            txt = "'";
            break; // single quote
        case '"'    :
            txt = "\"";
            break; // double quote
        default     :
            if(A2M_HTML_LTGT == 0 && ch == '<') txt = "&lt;";
            else if(A2M_HTML_LTGT == 0 && ch == '>') txt = "&gt;";
            else if(A2M_HTML_LTGT == 0 && ch == '&') txt = "&amp;";
            else {
                fprintf(fd, "%c", ch);
                return(0);
            }
            break;
        }
    }
    fprintf(fd, "%s", txt);
    return(0);
}

/*NH*/
A2mHtmlGetCenter(A2MPAR *ap)
{
    A2MPAR  tmpap;

    A2mSetParProps(ap->ap_tag, &(tmpap.ap_ppr));
    return(tmpap.ap_ppr.pp_just);
}

/*NH*/
A2mHtmlGetNowrap(A2MPAR *ap)
{
    A2MPAR  tmpap;

    if(ap == 0) return(0);
    A2mSetParProps(ap->ap_tag, &(tmpap.ap_ppr));
    return(tmpap.ap_ppr.pp_nowrap);
}

/*NH*/
char *A2mHtmlClass(char *class, char *dftclass)
{
    static char res[256];

    if(class == 0)               sprintf(res, " class=\"%s\"", dftclass);
    else if(strlen(class) > 0)   sprintf(res, " class=\"%s\"", class);
    else                         res[0] = 0;

    return(res);
}

/*NH*/
A2mHtmlPrintTbl(at)
A2MTBL  *at;
{
    A2MTC   *tc;
    int     i, j, col;
    int     b = at->at_border;
    char    *tdh, *baseclass, cellclass[256];

    A2M_NB_TABS = 0;
    if(b < 0) b = A2M_HTML_TBORDER;
    for(i = 0 ; i < A2M_LEVEL ; i++) fprintf(A2M_fdhtm, "</UL>\n");
    A2M_LEVEL = 0;
    if(A2M_HTML_TTITLE == 0) A2mHtmlPrintPar(at->at_title, 0, 0);

    // <TABLE>
    if(A2M_HTML_TABLECLASS == 0)
        fprintf(A2M_fdhtm, "<TABLE %s BORDER=%d cellspacing=\"0\" cellpadding=\"0\">\n", A2mHtmlClass(A2M_HTML_TABLECLASS, "A2mTable"), b);
    else
        fprintf(A2M_fdhtm, "<TABLE%s>\n", A2mHtmlClass(A2M_HTML_TABLECLASS, "A2mTable")); // JMP 3/10/2013

    // TITLE
    if(A2M_HTML_TTITLE && at->at_title && at->at_title->ap_strs) {
        // <TR>
        fprintf(A2M_fdhtm, " <THEAD><TR%s>\n", A2mHtmlClass(A2M_HTML_TRCLASS, "A2mTR"));

        // TH
        fprintf(A2M_fdhtm, "  <TH%s", A2mHtmlClass(A2M_HTML_THCLASS, "A2mTH"));
        if(at->at_nc > 1) fprintf(A2M_fdhtm, " COLSPAN=%d", at->at_nc);
        fprintf(A2M_fdhtm, ">\n");
        A2mHtmlPrintPar(at->at_title, 1, 0);
        fprintf(A2M_fdhtm, "&nbsp;</p></TH>\n </TR></THEAD>\n");
    }

    for(i = 0 ; i < at->at_nl ; i++) {
        // <TD> or <TH> ? + Classe par d�faut en fonction thead ou pas
        if(at->at_tls[i].atl_hbf == 1) {
            tdh = "TD";
            baseclass = A2M_HTML_TDCLASS;
            if(baseclass == 0)  baseclass =  "A2mTD";
        }
        else {
            tdh = "TH";
            baseclass = A2M_HTML_THCLASS;
            if(baseclass == 0)  baseclass =  "A2mTH";
        }

        // <TR>
        fprintf(A2M_fdhtm, " <TR%s", A2mHtmlClass(A2M_HTML_TRCLASS, "A2mTR"));
        /* SHADING */
        col = at->at_shadcol[at->at_tls[i].atl_hbf % 2];

        if(col && A2M_HTML_TCOLOR)
            fprintf(A2M_fdhtm, " BGCOLOR=\"%s\"", A2M_HTMLCOLORS[col]);
        fprintf(A2M_fdhtm, ">\n");

        // Ligne de s�p dans le tableau
        if(at->at_tls[i].atl_type) {
            fprintf(A2M_fdhtm, "  <%s class=\"A2mTRLine\" COLSPAN=%d><HR></%s></TR>\n", tdh, at->at_nc, tdh);
            continue;
        }

        // Ligne normale dans le tableau
        // Boucle sur les cells
        for(j = 0 ; j < at->at_nc ; j++) {
            tc = at->at_tls[i].atl_tcs + j;

            // <TD> or <TH>
            fprintf(A2M_fdhtm, "  <%s", tdh);

            // COLSPAN
            if(tc->atc_ncells > 1) {
                fprintf(A2M_fdhtm, " COLSPAN=%d", tc->atc_ncells);
                j += tc->atc_ncells - 1;
            }

            if(tc->atc_center == 5)
                tc->atc_center = A2mHtmlGetCenter(tc->atc_par); /* JMP 24-01-2004 */

            // CLASS de cadrage de la cellule
            switch(tc->atc_center) {
            case 0:
                sprintf(cellclass, "A2m%sLeft", tdh);
                break;
            case 1:
                sprintf(cellclass, "A2m%sCenter", tdh);
                break;
            case 2:
            case 3:
                sprintf(cellclass,  "A2m%sRight", tdh);
                break;
            case 4:
                sprintf(cellclass,  "A2m%sJustify", tdh);
                break;
            default :
                cellclass[0] = 0;
                break;
            }
            if(baseclass[0] || cellclass[0])
                fprintf(A2M_fdhtm, " class=\"%s %s\"", baseclass, cellclass);

            // NOWRAP
            if(tc->atc_center == 6 || A2mHtmlGetNowrap(tc->atc_par))
                fprintf(A2M_fdhtm, " NOWRAP"); /* JMP 24-01-2004 */

            fprintf(A2M_fdhtm, ">");

            A2mHtmlPrintPar(tc->atc_par, 1, j);
            /*            if(tc->atc_par == 0 || strncmp(tc->atc_par->ap_tag, "CellDecimal", 11))
            fprintf(A2M_fdhtm, "&nbsp;</%s>\n", tdh);
            else
            */
            fprintf(A2M_fdhtm, "</p>");
            fprintf(A2M_fdhtm, "</%s>\n", tdh);
        }

        fprintf(A2M_fdhtm, " </TR>\n");
    }

    fprintf(A2M_fdhtm, "</TABLE>\n");

    A2M_LEVEL = 0;
    A2mHtmlPrintPar(at->at_footnote, 0, 0);
    return(0);
}

/*NH*/
A2mHtmlPrintGrf(ag)
A2MGRF  *ag;
{
    int         w, h;
    char        buf[256];
    A2MPAR      *prTitle = ag->ag_title;
    extern  int A2M_SWF_NOGIF;

    // Cas GIF
    if(A2M_HTML_IS_GIF && A2M_SWF_NOGIF == 0) 
    {
#if defined(DOSWIN) || defined(SCRW32) || defined(WINDOWS)
        fprintf(A2M_fdhtm, "<TABLE BORDER=0><TR><TD>");
        if(prTitle) A2mHtmlPrintPar(prTitle, 1, 0);
        else        fprintf(A2M_fdhtm, "<P>Figure %d</P>\n", A2M_GIF_CNT);
        fprintf(A2M_fdhtm, "</TD></TR><TR><TD>");
        sprintf(buf, "img%d.gif", A2M_GIF_CNT);
        A2mGIF_HTML(ag, buf);
        A2mHtmlAFrame(buf);
        fprintf(A2M_fdhtm, "</TD></TR></TABLE>");
        A2M_GIF_CNT++;
        return(0);
#else
        return(-1);
#endif
    }


    // Cas FusionChart
    else {
        /*
          <script type="text/javascript">
           var chart1 = new FusionCharts("/FusionCharts/MSLine.swf", "sampleChart", "450", "400", "0", "1");
           chart1.setDataURL("line3.xml");
           chart1.setTransparent(true);
           chart1.render("chart1div");
        </script>

         */
        w = (int) (ag->ag_size[0] * 37.79527559);
        h = (int) (ag->ag_size[1] * 37.79527559);

        A2mSWF_HTML(A2M_fdhtm, w, h, ag, A2M_GIF_CNT);
        A2M_GIF_CNT++;
        return(0);
    }

}

/*NH*/
A2mHtmlAFrame(filename)
char    *filename;
{
    char    buf[128];

    strcpy(buf, filename);

    if(A2M_HTML_GIFTOBMP && U_index(buf, ".gif") > 0)
        SCR_change_ext(buf, filename, "bmp");
    if(A2M_HTML_GIFTOPNG && U_index(buf, ".gif") > 0) /* JMP 23-08-2015 */
        SCR_change_ext(buf, filename, "png");         /* JMP 23-08-2015 */

    fprintf(A2M_fdhtm, "<IMG SRC=\"%s\" ALIGN=MIDDLE ", buf);
    if(A2M_HTML_IMGWIDTH) fprintf(A2M_fdhtm, " WIDTH=%d", A2M_HTML_IMGWIDTH);
    if(A2M_HTML_IMGBORDER >= 0) fprintf(A2M_fdhtm, " BORDER=%d", A2M_HTML_IMGBORDER);
    fprintf(A2M_fdhtm, ">");
    return(0);
}

/*NH*/
int A2mHtmlAnchor(char* ref)
{
    int     aa_nb, i;

    aa_nb = A2mFindAnchor(ref);
    if(aa_nb < 0) return(-1);
    for(i = 0 ; i < A2M_ANCHORS[aa_nb]->aa_nb_aos ; i++)
        A2mHtmlPrintObj(A2M_ANCHORS[aa_nb]->aa_aos[i], 0);

    return(0);
}

/*NH*/
int A2mHtmlFNote(char* ref)
{
    A2mAddFNote(ref);
    fprintf(A2M_fdhtm, "<A HREF=\"#FNote%d\"><SUP><U>[%d]</U></SUP></A>", A2M_NB_FNOTES, A2M_NB_FNOTES);

    return(0);
}

/*NH*/
int A2mHtmlPrintFNotes()
{
    int     i;

    if(A2M_NB_FNOTES == 0) return(0);
    fprintf(A2M_fdhtm, "<HR SIZE=1 color=blue width=200 ALIGN=left>\n");
    for(i = 0 ; i < A2M_NB_FNOTES ; i++) {
        fprintf(A2M_fdhtm, "<TABLE BORDER=0>\n <TR>\n  <TD VALIGN=\"TOP\"><P><A NAME=\"FNOTE%d\"><SUP><U>[%d]</U></SUP></A></P></TD>\n\n  <TD>", i + 1, i + 1);
        A2mHtmlAnchor(A2M_FNOTES[i]);
        fprintf(A2M_fdhtm, "\n  </TD>\n </TR>\n</TABLE>\n");
    }

    return(0);
}

/* ================================================================
Fixe la valeur des variables globales avant l'interp�tation d'un fichier
a2m et la g�n�ration d'un fichier html en se basant sur les d�finitions du
fichier .ini associ�.

Les sections [A2M] et [HTML] du fichier .ini sont interpr�t�es.

&EN filename = nom du fichier .ini

La syntaxe des fichiers a2m est d�crite dans un chapitre sp�cifique.

&TI Fichier a2m.ini
���������������
Le fichier a2m.ini (ou un autre) contient des param�tres pour
la lecture du fichier a2m etla g�nration du fichier html. Pour que ces
param�tres soient pris en compte par le programme A2mToHtml(), il faut
appeler la fonction A2mHtmlReadIni(filename) avant de lancer la fonction
d'interpr�tation et d'impression.

&CO
    #include <s_a2m.h>

    A2mHtmlReadIni("myfile.ini");
    A2mToHtml("myfile.a2m", "myfile.htm");
&TX

&SA A2mToGdi(), A2mToRtf(), A2mToMif(), A2mToHtml(), A2mPrintError()
==================================================================== */
A2mHtmlReadIni(filename)
char    *filename;
{
    U_ch    buf[256];

    A2mReadA2mIni(filename);
    if(IniReadTxtParm(filename, "HTML", "BODY", buf, 255) == 0)
        A2M_HTML_BODY = SCR_stracpy(buf);
    if(IniReadTxtParm(filename, "HTML", "TITLE", buf, 255) == 0)
        A2M_HTML_TITLE = SCR_stracpy(buf);
    if(IniReadTxtParm(filename, "HTML", "FONTFAMILY", buf, 255) == 0)
        A2M_FONTFAMILY = buf[0];

    IniReadNumParm(filename, "HTML", "FONTSIZE",       &A2M_FONTSIZE);
    IniReadNumParm(filename, "HTML", "FONTINCR",       &A2M_FONTINCR);
    IniReadNumParm(filename, "HTML", "TABLEFONTSIZE",  &A2M_TFONTSIZE);
    IniReadNumParm(filename, "HTML", "NUMBERING",      &A2M_NUMBERS);
    IniReadNumParm(filename, "HTML", "TABLEBORDER",    &A2M_HTML_TBORDER);
    IniReadNumParm(filename, "HTML", "TABLECOLOR",     &A2M_HTML_TCOLOR);
    IniReadNumParm(filename, "HTML", "TOC",            &A2M_HTML_TOC);
    return(0);
}

/*NH*/
int A2mHtmlPrintTxt(U_ch* txt)
{
    return(A2mHtmlPrintFdTxt(A2M_fdhtm, txt));
}

/*NH*/
int A2mHtmlPrintCntTxt(U_ch* txt)
{
    return(A2mHtmlPrintFdTxt(A2M_fdhhc, txt));
}

/*NH*/
int A2mHtmlPrintTocTxt(U_ch* txt)
{
    return(A2mHtmlPrintFdTxt(A2M_fdtoc, txt));
}

/*NH*/
int A2mHtmlPrintKeyTxt(U_ch* txt)
{
    return(A2mHtmlPrintFdTxt(A2M_fdhhk, txt));
}

/*NH*/
A2mHtmlPrintFdTxt(fd, txt)
FILE    *fd;
U_ch    *txt;
{
    int     i;

    if(txt == 0) return(0);
    for(i = 0 ; txt[i] ; i++) A2mHtmlPrintChar(fd, txt[i]);
    return(0);
}

/*NH*/
int A2mHtmlPrintTopicCnt(A2MTOP *atp)
{
    int     i, clevel = 0;

    if(atp) clevel = atp->atp_level;

    if(A2M_HTML_TOCLEVEL >= 0) {
        if(clevel <= A2M_HTML_TOCLEVEL &&
                A2M_HTML_TOCLEVEL >= A2M_HTML_TOCPLEVEL)  {
            fprintf(A2M_fdhhc, "<LI><OBJECT type=\"text/sitemap\">\n");
            fprintf(A2M_fdhhc, "    <param name=\"Name\" value=\"");
            fprintf(A2M_fdtoc, "<LI><DIV NOWRAP><A HREF=\"T%04d.htm\">", A2M_HTML_TOCNB); /* JMP 25-01-2004 */
            if(A2M_HTML_OEMTOANSI) SCR_AnsiToOem(A2M_HTML_TOCTXT, A2M_HTML_TOCTXT);
            A2mHtmlPrintCntTxt(A2M_HTML_TOCTXT);
            A2mHtmlPrintTocTxt(A2M_HTML_TOCTXT);                              /* JMP 25-01-2004 */
            if(A2M_HTML_OEMTOANSI) SCR_OemToAnsi(A2M_HTML_TOCTXT, A2M_HTML_TOCTXT);
            fprintf(A2M_fdhhc, "\">\n    <param name=\"Local\" value=\"");
            fprintf(A2M_fdhhc, "T%04d.htm", A2M_HTML_TOCNB);
            fprintf(A2M_fdhhc, "\">\n    <param name=\"ImageNumber\" value=\"5");
            fprintf(A2M_fdhhc, "\"></OBJECT>\n");
            fprintf(A2M_fdtoc, "</A></DIV>\n");       /* JMP 25-01-2004 */
        }
        else {
            if(A2M_HTML_TOCLEVEL < A2M_HTML_TOCPLEVEL)
                for(i = 0; i < A2M_HTML_TOCPLEVEL - A2M_HTML_TOCLEVEL; i++) {
                    fprintf(A2M_fdhhc, "</UL>\n");
                    fprintf(A2M_fdtoc, "</UL>\n");
                }

            fprintf(A2M_fdhhc, "<LI><OBJECT type=\"text/sitemap\">\n");
            fprintf(A2M_fdhhc, "    <param name=\"Name\" value=\"");
            fprintf(A2M_fdtoc, "<LI><DIV NOWRAP><A HREF=\"T%04d.htm\">", A2M_HTML_TOCNB); /* JMP 25-01-2004 */
            if(A2M_HTML_OEMTOANSI) SCR_AnsiToOem(A2M_HTML_TOCTXT, A2M_HTML_TOCTXT);
            A2mHtmlPrintCntTxt(A2M_HTML_TOCTXT);
            A2mHtmlPrintTocTxt(A2M_HTML_TOCTXT);
            if(A2M_HTML_OEMTOANSI) SCR_OemToAnsi(A2M_HTML_TOCTXT, A2M_HTML_TOCTXT);
            fprintf(A2M_fdhhc, "\">\n    <param name=\"Local\" value=\"");
            fprintf(A2M_fdhhc, "T%04d.htm", A2M_HTML_TOCNB);
            fprintf(A2M_fdhhc, "\">\n    <param name=\"ImageNumber\" value=\"5");
            fprintf(A2M_fdhhc, "\"></OBJECT>\n");
            fprintf(A2M_fdhhc, "  <UL>\n");
            fprintf(A2M_fdtoc, "</A></DIV>\n<UL>\n");

            A2M_HTML_TOCLEVEL++;
        }
    }

    SCR_free(A2M_HTML_TOCTXT);
    A2M_HTML_TOCTXT = 0;
    if(atp) {
        A2M_HTML_TOCTXT   = SCR_stracpy(atp->atp_keyws);
        if(A2M_HTML_OEMTOANSI) SCR_OemToAnsi(A2M_HTML_TOCTXT, A2M_HTML_TOCTXT);
        SCR_replace(A2M_HTML_TOCTXT, "\"", "'");
        A2M_HTML_TOCPLEVEL = A2M_HTML_TOCLEVEL;
        A2M_HTML_TOCLEVEL = clevel;
        A2M_HTML_TOCNB    = atp->atp_nb;
    }
    return(0);
}

/*NH*/
int A2mHtmlPrintTopicKey(A2MTOP* atp)
{
    fprintf(A2M_fdhhk, "<LI><OBJECT type=\"text/sitemap\">\n");
    fprintf(A2M_fdhhk, "    <param name=\"Name\" value=\"");
    SCR_replace(atp->atp_keyws, "\"", "'");
    A2mHtmlPrintKeyTxt(atp->atp_keyws);
    fprintf(A2M_fdhhk, "\">\n    <param name=\"Local\" value=\"");
    fprintf(A2M_fdhhk, "T%04d.htm", A2M_HTML_TOCNB);
    fprintf(A2M_fdhhk, "\"></OBJECT>\n");
    return(0);
}




int A2mOemToAnsi(fd, title)
FILE    *fd;
unsigned char   *title;
{
    int j;

    if(title == 0) return(0);

    for(j = 0; title[j]; j++) {
        switch(title[j])  {
//          case '\'': fprintf(fd, "%s", "%26apos;"); break; /* GB 07/02/2011 */
        case '\'':
            fprintf(fd, "%s", "&apos;");
            break;
//          case '"': fprintf(fd, "\""); break;
        case '"':
            fprintf(fd, "&quot;");
            break;
        default:
            fprintf(fd, "%c", SCR_OemToAnsiChar(title[j]));
            break;
        }
    }
    return(0);
}


A2mGetModulePath(char *buf)
{
    //HANDLE hInst;                                     // JMP 22/11/2011
    int         i;

    //hInst = GetModuleHandle(NULL); // JMP 22/11/2011
    //GetModuleFileName(hInst, buf, 256); // JMP 22/11/2011
#ifdef LINUX
    //extern char *getexecname();
    //if(getexecname()) strcpy(buf, getexecname());  // JMP 31/10/2012
    strcpy(buf, "./");
#else
    GetModuleFileName(NULL, buf, 256);
#endif
    for(i = (int)strlen(buf); i > 0 && buf[i] != '\\' && buf[i] != '/'; i--);  // JMP 22/11/2011
    buf[i] = 0;
    return(0);
}






