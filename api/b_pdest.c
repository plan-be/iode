#include "iode.h"

extern int     KT_CUR_TOPIC; /* JMP 06-01-02 */
extern int     A2M_GIF_BGCOLOR_NB; // JMP 1/5/2022
extern int     A2M_GIF_TRCOLOR_NB; // JMP 1/5/2022 

int B_PrintDestFile(char *arg, int newf)
{
    char    **args = NULL;
    int     nb_args, type, rc;

    KT_CUR_TOPIC = 0; /* JMP 06-01-02 */
    SCR_strip(arg);
    if(arg != NULL && arg[0] != 0) {
        args = SCR_vtoms(arg, B_SEPS);
        nb_args = SCR_tbl_size(args);
        if(nb_args > 1) {
            switch(args[1][0]) {
                case 'h':
                case 'H':
                    A2M_HTML_RELSTYLE = "iodestyle.css"; /* JMP 05-02-2004 */
                    type = W_HTML;
                    break;
                case 'm':
                case 'M':
                    type = W_MIF;
                    break;
                case 'r':
                case 'R':
                    type = W_RTF;
                    break;
                case 'c':
                case 'C':
                    type = W_CSV;
                    break;
                case 'd':
                case 'D':
                    type = W_DUMMY;
                    break;
                default :
                    type = W_A2M;
                    break; /* A2M */
            }
        }
        else type = W_A2M;
        rc = B_PrintDestExt(args[0], newf, type);
        A_free(args);
    }
    else rc = B_PrintDestExt("", newf, W_GDI);

    return(rc);
}

int B_PrintDest(char *file)
{
    return(B_PrintDestFile(file, 0));
}

int B_PrintDestNew(char* file)
{
    return(B_PrintDestFile(file, 1));
}

int B_PrintDestExt(char* file, int newf, int type)           
{
    char    buf[K_MAX_FILE + 1];

    strcpy(buf, W_filename);
    W_dest(file, type);
    /*
        if(type > 3) {
    	if(newf) unlink(file);
    	if(W_open(file) < 0) {
    	    B_seterrn(52, W_filename);
    	    strcpy(W_filename, buf);
    	    return(-1);
    	}
    	else W_close();
        }
    */
    return(0);
}

int B_PrintNbDec(char* nbdec)
{
    // B_NBDEC globally replaced by K_NBDEC // JMP 18-04-2022
    K_NBDEC = atoi(nbdec); 
    if(K_NBDEC > 99 || (K_NBDEC < 0 && K_NBDEC != -1)) {
        B_seterrn(53, nbdec);
        K_NBDEC = 2;
        return(-1);
    }
   return(0);
}

int B_PrintLang(char* lang)       /* JMP38 26-09-92 */
{
    switch(lang[0]) {
        case 'e' :
        case 'E' :
            // B_LANG = 0; 
            K_LANG = 0; // JMP 18-04-2022
            break;
        case 'f' :
        case 'F' :
            K_LANG = 1; // JMP 18-04-2022
            //B_LANG = 1;
            break;
        case 'd' :
        case 'D' :
        case 'n' :
        case 'N' :
            //B_LANG = 2;
            K_LANG = 2; // JMP 18-04-2022
            break;
        default  :
            B_seterrn(83);
            return(-1);
    }
    return(0);
}


int B_PrintMulti(char* multi) /*GB 30/10/2007 */
{
    switch(multi[0]) {
        case 'n' :
        case 'N' :
            B_MULTIBAR = A2M_GR_MULTIBAR = 0;
            break;
        case 's' :
        case 'S' :
            B_MULTIBAR = A2M_GR_MULTIBAR = 1;
            break;
        case 'p' :
        case 'P' :
            B_MULTIBAR = A2M_GR_MULTIBAR = 2;
            break;
        default  :
            B_seterrn(303);
            return(-1);
    }
    return(0);
}

int B_PrintA2mAppend(char* arg)
{
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            W_a2mapp = 0;
            break;
        default :
            W_a2mapp = 1;
            break;
    }
    //BGUI_PrintA2mAppend();
    return(0);
}

