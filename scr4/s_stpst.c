#include <stdio.h>
#include "s_strs.h"

#define LINE_H  (PS_FONTSIZE + PS_SPACING)

FILE    *PS_IN_FD, *PS_OUT_FD;
int     PS_LINE_NB, PS_COL_NB, PS_PAGE_NB, PS_IN_FD_STD, PS_OUT_FD_STD,
	PS_PAGE_H, PS_PAGE_W, PS_STATUS,
	PS_G_SIZE, PS_G_POS, PS_G_LG, PS_LANDSC;
float   PS_MARGV, PS_MARGH, PS_SPACING;
float   PS_FONTSIZE;
int     *PS_VG_LG, *PS_VG_POS, *PS_VG_SIZE, PS_VNB;

/*NH*/
PS_init(ifl, ofl, font, size, margv, margh, spacing, landsc)
char    *ifl, *ofl, *font;
double  size, margv, margh, spacing;
int     landsc;
{
    char    fi[81], fo[81];

    if(landsc) {
	PS_PAGE_H   = 594;
	PS_PAGE_W   = 840;
	PS_MARGV    = 20.0;
	PS_MARGH    = 30.0;
	PS_FONTSIZE = 7.0;
	PS_SPACING  = 1.0;
	}
    else {
	PS_PAGE_H   = 840;
	PS_PAGE_W   = 594;
	PS_MARGV    = 30.0;
	PS_MARGH    = 20.0;
	PS_FONTSIZE = 10.0;
	PS_SPACING  = 1.0;
	}

    PS_LANDSC = landsc;
    if(margv > 0)    PS_MARGV    = (float)margv;
    if(margh > 0)    PS_MARGH    = (float)margh;
    if(size > 0)     PS_FONTSIZE = (float)size;
    if(spacing >= 0) PS_SPACING  = (float)spacing;

    fi[0] = fo[0] = 0;
    if(ifl) {
	strcpy(fi, ifl);
	SCR_strip(fi);
	}
    if(ofl) {
	strcpy(fo, ofl);
	SCR_strip(fo);
	}

    if(fi[0] == 0 || fi[0] == '-') {
	PS_IN_FD = stdin;
	PS_IN_FD_STD = 1;
	}
    else {
	PS_IN_FD_STD = 0;
	PS_IN_FD = fopen(fi, "r");
	if(PS_IN_FD == 0) return(-1);
	}

    if(fo[0] == 0 || fo[0] == '-') {
	PS_OUT_FD = stdout;
	PS_OUT_FD_STD = 1;
	}
    else {
	PS_OUT_FD_STD = 0;
#ifdef UNIX
	if(strncmp(fo, "lp -s -d", 8) == 0)
	    PS_OUT_FD = popen(fo, "w");   /* JMP 19-02-96 */
	else
#endif
	PS_OUT_FD = fopen(fo, "w+");
	if(PS_OUT_FD == 0) {
	    PS_close();
	    return(-1);
	    }
	}
    PS_profile(font, PS_FONTSIZE);
    PS_LINE_NB = PS_COL_NB = PS_PAGE_NB = PS_STATUS = 0;
    return(0);
}

/*NH*/
int PS_close()
{
    if(PS_IN_FD_STD  == 0 && PS_IN_FD)  fclose(PS_IN_FD);
    if(PS_OUT_FD_STD == 0 && PS_OUT_FD) fclose(PS_OUT_FD);
    PS_IN_FD = PS_OUT_FD = 0;
    return(0);
}

