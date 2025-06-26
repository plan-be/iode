#include "s_a2m.h"
#include "s_ini.h"

/********************* Parameters *************************/

/* Public */
int     A2M_DOKU_TCOLOR = 0;  /* JMP 12-02-99 */
int     A2M_DOKU_TBORDER = 1;
char    *A2M_DOKU_BODY = 0;
int     A2M_DOKU_IS_GIF = 1;
U_ch    *A2M_DOKU_TITLE = 0;
int     A2M_DOKU_GIFTOBMP = 0;
int     A2M_DOKU_GIFTOPNG = 0; /* JMP 23-08-2015 */
int     A2M_DOKU_AUTOLINK = 1;

int     A2M_DOKU_OEMTOANSI = 1;
int     A2M_DOKU_NOINLINESTYLE = 0;

/* Common with s_a2mhtm */  /* JMP 28-11-2013 */
extern FILE *A2M_fdtoc;
extern A2MFNT  A2M_CURFONT;
extern int     A2M_LEVEL, A2M_HTMLREF, A2M_GIF_CNT, A2M_TABS[10], A2M_NB_TABS;

/* Private */
FILE    *A2M_fdtxt;
int     A2M_DOKU_LTGT = 0;
int     A2M_DOKU_STRIP = 0;
int     A2M_DOKU_TTITLE= 1; /* JMP 12-02-99 */

/* Private */
int     A2M_DOKU_TOPIC = -1;
//int     A2M_DOKU_TOCLEVEL, A2M_DOKU_TOCPLEVEL, A2M_DOKU_TOCNB;
//U_ch    *A2M_DOKU_TOCTXT;
int     A2M_DOKU_GREEK = 0;
int     A2M_DOKU_TOCLEVEL = 1;

int     A2M_DOKU_IMGWIDTH = 0;
char    *A2M_DOKU_IMGDIR = "wiki";
char    *A2M_DOKU_PREFX = "section_"; // Préfixe des documents ss topics
int     A2M_DOKU_UTF8 = 1;

// Work vars
int     A2M_DOKU_FIGNB = 1;
int     A2M_DOKU_ESC = 1; // Escape /, _ and * in texts
int     A2M_DOKU_CURLEVEL = 0;


/* ================================================================
Interprète le contenu d'un fichier a2m et génère un fichier .txt en format DokuWiki.

&EN a2mfile = nom du fichier a2m à interpréter
&EN outfile = nom du fichier htm à générer

&RT La fonction retourne 0 si le processus s'est bien déroulé, -1 sinon.

La syntaxe des fichiers a2m est décrite dans un chapitre spécifique.

&TI Fichier a2m.ini
───────────────
Le fichier a2m.ini (ou un autre) contient des paramètres pour
l'impression et la lecture du fichier a2m. Pour que ces paramètres
soient pris en compte par le programme A2mToHtml(), il faut appeler la
fonction A2mDokuReadIni(filename) avant de lancer la fonction
d'interprétation et d'impression.

&CO
    #include <s_a2m.h>

    A2mDokuReadIni("myfile.ini");
    A2mToDoku("myfile", "test.txt");
&TX

&TI Variables globales
──────────────────
Les variables globales décrites dans le fichier .ini peuvent également
être modifiées dans le programme.

&IT Variables influençant la lecture du fichier
──────────────────────────────────────────
Ces variables sont définies dans la secion [A2M] du fichier ini.

&EN int A2M_ESCCH = caractère d'escape (enrichissements et caractères
    spéciaux) : '\'par défaut
&EN int A2M_CMDCH = caractère de commande ('.' par défaut)
    spéciaux)
&EN int A2M_DEFCH = caractère préfixant les macros ('&' par défaut)
&EN int A2M_SEPCH = caractère de séparation des cellules ('&' par défaut)
&EN int A2M_LFON = conserve les linefeed (1) ou non (0)
&EN int A2M_BLON = conserve les blancs (1) ou non (0)
&EN char A2M_CURTAG[41] = paragraphe par défaut ("par_0")

&IT Variables influençant le fichier généré
───────────────────────────────────────
Ces variables sont définies dans la section [HTML] du fichier .ini.

&EN U_ch *A2M_DOKU_TITLE = titre du fichier txt (?)
&EN int A2M_DOKU_LTGT = 0 si les caractères < et > doivent apparaître
    comme tel en HTML (&lt;) et non interprétés comme tag (on mettra 0
    dans ce cas).
&EN int A2M_DOKU_AUTOLINK = 0 pour supprimer la création automatique de liens
    par l'analyse des sujets d'aide (accélère le traitement)

&RT
&EN 0 en cas de succès
&EN -1 si le fichier ne peut être ouvert
&EN -2 si les fichiers output n'ont pu être créés


&SA A2mDokuReadIni(), A2mToRtf(), A2mToMif(), A2mToMif(), A2mPrintError()
==================================================================== */

