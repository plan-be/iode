/**
 *  @header4iode
 * 
 *  Functions (and their subfunctions) called by the report engine to set up printing parameters and
 *  to generate outputs in various formats.
 *  
 *  @See B_FNS in b_rep_syntax.c for the correspondance between report commands and C functions.
 *  
 *  The syntax of the report commands being thouroughly described in the manual, 
 *  only the command syntax is included here.
 *  
 *  List of functions
 *  -----------------
 *      int B_PrintDestExt(char* file, int type)                       | Define the printing destination.
 *      int B_PrintDestFile(const std::string& arg)                    | Define the output file for the following printouts.
 *      int B_PrintDest(char *file)                                    | $PrintDest [nom_fichier] [format]
 *      int B_PrintDestNew(const std::string& file, int unused)                     | $PrintDestNew [nom_fichier] [format]
 *      int B_PrintNbDec(const std::string& nbdec, int unused)                      | $PrintNbDec nb
 *      int B_PrintLang(const std::string& lang, int unused)                        | $PrintLang {English | French | Dutch}
 *      int B_PrintMulti(const std::string& multi, int unused)                      | $PrintMulti STACKMODE
 *      int B_PrintA2mAppend(const std::string& arg, int unused)                    | $PrintA2mAppend [NO|Yes]
 *      int B_PrintTBreak(const std::string& arg, int unused)                       | $PrintTableBreak [NO|Yes]
 *      int B_PrintTPage(const std::string& arg, int unused)                        | $PrintTablePage  [NO|Yes]
 *      int B_PrintGPage(const std::string& arg, int unused)                        | $PrintGraphPage [NO|Yes]
 *      int B_PrintParaNum(const std::string& arg, int unused)                      | $PrintParanum [NO|Yes]
 *      int B_PrintPageHeader(const std::string& arg, int unused)                   | $PrintPageHeader following_pages_title
 *      int B_PrintPageFooter(const std::string& arg, int unused)                   | $PrintPageFooter following_pages_footer
 *      int B_PrintFont(const std::string& arg, int unused)                         | $PrintFont Times|Helvetica|Courier|Bookman|Palatino [size [incr]]
 *      int B_PrintTFont(const std::string& arg, int unused)                        | $PrintTableFont Times|Helvetica|Courier|Bookman|Palatino [size]
 *      int B_PrintTBox(const std::string& arg, int unused)                         | $PrintTableBox n
 *      int B_PrintTColor(const std::string& arg, int unused)                       | $PrintTableColor [NO|Yes]
 *      int B_PrintTWidth(const std::string& arg, int unused)                       | $PrintTableWidth width [col1 [coln]]
 *      int B_PrintGSize(const std::string& arg, int unused)                        | $PrintGraphSize  width [height] [fontsize]
 *      int B_PrintGTheme(const std::string& arg, int unused)                       | $PrintGraphTheme theme
 *      int B_PrintGBand(const std::string& arg, int unused)                        | $PrintGraphBand [per_from per_to]
 *      int B_PrintGBox(const std::string& arg, int unused)                         | $PrintGraphBox n
 *      int B_PrintGBrush(const std::string& arg, int unused)                       | $PrintGraphBrush pct|Yes
 *      int B_GetColor(const std::string& arg, int unused)             | Sub function of B_PrintColor() to interpret color names.
 *      int B_PrintGColor(const std::string& arg, int unused)                       | $PrintBackground Black|Blue|Magenta|Cyan|Red|Green|Yellow|White
 *      int B_PrintRtfHelp(const std::string& arg, int unused)                      | $PrintRtfHelp [YES|No]
 *      int B_PrintHtmlHelp(const std::string& arg, int unused)                     | $PrintHtmlHelp [YES|No]
 *      int B_PrintRtfTitle(const std::string& arg, int unused)                     | $PrintRtfTitle Help title
 *      int B_PrintRtfCopy(const std::string& arg, int unused)                      | $PrintRtfCopyright copyright text
 *      int B_PrintRtfLevel(const std::string& arg, int unused)                     | $PrintRtfLevel [+|-|n]
 *      int B_PrintRtfTopic(const std::string& arg, int unused)                     | $PrintRtfTopic topic title
 *      int B_PrintGdiOrient(const std::string& arg, int unused)                    | $PrintOrientation {Portrait|Landscape}
 *      int B_PrintGdiDuplex(const std::string& arg, int unused)                    | $PrintDuplex {Simplex|Duplex|VerticalDuplex}
 *      int B_PrintGdiPrinter(const std::string& arg, int unused)                   | $SetPrinter printer_name
 *      int B_PrintGIFBackColor(const std::string& arg, int unused)                 | $PrintGIFBackColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
 *      int B_PrintGIFTransColor(const std::string& arg, int unused)                | $PrintGIFTransColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
 *      int B_PrintGIFInterlaced(const std::string& arg, int unused)                | $PrintGIFInterlaced {Yes|No} 
 *      int B_PrintGIFTransparent(const std::string& arg, int unused)               | $PrintGIFTransparent {Yes|No}
 *      int B_PrintGIFFilled(const std::string& arg, int unused)                    | $PrintGIFilled {Yes|No}
 *      int B_PrintGIFFont(const std::string& arg, int unused)                      | $PrintGIFFont FontNb (between 0 and 5)
 *      int B_PrintHtmlStrip(const std::string& arg, int unused)                    | $PrintHtmlStrip [YES|No]
 *      int B_PrintHtmlStyle(const std::string& arg, int unused)                    | $PrintHtmlStyle filename
 *      int B_A2mToAll(const std::string& arg, int type)               | Convert an A2M file to another format.
 *      int B_A2mToPrinter(const std::string& arg, int unused)                      | $A2mToPrinter file.a2m
 *      int B_A2mToHtml(const std::string& arg, int unused)                         | $A2mToHtml filein fileout
 *      int B_A2mToRtf(const std::string& arg, int unused)                          | $A2mToRtf filein fileout
 *      int B_A2mToMif(const std::string& arg, int unused)                          | $A2mToMif filein fileout
 *      int B_A2mToCsv(const std::string& arg, int unused)                          | $A2mToCsv filein fileout
 *      int B_A2mSetCol(int *dest, int col)                            | Extracts a color definition from B_GIFCOLS and saves it in dest[3].
 *      int B_PrintHtmlTableClass(const std::string& table_class, int unused)       | $PrintHtmlTableClass class_name
 *      int B_PrintHtmlTRClass(const std::string& tr_class, int unused)             | $PrintHtmlTRClass class_name
 *      int B_PrintHtmlTHClass(const std::string& th_class, int unused)             | $PrintHtmlTHClass class_name
 *      int B_PrintHtmlTDClass(const std::string& td_class, int unused)             | $PrintHtmlTDClass class_name
 */
