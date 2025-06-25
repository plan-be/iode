/**
 *  @header4iode
 * 
 *  Functions to read and write A2M parameters from and into the current iode.ini file (defined in ODE_INIFILE). 
 *  
 *  See http://www.xon.be/scr4/libs1/libs1454.htm for more details on the syntax and the global variables used by the A2M engine.
 *  
 *  List of functions
 *  -----------------
 *      void B_A2mGetGnlParms()             Reads the Section "A2MGNL" of the iode.ini file 
 *      void B_A2mSaveGnlParms()            Rewrites the Section "A2MGNL" of the iode.ini file with the current values of A2M* variables.
 *      void B_A2mGetRtfParms()             Reads the Section "A2MRTF" of the iode.ini file
 *      void B_A2mSetRtfTitle(U_ch* title)  Modifies the RTF title (A2M_RTF_TITLE)
 *      void B_A2mSetRtfCopy(U_ch* title)   Modifies the RTF copyrights text (A2M_RTF_COPYRIGHT) 
 *      void B_A2mSaveRtfParms()            Rewrites the Section "A2MRTF" of the iode.ini file with the current values of A2M* variables.
 *      void B_A2mGetHtmlParms()            Reads the Section "A2MHTML" of the iode.ini file
 *      void B_A2mSaveHtmlParms()           Rewrites the Section "A2MHTML" of the iode.ini file with the current values of A2M* variables.
 *      void B_A2mGetGIFParms()             Reads the Section "A2MGIF" of the iode.ini file
 *      void B_A2mSaveGIFParms()            Rewrites the Section "A2MGIF" of the iode.ini file with the current values of A2M* variables.
 *      void B_A2mGetCsvParms()             Reads the Section "A2MCSV" of the iode.ini file
 *      void B_A2mSaveCsvParms()            Rewrites the Section "A2MCSV" of the iode.ini file with the current values of A2M* variables.
 *      void B_A2mGetMifParms()             Reads the Section "A2MMIF" of the iode.ini file
 *      void B_A2mSaveMifParms()            Rewrites the Section "A2MMIF" of the iode.ini file with the current values of A2M* variables.
 *      void B_A2mGetGdiParms()             Reads the Section "A2MGDI" of the iode.ini file
 *      void B_A2mSaveGdiParms()            Rewrites the Section "A2MGDI" of the iode.ini file with the current values of A2M* variables.
 *      void B_A2mGetAllParms()             Reads all sections of iode.in related to A2M.
 *      void B_A2mSaveAllParms()            Rewrites all sections of iode.in related to A2M  with the current values of A2M* variables.
 *  
 *  
 *  List of global variables
 *  ------------------------
 *      double   A2M_GWIDTH = 9.5       Default graph width in cm
 *      double   A2M_GHEIGHT = 6.5      Default graph height in cm
 *      int      A2M_BOXWIDTH = 1       Width of the box around the graphs (0 for no box)
 *      int      A2M_BACKBRUSH = 50     Intensity of the graph background brushes
 *      int      A2M_BACKCOLOR = 'b'    Color of the graph background: one of "Bwrgbcmy"
 *      int      A2M_GIF_BGCOLOR_NB     Number of the BG color of the gif graphs // JMP 1/5/2022
 *      int      A2M_GIF_TRCOLOR_NB     Number of the TR color of the gif graphs // JMP 1/5/2022 
 *  
 */

#include "b_a2mini.h"
#include "b_iodeini.h"
#include <s_a2m.h>

// Variables
// ---------
double   A2M_GWIDTH = 9.5;     // Default graph width in cm
double   A2M_GHEIGHT = 6.5;    // Default graph height in cm
int      A2M_BOXWIDTH = 1;     // Width of the box around the graphs (0 for no box)
int      A2M_BACKBRUSH = 50;   // Intensity of the graph background brushes
int      A2M_BACKCOLOR = 'b';  // Color of the graph background: one of "Bwrgbcmy"
int      A2M_GIF_BGCOLOR_NB;   // Number of the BG color of the gif graphs // JMP 1/5/2022
int      A2M_GIF_TRCOLOR_NB;   // Number of the TR color of the gif graphs // JMP 1/5/2022 

/**
 *  Reads the Section "A2MGNL" of the iode.ini file and stores the
 *  read values in A2M* variables (see code for the complete list).
 *  
 *  These variables are A2M data used for reading A2M files and 
 *  for generating most of the output formats.
 *  
 *  Some other global vars are also (re-)defined here: W_a2mapp.
 *  
 *  See http://www.xon.be/scr4/libs1/libs1456.htm for more informations.
 */
