#include "s_ydvlp.h"

int     YY_OLD_DEF = 0;

int YY_undef(yy)
YYFILE  *yy;
{
    char    name[YY_MAX_DEF_LG + 1];

    YY_skip_spaces(yy);
    YY_read_word(yy);
    if(yy->yy_lg == 0) return(-1);
    return(YY_unsave_define(yy, name));
}

/* ===+=== YY_UNSAVE_DEFINE ==================================
    Suppress a dynamic definition
	yy : IN :
	name : IN : define name
   ==================================================== */
int YY_unsave_define(yy, name)
YYFILE  *yy;
char    *name;
{
    return(0);
}
/**/
int YY_define(yy)
YYFILE  *yy;
{
    char    name[YY_MAX_DEF_LG + 1];
    int     c;
    char    buf[2];

    YY_skip_spaces(yy);
    YY_read_word(yy);
    if(yy->yy_lg == 0) return(-1);
    memcpy(name, yy->yy_text, YY_MAX_DEF_LG);
    name[YY_MAX_DEF_LG] = 0;
    yy->yy_text[0] = 0;
    while(1) {
	c = YY_getc(yy);
	if(c == '\n' || c == YY_EOF) break;
	if(c == ' '  || c == '\t') continue;
	else {
	    buf[0] = c;
	    buf[1] = 0;
	    YY_record(yy, buf);
	    YY_read_to_char(yy, '\n');
	    break;
	}
    }
    return(YY_save_define(yy, name, yy->yy_text));
}

/**/
YY_ifdef(yy, flag)
YYFILE  *yy;
int     flag;
{
    char    name[YY_MAX_DEF_LG + 1];
    YYDEF   *yd;

    NBIF++;
    YY_skip_spaces(yy);
    YY_read_word(yy);
    if(yy->yy_lg == 0) return(-1);
    memcpy(name, yy->yy_text, YY_MAX_DEF_LG);
    name[YY_MAX_DEF_LG] = 0;
    yd = YY_search_def(DDEFS, NBDDEFS, name);
    if(flag == 1) {
	if(yd == 0)
	    if(YY_find_end_else(yy) != 0) return(-1);
    }
    else {
	if(yd != 0)
	    if(YY_find_end_else(yy) != 0) return(-1);
    }
    return(0);
}

YY_find_end_else(yy)
YYFILE  *yy;
{
    int     nbif = NBIF - 1;

    while(1) {
	YY_skip_to_char(yy, '#');
	YY_read_word(yy);
	if(yy->yy_lg == 0) return(-1);
	if(YY_OLD_DEF) {                                /* JMP_M 4.20 26-06-95 */
	    if(strcmp((char *)yy->yy_text, "endif") == 0) {     /* JMP_M 4.20 26-06-95 */
		NBIF--;                                 /* JMP_M 4.20 26-06-95 */
		return(0);                              /* JMP_M 4.20 26-06-95 */
		}                                       /* JMP_M 4.20 26-06-95 */
	    if(strcmp((char *)yy->yy_text, "else") == 0)        /* JMP_M 4.20 26-06-95 */
		return(0);                              /* JMP_M 4.20 26-06-95 */
	    }                                           /* JMP_M 4.20 26-06-95 */
	else {
	    if(strcmp((char *)yy->yy_text, "endif") == 0) {
		NBIF--;
		if(nbif == NBIF) return(0);
		}
	    if(strcmp((char *)yy->yy_text, "else") == 0 && NBIF == nbif + 1) return(0);
	    if(strcmp((char *)yy->yy_text, "ifdef") == 0) NBIF++;
	    if(strcmp((char *)yy->yy_text, "ifndef") == 0) NBIF++;
	    }
    }
}

#ifndef GENPROTO

int YY_defcmp(const void* pyd1, const void* pyd2)
{
    YYDEF   **yd1 = (YYDEF **)pyd1,
            **yd2 = (YYDEF **)pyd2;

    return(strcmp((char *)(*yd1)->yd_name, (char *)(*yd2)->yd_name));
}

