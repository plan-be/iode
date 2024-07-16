#include "s_a2m.h"
#ifndef UNIX
#include <io.h>
#endif

extern int A2MGRFBARCURRENT;
extern int A2MGRFBARNB;

/* Param�tre g�n�ral */

/********** PARAMETRES ************/

/* Public */
int    A2M_FRM_GIFTOBMP = 0; /* JMP 01-04-03 */
int    A2M_FRM_GIFTOPNG = 0; /* JMP 23-08-2015 */
int    A2M_FRM_TCOLOR = 1;
int    A2M_FRM_TCOL1 = 60; /* mm */
int    A2M_FRM_TCOLN = 15; /* mm */
int    A2M_FRM_TSPLIT = 1, A2M_FRM_TWIDTH = 165, A2M_FRM_TBORDER = 2;
int    A2M_FRM_REF = 1, A2M_FRM_HLINES = 0, A2M_FRM_VLINES = 0;
int    A2M_FRM_TTITLE= 1; /* JMP 30-07-98 */
double A2M_FRM_BRECT[2] = {9.33, 6.4}; /* cm */  /* JMP 25-02-99 */
int    A2M_FRM_DPI = 120;               /* JMP 18-02-99 */
double A2M_FRM_PENWIDTH = 2.0;          /* JMP 18-02-99 */
//char   *A2M_FRM_BASEFONT = "Trebuchet MS"; /* JMP 21-11-2011 */
char   *A2M_FRM_BASEFONT = "Arial"; /* JMP 05-01-2017 */

/* Private */
FILE    *A2M_fdfrm = 0, *A2M_fdtbl = 0, *A2M_fdfrw = 0;
int     A2M_TBLID = 0, A2M_FRAMEID = 0;
char    *A2M_FRCOLORS[] = {
    "Black",
    "White",
    "Red",
    "Green",
    "Blue",
    "Cyan",
    "Magenta",
    "Yellow",
    "c1", "c2", "c3", "c5", "c6", "c7", "c8", "c9"
};

/*NH*/
#ifdef SCRCPP
/*NH*/
FILE *A2mFrOpenFile(
    char    *outfile,
    char    *ext
)
#else
FILE *A2mFrOpenFile(outfile, ext)
char    *outfile, *ext;
#endif
{
    FILE    *fd;
    char    buf[256];

    strcpy(buf, outfile);
    if(ext) SCR_change_ext(buf, outfile, ext);
    fd = fopen(buf, "w+");
    return(fd);
}

/*NH*/
A2mFrUnlinkFiles(outfile)
char    *outfile;
{
    char    buf[256];

    SCR_change_ext(buf, outfile, "frt");
    
#ifdef __GNUC__
    unlink(buf);
#else
    _unlink(buf);
#endif

    SCR_change_ext(buf, outfile, "frw");

#ifdef __GNUC__
    unlink(buf);
#else
    _unlink(buf);
#endif
    
    return(0);
}

/* ================================================================
Interpr�te le contenu d'un fichier a2m et g�n�re un fichier .mif (Frame
Maker).

&EN a2mfile = nom du fichier a2m � interpr�ter
&EN outfile = nom du fichier mif � g�n�rer

&RT La fonction retourne 0 si le processus s'est bien d�roul�, -1 sinon.

La syntaxe des fichiers a2m est d�crite dans un chapitre sp�cifique.

&TI Fichier a2m.ini
���������������
Le fichier a2m.ini (ou un autre) contient des param�tres pour
l'impression et la lecture du fichier a2m. Pour que ces param�tres
soient pris en compte par le programme A2mToMif(), il faut appeler la
fonction A2mFrmReadIni(filename) avant de lancer la fonction
d'interpr�tation et d'impression.

&CO
    #include <s_a2m.h>

    A2mFrmReadIni("myfile.ini");
    A2mToMif("myfile", "test.mif");
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
Ces variables sont d�finies dans la section [MAKER] du fichier .ini.

&EN int A2M_FONTSIZE = taille par d�faut des caract�res dans les
    paragraphes (10 pts par d�faut)
&EN int A2M_FONTINCR = incr�ment de taille de caract�res (2 par d�faut)
&EN int A2M_FONTFAMILY = police de caract�re par d�faut ('H', 'T' ou 'C')
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

&EN int A2M_FRM_TCOLOR = 1 (dft) pour impression en couleur des
    tableaux, 0 en B/W
&EN int A2M_FRM_TBORDER = �paisseur des cadres des tableaux (1 par dft)
&EN int A2M_FRM_TCOL1 = largeur de la colonne 1 des tableaux en mm
	(dft 60)
&EN int A2M_FRM_TCOLN = largeur des autres colonnes des tableaux en mm
	(dft 15)
&EN int A2M_FRM_HLINES = 1 pour des lignes horizontales dans les
    tableaux (dft 0)
&EN int A2M_FRM_VLINES = 1 pour des lignes verticales dans les tableaux
    (dft 0)
&EN int A2M_FRM_REF = 1 g�n�rer des r�f�rences dans le texte associ� aux
    images, 0 sinon (dft 1)
&EN int A2M_FRM_TTITLE = 1 pour inclure le titre des tableaux dans le tableau

&RT
&EN 0 en cas de succ�s
&EN -1 si le fichier ne peut �tre ouvert
&EN -2 si les fichiers output n'ont p� �tre cr��s

&SA A2mFrmReadIni(), A2mToRtf(), A2mToGdi(), A2mToHtml(), A2mPrintError()
==================================================================== */

A2mToMif(a2mfile, outfile)
char    *a2mfile, *outfile;
{
    A2MFILE *af;
    A2MOBJ  *ao;

    af = A2mOpen(a2mfile, 0);
    if(af == 0) return(-1);

    if(A2mFrInit(outfile)) {
        A2mClose(af);
        return(-2);
    }

    while(1) {
        ao = A2mRead(af);
        if(ao == 0) break;
        A2mFrPrintObj(ao);
    }

    A2mFrEnd(outfile);
    A2mClose(af);
    return(0);
}

/*NH*/
A2mFrInit(outfile)
char    *outfile;
{
    extern int A2M_CUROBJ;

    A2M_fdfrm = A2M_fdtbl = A2M_fdfrw = 0;
    A2M_TBLID = A2M_FRAMEID = 1;
    A2M_CUROBJ = 1;
    A2MGRFOBJID = 1; // JMP 06/07/2022

    A2M_fdfrm = A2mFrOpenFile(outfile, 0L);
    A2M_fdtbl = A2mFrOpenFile(outfile, "frt");
    A2M_fdfrw = A2mFrOpenFile(outfile, "frw");
    if(A2M_fdtbl == 0 || A2M_fdfrw == 0 || A2M_fdfrm == 0) return(-2);

    fprintf(A2M_fdfrm, "<MIFFile 3.0> # created by SCR4/AL1\n");
    A2mFrFontCatalog();
    A2mFrColorCatalog();
    fprintf(A2M_fdfrm, "<AFrames\n");
    return(0);
}

/*NH*/
A2mFrPrintObj(ao)
A2MOBJ  *ao;
{
    char        msg[80];
    extern int  A2M_CUROBJ;

    sprintf(msg, "Printing Object %d", A2M_CUROBJ++);
    A2mMessage(msg);

    if(ao->ao_type == A2M_PAR)
        A2mFrPrintPar((A2MPAR *)ao->ao_ptr, 0, 0, 0, 0, -1, 0L);

    if(ao->ao_type == A2M_TBL)
        A2mFrPrintTbl((A2MTBL *)ao->ao_ptr);

    if(ao->ao_type == A2M_GRF)
        A2mFrPrintGrf((A2MGRF *)ao->ao_ptr);

    if(ao->ao_type == A2M_PAGE)
        A2mFrPrintNewPage((A2MPAGE *)ao->ao_ptr);

    A2mFreeObj(ao);
    return(0);
}