void B_A2mGetGnlParms()
{
    extern int W_a2mapp; /* JMP 25-02-98 */

    A2M_ESCCH   = B_IniReadChar("A2MGNL", "ESC", '\\');     // See https://iode.plan.be/doku.php?id=caracteres_reserves
    A2M_DEFCH   = B_IniReadChar("A2MGNL", "DEF", '&');      // Id.
    A2M_CMDCH   = B_IniReadChar("A2MGNL", "CMD", '.');      // Id.
    //A2M_SEPCH   = B_IniReadChar("A2MGNL", "SEP", '@');      // id.
    A2M_SEPCH   = B_IniReadChar("A2MGNL", "SEP", '&');      // id.
    //KT_sep = A2M_SEPCH;                                     // JMP 1/5/2022

    A2M_LFON    = B_IniReadYN("A2MGNL", "LFON",   0);       // Keep special chars (carriage return, linefeed). See https://iode.plan.be/doku.php?id=blancs_et_sauts_de_lignes
    A2M_BLON    = B_IniReadYN("A2MGNL", "BLON",   0);       // id.
    W_a2mapp    = B_IniReadYN("A2MGNL", "APPEND", 0);       
    A2M_GWIDTH  = 0.1 * B_IniReadNum("A2MGNL", "GW", 160);      
    A2M_GHEIGHT = 0.1 * B_IniReadNum("A2MGNL", "GH", 100);

    A2M_BACKCOLOR = B_IniReadChar("A2MGNL", "COL", 'b');
    A2M_BACKBRUSH = B_IniReadNum ("A2MGNL", "PCT", 0);
    A2M_BOXWIDTH  = B_IniReadNum ("A2MGNL", "BOX", 0);
    
    B_IniReadText("A2MGNL", "TAG", A2M_CURTAG, 40, "par_1");
    SCR_sqz(A2M_CURTAG);
}

/**
 *  Rewrites the Section "A2MGNL" of the iode.ini file with the current values of A2M* variables.
 */
void B_A2mSaveGnlParms()
{
    extern int W_a2mapp; /* JMP 25-02-98 */
    
    B_IniWriteChar("A2MGNL", "ESC", A2M_ESCCH);             // See https://iode.plan.be/doku.php?id=caracteres_reserves
    B_IniWriteChar("A2MGNL", "SEP", A2M_SEPCH);             // Id.
    B_IniWriteChar("A2MGNL", "DEF", A2M_DEFCH);             // Id.
    B_IniWriteChar("A2MGNL", "CMD", A2M_CMDCH);             // Id.

    B_IniWriteYN("A2MGNL", "LFON",   A2M_LFON);             // See https://iode.plan.be/doku.php?id=blancs_et_sauts_de_lignes
    B_IniWriteYN("A2MGNL", "BLON",   A2M_BLON);             // id.
    B_IniWriteYN("A2MGNL", "APPEND", W_a2mapp);
    B_IniWriteNum("A2MGNL", "GW",    (int)(10 * A2M_GWIDTH ));  
    B_IniWriteNum("A2MGNL", "GH",    (int)(10 * A2M_GHEIGHT));

    B_IniWriteChar("A2MGNL", "COL", A2M_BACKCOLOR);         
    B_IniWriteNum ("A2MGNL", "PCT", A2M_BACKBRUSH);
    B_IniWriteNum ("A2MGNL", "BOX", A2M_BOXWIDTH );
    
    SCR_sqz(A2M_CURTAG);
    B_IniWriteText("A2MGNL", "TAG", A2M_CURTAG);
}

/**
 *  Modifies the RTF title (A2M_RTF_TITLE = title of the .hlp help file).
 *  title is transformed in ANSI coding when saved in A2M_RTF_TITLE.
 *  
 *  @param [in] unsigned char* title    title in OEM coding
 */
void B_A2mSetRtfTitle(U_ch* title)
{
    if(title == 0) return;
    SCR_free(A2M_RTF_TITLE);
    A2M_RTF_TITLE = SCR_stracpy(title);
    SCR_strip(A2M_RTF_TITLE);
    SCR_OemToAnsi(A2M_RTF_TITLE, A2M_RTF_TITLE);
}


