#include "s_a2m.h"
#include "s_ini.h"

/********************* Parameters *************************/

/* Public */
int     A2M_MD_OEMTOANSI = 1;     /* JMP 16-08-10 */
int     A2M_MD_GIFTOBMP = 0, A2M_MD_GIFTOPNG = 0;
U_ch    *A2M_MD_TITLE = 0; 

/* Private */
FILE    *A2M_fdmd;
int     A2M_MD_TOPIC = -1, A2M_MD_HELP = 0, A2M_MD_LEVEL = 0, A2M_MDREF = 0;;
int     A2M_MD_TOC = 1;
int     A2M_MD_TOCLEVEL, A2M_MD_TOCPLEVEL, A2M_MD_TOCNB;
U_ch    *A2M_MD_TOCTXT;
int     A2M_MD_INTOPIC = 0, A2M_MD_GREEK = 0;
A2MFNT  A2M_MD_CURFONT;
int     A2M_MD_TTITLE = 1;
U_ch**  A2M_MD_TOPICS = 0;
int     A2M_MD_NB_TOPICS= 0;
int     A2M_MD_PREV = 0;

char* A2mMDacatff(char* txt1, char* txt2);

/* ================================================================
Interprète le contenu d'un fichier a2m et génère un fichier .md.

&EN a2mfile = nom du fichier a2m à interpréter
&EN outfile = nom du fichier md à générer

&RT La fonction retourne 0 si le processus s'est bien déroulé, -1 sinon.

La syntaxe des fichiers a2m est décrite dans un chapitre spécifique.


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

&EN int A2M_FONTSIZE = taille par défaut des caractères dans les
    paragraphes (10 pts par défaut)
&EN int A2M_FONTINCR = incrément de taille de caractères (2 par défaut)
&EN int A2M_TFONTSIZE = taille par défaut des caractères dans les
    tableaux (8 pts par défaut)
&EN int A2M_FONTFAMILY = police de caractère par défaut ('H', 'T' ou 'C')
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

&EN int A2M_MD_TCOLOR = 1 (dft) pour impression en couleur des
    tableaux, 0 en B/W
&EN int A2M_MD_TBORDER = épaisseur des cadres des tableaux (1 par dft)
&EN U_ch *A2M_MD_TITLE = titre du fichier html
&EN U_ch *A2M_MD_BODY = contenu du tag <BODY> du fichier HTML
&EN int A2M_MD_LTGT = 0 si les caractères < et > doivent apparaître
    comme tel en HTML (&lt;) et non interprétés comme tag (on mettra 0
    dans ce cas).
&EN int A2M_MD_TTITLE = 1 pour inclure le titre des tableaux dans le tableau
&EN int A2M_MD_AUTOLINK = 0 pour supprimer la création automatique de liens
    par l'analyse des sujets d'aide (accélère le traitement)

&RT
&EN 0 en cas de succès
&EN -1 si le fichier ne peut être ouvert
&EN -2 si les fichiers output n'ont pu être créés


&SA A2mToRtf(), A2mToMif(), A2mToMif(), A2mPrintError()
==================================================================== */

int A2mToMD(char* a2mfile, char* outfile)
{
    A2MFILE *af;
    A2MOBJ  *ao;

    A2mReadTopics(a2mfile);  /* JMP 17-05-99 */
    af = A2mOpen(a2mfile, 0);
    if(af == 0) {
        A2mFreeTopics();  /* JMP 24-05-99 */
        return(-1);
    }

    if(A2mMDInit(outfile)) {
        A2mClose(af);
        A2mFreeTopics();
        return(-2);
    }

    while(1) {
        ao = A2mRead(af);
        if(ao == 0) break;
        A2mMDPrintObj(ao, 1);
    }

    A2mMDEnd(outfile);
    A2mClose(af);
    A2mFreeTopics();  /* JMP 24-05-99 */
    return(0);
}