#include "api/iode_scr4.h"
#include "scr4/a2m/s_a2m.h"
#include "scr4/a2m/s_proa2m.h"

#include "api/pch.h"
#include "api/b_a2mini.h"
#include "api/b_errors.h"
#include "api/k_lang.h"
#include "api/print/print.h"
#include "api/write/write.h"
#include "api/report/undoc/undoc.h"


int A2M_GIF_BGCOLOR_NB;  // GIF background color nb
int A2M_GIF_TRCOLOR_NB;  // GIF transparent coor nb

/**
 *  Define the printing destination. Sub function of B_PrintDest() and B_PrintDestNew().
 *  
 *  @see W_dest().
 *  
 *  @param [in] file char*  output filename
 *  @param [in] type int    output type (W_HTML, W_MIF, W_RTF...)
 *  @return          int    0 always
 */
int B_PrintDestExt(char* file, int type)           
{
    W_dest(file, type);
    return 0;
}

/**
 *  Define the output file for the following printouts.
 *  
 *  @param [in] arg  const std::string&  filename followed by destination type (ex "testfile H") or
 *                                       empty to print on the printer
 *  @return description                  return code of B_PrintDestExt()
 */
int B_PrintDestFile(const std::string& arg)
{
    int     nb_args, type, rc;

    // If a printing session is currently open, that session is first flushed and closed
    W_flush();
    W_close();

    tbl_current_topic = 0;
    std::string dest = arg;
    SCR_strip((unsigned char*) dest.data());
    if(dest[0] != 0)
    {
        std::vector<std::string> args = split_skip_empty(std::string(dest.c_str()), " ,\n\t");
        nb_args = (int) args.size();
        if(nb_args > 1) 
        {
            switch(args[1][0]) 
            {
                case 'h':
                case 'H':
                    A2M_HTML_RELSTYLE = "iodestyle.css";
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
                    break;
            }
        }
        else 
            type = W_A2M;
        
        // NULL if arg only contains separators (as with SCR_vtoms())
        char* filename = (nb_args > 0) ? args[0].data() : NULL;
        rc = B_PrintDestExt(filename, type);
    }
    else 
        rc = B_PrintDestExt("", W_GDI);

    return rc;
}