/**
 *  Modifies the RTF copyrights text (A2M_RTF_COPYRIGHT = copyright text of the .hlp help file).
 *  title is transformed in ANSI coding when saved in A2M_RTF_COPYRIGHT.
 *  
 *  @param [in] unsigned char* title    title in OEM coding
 */
void B_A2mSetRtfCopy(U_ch* title)
{
    if(title == 0) return;
    SCR_free(A2M_RTF_COPYRIGHT);
    A2M_RTF_COPYRIGHT = SCR_stracpy(title);
    SCR_strip(A2M_RTF_COPYRIGHT);
    SCR_OemToAnsi(A2M_RTF_COPYRIGHT, A2M_RTF_COPYRIGHT);
}


/**
 *  Reads the Section "A2MRTF" of the iode.ini file and stores the
 *  read values in A2M* variables (see code for the complete list).
 *  
 *  These variables are A2M parameters used during the generation of files 
 *  in RTF format.
 *  
 *  See http://www.xon.be/scr4/libs1/libs1456.htm for more informations.
 */
void B_A2mGetRtfParms()
{
//    B_A2mGetGnlParms();
    char title[512];

    A2M_NUMBERS      = B_IniReadYN  ("A2MRTF", "PARANUM",       0);    
    A2M_FONTSIZE     = B_IniReadNum ("A2MRTF", "FONTSIZE",      10);
    A2M_FONTFAMILY   = B_IniReadChar("A2MRTF", "FONTFAMILY",    'H');
    A2M_FONTINCR     = B_IniReadNum ("A2MRTF", "FONTINCR",      2);

    A2M_RTF_TWIDTH   = B_IniReadNum ("A2MRTF", "TWIDTH",        165);
    A2M_RTF_TCOL1    = B_IniReadNum ("A2MRTF", "TCOL1",         60);
    A2M_RTF_TCOLN    = B_IniReadNum ("A2MRTF", "TCOLN",         15);
    A2M_TFONTFAMILY  = B_IniReadChar("A2MRTF", "TFONTFAMILY",   'H');
    A2M_TFONTSIZE    = B_IniReadNum ("A2MRTF", "TFONTSIZE",     10);

    A2M_RTF_TCOLOR   = B_IniReadYN  ("A2MRTF", "TCOLOR",        0);
    A2M_RTF_TSPLIT   = B_IniReadYN  ("A2MRTF", "TSPLIT",        0);
    A2M_RTF_TBORDER  = B_IniReadYN  ("A2MRTF", "TBORDER",       0);
    A2M_RTF_VLINES   = B_IniReadYN  ("A2MRTF", "TVLINES",       0);
    A2M_RTF_HLINES   = B_IniReadYN  ("A2MRTF", "THLINES",       0);
    
    A2M_RTF_COMPRESS = B_IniReadYN  ("A2MRTF", "COMPRESS",      0);
    A2M_RTF_TOC      = B_IniReadYN  ("A2MRTF", "TOC",           0);
    W_rtfhelp        = B_IniReadYN  ("A2MRTF", "HCW",           0);
    
    B_IniReadText("A2MRTF", "TITLE", title, 511, "");
    B_A2mSetRtfTitle(title);
    B_IniReadText("A2MRTF", "COPYR", title, 511, "");
    B_A2mSetRtfCopy(title);
}


/**
 *  Rewrites the Section "A2MRTF" of the iode.ini file  with the current values of A2M* variables.
 *  
 *  See http://www.xon.be/scr4/libs1/libs1456.htm for more informations.
 */