/*NH*/
int A2mMDInit(char* outfile)
{
    extern int  A2M_CUROBJ;

    A2M_CUROBJ = 1;
    A2M_MD_LEVEL = A2M_MDREF = 0;

    A2M_MD_TOCPLEVEL = A2M_MD_TOCLEVEL = A2M_MD_TOCNB = -1;
    A2M_MD_TOCTXT = 0;
//    A2M_MD_CURFIG = 1;
    A2M_MD_INTOPIC = 0;
    A2M_MD_TOPICS = 0;
    A2M_MD_NB_TOPICS = 0;

    A2M_fdmd = fopen(outfile, "w+");
    if(A2M_fdmd == 0) return(-2);
    
   
    A2mFreeFNotes(); /* JMP 05-09-99 */

    return(0);
}


U_ch *A2mReadFile(char *filename)
{
    FILE    *fd;
    U_ch    *buf = 0;
    int     ch, i;
    long    count;

    fd = fopen(filename, "r");
    if(fd == 0) return(buf);
    
    fseek(fd, 0L, 2);
    count = ftell(fd);
    if(count <= 0) goto fin;
    
    buf = (U_ch *)SCR_malloc(1 + count);
    
    if(buf) {
        fseek(fd, 0L, 0);
        i = 0;
        while(1) {
            ch = getc(fd);
            if(ch == EOF) {
               buf[i] = 0;
               break;
            }
            buf[i++] = ch;            
        }    
	}

fin:
    fclose(fd);
    return(buf);
}


/*NH*/
int A2mMDEnd(char* outfile)
{
    U_ch    *toc, *str_md;

    // Sauve les footnotes
    A2mMDPrintFNotes();
    
    // Ferme le fichier .md et relit son contenu
    fclose(A2M_fdmd);
    str_md = A2mReadFile(outfile);
    
    // Recrée le fichier .md
    A2M_fdmd = fopen(outfile, "w+");
    if(A2M_fdmd == 0) return(-2);
    
    // Header
    fprintf(A2M_fdmd, "<!-- This content was generated by scr4w_amd -->\n\n");
    
    // Sauve la TOC puis la free
    if(A2M_MD_TOC && A2M_MD_NB_TOPICS > 0) {    
        fprintf(A2M_fdmd, "# %s\n\n", "Table of Contents");
        SCR_add_ptr(&A2M_MD_TOPICS, &A2M_MD_NB_TOPICS, 0L);
        toc = SCR_mtovs(A2M_MD_TOPICS, "\n");
        fprintf(A2M_fdmd, "\n\n%s", toc); 
        SCR_free(toc);
        SCR_free_tbl(A2M_MD_TOPICS);
        A2M_MD_NB_TOPICS = 0;
        A2M_MD_TOPICS = NULL;
    }
    
    // Sauve le texte principal
    fprintf(A2M_fdmd, "\n\n%s", str_md); 
    SCR_free(str_md);
    
    fclose(A2M_fdmd);
    A2M_MD_LEVEL = 0;
    A2M_fdmd = 0;
    return(0);
}

/*NH*/
int A2mMDPrintObj(A2MOBJ* ao, int del)
{
    char        msg[80];
    extern int  A2M_CUROBJ;

    sprintf(msg, "Printing Object %d", A2M_CUROBJ++);
    A2mMessage(msg);
    if(ao->ao_type == A2M_PAR)
        A2mMDPrintPar((A2MPAR *)ao->ao_ptr, 0, 0);

    if(ao->ao_type == A2M_TOPIC)
        A2mMDPrintTopic((A2MTOP *)ao->ao_ptr, 0);

    if(ao->ao_type == A2M_TOPICALIAS) /* JMP 09-09-99 */
        A2mMDPrintTopic((A2MTOP *)ao->ao_ptr, 1);

    if(ao->ao_type == A2M_TBL)
        A2mMDPrintTbl((A2MTBL *)ao->ao_ptr);

    if(ao->ao_type == A2M_GRF)
        A2mMDPrintGrf((A2MGRF *)ao->ao_ptr);

    if(del) A2mFreeObj(ao);
    return(0);
}