A2mToDoku(a2mfile, outfile)
char    *a2mfile, *outfile;
{
    A2MFILE *af;
    A2MOBJ  *ao;

    A2mReadTopics(a2mfile);
    af = A2mOpen(a2mfile, 0);
    if(af == 0) {
        A2mFreeTopics();
        return(-1);
    }

    if(A2mDokuInit(outfile)) {
        A2mClose(af);
        A2mFreeTopics();
        return(-2);
    }

    while(1) {
        ao = A2mRead(af);
        if(ao == 0) break;
        A2mDokuPrintObj(ao, 0);
    }

    A2mDokuEnd();
    A2mClose(af);
    A2mFreeTopics();
    return(0);
}

/*NH*/
// Header de chaque fichier généré

A2mDokuPre(fd)
FILE    *fd;
{
}

/*NH*/
A2mDokuPost(fd)
FILE    *fd;
{
}

/*NH*/
A2mDokuInit(outfile)
char    *outfile;
{
    int         i;
    char        buf[256];
    extern int  A2M_CUROBJ;

    A2M_CUROBJ = 1;
    A2M_GIF_CNT = 0;
    A2M_LEVEL = A2M_HTMLREF = 0;

    A2M_fdtxt = fopen(outfile, "w+");
    if(A2M_fdtxt == 0) return(-2);
    if(A2M_DOKU_TITLE && A2M_DOKU_OEMTOANSI) SCR_OemToAnsi(A2M_DOKU_TITLE, A2M_DOKU_TITLE); /* JMP 16-08-10 */
    A2mDokuPre(A2M_fdtxt);
    A2mFreeFNotes();

    /*  TOC */
    SCR_change_ext(buf, outfile, "toc.txt");
    A2M_fdtoc = fopen(buf, "w+");
    if(A2M_fdtoc == 0) {
        fclose(A2M_fdtxt);
        fprintf("Error creating %s\n", buf);
        return(-2);
    }
    A2mDokuPre(A2M_fdtoc);
    return(0);
}


/*NH*/
int A2mDokuEnd()
{
    int     i;

    A2M_LEVEL = 0;
    A2mFreeFNotes();
    A2mDokuPost(A2M_fdtxt);
    fclose(A2M_fdtxt);
    A2M_fdtxt = 0;
    if(A2M_fdtoc) fclose(A2M_fdtoc);
    A2M_fdtoc = 0;

    if(A2M_DOKU_TITLE && A2M_DOKU_OEMTOANSI) SCR_AnsiToOem(A2M_DOKU_TITLE, A2M_DOKU_TITLE); /* JMP 30-04-99 */

    return(0);
}

/*NH*/
A2mDokuPrintObj(ao, del)
A2MOBJ  *ao;
int     del;
{
    char        msg[80];
    extern int  A2M_CUROBJ;

    sprintf(msg, "Printing Object %d", A2M_CUROBJ++);
    A2mMessage(msg);
    if(ao->ao_type == A2M_PAR)
        A2mDokuPrintPar((A2MPAR *)ao->ao_ptr, 0, 0);

    if(ao->ao_type == A2M_TOPIC)
        A2mDokuPrintTopic((A2MTOP *)ao->ao_ptr, 0);

    /*if(ao->ao_type == A2M_TOPICALIAS)
    A2mDokuPrintTopic((A2MTOP *)ao->ao_ptr, 1);
    */

    if(ao->ao_type == A2M_TBL)
        A2mDokuPrintTbl((A2MTBL *)ao->ao_ptr);

    if(ao->ao_type == A2M_GRF)
        A2mDokuPrintGrf((A2MGRF *)ao->ao_ptr);

    if(del) A2mFreeObj(ao);
    return(0);
}

