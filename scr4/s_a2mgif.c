#include "scr4.h"

#include "s_a2m.h"
#include "s_gd.h"
#include "s_gdftg.h"
#include "s_gdftl.h"
#include "s_gdftmb.h"
#include "s_gdfts.h"
#include "s_gdftt.h"
#include "s_gdftgr.h"


/* global */

int A2M_GIF_BGCOLOR[3] = {255,255,255};
int A2M_GIF_TRCOLOR[3] = {0,0,0};
int A2M_GIF_INTER = 1;
int A2M_GIF_TRANS = 0;
int A2M_GIF_WIDTH = 0;
int A2M_GIF_HEIGHT = 0;
int A2M_GIF_KEEP = 0;
int A2M_GIF_RESIZE = 0;
int A2M_GIF_FONT = 2;
int A2M_GIF_FILLED = 0;
char *A2M_GIF_FRAC = NULL;
int A2M_GIF_COUNT = 0;

/* GIF unit conversion */

// GIF: unit = dot, 39 dots / cm = 99 dots /inch
// A2M: unit = point, 72 pts /inch, ou 72 pts == 99 dots
// Conversion: pts * 1.37583 = dot
//             cm * 39 = dot
// #define PTStoDOTS 1.37583
// #define CMtoDOTS  39
// GIF: unit = dot, 36 dots / cm = 91 dots /inch
// A2M: unit = point, 72 pts /inch, ou 72 pts == 91 dots
// Conversion: pts * 1.26389 = dot
//             cm * 36 = dot
#define PTStoDOTS 1.26389
#define CMtoDOTS  36

/* local */
    /* output */
    FILE *GIF_out = NULL;
    /* image */
    gdImagePtr GIF_im = NULL;
    gdImagePtr GIF_tmp = NULL;
    /* couleurs */
    int GIF_fond;
    int GIF_trans;
    int GIF_blanc;
    int GIF_bleu;
    int GIF_rouge;
    int GIF_vert;
    int GIF_noir;
    int GIF_gris;
    int GIF_griscl;
    double GIF_EX = 1.0, GIF_EY = 1.0;

/*NH*/
A2mGIFPrep(filename)
U_ch    *filename;
{
    char dir[128];
    char file[128];
    int pos;

    A2M_GIF_COUNT = 0;
    SCR_sqz(filename);
    SCR_split_name(filename, dir, file);
    pos = U_pos('.', file);
    SCR_free(A2M_GIF_FRAC);
    A2M_GIF_FRAC = 0;
    if(pos > 0) {
	file[pos] = 0;
	A2M_GIF_FRAC = SCR_stracpy(file);
	}
    else
	A2M_GIF_FRAC = SCR_stracpy("img");

    if(A2M_GIF_FRAC) return(0);
    else return(-1);
}

/*NH*/
A2mGIFInitImg(go)
A2MGRF *go;
{
    double ratio, width, height;
    int i;

    /* sizing , scaling */
    if(A2M_GIF_RESIZE == 1 && (A2M_GIF_WIDTH != 0 && A2M_GIF_HEIGHT != 0))
    {
	if(A2M_GIF_KEEP == 1)
	{
	    ratio = (double) go->ag_size[1] / (double) go->ag_size[0];
	    width = (double) A2M_GIF_WIDTH;
	    height = width * ratio;
	}
	else
	{
	    width = (double) A2M_GIF_WIDTH;
	    height = (double) A2M_GIF_HEIGHT;
	    ratio = 1.0;
	}
	GIF_EX = (double) A2M_GIF_WIDTH / (double)(go->ag_size[0] * CMtoDOTS);
	// GIF_EY = (1.0 / ratio) * (double) A2M_GIF_HEIGHT / (double)(go->ag_size[1] * CMtoDOTS);
	GIF_EY =  (double) height / (double)(go->ag_size[1] * CMtoDOTS);
    }
    else
    {
	width = (double)(go->ag_size[0] * CMtoDOTS);
	height = (double)(go->ag_size[1] * CMtoDOTS);
	GIF_EX = 1.0;
	GIF_EY = 1.0;
    }
    GIF_im = gdImageCreate((int)width, (int)height);
    if(!GIF_im) return(-1);

    /* couleurs */
    GIF_fond = gdImageColorAllocate(GIF_im,A2M_GIF_BGCOLOR[0],A2M_GIF_BGCOLOR[1],A2M_GIF_BGCOLOR[2]);
    GIF_blanc = gdImageColorAllocate(GIF_im, 255, 255, 255);
    GIF_noir = gdImageColorAllocate(GIF_im, 0, 0, 0);
    GIF_bleu = gdImageColorAllocate(GIF_im, 0, 0, 255);
    GIF_rouge = gdImageColorAllocate(GIF_im, 255, 0, 0);
    GIF_vert = gdImageColorAllocate(GIF_im, 0, 255, 0);
    GIF_gris = gdImageColorAllocate(GIF_im, 192, 192, 192);
    GIF_griscl = gdImageColorAllocate(GIF_im, 128, 128, 128);
    GIF_trans = gdImageColorAllocate(GIF_im,A2M_GIF_TRCOLOR[0],A2M_GIF_TRCOLOR[1],A2M_GIF_TRCOLOR[2]);
    if(A2M_GIF_TRANS)
	gdImageColorTransparent(GIF_im, GIF_trans);

    return(0);
}

