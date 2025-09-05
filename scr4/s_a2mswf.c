#include "s_a2m.h"

/****************** SWF ***************************/

/* Public SWF */
int     A2M_SWF_NOGIF = 1; /* GB 15-06-10 */

/* Private SWF */
U_ch    SWF_JSPATH[256];
U_ch    SWF_JSEXPORTPATH[256];
U_ch    SWF_PHPEXPORTPATH[256];
U_ch    SWF_GRAPHPATH[256];
U_ch    SWF_PALETTE[256];
U_ch    SWF_FONTNAME[256];
U_ch    SWF_FONTCOLOR[256];
int     SWF_ANIMATE, SWF_DEBUG, SWF_ROUNDED, SWF_BORDER, SWF_GRADIENT, SWF_EXPORTENABLED,
	SWF_ANCHORS, SWF_FONTSIZE;


char *A2mPerReal2a(rper, nbper)
double  rper;
int     nbper;
{
    int     sper, year;
    double  floor();
    static char buf[25];

    year = (int)floor(rper + 0.001);
    if(nbper == 1) sprintf(buf, "%d", year);
    else {
	sper = (int) (.2 + ((double) nbper) * (rper - year));
	sprintf(buf, "%d:%d", year, sper);
    }
    return(buf);
}


int A2mSWF_HTML(FILE* fd, int w, int h, A2MGRF* grf, int cnt)
{
    int             i, j, maxobs = 0, paxis = 0, saxis = 0, period, type,
                    ndivlines, labelstep, showlegend, showsecondary = 0;
    unsigned char   *title, *stitle; //, *footnote;
    double          *vals;
    double          minx = 999999.0, maxx = -999999.0, step = SCR_NAN;

    if(grf->ag_gxy == NULL) return(0);

    title = 0;
    if(grf->ag_title) 
        title = grf->ag_title->ap_strs[0]->as_txt;
    if(title == 0) title = "No title";

    //if(grf->ag_footnote) 
    //    footnote = grf->ag_footnote->ap_strs[0]->as_txt;

    stitle = 0;
    for(i=1; grf->ag_title->ap_strs[i]; i++) {
	type = grf->ag_title->ap_strs[i]->as_type;
	if(type == A2M_TEXT)
	    stitle = grf->ag_title->ap_strs[i]->as_txt;
    }

    showlegend = 0;
    for(i = 0; grf->ag_gxy[i]; i++) {
	vals = grf->ag_gxy[i]->xy_vals;
	maxobs = _max_(maxobs, grf->ag_gxy[i]->xy_nobs);
	if(grf->ag_gxy[i]->xy_title) if(grf->ag_gxy[i]->xy_title->ap_strs[0]->as_txt) showlegend++;
	if(grf->ag_gxy[i]->xy_axis) showsecondary = 1;
	if(maxobs > 1 && !SCR_ISAN(step)) step = vals[2] - vals[0];
	for(j = 0; j < grf->ag_gxy[i]->xy_nobs; j++)  {
	    minx = _min_(minx, vals[j*2]);
	    maxx = _max_(maxx, vals[j*2]);
	}
    }

    period = 1; // year
    if(step < 1)  {
	if(step  < 0.2) period = 12;
	else period  = 4;
    }

    ndivlines = maxobs - 1; // not used in Combi Chart
    labelstep = period;
    while (maxobs > labelstep * 8) labelstep +=period;

    fprintf(fd, "<div id=\"chartdiv%d\">The chart will appear within this DIV.</div>", cnt);
    if(A2M_GR_MULTIBAR)
	fprintf(fd, "<script type=\"text/javascript\">   var myChart = new FusionCharts(\"%s/StackedColumn2D.swf\", \"IodeChrtId%d\", \"%d\", \"%d\", \"%d\", \"0\");   myChart.setDataXML(\"", SWF_GRAPHPATH, cnt, w, h, SWF_DEBUG);
    else
	fprintf(fd, "<script type=\"text/javascript\">   var myChart = new FusionCharts(\"%s/MSCombiDY2D.swf\", \"IodeChrtId%d\", \"%d\", \"%d\", \"%d\", \"0\");   myChart.setDataXML(\"", SWF_GRAPHPATH, cnt, w, h, SWF_DEBUG);
    fprintf(fd, "<chart Caption='");
    A2mOemToAnsi(fd, title);
    fprintf(fd, "' animation='%d' useRoundEdges='%d' showValues='0' formatNumberScale='0' thousandSeparator='.' decimalSeparator=',' setAdaptiveYMin='1' legendShadow='0' ", SWF_ANIMATE, SWF_ROUNDED);
    if(A2M_GR_MULTIBAR == 2) fprintf(fd, " stack100Percent='1' ");
    if(stitle) {
	fprintf(fd, "SubCaption='");
	A2mOemToAnsi(fd, stitle);
	fprintf(fd, "'");
    }
    fprintf(fd, " legendBgAlpha='0' legendBorderAlpha='0' ");
    if(!SWF_GRADIENT)
	fprintf(fd, " plotGradientColor=' ' ");

    if(SWF_PALETTE[0])
	fprintf(fd, " paletteColors='%s' ", SWF_PALETTE);

    if(!SWF_BORDER)
	fprintf(fd, " showPlotBorder='0' ");

    if(!A2M_GR_MULTIBAR) fprintf(fd, " labelStep='%d' numVDivLines='%d' ", labelstep, ndivlines);
    if(SWF_EXPORTENABLED) fprintf(fd, " exportEnabled='1' exportHandler='%s' exportAtClient='0' exportAction='download' ", SWF_PHPEXPORTPATH);
    if(!showlegend) fprintf(fd, " showLegend='0'");
    if(!showsecondary) fprintf(fd, " showDivLineSecondaryValue='0'  showSecondaryLimits='0'");
    if(maxobs > SWF_ANCHORS) fprintf(fd, " drawAnchors='0'");

    /* mins and maxs */
    if(SCR_ISAN(grf->ag_mins[1])) {
	fprintf(fd, " PYAxisMinValue ='%lg'", grf->ag_mins[1]);
	paxis = 1;
    }
    if(SCR_ISAN(grf->ag_maxs[1])) {
	fprintf(fd, " PYAxisMaxValue ='%lg'", grf->ag_maxs[1]);
	paxis = 1;
    }
    if(SCR_ISAN(grf->ag_mins[2])) {
	fprintf(fd, " SYAxisMinValue ='%lg'", grf->ag_mins[2]);
	saxis = 1;
    }

    if(SCR_ISAN(grf->ag_maxs[2])) {
	fprintf(fd, " SYAxisMaxValue ='%lg'", grf->ag_maxs[2]);
	saxis = 1;
    }

//    if(!paxis) fprintf(fd, " setAdaptiveYMin='1'");
//    if(!saxis) fprintf(fd, " setAdaptiveSYMin='1'");
    fprintf(fd, ">");

    fprintf(fd, "<categories>");
    if(!A2M_GR_MULTIBAR)  {
	for(i = 0; i < maxobs; i++) {
	    fprintf(fd, "<category label='%s' />", A2mPerReal2a(minx + i*step, period));
	    if (DrawVLine(minx + i*step, period))
		fprintf(fd, "<vLine thickness='1' dashed='1' color='%s' divLineDashLen='1' divLineDashGap='1' linePosition='%f' />",
			SWF_FONTCOLOR, (period > 1) ? 0.5 : 0 );
	}
    }
    else
    {
	for(i = 0; i < grf->ag_gxy[0]->xy_nobs; i++)  {
	    if(SCR_ISAN(vals[i*2+1])) {
		fprintf(fd, "<category label='%s' />", A2mPerReal2a(minx + i*step, period));
		if (DrawVLine(minx + i*step, period))
		    fprintf(fd, "<vLine thickness='1' dashed='1' color='%s' divLineDashLen='1' divLineDashGap='1' linePosition='%f' />",
			    SWF_FONTCOLOR, (period > 1) ? 0.5 : 0 );
	    }
	}
    }
    fprintf(fd, "</categories>");


    for(i = 0; grf->ag_gxy[i]; i++) {
	title = 0;
	if(grf->ag_gxy[i]->xy_title)
	    title = grf->ag_gxy[i]->xy_title->ap_strs[0]->as_txt;
	if(title == 0)  fprintf(fd, "<dataset seriesName='IODE'");
	else {
	    fprintf(fd, "<dataset seriesName='");
	    A2mOemToAnsi(fd, title);
	    fprintf(fd, "'");
	}
	if(grf->ag_gxy[i]->xy_type == 'L')
	    fprintf(fd, " renderAs='Line'");
	else
	    fprintf(fd, " renderAs='Bar'");
	if(grf->ag_gxy[i]->xy_axis)
	    fprintf(fd, " parentYAxis='S'");
	else
	    fprintf(fd, " parentYAxis='P'");
	fprintf(fd, ">");
	vals = grf->ag_gxy[i]->xy_vals;
	for(j = 0; j < grf->ag_gxy[i]->xy_nobs; j++)  {
	    if(SCR_ISAN(vals[j*2+1]))
		fprintf(fd, "<set value='%lg' />", vals[j*2+1]);
	    else if(!A2M_GR_MULTIBAR) fprintf(fd, "<set value='' />");
	}
	fprintf(fd, "</dataset>");

    }

    fprintf(fd, " <styles> <definition> <style name='CaptionFont' type='font' font='%s,Verdana' size='%d' color='%s' bold='1'  align ='left' /> <style name='SubCaptionFont' type='font' font='%s,Verdana' size='%d' color='%s' italic='1' align ='left' /> <style name='BFPFont' type='font' font='%s,Verdana' size='%d' color='%s' align ='left'/> </definition><application><apply toObject='Caption' styles='CaptionFont' /><apply toObject='SubCaption' styles='SubCaptionFont' /><apply toObject='Legend' styles='BFPFont' /><apply toObject='YAXISVALUES' styles='BFPFont' /><apply toObject='DATALABELS' styles='BFPFont' /></application></styles> ",
	    SWF_FONTNAME, SWF_FONTSIZE + 2, SWF_FONTCOLOR,
	    SWF_FONTNAME, SWF_FONTSIZE, SWF_FONTCOLOR,
	    SWF_FONTNAME, SWF_FONTSIZE, SWF_FONTCOLOR);

    fprintf(fd, "</chart>\");   myChart.render(\"chartdiv%d\");</script>\n", cnt);

    return(0);

}