A2mDokuTopicFilename(char *title, char *topicname, int topicnb)
{
    int     i, j;
    char    *word, res[256];

    strcpy(topicname, title);
    SCR_upper(topicname);  // à -> E
    SCR_lower(topicname);  // E -> e

    for(i = j = 0 ; topicname[i] ;  i++) {
        if(!SCR_is_anum(topicname[i])) {
            res[j] = 0;
            switch(topicname[i]) {
                case '@' :
                    word = "at_";
                    break;
                case '-' :
                    word = "minus_";
                    break;
                case '+' :
                    word = "plus_";
                    break;
                case '=' :
                    word = "equal_";
                    break;
                case '/' :
                    word = "slash_";
                    break;
                case '\\' :
                    word = "backslash_";
                    break;
                case '#' :
                    word = "cross_";
                    break;
                case '|' :
                    word = "pipe_";
                    break;
                case '~' :
                    word = "tilde_";
                    break;
                case ':' :
                    word = "colon_";
                    break;
                case '.' :
                    word = "dot_";
                    break;
                case ',' :
                    word = "comma_";
                    break;
                default :
                    word = "_";
                    break;
            }
            if(word[0] != '_') {
                strcat(res, word);
                j = strlen(res);
            }
            else {
                if(topicname[i + 1] && j > 0 && res[j - 1] != '_') res[j++] = '_';
            }
        }
        else
            res[j++] = topicname[i];
    }
    res[j] = 0;
    if(j < 3)
        sprintf(res + j, "top_%d", topicnb);

    strcpy(topicname, res);
    return(0);
}

/*NH*/
A2mDokuPrintTopic(atp, alias)
A2MTOP  *atp;
int     alias;
{
    char    buf[256], topicname[256];
    int     i;

    if(atp == 0) return(0);
    A2mDokuPrintTopicCnt(atp);

    if(A2M_fdtxt != NULL) {
        A2M_LEVEL = 0;
        A2mFreeFNotes();
        A2mDokuPost(A2M_fdtxt);
        fclose(A2M_fdtxt);
        A2M_fdtxt = NULL;
    }

    //sprintf(buf, "%s%04d.txt", A2M_DOKU_PREFX, atp->atp_nb);
    A2mDokuTopicFilename(atp->atp_keyws, topicname, atp->atp_nb);
    sprintf(buf, "%s.txt", topicname);
    A2M_fdtxt = fopen(buf, "w+");
    if(A2M_fdtxt == 0) {
        printf("Impossible d'ouvrir '%s'. Stopped.\n", buf);
        return(-2);
    }
    A2mDokuPre(A2M_fdtxt, 0);
    A2M_DOKU_CURLEVEL = atp->atp_level;

    return(0);
}

/*NH*/
A2mDokuCalcBlc(ap)
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
A2mDokuPrintTitlePrefx(int level)
{
    int i;

    level -= A2M_DOKU_CURLEVEL;
    level = 7 - level;
    if(level <= 0) level = 1;
    if(level > 6) level = 6;
    for(i = 0; i < level ; i++) fprintf(A2M_fdtxt, "=");
}

/*NH*/
A2mDokuPrintEnumPrefx(int lmarg)
{
    int i, level;

    if(lmarg <= 0) return(0);

    level = (lmarg - 24) / 8;
    for(i = 0; i < level ; i++) fprintf(A2M_fdtxt, "  ");
    fprintf(A2M_fdtxt, "* ");
}

A2mDokuIsPre(A2MPPR *pp)
{
    return(pp->pp_fnt.af_family == 'C');
}

