#include "s_a2m.h"

extern  U_ch    A2M_GALIGN;

int    A2M_GR_MULTIBAR; /* JMP 21-11-2011 remplace B_MULTIBAR */

int     A2MGRFBARCURRENT, A2MGRFBARNB, A2MGRFMAXOBS, A2MGRFHASZ;
double  A2MGRFBARWIDTH;

int     A2MGRFID = 0, A2MGRFOBJID=1, A2MGRFFONTSIZE=10;
double  A2MGRFDIM[3], A2MGRF0[3], A2MGRFSCALE[3], A2MGRFORIG[3],
	A2M_Margins[3];


#ifdef __cplusplus
int (**A2M_GRF_FNS)(...);
#else
int (**A2M_GRF_FNS)();
#endif

A2mGrfPrint(agrf)
A2MGRF  *agrf;
{
    A2mGrfInit(agrf);
    A2mGrf(agrf);
    A2mGrfEnd(agrf);
    return(0);
}

A2mGrfInit(ag)
A2MGRF  *ag;
{
    double  width = ag->ag_size[0],
	    height = ag->ag_size[1];

	A2MGRFID = A2mGrfBeginObj();
    memset(A2MGRF0, 0, sizeof(double) * 3);
    A2MGRFDIM[0] = width * A2MPCM;
    A2MGRFDIM[1] = height * A2MPCM;
    A2MGRFDIM[2] = height * A2MPCM;

    A2mDEVGrfInit(ag);
    return(0);
}

A2mGrfEnd(ag)
A2MGRF  *ag;
{
    A2mGrfGroup();
    A2mDEVGrfEnd(ag);
    return(0);
}

double GR_Box[4];

A2mGrf(agrf)
A2MGRF  *agrf;
{
    A2mGrfTitle(agrf);
    A2mGrfGrid(agrf);
    A2mGrfXy(agrf);
    A2mGrfLegend(agrf);

	/* add box last */
	A2mGrfBox(0, GR_Box[0], GR_Box[1], GR_Box[2], GR_Box[3], 0);
    return(0);
}

A2mGrfTitle(agrf)
A2MGRF  *agrf;
{
	return(0);
}

A2mGrfLegend(agrf)
A2MGRF  *agrf;
{
    double  x, y;

    if(agrf->ag_legends[0]) {
		x = GR_WIDTH-A2M_Margins[0]; y = A2M_Margins[1]- 2*A2M_FONTSIZE;
		A2mDEVGrfPar(-1, x, y, agrf->ag_legends[0], "Right", "Black");
	}

	if(agrf->ag_legends[1]) {
		x = A2M_Margins[0]; y = GR_HEIGHT - A2M_FONTSIZE;
		A2mDEVGrfPar(-1, x, y, agrf->ag_legends[1], "Left", "Black");
	}

	if(agrf->ag_legends[2]) {
		x = GR_WIDTH-A2M_Margins[0]; y = GR_HEIGHT - A2M_FONTSIZE;
		A2mDEVGrfPar(-1, x, y, agrf->ag_legends[2], "Right", "Black");
    }
    return(0);

}

A2mGrfBarNbr(agrf)
A2MGRF  *agrf;
{
	int i, j, maxobs = 0, nobs;

	A2MGRFHASZ                  = 0;
	A2MGRFBARNB         = 0;
	A2MGRFBARCURRENT    = 0;
	A2MGRFBARWIDTH      = 10.0;


	for(i = 0; agrf->ag_gxy[i]; i++) {
		if((agrf->ag_gxy[i])->xy_axis == 1) A2MGRFHASZ = 1;
		if((agrf->ag_gxy[i])->xy_type != 'B') continue;

		A2MGRFBARNB++;
		nobs = 0;
		for(j = 1; j < 2 * (agrf->ag_gxy[i])->xy_nobs; j = j + 2) {
		    if(SCR_ISAN((agrf->ag_gxy[i])->xy_vals[j])) nobs ++;
		}
		if(nobs > maxobs) maxobs = nobs;
    }
	if(A2MGRFBARNB != 0) {
		if(A2M_GR_MULTIBAR > 0) A2MGRFBARNB = 1 + A2MGRFHASZ;
		A2MGRFBARWIDTH = (agrf->ag_size[0] * A2MPCM)/((maxobs + 1) * A2MGRFBARNB * 2);
	}

	A2MGRFMAXOBS = maxobs;
    return(0);
}

