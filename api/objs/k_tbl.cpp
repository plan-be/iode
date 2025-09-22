/**
 *  @header4iode
 *
 *  Functions to manage TBL objects.
 *  
 *  List of functions 
 *  -----------------
 *      TBL *T_create(int dim)                                                                     | Creates a new TBL object.
 *      void T_free(TBL* tbl)                                                                      | Frees a TBL object
 *      char* T_cell_cont(TCELL* cell, int mode)                                                   | Returns the formated contents of a TCELL.
 *      char* T_cell_cont_tbl(TBL* tbl, int row, int col, int mode)                                | Returns the formated contents of a TCELL.
 *      char* T_div_cont_tbl(TBL* tbl, int col, int mode)                                          | Returns the formated contents of TBL divisor column.
 *      int T_append_line(TBL* tbl, int type)                                                      | Appends a TLINE to a TBL.
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
    if(dim < 1) 
    {
        kwarning("Table: could not create new table -> number of columns < 1");
        return NULL;
    }
    
    TBL* tbl = new TBL(dim);
    return tbl;
}


/**
 *  Frees a TBL object.
 *  
 *  @param [in] tbl     TBL*    pointer to the TBL
 *  
 */
void T_free(TBL* tbl)
{
    if(tbl == NULL) 
        return;
    
    tbl->divider_line.cells.clear();
    if(tbl->lines.size() > 0)
    {
        for(int i = 0; i < tbl->lines.size(); i++)
            tbl->lines[i].cells.clear();
        tbl->lines.clear();
    }

    delete tbl;
}


/**
 *  Returns the formatted contents of a TCELL: 
 *      - lec expression or (A+B)
 *      - text possibly between double quotes (if mode == 1 => "Line title:", if not => Line title)
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
    if(cell->type == TABLE_CELL_LEC)
    {
        if(!cell->idt) 
            return "";
        char* c_lec = (char*) cell->idt->lec.c_str();
        return c_lec;
    }
    else
    {
        if(cell->content.empty()) 
            return "";
        char* buf = SW_nalloc((int) cell->content.size() + 3);
        char* text = (char*) cell->content.c_str();
        if(mode) 
            sprintf(buf, "\"%s\"", text);
        else 
            sprintf(buf, "%s", text);
        return buf;
    }
}


/**
 *  Returns the formatted contents of a TCELL:
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
    TLINE line = tbl->lines[row];
    switch (line.type)
    {
        case TABLE_LINE_TITLE:
            return((char*) line.cells[0].content.c_str());
            break;
        case TABLE_LINE_CELL:
            return(T_cell_cont(&line.cells[col], mode));
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
    char* content = T_cell_cont(&(tbl->divider_line.cells[col]), mode);
    return content;
}


static bool T_initialize_line(TLINE& line, const int nb_columns)
{
    bool success = true;
    switch(line.type) 
    {
        case TABLE_LINE_CELL:
            line.graph_type = T_GRAPHDEFAULT;
            for(int i = 0; i < nb_columns; i++)
                line.cells.push_back(TCELL(TABLE_CELL_LEC, i));
            break;
        case TABLE_LINE_TITLE:
            line.cells.push_back(TCELL(TABLE_CELL_STRING));
            line.cells.back().attribute = TABLE_CELL_CENTER + TABLE_CELL_BOLD;
            break;
        case TABLE_LINE_SEP:
        case TABLE_LINE_FILES:
        case TABLE_LINE_MODE:
        case TABLE_LINE_DATE:
            break;
        default:
            kwarning("Table: could not initialize new line -> unknown line type %d", line.type);
            success = false;
    }

    return success;
}


/**
 *  Inserts a TLINE in a TBL.
 *  
 *  @param [in, out] tbl     TBL*    TBL where a new line must be inserted
 *  @param [in]      type    int     TLINE type (TABLE_LINE_CELL, TABLE_LINE_TITLE...)
 *  @return                  int     position of the new line in TBL
 *  **TODO: Check where definition 
 */