/*NH*/
int A2mMDPrintTopic(A2MTOP *atp, int alias)
{
//    char    buf[256];
//    int     i;
//    
    return(0);
//    if(atp == 0) return(0);
//    if(A2M_fdmd != NULL) {
//        A2M_MD_LEVEL = 0;
//        fclose(A2M_fdmd);
//        A2M_fdmd = NULL;
//    }
//
//    sprintf(buf, "T%04d.htm", atp->atp_nb);
//    A2M_fdmd = fopen(buf, "w+");
//    return(0);
}

/*NH*/
int A2mMDCalcBlc(A2MPAR* ap)
{
    int     i;
    U_ch    *txt;

    if(ap->ap_strs == 0 || ap->ap_strs[0] == 0) return(0);

    txt = ap->ap_strs[0]->as_txt;
    if(txt == 0) return(0);
    for(i = 0 ; txt[i] == ' ' ; i++);
    if(txt[i] == 0) return(0);
    /*!!!*/
    return(i); 
}

U_ch* A2mMDCalcResetAttrSTATIC()
{
    static U_ch buf[80];

    /* RESET */
    buf[0] = 0;
    if(A2M_MD_CURFONT.af_family) {
        sprintf(buf + strlen(buf), "%s", "`");
        A2M_MD_CURFONT.af_family = 0;
    }    
      
    if(A2M_MD_CURFONT.af_pos == -1) {
        sprintf(buf + strlen(buf), "%s", "</sub>");
        A2M_MD_CURFONT.af_pos = 0;
    }    

    if(A2M_MD_CURFONT.af_pos == 1) {
        sprintf(buf + strlen(buf), "%s", "</sup>");
        A2M_MD_CURFONT.af_pos = 0;
    }    

    if(A2M_MD_CURFONT.af_italic) {
        sprintf(buf + strlen(buf), "%s", "*");
        A2M_MD_CURFONT.af_italic = 0;
    }    

    if(A2M_MD_CURFONT.af_bold) {
        sprintf(buf + strlen(buf), "%s", "**");
        A2M_MD_CURFONT.af_bold = 0;
    }    

    if(A2M_MD_CURFONT.af_underline) {
        sprintf(buf + strlen(buf), "%s", "*");
        A2M_MD_CURFONT.af_underline = 0;
    }    

    if(A2M_MD_CURFONT.af_strike) {
        sprintf(buf + strlen(buf), "%s", "~~");
        A2M_MD_CURFONT.af_strike = 0;
    }    
    
    return(buf);
}

