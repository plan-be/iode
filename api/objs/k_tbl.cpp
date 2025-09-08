/**
 *  @header4iode
 *
 *  Functions to manage TBL objects.
 *  
 *  List of functions 
 *  -----------------
 *      TBL *T_create(int dim)                                                                     | Creates a new TBL object.
 *      void T_free(TBL* tbl)                                                                      | Frees a TBL object
 *      void T_free_line(TLINE* line, int dim)                                                     | Frees a TLINE struct and all its TCELL.
 *      void T_free_cell(TCELL* cell)                                                              | Frees a TCELL struct.
 *      int T_add_line(TBL* tbl)                                                                   | Extents a TBL by adding at least one line.
 *      TCELL *T_create_cell(TBL* tbl, TLINE* line)                                                | Initialises a TLINE of the type TABLE_LINE_CELL. 
 *      TCELL *T_create_title(TBL* tbl, TLINE* line)                                               | Initialises a TLINE of the type TABLE_LINE_TITLE. 
 *      char* T_cell_cont(TCELL* cell, int mode)                                                   | Returns the formated contents of a TCELL.
 *      char* T_cell_cont_tbl(TBL* tbl, int row, int col, int mode)                                | Returns the formated contents of a TCELL.
 *      char* T_div_cont_tbl(TBL* tbl, int col, int mode)                                          | Returns the formated contents of TBL divisor column.
 *      int T_insert_line(TBL* tbl, int nbr, int type, int where)                                  | Inserts a TLINE in a TBL.
 *      int T_set_lec_cell(TCELL* cell, unsigned char* lec)                                        | Assigns a LEC expression to a TCELL. Checks the syntax.
 *      int T_set_lec_cell_tbl(TBL* tbl, int row, int col, unsigned char* lec)                     | Assigns a LEC expression to a TCELL. Checks the syntax.
 *      void T_set_string_cell(TCELL* cell, unsigned char* txt)                                    | Assigns a TEXT to a TCELL.
 *      void T_set_string_cell_tbl(TBL* tbl, int row, int col, unsigned char* txt)                 | Assigns a TEXT to a TCELL.
 *      void T_set_cell_attr(TBL* tbl, int i, int j, int attr)                                     | Assigns justification (TABLE_CELL_CENTER...) and typographic (TABLE_CELL_BOLD...) attributes to a TCELL.
 *      int T_default(TBL* tbl, char*titg, char**titls, char**lecs, int mode, int files, int date) | Fills a TBL with some basic data: a title, line titles and LEC expressions.
 *      void T_auto(TBL* tbl, char* def, char** vars, int mode, int files, int date)               | Fills a TBL with a list of variables and their CMT. 
 *  
 */
#include "scr4/s_prost.h"

#include "api/constants.h"
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/tables.h"
#include "api/utils/buf.h"


/**
 *  Creates a new TBL object.
 *  
 *  @param [in]  dim    int     Number of expression columns
 *  @return             TBL*    pointer to a new allocated TBL
 */
TBL *T_create(int dim)
{
    TBL     *tbl = NULL;
    TCELL   *cell;
    int     i;


    tbl = (TBL *)SW_nalloc(sizeof(TBL));
    if(tbl == NULL) {
        kerror(0, "Allocation error");
        return(NULL);

    }
    T_NC(tbl)   = dim;
    T_LANG(tbl) = TABLE_ENGLISH;

    tbl->t_zmin = (float)IODE_NAN;
    tbl->t_zmax = (float)IODE_NAN;
    tbl->t_ymin = (float)IODE_NAN;
    tbl->t_ymax = (float)IODE_NAN;

    tbl->t_div.tl_type = TABLE_LINE_CELL;
    tbl->t_div.tl_val  = SW_nalloc(dim * sizeof(TCELL));
    cell               = (TCELL *) tbl->t_div.tl_val;

    for(i = 0; i < dim; i++) {
        cell[i].type = TABLE_CELL_LEC;
        cell[i].attribute = TABLE_CELL_LEFT;
    }
    K_ipack(&(cell[0].content), "1");
    /* rest is repetitive if val[i] == 0, val[i] = val[i-1] */

    return(tbl);
}