/*
A2mGrfBarNbr(agrf)
A2MGRF  *agrf;
{
    int i, maxobs = 0;

    A2MGRFBARNB         = 0;
    A2MGRFBARCURRENT    = 0;
    A2MGRFBARWIDTH      = 10.0;

    for(i = 0; agrf->ag_gxy[i]; i++) {
	if((agrf->ag_gxy[i])->xy_type != 'B') continue;

	A2MGRFBARNB++;
	if((agrf->ag_gxy[i])->xy_nobs > maxobs)
	    maxobs = (agrf->ag_gxy[i])->xy_nobs;
    }
    if(A2MGRFBARNB != 0)
	A2MGRFBARWIDTH = (agrf->ag_size[0] * A2MPCM)/
	    ((maxobs + 1) * A2MGRFBARNB * 2);

	return(0);
}
*/
int A2M_NBGR;

int     GR_nbdec[3];
double  GR_incr[3], GR_left[3], GR_right[3];

A2mGrfGrid(agrf)
A2MGRF  *agrf;
{
	double  nb_axes;
	int     i;
	int     nbdec[3], nbgr;
	double  mins[3], maxs[3], incr[3], left[3], right[3], lbar[3], diff;

	// GB 16/03/2009
	if(agrf->ag_gxy == NULL) return(0);
	A2mGrfBarNbr(agrf);
	A2M_NBGR = A2mGrfMins(agrf, mins, maxs); // GB 16/03/2009
	if(A2M_NBGR > 2) nbgr = 3;
	else  nbgr = A2M_NBGR;

	for(i = 0; i < 3; i ++) {
		A2mGrfIncr(agrf, agrf->ag_grid[i], mins[i], maxs[i],
			incr + i, nbdec + i, left + i, right + i, lbar +i);

		if(SCR_ISAN(agrf->ag_mins[i]) && left[i] < agrf->ag_mins[i])
			left[i] = agrf->ag_mins[i];
		if(SCR_ISAN(agrf->ag_maxs[i]) && right[i] > agrf->ag_maxs[i])
			right[i] = agrf->ag_maxs[i];

		if(i == 0 && A2MGRFBARNB != 0) {
			if(left[0]  == mins[0]) left[0]  -= lbar[0];
			if(right[0] == maxs[0]) right[0] += lbar[0];
		}
	}

	if(SCR_ISAN(left[2]) && SCR_ISAN(right[2])) // GB 16/03/2009
		nb_axes = 2.0;
	else
		nb_axes = 1.333;

	A2M_Margins[0] = 3*A2M_FONTSIZE;
	A2M_Margins[1] = (2 + nbgr) *A2M_FONTSIZE; // GB 16/03/2009
	A2M_Margins[2] = (2 + nbgr) *A2M_FONTSIZE; // GB 16/03/2009

	A2mGrfTranslate(A2M_Margins[0], A2M_Margins[1], A2M_Margins[2]);
	for(i = 0; i < 3; i ++) { // GB 16/03/2009
		if(!SCR_ISAN(right[i]) || !SCR_ISAN(left[i]))
			A2MGRFSCALE[i] = SCR_NAN;
		else {
			diff = right[i] - left[i];
			if(diff == 0)
				A2MGRFSCALE[i] = 1.0; /* JMP 26-04-02 */
			else {
			if(i == 0)
//                              A2MGRFSCALE[i] = (A2MGRFDIM[i] - (2*A2M_Margins[i]))/(right[i] - left[i]);
				A2MGRFSCALE[i] = (A2MGRFDIM[i] - (nb_axes*A2M_Margins[i]))/(right[i] - left[i]);
			else
//                              A2MGRFSCALE[i] = (A2MGRFDIM[i] - (A2M_Margins[i]+A2M_Margins[0]/2))/(right[i] - left[i]);
				A2MGRFSCALE[i] = (A2MGRFDIM[i] - A2M_Margins[i] - A2M_FONTSIZE)/(right[i] - left[i]);
			}
		}
	}

	A2mGrfOrigin(left, right);
	A2mGrfPlotGrid(agrf, left, right, incr, nbdec);
	return(0);
}