/* ======================================================================
Transforme un fichier ASCII PC-8 en Postscript.

&EN ifl : input filename. Si nul, stdin.
&EN ofl : output filename. Si nul, stdout.
&EN font : police de caractère. Si nul, "Helvetica"
&EN size : taille en points. Si nul, vaut 10.0.
&EN margv : marge verticale en points. Si nul, vaut 30.0
&EN margh : marge horizontale en points. Si nul, vaut 20.0
&EN spacing : interligne en points. Si nul, vaut 2.0

L'interligne est calculé comme size + spacing.

&EX
    PS_text("tmp", "lpt1", "Courier", 12.0, 30.0, 20.0, 1.0, 0);
&TX
&RT -1 en cas d'erreur (fichier non trouvé), 0 sinon.
&SA PS_labels()
------------------------------------------------------------------------- */

PS_text(ifl, ofl, font, size, margv, margh, spacing, landsc)
char    *ifl, *ofl, *font;
double  size, margv, margh, spacing;
int     landsc;
{
    int     ch;
/*    void    *malloc(); /* JMP 04-01-98 */

    if(PS_init(ifl, ofl, font, size, margv, margh, spacing, landsc)) return(-1);
    PS_VNB = 1 + (int) ((PS_PAGE_W - PS_MARGH * 2.0) / (PS_FONTSIZE * 0.6));
    PS_VG_LG   = (int *) malloc(PS_VNB * sizeof(int));
    PS_VG_POS  = (int *) malloc(PS_VNB * sizeof(int));
    PS_VG_SIZE = (int *) malloc(PS_VNB * sizeof(int));
    if(PS_VG_LG == 0 || PS_VG_POS == 0 || PS_VG_SIZE == 0) PS_VNB = -1;
    if(PS_VNB > 0) {
	memset(PS_VG_LG  , 0, PS_VNB * sizeof(int));
	memset(PS_VG_POS , 0, PS_VNB * sizeof(int));
	memset(PS_VG_SIZE, 0, PS_VNB * sizeof(int));
	}

    while((ch = getc(PS_IN_FD)) != EOF) {
	switch(ch) {
	    case '\n' :
		PS_new_line();
		break;
	    case '\f' :
		PS_new_page();
		PS_STATUS = 2;
		break;
	    case '\t' :
		PS_show();
		PS_COL_NB += 8 - PS_COL_NB % 8;
		break;
	    default :
		if(ch > 178 && ch < 219) {
		    PS_show();
		    PS_graph(ch);
		    PS_COL_NB++;
		    break;
		    }
		if(ch == ' ') {
		    if(PS_STATUS != 1) {
			PS_COL_NB++;
			break;
			}
		    ch = getc(PS_IN_FD);
		    if(ch == ' ') {
			PS_show();
			PS_COL_NB += 2;
			break;
			}
		    ungetc(ch, PS_IN_FD);
		    ch = ' ';
		    }
		PS_new_string();
		PS_mif_char(ch);
		PS_COL_NB++;
		break;
	    }
	}
    if(PS_STATUS != 2) PS_new_page();
    PS_close();
    if(PS_VG_LG  ) free(PS_VG_LG  );
    if(PS_VG_POS ) free(PS_VG_POS );
    if(PS_VG_SIZE) free(PS_VG_SIZE);
    return(0);
}

/*NH*/
int PS_new_string()
{
    if(PS_STATUS != 1) {
	PS_moveto();
	fprintf(PS_OUT_FD, "(");
	}
    PS_STATUS = 1;
    return(0);
}

/*NH*/
int PS_show()
{
    if(PS_STATUS == 1) fprintf(PS_OUT_FD, ")show\n");
    PS_STATUS = 0;
    return(0);
}

/*NH*/
float PS_y()
{
    return(PS_PAGE_H - PS_MARGV - PS_LINE_NB * LINE_H);
}

/*NH*/
float PS_x()
{
    return((float)(PS_MARGH + (PS_COL_NB * PS_FONTSIZE * 6.0) / 10.0));
}


/*NH*/
int PS_moveto()
{
    fprintf(PS_OUT_FD, "%g %g moveto ", PS_x(), PS_y());
    return(0);
}