/**
 *  Frees a TBL object.
 *  
 *  @param [in] tbl     TBL*    pointer to the TBL
 *  
 */
void T_free(TBL* tbl)
{
    int     i;

    T_free_line(&(tbl->t_div), T_NC(tbl));
    for(i = 0; i < T_NL(tbl); i++) T_free_line(tbl->t_line + i, T_NC(tbl));

    SW_nfree(tbl->t_line);
    SW_nfree(tbl);
}


/**
 *  Frees a TLINE struct and all its TCELL.
 *  
 *  @param [in, out]    line    TLINE*     pointer to the table line to delete
 *  @param [in]         dim     int        Number columns in the TBL which contains the TLINE.
 *  @return             void
 *
 */
void T_free_line(TLINE* line, int dim)
{
    int     i;
    TCELL   *cell;


    cell = (TCELL *) line->tl_val;
    if(line->tl_type == TABLE_LINE_CELL)
        for(i = 0; i < dim; i++) T_free_cell(cell + i);

    if(line->tl_type == TABLE_LINE_TITLE) T_free_cell(cell);


    SW_nfree(line->tl_val);
}


/**
 *  Frees a TCELL struct.
 *  
 *  @param [in, out]    cell    TCELL*      TCELL to delete
 *  @return             void
 *  
 */
void T_free_cell(TCELL* cell)
{
    SW_nfree(cell->content);
    cell->content = NULL;
}


/**
 *  Extents a table by adding KT_CHUNCK lines if the current number of lines is a multiple of KT_CHUNCK.
 *  
 *  @param  [in] tbl    TBL*    TBL to be extended
 *  @return             int     0 if ok, -1 if allocation is impossible
 */
int T_add_line(TBL* tbl)
{
    int nl = T_NL(tbl);

    if(nl % KT_CHUNCK == 0)
        T_L(tbl) = (TLINE *) SW_nrealloc(T_L(tbl),
                                        sizeof(TLINE) * nl, sizeof(TLINE) * (nl + KT_CHUNCK));
    if(T_L(tbl) == 0) return(-1);

    tbl->t_nl++;
    return(0);
}


/**
 *  Initialises a TLINE of the type TABLE_LINE_CELL. 
 *  
 *  All cells are "decimal" justified except the first one which is left justified.
 *  
 *  @param [in] tbl     TBL*    table to which line belongs
 *  @param [in] line    TLINE*  line to initialise
 *  @return             TCELL*  pointer to the first TCELL of TLINE
 *
 */
TCELL   *T_create_cell(TBL* tbl, TLINE* line)
{
    int     i, nc = T_NC(tbl);

    line->tl_type = TABLE_LINE_CELL;
    line->tl_val = SW_nalloc(nc * sizeof(TCELL));
    line->tl_graph = T_GRAPHDEFAULT; /* GB 10/03/2011 */
    for(i = 0; i < nc; i++) {
        ((TCELL *) line->tl_val + i)->type = TABLE_CELL_LEC;
        ((TCELL *) line->tl_val + i)->attribute = (i > 0) ? TABLE_CELL_DECIMAL : TABLE_CELL_LEFT;
        ((TCELL *) line->tl_val + i)->content = NULL; 
    }

    return((TCELL *) line->tl_val);
}


/**
 *  Initialises a TLINE of the type TABLE_LINE_TITLE. 
 *  
 *  The first (and only) cell receives the attributes TABLE_CELL_CENTER and TABLE_CELL_BOLD.
 *  
 *  @param [in] tbl     TBL*    table to which line belongs
 *  @param [in] line    TLINE*  line to initialise
 *  @return             TCELL*  pointer to the first TCELL of TLINE
 *
 */