//  $PrintDest [nom_fichier] [format]
//  where format = A2M, MIF, HTML, RTF or CSV
int B_PrintDest(const std::string& file, int unused)
{
    return(B_PrintDestFile(file));
}


// $PrintDestNew [nom_fichier] [format]
//  where format = A2M, MIF, HTML, RTF or CSV 
int B_PrintDestNew(const std::string& file, int unused)
{
    return(B_PrintDestFile(file));
}


// $PrintNbDec nb
int B_PrintNbDec(const std::string& nbdec, int unused)
{
    tbl_nb_decimals = atoi(nbdec.c_str());
    if(tbl_nb_decimals > 99 || (tbl_nb_decimals < 0 && tbl_nb_decimals != -1)) {
        error_manager.append_error(nbdec + ": invalid number of decimals (value = 2)");
        tbl_nb_decimals = 2;
        return -1;
    }
   return 0;
}

// $PrintLang {English | French | Dutch}
int B_PrintLang(const std::string& lang, int unused)       
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
            error_manager.append_error("Invalid language");
            return -1;
    }
    return 0;
}

// $PrintMulti STACKMODE
// where STACKMODE = None(=default), Stacked, Percent
// GB 30/10/2007 
int B_PrintMulti(const std::string& multi, int unused) 
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
            error_manager.append_error("Invalid PrintMulti option (None, Stacked, Percent)");
            return -1;
    }
    return 0;
}


// $PrintA2mAppend [NO|Yes]
int B_PrintA2mAppend(const std::string& arg, int unused)
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
    return 0;
}


// $PrintTableBreak [NO|Yes]
int B_PrintTBreak(const std::string& arg, int unused)
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
    return 0;
}


// $PrintTablePage  [NO|Yes]
int B_PrintTPage(const std::string& arg, int unused)
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
    return 0;
}


// $PrintGraphPage [NO|Yes]
int B_PrintGPage(const std::string& arg, int unused)
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
    return 0;
}


// $PrintParanum [NO|Yes]
int B_PrintParaNum(const std::string& arg, int unused)
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
    return 0;
}


// $PrintPageHeader following_pages_title
int B_PrintPageHeader(const std::string& arg, int unused)
{
    std::string header = arg;               // W_print_pg_header() takes a char*
    W_print_pg_header(header.data());       // JMP 27/6/2022
    return 0;
}


// $PrintPageFooter following_pages_footer
int B_PrintPageFooter(const std::string& arg, int unused)
{
    std::string footer = arg;               // W_print_pg_footer() takes a char*
    W_print_pg_footer(footer.data());       // JMP 27/6/2022
    return 0;
}


// $PrintFont Times|Helvetica|Courier|Bookman|Palatino [size [incr]]
int B_PrintFont(const std::string& arg, int unused)
{
    std::vector<std::string> tbl;
    int     ntbl = 0;

    tbl = split_skip_empty(std::string(arg), " ");
    ntbl = (int) tbl.size();
    if(ntbl > 0) {
        A2M_FONTFAMILY = SCR_upper_char((unsigned char) tbl[0][0]);
        // BGUI_PrintFont1();
    }
    if(ntbl > 1) {
        A2M_FONTSIZE = atoi(tbl[1].data());
        A2M_FONTSIZE = std::max(A2M_FONTSIZE, 4);
        // BGUI_PrintFont2();
    }

    if(ntbl > 2) {
        A2M_FONTINCR = atoi(tbl[2].data());
        A2M_FONTINCR = std::max(A2M_FONTINCR, 1);
        // BGUI_PrintFont3();
    }

    return 0;
}