/*NH*/
A2mFrEnd(outfile)
char    *outfile;
{
    fclose(A2M_fdfrw);
    fclose(A2M_fdtbl);
    A2M_fdtbl = A2M_fdfrw = 0;

    fprintf(A2M_fdfrm, ">\n");
    A2mFrCatalogPar();
    fprintf(A2M_fdfrm, "<Tbls\n");
    A2mFrCatFile(A2M_fdfrm, outfile, "frt");
    fprintf(A2M_fdfrm, ">\n");
    fprintf(A2M_fdfrm, "<TextFlow\n");
    A2mFrCatFile(A2M_fdfrm, outfile, "frw");
    fprintf(A2M_fdfrm, ">\n"); /* JMP 04-12-06 */
    fclose(A2M_fdfrm);
    A2M_fdfrm = 0;

    if(A2M_fdtbl) fclose(A2M_fdtbl);
    if(A2M_fdfrw) fclose(A2M_fdfrw);
    if(A2M_fdfrm) fclose(A2M_fdfrm);
    A2M_fdfrm = A2M_fdtbl = A2M_fdfrw = 0;
    A2mFrUnlinkFiles(outfile);
    return(0);
}

/*NH*/
A2mFrPrintNewPage(A2MPAGE *apg)
{
    fprintf(A2M_fdfrw, " <Para <Pgf <PgfTag `newpage'>> <ParaLine <String ` '> >>\n");
    /* JMP 17-01-02 */
}
/*NH*/
A2mFrCalcBlc(ap)
A2MPAR  *ap;
{
    /* GB 03/05/04 */
    int     i;
    U_ch    *txt;

    if(ap->ap_strs == 0 || ap->ap_strs[0] == 0) return(0);
    txt = ap->ap_strs[0]->as_txt;
    if(txt == 0) return(0);
    for(i = 0 ; txt[i] == ' ' ; i++);
    if(txt[i] == 0) return(0);
    return(i); /* JMP 07-04-08 */

    U_ljust_text(ap->ap_strs[0]->as_txt);
    SCR_strip(ap->ap_strs[0]->as_txt);
    return(i);
}

/*NH*/
A2mFrPrintPar(ap, dest, shift, tbl, grf, just, var)
A2MPAR  *ap;
int     dest, shift, tbl, grf, just;
U_ch    *var;
{
    FILE    *fd;
    int     i;
    //int     blc;

    if(ap == 0 || ap->ap_strs == 0) return(0);
    A2mSetParProps(ap->ap_tag, &(ap->ap_ppr));
    if(dest) fd = A2M_fdtbl;
    else     fd = A2M_fdfrw;

    if(shift) fprintf(fd, "     ");
    fprintf(fd, " <Para <Pgf <PgfTag `%s'> ", ap->ap_tag);

    /* SUPPR JMP 7/4/2008 pb frame et html
        blc = A2mFrCalcBlc(ap);
    */
    /* GB 03/05/04 */
    /*    if(blc != 0) fprintf(fd, " <PgfLIndent  %d pt>  <PgfFIndent  %d pt> ", 2* blc, 2* blc);
    SUPPR JMP 7/4/2008 pb frame et html
    */
    if(just >= 0 && just != ap->ap_ppr.pp_just) {  /* JMP 14-04-00 */
        fprintf(fd, "<PgfAlignment ");
        switch(just) {
            case 1 :
                fprintf(fd, "Center>");
                break;
            case 2 :
                fprintf(fd, "Right>");
                break;
            case 3 :
                fprintf(fd, "Left>");
                break; /* JMP 14-04-00 */
            case 4 :
                fprintf(fd, "LeftRight>");
                break;
            default:
                fprintf(fd, "Left>");
                break;
        }
    }

    fprintf(fd, "> <ParaLine ");
    for(i = 0 ; ap->ap_strs[i] ; i++)
        A2mFrPrintStr(fd, ap, i);

    if(var) fprintf(fd, "\n    <Variable <VariableName `%s'>>\n", var);

    if(shift) fprintf(fd, "     ");
    fprintf(fd, " >");
    if(tbl) fprintf(fd, " <ParaLine <ATbl %d>>\n", tbl);
    if(grf) fprintf(fd, " <ParaLine <AFrame %d>>\n", grf);
    fprintf(fd, " >\n");
    return(0);

}

/*NH*/
A2mFrPrintStr(fd, ap, n)
FILE    *fd;
A2MPAR  *ap;
int     n;
{
    A2MSTR  *as = ap->ap_strs[n];
    int     i;

    A2mFrPrintAttr(fd, ap, n);

    switch(as->as_type) {
        case A2M_TEXT :
            fprintf(fd, " <String `");
            for(i = 0 ; as->as_txt[i] ; i++) A2mFrPrintChar(fd, as->as_txt[i]);
            fprintf(fd, "'>");
            return(0);

        case A2M_IMAGEFR:
        case A2M_IMAGER:
        case A2M_IMAGEF:
        case A2M_IMAGE :
        case A2M_ANCHOR :  /* JMP 18-05-11 */
            A2mFrAFrame(fd, as->as_txt, as->as_type);
            return(0);
        case A2M_NEWLINE  :
            fprintf(fd, " > <ParaLine <Char HardReturn>> <ParaLine ");
            return(0);
        case A2M_TAB :
            fprintf(fd, " <Char Tab>");
            return(0);
        case A2M_BHREF:
            fprintf(fd, "<Marker <MType 8> <MTypeName `Hypertext'> <MText `message URL %s'> >", as->as_txt);
            return(0);
        case A2M_EHREF:
            return(0);
    }
    return(0);
}

/*NH*/
A2mFrPrintChar(fd, ch)
FILE    *fd;
int     ch;
{
    U_ch    *str;

    switch(ch) {
        case '('    :
            str = "(";
            break;
        case ')'    :
            str = ")";
            break;
        case '{'    :
            str = "{";
            break;
        case '}'    :
            str = "}";
            break;
        case '<'    :
            str = "<";
            break;
        case '>'    :
            str = "\\x3E ";
            break;
        case '`'    :
            str = "\\xD4 ";
            break;
        case '\''   :
            str = "\\xD5 ";
            break;
        case '\\'   :
            str = "\\\\";
            break;
        case 21     :
            str = "\\xa4 ";
            break;  /* Par */
        case 129    :
            str = "\\x9f ";
            break; /* u trema */
        case 130    :
            str = "\\x8e ";
            break; /* e aigu */
        case 131    :
            str = "\\x89 ";
            break; /* a circomflexe */
        case 132    :
            str = "\\x8a ";
            break; /* a trema */
        case 133    :
            str = "\\x88 ";
            break; /* a grave */
        case 135    :
            str = "\\x8d ";
            break; /* c cedille */
        case 136    :
            str = "\\x90 ";
            break; /* e circomflexe */
        case 137    :
            str = "\\x91 ";
            break; /* e trema */
        case 138    :
            str = "\\x8f ";
            break; /* e grave */
        case 139    :
            str = "\\x95 ";
            break; /* i trema */
        case 140    :
            str = "\\x94 ";
            break; /* i circomflexe */
        case 142    :
            str = "\\x80 ";
            break; /* A trema */
        case 147    :
            str = "\\x99 ";
            break; /* o circomfl */
        case 148    :
            str = "\\x9a ";
            break; /* o trema */
        case 150    :
            str = "\\x9e ";
            break; /* u circomflexe */
        case 151    :
            str = "\\x9d ";
            break; /* u grave */
        case 153    :
            str = "\\x85 ";
            break; /* O trema */
        case 154    :
            str = "\\x86 ";
            break; /* U trema */
        case 167    :
            str = "\\xbc ";
            break; /* number  */
        case 211    :
            str = "\\xe8 ";
            break; /* E trema */
        case 225    :
            str = "\\xa7 ";
            break; /* ss allemand */
        case 238    :
            str = "\\xd5";
            break;  /* quote  */
        case 239    :
            str = "\\xd2";
            break;  /* double quote left  */
        case 240    :
            str = "\\xd3";
            break;  /* double quote right */
        case 248    :
            str = "\\xfb ";
            break; /* number  */
        default     :
            str = "";
    }
    if(str[0] == 0) fprintf(fd, "%c", ch);
    else            fprintf(fd, "%s", str);

    return(0);
}

