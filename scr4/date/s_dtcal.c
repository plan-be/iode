#include "s_date.h"
#include "scr.h"

char    *DT_MONTHS_NAMES[] = {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
    };

char    *DT_DAYS_NAMES = "Mo Tu Wd Th Fr Sa Su";     /* BP_M 25-10-95 */

/* ====================================================================
Affiche un petit calendrier dans l'écran et permet de visualiser les
mois et les années qui entourent la date donnée.

&EN date indique le mois qui s'affiche au départ
&EN bline indique la ligne du bord supérieur du calendrier
&EN bcol indique la colonne du bord gauche du calendrier

L'éditeur reconnaît les touches suivantes :

&EN UP ou PGUP : retour en arrière de une année
&EN DOWN ou PGDN : avance d'une année
&EN LEFT : retour en arrière d'un mois
&EN RIGHT : avance d'un mois
&EN ESCAPE : quitte l'édition du calendrier
&EN Touche de l'application : quitte l'édition du calendrier

&EX
    OPTION "Calendar" {C_FN DT_calendar(SCR_current_date(), 2, 2);}
&TX
&SA SCR_current_date()
======================================================================= */

int DT_calendar(date, bline, bcol)
long    date;
int     bline, bcol;
{
    long    d[3],
	    boxattrs = WIN_CLOSEBOX | WIN_UP | WIN_DOWN |
		       WIN_PREV | WIN_NEXT | WIN_MOVE;
    int     i, day, nj, line;
    STATUS  *sta;
    IMAGE   *im;
    char    buf[20];

    DT_dmy(date, d);
    d[0] = 1;
    date = DT_dmy_date(d);


    /* DISPLAY FIX part of the SCREEN */
    sta = SCR_save_status();
    SCR_cursor_off();
again:
    im = SCR_save_image(bline, bcol, 10, 22);
    SCR_set_attr(SCR_REVERSE);
    SCR_fill(' ', bline, bcol, 10, 22);
    SCR_box(2, bline, bcol, 10, 22);
/*    SCR_write(bline + 9, bcol + 2, "Up/Dn/Left/Right", 16); */
    SCR_write(bline + 2, bcol + 1, DT_DAYS_NAMES, 14);   /* BP_M 25-10-95 */
    SCR_set_attr(SCR_WHITE);
    SCR_write(bline + 2, bcol + 16, DT_DAYS_NAMES + 15, 5); /* BP_M 25-10-95 */
    SCR_box_attrs(boxattrs, bline, bcol, 10, 22, SCR_CHELP1);

    while(1) {
	DT_dmy(date, d);
	if(d[2] < 0) {
	    d[2] = 0;
	    date = DT_dmy_date(d);
	    }
	/* DISPLAY SCREEN */
	SCR_set_attr(SCR_REVERSE);
	sprintf(buf, "%s %ld", DT_MONTHS_NAMES[d[1] - 1], 1900L + d[2]);
	SCR_write(bline + 1, bcol + 7, buf, (int)strlen(buf));
	day = DT_day_w(date);
	nj = DT_month_days(date);
	line = bline + 3;
	for(i = 0 ; i < day - 1; i++)
	    SCR_write(line, bcol + 1 + 3 * i, "  ", 2);

	for( ; i < 42 ; i++) {
	    if(i % 7 >= 5) SCR_set_attr(SCR_WHITE);
	    if(i % 7 == 0 && i > 0) {
		line++;
		SCR_set_attr(SCR_REVERSE);
		}
	    if(i + 2 - day <= nj) {
		sprintf(buf, "%2d", i + 2 - day );
		SCR_write(line, bcol + 1 + 3 * (i % 7), buf, 2);
		}
	    else
		SCR_write(line, bcol + 1 + 3 * (i % 7), "  ", 2);
	    }

	switch(SCR_get_key()) {
	    case SCR_CSR_PG_UP :
	    case SCR_CSR_UP    :   date = DT_add_months(date, 12); break;
	    case SCR_CSR_DOWN  :
	    case SCR_CSR_PG_DN :   date = DT_add_months(date, -12); break;
	    case SCR_CSR_LEFT  :   date = DT_add_months(date, -1); break;
	    case SCR_CSR_RIGHT :   date = DT_add_months(date, 1); break;
	    case SCR_ESCAPE    :   goto fin;
#ifdef MS_MOUSE
	    case SCR_MOUSE     :
		if(MS_B == 0) break;
		switch(SCR_box_edit(boxattrs, bline, bcol, 10, 22, SCR_CHELP2)) {
		    case WIN_CLOSEBOX : SCR_record_key(SCR_ESCAPE); break;
		    case WIN_UP       : SCR_record_key(SCR_CSR_UP); break;
		    case WIN_DOWN     : SCR_record_key(SCR_CSR_DOWN); break;
		    case WIN_PREV     : SCR_record_key(SCR_CSR_LEFT); break;
		    case WIN_NEXT     : SCR_record_key(SCR_CSR_RIGHT); break;
		    case WIN_MOVE     :
			SCR_box_move(&bline, &bcol, 10, 22, 1, 0,
			       SCR_PAGE_SIZE[0] - 2, SCR_PAGE_SIZE[1] - 2);
			SCR_display_image(im);
			SCR_free_image(im);
			goto again;
		    }
		break;

#endif
	    default :              if(SCR_app_key()) goto fin;
				   break;
	    }
	}

fin:
    SCR_display_image(im);
    SCR_free_image(im);
    SCR_reset_status(sta);
    return(0);
}