A2mGrfXy(agrf)
A2MGRF  *agrf;
{
    if(agrf->ag_gxy == NULL) return(0);

    A2mGrfBeginObj();
    A2mGrfPlot(agrf);
    A2mGrfEndObj();
    return(0);
}

A2mGrfBeginObj()
{
    return(GR_obj_id++);
}

A2mGrfEndObj()
{
    A2mDEVGrfGroupObj();
    return(0);
}

A2mGrfGroup()
{
    A2mDEVGrfGroup();
    return(0);
}

double *A2MGRFMULTI_Y, *A2MGRFMULTI_Z;
int             A2MGRFMULTI_Y_STACK, A2MGRFMULTI_Z_STACK;

A2mGrfPlot(agrf)
A2MGRF  *agrf;
{
	int     i, pen = 1, brush = 1, sign = 1;
	A2MGXY  *gxy;

	A2MGRFBARNB = 0;
	for(i = 0; agrf->ag_gxy[i]; i++) {
		gxy = agrf->ag_gxy[i];
		if(gxy->xy_type == 'B' && A2M_GR_MULTIBAR == 0) A2MGRFBARNB++;
	}

	if(A2M_GR_MULTIBAR > 0) {
		A2MGRFBARNB = 1 + A2MGRFHASZ;
		A2MGRFMULTI_Y = SCR_malloc(A2MGRFMAXOBS * sizeof(double));
		A2MGRFMULTI_Y_STACK = 0;
		A2MGRFMULTI_Z = SCR_malloc(A2MGRFMAXOBS * sizeof(double));
		A2MGRFMULTI_Z_STACK = 0;
	}

	for(i = 0; agrf->ag_gxy[i]; i++) {
		gxy = agrf->ag_gxy[i];

		A2mGrfBeginObj();

		switch(gxy->xy_type) {
			case 'L' :  A2mGrfPlotXy(gxy->xy_axis, gxy->xy_type,
				gxy->xy_nobs, gxy->xy_vals, pen);
				A2mGrfLabel(i, gxy->xy_type, gxy->xy_title, pen);
				pen++;
				break;
			case 'B' :  A2mGrfPlotXy(gxy->xy_axis, gxy->xy_type,
				gxy->xy_nobs, gxy->xy_vals, brush);
				A2mGrfLabel(i, gxy->xy_type, gxy->xy_title, brush);
				if(A2M_GR_MULTIBAR > 0) {
					if(gxy->xy_axis == 0) A2MGRFMULTI_Y_STACK = 1;
					else                              A2MGRFMULTI_Z_STACK = 1;
				}
				brush ++;
				break;
			case 'S' :
				sign ++;
				break;
		}
		A2mGrfEndObj();
	}

	if(A2M_GR_MULTIBAR > 0) {
		SCR_free(A2MGRFMULTI_Y); A2MGRFMULTI_Y = NULL;
		SCR_free(A2MGRFMULTI_Z); A2MGRFMULTI_Z = NULL;
	}
	return(0);
}