TCELL *T_create_title(TBL* tbl, TLINE* line)
{
    line->tl_type = TABLE_LINE_TITLE;
    line->tl_val = SW_nalloc(sizeof(TCELL));
    ((TCELL *) line->tl_val)->attribute = TABLE_CELL_CENTER + TABLE_CELL_BOLD; /* JMP 11-11-93 */

    return((TCELL *) line->tl_val);
}


/**
 *  Returns the formated contents of a TCELL: 
 *      - lec expression or (A+B)
 *      - text possibly between double quotes (if mode == 1 => "Line title:", if not => Line title)
 *  
 *  The returned value is the global allocated buffer BUF_DATA (see buf.c) or a pointer to a static memory "".
 *  
 *  mode is set to 1 only for the TBL editor where the CELL type is deduced from the first character (" => text).
 *  
 *  @param [in] cell    TCELL*  cell to read
 *  @param [in] mode    int     1 if the text (not the LEC) must be enclosed between ""
 *                              0 if not 
 *  @return             char*   pointer to BUF_DATA (big buffer - see buf.c) -- Do NOT free!
 */
char* T_cell_cont(TCELL* cell, int mode)
{
    char    *buf;

    if(cell->content == NULL) return(""); /* JMP 20-11-93 */
    if(cell->type == TABLE_CELL_LEC)
        return(BUF_strcpy((char*) P_get_ptr(cell->content, 0)));
    buf = BUF_alloc((int)strlen(cell->content) + 3);
    if(mode) sprintf(buf, "\"%s\"", cell->content);
    else BUF_strcpy(cell->content);
    return(BUF_DATA);
}


/**
 *  Returns the formated contents of a TCELL:
 *      - lec expression or (A+B)
 *      - text possibly between double quotes (if mode == 1 => "Line title:", if not => Line title)
 *
 *  mode is set to 1 only for the TBL editor where the CELL type is deduced from the first character (" => text).
 *
 *  @param [in] tbl     TBL*    pointer to the table
 *  @param [in] row     int     position of the line
 *  @param [in] col     int     position of the cell
 *  @param [in] mode    int     1 if the text (not the LEC) must be enclosed between ""
 *                              0 if not
 *  @return             char*   pointer to BUF_DATA (big buffer - see buf.c) -- Do NOT free!
 */
char* T_cell_cont_tbl(TBL* tbl, int row, int col, int mode)
{
    TLINE line = tbl->t_line[row];
    TCELL* cell = (TCELL*) line.tl_val;
    switch (line.tl_type)
    {
        case TABLE_LINE_TITLE:
            return(cell->content);
            break;
        case TABLE_LINE_CELL:
            return(T_cell_cont(cell + col, mode));
            break;
        default:
            return(NULL);
    }
}


/**
 *  Returns the formated contents of a TBL divisor.
 *
 *  mode is set to 1 only for the TBL editor where the CELL type is deduced from the first character (" => text).
 *
 *  @param [in] tbl     TBL*    pointer to the table
 *  @param [in] col     int     position of the cell in the divisor
 *  @param [in] mode    int     1 if the text (not the LEC) must be enclosed between ""
 *                              0 if not
 *  @return             char*   pointer to BUF_DATA (big buffer - see buf.c) -- Do NOT free!
 */

char* T_div_cont_tbl(TBL* tbl, int col, int mode)
{
    TCELL* cell = (TCELL*)tbl->t_div.tl_val;;
    return(T_cell_cont(cell + col, mode));
}


/**
 *  Inserts a TLINE in a TBL.
 *  
 *  @param [in, out] tbl     TBL*    TBL where a new line must be inserted
 *  @param [in]      nbr     int     reference position of the new line in TBL (see param where below)
 *  @param [in]      type    int     TLINE type (TABLE_LINE_CELL, TABLE_LINE_TITLE...)
 *  @param [in]      where   int     0 to insert before line nbr, 1 to insert after line nbr
 *  @return                  int     position of the new line in TBL
 *  **TODO: Check where definition 
 */