/*NH*/
U_ch* A2mMDCalcAttrSTATIC(A2MPAR* ap, int n, A2MFNT* fntt)
{
    A2MFNT  fnt, fntp;
    static U_ch buf[80];

    A2mSetFont(ap, ap->ap_strs[n], &fnt);

    if(n) A2mSetFont(ap, ap->ap_strs[n - 1], &fntp);
    else memcpy(&fntp, fntt, sizeof(A2MFNT));

    /* RESET */
    buf[0] = 0;
    if(fnt.af_family != 'C' && fntp.af_family == 'C')
    {
        sprintf(buf + strlen(buf), "%s", "`");
        A2M_MD_CURFONT.af_family = 0;
    }

    if(fnt.af_pos != -1 && fntp.af_pos == -1)
    {
        sprintf(buf + strlen(buf), "%s", "</sub>");
        A2M_MD_CURFONT.af_pos = 0;
    }

    if(fnt.af_pos != 1 && fntp.af_pos == 1)
    {
        sprintf(buf + strlen(buf), "%s", "</sup>");
        A2M_MD_CURFONT.af_pos = 0;
    }

    if(fnt.af_family != 'G' && fntp.af_family == 'G')
    {
        A2M_MD_GREEK = 0;
    }

    if(!fnt.af_italic && fntp.af_italic)
    {
        sprintf(buf + strlen(buf), "%s", "*");
        A2M_MD_CURFONT.af_italic = 0;
    }

    if(fnt.af_pos == 0 && fntp.af_pos == -1)
    {
        A2M_MD_CURFONT.af_pos = 0;
    }

    if(fnt.af_pos == 0 && fntp.af_pos == 1)
    {
        A2M_MD_CURFONT.af_pos = 0;
    }

    if(!fnt.af_bold && fntp.af_bold)
    {
        sprintf(buf + strlen(buf), "%s", "**");
        A2M_MD_CURFONT.af_bold = 0;
    }

    if(!fnt.af_underline && fntp.af_underline)
    {
        sprintf(buf + strlen(buf), "%s", "*");
        A2M_MD_CURFONT.af_underline = 0;
    }

    if(!fnt.af_strike && fntp.af_strike)
    {
        sprintf(buf + strlen(buf), "%s", "~~");
        A2M_MD_CURFONT.af_strike = 0;
    }


    /* SET */
    if(fnt.af_family == 'C' && fntp.af_family != 'C')
    {
        sprintf(buf + strlen(buf), "%s", "`");
        A2M_MD_CURFONT.af_family = 1;
        return(buf); // On ne set rien d'autre dans ce cas car si en mode "code" les autres attributs ne sont pas traduits (** reste ** par ex.)
    }

    if(fnt.af_pos == -1 && fntp.af_pos != -1)
    {
        sprintf(buf + strlen(buf), "%s", "<sub>");
        A2M_MD_CURFONT.af_pos = -1;
    }

    if(fnt.af_pos == 1 && fntp.af_pos != 1)
    {
        sprintf(buf + strlen(buf), "%s", "<sup>");
        A2M_MD_CURFONT.af_pos = 1;
    }
    
    if(fnt.af_strike && !fntp.af_strike)
    {
        sprintf(buf + strlen(buf), "%s", "~~");
        A2M_MD_CURFONT.af_strike = 1;
    }

    if(fnt.af_underline && !fntp.af_underline)
    {
        sprintf(buf + strlen(buf), "%s", "*");
        A2M_MD_CURFONT.af_underline = 1;
    }

    if(fnt.af_bold && !fntp.af_bold)
    {
        sprintf(buf + strlen(buf), "%s", "**");
        A2M_MD_CURFONT.af_bold = 1;
    }

    if(fnt.af_italic && !fntp.af_italic)
    {
        sprintf(buf + strlen(buf), "%s", "*");
        A2M_MD_CURFONT.af_italic = 1;
    }


    if(fnt.af_family == 'G' && fntp.af_family != 'G')
    {
        A2M_MD_GREEK = 1;
    }

    return(buf);
}


/*NH*/
int A2mMDPrintAttr(A2MPAR* ap, int n, A2MFNT* fntt)
{
    char*   txt;
    
    txt = A2mMDCalcAttrSTATIC(ap, n, fntt); // buffer static
    fprintf(A2M_fdmd, "%s", txt);
    // SCR_free(txt);
    return(0);
}

/**
 *  @brief Retourne le caractère escapé si nécessaire
 *  
 *  @param [in] ch   int    le caractère à escaper
 *  @param [in] code int    1 si on est dans une section code et qu'il ne faut pas l'escaper
 *  @return          char*  buffer STATIC avec le resultat
 *  
 */
U_ch* A2mMDCalcCharSTATIC(int ch, int code)
{
    static U_ch    buf[10];

    if(A2M_MD_OEMTOANSI) ch = SCR_OemToAnsiChar(ch);
    //if(U_is_in(ch, "\\`*_{}[]<>()#+-.!|"))
    if(code == 0 && U_is_in(ch, "\\`*_{}[]#+-!|"))
        sprintf(buf, "\\%c", ch);
    else 
        sprintf(buf, "%c", ch);
    
    return(buf);
}

/*NH*/
char* A2mMDAFrame(char* filename)
{
    char    buf[256], buf2[256];

    strcpy(buf, filename);

    if(A2M_MD_GIFTOBMP && U_index(buf, ".gif") > 0)
        SCR_change_ext(buf, filename, "bmp");
    if(A2M_MD_GIFTOPNG && U_index(buf, ".gif") > 0) /* JMP 23-08-2015 */
        SCR_change_ext(buf, filename, "png");         /* JMP 23-08-2015 */

    sprintf(buf2, "![](%s)", buf);
    // ![Tux, the Linux mascot](/assets/images/tux.png)
    return(SCR_stracpy(buf2));
}