/*NH*/
A2mFrAFrame(fd, rfilename, type)
FILE    *fd;
char    *rfilename;
int     type;
{
    int     isgif = 0, ispdf = 0; /* JMP 03-03-99 */
    char    filename[256];

    /* TEXTFLOW */
    fprintf(fd," <AFrame %d>\n", A2M_FRAMEID);
    /*    if(A2M_FRM_REF) fprintf(fd," <String `FIGURE %d'>\n", A2M_FRAMEID); /* JMP 14-12-97 */
    if(type == A2M_IMAGER || type == A2M_IMAGEFR) 
        fprintf(fd," <String `Figure %d'>\n", A2M_FRAMEID); /* JMP 14-12-97 */

    /* ANCHORED FRAME */
    if(A2M_FRM_GIFTOBMP && U_index(rfilename, ".gif") > 0) /* JMP 01-04-03 */
        SCR_change_ext(filename, rfilename, "bmp");
    else if(A2M_FRM_GIFTOPNG && U_index(rfilename, ".gif") > 0) /* JMP 23-08-2015 */
        SCR_change_ext(filename, rfilename, "png");
    else
        strcpy(filename, rfilename);

    if(U_index(filename, ".gif") > 0 || U_index(filename, ".GIF") > 0) isgif = 1;
    if(U_index(filename, ".pdf") > 0 || U_index(filename, ".PDF") > 0) ispdf = 1;
    fprintf(A2M_fdfrm, " <Frame\n  <ID %d>\n", A2M_FRAMEID);
    if(A2M_FRM_PENWIDTH > 0.0) fprintf(A2M_fdfrm, "  <Pen 15>  <PenWidth %lf pt>\n", A2M_FRM_PENWIDTH); /* JMP 18-02-99 */
    fprintf(A2M_fdfrm, "  <Fill 15>\n  <Separation 0>\n"); /* JMP 18-02-99 */

    /*    if(isgif)
    	fprintf(A2M_fdfrm, "  <BRect  1.29103\" 5.18056\" 5.91794\" 3.23\">\n");
        else
    	fprintf(A2M_fdfrm, "  <BRect  1.29103\" 5.18056\" 5.91794\" 3.23\">\n");
    /* JMP 18-02-99 */
    if(type != A2M_ANCHOR) { /* JMP 18-05-11 */
        fprintf(A2M_fdfrm, "  <BRect  1.0cm 1.0cm %lfcm %lfcm>\n",      /* JMP 18-02-99 */
                A2M_FRM_BRECT[0], A2M_FRM_BRECT[1]); /* JMP 18-02-99 */

        fprintf(A2M_fdfrm, "  <FrameType Below>\n  <NSOffset 0.00\">\n");       /* JMP 18-05-11 */
        fprintf(A2M_fdfrm, "  <BLOffset -3.06\">\n  <AnchorAlign Center>\n");   /* JMP 18-05-11 */
        fprintf(A2M_fdfrm, "  <Cropped Yes>\n");                                /* JMP 18-05-11 */
    }
    else
        fprintf(A2M_fdfrm, "  <FrameType Inline>\n  <NSOffset 0.00\">\n"); /* JMP 18-05-11 */

    if(type == A2M_IMAGEF || type == A2M_IMAGEFR)
        fprintf(A2M_fdfrm, "  <Float Yes >\n");

    if(filename[0] != 0) {
        fprintf(A2M_fdfrm, "  <ImportObject\n");
        if(A2M_FRM_PENWIDTH > 0.0) fprintf(A2M_fdfrm, "   <Pen 0>\n"); /* JMP 18-02-99 */
        fprintf(A2M_fdfrm, "   <ImportObFile `%s'>\n", filename);
        /* fprintf(A2M_fdfrm, "   <BRect  0.29231\" 0.375\" 5.33333\" 2.5\">\n"); /* JMP 18-02-99 */
        /* fprintf(A2M_fdfrm, "   <BRect  0.29231\" 0.375\" %lf\" %lf\">\n",
        			A2M_FRM_BRECT[0] - 0.6, A2M_FRM_BRECT[1] - 0.73);
        /* JMP 25-02-99 */
        if(1 || type == A2M_IMAGER || type == A2M_IMAGEFR) // JMP 8/1/2017
            //fprintf(A2M_fdfrm, "   <BRect  0.75 cm 0.95 cm %lf cm %lf cm>\n",
            fprintf(A2M_fdfrm, "   <BRect  0.75 cm 0.65 cm %lf cm %lf cm>\n",
                    A2M_FRM_BRECT[0] - 1.5, A2M_FRM_BRECT[1] - 1.2);
        else
            fprintf(A2M_fdfrm, "   <BRect  0.75 cm 0.2 cm %lf cm %lf cm>\n",
                    A2M_FRM_BRECT[0] - 1.5, A2M_FRM_BRECT[1] - 0.4);

        if(isgif) {
            fprintf(A2M_fdfrm, "   <ImportObUpdater `GIF'>\n");
            fprintf(A2M_fdfrm, "   <BitMapDpi %d>\n", A2M_FRM_DPI); /* JMP 18-02-99 */
        }
        else if(ispdf) {
            fprintf(A2M_fdfrm, "   <ImportObUpdater `PDF'>\n"); /* JMP 03-03-99 */
            fprintf(A2M_fdfrm, "   <BitMapDpi 0>\n");           /* JMP 18-02-99 */
            fprintf(A2M_fdfrm, "   <ImportObFixedSize Yes>\n"); /* JMP 03-03-99 */
        }
        else {
            fprintf(A2M_fdfrm, "   <BitMapDpi %d>\n", A2M_FRM_DPI); /* JMP 18-02-99 */
        }
        fprintf(A2M_fdfrm, "  > # end import\n");
    }

    /*    if(A2M_FRM_REF) { */                          /* JMP 14-12-97 */
    if(1 || type == A2M_IMAGER || type == A2M_IMAGEFR) { /* JMP remis le 25-02-99 */ // JMP 8/1/2017
        fprintf(A2M_fdfrm, "  <TextLine\n");
        //fprintf(A2M_fdfrm, "   <TLOrigin  %lf cm 0.6 cm>\n", A2M_FRM_BRECT[0] / 2.0);
        fprintf(A2M_fdfrm, "   <TLOrigin  %lf cm 0.4 cm>\n", A2M_FRM_BRECT[0] / 2.0);
        fprintf(A2M_fdfrm, "   <TLAlignment Center >\n");
        fprintf(A2M_fdfrm, "   <Angle 0>\n");
        fprintf(A2M_fdfrm, "   <Font\n");
        fprintf(A2M_fdfrm, "    <FTag `Figure'>\n");
        fprintf(A2M_fdfrm, "   > # end of Font\n");
        fprintf(A2M_fdfrm, "   <String `FIGURE %d'>\n", A2M_FRAMEID);
        fprintf(A2M_fdfrm, "  > # end of TextLine\n");

    } /* JMP 25-02-99 */

    fprintf(A2M_fdfrm, " > # end frame %d\n", A2M_FRAMEID++);
    return(0);
}