int T_insert_line(TBL* tbl, int nbr, int type, int where)
{
    TLINE   *oline, *nline;

    if(type < 0) return(nbr);
    oline = tbl->t_line;
    nline = (TLINE *)
            SW_nalloc(((T_NL(tbl) + 1)/KT_CHUNCK + 1) * KT_CHUNCK * sizeof(TLINE));

    if(nbr < 0) {
        where = 0;
        nbr = -1;
    }
    if(where == 0) nbr += 1;
    if(T_NL(tbl) > 0) {
        if(nbr > 0)
            memcpy(nline, oline, nbr * sizeof(TLINE));
        if(T_NL(tbl) > nbr)
            memcpy(nline + nbr + 1, oline + nbr, (T_NL(tbl) - nbr) * sizeof(TLINE));
    }
    else nbr = 0;

    if(type == TABLE_LINE_CELL)  T_create_cell(tbl, nline + nbr);
    if(type == TABLE_LINE_TITLE) T_create_title(tbl, nline + nbr);

    nline[nbr].tl_type = type;
    tbl->t_line = nline;
    T_NL(tbl)++;
    SW_nfree(oline);
    return(nbr);
}


/**
 *  Assigns a LEC expression to a TCELL. Checks the syntax.
 *  
 *  @param [in, out] cell   TCELL*            Cell to modify
 *  @param [in]      lec    unsigned char*    LEC expression
 *  @return                 int               0 if ok, -1 if syntax error in LEC  
 *  
 *  In case of LEC error, kerror() is called and L_errno is set.
 */
int T_set_lec_cell(TCELL* cell, unsigned char* lec)
{
    unsigned char   *ptr = 0;

    cell->type = TABLE_CELL_LEC;
    cell->attribute = TABLE_CELL_ALIGN(cell->attribute, TABLE_CELL_DECIMAL);
    if(K_ipack((char**) &ptr, (char*) lec) < 0 && L_errno) {
        kerror(0, "Illegal lec-formula");
        return(-1);
    }
    else {
        T_free_cell(cell);
        cell->content = (char*) ptr;
    }
    return(0);
}


/**
 *  Assigns a LEC expression to a TCELL. Checks the syntax.
 *
 *  @param [in] tbl     TBL*              pointer to the table
 *  @param [in] row     int               position of the line
 *  @param [in] col     int               position of the cell
 *  @param [in] lec     unsigned char*    LEC expression
 *  @return             int               0 if ok, -1 if syntax error in LEC
 *
 *  In case of LEC error, kerror() is called and L_errno is set.
 */
int T_set_lec_cel_tbl(TBL* tbl, int row, int col, unsigned char* lec)
{
    TLINE line = tbl->t_line[row];
    TCELL* cell = (TCELL*) line.tl_val;
    return(T_set_lec_cell(cell + col, lec));
}


/**
 *  Assigns a TEXT to a TCELL. The alignment attributes are set to:
 *      - TABLE_CELL_LEFT if previously TABLE_CELL_DECIMAL
 *      - TABLE_CELL_CENTER if the txt contains the char '#' indicating a time period (col title).
 *  
 *  @param [in, out] cell   TCELL*            Cell to modify
 *  @param [in]      lec    unsigned char*    Any text
 *  @return                 void
 *  
 *  In case of LEC error, kerror() is called and L_errno is set.
 */
void T_set_string_cell(TCELL* cell, unsigned char* txt)
{
    int     len, attr;

    cell->type = TABLE_CELL_STRING;
    /*    cell->attribute |= TABLE_CELL_LEFT; /* JMP 11-11-93 */
    attr = cell->attribute;
    if(attr & TABLE_CELL_DECIMAL) attr = TABLE_CELL_ALIGN(attr, TABLE_CELL_LEFT);  /* JMP 19-11-93 */
    if(U_is_in('#', (char*) txt)) attr = TABLE_CELL_ALIGN(attr, TABLE_CELL_CENTER);  /* JMP 19-11-93 */
    cell->attribute = attr;
    len = (int) strlen((char*) txt);
    if (len > 0) {
        if (txt[0] == '\"') {
            txt++;
            len--;
        }
        if (len > 0 && txt[len - 1] == '\"') txt[len - 1] = 0;
    }
    T_free_cell(cell);
    cell->content = (char*) SCR_stracpy(txt);
}

