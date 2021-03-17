#include <iodedos.h>
#include <s_a2m.h>

double   A2M_GWIDTH = 9.5;
double   A2M_GHEIGHT = 6.5;
int      A2M_BOXWIDTH = 1;
int      A2M_BACKBRUSH = 50;
int      A2M_BACKCOLOR = 'b';

C_PrintDestDecLang(view, nbdec, lang) /* GB 08-97 */
int     view, nbdec, lang;           /* JMP 14-07-96 */
{
    int     rc = 0;
    char    tnbdec[10], tlang[2];


    sprintf(tnbdec, "%d", nbdec);
    tlang[0] = "EFD"[lang];
    tlang[1] = 0;
    if(view == 0) {
	 if(B_dest == 1)
	     if(PG_edit_window(vkp_fprintfile) < 0) return(-1);

	 rc = B_PrintDest(B_outfile);
    }

    if(rc || B_PrintNbDec(tnbdec) || B_PrintLang(tlang)) {
	B_display_last_error();
	return(-1);
	}
/*    if(reset) B_PrintDestNew(dest);   /* GB 08-97 */
    return(0);
}

void B_A2mGetGnlParms()
{
    extern int W_a2mapp; /* JMP 25-02-98 */

    A2M_ESCCH = p_a2mgnl_ESC[0];
    A2M_SEPCH = p_a2mgnl_SEP[0];
    A2M_DEFCH = p_a2mgnl_DEF[0];
    A2M_CMDCH = p_a2mgnl_CMD[0];
    A2M_LFON  = p_a2mgnl_LF;
    A2M_BLON  = p_a2mgnl_BL;
    A2M_GWIDTH  = 0.1 * p_a2mgnl_GW;  /* JMP 19-02-98 */
    A2M_GHEIGHT = 0.1 * p_a2mgnl_GH;  /* JMP 19-02-98 */
    A2M_BACKCOLOR = "byBwrgcm"[p_a2mgnl_COL];  /* JMP 23-02-98 */
    A2M_BACKBRUSH = p_a2mgnl_PCT;  /* JMP 23-02-98 */
    A2M_BOXWIDTH  = p_a2mgnl_BOX;  /* JMP 23-02-98 */
    W_a2mapp  = p_a2mgnl_AP;  /* JMP 25-02-98 */
    strcpy(A2M_CURTAG, p_a2mgnl_TAG);
    SCR_sqz(A2M_CURTAG);
}


void B_A2mGetRtfParms()
{
    B_A2mGetGnlParms();
    A2M_FONTSIZE     = p_a2mrtf_FONTSZ;
    A2M_FONTFAMILY  = "HTCBP"[p_a2mrtf_FONTFAM];
    A2M_TFONTFAMILY = "HTCBP"[p_a2mrtf_TFONTFAM];
    A2M_FONTINCR     = p_a2mrtf_INCR;
    A2M_TFONTSIZE    = p_a2mrtf_TFONTSZ;
    A2M_RTF_TCOLOR   = p_a2mrtf_TC;
    A2M_RTF_TCOL1    = p_a2mrtf_TCOL1;
    A2M_RTF_TCOLN    = p_a2mrtf_TCOLN;
    A2M_RTF_TSPLIT   = p_a2mrtf_TS;
    A2M_RTF_TWIDTH   = p_a2mrtf_TW;
    A2M_RTF_TBORDER  = p_a2mrtf_TB;
    A2M_RTF_HLINES   = p_a2mrtf_TH;
    A2M_RTF_VLINES   = p_a2mrtf_TV;
    W_rtfhelp        = p_a2mrtf_HCW;  /* JMP 26-02-98 */
    A2M_RTF_COMPRESS = p_a2mrtf_CMP;
    A2M_NUMBERS      = p_a2mrtf_NU;
    A2M_RTF_TOC      = p_a2mrtf_TOC;

    B_A2mSetRtfTitle(p_a2mrtf_TITLE);
    B_A2mSetRtfCopy(p_a2mrtf_COPYR);
}