/*NH*/
A2mFrPrintAttr(fd, ap, n)
FILE    *fd;
A2MPAR  *ap;
int     n;
{
    A2MFNT  fnt, fntp;
    A2MSTR  *asp = 0, *as;

    as = ap->ap_strs[n];
    if(n) asp = ap->ap_strs[n - 1];
    A2mSetFont(ap, as, &fnt);
    A2mSetFont(ap, asp, &fntp);

    if(fnt.af_underline != fntp.af_underline) {
        if(fnt.af_underline) fprintf(fd, " <Font <FUnderline Yes>>");
        else                 fprintf(fd, " <Font <FUnderline No>>");
    }

    if(fnt.af_strike != fntp.af_strike) {
        if(fnt.af_strike) fprintf(fd, " <Font <FStrike Yes>>");
        else              fprintf(fd, " <Font <FStrike No>>");
    }

    if(fnt.af_bold != fntp.af_bold) {
        if(fnt.af_bold) fprintf(fd, " <Font <FBold Yes>>");
        else            fprintf(fd, " <Font <FBold No>>");
    }

    if(fnt.af_italic != fntp.af_italic) {
        if(fnt.af_italic) fprintf(fd, " <Font <FItalic Yes>>");
        else              fprintf(fd, " <Font <FItalic No>>");
    }

    if(fnt.af_family != fntp.af_family) {
        if(fnt.af_family == 'C') fprintf(fd, " <Font <FTag `Courier'>>");
        else if(fnt.af_family == 'G') fprintf(fd, " <Font <FTag `Symbol'>>"); /* JMP 18-05-11 */
        else                     fprintf(fd, " <Font <FTag `'>>");
    }

    if(fnt.af_size != fntp.af_size)
        fprintf(fd, " <Font <FSize %d>>", fnt.af_size);

    if(fnt.af_color != fntp.af_color)
        fprintf(fd, " <Font <FColor `%s'>>", A2M_FRCOLORS[fnt.af_color]);

    if(fnt.af_pos != fntp.af_pos)
        fprintf(fd, " <Font <FDY %d%%>>", -fnt.af_pos * 30); /* JMP 17-05-11 */

    return(0);
}

/*NH*/
A2mFrPrintTbl(at)
A2MTBL  *at;
{
    /* 1. fdfrw */
    if(at->at_title && A2M_FRM_TTITLE == 0)
        A2mFrPrintPar(at->at_title, 0, 0, A2M_TBLID, 0, -1, 0L);
    else {
        fprintf(A2M_fdfrw, " <Para <Pgf <PgfTag `Table'>>\n"); /* JMP 25-02-99 */
        fprintf(A2M_fdfrw, "  <ParaLine <ATbl %d>>\n", A2M_TBLID);
        fprintf(A2M_fdfrw, " >\n");
    }

    /* 2. fdtbl */
    fprintf(A2M_fdtbl, " <Tbl <TblID %d>\n", A2M_TBLID++);

    if(at->at_tag) fprintf(A2M_fdtbl, " <TblTag `%s'>",  at->at_tag);
    else           fprintf(A2M_fdtbl, " <TblTag `Format A'>");

    fprintf(A2M_fdtbl, " <TblNumColumns %d>\n", at->at_nc);
    A2mFrPrintTblFmt(A2M_fdtbl, at);
    if(at->at_title && A2M_FRM_TTITLE) {
        fprintf(A2M_fdtbl, " <TblTitle\n");
        fprintf(A2M_fdtbl, "    <TblTitleContent\n");
        A2mFrPrintPar(at->at_title, 1, 0, 0, 0, -1, "Table Continuation");
        fprintf(A2M_fdtbl, "    > # end of TblTitleContent\n");
        fprintf(A2M_fdtbl, "   > # end of TblTitle\n");
    }
    A2mFrPrintTblLines(at, 0); /* Heading */
    A2mFrPrintTblLines(at, 1); /* Body */
    A2mFrPrintTblLines(at, 2); /* Footing */

    fprintf(A2M_fdtbl, " >\n");

    if(at->at_footnote) A2mFrPrintPar(at->at_footnote, 0, 0, 0, 0, -1, 0L);
    return(0);
}

/*NH*/
A2mFrCalcShading(val)
int     val;
{
    switch(val) {
        case 8 :
            return(3);
        case 7 :
            return(4);
        case 6 :
            return(4);
        case 5 :
            return(5);
        case 4 :
            return(5);
        case 3 :
            return(6);
        default:
            return(15);
    }
}

/*NH*/
char *A2mFrRuling(line)
int     line;
{
    if(line == 0) return("None");
    else if(line == 1) return("Thin");
    return("Thick");
}


/*NH*/
A2mFrPrintTblFmt(fd, at)
FILE    *fd;
A2MTBL  *at;
{
    int     i, b = at->at_border;

    if(b < 0) b = A2M_FRM_TBORDER;

    fprintf(fd, "  <TblFormat\n");
    if(A2M_FRM_TTITLE == 0) fprintf(fd, "   <TblTitlePlacement None>\n");
    fprintf(fd, "   <TblWidth %ld mm>\n", A2M_FRM_TWIDTH);  /* JMP 02-04-98 */
    fprintf(fd, "   <TblCellMargins 2pt 2pt 2pt 2pt>\n");           /* JMP 02-04-98 */
    fprintf(fd, "   <TblBlockSize 10>\n");                  /* JMP 02-04-98 */
    fprintf(fd, "   <TblAlignment TblLRuling>\n");             /* JMP 02-04-98 */

    if(at->at_widths) {
        for(i = 0 ; i < at->at_nc ; i++)
            fprintf(A2M_fdtbl, " <TblColumn <TblColumnNum %d> <TblColumnWidth %lf cm>>\n", i, at->at_widths[i]); /* JMP 30-07-98 */
    }
    else {
        fprintf(A2M_fdtbl, " <TblColumn <TblColumnNum 0> <TblColumnWidthA 0 %d mm>>\n", A2M_FRM_TCOL1);
        for(i = 1 ; i < at->at_nc ; i++) {
            if(at->at_nc == 2)
                fprintf(A2M_fdtbl, " <TblColumn <TblColumnNum 1> <TblColumnWidthA 0 %d mm>>\n", A2M_FRM_TCOL1);
            else
                fprintf(A2M_fdtbl, " <TblColumn <TblColumnNum %d> <TblColumnWidthP 1>>\n", i);
        }
    }

    if(at->at_shadbrush[0]) {
        fprintf(fd, "   <TblHFFill %d>\n", A2mFrCalcShading(at->at_shadbrush[0]));
        fprintf(fd, "   <TblHFSeparation 4>\n");
        if(A2M_FRM_TCOLOR)
            fprintf(fd, "   <TblHFColor `%s'>\n", A2M_FRCOLORS[at->at_shadcol[0]]);
        else
            fprintf(fd, "   <TblHFColor `None'>\n");
    }
    if(at->at_shadbrush[1]) {
        fprintf(fd, "   <TblBodyFill %d>\n", A2mFrCalcShading(at->at_shadbrush[1]));
        fprintf(fd, "   <TblBodySeparation 4>\n");
        if(A2M_FRM_TCOLOR)
            fprintf(fd, "   <TblBodyColor `%s'>\n", A2M_FRCOLORS[at->at_shadcol[1]]);
        else
            fprintf(fd, "   <TblBodyColor `None'>\n");
        fprintf(fd, "   <TblXFill %d>\n", A2mFrCalcShading(at->at_shadbrush[1]));
        fprintf(fd, "   <TblXSeparation 4>\n");
        if(A2M_FRM_TCOLOR)
            fprintf(fd, "   <TblXColor `%s'>\n", A2M_FRCOLORS[at->at_shadcol[1]]);
        else
            fprintf(fd, "   <TblXColor `None'>\n");
    }
    fprintf(fd, "   <TblLRuling `%s'>\n", A2mFrRuling(b));
    fprintf(fd, "   <TblBRuling `%s'>\n", A2mFrRuling(b));
    fprintf(fd, "   <TblRRuling `%s'>\n", A2mFrRuling(b));
    fprintf(fd, "   <TblTRuling `%s'>\n", A2mFrRuling(b));
    fprintf(fd, "   <TblColumnRuling `%s'>\n", A2mFrRuling(A2M_FRM_VLINES));
    fprintf(fd, "   <TblXColumnRuling `%s'>\n", A2mFrRuling(A2M_FRM_VLINES));
    fprintf(fd, "   <TblBodyRowRuling `%s'>\n", A2mFrRuling(A2M_FRM_HLINES));
    fprintf(fd, "   <TblXRowRuling `%s'>\n", A2mFrRuling(A2M_FRM_HLINES));
    fprintf(fd, "   <TblHFRowRuling `%s'>\n", A2mFrRuling(A2M_FRM_HLINES));
    fprintf(fd, "   <TblSeparatorRuling `%s'>\n", A2mFrRuling(b));
    fprintf(fd, "  >\n");
    return(0);
}