// $PrintTableFont Times|Helvetica|Courier|Bookman|Palatino [size]
int B_PrintTFont(const std::string& arg, int unused)
{
    std::vector<std::string> tbl;
    int     ntbl = 0;

    tbl = split_skip_empty(std::string(arg), " ");
    ntbl = (int) tbl.size();
    if(ntbl > 0) {
        A2M_TFONTFAMILY = SCR_upper_char((unsigned char) tbl[0][0]);
        // BGUI_PrintTFont1();
    }
    if(ntbl > 1) {
        A2M_TFONTSIZE = atoi(tbl[1].data());
        A2M_TFONTSIZE = std::max(A2M_TFONTSIZE, 4);
        // BGUI_PrintTFont2();
    }
    return 0;
}


// $PrintTableBox  n
int B_PrintTBox(const std::string& arg, int unused)
{
    int     box = atoi(arg.c_str());

    if(box == 0 && SCR_upper_char(arg[0]) == 'Y') box = 1;
    A2M_FRM_TBORDER = box;
    A2M_HTML_TBORDER = box;
    A2M_RTF_TBORDER = box;
#ifndef UNIX
    A2M_GDI_TBORDER = box;
#endif
    // BGUI_PrintTBox();
    return 0;
}


// $PrintTableColor [NO|Yes]
int B_PrintTColor(const std::string& arg, int unused)
{
    int     box = atoi(arg.c_str());

    if(box == 0 && SCR_upper_char(arg[0]) == 'Y') box = 1;
    A2M_FRM_TCOLOR = box;
    A2M_HTML_TCOLOR = box;
    A2M_RTF_TCOLOR = box;
    // BGUI_PrintTColor();
    /*    A2M_GDI_TCOLOR = box; */
    return 0;
}


// $PrintTableWidth width [col1 [coln]]
int B_PrintTWidth(const std::string& arg, int unused)
{
    std::vector<std::string> tbl;
    int     ntbl = 0, w, col1, coln;

    tbl = split_skip_empty(std::string(arg), " ");
    ntbl = (int) tbl.size();
    if(ntbl > 0) {
        w = atoi(tbl[0].data());
        w = std::max(w, 65);
        A2M_RTF_TWIDTH = w;
        A2M_FRM_TWIDTH = w;
        // BGUI_PrintTWidth1();
    }

    if(ntbl > 1) {
        col1 = atoi(tbl[1].data());
        col1 = std::max(col1, 50);
        A2M_RTF_TCOL1 = col1;
        A2M_FRM_TCOL1 = col1;
        // BGUI_PrintTWidth2();
    }

    if(ntbl > 2) {
        coln = atoi(tbl[2].data());
        coln = std::max(coln, 15);
        A2M_RTF_TCOLN = coln;
        A2M_FRM_TCOLN = coln;
        // BGUI_PrintTWidth3();
    }

    return 0;
}


// $PrintGraphSize  width [height] [fontsize]
int B_PrintGSize(const std::string& arg, int unused)
{
    std::vector<std::string> tbl;
    int     ntbl = 0, w;

    tbl = split_skip_empty(std::string(arg), " ");
    ntbl = (int) tbl.size();
    if(ntbl > 0) {
        w = atoi(tbl[0].data());
        w = std::max(w, 35);
        A2M_GWIDTH = 0.1 * w;
        // BGUI_PrintGSize1();
    }

    if(ntbl > 1) {
        w = atoi(tbl[1].data());
        w = std::max(w, 20);
        A2M_GHEIGHT = 0.1 * w;
        // BGUI_PrintGSize2();
    }
    if(ntbl > 2) { // GB 16/03/2009
        A2M_FONTSIZE = atoi(tbl[2].data());
        A2M_FONTSIZE = std::max(A2M_FONTSIZE, 4);
        // BGUI_PrintFont2();
    }

    return 0;
}


