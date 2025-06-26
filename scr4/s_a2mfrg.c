#include "s_a2m.h"

/*********************************************************************
 Fonctions et vars par défaut qui peuvent être remplacées par des fonctions
    spécifiques au logiciel appelant (par exemple IODE et chart.cpp

  A2mFrGrfPrepare : construit les brushes et les pens nécessaires
*********************************************************************/

int     A2MNBRPEN   = 12;
int     A2MNBRBRUSH  = 6;

extern  A2MPEN      *A2mFrGrfdPen;
extern  A2MBRUSH    *A2mFrGrfdBrush;

A2MPEN      *A2mFrGrfdPen;
A2MBRUSH    *A2mFrGrfdBrush;

A2MPEN      A2mFrGridPen = {0.5, "Grid", 0, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};

double A2MGRFPEN[5][5] = {0.0, 0.0, 0.0, 0.0, 0.0,
			  2.0, 8.0, 2.0, 0.0, 0.0,
			  3.0, 4.0, 2.0, 4.0, 0.0,
			  4.0, 2.0, 4.0, 2.0, 4.0,
			  4.0, 3.0, 3.0, 3.0, 3.0};

double A2MGRFBRUSH[5] = {3, 5, 8, 9, 12};

/*NH*/
A2mFrGrfPrepare()
{
    int         i, j, p;
    A2MPEN      *Pen;
    A2MBRUSH    *Brush;

    /* Alloc des A2MPEN et des A2MBRUSH */
    A2mFrGrfdPen   = (A2MPEN *) SCR_malloc(A2MNBRPEN * sizeof(A2MPEN));
    A2mFrGrfdBrush = (A2MBRUSH *) SCR_malloc(A2MNBRBRUSH * sizeof(A2MBRUSH));

    /* Default pen */
    Pen = A2mFrGrfdPen;
    Pen->p_width = 0.2;
    strcpy(Pen->p_color, "Black");
    Pen->p_nbsegments = 0;
    for(j = 0; j < 8; j++) Pen->p_segment[j] = 0.0;

    /* Pen */
    for(i = 1; i < A2MNBRPEN - 1; i++) {
	Pen = A2mFrGrfdPen + i;

	p = (i - 1)/2;
	if (i%2) Pen->p_width = 0.5;
	else     Pen->p_width = 1.0;
	strcpy(Pen->p_color, "Blue");

	Pen->p_nbsegments = (int) floor(A2MGRFPEN[p][0]);
	for(j = 0; j < Pen->p_nbsegments; j++) Pen->p_segment[j] = A2MGRFPEN[p][j];
	for(; j < 8; j++) Pen->p_segment[j] = 0.0;
    }

    /* Default Brush */
    Brush = A2mFrGrfdBrush;
    strcpy(Brush->b_color, "Black");
    Brush->b_fill = 15;

    /* Brush */
    for(i = 1; i < A2MNBRBRUSH; i++) {
	Brush = A2mFrGrfdBrush + i;

	strcpy(Brush->b_color, "Blue");
	Brush->b_fill = A2MGRFBRUSH[i-1];
    }
    return(0);
}