/*NH*/
A2mFrTblNbLines(at, hbf)
A2MTBL  *at;
int     hbf;
{
    int     nb = 0, i;

    for(i = 0 ; i < at->at_nl ; i++) {
        if(at->at_tls[i].atl_hbf != hbf) continue;
        if(at->at_tls[i].atl_type) continue;
        nb++;
    }
    return(nb);
}

/*NH*/
A2mFrPrintTblLines(at, hbf)
A2MTBL  *at;
int     hbf;
{
    A2MTC   *tc;
    int     i, j, line = 0, just;

    if(A2mFrTblNbLines(at, hbf) == 0) return(0); /* JMP 04-12-06 */

    switch(hbf) {
        case 0 :
            fprintf(A2M_fdtbl, "  <TblH\n");
            break;
        case 1 :
            fprintf(A2M_fdtbl, "  <TblBody\n");
            break;
        case 2 :
            fprintf(A2M_fdtbl, "  <TblF\n");
            break;
    }

    for(i = 0 ; i < at->at_nl ; i++) {
        if(at->at_tls[i].atl_hbf != hbf) continue;
        if(at->at_tls[i].atl_type) {
            line = 1;
            continue;
        }
        fprintf(A2M_fdtbl, "   <Row\n");
        for(j = 0 ; j < at->at_nc ; j++) {
            tc = at->at_tls[i].atl_tcs + j;
            fprintf(A2M_fdtbl, "    <Cell ");
            if(tc->atc_ncells > 1) fprintf(A2M_fdtbl, " <CellColumns %d>",
                                               tc->atc_ncells);
            fprintf(A2M_fdtbl, "\n     <CellContent\n");
            just = tc->atc_center;                                      /* JMP 28-01-99 */
            if(tc->atc_center == 5) just = -1; /* JMP 28-01-99 */
            if(tc->atc_center >= 6) just -= 6; /* JMP 17-03-00 */
            A2mFrPrintPar(tc->atc_par, 1, 6, 0, 0, just, 0L);
            fprintf(A2M_fdtbl, "     >\n");
            if(line) fprintf(A2M_fdtbl, "     <CellTRuling `Thin'>\n");
            fprintf(A2M_fdtbl, "    >\n");
        }
        fprintf(A2M_fdtbl, "   >\n");
        line = 0;
    }
    fprintf(A2M_fdtbl, "  >\n");
    return(0);
}

/*NH*/
A2mFrFontCatalog()
{
    int     pts = 10;
    
    fprintf(A2M_fdfrm, "<FontCatalog\n");
    fprintf(A2M_fdfrm, " <Font\n");
    fprintf(A2M_fdfrm, "  <FTag `Courier'>\n");
    fprintf(A2M_fdfrm, "  <FFamily `Courier New'>\n");
    fprintf(A2M_fdfrm, "  <FSize  %d pt>\n", pts);
    fprintf(A2M_fdfrm, "  <FWeight `Regular'>\n");
    fprintf(A2M_fdfrm, "  <FAngle `Regular'>\n");
    fprintf(A2M_fdfrm, " > # end of Font\n");
    fprintf(A2M_fdfrm, " <Font\n");
    fprintf(A2M_fdfrm, "  <FTag `Symbol'>\n");
    fprintf(A2M_fdfrm, "  <FFamily `Symbol'>\n");
    fprintf(A2M_fdfrm, "  <FSize  %d pt>\n", pts);
    fprintf(A2M_fdfrm, "  <FWeight `Regular'>\n");
    fprintf(A2M_fdfrm, "  <FAngle `Regular'>\n");
    fprintf(A2M_fdfrm, " > # end of Font\n");
    fprintf(A2M_fdfrm, " <Font\n");
    fprintf(A2M_fdfrm, "  <FTag `Figure'>\n");
    //fprintf(A2M_fdfrm, "  <FFamily `%s'>\n", A2M_FRM_BASEFONT);
    fprintf(A2M_fdfrm, "  <FFamily `Cambria'>\n");
    fprintf(A2M_fdfrm, "  <FWeight `Bold'>\n");
    fprintf(A2M_fdfrm, "  <FSize  9.0 pt>\n");
    fprintf(A2M_fdfrm, "  <FWeight `Regular'>\n");
    fprintf(A2M_fdfrm, "  <FAngle `Regular'>\n");
    fprintf(A2M_fdfrm, " > # end of Font\n");
    fprintf(A2M_fdfrm, " <Font\n");
    fprintf(A2M_fdfrm, "  <FTag `Graph'>\n");
    fprintf(A2M_fdfrm, "  <FFamily `%s'>\n", A2M_FRM_BASEFONT);
    fprintf(A2M_fdfrm, "  <FSize  8.0 pt>\n");
    fprintf(A2M_fdfrm, "  <FWeight `Regular'>\n");
    fprintf(A2M_fdfrm, "  <FAngle `Regular'>\n");
    fprintf(A2M_fdfrm, " > # end of Font\n");
    fprintf(A2M_fdfrm, " <Font\n");
    fprintf(A2M_fdfrm, "  <FTag `GraphLegend'>\n");
    fprintf(A2M_fdfrm, "  <FFamily `%s'>\n", A2M_FRM_BASEFONT);
    fprintf(A2M_fdfrm, "  <FSize  8.0 pt>\n");
    fprintf(A2M_fdfrm, "  <FItalic Yes>\n");
    fprintf(A2M_fdfrm, "  <FWeight `Regular'>\n");
    fprintf(A2M_fdfrm, " > # end of Font\n");
    fprintf(A2M_fdfrm, " <Font\n");
    fprintf(A2M_fdfrm, "  <FTag `GraphLabel'>\n");
    fprintf(A2M_fdfrm, "  <FFamily `%s'>\n", A2M_FRM_BASEFONT);
    fprintf(A2M_fdfrm, "  <FSize  8.0 pt>\n");
    fprintf(A2M_fdfrm, "  <FWeight `Regular'>\n");
    fprintf(A2M_fdfrm, "  <FAngle `Regular'>\n");
    fprintf(A2M_fdfrm, " > # end of Font\n");
    fprintf(A2M_fdfrm, " <Font\n");
    fprintf(A2M_fdfrm, "  <FTag `GraphFooter'>\n");
    fprintf(A2M_fdfrm, "  <FFamily `%s'>\n", A2M_FRM_BASEFONT);
    fprintf(A2M_fdfrm, "  <FBold Yes>\n");
    fprintf(A2M_fdfrm, "  <FSize  8.0 pt>\n");
    fprintf(A2M_fdfrm, "  <FAngle `Regular'>\n");
    fprintf(A2M_fdfrm, " > # end of Font\n");
    fprintf(A2M_fdfrm, " <Font\n");
    fprintf(A2M_fdfrm, "  <FTag `GraphHeader'>\n");
    fprintf(A2M_fdfrm, "  <FFamily `%s'>\n", A2M_FRM_BASEFONT);
    fprintf(A2M_fdfrm, "  <FBold Yes>\n");
    fprintf(A2M_fdfrm, "  <FSize  8.0 pt>\n");
    fprintf(A2M_fdfrm, "  <FAngle `Regular'>\n");
    fprintf(A2M_fdfrm, " > # end of Font\n");
    fprintf(A2M_fdfrm, ">\n");
    return(0);
}