A2mGrfOrigin(left, right)
double  *left, *right;
{
	GR_X_origin = GR_Y_origin = GR_Z_origin = 0.0;

	if(right[1] < 0) {
	A2mGrfTranslate(0.0, (double) fabs(right[1] - left[1]) * GR_Y_scale, 0.0);
	GR_Y_origin = right[1];
	}
	else {
	if(left[1] < 0) A2mGrfTranslate(0.0, (double) fabs(left[1]) * GR_Y_scale, 0.0);
	else GR_Y_origin = left[1];
	}

	if(right[2] < 0) {
	A2mGrfTranslate(0.0, 0.0, (double) fabs(right[2] - left[2]) * GR_Z_scale);
	GR_Z_origin = right[2];
	}
	else {
	if(left[2] < 0) A2mGrfTranslate(0.0, 0.0, (double) fabs(left[2]) * GR_Z_scale);
	else GR_Z_origin = left[2];
	}

    if(left[0] < 0) A2mGrfTranslate((double) fabs(left[0]) * GR_X_scale, 0.0, 0.0);
    else GR_X_origin = left[0];

    return(0);
}

A2mGrfTranslate(x, y, z)
double  x, y, z;
{
    GR_X_0 += x;
    GR_Y_0 += y;
    GR_Z_0 += z;
    return(0);
}

A2mGrfBox(axis, x, y, w, h, props)
int     axis, props;
double  x, y, w, h;
{
    double  y_origin, y_scale;

    if(axis == 0) {
		y_origin = GR_Y_origin;
		y_scale  = GR_Y_scale;
	}
	else {
		y_origin = GR_Z_origin;
		y_scale  = GR_Z_scale;
	}

	A2mDEVGrfBox(axis, (x - GR_X_origin) * GR_X_scale, (y - y_origin) * y_scale,
	w * GR_X_scale, h * GR_Y_scale, props);

	return(0);
}


A2mGrfLine(axis, x1, y1, x2, y2, props)
int     axis, props;
double  x1, y1, x2, y2;
{
	double  y_origin, y_scale;

	if(axis == 0) {
		y_origin = GR_Y_origin;
		y_scale  = GR_Y_scale;
	}
	else {
		y_origin = GR_Z_origin;
		y_scale  = GR_Z_scale;
	}
	A2mDEVGrfLine(axis, (x1 - GR_X_origin) * GR_X_scale, (y1 - y_origin) * y_scale,
		(x2 - GR_X_origin) * GR_X_scale, (y2 - y_origin) * y_scale, props);

	return(0);
}

A2mGrfPlotGrid(agrf, left, right, incr, nbdec)
A2MGRF  *agrf;
int     *nbdec;
double  *left, *right, *incr;
{
	int      i, k, l, level, clevel;
	double   j, start, stop;
	char     fmt[10], label[80];
	A2MPAR   *ap;
	A2MLAB   **labs;
	A2MGRID  **grids;

//    A2mGrfBox(0, left[0], left[1], right[0] - left[0], right[1] - left[1], 0);
	for(i = 0; i < 3; i++) {
		A2mGrfBeginObj();
		if(!SCR_ISAN(left[i]) || !SCR_ISAN(right[i])) continue;

		if(i == 0) k = 1;
		else k = 0;

		sprintf(fmt, "%c.%df", '%', nbdec[i]);
		start = ceil(left[i]/incr[i]) * incr[i];
		stop = right[i];

		level = 1 + ((stop - start)/incr[i])/20;   // GB 22/04/04
		clevel = 0;                                // GB 22/04/04
		for(j = start; j <= stop; j += incr[i]) {
			if(clevel == level) clevel = 0;
			sprintf(label, fmt, j);
			ap = A2mTxt2Par("Graph", label);
			A2mGrfGridLine(agrf, agrf->ag_grid[i],
				left[k], right[k], j, ap, i, right[i], clevel % level); // GB 22/04/04
			A2mFreePar(ap);
			clevel ++;
		}
		A2mGrfEndObj();
	}

	for(i = 0; i < 3; i++) {
		if(agrf->ag_labs[i] == NULL) continue;

		A2mGrfBeginObj();
		if(i == 0) k = 1;
		else k = 0;

		labs = agrf->ag_labs[i];
		for(l = 0; labs[l]; l++) {
			A2mGrfGridLine(agrf, 'M', left[k], right[k],
			labs[l]->al_val, labs[l]->al_par, i, right[i], 0); // GB 22/04/04
		}
		A2mGrfEndObj();
	}

	for(i = 0; i < 3; i++) {
		if(agrf->ag_grids[i] == NULL) continue;

		A2mGrfBeginObj();
		if(i == 0) k = 1;
		else k = 0;

		grids = agrf->ag_grids[i];
		for(l = 0; grids[l]; l++) {
			A2mGrfGridLine(agrf, grids[l]->ag_type, left[k], right[k],
			grids[l]->ag_val, NULL, i, right[i], 0);          // GB 22/04/04
		}
		A2mGrfEndObj();
	}


/*      A2mGrfBox(0, left[0], left[1], right[0] - left[0], right[1] - left[1], 0);*/

	GR_Box[0] = left[0];
	GR_Box[1] = left[1];
	GR_Box[2] = right[0] - left[0];
	GR_Box[3] = right[1] - left[1];

	return(0);
}