/*NH*/
A2mGIFInit(go)
A2MGRF *go;
{
    char filename[256];
    double ratio, width, height;
    int i;

    if(A2M_GIF_COUNT == 0)
	sprintf(filename, "%s%s", A2M_GIF_FRAC, ".gif");
    else
	sprintf(filename, "%s%d%s", A2M_GIF_FRAC,A2M_GIF_COUNT, ".gif");
    GIF_out = fopen(filename, "wb");
    if(GIF_out == NULL) return(-2);
    return(A2mGIFInitImg(go));
}

/*NH*/
A2mGIFEnd()
{
    /* Interlacé */
    if(A2M_GIF_INTER)
	gdImageInterlace(GIF_im, 1);
    /* Write GIF */
    gdImageGif(GIF_im, GIF_out);
    gdImageDestroy(GIF_im);

    if(GIF_out) fclose(GIF_out);
    GIF_out = NULL;
    GIF_im = NULL;
    return(0);
}


A2mToGIF(a2mfile, outfile)
U_ch    *a2mfile, *outfile;
{
    A2MFILE *af;
    A2MOBJ  *ao;
    int rc = 0;

    af = A2mOpen(a2mfile, 0);
    if(af == 0) return(-1);

    if(A2mGIFPrep(outfile) != 0)
    {
	A2mClose(af);
	return(-2);
    }

    while(1)
    {
	ao = A2mRead(af);
	if(ao == 0) break;
	if(ao->ao_type == A2M_GRF)
	{
	    if(A2mGIFInit((A2MGRF *)ao->ao_ptr) != 0)
	    {
		rc = -2;
		break;
	    }
	    A2mGIFPrintGrf((A2MGRF *)ao->ao_ptr);
	    A2mGIFEnd();
	    A2M_GIF_COUNT++;
	}
    }
    A2mClose(af);
    return(rc);
}

/* reçoit un objet graphique et un nom de fichier output */
int A2mGIF_HTML(A2MGRF *go, U_ch* filename)
{
    int rc=0;
    GIF_out = fopen(filename, "wb");
    if(GIF_out == NULL) return(-2);
    if(A2mGIFInitImg(go) != 0) return(-1);
    A2mGIFPrintGrf(go);
    A2mGIFEnd();
    return(0);
}


////////////////////////////////////////////////////////////////////////////////
/***************** GRAPHICS ***********************/

extern int (*A2M_GIF_GRF_FNS[])();



/*NH*/
A2mGIFPrintGrf(agrf)
A2MGRF  *agrf;
{
    A2M_GRF_FNS = A2M_GIF_GRF_FNS;
    A2mGrfPrint(agrf);
    return(0);
}

A2mGIFGrfInit(ag)
A2MGRF  *ag;
{
}
A2mGIFGrfEnd(ag)
A2MGRF  *ag;
{
}
A2mGIFGrfPen(pen)
A2MPEN  *pen;
{
}
A2mGIFGrfBrush(brush)
A2MBRUSH  *brush;
{
}