/*NH*/
A2mDokuPrintPar(ap, tbl, tcol)
A2MPAR  *ap;
int     tbl, tcol;
{
    int     i, level, blc, cur_fignb;
    char    tag[32], *just;
    A2MFNT  fntd;
    A2MPPR  *pp;

    if(ap == 0 || ap->ap_strs == 0) {
        if(tbl) fprintf(A2M_fdtxt, " ");
        return(0);
    }

    pp = &(ap->ap_ppr);
    A2mSetParProps(ap->ap_tag, &(ap->ap_ppr));
    if(ap->ap_tag && strcmp(ap->ap_tag, "parb") == 0)
        pp->pp_html = A2M_DOKU_CURLEVEL + 2;
    if(ap->ap_tag && strcmp(ap->ap_tag, "pari") == 0)
        pp->pp_html = A2M_DOKU_CURLEVEL + 3;

    if(!tbl) fprintf(A2M_fdtxt, "\n");

    A2M_DOKU_ESC = 1; // escape * / and _ ?
    if(pp->pp_html)  {
        A2mDokuPrintTitlePrefx(pp->pp_html);
        A2M_DOKU_ESC = 0;
    }
    else if(pp->pp_bullet)
        A2mDokuPrintEnumPrefx(pp->pp_lmarg);

    else if(A2mDokuIsPre(pp)) {
        fprintf(A2M_fdtxt, "<code>\n");
        A2M_DOKU_ESC = 0;
    }


    // Print paragraph
    A2mDokuCalcTagFnt(tag, &fntd);
    memset(&A2M_CURFONT, 0, sizeof(A2MFNT));

    cur_fignb = A2M_DOKU_FIGNB; // save before texts figure nb++
    for(i = 0 ; ap->ap_strs[i] ; i++)
        A2mDokuPrintStr(ap, i, &fntd, tbl, pp);

    // Print images at the end of paragraph
    A2M_DOKU_FIGNB = cur_fignb; // reset before image titles figure nb++
    for(i = 0 ; ap->ap_strs[i] ; i++)
        A2mDokuPrintStrImgs(ap, i);

    A2mDokuResetFont();

    // Ferme des paras
    if(pp->pp_html)        A2mDokuPrintTitlePrefx(pp->pp_html);
    else if(A2mDokuIsPre(pp)) fprintf(A2M_fdtxt, "\n</code>");
    if(!pp->pp_bullet && !tbl) fprintf(A2M_fdtxt, "\n");
    A2M_DOKU_ESC = 1;
    return(0);
}

/*NH*/
A2mDokuCalcTagFnt(tag, fnt)
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

int A2mFindTopic(int topicnb)
{
    int i;
    for(i = 0 ; i < A2M_NB_TOPICS ; i++)
        if(A2M_TOPICS[i]->atp_nb == topicnb) return(i);

    return(-1);
}

/*NH*/
A2mDokuPrintStr(ap, n, fntt, tbl, pp)
A2MPAR  *ap;
int     n;
A2MFNT  *fntt;
int     tbl;
A2MPPR  *pp;
{
    A2MSTR  *as = ap->ap_strs[n];
    int     i, oesc, topicnb;
    char    buf[256], topicname[256];

    if(A2M_DOKU_ESC) A2mDokuPrintAttr(ap, n, fntt, pp);
    if(A2M_NUMBERS && n == 0 && ap->ap_ppr.pp_number >= 1) {
        A2mCalcNumbering(ap->ap_ppr.pp_number - 1, buf);
        A2mDokuPrintFdTxt(A2M_fdtxt, buf, A2M_DOKU_ESC);
    }

    switch(as->as_type) {
        case A2M_TEXT :
            A2mDokuPrintFdTxt(A2M_fdtxt, as->as_txt, A2M_DOKU_ESC);
            return(0);

        case A2M_IMAGEFR:
        case A2M_IMAGER:
            fprintf(A2M_fdtxt, "**Figure %d**", A2M_DOKU_FIGNB++);
            return(0);
        case A2M_IMAGEF:
        case A2M_IMAGE :
            A2M_DOKU_FIGNB++;
            return(0);
        case A2M_ANCHOR :
            A2mDokuAnchor(as->as_txt);
            return(0);
        case A2M_FNOTE :
            A2mDokuFNote(as->as_txt);
            return(0);
        case A2M_BHREF:
            fprintf(A2M_fdtxt, "[[%s|", as->as_txt);
            return(0);
        case A2M_EHREF:
            fprintf(A2M_fdtxt, "]]");
            return(0);
        case A2M_NEWLINE : //if(tbl == 0) fprintf(A2M_fdtxt, "<BR>");
            //else fprintf(A2M_fdtxt, "</P><P>");
            fprintf(A2M_fdtxt, "\n");
            return(0);

        case A2M_TAB :
            fprintf(A2M_fdtxt, " ");
            return(0);
        case A2M_TOPIC  :
            if(!A2M_DOKU_ESC) break;
            A2M_DOKU_TOPIC = atoi(as->as_txt);
            topicnb = A2mFindTopic(A2M_DOKU_TOPIC);
            if(topicnb < 0) break;
            oesc = A2M_DOKU_ESC;
            A2M_DOKU_ESC = 0;
            //fprintf(A2M_fdtxt, "[[%s%04d|", A2M_DOKU_PREFX, A2M_DOKU_TOPIC);
            A2mDokuTopicFilename(A2M_TOPICS[topicnb]->atp_keyws, topicname, topicnb);
            fprintf(A2M_fdtxt, "[[%s|", topicname);
            A2M_DOKU_ESC = oesc;
            break;

        case A2M_ETOPIC :
            if(!A2M_DOKU_ESC) break;
            fprintf(A2M_fdtxt, "]]");
            A2M_DOKU_TOPIC = -1;
            break;
    }

    return(0);
}