/* now in b_pdest.c
B_A2mSetRtfTitle(title)
U_ch    *title;
{
    if(title == 0) return(0);
    SCR_free(A2M_RTF_TITLE);
    A2M_RTF_TITLE = SCR_stracpy(title);
    SCR_strip(A2M_RTF_TITLE);
    SCR_OemToAnsi(A2M_RTF_TITLE, A2M_RTF_TITLE);
    return(0);
}

B_A2mSetRtfCopy(title)
U_ch    *title;
{
    if(title == 0) return(0);
    SCR_free(A2M_RTF_COPYRIGHT);
    A2M_RTF_COPYRIGHT = SCR_stracpy(title);
    SCR_strip(A2M_RTF_COPYRIGHT);
    SCR_OemToAnsi(A2M_RTF_COPYRIGHT, A2M_RTF_COPYRIGHT);
    return(0);
}
*/

void B_A2mGetCsvParms()
{
    B_A2mGetGnlParms();
}

void B_A2mGetHtmlParms()
{
    B_A2mGetGnlParms();
    A2M_FONTSIZE     = p_a2mhtml_FONTSZ;
    A2M_FONTINCR     = p_a2mhtml_INCR;
    A2M_FONTFAMILY  = "HTCBP"[p_a2mhtml_FONTFAM];
    A2M_TFONTFAMILY  = "HTCBP"[p_a2mhtml_TFONTFAM];
    A2M_TFONTSIZE    = p_a2mhtml_TFONTSZ;
    A2M_NUMBERS      = p_a2mhtml_NU;
    A2M_HTML_TBORDER = p_a2mhtml_TB;
    A2M_HTML_TCOLOR  = p_a2mhtml_TC;
    A2M_HTML_TOC     = p_a2mhtml_TOC;
    SCR_free(A2M_HTML_BODY);
    SCR_free(A2M_HTML_TITLE);
    A2M_HTML_BODY    = SCR_stracpy(p_a2mhtml_BODY);
    A2M_HTML_TITLE   = SCR_stracpy(p_a2mhtml_TITLE);
    SCR_strip(A2M_HTML_BODY);
    SCR_strip(A2M_HTML_TITLE);
}

void B_A2mGetGdiParms()
{
    extern  int W_gdiask; /* GB 23/03/98 */
    extern  int W_gdiduplex, W_gdiorient; /* JMP 18-04-98 */

    B_A2mGetGnlParms();
    A2M_FONTSIZE    = p_a2mgdi_FONTSZ;
    A2M_FONTFAMILY  = "HTCBP"[p_a2mgdi_FONTFAM];
    A2M_TFONTFAMILY  = "HTCBP"[p_a2mgdi_TFONTFAM];
    A2M_FONTINCR    = p_a2mgdi_INCR;
    A2M_TFONTSIZE   = p_a2mgdi_TFONTSZ;
    A2M_NUMBERS     = p_a2mgdi_NU;
#ifndef UNIX
    A2M_GDI_LMARG   = p_a2mgdi_LMARG; // JMP 22/8/2016
    A2M_GDI_RMARG   = p_a2mgdi_RMARG; // JMP 22/8/2016
    A2M_GDI_TMARG   = p_a2mgdi_TMARG; // JMP 22/8/2016
    A2M_GDI_BMARG   = p_a2mgdi_BMARG; // JMP 22/8/2016
    A2M_GDI_COLOR   = !p_a2mgdi_CO;
    A2M_GDI_TBORDER = p_a2mgdi_TB;
#endif
    A2M_TBREAK      = p_a2mgdi_TBR;
    A2M_TPAGE       = p_a2mgdi_TP;
    A2M_GPAGE       = p_a2mgdi_GP;
    if(p_a2mgdi_TS) {
	A2M_TSHADING_BRUSH[0] = 7;
	A2M_TSHADING_BRUSH[1] = 5;
	}
    else {
	A2M_TSHADING_BRUSH[0] = 0;
	A2M_TSHADING_BRUSH[1] = 0;
	}
    SCR_free(A2M_PGHEAD);
    SCR_free(A2M_PGFOOT);
    A2M_PGHEAD = SCR_stracpy(p_a2mgdi_PGHEAD);
    SCR_strip(A2M_PGHEAD);
    A2M_PGFOOT = SCR_stracpy(p_a2mgdi_PGFOOT);
    SCR_strip(A2M_PGFOOT);

    W_gdiask    = p_a2mgdi_PGPROMPT;    /* GB 23/03/98 */
    //W_gdiduplex = p_a2mgdi_DUPLEX;    // JMP 22/8/2016
    //W_gdiorient = p_a2mgdi_ORIENT;    // JMP 22/8/2016
}