void B_A2mSaveRtfParms()
{
//    B_A2mGetGnlParms();
    char title[512];

    B_IniWriteYN  ("A2MRTF", "PARANUM",    A2M_NUMBERS    );
    B_IniWriteNum ("A2MRTF", "FONTSIZE",   A2M_FONTSIZE   );
    B_IniWriteChar("A2MRTF", "FONTFAMILY", A2M_FONTFAMILY );
    B_IniWriteNum ("A2MRTF", "FONTINCR",   A2M_FONTINCR   );

    B_IniWriteNum ("A2MRTF", "TWIDTH",      A2M_RTF_TWIDTH );
    B_IniWriteNum ("A2MRTF", "TCOL1",       A2M_RTF_TCOL1  );
    B_IniWriteNum ("A2MRTF", "TCOLN",       A2M_RTF_TCOLN  );
    B_IniWriteChar("A2MRTF", "TFONTFAMILY", A2M_TFONTFAMILY);
    B_IniWriteNum ("A2MRTF", "TFONTSIZE",   A2M_TFONTSIZE  );

    B_IniWriteYN  ("A2MRTF", "TCOLOR",  A2M_RTF_TCOLOR );
    B_IniWriteYN  ("A2MRTF", "TSPLIT",  A2M_RTF_TSPLIT );
    B_IniWriteYN  ("A2MRTF", "TBORDER", A2M_RTF_TBORDER);
    B_IniWriteYN  ("A2MRTF", "TVLINES", A2M_RTF_VLINES );
    B_IniWriteYN  ("A2MRTF", "THLINES", A2M_RTF_HLINES );
    
    B_IniWriteYN  ("A2MRTF", "COMPRESS", A2M_RTF_COMPRESS);
    B_IniWriteYN  ("A2MRTF", "TOC",      A2M_RTF_TOC     );
    B_IniWriteYN  ("A2MRTF", "HCW",      W_rtfhelp       );
    
    SCR_strlcpy(title, A2M_RTF_TITLE, sizeof(title) - 1);
    SCR_AnsiToOem(title, title);
    B_IniWriteText("A2MRTF", "TITLE", title); 

    SCR_strlcpy(title, A2M_RTF_COPYRIGHT, sizeof(title) - 1);
    SCR_AnsiToOem(title, title);
    B_IniWriteText("A2MRTF", "COPYR", title); 
}


/**
 *  Reads the Section "A2MHTML" of the iode.ini file and stores the
 *  read values in A2M* variables (see code for the complete list).
 *  
 *  These variables are A2M parameters used during the generation of files 
 *  in HTML format.
 *  
 *  See also http://www.xon.be/scr4/libs1/libs1460.htm.
 */
void B_A2mGetHtmlParms()
{
    char buffer[512];
    //B_A2mGetGnlParms();
    A2M_NUMBERS      = B_IniReadYN  ("A2MHTML", "PARANUM", 0);
    A2M_FONTSIZE     = B_IniReadNum ("A2MHTML", "FONTSIZE", 10);
    A2M_FONTFAMILY   = B_IniReadChar("A2MHTML", "FONTFAMILY", 'H');
    A2M_FONTINCR     = B_IniReadNum ("A2MHTML", "FONTINCR", 2);
    A2M_TFONTFAMILY  = B_IniReadChar("A2MHTML", "TFONTFAMILY", 'H');
    A2M_TFONTSIZE    = B_IniReadNum ("A2MHTML", "TFONTSIZE", 10);
    A2M_HTML_TCOLOR  = B_IniReadYN  ("A2MHTML", "TCOLOR", 0);
    A2M_HTML_TBORDER = B_IniReadYN  ("A2MHTML", "TBORDER", 0);
    A2M_HTML_TOC     = B_IniReadYN  ("A2MHTML", "TOC", 0);

    SCR_free(A2M_HTML_TITLE);
    B_IniReadText("A2MHTML", "TITLE", buffer, 511, "");
    A2M_HTML_TITLE = SCR_stracpy(buffer);
    SCR_strip(A2M_HTML_TITLE);

    SCR_free(A2M_HTML_BODY);
    B_IniReadText("A2MHTML", "BODYTAG", buffer, 511, "");
    A2M_HTML_BODY = SCR_stracpy(buffer);
    SCR_strip(A2M_HTML_BODY);   
}


/**
 *  Rewrites the Section "A2MHTML" of the iode.ini file  with the current values of A2M* variables.
 *  
 *  See also http://www.xon.be/scr4/libs1/libs1460.htm.
 */
void B_A2mSaveHtmlParms()
{
    B_IniWriteYN  ("A2MHTML", "PARANUM",     A2M_NUMBERS      );
    B_IniWriteNum ("A2MHTML", "FONTSIZE",    A2M_FONTSIZE     );
    B_IniWriteChar("A2MHTML", "FONTFAMILY",  A2M_FONTFAMILY   );
    B_IniWriteNum ("A2MHTML", "FONTINCR",    A2M_FONTINCR     );
    B_IniWriteChar("A2MHTML", "TFONTFAMILY", A2M_TFONTFAMILY  );
    B_IniWriteNum ("A2MHTML", "TFONTSIZE",   A2M_TFONTSIZE    );
    B_IniWriteYN  ("A2MHTML", "TCOLOR",      A2M_HTML_TCOLOR  );
    B_IniWriteYN  ("A2MHTML", "TBORDER",     A2M_HTML_TBORDER );
    B_IniWriteYN  ("A2MHTML", "TOC",         A2M_HTML_TOC     );
    B_IniWriteText("A2MHTML", "TITLE",       A2M_HTML_TITLE   );
    B_IniWriteText("A2MHTML", "BODYTAG",     A2M_HTML_BODY    );
}