/**
 *  Assigns a TEXT to a TCELL. The alignment attributes are set to:
 *      - TABLE_CELL_LEFT if previously TABLE_CELL_DECIMAL
 *      - TABLE_CELL_CENTER if the txt contains the char '#' indicating a time period (col title).
 *
 *  @param [in] tbl     TBL*              pointer to the table
 *  @param [in] row     int               position of the line
 *  @param [in] col     int               position of the cell
 *  @param [in] lec     unsigned char*    Any text
 *  @return             void
 *
 *  In case of LEC error, kerror() is called and L_errno is set.
 */
void T_set_string_cell_tbl(TBL* tbl, int row, int col, unsigned char* txt)
{
    TLINE line = tbl->t_line[row];
    TCELL* cell = (TCELL*) line.tl_val;
    T_set_string_cell(cell + col, txt);
}

/*
T_get_cell_attr(tbl, i, j)
TBL *tbl;
int i, j;
{
    TLINE   *line = tbl->t_line + i;
    TCELL   *cell;

    switch(line->tl_type) {
	case TABLE_LINE_TITLE : if(j > 0) return(TABLE_CELL_BOLD & TABLE_CELL_CENTER);
	case TABLE_LINE_CELL  : break;
	default :       return(TABLE_CELL_BOLD & TABLE_CELL_LEFT);
	}
    cell = (TCELL *)line->tl_val + j;
    return(cell->attribute);
}
/* JMP 11-11-93 */


/**
 *  Assigns justification (TABLE_CELL_CENTER...) and typographic (TABLE_CELL_BOLD...) attributes to a TCELL.
 *  
 *  @param [in, out]    tbl     TBL*    table to modify
 *  @param [in]         i       int     line 
 *  @param [in]         j       int     column 
 *  @param [in]         attr    int     combination of attributes (TABLE_CELL_CENTER & TABLE_CELL_BOLD...) 
 *  @return                     void
 *  
 */
void T_set_cell_attr(TBL* tbl, int i, int j, int attr) /* JMP 11-11-93 */
{
    TLINE   *line = tbl->t_line + i;
    TCELL   *cell;

    switch(line->tl_type) {
        case TABLE_LINE_TITLE :
            if(j > 0) return;
        case TABLE_LINE_CELL  :
            break;
        default :
            return;
    }
    cell = (TCELL *)line->tl_val + j;
    cell->attribute = attr;
}


/**
 *  Fills a TBL with some basic data: a title, line titles and LEC expressions.
 *  The TBL must exist and is normally empty.
 *  
 *  @param [in, out] tbl   TBL*     Table to modify
 *  @param [in]      titg  char*    Title of the table
 *  @param [in]      titls char**   Titles of the lines
 *  @param [in]      lecs  char**   LEC formulas of the lines (// titls)
 *  @param [in]      mode  int      if 1, includes a special TABLE_LINE_MODE line
 *  @param [in]      files int      if 1, includes a special TABLE_LINE_FILES line
 *  @param [in]      date  int      if 1, includes a special TABLE_LINE_DATE line
 *  @return                int      0
 *  
 */