int A2mGrfGridLine(agrf, type, left, right, span, text, dir, maxi, level)
A2MGRF  *agrf;
double  left, right, span, maxi;
A2MPAR *text;
int     type, dir, level;
{
	int     axis;
	char    align[10];
	double  w, x1, x2, y1, y2, xt, yt, lf, m;

	if(U_is_in(agrf->ag_grid[dir], "Tt") && A2M_GALIGN == 'M') m = 0.5;
	else m = 0.0;

	if(span == 0.0) type = 'M'; /* NULL Axis */
	if(type == 'N') return(0);

	switch(dir) {
	case 1:
	case 2: lf = A2M_FONTSIZE/GR_X_scale;
		break;
	case 0: lf = A2M_FONTSIZE/GR_Y_scale;
		break;
    }

    switch(type) {
	case 't' :
	case 'm' :
	    w = left + lf/2; break;
	case 'T' :
	case 'M' :
	    w = right; break;
    }

	switch(dir) {
	case 0: x1 = span, y1 = left,
		x2 = span, y2 = w;
		xt = x2, yt = y1 - lf * (1+level*0.75);  // GB 22/04/04
		axis = 0;
		strcpy(align, "Center");
		break;
	case 1: x1 = left, y1 = span,
		x2 = w, y2 = span;
		xt = x1 - lf/2, yt = y1;
		axis = 0;
		strcpy(align, "Right");
		break;
	case 2:
		x1 = right - lf/2; /* Right always minor */
		x2 = right;
		y1 = span;
		y2 = span;
		xt = right + lf/2, yt = y1;
		axis = 1;
		strcpy(align, "Left");
		break;
    }

    if(type != 'N')  A2mGrfLine(axis, x1, y1, x2, y2, -1);   /* JMP 26-04-02 */

    if(text != NULL) {
	if(U_is_in(agrf->ag_grid[dir], "Tt") && A2M_GALIGN == 'M') {
	    if(xt + m <= maxi)
	       A2mGrfPar(axis, xt + m, yt, text, align, "Black");
	}
	else
	    A2mGrfPar(axis, xt, yt, text, align, "Black");
	}

    return(0);
}

A2mGrfPar(axis, x1, y1, par, align, color)
int     axis;
double  x1, y1;
A2MPAR  *par;
char    *align, *color;
{
    char    *ptr;
    double  y_origin, y_scale;

    if(axis == 0) {
	y_origin = GR_Y_origin;
	y_scale  = GR_Y_scale;
    }
    else {
	y_origin = GR_Z_origin;
	y_scale  = GR_Z_scale;
    }

    A2mDEVGrfPar(axis, (x1 - GR_X_origin) * GR_X_scale,
		 (y1 - y_origin) * y_scale,
		 par, align, color);

    return(0);
}