// $PrintGraphTheme theme
// TODO: add to manual
int B_PrintGTheme(const std::string& arg, int unused)
{
#if !defined(_MSC_VER) && !defined(__GNUC__)    
    std::string theme = arg;
    ChrtThemeInit(theme.data());
#endif    
    return 0;
}


// $PrintGraphBand [per_from per_to]
// TODO: add to manual (see GB)
int B_PrintGBand(const std::string& arg, int unused)
{
#if !defined(_MSC_VER) && !defined(__GNUC__)    
    int     rc = 0, nbargs;
    double  startvalue = 0, endvalue = 0;

    std::vector<std::string> args = split_skip_empty(std::string(arg), " ");
    nbargs = (int) args.size();

    if(nbargs == 2)
    {
        Period  *per;

        per = new Period(args[0]);
        startvalue = PER_per2real(per, 0);
        delete per;
        per = nullptr;

        per = new Period(args[1]);
        endvalue = PER_per2real(per, 0);
        delete per;
        per = nullptr;
    }

    ChrtColorBand(startvalue, endvalue);
#endif    
    return 0;
}


// $PrintGraphBox n
int B_PrintGBox(const std::string& arg, int unused)
{
    int     box = atoi(arg.c_str());

    if(box == 0 && SCR_upper_char(arg[0]) == 'Y') box = 1;
    A2M_BOXWIDTH = box;
    // BGUI_PrintGBox();
    return 0;
}


// $PrintGraphBrush pct|Yes
int B_PrintGBrush(const std::string& arg, int unused)
{
    int     box = atoi(arg.c_str());

    if(box == 0 && SCR_upper_char(arg[0]) == 'Y') box = 50;
    A2M_BACKBRUSH = box;
    // BGUI_PrintGBrush();
    return 0;
}


// Sub function of B_PrintColor() to interpret color names.
// Returns -1 if arg is not a valid color name.
int B_GetColor(const std::string& arg, int unused)
{
    int     col = 0;

    std::string color = arg;
    SCR_lower((unsigned char*) color.data());
    if(color == "blue")                 col  = 'b';
    else if(color == "black")           col  = 'B';
    else if(color == "magenta")         col  = 'm';
    else if(color == "cyan")            col  = 'c';
    else if(color == "red")             col  = 'r';
    else if(color == "green")           col  = 'g';
    else if(color == "yellow")          col  = 'y';
    else if(color == "white")           col  = 'w';
    return(U_pos(col, (unsigned char*) "byBwrgcm"));
}


// $PrintBackground Black|Blue|Magenta|Cyan|Red|Green|Yellow|White
int B_PrintGColor(const std::string& arg, int unused)
{
    int     col;

    col = B_GetColor(arg);
    if(col < 0) col = 0;
    // BGUI_PrintGColor(col);
    return 0;
}


// $PrintRtfHelp [YES|No]
int B_PrintRtfHelp(const std::string& arg, int unused)
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
    return 0;
}


// $PrintHtmlHelp [YES|No]
int B_PrintHtmlHelp(const std::string& arg, int unused)
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
    return 0;
}


// $PrintRtfTitle Help title
int B_PrintRtfTitle(const std::string& arg, int unused)
{
    std::string title = arg;
    B_A2mSetRtfTitle((unsigned char*) title.data());
    // BGUI_PrintRtfTitle(arg);
    return 0;
}


// $PrintRtfCopyright copyright text
int B_PrintRtfCopy(const std::string& arg, int unused)
{
    std::string copyright = arg;
    B_A2mSetRtfCopy((unsigned char*) copyright.data());
    // BGUI_PrintRtfCopy(arg);
    return 0;
}


// $PrintRtfLevel [+|-|n]
int B_PrintRtfLevel(const std::string& arg, int unused)
{
    int     level = atoi(arg.c_str());

    if(level == 0) {
        if(arg[0] == '+') tbl_current_level++;
        else              tbl_current_level--;
    }
    else tbl_current_level = level;
    tbl_current_level = std::max(0, tbl_current_level);
    return 0;
}


// $PrintRtfTopic topic title
int B_PrintRtfTopic(const std::string& arg, int unused)
{   
    std::string topic = arg;                // W_print_rtf_topic() takes a char*
    W_print_rtf_topic(topic.data());
    return 0;
}