/*NH*/
A2mDokuPrintStrImgs(ap, n)
A2MPAR  *ap;
int     n;
{
    A2MSTR  *as = ap->ap_strs[n];

    switch(as->as_type) {
        case A2M_IMAGEFR:
        case A2M_IMAGER:
        case A2M_IMAGEF:
        case A2M_IMAGE :
            A2mDokuAFrame(as->as_txt, as->as_type);
            break;
        default :
            break;
    }

    return(0);
}



/*NH*/
A2mDokuResetFont()
{
    //if(A2M_CURFONT.af_pos == 1) fprintf(A2M_fdtxt, "</SUP>");
    //if(A2M_CURFONT.af_pos == -1) fprintf(A2M_fdtxt, "</SUB>");
    if(A2M_CURFONT.af_family)   fprintf(A2M_fdtxt, "''");
    if(A2M_CURFONT.af_italic)   fprintf(A2M_fdtxt, "//");
    if(A2M_CURFONT.af_bold)     fprintf(A2M_fdtxt, "**");
    if(A2M_CURFONT.af_underline)fprintf(A2M_fdtxt, "__");
    if(A2M_CURFONT.af_strike)   fprintf(A2M_fdtxt, "</del>");
    if(A2M_CURFONT.af_pos == 1)   fprintf(A2M_fdtxt, "</sup>");
    if(A2M_CURFONT.af_pos == -1)   fprintf(A2M_fdtxt, "</sub>");

    return(0);
}