/*NH*/
int PS_new_page()
{
    int     i;

    PS_show();
    PS_hdraw();
    for(i = 0 ; i < PS_VNB ; i++) PS_vdraw(i);
    fprintf(PS_OUT_FD, "showpage\n");
    if(PS_LANDSC) fprintf(PS_OUT_FD, "landscape\n");
    PS_LINE_NB = PS_COL_NB = 0;
    PS_PAGE_NB++;
    return(0);
}

/*NH*/
int PS_new_line()
{
    PS_show();
    PS_hdraw();
    PS_LINE_NB++;
    PS_COL_NB = 0;
    if(PS_y() < PS_MARGV) PS_new_page();
    return(0);
}

/*NH*/
int PS_mif_char(c)
int     c;
{
   if(c >= ' ' && c < 127 && c != ')' && c != '(' && c != '\\')
	fprintf(PS_OUT_FD, "%c", c);
   else
	fprintf(PS_OUT_FD, "\\%03o", c & 0377);
    return(0);
}

/*NH*/
int PS_profile(font, size)
char    *font;
double  size;
{
     fprintf(PS_OUT_FD, "%%!PS-Adobe (scr4_ps)\n\n");

     fprintf(PS_OUT_FD, "/reencsmalldict 12 dict def\n\n");

     fprintf(PS_OUT_FD, "/ReEncodeSmall\n");
     fprintf(PS_OUT_FD, "  { reencsmalldict begin\n");
     fprintf(PS_OUT_FD, "    /newcodesandnames exch def\n");
     fprintf(PS_OUT_FD, "    /newfontname exch def\n");
     fprintf(PS_OUT_FD, "    /basefontname exch def\n\n");

     fprintf(PS_OUT_FD, "    /basefontdict basefontname findfont def\n\n");

     fprintf(PS_OUT_FD, "    /newfont basefontdict maxlength dict def\n\n");

     fprintf(PS_OUT_FD, "    basefontdict\n");
     fprintf(PS_OUT_FD, "      { exch dup /FID ne\n");
     fprintf(PS_OUT_FD, "          { dup /Encoding eq\n");
     fprintf(PS_OUT_FD, "            { exch dup length array copy\n\n");

     fprintf(PS_OUT_FD, "                newfont 3 1 roll put }\n");
     fprintf(PS_OUT_FD, "            { exch newfont 3 1 roll put }\n");
     fprintf(PS_OUT_FD, "            ifelse }\n");
     fprintf(PS_OUT_FD, "          { pop pop }\n");
     fprintf(PS_OUT_FD, "          ifelse\n");
     fprintf(PS_OUT_FD, "       } forall\n\n");

     fprintf(PS_OUT_FD, "    newfont /FontName newfontname put\n");
     fprintf(PS_OUT_FD, "    newcodesandnames aload pop\n\n");

     fprintf(PS_OUT_FD, "    newcodesandnames length 2 idiv\n");
     fprintf(PS_OUT_FD, "      { newfont /Encoding get 3 1 roll put}\n");
     fprintf(PS_OUT_FD, "      repeat\n\n");

     fprintf(PS_OUT_FD, "    newfontname newfont definefont pop\n");
     fprintf(PS_OUT_FD, "    end\n");
     fprintf(PS_OUT_FD, "   } def\n\n");

     fprintf(PS_OUT_FD, "/frenchvec [\n");
     fprintf(PS_OUT_FD, "  8#201 /udieresis\n");
     fprintf(PS_OUT_FD, "  8#202 /eacute\n");
     fprintf(PS_OUT_FD, "  8#203 /acircumflex\n");
     fprintf(PS_OUT_FD, "  8#204 /adieresis\n");
     fprintf(PS_OUT_FD, "  8#205 /agrave\n");
     fprintf(PS_OUT_FD, "  8#207 /ccedilla\n");
     fprintf(PS_OUT_FD, "  8#210 /ecircumflex\n");
     fprintf(PS_OUT_FD, "  8#211 /edieresis\n");
     fprintf(PS_OUT_FD, "  8#212 /egrave\n");
     fprintf(PS_OUT_FD, "  8#213 /idieresis\n");
     fprintf(PS_OUT_FD, "  8#214 /icircumflex\n");
     fprintf(PS_OUT_FD, "  8#215 /igrave\n");
     fprintf(PS_OUT_FD, "  8#223 /ocircumflex\n");
     fprintf(PS_OUT_FD, "  8#224 /odieresis\n");
     fprintf(PS_OUT_FD, "  8#225 /ograve\n");
     fprintf(PS_OUT_FD, "  8#226 /ucircumflex\n");
     fprintf(PS_OUT_FD, "  8#227 /ugrave\n");
     fprintf(PS_OUT_FD, "  8#240 /aacute\n");
     fprintf(PS_OUT_FD, "  8#241 /iacute\n");
     fprintf(PS_OUT_FD, "  8#242 /oacute\n");
     fprintf(PS_OUT_FD, "  8#243 /uacute\n");
     fprintf(PS_OUT_FD, "  8#256 /guillemotleft\n");
     fprintf(PS_OUT_FD, "  8#257 /guillemotright\n");
     fprintf(PS_OUT_FD, "  8#360 /daggerdbl\n");
     fprintf(PS_OUT_FD, "  8#370 /ring\n");
     fprintf(PS_OUT_FD, "  8#376 /bullet\n");
     fprintf(PS_OUT_FD, "  ] def\n\n");

     fprintf(PS_OUT_FD, "/landscape {0 840 translate -90 rotate} def\n");
     fprintf(PS_OUT_FD, "/ss { 72 yline moveto show\n");
     fprintf(PS_OUT_FD, "      /yline yline 28 sub def } def\n\n");

     fprintf(PS_OUT_FD, "/box {moveto rlineto setlinewidth setlinecap stroke} def\n");


     fprintf(PS_OUT_FD, "/%s /French frenchvec\n", ((font && font[0]) ? font : "Times-Roman"));
     fprintf(PS_OUT_FD, "  ReEncodeSmall\n\n");

     fprintf(PS_OUT_FD, "/French findfont %lg scalefont\n", size);
     fprintf(PS_OUT_FD, "  setfont\n\n");
     if(PS_LANDSC) fprintf(PS_OUT_FD, "landscape\n");
     return(0);
}