// ANNULE JMP 4/5/2022
void B_A2mGetGIFParms() {}
void B_A2mSaveGIFParms() {}
/*
void B_A2mGetGIFParms()
{
    int col;

    col = B_IniReadNum ("A2MGIF", "BGCOLOR", 1); 
    if(col > 7 || col < 0) col = 1;
    B_A2mSetCol(A2M_GIF_BGCOLOR, col);
    A2M_GIF_BGCOLOR_NB = col;
    
    col = B_IniReadNum ("A2MGIF", "TRCOLOR", 1); 
    if(col > 7 || col < 0) col = 1;
    B_A2mSetCol(A2M_GIF_TRCOLOR, col);
    A2M_GIF_TRCOLOR_NB = col;

    A2M_GIF_TRANS     = B_IniReadYN  ("A2MGIF", "TRANS", 0);
    A2M_GIF_INTER     = B_IniReadYN  ("A2MGIF", "INTER", 0);
    A2M_GIF_FONT      = B_IniReadNum ("A2MGIF", "FONT",  1);
    A2M_GIF_FILLED    = B_IniReadYN  ("A2MGIF", "FILLED", 0);    
}

void B_A2mSaveGIFParms()
{
    B_IniWriteNum("A2MGIF", "BGCOLOR", A2M_GIF_BGCOLOR_NB);
    B_IniWriteNum("A2MGIF", "TRCOLOR", A2M_GIF_TRCOLOR_NB);

    B_IniWriteYN  ("A2MGIF", "TRANS",  A2M_GIF_TRANS );
    B_IniWriteYN  ("A2MGIF", "INTER",  A2M_GIF_INTER );
    B_IniWriteNum ("A2MGIF", "FONT",   A2M_GIF_FONT  );
    B_IniWriteYN  ("A2MGIF", "FILLED", A2M_GIF_FILLED);    
}
*/
void B_A2mGetCsvParms() {}
void B_A2mSaveCsvParms() {}


/**
 *  Reads the Section "A2MMIF" of the iode.ini file and stores the
 *  read values in A2M* variables (see code for the complete list).
 *  
 *  These variables are A2M parameters used during the generation of files 
 *  in MIF (FrameMaker) format.
 *  
 *  See also http://www.xon.be/scr4/libs1/libs1458.htm.
 */
void B_A2mGetMifParms()
{
    //B_A2mGetGnlParms();
    
    A2M_FONTSIZE     = B_IniReadNum ("A2MMIF", "FONTSIZE", 10);
    A2M_FONTFAMILY   = B_IniReadChar("A2MMIF", "FONTFAMILY", 'H');
    A2M_FONTINCR     = B_IniReadNum ("A2MMIF", "FONTINCR", 2);

    A2M_TFONTFAMILY  = B_IniReadChar("A2MMIF", "TFONTFAMILY", 'H');
    A2M_TFONTSIZE    = B_IniReadNum ("A2MMIF", "TFONTSIZE", 10);
                                         
    A2M_FRM_TCOLOR   = B_IniReadYN  ("A2MMIF", "TCOLOR", 0);
    A2M_FRM_TCOL1    = B_IniReadNum ("A2MMIF", "TCOL1", 60);
    A2M_FRM_TCOLN    = B_IniReadNum ("A2MMIF", "TCOLN", 15);
    A2M_FRM_TSPLIT   = B_IniReadYN  ("A2MMIF", "TSPLIT", 0);
    A2M_FRM_TWIDTH   = B_IniReadNum ("A2MMIF", "TWIDTH", 165);
    A2M_FRM_TBORDER  = B_IniReadYN  ("A2MMIF", "TBORDER", 0);
    A2M_FRM_VLINES   = B_IniReadYN  ("A2MMIF", "TVLINES", 0);
    A2M_FRM_HLINES   = B_IniReadYN  ("A2MMIF", "THLINES", 0);
    A2M_FRM_REF      = B_IniReadYN  ("A2MMIF", "IMGREF", 0);
}