/****************** SWF ***************************/

int A2mSWFIni()
{
    char filename[256], buf[256];

    A2mGetModulePath(filename);
    strcat(filename, "\\swf.ini");

    if(IniReadTxtParm(filename, "SWF", "JSPATH", buf, 255) == 0)
	strcpy(SWF_JSPATH, buf);
    else strcpy(SWF_JSPATH, "https://www.plan.be/FusionChartsXT/FusionCharts.js");

    if(IniReadTxtParm(filename, "SWF", "JSEXPORTPATH", buf, 255) == 0)
	strcpy(SWF_JSEXPORTPATH, buf);
    else SWF_JSEXPORTPATH[0] = 0;

    if(IniReadTxtParm(filename, "SWF", "PHPEXPORTPATH", buf, 255) == 0)
	strcpy(SWF_PHPEXPORTPATH, buf);
    else SWF_PHPEXPORTPATH[0] = 0;

    if(IniReadTxtParm(filename, "SWF", "GRAPHPATH", buf, 255) == 0)
	strcpy(SWF_GRAPHPATH, buf);
    else strcpy(SWF_GRAPHPATH, "/FusionCharts/Charts");

    if(IniReadTxtParm(filename, "SWF", "PALETTE", buf, 255) == 0)
	strcpy(SWF_PALETTE, buf);
    else SWF_PALETTE[0] = 0;

    if(IniReadTxtParm(filename, "SWF", "FONTNAME", buf, 255) == 0)
	strcpy(SWF_FONTNAME, buf);
    else strcpy(SWF_FONTNAME, "Verdana");

    if(IniReadTxtParm(filename, "SWF", "FONTCOLOR", buf, 255) == 0)
	strcpy(SWF_FONTCOLOR, buf);
    else strcpy(SWF_FONTCOLOR,"FFFFFF");

    IniReadNumParm(filename, "SWF", "ANIMATE", &SWF_ANIMATE);
    IniReadNumParm(filename, "SWF", "DEBUG", &SWF_DEBUG);
    IniReadNumParm(filename, "SWF", "ROUNDED", &SWF_ROUNDED);
    IniReadNumParm(filename, "SWF", "GRADIENT", &SWF_GRADIENT);
    IniReadNumParm(filename, "SWF", "BORDER", &SWF_BORDER);
    IniReadNumParm(filename, "SWF", "MAXANCHORS", &SWF_ANCHORS);
    SWF_FONTSIZE = 10;
    IniReadNumParm(filename, "SWF", "FONTSIZE", &SWF_FONTSIZE);
    IniReadNumParm(filename, "SWF", "EXPORTENABLED", &SWF_EXPORTENABLED);
    
    return(0);
}

DrawVLine(double rper, int nbper)
{
    int     sper, year;
    double  floor();

    if(nbper == 1) return(1);

    year = (int)floor(rper + 0.001);
    sper = (int)(1.2 + ((double) nbper) * (rper - year));
    if(sper == nbper) return(1);

    return(0);
}