A2mGrfPlotXy(axis, type, nobs, vals, props)
int     axis, type, nobs, props;
double  *vals;
{
	int     i, *stacked;
	double  *scaled, width;
	double  y_origin, y_scale;
	double  *multi;

	if(axis == 0) {
		y_origin = GR_Y_origin;
		y_scale  = GR_Y_scale;
		multi = A2MGRFMULTI_Y;
		stacked = &A2MGRFMULTI_Y_STACK;
	}
	else {
		y_origin = GR_Z_origin;
		y_scale  = GR_Z_scale;
		multi = A2MGRFMULTI_Z;
		stacked = &A2MGRFMULTI_Z_STACK;
	}

	scaled = (double *) SCR_malloc(2 * nobs * sizeof(double));
	for(i = 0; i < nobs; i++) {
		if(SCR_ISAN(vals[i*2])) scaled[i*2] = (vals[i*2] - GR_X_origin) * GR_X_scale;
		else scaled[i*2] = SCR_NAN;
		if(SCR_ISAN(vals[i*2+1])) {
			if(A2M_GR_MULTIBAR == 0 || *stacked == 0)
				scaled[i*2+1] = (vals[i*2+1] - y_origin) * y_scale;
			else {
				scaled[i*2+1] = (vals[i*2+1]) * y_scale;
			}
		}
		else scaled[i*2+1] = SCR_NAN;

	}

	width = A2MGRFBARWIDTH;
	switch(type) {
	case 'L' :
		A2mGrfPolyLine(axis, nobs, scaled, props);
		break;

	case 'B' :
		if(A2M_GR_MULTIBAR > 0) {
			if (axis == 0) A2MGRFBARCURRENT = 0;
			else               A2MGRFBARCURRENT = 1;
		}
		A2mGrfPolyBar(axis, nobs, scaled, width, props);
		if(A2M_GR_MULTIBAR == 0) A2MGRFBARCURRENT++;
		break;
	}

	for(i = 0; i < nobs; i++) {
		if(SCR_ISAN(vals[i*2+1])) {
			if(A2M_GR_MULTIBAR > 0)
				if(*stacked == 0) {
					multi[i] += (vals[i*2+1] - y_origin) * y_scale;
				}
				else
					multi[i] += (vals[i*2+1]) * y_scale;
		}
	}

    SCR_free(scaled);
    return(0);
}

A2mGrfPolyBar(axis, nobs, vals, width, props)
int     axis, nobs, props;
double  *vals, width;
{
    int     i, nbar = A2MGRFBARNB, cbar = A2MGRFBARCURRENT;
    double      *multi;


    if(axis == 0) {
		multi = A2MGRFMULTI_Y;
    }
    else {
		multi = A2MGRFMULTI_Z;
	}

	for(i = 0; i < nobs; i++) {
		if(!SCR_ISAN(vals[i*2]) || !SCR_ISAN(vals[i*2+1])) continue;
		if(A2M_GR_MULTIBAR == 0) A2mDEVGrfBox(axis, vals[i*2] - (nbar/2.0 - cbar)*width, 0.0, width, vals[i*2+1], props);
		else A2mDEVGrfBox(axis, vals[i*2] - (nbar/2.0 - cbar)*width, multi[i], width, vals[i*2+1], props);
	}

	return(0);
}