/**
 *  Rewrites the Section "A2MMIF" of the iode.ini file  with the current values of A2M* variables.
 *  
 *  See also http://www.xon.be/scr4/libs1/libs1458.htm.
 */
void B_A2mSaveMifParms()
{
    B_IniWriteNum ("A2MMIF", "FONTSIZE",      A2M_FONTSIZE   );
    B_IniWriteChar("A2MMIF", "FONTFAMILY",    A2M_FONTFAMILY );
    B_IniWriteNum ("A2MMIF", "FONTINCR",      A2M_FONTINCR   );
                                                            
    B_IniWriteChar("A2MMIF", "TFONTFAMILY",   A2M_TFONTFAMILY);
    B_IniWriteNum ("A2MMIF", "TFONTSIZE",     A2M_TFONTSIZE  );
                                                            
    B_IniWriteYN  ("A2MMIF", "TCOLOR",        A2M_FRM_TCOLOR );
    B_IniWriteNum ("A2MMIF", "TCOL1",         A2M_FRM_TCOL1  );
    B_IniWriteNum ("A2MMIF", "TCOLN",         A2M_FRM_TCOLN  );
    B_IniWriteYN  ("A2MMIF", "TSPLIT",        A2M_FRM_TSPLIT );
    B_IniWriteNum ("A2MMIF", "TWIDTH",        A2M_FRM_TWIDTH );
    B_IniWriteYN  ("A2MMIF", "TBORDER",       A2M_FRM_TBORDER);
    B_IniWriteYN  ("A2MMIF", "TVLINES",       A2M_FRM_VLINES );
    B_IniWriteYN  ("A2MMIF", "THLINES",       A2M_FRM_HLINES );
    B_IniWriteYN  ("A2MMIF", "IMGREF",        A2M_FRM_REF    );
}


/**
 *  Reads the Section "A2MGDI" (Windows printer) of the iode.ini file and stores the
 *  read values in A2M* variables (see code for the complete list).
 *  
 *  These variables are A2M parameters used to print A2M formated files.
 *  
 *  See also http://www.xon.be/scr4/libs1/libs1465.htm.
 */
void B_A2mGetGdiParms()
{
    extern  int W_gdiask; /* GB 23/03/98 */
    extern  int W_gdiduplex, W_gdiorient; /* JMP 18-04-98 */
    int     table_shading;
    char    buffer[512];

    // B_A2mGetGnlParms();
    W_gdiask         = B_IniReadYN  ("A2MGDI", "PROMPT", 0);
    A2M_FONTSIZE     = B_IniReadNum ("A2MGDI", "FONTSIZE", 10);
    A2M_FONTFAMILY   = B_IniReadChar("A2MGDI", "FONTFAMILY", 'H');
    A2M_FONTINCR     = B_IniReadNum ("A2MGDI", "FONTINCR", 2);

    A2M_TFONTFAMILY  = B_IniReadChar("A2MGDI", "TFONTFAMILY", 'H');
    A2M_TFONTSIZE    = B_IniReadNum ("A2MGDI", "TFONTSIZE", 10);
    A2M_NUMBERS      = B_IniReadYN  ("A2MGDI", "PARANUM", 0);;

#ifndef UNIX
    A2M_GDI_LMARG   = B_IniReadNum ("A2MGDI", "LMARG", 10);
    A2M_GDI_RMARG   = B_IniReadNum ("A2MGDI", "RMARG", 10);
    A2M_GDI_TMARG   = B_IniReadNum ("A2MGDI", "TMARG", 10);
    A2M_GDI_BMARG   = B_IniReadNum ("A2MGDI", "BMARG", 10);

    A2M_GDI_COLOR   = !B_IniReadYN ("A2MGDI", "COLOR", 0);
    A2M_GDI_TBORDER = B_IniReadNum("A2MGDI", "TBORDER", 0);
#endif
    A2M_TBREAK      = B_IniReadYN ("A2MGDI", "TABLEBREAK", 0);
    A2M_TPAGE       = B_IniReadYN ("A2MGDI", "TABLEPAGE", 0);
    A2M_GPAGE       = B_IniReadYN ("A2MGDI", "GRAPHPAGE", 0);
    
    table_shading   = B_IniReadYN ("A2MGDI", "TABLESHADING", 0);
    if(table_shading) {
        A2M_TSHADING_BRUSH[0] = 7;
        A2M_TSHADING_BRUSH[1] = 5;
	}
    else {
        A2M_TSHADING_BRUSH[0] = 0;
        A2M_TSHADING_BRUSH[1] = 0;
	}
   
    SCR_free(A2M_PGHEAD);
    B_IniReadText("A2MGDI", "PGHEADER", buffer, 511, "");
    A2M_PGHEAD = SCR_stracpy(buffer);
    SCR_strip(A2M_PGHEAD);   
    
    SCR_free(A2M_PGFOOT);
    B_IniReadText("A2MGDI", "PGFOOTER", buffer, 511, "");
    A2M_PGFOOT = SCR_stracpy(buffer);
    SCR_strip(A2M_PGFOOT);   

    //W_gdiask    = p_a2mgdi_PGPROMPT;    /* GB 23/03/98 */
    //W_gdiduplex = p_a2mgdi_DUPLEX;    // JMP 22/8/2016
    //W_gdiorient = p_a2mgdi_ORIENT;    // JMP 22/8/2016
}