double A2mGIFSx(x)
double x;
{
    return(PTStoDOTS * x * GIF_EX);
}
double A2mGIFSy(y)
double y;
{
    return(PTStoDOTS * y * GIF_EY);
}

A2mGIFSetProps(type, color)
int type, color;
{
    int styleDotted[2];
    int styleDashed[6];

    if(type == 0) // style dotted
    {
	styleDotted[0] = color;
	styleDotted[1] = GIF_trans;
	gdImageSetStyle(GIF_im, styleDotted, 2);
    }
    else // style dashed
    {
	styleDashed[0] = color;
	styleDashed[1] = color;
	styleDashed[2] = color;
	styleDashed[3] = GIF_trans;
	styleDashed[4] = GIF_trans;
	styleDashed[5] = GIF_trans;
	gdImageSetStyle(GIF_im, styleDashed, 6);
    }
    return(gdStyled);
}

A2mGIFProps(props)
int props;
{
    /* noir bleu rouge vert gris griscl */
    /* 0    1    2     3    4    5      */
    /* ...  ...  ...   ...  ...  ...    */
    /*      6    7     8    9    10     */
    /* ---  ---  ---   ---  ---  ---    */
    /*      11   12    13   14   15     */
    int style;

    if(props < 0) props = 0;   /* JMP 26-04-02 */
    style = GIF_noir + props;
    if(style > GIF_griscl)
    {
	switch(style)
	{
	    case 6:
		return A2mGIFSetProps(0, GIF_bleu);
	    case 7:
		return A2mGIFSetProps(1, GIF_rouge);
	    case 8:
		return A2mGIFSetProps(0, GIF_vert);
	    case 9:
		return A2mGIFSetProps(1, GIF_gris);
	    case 10:
		return A2mGIFSetProps(0, GIF_griscl);
	    case 11:
		return A2mGIFSetProps(1, GIF_bleu);
	    case 12:
		return A2mGIFSetProps(0, GIF_rouge);
	    case 13:
		return A2mGIFSetProps(1, GIF_vert);
	    case 14:
		return A2mGIFSetProps(0, GIF_gris);
	    case 15:
		return A2mGIFSetProps(1, GIF_griscl);
	    default:
		return A2mGIFSetProps(1, GIF_noir);
	}
    }
    else return(style);
}

A2mGIFGrfLine(axis, x1, y1, x2, y2, props)  // unités en points
double  x1, y1, x2, y2;
int     axis, props;
{
    int     gx1, gy1, gx2, gy2;

    switch(axis) {
	case -1 : break;
	case  0 : x1 += GR_X_0; y1 += GR_Y_0;
		  x2 += GR_X_0; y2 += GR_Y_0;
		  break;
	default : x1 += GR_X_0; y1 += GR_Z_0;
		  x2 += GR_X_0; y2 += GR_Z_0;
		  break;
    }

    gx1 = A2mGIFSx(x1);
    gy1 = A2mGIFSy(GR_HEIGHT - y1);
    gx2 = A2mGIFSx(x2);
    gy2 = A2mGIFSy(GR_HEIGHT - y2);
    gdImageLine(GIF_im, gx1, gy1, gx2, gy2, A2mGIFProps(props));
    return(0);
}
A2mGIFGrfText(axis, x, y, string, align, color)
int     axis;
double  x, y;
char    *string, *align, *color;
{
}
A2mGIFGrfLabel(nbr, type, title, props)
int     nbr, type, props;
A2MPAR  *title;
{
}

gdFont *A2mGIFGetFont(index)
int index;
{
    gdFont *font;
    switch(index)
    {
	case 0: font = NULL; break;
	case 1: font = gdFontTiny; break;
	case 2: font = gdFontSmall; break;
	case 3: font = gdFontMediumBold; break;
	case 4: font = gdFontLarge; break;
	case 5: font = gdFontGiant; break;
	case 6: font = gdFontGray; break;
    }
    return font;
}

