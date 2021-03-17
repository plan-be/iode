#include "scr4w.h"

HANDLE  W_SCRL_FONT = 0, W_SCRL_FONTI = 0,
	W_SCRL_FONTB = 0, W_SCRL_FONTBI = 0,
	W_SCRL_FONT_TERM = 0;

int     WSCR_FONTHEIGHT = 12; 
char    *WSCR_FONTNAME = "Terminal";
//extern int      WSCR_FONTHEIGHT; 
//extern char     *WSCR_FONTNAME;  /* JMP 28-11-2008 */

//// Test JMP
/*HFONT FAR PASCAL MyCreateFont( void )
{
    CHOOSEFONT cf;
    LOGFONT lf;
    HFONT hfont;
     // Initialize members of the CHOOSEFONT structure.
     cf.lStructSize = sizeof(CHOOSEFONT);
    cf.hwndOwner = (HWND)NULL;
    cf.hDC = (HDC)NULL;
    cf.lpLogFont = &lf;
    cf.iPointSize = 0;
    cf.Flags = CF_SCREENFONTS;
    cf.rgbColors = RGB(0,0,0);
    cf.lCustData = 0L;
    cf.lpfnHook = (LPCFHOOKPROC)NULL;
    cf.lpTemplateName = (LPSTR)NULL;
    cf.hInstance = (HINSTANCE) NULL;
    cf.lpszStyle = (LPSTR)NULL;
    cf.nFontType = SCREEN_FONTTYPE;
    cf.nSizeMin = 0;
    cf.nSizeMax = 0;
     // Display the CHOOSEFONT common-dialog box.
     ChooseFont(&cf);
     // Create a logical font based on the user's
    // selection and return a handle identifying
    // that font.
     hfont = CreateFontIndirect(cf.lpLogFont);
    return (hfont);
}
*/

WscrCreateFonts()
{
    LOGFONT     LogFont;

    memset(&LogFont, 0, sizeof(LOGFONT));

    LogFont.lfHeight = WSCR_FONTHEIGHT;
    lstrcpy(LogFont.lfFaceName, "MS Sans Serif");
//    lstrcpy(LogFont.lfFaceName, WSCR_FONTNAME);
//    lstrcpy(LogFont.lfFaceName, "");
    W_SCRL_FONT = CreateFontIndirect(&LogFont);
    LogFont.lfItalic = 1;
    W_SCRL_FONTI = CreateFontIndirect(&LogFont);
    LogFont.lfWeight = FW_BOLD;
    LogFont.lfItalic = 0;
    W_SCRL_FONTB = CreateFontIndirect(&LogFont);
    LogFont.lfItalic = 1;
    W_SCRL_FONTBI = CreateFontIndirect(&LogFont);

    memset(&LogFont, 0, sizeof(LOGFONT));
    LogFont.lfHeight = 10;
    LogFont.lfWidth  = 0;
    LogFont.lfQuality = PROOF_QUALITY;
    LogFont.lfPitchAndFamily = FF_MODERN;
    lstrcpy(LogFont.lfFaceName, "Courier");
    W_SCRL_FONT_TERM = CreateFontIndirect(&LogFont);
    return(0);
}

COLORREF WSCR_COLORS[][2] = {
		    /* BACKGROUND       FOREGROUND */
/*  Default   */    WCOL_WHITE,        WCOL_BLACK,
/*  Reverse   */    WCOL_BLUE,         WCOL_BLACK,  /* JMP 23-05-02 */
/*  Underline */    WCOL_YELLOW,       WCOL_BLACK,
/*  Bold      */    WCOL_YELLOW,       WCOL_BLUE,
/*  Blinking  */    WCOL_BLUE,         WCOL_YELLOW,
/*  Dim       */    WCOL_WHITE,        WCOL_YELLOW,
/*  Black     */    WCOL_BLACK,        WCOL_WHITE,
/*  Blue      */    WCOL_BLUE,         WCOL_WHITE,
/*  Green     */    WCOL_GREEN,        WCOL_BLACK,
/*  Cyan      */    WCOL_CYAN,         WCOL_BLACK,
/*  Red       */    WCOL_RED,          WCOL_BLACK,
/*  Magenta   */    WCOL_MAGENTA,      WCOL_WHITE, // JMP 7/5/2013
/*  White     */    WCOL_WHITE,        WCOL_BLACK,
/*  Yellow    */    WCOL_YELLOW,       WCOL_BLACK,
/*  Shadow    */    WCOL_BLACK,        WCOL_BLACK,
/*  Chelp1    */    WCOL_RED,          WCOL_BLACK,
/*  Chelp2    */    WCOL_BLUE,         WCOL_BLACK
};

HBRUSH
 W_BRUSH_BLACK   = 0,
 W_BRUSH_BLUE    = 0,
 W_BRUSH_GREEN   = 0,
 W_BRUSH_RED     = 0,
 W_BRUSH_CYAN    = 0,
 W_BRUSH_MAGENTA = 0,
 W_BRUSH_YELLOW  = 0,
 W_BRUSH_WHITE   = 0,
 W_BRUSH_GRAY    = 0,
 W_BRUSH_LTGRAY  = 0,
 W_BRUSH_DKGRAY  = 0,
 W_BRUSH_LTBLUE  = 0,
 W_BRUSH_DKBLUE  = 0,
 WSCR_BRUSHES[17];          /* JMP 10-02-97 */