/*NH*/
A2mFrColorCatalog()
{
    fprintf(A2M_fdfrm, "<ColorCatalog                            \n");
    fprintf(A2M_fdfrm, " <Color                                  \n");
    fprintf(A2M_fdfrm, "  <ColorTag `Black'>                     \n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>                  \n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  0.000000>               \n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>                \n");
    fprintf(A2M_fdfrm, "  <ColorBlack  100.000000>               \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsBlack>          \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsReserved>       \n");
    fprintf(A2M_fdfrm, " > # end of Color                        \n");
    fprintf(A2M_fdfrm, " <Color                                  \n");
    fprintf(A2M_fdfrm, "  <ColorTag `White'>                     \n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>                  \n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  0.000000>               \n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>                \n");
    fprintf(A2M_fdfrm, "  <ColorBlack  0.000000>                 \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsWhite>          \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsReserved>       \n");
    fprintf(A2M_fdfrm, " > # end of Color                        \n");
    fprintf(A2M_fdfrm, " <Color                                  \n");
    fprintf(A2M_fdfrm, "  <ColorTag `Red'>                       \n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>                  \n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  100.000000>             \n");
    fprintf(A2M_fdfrm, "  <ColorYellow  100.000000>              \n");
    fprintf(A2M_fdfrm, "  <ColorBlack  0.000000>                 \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsRed>            \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsReserved>       \n");
    fprintf(A2M_fdfrm, " > # end of Color                        \n");
    fprintf(A2M_fdfrm, " <Color                                  \n");
    fprintf(A2M_fdfrm, "  <ColorTag `Green'>                     \n");
    fprintf(A2M_fdfrm, "  <ColorCyan  100.000000>                \n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  0.000000>               \n");
    fprintf(A2M_fdfrm, "  <ColorYellow  100.000000>              \n");
    fprintf(A2M_fdfrm, "  <ColorBlack  0.000000>                 \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsGreen>          \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsReserved>       \n");
    fprintf(A2M_fdfrm, " > # end of Color                        \n");
    fprintf(A2M_fdfrm, " <Color                                  \n");
    fprintf(A2M_fdfrm, "  <ColorTag `Blue'>                      \n");
    fprintf(A2M_fdfrm, "  <ColorCyan  100.000000>                \n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  100.000000>             \n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>                \n");
    fprintf(A2M_fdfrm, "  <ColorBlack  0.000000>                 \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsBlue>           \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsReserved>       \n");
    fprintf(A2M_fdfrm, " > # end of Color                        \n");
    fprintf(A2M_fdfrm, " <Color                                     \n");
    fprintf(A2M_fdfrm, "  <ColorTag `Cyan'>                         \n");
    fprintf(A2M_fdfrm, "  <ColorCyan  100.000000>                   \n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  0.000000>                  \n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>                   \n");
    fprintf(A2M_fdfrm, "  <ColorBlack  0.000000>                    \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsCyan>              \n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsReserved>          \n");
    fprintf(A2M_fdfrm, " > # end of Color                           \n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `Magenta'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  100.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsMagenta>\n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsReserved>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `Yellow'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  100.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsYellow>\n");
    fprintf(A2M_fdfrm, "  <ColorAttribute ColorIsReserved>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `Grid'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  15.000000>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `C1'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  31.764709>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  8.627457>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  50.588242>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `C2'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  74.509811>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `C3'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  45.098053>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  83.529419>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  3.921570>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `C4'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  57.254913>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `C5'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  45.098053>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  83.529419>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  3.921570>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `C7'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  39.607849>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  5.882355>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  17.647064>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `C6'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  9.803925>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  5.098038>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  25.490204>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `C8'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  21.960785>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  76.862747>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  0.000000>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, " <Color\n");
    fprintf(A2M_fdfrm, "  <ColorTag `C9'>\n");
    fprintf(A2M_fdfrm, "  <ColorCyan  24.705887>\n");
    fprintf(A2M_fdfrm, "  <ColorMagenta  12.156860>\n");
    fprintf(A2M_fdfrm, "  <ColorYellow  0.000000>\n");
    fprintf(A2M_fdfrm, "  <ColorBlack  64.705887>\n");
    fprintf(A2M_fdfrm, " > # end of Color\n");
    fprintf(A2M_fdfrm, "> # end of ColorCatalog\n");

    return(0);
}

/*NH*/
char *A2mFrAlignment(just)
int     just;
{
    switch(just) {
        case 1 :
            return("Center");
        case 2 :
            return("Right");
        case 3 :
            return("Left");   /* JMP 14-04-00 */
        case 4 :
            return("LeftRight");
        default:
            return("Left");
    }
}

/*NH*/
char *A2mFrNoYes(ny)
int     ny;
{
    switch(ny) {
        case 1 :
            return("Yes");
        default:
            return("No");
    }
}

/*NH*/
char *A2mFrPlacement(newpage)
int     newpage;
{
    switch(newpage) {
        case 1 :
            return("PageTop");
        default:
            return("Anywhere");
    }
}

/*NH*/
char *A2mFrFontFamily(family)
int     family;
{
    switch(family) {
        case 'B' :
            return("Bookman");         /* JMP 26-02-98 */
        case 'P' :
            return("Palatino");        /* JMP 26-02-98 */
        case 'T' :
            //return("Times");
            return("Arial");
        case 'M' :
            return("Trebuchet MS");
        case 'C' :
            //return("Courier");
            return("Courier New");
        default:
            return(A2M_FRM_BASEFONT);    /* JMP 21-11-2011 */
    }
}

/*NH*/
A2mFrCatalogPar()
{
    int     i;

    fprintf(A2M_fdfrm, "<PgfCatalog\n");
    for(i = 0 ; i < A2M_NB_PPRS ; i++)
        A2mFrCatalog1Par(A2M_fdfrm, A2M_PPRS + i);

    fprintf(A2M_fdfrm, "> # End of PgfCatalog\n");
    return(0);
}

/*NH*/
A2mFrCatalog1Par(fd, pp)
FILE    *fd;
A2MPPR  *pp;
{
    int     i;

    fprintf(fd, " <Pgf\n");
    fprintf(fd, "  <PgfTag `%s'>\n", pp->pp_tag);
    fprintf(fd, "  <PgfAlignment %s>\n", A2mFrAlignment(pp->pp_just));
    fprintf(fd, "  <PgfPlacement %s>\n", A2mFrPlacement(pp->pp_newpage));
    fprintf(fd, "  <PgfSpBefore  %d pt>\n", pp->pp_spaceb);
    fprintf(fd, "  <PgfSpAfter  %d pt>\n", pp->pp_spacea);
    fprintf(fd, "  <PgfWithNext %s>\n", A2mFrNoYes(pp->pp_keepwn));
    fprintf(fd, "  <PgfFont\n");
    fprintf(fd, "   <FFamily `%s'>\n", A2mFrFontFamily(pp->pp_fnt.af_family));
    if(pp->pp_fnt.af_bold)      fprintf(fd, "   <FWeight `Bold'>\n");
    else                        fprintf(fd, "   <FWeight `Regular'>\n");
    if(pp->pp_fnt.af_italic)    fprintf(fd, "   <FAngle `Oblique'>\n");
    else                        fprintf(fd, "   <FAngle `Regular'>\n");
    if(pp->pp_fnt.af_underline) fprintf(fd, "   <FUnderlining FSingle>\n");
    else                        fprintf(fd, "   <FUnderlining FNoUnderlining>\n");
    fprintf(fd, "   <FSize  %d pt>\n", pp->pp_fnt.af_size);
    fprintf(fd, "   <FColor `%s'>\n", A2M_FRCOLORS[pp->pp_fnt.af_color]);
    fprintf(fd, "  > # end of PgfFont\n");
    fprintf(fd, "  <PgfLineSpacing Fixed>\n");
    if(pp->pp_bullet) {
        fprintf(fd, "  <PgfFIndent  %d pt>\n", pp->pp_lmarg);
        fprintf(fd, "  <PgfLIndent  %d pt>\n", pp->pp_lmarg + 8);
        fprintf(fd, "  <PgfAutoNum Yes>\n");
        fprintf(fd, "  <PgfNumFormat `\\xa5 \\\\t'>\n");
        fprintf(fd, "  <PgfNumTabs 1>\n");
        fprintf(fd, "  <TabStop\n");
        fprintf(fd, "   <TSX  %d pt>\n", pp->pp_lmarg + 8);
        fprintf(fd, "   <TSType Left>\n");
        fprintf(fd, "   <TSLeaderStr ` '>\n");
        fprintf(fd, "  > # end of TabStop\n");
    }
    else if(pp->pp_number) {
        fprintf(fd, "  <PgfFIndent  %d pt>\n", pp->pp_lmarg);
        fprintf(fd, "  <PgfLIndent  %d pt>\n", pp->pp_lmarg + 16);
        fprintf(fd, "  <PgfAutoNum Yes>\n");
        fprintf(fd, "  <PgfNumFormat `< \\>");
        for(i = 1 ; i < pp->pp_number - 1 ; i++) fprintf(fd, "<n\\>.");
        if(pp->pp_number > 1) fprintf(fd, "<n+\\> \\\\t'>\n");
        else                  fprintf(fd, " \\\\t'>\n");
    }
    else {
        fprintf(fd, "  <PgfAutoNum No>\n");
        fprintf(fd, "  <PgfLIndent  %d pt>\n", pp->pp_lmarg);
        fprintf(fd, "  <PgfFIndent  %d pt>\n", pp->pp_lmarg);
    }

    fprintf(fd, " > # end of Pgf\n");
    return(0);

}

/* ================================================================
Fixe la valeur des variables globales avant l'interp�tation d'un fichier
a2m etla g�n�ration d'un fichier mif en se basant sur les d�finitions du
fichier .ini associ�.

Les sections [A2M] et [MAKER] du fichier .ini sont interpr�t�es.

&EN filename = nom du fichier .ini

La syntaxe des fichiers a2m est d�crite dans un chapitre sp�cifique.

&TI Fichier a2m.ini
���������������
Le fichier a2m.ini (ou un autre) contient des param�tres pour
la lecture du fichier a2m et la g�n�ration du fichier mif. Pour que ces
param�tres soient pris en compte par le programme A2mToMif(), il faut
appeler la fonction A2mFrmReadIni(filename) avant de lancer la fonction
d'interpr�tation et d'impression.

&CO
    #include <s_a2m.h>

    A2mFrmReadIni("myfile.ini");
    A2mToMif("myfile.a2m");
&TX

&SA A2mToGdi(), A2mToRtf(), A2mToMif(), A2mToHtml(), A2mPrintError()
==================================================================== */

A2mFrmReadIni(filename)
char    *filename;
{
    char    buf[256];

    A2mReadA2mIni(filename);
    IniReadNumParm(filename, "MAKER", "FONTSIZE",       &A2M_FONTSIZE);
    IniReadNumParm(filename, "MAKER", "FONTINCR",       &A2M_FONTINCR);
    IniReadNumParm(filename, "MAKER", "TABLEFONTSIZE",  &A2M_TFONTSIZE);
    IniReadNumParm(filename, "MAKER", "TABLECOLOR",     &A2M_FRM_TCOLOR);
    IniReadNumParm(filename, "MAKER", "TABLECOL1",      &A2M_FRM_TCOL1);
    IniReadNumParm(filename, "MAKER", "TABLECOLN",      &A2M_FRM_TCOLN);
    IniReadNumParm(filename, "MAKER", "TABLESPLIT",     &A2M_FRM_TSPLIT);
    IniReadNumParm(filename, "MAKER", "TABLEWIDTH",     &A2M_FRM_TWIDTH);
    IniReadNumParm(filename, "MAKER", "TABLEBORDER",    &A2M_FRM_TBORDER);
    IniReadNumParm(filename, "MAKER", "TABLEHLINES",    &A2M_FRM_HLINES);
    IniReadNumParm(filename, "MAKER", "TABLEVLINES",    &A2M_FRM_VLINES);
    IniReadNumParm(filename, "MAKER", "IMAGEREF",       &A2M_FRM_REF);
    if(IniReadTxtParm(filename, "MAKER", "FONTFAMILY", buf, 255) == 0)
        A2M_FONTFAMILY = buf[0];

    return(0);
}

/*NH*/
/***************** GRAPHICS ***********************/

extern int (*A2M_FR_GRF_FNS[])();

/*NH*/
A2mFrPrintGrf(agrf)
A2MGRF  *agrf;
{
    A2M_GRF_FNS = A2M_FR_GRF_FNS;
    A2mGrfPrint(agrf);
    return(0);
}

/*NH*/
A2mFrGrfInit(ag)
A2MGRF  *ag;
{
    FILE    *fdfrw = A2M_fdfrw,
             *fdfrm = A2M_fdfrm;
    double  width = ag->ag_size[0],
            height = ag->ag_size[1];
    A2MPAR  *prTitle = ag->ag_title;

    A2mFrGrfPrepare();
    if(prTitle)
        A2mFrPrintPar(prTitle, 0, 0, 0, A2M_FRAMEID, -1, 0L);
    else
        fprintf(fdfrw, " <Para <Pgf <PgfTag `GraphTitle'>> <ParaLine <AFrame %d>>>\n", A2M_FRAMEID);

    /* ANCHORED FRAME */
    fprintf(fdfrm, " <Frame\n  <ID %d>\n  <Pen 15>\n", A2M_FRAMEID);
    fprintf(fdfrm, "  <Units Upt>");
//    fprintf(fdfrm, "  <PenWidth 0.2 pt>\n  <Fill 5>\n  <ObColor `Blue'>\n"); /*4.04*/
    fprintf(fdfrm, "  <PenWidth 0.2 pt>\n"); /*4.04*/
    fprintf(fdfrm, "  <BRect  0 cm 0 cm %5.2lf cm %5.2lf cm>\n", width, height); /*4.04*/
    fprintf(fdfrm, "  <FrameType Below>\n  <NSOffset 0 cm>\n");
    fprintf(fdfrm, "  <Float No >\n");
    fprintf(fdfrm, "  <BLOffset 0.0 cm>\n  <AnchorAlign Left>\n");
    fprintf(fdfrm, "  <Cropped Yes>\n");
    return(0);
}

/*NH*/
A2mFrGrfEnd(ag)
A2MGRF  *ag;
{
    FILE    *fd = A2M_fdfrm;
    A2MPAR  *prFootnote = ag->ag_footnote;

    fprintf(fd, " > # end frame %d\n", A2M_FRAMEID++);
    A2mFrPrintPar(prFootnote, 0, 0, 0, 0, -1, 0L);
    return(0);
}

A2mFrGrfPen(pen)
A2MPEN  *pen;
{
    int     i;
    FILE    *fd = A2M_fdfrm;

    fprintf(fd, "<PenWidth %3.1lf pt> <ObColor `%s'> ", pen->p_width, pen->p_color);
    if(pen->p_nbsegments == 0) fprintf(fd, "<DashedPattern <DashedStyle Solid>>");
    else {
        fprintf(fd, "<DashedPattern <DashedStyle Dashed> <NumSegments %d>", pen->p_nbsegments);
        for(i = 0; i < pen->p_nbsegments; i++)
            fprintf(fd, "<DashSegment  %3.1lf pt> ", pen->p_segment[i]);
        fprintf(fd, ">");
    }
    return(0);
}

/*NH*/
A2mFrGrfBrush(brush)
A2MBRUSH  *brush;
{
    FILE    *fd = A2M_fdfrm;

    fprintf(fd, "<DashedPattern <DashedStyle Solid>>");
    fprintf(fd, "<Fill %d> <ObColor `%s'> ", brush->b_fill, brush->b_color);
    return(0);
}

/*NH*/
A2mFrGrfLine(axis, x1, y1, x2, y2, props)
double  x1, y1, x2, y2;
int     axis, props;
{
    A2MPEN  pen;
    FILE    *fd = A2M_fdfrm;

    switch(axis) {
        case -1 :
            break;
        case  0 :
            x1 += GR_X_0;
            y1 += GR_Y_0;
            x2 += GR_X_0;
            y2 += GR_Y_0;
            break;
        default :
            x1 += GR_X_0;
            y1 += GR_Z_0;
            x2 += GR_X_0;
            y2 += GR_Z_0;
            break;
    }

    fprintf(fd, " <PolyLine <GroupID %d> <Pen 0> ", GR_obj_id);
    A2mFrGrfGetPen(props, &pen);
    A2mFrGrfPen(&pen);

    fprintf(fd, " <NumPoints 2> <Point %5.2f %5.2f> <Point %5.2f %5.2f>",
            x1, GR_HEIGHT -  y1,
            x2, GR_HEIGHT -  y2);
    fprintf(fd, " >\n");
    return(0);
}

/*NH*/
A2mFrGrfText(axis, x, y, string, align, color)
int     axis;
double  x, y;
char    *string, *align, *color;
{
    FILE    *fd = A2M_fdfrm;
    double  y_0;

    if(axis == 0) y_0 = GR_Y_0;
    else y_0 = GR_Z_0;

    fprintf(fd, "<TextLine <GroupID %d>", GR_obj_id);
    fprintf(fd, "  <Font <FTag `Graph'> <FColor `%s'>>", color);
    fprintf(fd, "  <TLAlignment %s> <TLOrigin %5.2f %5.2f> <String `%s'>",
            align, GR_X_0 + x, GR_HEIGHT - (y_0 + y), string);
    fprintf(fd, " >\n");
    return(0);
}

/*NH
A2mFrGrfLabel(nbr, type, title, props)
int     nbr, type, props;
A2MPAR  *title;
{
    int         i, line, col;
    double      x, y, width;
    FILE        *fd = A2M_fdfrm;

    line = nbr%3;
    col = nbr/3;

    x = col * GR_WIDTH/2.0 + A2M_FONTSIZE;
    y = (3.0 - line) * A2M_FONTSIZE;

    switch(type) {
	case 'L' :  A2mFrGrfLine(0,
			x - GR_X_0, y - GR_Y_0,
			x + 2 * A2M_FONTSIZE - GR_X_0, y - GR_Y_0,
			props);
		    break;
	case 'B' :  A2mFrGrfBox(0,
			x - GR_X_0, y - GR_Y_0 - A2M_FONTSIZE/2,
			(double) 2.0 * A2M_FONTSIZE, (double) 0.75 * A2M_FONTSIZE,
			props);
		    break;
    }

    A2mFrGrfPar(-1, x + 3.0*A2M_FONTSIZE, y - A2M_FONTSIZE/2,
	title, "Left", "Black");
    return(0);
}
*/
/*NH*/
A2mFrGrfPar(axis, x, y, par, align, color)
double  x, y;
char    *align, *color;
A2MPAR  *par;
{
    int         i;
    FILE        *fd = A2M_fdfrm;

    switch(axis) {
        case -1 :
            break;
        case  0 :
            x += GR_X_0;
            y += GR_Y_0;
            break;
        default :
            x += GR_X_0;
            y += GR_Z_0;
            break;
    }

    fprintf(fd, "<TextLine <GroupID %d>", GR_obj_id);
    fprintf(fd, "  <Font <FTag `%s'> <FColor `%s'>>", par->ap_tag, color);
    fprintf(fd, "  <TLAlignment %s> <TLOrigin %5.2f %5.2f>", align,
            x,  GR_HEIGHT - y);
    for(i = 0 ; par->ap_strs[i] ; i++) A2mFrPrintStr(fd, par, i);
    fprintf(fd, ">\n");
    return(0);
}

/*NH*/
A2mFrGrfGroupObj()
{
    FILE    *fd = A2M_fdfrm;

    fprintf(fd, "<Group <GroupID %d> <ID %d>>\n", GR_graph_id, GR_obj_id);
    return(0);
}

/*NH*/
A2mFrGrfGroup()
{
    FILE    *fd = A2M_fdfrm;

    fprintf(fd, "<Group <ID %d>>\n", GR_graph_id);
    return(0);
}


/*NH*/
A2mFrGrfBox(axis, x, y, w, h, props)
double  x, y, w, h;
int     axis, props;
{
    A2MBRUSH    brush;
    FILE        *fd = A2M_fdfrm;
    double      t, y_0 = 0;

    switch(axis) {
        case -1 :
            break;
        case  0 :
            x += GR_X_0;
            y_0 = GR_Y_0;
            break;
        default :
            x += GR_X_0;
            y_0 = GR_Z_0;
            break;
    }

    fprintf(fd,"<Rectangle <GroupID %d> <Pen 0> <PenWidth 0.2 pt>", GR_obj_id);
    A2mFrGrfGetBrush(props, &brush);
    A2mFrGrfBrush(&brush);

    if(h >= 0) t = y + h;
    else t = y;
    fprintf(fd,"<ShapeRect %5.2f %5.2f %5.2f %5.2f>>\n",
            x, GR_HEIGHT - y_0 - t, w, fabs(h));

    return(0);
}

/*NH*/
A2mFrGrfPolyLine(axis, nobs, vals, props)
int     axis, nobs, props;
double  *vals;
{
    int     i;
    A2MPEN  pen;
    FILE    *fd = A2M_fdfrm;
    double  y_0;

    if(axis == 0) y_0 = GR_Y_0;
    else y_0 = GR_Z_0;

    fprintf(fd, "<PolyLine <GroupID %d> <Fill 15> <Pen 0>", GR_obj_id);
    A2mFrGrfGetPen(props, &pen);
    A2mFrGrfPen(&pen);

    fprintf(fd, "<NumPoints %d>", nobs);
    for(i = 0; i < nobs; i++) {
        fprintf(fd, "<Point %5.2f %5.2f>",
                GR_X_0 + vals[i*2], GR_HEIGHT - (y_0 + vals[i*2+1]));
    }
    fprintf(fd, " >\n");
    return(0);
}

/*NH*/
A2mFrGrfPolyBar(axis, nobs, vals, width, props)
int     axis, nobs, props;
double  *vals, width;
{
    /*
        int     i,
    	    nbar = A2MGRFBARNB,
    	    cbar = A2MGRFBARCURRENT;

        for(i = 0; i < nobs; i++) {
    	A2mFrGrfBox(axis, vals[i*2] - (nbar/2.0 - cbar)*width, 0.0,
    			  width, vals[i*2+1], props);
        }
    */
    return(0);
}

/*********** TEMPORARY OBJECTS *******************/
/*NH*/
A2mFrGrfGetPen(props, pen)
int     props;
A2MPEN  *pen;
{
    extern A2MPEN      A2mFrGridPen;
    extern  A2MPEN     *A2mFrGrfdPen;

    if(props < 0)
        memcpy(pen, &A2mFrGridPen, sizeof(A2MPEN));
    else
        memcpy(pen, &(A2mFrGrfdPen[props]), sizeof(A2MPEN));
    return(0);
}


/*NH*/
A2mFrGrfGetBrush(props, brush)
int         props;
A2MBRUSH    *brush;
{
    extern  A2MBRUSH    *A2mFrGrfdBrush;

    memcpy(brush, &(A2mFrGrfdBrush[props]), sizeof(A2MBRUSH));
    return(0);
}


/******* FUNCTION TABLE FOR FRAME ***********/

int (*A2M_FR_GRF_FNS[])() = {
    A2mFrGrfInit,
    A2mFrGrfEnd,
    A2mFrGrfPen,
    A2mFrGrfBrush,
    A2mFrGrfLine,
    A2mFrGrfText,
    0,
    A2mFrGrfPar,
    A2mFrGrfGroupObj,
    A2mFrGrfGroup,
    A2mFrGrfBox,
    A2mFrGrfPolyLine,
    A2mFrGrfPolyBar,
    A2mFrGrfPrepare,
    A2mFrGrfGetPen,
    A2mFrGrfGetBrush
};