/*NH*/
A2mDokuPrintAttr(ap, n, fntt, pp)
A2MPAR  *ap;
int     n;
A2MFNT  *fntt;
A2MPPR  *pp;
{
    A2MFNT  fnt, fntp;

    if(A2M_DOKU_ESC == 0) return(0);

    A2mSetFont(ap, ap->ap_strs[n], &fnt);

    if(n) A2mSetFont(ap, ap->ap_strs[n - 1], &fntp);
    else memcpy(&fntp, fntt, sizeof(A2MFNT));

    /* RESET */
    if(fnt.af_family != 'C' && fntp.af_family == 'C') {
        fprintf(A2M_fdtxt, "''");
        A2M_CURFONT.af_family = 0;
    }

    if(fnt.af_family != 'G' && fntp.af_family == 'G') {
        A2M_DOKU_GREEK = 0;
    }

    if(!fnt.af_italic && fntp.af_italic) {
        fprintf(A2M_fdtxt, "//");
        A2M_CURFONT.af_italic = 0;
    }

    if(fnt.af_pos == 0 && fntp.af_pos == -1) {
        fprintf(A2M_fdtxt, "</sub>");
        A2M_CURFONT.af_pos = 0;
    }

    if(fnt.af_pos == 0 && fntp.af_pos == 1) {
        fprintf(A2M_fdtxt, "</sup>");
        A2M_CURFONT.af_pos = 0;
    }

    if(!fnt.af_bold && fntp.af_bold) {
        fprintf(A2M_fdtxt, "**");
        A2M_CURFONT.af_bold = 0;
    }

    if(!fnt.af_underline && fntp.af_underline) {
        fprintf(A2M_fdtxt, "__");
        A2M_CURFONT.af_underline = 0;
    }

    if(!fnt.af_strike && fntp.af_strike) {
        fprintf(A2M_fdtxt, "</del>");
        A2M_CURFONT.af_strike = 0;
    }

    /* SET */
    if(fnt.af_strike && !fntp.af_strike) {
        fprintf(A2M_fdtxt, "<del>");
        A2M_CURFONT.af_strike = 1;
    }

    if(fnt.af_underline && !fntp.af_underline) {
        fprintf(A2M_fdtxt, "__");
        A2M_CURFONT.af_underline = 1;
    }

    if(fnt.af_bold && !fntp.af_bold) {
        fprintf(A2M_fdtxt, "**");
        A2M_CURFONT.af_bold = 1;
    }

    if(fnt.af_italic && !fntp.af_italic) {
        fprintf(A2M_fdtxt, "//");
        A2M_CURFONT.af_italic = 1;
    }

    if(fnt.af_pos == 1 && fntp.af_pos != 1) {
        fprintf(A2M_fdtxt, "<sup>");
        A2M_CURFONT.af_pos = 1;
    }

    if(fnt.af_pos == -1 && fntp.af_pos != -1) {
        fprintf(A2M_fdtxt, "<sub>");
        A2M_CURFONT.af_pos = -1;
    }

    if(fnt.af_family == 'G' && fntp.af_family != 'G') {
        A2M_DOKU_GREEK = 1;
    }

    if(fnt.af_family == 'C' && fntp.af_family != 'C') {
        fprintf(A2M_fdtxt, "''");
        A2M_CURFONT.af_family = 1;
    }



    return(0);
}

/*NH*/
A2mDokuPrintChar(fd, ch)
FILE    *fd;
int     ch;
{
    char    str[10];

    if(A2M_DOKU_UTF8) fprintf(fd, "%s", SCR_OemToUTF8Char(ch));
    else if(A2M_DOKU_OEMTOANSI) fprintf(fd, "%c", SCR_OemToAnsiChar(ch));

    return(0);
}

/*NH*/
A2mDokuPrintFdTxt(fd, txt, esc)
FILE    *fd;
U_ch    *txt;
int     esc;
{
    int     i, oesc, spec = 0;

    oesc  = A2M_DOKU_ESC;
    A2M_DOKU_ESC = esc;

    if(txt == 0) return(0);
    for(i = 0 ; txt[i] ; i++) {
        spec = esc && U_is_in(txt[i], "*/_(){}") && txt[i] == txt[i+1];
        if(spec) fprintf(fd, "%%%%");
        A2mDokuPrintChar(fd, txt[i]);
        if(spec) fprintf(fd, "%%%%");
    }
    A2M_DOKU_ESC = oesc;
    return(0);
}

/*NH*/
A2mDokuPrintTxt(txt)
U_ch    *txt;
{
    A2mDokuPrintFdTxt(A2M_fdtxt, txt, A2M_DOKU_ESC);
}

/*NH*/
A2mDokuGetCenter(A2MPAR *ap)
{
    A2MPAR  tmpap;

    A2mSetParProps(ap->ap_tag, &(tmpap.ap_ppr));
    return(tmpap.ap_ppr.pp_just);
}

/*NH*/
A2mDokuGetNowrap(A2MPAR *ap)
{
    A2MPAR  tmpap;

    if(ap == 0) return(0);
    A2mSetParProps(ap->ap_tag, &(tmpap.ap_ppr));
    return(tmpap.ap_ppr.pp_nowrap);
}