int T_append_line(TBL* tbl, int type)
{
    TLINE line((char) type);
    bool success = T_initialize_line(line, tbl->nb_columns);
    if(!success) 
        return -1;

    tbl->lines.push_back(line);
    return tbl->lines.size() - 1;
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
    if(nbr < 0 || nbr > tbl->lines.size())
    {
        std::string error_msg = "Table: failed to insert a line in the table at position " 
                                + std::to_string(nbr) + ": position out of range [0," 
                                + std::to_string(tbl->lines.size() - 1) + "]";
        kerror(0, (char*) error_msg.c_str());
    }

    TLINE line((char) type);
    bool success = T_initialize_line(line, tbl->nb_columns);
    if(!success) 
        return -1;

    int insert_pos = (where == 0) ? nbr : nbr + 1;
    tbl->lines.insert(tbl->lines.begin() + insert_pos, line);
    return insert_pos;
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
int T_set_lec_cell(TCELL* cell, unsigned char* u_lec)
{
    unsigned char* ptr = NULL;

    cell->type = TABLE_CELL_LEC;
    cell->attribute = TABLE_CELL_ALIGN(cell->attribute, TABLE_CELL_DECIMAL);
    cell->content = "";
    if(cell->idt)
        delete cell->idt;
    cell->idt = nullptr;

    if(u_lec == NULL || strlen((char*) u_lec) == 0) 
        return(0);

    char* lec_copy = (char*) SCR_stracpy(u_lec);
    if(K_ipack((char**) &ptr, lec_copy) < 0 && L_errno) 
    {
        kerror(0, "Illegal lec-formula: %s", (char*) u_lec);
        return(-1);
    }
    std::string lec((char*) u_lec);
    cell->idt = new Identity(lec);
    
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
    TLINE line = tbl->lines[row];
    return(T_set_lec_cell(&(line.cells[col]), lec));
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

    attr = cell->attribute;
    if(attr & TABLE_CELL_DECIMAL) 
        attr = TABLE_CELL_ALIGN(attr, TABLE_CELL_LEFT);
    if(U_is_in('#', (char*) txt)) 
        attr = TABLE_CELL_ALIGN(attr, TABLE_CELL_CENTER);
    cell->attribute = attr;

    len = (int) strlen((char*) txt);
    if (len > 0) 
    {
        if (txt[0] == '\"') 
        {
            txt++;
            len--;
        }

        if (len > 0 && txt[len - 1] == '\"') 
            txt[len - 1] = 0;
    }

    cell->content = std::string((char*) txt);
    if(cell->idt)
        delete cell->idt;
    cell->idt = nullptr;
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
    TLINE line = tbl->lines[row];
    T_set_string_cell(&(line.cells[col]), txt);
}

/*
T_get_cell_attr(tbl, i, j)
TBL *tbl;
int i, j;
{
    TLINE   *line = tbl->lines + i;
    TCELL   *cell;

    switch(line->type) {
	case TABLE_LINE_TITLE : if(j > 0) return(TABLE_CELL_BOLD & TABLE_CELL_CENTER);
	case TABLE_LINE_CELL  : break;
	default :       return(TABLE_CELL_BOLD & TABLE_CELL_LEFT);
	}
    cell = (TCELL *)line->cells + j;
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
    TLINE& line = tbl->lines[i];

    switch(line.type) 
    {
        case TABLE_LINE_TITLE :
            if(j > 0) 
                return;
        case TABLE_LINE_CELL  :
            break;
        default :
            return;
    }

    line.cells[j].attribute = attr;
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
    TCELL* cell;

    if(titg) 
    {
        T_append_line(tbl, TABLE_LINE_TITLE);
        SCR_strip((unsigned char*) titg);
        if(titg[0])
        {
            cell = &(tbl->lines.back().cells[0]);
            T_set_string_cell(cell, (unsigned char*) titg);
        }
        T_append_line(tbl, TABLE_LINE_SEP);
    }

    T_append_line(tbl,  TABLE_LINE_CELL);
    cell = &(tbl->lines.back().cells[0]);
    T_set_lec_cell(cell, (unsigned char*) "");
    for(int j = 1 ; j < T_NC(tbl) ; j++) 
    {
        cell = &(tbl->lines.back().cells[j]);
        T_set_string_cell(cell, (unsigned char*) "\"#S");
        T_set_cell_attr(tbl, tbl->lines.size() - 1, j, TABLE_CELL_CENTER);
    }

    T_append_line(tbl, TABLE_LINE_SEP);

    if(lecs && titls) 
    {
        for(int i = 0 ; lecs[i] && titls[i]; i++) 
        {
            T_append_line(tbl, TABLE_LINE_CELL);
            cell = &(tbl->lines.back().cells[0]);
            T_set_string_cell(cell, (unsigned char*) titls[i]);
            for(int j = 1 ; j < T_NC(tbl) ; j++)
            {
                cell = &(tbl->lines.back().cells[j]);
                T_set_lec_cell(cell, (unsigned char*) lecs[i]);
            }
        }
    }
    else 
        T_append_line(tbl, TABLE_LINE_CELL);
    
    if(mode || files || date)
        T_append_line(tbl,  TABLE_LINE_SEP);
    
    if(mode)  
        T_append_line(tbl,  TABLE_LINE_MODE);
    
    if(files) 
        T_append_line(tbl,  TABLE_LINE_FILES);
    
    if(date)  
        T_append_line(tbl,  TABLE_LINE_DATE);

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
    
    if(pos < 0) 
        titg = (char*) SCR_stracpy((unsigned char*) def);
    else 
        titg = (char*) SCR_stracpy((unsigned char*) KCVAL(kdb, pos));
    
    for(i = 0; i < nb; i++) 
    {
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

std::size_t hash_value(const TBL& table)
{
    std::hash<TBL> tbl_hash;
    return tbl_hash(table);
}