int B_PrintTBreak(char* arg)
{
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            A2M_TBREAK = 0;
            break;
        default :
            A2M_TBREAK = 1;
            break;
    }
    //BGUI_PrintTBreak();
    return(0);
}

int B_PrintTPage(char* arg)
{
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            A2M_TPAGE = 0;
            break;
        default :
            A2M_TPAGE = 1;
            break;
    }
    // BGUI_PrintTPage();
    return(0);
}

int B_PrintGPage(char* arg)
{
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            A2M_GPAGE = 0;
            break;
        default :
            A2M_GPAGE = 1;
            break;
    }
    // BGUI_PrintGPage();
    return(0);
}

int B_PrintParaNum(char* arg)
{
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            A2M_NUMBERS = 0;
            break;
        default :
            A2M_NUMBERS = 1;
            break;
    }
    // BGUI_PrintParaNum();
    return(0);
}

int B_PrintPageHeader(char* arg)
{
    W_print_pg_header(arg);      // JMP 27/6/2022
    return(0);
}

int B_PrintPageFooter(char* arg)
{
    W_print_pg_footer(arg);      // JMP 27/6/2022
    return(0);
}

int B_PrintFont(char* arg)
{
    U_ch    **tbl = 0;
    int     ntbl = 0;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl > 0) {
        A2M_FONTFAMILY = SCR_upper_char(tbl[0][0]);
        // BGUI_PrintFont1();
    }
    if(ntbl > 1) {
        A2M_FONTSIZE = atoi(tbl[1]);
        A2M_FONTSIZE = max(A2M_FONTSIZE, 4);
        // BGUI_PrintFont2();
    }

    if(ntbl > 2) {
        A2M_FONTINCR = atoi(tbl[2]);
        A2M_FONTINCR = max(A2M_FONTINCR, 1);
        // BGUI_PrintFont3();
    }
    SCR_free_tbl(tbl);

    return(0);
}

int B_PrintTFont(char* arg)
{
    U_ch    **tbl = 0;
    int     ntbl = 0;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl > 0) {
        A2M_TFONTFAMILY = SCR_upper_char(tbl[0][0]);
        // BGUI_PrintTFont1();
    }
    if(ntbl > 1) {
        A2M_TFONTSIZE = atoi(tbl[1]);
        A2M_TFONTSIZE = max(A2M_TFONTSIZE, 4);
        // BGUI_PrintTFont2();
    }
    SCR_free_tbl(tbl);
    return(0);
}

int B_PrintTBox(char* arg)
{
    int     box = atoi(arg);

    if(box == 0 && SCR_upper_char(arg[0]) == 'Y') box = 1;
    A2M_FRM_TBORDER = box;
    A2M_HTML_TBORDER = box;
    A2M_RTF_TBORDER = box;
#ifndef UNIX
    A2M_GDI_TBORDER = box;
#endif
    // BGUI_PrintTBox();
    return(0);
}

int B_PrintTColor(char* arg)
{
    int     box = atoi(arg);

    if(box == 0 && SCR_upper_char(arg[0]) == 'Y') box = 1;
    A2M_FRM_TCOLOR = box;
    A2M_HTML_TCOLOR = box;
    A2M_RTF_TCOLOR = box;
    // BGUI_PrintTColor();
    /*    A2M_GDI_TCOLOR = box; */
    return(0);
}

int B_PrintTWidth(char* arg)
{
    U_ch    **tbl = 0;
    int     ntbl = 0, w, col1, coln;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl > 0) {
        w = atoi(tbl[0]);
        w = max(w, 65);
        A2M_RTF_TWIDTH = w;
        A2M_FRM_TWIDTH = w;
        // BGUI_PrintTWidth1();
    }

    if(ntbl > 1) {
        col1 = atoi(tbl[1]);
        col1 = max(col1, 50);
        A2M_RTF_TCOL1 = col1;
        A2M_FRM_TCOL1 = col1;
        // BGUI_PrintTWidth2();
    }

    if(ntbl > 2) {
        coln = atoi(tbl[2]);
        coln = max(coln, 15);
        A2M_RTF_TCOLN = coln;
        A2M_FRM_TCOLN = coln;
        // BGUI_PrintTWidth3();
    }

    SCR_free_tbl(tbl);
    return(0);
}