char *A2mMDFNote(char* ref)
{
    char    *txt, buf[80];

    A2mAddFNote(ref);
    sprintf(buf, "[^%d] ", A2M_NB_FNOTES);
    txt = SCR_stracpy(buf);
    return(txt);
}

/*NH*/
A2mMDPrintFNotes()
{
    int     i;

    if(A2M_NB_FNOTES == 0) return(0);
    
    fprintf(A2M_fdmd, "\n\n");
    for(i = 0 ; i < A2M_NB_FNOTES ; i++) {
        fprintf(A2M_fdmd, "[^%d]: %s\n", i + 1, A2M_FNOTES[i]);
    }

    return(0);
}

U_ch* A2mMDCalcStr(A2MPAR* ap, int n, A2MFNT  *fntt, int tbl, int code)
{
    A2MSTR          *as = ap->ap_strs[n];
    int             i;
    char            buf[256];
    U_ch            *txt = 0;
    static  char    *lasthref;
    
    if(fntt) {
        txt = SCR_strafcat(txt, A2mMDCalcAttrSTATIC(ap, n, fntt));
    }    
    if(A2M_NUMBERS && n == 0 && ap->ap_ppr.pp_number >= 1) {
        A2mCalcNumbering(ap->ap_ppr.pp_number - 1, buf);
        for(i = 0 ; buf[i] ; i++) {
            txt = SCR_strafcat(txt, A2mMDCalcCharSTATIC(buf[i], code));
        }    
    }

    switch(as->as_type) {
    case A2M_TEXT :
        for(i = 0 ; as->as_txt[i] ; i++) {
            txt = SCR_strafcat(txt, A2mMDCalcCharSTATIC(as->as_txt[i], code || A2M_MD_CURFONT.af_family == 1)); 
                     // code || A2M_MD_CURFONT.af_family == 1 : Pas de translation des char spéciaux si dans un PRE ou dans un ~c..~C
        }        
        return(txt);

    case A2M_IMAGEFR:
    case A2M_IMAGER:
    case A2M_IMAGEF:
    case A2M_IMAGE :
        txt = A2mMDacatff(txt, A2mMDAFrame(as->as_txt));
        return(txt);
    case A2M_ANCHOR :
        A2mMDAnchor(as->as_txt);
        return(txt);
    case A2M_FNOTE :
        txt = A2mMDacatff(txt, A2mMDFNote(as->as_txt));
        return(txt);
    case A2M_BHREF:
        lasthref = SCR_stracpy(as->as_txt);
        return(SCR_strafcat(txt, "["));
    case A2M_EHREF:
        txt = SCR_strafcat(txt, "](");
        if(lasthref) {
            txt = SCR_strafcat(txt, lasthref);
            SCR_free(lasthref);
            lasthref = 0;
        }    
        txt = SCR_strafcat(txt, ")");
        return(txt);
    case A2M_NEWLINE : 
        if(code)
            txt = SCR_strafcat(txt, "\n");
        else
            txt = SCR_strafcat(txt, "\n\n");
        return(txt);
    case A2M_TAB :
        txt = SCR_strafcat(txt, " ");
        return(txt);
    case A2M_TOPIC  :    
        break;
        if(A2M_MD_HELP) {
            A2M_MD_TOPIC = atoi(as->as_txt);
            sprintf(buf, "[T%04d.htm]", A2M_MD_TOPIC);
            txt = SCR_strafcat(txt, buf);
        }
        break;
    case A2M_ETOPIC :                              /* JMP 29-04-99 */
        break;
        if(A2M_MD_HELP) {
            //fprintf(A2M_fdmd, "");
            A2M_MD_TOPIC = -1;
        }
        //else
        //    fprintf(A2M_fdmd, "");
        break;
    }

    return(txt);
}
 