// $PrintOrientation {Portrait|Landscape}
int B_PrintGdiOrient(const std::string& arg, int unused)
{
    switch(SCR_upper_char(arg[0])) {
        case 'L' :
            W_gdiorient = 1;
            break;
        default  :
            W_gdiorient = 0;
            break;
    }

    // BGUI_PrintGdiOrient();
    return 0;
}


// $PrintDuplex {Simplex|Duplex|VerticalDuplex}
int B_PrintGdiDuplex(const std::string& arg, int unused)
{
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
    return 0;
}


// $SetPrinter printer_name
int B_PrintGdiPrinter(const std::string& arg, int unused)
{
    SCR_strlcpy((unsigned char*) W_gdiprinter, (unsigned char*) arg.c_str(), 70);
    return 0;
}


// $PrintGIFBackColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
int B_PrintGIFBackColor(const std::string& arg, int unused)
{
#if !defined(_MSC_VER) && !defined(__GNUC__)
    int     col;

    col = B_GetColor(arg);
    if(col < 0) col = 0;
    // BGUI_PrintGIFBackColor(col);
    B_A2mSetCol(A2M_GIF_BGCOLOR, col);
    A2M_GIF_BGCOLOR_NB = col; // JMP 1/5/2022
#endif    
    return 0;
}


// $PrintGIFTransColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
int B_PrintGIFTransColor(const std::string& arg, int unused)
{
#if !defined(_MSC_VER) && !defined(__GNUC__)
    int     col;

    col = B_GetColor(arg);
    if(col < 0) col = 0;
    // BGUI_PrintGIFTransColor(col);
    B_A2mSetCol(A2M_GIF_TRCOLOR, col);
    A2M_GIF_TRCOLOR_NB = col; // JMP 1/5/2022
#endif    
    return 0;
}


// $PrintGIFInterlaced {Yes|No} 
int B_PrintGIFInterlaced(const std::string& arg, int unused)
{
#if !defined(_MSC_VER) && !defined(__GNUC__)    
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
    return 0;
}


// $PrintGIFTransparent {Yes|No}
int B_PrintGIFTransparent(const std::string& arg, int unused)
{
#if !defined(_MSC_VER) && !defined(__GNUC__)
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
    return 0;
}


// $PrintGIFilled {Yes|No}
int B_PrintGIFFilled(const std::string& arg, int unused)
{
#if !defined(_MSC_VER) && !defined(__GNUC__)
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
    return 0;
}


// $PrintGIFFont FontNb (between 0 and 5)
int B_PrintGIFFont(const std::string& arg, int unused)
{
#if !defined(_MSC_VER) && !defined(__GNUC__)
    int     font;

    font = atoi(arg.c_str());
    if(font > 5 || font < 0) font = 1;
    // BGUI_PrintGIFFont(font);
    A2M_GIF_FONT = font + 1;

#endif    
    return 0;
}


// $PrintHtmlStrip [YES|No]
int B_PrintHtmlStrip(const std::string& arg, int unused)
{
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

    return 0;
}


// $PrintHtmlStyle filename
// TODO: add to manual

int B_PrintHtmlStyle(const std::string& arg, int unused) 
{
    A2M_HTML_RELSTYLE = (char*) SCR_stracpy((unsigned char*) arg.c_str());
    return 0;
}

/**
 *  Convert an A2M file to another format.
 *  Sub function of B_A2mTo*().
 *  
 *  @param [in] arg  const std::string&  "input_file output_file"
 *  @param [in] type int                 output type
 *  @return          int                 -1 on error, 0 on success
 */