/**
 *  Rewrites the Section "A2MGDI" of the iode.ini file  with the current values of A2M* variables.
 *  
 *  See also http://www.xon.be/scr4/libs1/libs1465.htm.
 */
void B_A2mSaveGdiParms()
{
    extern  int W_gdiask;                 /* GB 23/03/98 */
    extern  int W_gdiduplex, 
                W_gdiorient;              /* JMP 18-04-98 */

    // B_A2mGetGnlParms();
    B_IniWriteYN  ("A2MGDI", "PROMPT",      W_gdiask);
    B_IniWriteNum ("A2MGDI", "FONTSIZE",    A2M_FONTSIZE   );
    B_IniWriteChar("A2MGDI", "FONTFAMILY",  A2M_FONTFAMILY );
    B_IniWriteNum ("A2MGDI", "FONTINCR",    A2M_FONTINCR   );
                                            
    B_IniWriteChar("A2MGDI", "TFONTFAMILY", A2M_TFONTFAMILY);
    B_IniWriteNum ("A2MGDI", "TFONTSIZE",   A2M_TFONTSIZE  );
    B_IniWriteYN  ("A2MGDI", "PARANUM",     A2M_NUMBERS    );

#ifndef UNIX
    B_IniWriteNum ("A2MGDI", "LMARG",       A2M_GDI_LMARG  );
    B_IniWriteNum ("A2MGDI", "RMARG",       A2M_GDI_RMARG  );
    B_IniWriteNum ("A2MGDI", "TMARG",       A2M_GDI_TMARG  );
    B_IniWriteNum ("A2MGDI", "BMARG",       A2M_GDI_BMARG  );
                                            
    B_IniWriteYN  ("A2MGDI", "COLOR",       !A2M_GDI_COLOR  );
    B_IniWriteNum ("A2MGDI", "TBORDER",      A2M_GDI_TBORDER);
#endif            
    B_IniWriteYN  ("A2MGDI", "TABLEBREAK",   A2M_TBREAK     );
    B_IniWriteYN  ("A2MGDI", "TABLEPAGE",    A2M_TPAGE      );
    B_IniWriteYN  ("A2MGDI", "GRAPHPAGE",    A2M_GPAGE      );
    
    B_IniWriteYN ("A2MGDI", "TABLESHADING", A2M_TSHADING_BRUSH[0]);
   
    B_IniWriteText("A2MGDI", "PGHEADER", A2M_PGHEAD);
    B_IniWriteText("A2MGDI", "PGFOOTER", A2M_PGFOOT);
}


/**
 *  Reads all sections of iode.in related to A2M.
 */
void B_A2mGetAllParms()
{
    B_A2mGetGnlParms() ;
    B_A2mGetRtfParms() ;
    B_A2mGetHtmlParms();
    B_A2mGetGIFParms() ;
    B_A2mGetCsvParms() ;
    B_A2mGetMifParms() ;
    B_A2mGetGdiParms() ;
    
}


/**
 *  Rewrites all sections of iode.in related to A2M  with the current values of A2M* variables.
 */
void B_A2mSaveAllParms()
{
    B_A2mSaveGnlParms() ;
    B_A2mSaveRtfParms() ;
    B_A2mSaveHtmlParms();
    B_A2mSaveGIFParms() ;
    B_A2mSaveCsvParms() ;
    B_A2mSaveMifParms() ;
    B_A2mSaveGdiParms() ;
}