int B_PrintGSize(char* arg)
{
    U_ch    **tbl = 0;
    int     ntbl = 0, w;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl > 0) {
        w = atoi(tbl[0]);
        w = max(w, 35);
        A2M_GWIDTH = 0.1 * w;
        // BGUI_PrintGSize1();
    }

    if(ntbl > 1) {
        w = atoi(tbl[1]);
        w = max(w, 20);
        A2M_GHEIGHT = 0.1 * w;
        // BGUI_PrintGSize2();
    }
    if(ntbl > 2) { // GB 16/03/2009
        A2M_FONTSIZE = atoi(tbl[2]);
        A2M_FONTSIZE = max(A2M_FONTSIZE, 4);
        // BGUI_PrintFont2();
    }


    SCR_free_tbl(tbl);
    return(0);
}

int B_PrintGTheme(char* arg)
{
#ifndef _MSC_VER    
    ChrtThemeInit(arg);
#endif    
    return(0);
}

int B_PrintGBand(char* arg)
{
#ifndef _MSC_VER    
    char    **args;
    int     rc = 0, nbargs;
    PERIOD  *per;
    double  startvalue = 0, endvalue = 0;

    args = SCR_vtom(arg, ' ');
    nbargs = SCR_tbl_size(args);

    if(nbargs == 2) {
        per = PER_atoper(args[0]);
        startvalue = PER_per2real(per, 0);
        SW_nfree(per);

        per = PER_atoper(args[1]);
        endvalue = PER_per2real(per, 0);
        SW_nfree(per);
    }


    ChrtColorBand(startvalue, endvalue);
    SCR_free_tbl(args);
#endif    
    return(0);
}

int B_PrintGBox(char* arg)
{
    int     box = atoi(arg);

    if(box == 0 && SCR_upper_char(arg[0]) == 'Y') box = 1;
    A2M_BOXWIDTH = box;
    // BGUI_PrintGBox();
    return(0);
}

int B_PrintGBrush(char* arg)
{
    int     box = atoi(arg);

    if(box == 0 && SCR_upper_char(arg[0]) == 'Y') box = 50;
    A2M_BACKBRUSH = box;
    // BGUI_PrintGBrush();
    return(0);
}

int B_GetColor(char* arg)
{
    int     col;

    SCR_lower(arg);
    if(strcmp(arg, "blue") == 0)           col  = 'b';
    else if(strcmp(arg, "black") == 0)     col  = 'B';
    else if(strcmp(arg, "magenta") == 0)   col  = 'm';
    else if(strcmp(arg, "cyan") == 0)      col  = 'c';
    else if(strcmp(arg, "red") == 0)       col  = 'r';
    else if(strcmp(arg, "green") == 0)     col  = 'g';
    else if(strcmp(arg, "yellow") == 0)    col  = 'y';
    else if(strcmp(arg, "white") == 0)     col  = 'w';
    return(U_pos(col, "byBwrgcm"));
}

int B_PrintGColor(char* arg)
{
    int     col;

    col = B_GetColor(arg);
    if(col < 0) col = 0;
    // BGUI_PrintGColor(col);
    return(0);
}

int B_PrintRtfHelp(char* arg)
{
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            W_rtfhelp = 0;
            break;
        default :
            W_rtfhelp = 1;
            A2M_RTF_COMPRESS = 1;
            A2M_RTF_TOC      = 1;
            break;
    }

    // BGUI_PrintRtfHelp();
    return(0);
}

int B_PrintHtmlHelp(char* arg)
{
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            W_htmlhelp = 0;
            break;
        default :
            W_htmlhelp = 1;
            break;
    }

    /*    p_a2mrtf_HCW = W_rtfhelp; */
    return(0);
}