/* GRAPHIC CHARS */

unsigned char PS_LEFT_PINS[40] = {
    0, 1, 2, 1, 1, 2, 2, 0, 2, 2, 1,
       2, 1, 0, 1, 1, 0, 1, 1, 0, 0,
       0, 0, 2, 2, 0, 2, 2, 2, 1, 2,
       1, 0, 0, 0, 0, 1, 2, 1, 0 };

unsigned char PS_RIGHT_PINS[40] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 1, 1, 1, 1, 1, 1, 2, 1,
       2, 2, 2, 2, 2, 2, 2, 2, 1, 2,
       1, 1, 2, 2, 1, 1, 2, 0, 1 };

unsigned char PS_UPPER_PINS[40] = {
    1, 1, 1, 2, 0, 0, 2, 2, 0, 2, 2,
       1, 0, 1, 1, 0, 1, 0, 1, 1, 2,
       2, 0, 2, 0, 2, 0, 2, 1, 2, 0,
       0, 2, 1, 0, 0, 2, 1, 1, 0 };

unsigned char PS_LOWER_PINS[40] = {
    1, 1, 1, 2, 2, 1, 2, 2, 2, 0, 0,
       0, 1, 0, 0, 1, 1, 0, 1, 1, 2,
       0, 2, 0, 2, 2, 0, 2, 0, 0, 1,
       2, 0, 0, 1, 2, 2, 1, 0, 1 };