int T_default(TBL* tbl, char*titg, char**titls, char**lecs, int mode, int files, int date)
{
    int     i, j;

    if(titg) {
        T_insert_line(tbl, T_NL(tbl) - 1, TABLE_LINE_TITLE, 0);
        SCR_strip((unsigned char*) titg);
        if(titg[0])
            T_set_string_cell((TCELL *)(tbl->t_line[T_NL(tbl) - 1].tl_val), (unsigned char*) titg);
        T_insert_line(tbl, T_NL(tbl) - 1, TABLE_LINE_SEP, 0);
    }
    T_insert_line(tbl, T_NL(tbl) - 1,  TABLE_LINE_CELL, 0);
    for(j = 1 ; j < T_NC(tbl) ; j++) {
        T_set_string_cell((TCELL *)(tbl->t_line[T_NL(tbl) - 1].tl_val) + j, (unsigned char*) "\"#S");  /* JMP 24-03-2004 */
        T_set_cell_attr(tbl, T_NL(tbl) - 1, j, TABLE_CELL_CENTER); /* JMP 11-11-93 */
    }
    T_insert_line(tbl, T_NL(tbl) - 1, TABLE_LINE_SEP, 0);

    if(lecs && titls) {
        for(i = 0 ; lecs[i] && titls[i]; i++) {
            T_insert_line(tbl, T_NL(tbl) - 1,  TABLE_LINE_CELL, 0);
            T_set_string_cell((TCELL *)(tbl->t_line[T_NL(tbl) - 1].tl_val), (unsigned char*) titls[i]);
            for(j = 1 ; j < T_NC(tbl) ; j++)
                T_set_lec_cell((TCELL *)(tbl->t_line[T_NL(tbl) - 1].tl_val) + j, (unsigned char*) lecs[i]);
        }
    }
    else T_insert_line(tbl, T_NL(tbl) - 1, TABLE_LINE_CELL, 0);
    if(mode || files || date)
        T_insert_line(tbl, T_NL(tbl) - 1,  TABLE_LINE_SEP, 0);
    if(mode)  T_insert_line(tbl, T_NL(tbl) - 1,  TABLE_LINE_MODE, 0);
    if(files) T_insert_line(tbl, T_NL(tbl) - 1,  TABLE_LINE_FILES, 0);
    if(date)  T_insert_line(tbl, T_NL(tbl) - 1,  TABLE_LINE_DATE, 0);

    return(0);
}


/**
 *  Fills a TBL with a list of variables and their CMT. 
 *  The TBL must exist and is normally empty.
 *  
 *    - def can be either a CMT name or a free text. If it's a CMT name, the contents of the CMT becomes the table title.
 *    - vars is a list of VAR names. If a CMT with the same name exists, the CMT become the line title and the variable the LEC expression.
 *  
 *  @param [in, out] tbl   TBL*     Table to modify
 *  @param [in]      def   char*    Table title or name of the CMT that must become the table title
 *  @param [in]      vars  char**   NULL terminated list of variable names
 *  @param [in]      mode  int      if 1, includes a special TABLE_LINE_MODE line
 *  @param [in]      files int      if 1, includes a special TABLE_LINE_FILES line
 *  @param [in]      date  int      if 1, includes a special TABLE_LINE_DATE line
 *  @return                int      0
 *  
 */
void T_auto(TBL* tbl, char* def, char** vars, int mode, int files, int date)
{
    int     i, pos,
            nb = SCR_tbl_size((unsigned char**) vars),
            nbt = 0, nbl = 0;
    KDB     *kdb = K_WS[COMMENTS];
    char    *titg, **titls = NULL, **lecs = NULL;

    pos = K_find(kdb, def);
    if(pos < 0) titg = (char*) SCR_stracpy((unsigned char*) def);
    else titg = (char*) SCR_stracpy((unsigned char*) KCVAL(kdb, pos));
    for(i = 0; i < nb; i++) {
        pos = K_find(kdb, vars[i]);
        if(pos < 0)
            SCR_add_ptr((unsigned char***) &titls, &nbt, (unsigned char*) vars[i]);
        else
            SCR_add_ptr((unsigned char***) &titls, &nbt, (unsigned char*) KCVAL(kdb, pos));

        SCR_add_ptr((unsigned char***) &lecs, &nbl, (unsigned char*) vars[i]);
    }

    SCR_add_ptr((unsigned char***) &titls, &nbt, NULL);
    SCR_add_ptr((unsigned char***) &lecs, &nbl, NULL);

    T_default(tbl, titg, titls, lecs, mode, files, date);

    SCR_free(titg);
    SCR_free_tbl((unsigned char**) titls);
    SCR_free_tbl((unsigned char**) lecs);
}