int B_A2mToAll(const std::string& arg, int type)
{
    U_ch    ibuf[256], obuf[256], *ext;
    int     ntbl;

    std::vector<std::string> tbl = split_skip_empty(arg, " ");
    ntbl = (int) tbl.size();
    if(ntbl == 0) return -1;
    SCR_cat_ext((char*) ibuf, tbl[0].data(), (char*) "a2m");
    switch(type) {
        case A2M_DESTFRM :
            ext = (unsigned char*) "mif";
            break;
        case A2M_DESTHTML:
            ext = (unsigned char*) "htm";
            break;
        case A2M_DESTRTF :
            ext = (unsigned char*) "rtf";
            break;
        case A2M_DESTCSV :
            ext = (unsigned char*) "csv";
            break;
        default : 
            ext = (unsigned char*) "";           
            break;                  
    }

    if(ntbl > 1) SCR_cat_ext((char*) obuf, tbl[1].data(), (char*) ext);
    else         SCR_change_ext((char*) obuf, tbl[0].data(), (char*) ext);
    switch(type) {
        case A2M_DESTFRM :
            return(A2mToMif((char*) ibuf, (char*) obuf));
        case A2M_DESTHTML:
            return(A2mToHtml((char*) ibuf, (char*) obuf, W_htmlhelp));
        case A2M_DESTRTF :
            return(A2mToRtf((char*) ibuf, (char*) obuf, W_rtfhelp));
        case A2M_DESTCSV :
            return(A2mToCsv((char*) ibuf, (char*) obuf));
            case A2M_DESTGDIPRT :
#ifdef WIN32
            return(A2mToGdiPrinter(0L, 1, (unsigned char*) "Iode", ibuf));
#else
            return -1; // Not supported on non-Windows platforms
#endif
        default : 
            return -1;
    }
}



// $A2mToPrinter file.a2m
int B_A2mToPrinter(const std::string& arg, int unused)
{
    return(B_A2mToAll(arg, A2M_DESTGDIPRT));
}


// $A2mToHtml filein fileout
int B_A2mToHtml(const std::string& arg, int unused)
{
    return(B_A2mToAll(arg, A2M_DESTHTML));
}


// $A2mToRtf filein fileout
int B_A2mToRtf(const std::string& arg, int unused)
{
    return(B_A2mToAll(arg, A2M_DESTRTF));
}


// $A2mToMif filein fileout
int B_A2mToMif(const std::string& arg, int unused)
{
    return(B_A2mToAll(arg, A2M_DESTFRM));
}


// $A2mToCsv filein fileout
int B_A2mToCsv(const std::string& arg, int unused)
{
    return(B_A2mToAll(arg, A2M_DESTCSV));
}

// RGB values used to construct GIF graphs
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


/**
 *  Extracts a color definition from B_GIFCOLS and saves it in dest[3].
 *  
 *  @param [out] dest   int[3]  output vector of RgB colors
 *  @param [in]  col    int     color position in B_GIFCOLS 
 *  @return             int     0   
 */
int B_A2mSetCol(int *dest, int col)
{
    dest[0]= B_GIFCOLS[col][0];
    dest[1]= B_GIFCOLS[col][1];
    dest[2]= B_GIFCOLS[col][2];
    return 0;
}


// $PrintHtmlTableClass class_name
// TODO: add to manual
int B_PrintHtmlTableClass(const std::string& table_class, int unused)
{
    SCR_free(A2M_HTML_TABLECLASS);
    A2M_HTML_TABLECLASS = (char*) SCR_stracpy((unsigned char*) table_class.c_str());
    return 0;
}

// $PrintHtmlTRClass class_name
// TODO: add to manual
int B_PrintHtmlTRClass(const std::string& tr_class, int unused)
{
    SCR_free(A2M_HTML_TRCLASS);
    A2M_HTML_TRCLASS = (char*) SCR_stracpy((unsigned char*) tr_class.c_str());
    return 0;
}


// $PrintHtmlTHClass class_name
// TODO: add to manual
int B_PrintHtmlTHClass(const std::string& th_class, int unused)
{
    SCR_free(A2M_HTML_THCLASS);
    A2M_HTML_THCLASS = (char*) SCR_stracpy((unsigned char*) th_class.c_str());
    return 0;
}


// $PrintHtmlTDClass class_name
// TODO: add to manual
int B_PrintHtmlTDClass(const std::string& td_class, int unused)
{
    SCR_free(A2M_HTML_TDCLASS);
    A2M_HTML_TDCLASS = (char*) SCR_stracpy((unsigned char*) td_class.c_str());
    return 0;
}