/*NH*/
int PS_vert_graph(ch)
int     ch;
{
    int     pins;

    if(PS_UPPER_PINS[ch] && PS_LOWER_PINS[ch])
	fprintf(PS_OUT_FD, "2 %d %g %g %g %g box\n",
			    PS_UPPER_PINS[ch] - 1,
			    0.0, LINE_H,
			    PS_x() + (PS_FONTSIZE * 6.0) / 20.0,
			    PS_y());
    else {
	pins = PS_UPPER_PINS[ch];
	if(pins) fprintf(PS_OUT_FD, "2 %d %g %g %g %g box\n", pins - 1,
			    0.0, LINE_H / 2.0,
			    PS_x() + (PS_FONTSIZE * 6.0) / 20.0,
			    PS_y() + LINE_H / 2.0);
	pins = PS_LOWER_PINS[ch];
	if(pins) fprintf(PS_OUT_FD, "2 %d %g %g %g %g box\n", pins - 1,
				0.0, -LINE_H / 2.0,
			    PS_x() + (PS_FONTSIZE * 6.0) / 20.0,
			    PS_y() + LINE_H / 2.0);
	}
    return(0);
}

/*NH*/
int PS_graph(ch)
int     ch;
{
    PS_show();
    ch -= 179;
    if(PS_VNB < 0) PS_vert_graph(ch);
    else {
	if(PS_UPPER_PINS[ch]) PS_vsegm(PS_COL_NB, PS_LINE_NB * 2,     PS_UPPER_PINS[ch]);
	if(PS_LOWER_PINS[ch]) PS_vsegm(PS_COL_NB, PS_LINE_NB * 2 + 1, PS_LOWER_PINS[ch]);
	}
    if(PS_LEFT_PINS[ch])  PS_hsegm(PS_COL_NB * 2,     PS_LEFT_PINS[ch]);
    if(PS_RIGHT_PINS[ch]) PS_hsegm(PS_COL_NB * 2 + 1, PS_RIGHT_PINS[ch]);
    return(0);
}

/*NH*/
int PS_hsegm(pos, size)
int     pos, size;
{
    if(size != PS_G_SIZE || PS_G_POS + PS_G_LG != pos) {
	PS_hdraw();
	PS_G_POS = pos;
	}
    PS_G_SIZE = size;
    PS_G_LG++;
    return(0);
}

/*NH*/
int PS_hdraw()
{
    if(PS_G_LG <= 0) return(0);
    fprintf(PS_OUT_FD, "2 %d %g %g %g %g box\n", PS_G_SIZE - 1,
			(PS_FONTSIZE * PS_G_LG * 6.0) / 20.0, 0.0,
			(float)(PS_MARGH + (PS_G_POS * PS_FONTSIZE * 6.0) / 20.0),
			PS_y() + LINE_H / 2.0);
    PS_G_LG = PS_G_POS = PS_G_SIZE = 0;
    return(0);
}

/*NH*/
int PS_vsegm(col, pos, size)
int     col, pos, size;
{
    if(col >= PS_VNB) return(0);
    if(size != PS_VG_SIZE[col] || PS_VG_POS[col] + PS_VG_LG[col] != pos) {
	PS_vdraw(col);
	PS_VG_POS[col] = pos;
	}
    PS_VG_SIZE[col] = size;
    PS_VG_LG[col]++;
    return(0);
}

/*NH*/
int PS_vdraw(col)
int     col;
{
    if(col >= PS_VNB || PS_VG_LG[col] <= 0) return(0);
    fprintf(PS_OUT_FD, "2 %d %g %g %g %g box\n", PS_VG_SIZE[col] - 1,
	  0.0, -LINE_H * 0.5 * PS_VG_LG[col],
	  (float)(PS_MARGH + (col + 0.5) * PS_FONTSIZE * 0.6),
	  (float)(PS_PAGE_H - PS_MARGV - (PS_VG_POS[col] - 2) * LINE_H * 0.5));

    PS_VG_LG[col] = PS_VG_POS[col] = PS_VG_SIZE[col] = 0;
    return(0);
}