int A2mGrfMins(agrf, mins, maxs) // GB 16/03/2009
A2MGRF  *agrf;
double  mins[3], maxs[3];
{
	int     i, j, a, nbgr,hasZ;
	A2MGXY  *gxy;
	double  *multiy = 0, *multiz = 0;

	for(i = 0; i < 3; i++) {
		mins[i] = SCR_NAN;
		maxs[i] = SCR_NAN;
	}

	hasZ = A2MGRFHASZ;
	if(A2M_GR_MULTIBAR > 0) {
/*
		maxnobs = 0;
		for(i = 0; agrf->ag_gxy[i]; i++) {
			gxy = agrf->ag_gxy[i];
			maxnobs = _max_(maxnobs, gxy->xy_nobs);
		}
*/
		multiy = SCR_malloc(A2MGRFMAXOBS * sizeof(double));
		multiz = SCR_malloc(A2MGRFMAXOBS * sizeof(double));
	}

	nbgr = 0;
	for(i = 0; agrf->ag_gxy[i]; i++) {
		nbgr ++;
		gxy = agrf->ag_gxy[i];
		for(j = 0; j < gxy->xy_nobs; j++) {
			a = 1 + gxy->xy_axis;

			if(SCR_ISAN(gxy->xy_vals[2*j])) {
				/* MIN */
				if(!SCR_ISAN(mins[0])) mins[0] = gxy->xy_vals[2 * j];
				else {
					if(mins[0] > gxy->xy_vals[2 * j]) mins[0] = gxy->xy_vals[2 * j];
				}

				/* MAX */
				if(!SCR_ISAN(maxs[0])) maxs[0] = gxy->xy_vals[2 * j];
				else {
					if(maxs[0] < gxy->xy_vals[2 * j])
					maxs[0] = gxy->xy_vals[2 * j];
				}
			}

			if(SCR_ISAN(gxy->xy_vals[2*j +1])) {
				/* MIN */
				if(!SCR_ISAN(mins[a])) mins[a] = gxy->xy_vals[2 * j + 1];
				else {
					if(mins[a] > gxy->xy_vals[2 * j + 1])
					mins[a] = gxy->xy_vals[2 * j + 1];
				}

				/* MAX */
				if(A2M_GR_MULTIBAR > 0) {
					if(a == 1) multiy[j] += gxy->xy_vals[2 * j + 1];
					else       multiz[j] += gxy->xy_vals[2 * j + 1];
				}
				if(!SCR_ISAN(maxs[a])) maxs[a] = gxy->xy_vals[2 * j + 1];
				else {
					if(maxs[a] < gxy->xy_vals[2 * j + 1])
					maxs[a] = gxy->xy_vals[2 * j + 1];
				}
			}
		}
	}

	if(A2M_GR_MULTIBAR > 0) {
		mins[1] = mins[2] = 0;
		for (j = 0; j < A2MGRFMAXOBS; j ++) {
			if(!SCR_ISAN(maxs[1]))  maxs[1] = multiy[j];
			else if(maxs[1] < multiy[j]) maxs[1] = multiy[j];

			if(hasZ)  {
				if(!SCR_ISAN(maxs[2]))  maxs[2] = multiz[j];
				else if(maxs[2] < multiz[j]) maxs[2] = multiz[j];
			}
		}

		SCR_free(multiy);
		SCR_free(multiz);
	}

	for(i = 0; i < 3; i++) {
		if(SCR_ISAN(agrf->ag_mins[i])) mins[i] = _min_(mins[i], agrf->ag_mins[i]);
		if(SCR_ISAN(agrf->ag_maxs[i])) maxs[i] = _max_(maxs[i], agrf->ag_maxs[i]);

		if(mins[i] == maxs[i]) {
			mins[i] -= 1.0;
			maxs[i] += 1.0;
		}
	}

	return(nbgr);
}