void B_A2mGetMifParms()
{
    B_A2mGetGnlParms();
    A2M_FONTSIZE     = p_a2mmif_FONTSZ;
    A2M_FONTINCR     = p_a2mmif_INCR;
    A2M_FONTFAMILY  = "HTCBP"[p_a2mmif_FONTFAM];
    A2M_TFONTFAMILY  = "HTCBP"[p_a2mmif_TFONTFAM];
    A2M_TFONTSIZE    = p_a2mmif_TFONTSZ;
    A2M_FRM_TCOLOR   = p_a2mmif_TC;
    A2M_FRM_TCOL1    = p_a2mmif_TCOL1;
    A2M_FRM_TCOLN    = p_a2mmif_TCOLN;
    A2M_FRM_TSPLIT   = p_a2mmif_TS;
    A2M_FRM_TWIDTH   = p_a2mmif_TW;
    A2M_FRM_TBORDER  = p_a2mmif_TB;
    A2M_FRM_HLINES   = p_a2mmif_TH;
    A2M_FRM_VLINES   = p_a2mmif_TV;
    A2M_FRM_REF      = p_a2mmif_IMG;
}



/* now in b_pdest.c */

//int B_GIFCOLS[8][3] = {
//    /* BLUE    */ {171, 172, 211},
//    /* YELLOW  */ {255, 255, 128},
//    /* BLACK   */ {0, 0, 0},
//    /* WHITE   */ {255, 255, 255},
//    /* RED     */ {218, 69, 37},
//    /* GREEN   */ {111, 197, 58},
//    /* CYAN    */ {154, 255, 228},
//    /* MAGENTA */ {157, 98, 157}
//};

//void B_A2mSetCol(dest, col)
//int     *dest, col;
//{
//    dest[0]= B_GIFCOLS[col][0];
//    dest[1]= B_GIFCOLS[col][1];
//    dest[2]= B_GIFCOLS[col][2];
//}

void B_A2mGetGIFParms()
{
    int col;

    col = p_a2mgif_COL;
    if(col > 7 || col < 0) col = 1;
    B_A2mSetCol(A2M_GIF_BGCOLOR, col);
    col = p_a2mgif_TRS;
    if(col > 7 || col < 0) col = 1;
    B_A2mSetCol(A2M_GIF_TRCOLOR, col);

    A2M_GIF_INTER = p_a2mgif_INTER;
    A2M_GIF_TRANS = p_a2mgif_TRANS;
/*    A2M_GIF_WIDTH = p_a2mgif_WIDTH;
    A2M_GIF_HEIGHT = p_a2mgif_HEIGHT;
    A2M_GIF_RESIZE = p_a2mgif_RESIZE;
    A2M_GIF_KEEP = p_a2mgif_KEEP;
*/
    A2M_GIF_FONT = p_a2mgif_GIFFONT + 1;
    A2M_GIF_FILLED = p_a2mgif_FILLED;
}