HPEN WSCR_PENS[2];

WscrCreateBrushes()
{
/*    static LOGBRUSH logbr = {BS_SOLID, RGB(0, 255, 255), 0}; */

    WSCR_BRUSHES[0]  = W_BRUSH_WHITE   = CreateSolidBrush(WCOL_WHITE  );
    WSCR_BRUSHES[1]  = W_BRUSH_DKGRAY  = CreateSolidBrush(WCOL_DKGRAY ); /* JMP 10-02-97 */
    WSCR_BRUSHES[2]  = W_BRUSH_YELLOW  = CreateSolidBrush(WCOL_YELLOW );
    WSCR_BRUSHES[3]  = W_BRUSH_CYAN    = CreateSolidBrush(WCOL_CYAN   );
    WSCR_BRUSHES[4]  = W_BRUSH_BLUE    = CreateSolidBrush(WCOL_BLUE   );
    WSCR_BRUSHES[5]  = W_BRUSH_WHITE;
    WSCR_BRUSHES[6]  = W_BRUSH_BLACK   = CreateSolidBrush(WCOL_BLACK  );
    WSCR_BRUSHES[7]  = W_BRUSH_BLUE  ;
    WSCR_BRUSHES[8]  = W_BRUSH_GREEN   = CreateSolidBrush(WCOL_GREEN  );
    WSCR_BRUSHES[9]  = W_BRUSH_CYAN  ;
    WSCR_BRUSHES[10]  = W_BRUSH_RED     = CreateSolidBrush(WCOL_RED    );
    WSCR_BRUSHES[11]  = W_BRUSH_MAGENTA = CreateSolidBrush(WCOL_MAGENTA);
    WSCR_BRUSHES[12]  = W_BRUSH_WHITE ;
    WSCR_BRUSHES[13]  = W_BRUSH_YELLOW;
    WSCR_BRUSHES[14]  = W_BRUSH_BLACK ;
    WSCR_BRUSHES[15]  = W_BRUSH_RED   ;
    WSCR_BRUSHES[16]  = W_BRUSH_BLUE  ;

    WSCR_BRUSHES[1]  = W_BRUSH_CYAN; /* JMP 23-05-02 */
    //WSCR_BRUSHES[1]  = W_BRUSH_WHITE; 

    W_BRUSH_GRAY   = CreateSolidBrush(WCOL_GRAY);
    W_BRUSH_LTGRAY = CreateSolidBrush(WCOL_LTGRAY);
    W_BRUSH_DKGRAY = CreateSolidBrush(WCOL_DKGRAY);
    W_BRUSH_LTBLUE = CreateSolidBrush(WCOL_LTBLUE);
    W_BRUSH_DKBLUE = CreateSolidBrush(WCOL_DKBLUE);

    return(0);
}


WscrCreatePens()
{
    WSCR_PENS[0] = CreatePen(PS_SOLID, 1, 0L);
    WSCR_PENS[1] = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    return(0);
}

WscrCreateGDIObjects()
{
    WscrCreateFonts();
    WscrCreateBrushes();
    WscrCreatePens();
    return(0);
}

WscrDeleteGDIObjects()
{
    if(W_SCRL_FONT    ) DeleteObject(W_SCRL_FONT    );
    if(W_SCRL_FONTI   ) DeleteObject(W_SCRL_FONTI   );
    if(W_SCRL_FONTB   ) DeleteObject(W_SCRL_FONTB   );
    if(W_SCRL_FONTBI  ) DeleteObject(W_SCRL_FONTBI  );
    if(W_SCRL_FONT_TERM) DeleteObject(W_SCRL_FONT_TERM);
    if(W_BRUSH_BLACK  ) DeleteObject(W_BRUSH_BLACK  );
    if(W_BRUSH_BLUE   ) DeleteObject(W_BRUSH_BLUE   );
    if(W_BRUSH_GREEN  ) DeleteObject(W_BRUSH_GREEN  );
    if(W_BRUSH_RED    ) DeleteObject(W_BRUSH_RED    );
    if(W_BRUSH_CYAN   ) DeleteObject(W_BRUSH_CYAN   );
    if(W_BRUSH_MAGENTA) DeleteObject(W_BRUSH_MAGENTA);
    if(W_BRUSH_YELLOW ) DeleteObject(W_BRUSH_YELLOW );
    if(W_BRUSH_WHITE  ) DeleteObject(W_BRUSH_WHITE  );
    if(W_BRUSH_GRAY   ) DeleteObject(W_BRUSH_GRAY   );
    if(W_BRUSH_LTGRAY ) DeleteObject(W_BRUSH_LTGRAY );
    if(W_BRUSH_DKGRAY ) DeleteObject(W_BRUSH_DKGRAY );
    if(W_BRUSH_LTBLUE ) DeleteObject(W_BRUSH_LTBLUE );
    if(W_BRUSH_DKBLUE ) DeleteObject(W_BRUSH_DKBLUE );
    if(WSCR_PENS[0]) DeleteObject(WSCR_PENS[0]);
    if(WSCR_PENS[1]) DeleteObject(WSCR_PENS[1]);
    return(0);
}