/*NH*/
A2mDokuPrintTbl(at)
A2MTBL  *at;
{
    A2MTC   *tc;
    int     i, j, k, col;
    int     b = at->at_border;
    char    *tdh;

    A2M_NB_TABS = 0;
    if(b < 0) b = A2M_DOKU_TBORDER;
    //for(i = 0 ; i < A2M_LEVEL ; i++) fprintf(A2M_fdtxt, "</UL>\n");
    A2M_LEVEL = 0;
    if(A2M_DOKU_TTITLE == 0) A2mDokuPrintPar(at->at_title, 0, 0);

    //Titre du tableau
    if(A2M_DOKU_TTITLE && at->at_title && at->at_title->ap_strs) {
        fprintf(A2M_fdtxt, "^  ");
        A2mDokuPrintPar(at->at_title, 1, 0);
        fprintf(A2M_fdtxt, "  ");
        for(i = 0 ; i < at->at_nc ; i++) fprintf(A2M_fdtxt, "^");
        fprintf(A2M_fdtxt, "\n");
    }

    // Lignes
    for(i = 0 ; i < at->at_nl ; i++) {
        tdh = (at->at_tls[i].atl_hbf == 1) ? "|" : "^";
        if(at->at_tls[i].atl_type) { // Ligne de sépration : TODO
            //
            continue;
        }
        fprintf(A2M_fdtxt, "%s", tdh); // Séparateur début de la ligne

        // Ligne normale -> boucle sur nc
        for(j = 0 ; j < at->at_nc ; j++) {
            tc = at->at_tls[i].atl_tcs + j;
            if(tc->atc_center == 5) {
                tc->atc_center = A2mDokuGetCenter(tc->atc_par); /* JMP 24-01-2004 */
            }

            switch(tc->atc_center) {
                case 1: // Center
                case 2: // Right
                case 3: // Decimal
                    fprintf(A2M_fdtxt, "  ");
                    break;

                case 4: // Justify
                case 0: // Left
                default:
                    break;
            }

            A2mDokuPrintPar(tc->atc_par, 1, j);
            switch(tc->atc_center) {
                case 0: // Left
                case 1: // Center
                    fprintf(A2M_fdtxt, "  ");
                    break;
                case 3: // Decimal
                case 4: // Justify
                case 2: // Right
                default:
                    break;
            }
            // Ferme la cellule
            for(k = 0 ; k < tc->atc_ncells; k++) fprintf(A2M_fdtxt, "%s", tdh);
            j += tc->atc_ncells - 1;
        }
        fprintf(A2M_fdtxt, "\n");
    }

    A2M_LEVEL = 0;
    A2mDokuPrintPar(at->at_footnote, 0, 0);
    return(0);
}

/*NH*/
A2mDokuPrintGrf(ag)
A2MGRF  *ag;
{
    int         w, h;
    char        buf[256];
    A2MPAR      *prTitle = ag->ag_title;
    extern int  A2M_SWF_NOGIF;

    // Cas GIF
    if(A2M_DOKU_IS_GIF && A2M_SWF_NOGIF == 0) {
        fprintf(A2M_fdtxt, "<TABLE BORDER=0><TR><TD>");
        if(prTitle) A2mDokuPrintPar(prTitle, 1, 0);
        else        fprintf(A2M_fdtxt, "<P>Figure %d</P>\n", A2M_GIF_CNT);
        fprintf(A2M_fdtxt, "</TD></TR><TR><TD>");
        sprintf(buf, "img%d.gif", A2M_GIF_CNT);
        A2mGIF_HTML(ag, buf);
        A2mDokuAFrame(buf, A2M_IMAGEFR);
        fprintf(A2M_fdtxt, "</TD></TR></TABLE>");
        A2M_GIF_CNT++;
        return(0);
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
        w = ag->ag_size[0] * 37.79527559;
        h = ag->ag_size[1] * 37.79527559;

        A2mSWF_HTML(A2M_fdtxt, w, h, ag, A2M_GIF_CNT);
        A2M_GIF_CNT++;
    }

}