int B_PrintRtfTitle(char* arg)
{
    B_A2mSetRtfTitle(arg);
    // BGUI_PrintRtfTitle(arg);
    return(0);
}

int B_PrintRtfCopy(char* arg)
{
    B_A2mSetRtfCopy(arg);
    // BGUI_PrintRtfCopy(arg);
    return(0);
}

int B_PrintRtfLevel(char* arg)
{
    int     level = atoi(arg);
    extern int KT_CUR_LEVEL;

    if(level == 0) {
        if(arg[0] == '+') KT_CUR_LEVEL++;
        else              KT_CUR_LEVEL--;
    }
    else KT_CUR_LEVEL = level;
    KT_CUR_LEVEL = max(0, KT_CUR_LEVEL);
    return(0);
}

int B_PrintRtfTopic(char* arg)
{   
    W_print_rtf_topic(arg);
    return(0);
}

int B_PrintGdiOrient(char* arg)
{
    extern int W_gdiorient;

    switch(SCR_upper_char(arg[0])) {
        case 'L' :
            W_gdiorient = 1;
            break;
        default  :
            W_gdiorient = 0;
            break;
    }

    // BGUI_PrintGdiOrient();
    return(0);
}

int B_PrintGdiDuplex(char* arg)
{
    extern int W_gdiduplex;

    switch(SCR_upper_char(arg[0])) {
        /*case 'D' : W_gdiduplex = 1; break;
        case 'V' : W_gdiduplex = 2; break;
        */
        case 'D' :
            W_gdiduplex = 2;
            break; // JMP 5/3/2012
        case 'V' :
            W_gdiduplex = 1;
            break; // JMP 5/3/2012
        default  :
            W_gdiduplex = 0;
            break;
    }

    // BGUI_PrintGdiDuplex();
    return(0);
}

int B_PrintGdiPrinter(char* arg)
{
    extern char W_gdiprinter[];

    SCR_strlcpy(W_gdiprinter, arg, 70);
    return(0);
}

int B_PrintGIFBackColor(char* arg)
{
#ifndef _MSC_VER
    int     col;

    col = B_GetColor(arg);
    if(col < 0) col = 0;
    // BGUI_PrintGIFBackColor(col);
    B_A2mSetCol(A2M_GIF_BGCOLOR, col);
    A2M_GIF_BGCOLOR_NB = col; // JMP 1/5/2022
#endif    
    return(0);
}

int B_PrintGIFTransColor(char* arg)
{
#ifndef _MSC_VER
    int     col;

    col = B_GetColor(arg);
    if(col < 0) col = 0;
    // BGUI_PrintGIFTransColor(col);
    B_A2mSetCol(A2M_GIF_TRCOLOR, col);
    A2M_GIF_TRCOLOR_NB = col; // JMP 1/5/2022
#endif    
    return(0);
}

int B_PrintGIFInterlaced(char* arg)
{
#ifndef _MSC_VER    
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            A2M_GIF_INTER = 0;
            break;
        default :
            A2M_GIF_INTER = 1;
            break;
    }
#endif
    // BGUI_PrintGIFInterlaced();
    return(0);
}

int B_PrintGIFTransparent(char* arg)
{
#ifndef _MSC_VER
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            A2M_GIF_TRANS = 0;
            break;
        default :
            A2M_GIF_TRANS = 1;
            break;
    }

#endif
    // BGUI_PrintGIFTransparent();
    return(0);
}

int B_PrintGIFFilled(char* arg)
{
#ifndef _MSC_VER
    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            A2M_GIF_FILLED = 0;
            break;
        default :
            A2M_GIF_FILLED = 1;
            break;
    }

#endif
    // BGUI_PrintGIFFilled();
    return(0);
}

int B_PrintGIFFont(char* arg)
{
#ifndef _MSC_VER
    int     font;

    font = atoi(arg);
    if(font > 5 || font < 0) font = 1;
    // BGUI_PrintGIFFont(font);
    A2M_GIF_FONT = font + 1;

#endif    
    return(0);
}

