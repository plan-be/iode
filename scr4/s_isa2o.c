#include "scr.h"

/* ======================================================================
Transforme tous les champs texte d'un ISAM d'Ansi vers Oem.

&RT -1 si le fichier ne peut être ouvert, 0 sinon.
&SA IS_o2a_rec()
------------------------------------------------------------------------- */
IS_a2o_rec(is)
ISAM    *is;
{
    long    number = 0;

    if(IS_open_files(is, 0L) != 0) {
	PG_display_error( "cannot open file '%s'\n", is->is_filename);
	return(-1);
    }

    IS_empty_rec(is);
    if(IS_search(is, 0, SCR_DB, (char *)0) != 0) {
	PG_display_error( "record not found in file '%s'\n", is->is_filename);
	return(-1);
    }
    while(1) {
	IS_a2o_rec_isam(is);
	if(number % 10 == 0)
	    PG_display_error("'%ld' records converted\r", number);
	number++;
	if(IS_next(is) != 0) break;
    }
    IS_close(is);
    PG_display_error( "'%d' record dumped to output\n", number);
    return(0);
}

/*NH*/
IS_a2o_rec_isam(is)
ISAM    *is;
{
    int     i;
    char    buf[2000], *buf2;
    ISFIELD *isf;

    for(i = 0, isf = is->is_flds ; i < is->is_nb_flds ; i++, isf++) {
	    switch(isf->isf_type) {
		case SCR_STRING:
		case SCR_ZSTRING:
		case SCR_CHAR:
		    SCR_AnsiToOemLg(is->is_rec + isf->isf_start,
			    is->is_rec + isf->isf_start, isf->isf_length);
		}
	    }

    if(IS_rewrite(is)) {
	    fprintf(stderr, "Rewrite : %s\n", IS_error());
	    return(-1);
	    }

    return(0);
}

/* ======================================================================
Transforme tous les champs texte d'un ISAM d'Oem vers Ansi

&RT -1 si le fichier ne peut être ouvert, 0 sinon.
&SA IS_o2a_rec()
------------------------------------------------------------------------- */
IS_o2a_rec(is)
ISAM    *is;
{
    long    number = 0;

    if(IS_open_files(is, 0L) != 0) {
	PG_display_error( "cannot open file '%s'\n", is->is_filename);
	return(-1);
    }

    IS_empty_rec(is);
    if(IS_search(is, 0, SCR_DB, (char *)0) != 0) {
	PG_display_error( "record not found in file '%s'\n", is->is_filename);
	return(-1);
    }
    while(1) {
	IS_o2a_rec_isam(is);
	if(number % 10 == 0)
	    PG_display_error("'%ld' records converted\r", number);
	number++;
	if(IS_next(is) != 0) break;
    }
    IS_close(is);
    PG_display_error( "'%d' records converted\n", number);
    return(0);
}


IS_o2a_rec_isam(is)
ISAM    *is;
{
    int     i;
    char    buf[2000], *buf2;
    ISFIELD *isf;

    for(i = 0, isf = is->is_flds ; i < is->is_nb_flds ; i++, isf++) {
	    switch(isf->isf_type) {
		case SCR_STRING:
		case SCR_ZSTRING:
		case SCR_CHAR:
		    SCR_OemToAnsiLg(is->is_rec + isf->isf_start,
			    is->is_rec + isf->isf_start, isf->isf_length);
		}
	    }

    if(IS_rewrite(is)) {
	    fprintf(stderr, "Rewrite : %s\n", IS_error());
	    return(-1);
	    }

    return(0);
}