A2mGrfIncr(agrf, grid, mini, maxi, incr, nbdec, left, right, lbar)
A2MGRF  *agrf;
short   grid;
double  mini, maxi, *left, *right, *incr, *lbar;
int     *nbdec;
{
    double  d, ld, x;

    if(!SCR_ISAN(mini) || !SCR_ISAN(maxi)) {
		*left = *incr = *right = SCR_NAN;
		*nbdec = -1;
		return(0);
	}

	d = (double) maxi - mini;
	ld =  pow(10.0, floor(log10(d)));

	while(d/ld  <= 3) ld /= 2;
	*incr = ld;
//  *lbar = ld;
	if(ld > 1) *lbar = ld/5.0;
	else *lbar = ld;

	if(grid == 'T' || grid == 't') {  /* GB 22/04/2004  */
		if (A2M_GALIGN == 'M' || A2M_GALIGN == 'm') *incr = 1;
		else *incr = _max_(1.0, floor(*incr));
	}

	/* get nbr of significant decimals */
	x = *incr - floor(*incr);
	if (x == 0.0) x = *incr;
	*nbdec = 0;
	while(x < 1 && x != 0) {
		(*nbdec)++;
		x *= 10;
		x =  x - floor(x);
	}

	if(grid == 'T' || grid == 't') {  /* GB 20/05/2004 no gap at right and left*/
		*left = mini;
		*right = maxi;
	}
	else {
		*left = (ceil(mini/(*incr)) - 1) * *incr;
		*right= (floor(maxi/(*incr)) + 1) * *incr;
	}

	if(fabs(mini) - fabs(*left) == *incr) *left = mini;
	if(fabs(*right) - fabs(maxi) == *incr) *right = maxi;

	return(0);
}

int A2mEmptyPar(ap)
A2MPAR  *ap;
{
	int             i;
	A2MSTR  *as;

	for(i = 0; i < ap->ap_strs[i]; i++) {
		as = ap->ap_strs[i];
		if(as  && as->as_txt && as->as_txt[0] != 0) return(0);   // Not Empty
	}
	return(1); // Empty Par
}

A2mGrfLabel(nbr, type, title, props)
int     nbr, type, props;
A2MPAR  *title;
{
	int         i, line, col;
	double      x, y, width;

	if(A2mEmptyPar(title)) return(0); // GB 17/03/2009

	line = nbr%3;
	col = nbr/3;

	x = col * GR_WIDTH/2.0 + A2M_FONTSIZE;
	if(A2M_NBGR < 4)
		y = (0.9 * A2M_NBGR - line) * A2M_FONTSIZE;
	else
		y = (2.7 - line) * A2M_FONTSIZE;

    switch(type) {
    /*
	case 'L' :  A2mDEVGrfLine(0,
			x - GR_X_0,
			y - GR_Y_0 - A2M_FONTSIZE / 2,
			x + 2 * A2M_FONTSIZE - GR_X_0,
			y - GR_Y_0 - A2M_FONTSIZE / 2,
			props);
		    break;
	case 'B' :  A2mDEVGrfBox(0,
			x - GR_X_0, y - GR_Y_0 - A2M_FONTSIZE,
			(double) 2.0 * A2M_FONTSIZE, (double) 0.75 * A2M_FONTSIZE,
			props);
		    break;
    */
	case 'L' :  A2mDEVGrfLine(-1,
			x,
			y + A2M_FONTSIZE / 3,
			x + 2 * A2M_FONTSIZE,
			y + A2M_FONTSIZE / 3,
			props);
		    break;
	case 'B' :  A2mDEVGrfBox(-1,
			x, y - 0.075 * A2M_FONTSIZE,
			(double) 2.0 * A2M_FONTSIZE, (double) 0.75 * A2M_FONTSIZE,
			props);
		    break;
    }

    A2mDEVGrfPar(-1, x + 3.0*A2M_FONTSIZE, y, title, "Left", "Black");

    return(0);
}


A2mGrfPolyLine(axis, nobs, vals, props)
int     axis, nobs, props;
double  *vals;
{
    int     i, start, stop;

    i = 0;
    while(i < nobs) {
		for(; i < nobs; i++) if(SCR_ISAN(vals[i*2]) && SCR_ISAN(vals[i*2+1])) break;
		start = i;

		for(; i < nobs; i++) if(!SCR_ISAN(vals[i*2]) || !SCR_ISAN(vals[i*2+1])) break;
		stop = i;

		A2mDEVGrfPolyLine(axis, stop - start, vals + (start * 2), props);
	}

	return(0);
}