U_ch* A2mMDCalcStrs(A2MPAR* ap, A2MFNT* fnt, int tbl, int code) 
{
    int     i;
    U_ch*   txt = 0;
    
    for(i = 0 ; ap->ap_strs[i] ; i++) {
        txt = A2mMDacatff(txt, A2mMDCalcStr(ap, i, fnt, tbl, code));
    }
    txt = SCR_strafcat(txt, A2mMDCalcResetAttrSTATIC());
    
    return(txt);
}

/*NH*/
U_ch *A2mMDCalcPar(A2MPAR* ap, int tbl, int tcol)
{
    int     i, level;
    char    tag[32];
    A2MFNT  fntd;
    U_ch    *txt = 0, *tit, *bullets = 0, buf[80];

    if(ap == 0 || ap->ap_strs == 0) {
        return(SCR_stracpy(""));
    }
    A2mSetParProps(ap->ap_tag, &(ap->ap_ppr));
    A2mMDFindTag(&(ap->ap_ppr), tag);

    level = ap->ap_ppr.pp_lmarg - 16;
    if(level < 0) level = 0;
    else level = level / 8;

    if(level > 6) level = 6; /* JMP 11-09-99 */
    A2M_MD_LEVEL = level;

    A2mMDCalcTagFnt(tag, &fntd);
    memset(&A2M_MD_CURFONT, 0, sizeof(A2MFNT));

    if(tbl == 0 || ap->ap_tag) {
        // Ajouter une ligne blanche en fin de listes LI
        if(A2M_MD_PREV == 'L' && tag[0] != 'L') 
            txt = SCR_strafcat(txt, "\n");
        
        if(tag[0] == 'H') {
            bullets = 0; // Début de la ligne "   - "
            for(i = 0 ; i < atol(1 + tag) ; i++) { 
                txt     = SCR_strafcat(txt, "#");
                if(i > 0) bullets = SCR_strafcat(bullets, "  ");
            }    
            
            tit = A2mMDCalcStrs(ap, &fntd, tbl, 0);
            
            // bullets = pour la TOC à la fin
            bullets = SCR_strafcat(bullets, "- [");
            bullets = SCR_strafcat(bullets, tit);
            sprintf(buf, "](#T%d)", A2M_MD_NB_TOPICS + 1);
            bullets = SCR_strafcat(bullets, buf);    
            SCR_add_ptr(&A2M_MD_TOPICS, &A2M_MD_NB_TOPICS, bullets);
            SCR_free(bullets);
            
            // Partie principale avec ou sans link selon A2M_MD_HELP
            txt = SCR_strafcat(txt, " ");
            txt = A2mMDacatff(txt, tit); // Frees tit
            if(!tbl) {
                if(A2M_MD_TOC)
                    sprintf(buf, " {#T%d}\n\n", A2M_MD_NB_TOPICS);
                else
                    sprintf(buf, "\n\n");
                txt = SCR_strafcat(txt, buf);
            }
            A2M_MD_PREV = 'H';
        }   
        
        else if(tag[0] == 'L') {
            txt = SCR_strafcat(txt, "- ");
            txt = A2mMDacatff(txt, A2mMDCalcStrs(ap, &fntd, tbl, 0));
            txt = SCR_strafcat(txt, "\n");
            A2M_MD_PREV = 'L';
        }    

        else if(strcmp(tag, "PRE") == 0) {
            txt = SCR_strafcat(txt, "```\n");
            txt = A2mMDacatff(txt, A2mMDCalcStrs(ap, NULL, tbl, 1));
            txt = SCR_strafcat(txt, "\n```");
            if(!tbl) txt = SCR_strafcat(txt, "\n\n");
            A2M_MD_PREV = 'C';

        }    
        else {
            txt = A2mMDacatff(txt, A2mMDCalcStrs(ap, &fntd, tbl, 0));
            if(!tbl) txt = SCR_strafcat(txt, "\n\n");
            A2M_MD_PREV = 'P';

        }
    }
    
    return(txt);
}