/*NH*/
A2mDokuAFrame(filename, type)
char    *filename;
int		type;
{
    char    buf[256], img[256];

    strcpy(buf, filename);
    if(A2M_DOKU_GIFTOBMP && U_index(buf, ".gif") > 0)
        SCR_change_ext(buf, filename, "bmp");
    else if(A2M_DOKU_GIFTOPNG && U_index(buf, ".gif") > 0)
        SCR_change_ext(buf, filename, "png");

	if(type == A2M_IMAGEFR || type == A2M_IMAGEF) { // JMP 27/7/2021
		sprintf(img, "\n\n^  Figure %d  ^\n|  {{  %s:\"%s\"", A2M_DOKU_FIGNB++, A2M_DOKU_IMGDIR, buf); // JMP 22/8/2015
		if(A2M_DOKU_IMGWIDTH) sprintf(img+strlen(img), ":%d", A2M_DOKU_IMGWIDTH);
		sprintf(img+strlen(img), "  }}|");
	}
	else {
		sprintf(img, "\n\n{{%s:\"%s\"", A2M_DOKU_IMGDIR, buf); 
		if(A2M_DOKU_IMGWIDTH) sprintf(img+strlen(img), ":%d", A2M_DOKU_IMGWIDTH);
		sprintf(img+strlen(img), "}}");
	}
		
    fprintf(A2M_fdtxt, "%s\n", img);
    return(0);
}

/*NH*/
A2mDokuAnchor(ref)
char    *ref;
{
    char    buf[128];
    int     aa_nb, i;

    aa_nb = A2mFindAnchor(ref);
    if(aa_nb < 0) return(-1);
    for(i = 0 ; i < A2M_ANCHORS[aa_nb]->aa_nb_aos ; i++)
        A2mDokuPrintObj(A2M_ANCHORS[aa_nb]->aa_aos[i], 0);

    return(0);
}

/*NH*/
A2mDokuFNote(char *txt)
{
    fprintf(A2M_fdtxt, "((");
    A2mDokuPrintTxt(txt);
    fprintf(A2M_fdtxt, "))");
    return(0);
}



/* ================================================================
Fixe la valeur des variables globales avant l'interpétation d'un fichier
a2m et la génération d'un fichier html en se basant sur les définitions du
fichier .ini associé.

Les sections [A2M] et [HTML] du fichier .ini sont interprétées.

&EN filename = nom du fichier .ini

La syntaxe des fichiers a2m est décrite dans un chapitre spécifique.

&TI Fichier a2m.ini
───────────────
Le fichier a2m.ini (ou un autre) contient des paramètres pour
la lecture du fichier a2m etla génration du fichier html. Pour que ces
paramètres soient pris en compte par le programme A2mToHtml(), il faut
appeler la fonction A2mDokuReadIni(filename) avant de lancer la fonction
d'interprétation et d'impression.

&CO
    #include <s_a2m.h>

    A2mDokuReadIni("myfile.ini");
    A2mToHtml("myfile.a2m", "myfile.htm");
&TX

&SA A2mToGdi(), A2mToRtf(), A2mToMif(), A2mToHtml(), A2mPrintError()
==================================================================== */
A2mDokuReadIni(filename)
char    *filename;
{
    A2mReadA2mIni(filename);
    IniReadNumParm(filename, "DOKU", "NUMBERING",      &A2M_NUMBERS);
    IniReadNumParm(filename, "DOKU", "TOC",            &A2M_DOKU_TOCLEVEL);
    return(0);
}


A2mDokuPrintTopicCnt(atp)
A2MTOP  *atp;
{
    int     level, clevel = 0;
    char    topicname[256];

    if(atp) clevel = atp->atp_level;
    if(clevel <= A2M_DOKU_TOCLEVEL) {
        for(level = 0 ; level < clevel ; level++) 
            fprintf(A2M_fdtoc, "  ");  // JMP 11/1/2017
        //fprintf(A2M_fdtoc, "  * [[%s%04d|", A2M_DOKU_PREFX, atp->atp_nb);

        A2mDokuTopicFilename(atp->atp_keyws, topicname, atp->atp_nb);
        if(clevel > 0) fprintf(A2M_fdtoc, "* ");
        if(clevel <= 1) fprintf(A2M_fdtoc, "**");
        fprintf(A2M_fdtoc, "[[%s|", topicname);
        A2M_DOKU_ESC = 0;
        A2mDokuPrintFdTxt(A2M_fdtoc, atp->atp_keyws, 0);
        fprintf(A2M_fdtoc, "]]");
        if(clevel <= 1) fprintf(A2M_fdtoc, "**");
        fprintf(A2M_fdtoc, "\n");
    }
    return(0);
}


