A2mGIFGrfPar(axis, x, y, par, align, color)
double  x, y;
char    *align, *color;
A2MPAR  *par;
{
    int         i;
    gdFont      *font;
    int         fw, fh;
    char        *ptr;
    double      gx, gy;

    font = A2mGIFGetFont(A2M_GIF_FONT);
    if(font == NULL) return(0);
    fw = font->w;
    fh = font->h;

    switch(axis) {
	case -1 : break;
	case  0 : x += GR_X_0; y += GR_Y_0; break;
	default : x += GR_X_0; y += GR_Z_0; break;
	}

    y += A2M_FONTSIZE / 3;
    gx = A2mGIFSx(x);
    gy = A2mGIFSy(GR_HEIGHT - (y));

    ptr = (par->ap_strs[0]->as_txt);
    switch(align[0])
    {
	case 'L' : break;
	case 'C' : gx -= ((double) strlen(ptr) * (double) fw) / 2; break;
	case 'R' : gx -= ((double) strlen(ptr) * (double) fw); break;
    }
    gy -= (double) fh / 2;
    gdImageString(GIF_im, font, (int) gx, (int) gy, ptr, GIF_noir);
    return(0);
}
A2mGIFGrfGroupObj()
{
}
A2mGIFGrfGroup()
{
}
A2mGIFGrfBox(axis, x, y, w, h, props)
double  x, y, w, h;
int     axis, props;
{
    double  y_0 = 0;
    int     gx1, gy1, gx2, gy2;
    int x1, y1, x2, y2;

    switch(axis) {
	case -1 : break;
	case  0 : x += GR_X_0; y_0 = GR_Y_0; break;
	default : x += GR_X_0; y_0 = GR_Z_0; break;
    }

    gx1 = A2mGIFSx(x );
    gy1 = A2mGIFSy(GR_HEIGHT - (y_0 + y));
    gx2 = A2mGIFSx(x + w);
    gy2 = A2mGIFSy(GR_HEIGHT - (y_0 + y) - h);
    if(gx1 < gx2)
    {
	x1 = gx1;
	x2 = gx2;
    }
    else
    {
	x1 = gx2;
	x2 = gx1;
    }
    if(gy1 < gy2)
    {
	y1 = gy1;
	y2 = gy2;
    }
    else
    {
	y1 = gy2;
	y2 = gy1;
    }
    if (props == 0)
	gdImageFilledRectangle(GIF_im, x1, y1, x2, y2, GIF_fond);
    else if(A2M_GIF_FILLED == 0)
	gdImageFilledRectangle(GIF_im, x1, y1, x2, y2, A2mGIFProps(props));
    else
	gdImageRectangle(GIF_im, x1, y1, x2, y2, A2mGIFProps(props));
    return(0);
}
A2mGIFGrfPolyLine(axis, nobs, vals, props)
int     axis, nobs, props;
double  *vals;
{
    int i;
    for(i = 0; i < nobs - 1; i++)
    {
	A2mGIFGrfLine(axis, vals[i*2],vals[i*2+1], vals[i*2+2], vals[i*2+3], props);
    }
}
A2mGIFGrfPolyBar(axis, nobs, vals, width, props)
int     axis, nobs, props;
double  *vals, width;
{
}

A2mGIFGrfPrepare()
{
}

A2mGIFGrfGetPen(props, pen)
int     props;
A2MPEN  *pen;
{
}
A2mGIFGrfGetBrush(props, brush)
int         props;
A2MBRUSH    *brush;
{
}


/******* FUNCTION TABLE FOR GIF ***********/

int (*A2M_GIF_GRF_FNS[])() = {
    A2mGIFGrfInit,
    A2mGIFGrfEnd,
    A2mGIFGrfPen,
    A2mGIFGrfBrush,
    A2mGIFGrfLine,
    A2mGIFGrfText,
    A2mGIFGrfLabel,
    A2mGIFGrfPar,
    A2mGIFGrfGroupObj,
    A2mGIFGrfGroup,
    A2mGIFGrfBox,
    A2mGIFGrfPolyLine,
    A2mGIFGrfPolyBar,
    A2mGIFGrfPrepare,
    A2mGIFGrfGetPen,
    A2mGIFGrfGetBrush
};