int YY_defcmp1(const void* ptr, const void* pyd2)
{
    YYDEF   **yd2 = (YYDEF **)pyd2;

    return(strcmp(ptr, (char *)(*yd2)->yd_name));
}

#endif /* GENPROTO */

/* ===+=== YY_SAVE_DEFINE ==================================
    Add a dynamic definition
	yy : IN :
	name : IN : define name
	def  : text of define
   ==================================================== */

YY_save_define(yy, name, def)
YYFILE  *yy;
char    *name,
	*def;
{
    YYDEF   **yd, *yd_new;
    int     lg;

    yd_new = YY_search_def(DDEFS, NBDDEFS, name);
    if(yd_new != 0) {
	//if(yd_new->yd_def != 0) free(yd_new->yd_def);
	if(yd_new->yd_def != 0) SCR_free(yd_new->yd_def); /* JMP 22-08-2012 */
	}
    else {
	if(NBDDEFS % 20 == 0) {
	    //if(NBDDEFS > 0) yd = (YYDEF **) realloc(DDEFS, sizeof(YYDEF *) * (NBDDEFS + 20));
	    //else            yd = (YYDEF **) malloc(sizeof(YYDEF *) * 20);
	    if(NBDDEFS > 0) yd = (YYDEF **) SCR_realloc(DDEFS, sizeof(YYDEF *), NBDDEFS, (NBDDEFS + 20)); /* JMP 22-08-2012 */
	    else            yd = (YYDEF **) SCR_malloc(sizeof(YYDEF *) * 20); /* JMP 22-08-2012 */

	    if(yd == 0) {
		fprintf(stderr, "\nYY_define() : Memory full.\n");
		return(-1);
		}
	    DDEFS = yd;
	    }
	//DDEFS[NBDDEFS] = yd_new = (YYDEF *) malloc(sizeof(YYDEF));
	DDEFS[NBDDEFS] = yd_new = (YYDEF *) SCR_malloc(sizeof(YYDEF)); /* JMP 22-08-2012 */
	NBDDEFS++;
	}

    lg = (int)strlen(name);
    if(lg > YY_MAX_DEF_LG) lg = YY_MAX_DEF_LG;
    memcpy(yd_new->yd_name, name, lg);
    yd_new->yd_name[lg] = 0;

    lg = (!*def) ? 0:(int)strlen(def);
    //yd_new->yd_def = (unsigned char *)malloc(lg + 1);
    yd_new->yd_def = (unsigned char *)SCR_malloc(lg + 1); /* JMP 22-08-2012 */
    memcpy(yd_new->yd_def, def, lg + 1);

    qsort(DDEFS, NBDDEFS, sizeof(YYDEF *), YY_defcmp);
    return(0);
}
/**/
/* =+====== YY_SEARCH_DEF =======================================
     Find a definition in a YYDEF * table
     If found, record pos
     yy : IN : YYFILE pointer
     yd : IN : YYDEF **
     nb : IN : nb YYDEF
     txt: IN : text to be found
   ============================================================== */

YYDEF *YY_search_def(YYDEF   **tbl, int nb, char *txt)
{
    YYDEF   **yd;

    if(nb == 0) return((YYDEF *)0);
    yd = (YYDEF **) bsearch(txt, tbl, nb, sizeof(YYDEF *), YY_defcmp1);
    return((yd == 0) ? (YYDEF *)0 : *yd);
}
/**/
/* =+====== YY_FIND_DEF =========================================
     Record a text following a $ and try to find definition
     If found, record definition
     yy : IN : YYFILE pointer
   ============================================================== */

YY_find_def(yy)
YYFILE  *yy;
{
    YYDEF   *yd;

    YY_read_word(yy);
    if(yy->yy_lg == 0) return(-1);

    yd = YY_search_def(DDEFS, NBDDEFS, (char *)yy->yy_text);
    if(yd == 0)
	yd = YY_search_def(SDEFS, NBSDEFS, (char *)yy->yy_text);
    if(yd != 0) {
	if(yd->yd_def != 0)
	    YY_record(yy, yd->yd_def);
	return(0);
	}
    YY_record(yy, yy->yy_text);
    return(-1);
}