int B_PrintHtmlStrip(char* arg)
{
    extern int  A2M_HTML_STRIP;

    switch(arg[0]) {
        case '0':
        case 'n':
        case 'N':
            A2M_HTML_STRIP = 0;
            break;
        default :
            A2M_HTML_STRIP = 1;
            break;
    }

    return(0);
}

int B_PrintHtmlStyle(char* arg) /* JMP 05-01-02 */
{
    extern char *A2M_HTML_STYLE;

    A2M_HTML_RELSTYLE = SCR_stracpy(arg);
    return(0);
}

int B_A2mToAll(char* arg, int type)
{
    U_ch    **tbl = 0, ibuf[256], obuf[256], *ext;
    int     ntbl;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl == 0) return(-1);
    SCR_cat_ext(ibuf, tbl[0], "a2m");
    switch(type) {
        case A2M_DESTFRM :
            ext = "mif";
            break;
        case A2M_DESTHTML:
            ext = "htm";
            break;
        case A2M_DESTRTF :
            ext = "rtf";
            break;
        case A2M_DESTCSV :
            ext = "csv";
            break;
        default : 
            ext = "";           
            break;                  
    }

    if(ntbl > 1) SCR_cat_ext(obuf, tbl[1], ext);
    else         SCR_change_ext(obuf, tbl[0], ext);
    SCR_free_tbl(tbl);
    switch(type) {
        case A2M_DESTFRM :
            return(A2mToMif(ibuf, obuf));
        case A2M_DESTHTML:
            return(A2mToHtml(ibuf, obuf, W_htmlhelp));
        case A2M_DESTRTF :
            return(A2mToRtf(ibuf, obuf, W_rtfhelp));
        case A2M_DESTCSV :
            return(A2mToCsv(ibuf, obuf));
        case A2M_DESTGDIPRT :
            return(A2mToGdiPrinter(0L, 1, "Iode", ibuf));
        default : 
            return(-1);
    }
}

int B_A2mToPrinter(char* arg)
{
    return(B_A2mToAll(arg, A2M_DESTGDIPRT));
}

int B_A2mToHtml(char* arg)
{
    return(B_A2mToAll(arg, A2M_DESTHTML));
}

int B_A2mToRtf(char* arg)
{
    return(B_A2mToAll(arg, A2M_DESTRTF));
}

int B_A2mToMif(char* arg)
{
    return(B_A2mToAll(arg, A2M_DESTFRM));
}

int B_A2mToCsv(char* arg)
{
    return(B_A2mToAll(arg, A2M_DESTCSV));
}


int B_GIFCOLS[8][3] = {
    /* BLUE    */ {171, 172, 211},
    /* YELLOW  */ {255, 255, 128},
    /* BLACK   */ {0, 0, 0},
    /* WHITE   */ {255, 255, 255},
    /* RED     */ {218, 69, 37},
    /* GREEN   */ {111, 197, 58},
    /* CYAN    */ {154, 255, 228},
    /* MAGENTA */ {157, 98, 157}
};

int B_A2mSetCol(int *dest, int col)
{
    dest[0]= B_GIFCOLS[col][0];
    dest[1]= B_GIFCOLS[col][1];
    dest[2]= B_GIFCOLS[col][2];
    return(0);
}

int B_PrintHtmlTableClass(char *table_class)
{
    SCR_free(A2M_HTML_TABLECLASS);
    A2M_HTML_TABLECLASS = SCR_stracpy(table_class);
    return(0);
}

int B_PrintHtmlTRClass(char *tr_class)
{
    SCR_free(A2M_HTML_TRCLASS);
    A2M_HTML_TRCLASS = SCR_stracpy(tr_class);
    return(0);
}
int B_PrintHtmlTHClass(char *th_class)
{
    SCR_free(A2M_HTML_THCLASS);
    A2M_HTML_THCLASS = SCR_stracpy(th_class);
    return(0);
}
int B_PrintHtmlTDClass(char *td_class)
{
    SCR_free(A2M_HTML_TDCLASS);
    A2M_HTML_TDCLASS = SCR_stracpy(td_class);
    return(0);
}