/*NH*/
int A2mMDPrintPar(A2MPAR* ap, int tbl, int tcol)
{
    char*   txt;
    
    txt = A2mMDCalcPar(ap, tbl, tcol);
    fprintf(A2M_fdmd, "%s", txt);
    SCR_free(txt);
    
    return(0);
}

/*NH*/
int A2mMDFindTag(A2MPPR* pp, char* tag)
{
    A2M_MD_GREEK = 0;
    if(pp->pp_html)                      sprintf(tag, "H%d", pp->pp_html);
    else if(pp->pp_bullet)               strcpy(tag, "LI");
    else if(pp->pp_fnt.af_family == 'C') strcpy(tag, "PRE");
    else if(pp->pp_fnt.af_family == 'G') A2M_MD_GREEK = 1;
    else strcpy(tag, "P");

    return(0);
}


/*NH*/
int A2mMDCalcTagFnt(char* tag, A2MFNT* fnt)
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


int A2mMDGetCenter(A2MPAR *ap)
{
    A2MPAR  tmpap;

    A2mSetParProps(ap->ap_tag, &(tmpap.ap_ppr));
    return(tmpap.ap_ppr.pp_just);
}


/*NH*/
int A2mMDPrintTbl(A2MTBL* at)
{
    A2MTC   *tc;
    int     i, j, firstline = 1;
    int     b = at->at_border;

    A2M_MD_LEVEL = 0;

    // TITLE
    if(A2M_MD_TTITLE && at->at_title && at->at_title->ap_strs) {
        // TH
        fprintf(A2M_fdmd, "\n**");
        A2mMDPrintPar(at->at_title, 1, 0);
        fprintf(A2M_fdmd, "**\n\n");
    }

    // BODY
    for(i = 0 ; i < at->at_nl ; i++) {
        // <TD> or <TH> ? + Classe par défaut en fonction thead ou pas
        
        // Ligne dans le tableau 
        if(at->at_tls[i].atl_type != 0) continue; // .tl, .theader, ...

        // Boucle sur les cells
        for(j = 0 ; j < at->at_nc ; j++) {
            tc = at->at_tls[i].atl_tcs + j;
            fprintf(A2M_fdmd, "|");
            A2mMDPrintPar(tc->atc_par, 1, j);
        }
        fprintf(A2M_fdmd, "|\n");
        
        // Si premiçre ligne => |----|
        if(firstline == 1) {
            for(j = 0 ; j < at->at_nc ; j++) {
                tc = at->at_tls[i].atl_tcs + j;
                switch(tc->atc_center) {
                case 1:
                    fprintf(A2M_fdmd, "|:----:");
                    break;
                case 2:
                case 3:
                    fprintf(A2M_fdmd, "|----:");
                    break;
                default :
                    fprintf(A2M_fdmd, "|:---");
                    break;
                }
            }
            fprintf(A2M_fdmd, "|\n"); 
            firstline = 0;
        }
    }
    fprintf(A2M_fdmd, "\n");        
    A2M_MD_LEVEL = 0;
    return(0);
}

/*NH*/
int A2mMDPrintGrf(A2MGRF* ag)
{
    return(0);
}


/*NH*/
int A2mMDAnchor(char* ref)
{
    int     aa_nb, i;

    aa_nb = A2mFindAnchor(ref);
    if(aa_nb < 0) return(-1);
    for(i = 0 ; i < A2M_ANCHORS[aa_nb]->aa_nb_aos ; i++)
        A2mMDPrintObj(A2M_ANCHORS[aa_nb]->aa_aos[i], 0);

    return(0);
}


/* UTILS */

/**
 *  @brief Concatenates txt1 and txt2 in a new allocated buffer, then frees txt1 and txt2.
 *  
 *  @param [in, out] txt1  char* Any non NULL buffer (freed
 *  @param [in, out] txt2  char* Any non NULL buffer (freed
 *  @return                char* allocated buffer = concatenation of txt1 and txt2    
 *  
 */
char* A2mMDacatff(char* txt1, char* txt2)
{
    char *tmp;
    
    tmp = SCR_strafcat(txt1, txt2);
    SCR_free(txt2);
    return(tmp);
} 
            


