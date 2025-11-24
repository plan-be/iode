/**
 *  @header4iode
 *  
 *  Functions to generate IODE tables in A2M format based on Table structures and GSample definition.
 *  Includes some A2M helper functions. 
 *  
 *  Note that the functions needed to generate graphs from tables can be found in k_graph.c.
 *  
 *  List of functions 
 *  -----------------
 *      int T_prep_cls(Table* tbl, char* smpl, COLS** cls)            Compiles a GSample into a COLS struct and resizes COLS according to the nb of cols in Table
 *      void T_fmt_val(char* buf, double val, int lg, int nd)       Formats a double value
 *      void T_print_val(double val)                                Prints a double value using W_printf()
 *      void T_open_cell(int attr, int straddle, int type)          Prints the header of an a2m table cell
 *      void T_open_attr(int attr)                                  Opens an A2M attribute sequence.
 *      void T_close_attr(int attr)                                 Closes an A2M attribute sequence.
 *      void T_print_cell(TableCell* cell, COL* cl, int straddle)       Prints a Table cell on a specific GSample column. 
 *      char **T_find_files(COLS* cls)                              Retrieves the filenames used in the COLS (from GSample) needed to print the special table line TABLE_LINE_FILES.
 *      unsigned char *T_get_title(Table* tbl)                        Retrieves a Table title, i.e. the contents of the first line of type TABLE_LINE_TITLE
 *      int T_print_tbl(Table* tbl, char* smpl)                       Computes a table on a GSample and saves the result in A2M format
 *  
 *  Global variables
 *  ----------------
 *      char     KT_sep = '&';                                      Table cell separator => replaced by A2M_SEPCH
 *      int      K_NBDEC = -1;                                      Default nb of decimals
 *  
 */
#include "scr4/s_a2m.h"

#include "api/constants.h"
#include "api/k_lang.h"
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/print/print.h"
#include "api/write/write.h"


/**
 *  Compiles a GSample into a COLS struct and resizes COLS according to the nb of cols in Table.
 *  
 *  @param [in] Table*   tbl      table to compute
 *  @param [in] char*  smpl     GSample 
 *  @param [in] COLS** cls      result = column definition for computing of the table
 *  @return     int             total number of columns for the computed table
 *  
 */
int T_prep_cls(Table* tbl, char* smpl, COLS** cls)
{
    int     dim;

    *cls = COL_cc(smpl);
    if(*cls == NULL) {
        std::string error_msg = "Illegal sample '" + std::string(smpl) + "': syntax error";
        error_manager.append_error(error_msg);
        return(-1);
    }

    dim = COL_resize(tbl, *cls);
    return(dim);
}

/**
 *  Formats a double value. Uses SCR_fmt_dbl(). See http://xon.be/scr4/libs1/libs1167.htm.
 *  
 *  @param [in, out]    char*       buf     placeholder of the result
 *  @param [in]         double   val     input real value
 *  @param [in]         int         lg      max result string length
 *  @param [in]         int         nd      number of decimal places 
 *  
 */
 
void T_fmt_val(char* buf, double val, int lg, int nd) 
{
    if(IODE_IS_A_NUMBER(val)) 
        SCR_fmt_dbl(val, (unsigned char*) buf, lg, nd);
    else strcpy(buf, "-.-");
    SCR_sqz((unsigned char*) buf);
}


/**
 *  Prints a double value using W_printf().
 *  
 *  @param  [in] double  val      value to print
 *  @global [in] int        K_NBDEC  number of decimal places
 *  
 */
 
void T_print_val(double val)
{
    char    buf[64];

    T_fmt_val(buf, val, 30, K_NBDEC);   // JMP 18-04-2022
    W_printf(buf);
}


/**
 *  Translates a TableCell of type KT_TEXT into a text using COL_text(). Sends the result to W_printf().
 *  
 *  @param  [in] cl         the column of the GSample to be printed (period, file nb, operation...)
 *  @param  [in] string     the table column definition (ex "#s")  
 *  @global [in] KT_nbnames number of compared files in the COLS definition
 *  
 */
 
void T_print_string(COL* cl, char* string)
{
    char   *ptr = NULL;

    ptr = (char *) COL_text(cl, string, KT_nbnames);
    if(ptr != NULL) W_printf("%s", ptr);
    SW_nfree(ptr);
}


/**
 *  Prints the header of an a2m table cell: <cellsep><span><align>.
 *  Example: "@2C" if @ is the cell separator, 2 the number of spanned columns and the text must be centered in the column.
 *   
 *  @param  [in] int attr        alignment attribute: TABLE_CELL_CENTER, TABLE_CELL_RIGHT, TABLE_CELL_DECIMAL, TABLE_CELL_LEFT.
 *  @param  [in] int straddle    number of spanned columns
 *  @param  [in] int type        column type (TABLE_CELL_STRING, TABLE_CELL_LEC...)
 *  @global [in] int A2M_SEPCH   a2m table cell separator
 *  
 */
 
void T_open_cell(int attr, int straddle, int type) 
{
    char    align = 'L';

    if(attr & TABLE_CELL_CENTER)    align = 'C';
    if(attr & TABLE_CELL_RIGHT)     align = 'R';
    if(type != TABLE_CELL_STRING && (attr & TABLE_CELL_DECIMAL))   align = 'D'; /* JMP 17-12-93 */

    W_printf("%c%d%c", A2M_SEPCH, straddle, align);
}


/**
 *  Opens an A2M attribute sequence.
 *  
 *  @param [in] int     attr    Cell attribute: TABLE_CELL_BOLD...
 *  
 */
void T_open_attr(int attr)
{

    if(attr & TABLE_CELL_BOLD)      W_printfReplEsc("~b");
    if(attr & TABLE_CELL_ITALIC)    W_printfReplEsc("~i");
    if(attr & TABLE_CELL_UNDERLINE) W_printfReplEsc("~u");
}


/**
 *  Closes an A2M attribute sequence.
 *  
 *  @param [in] int     attr    Cell attribute: TABLE_CELL_BOLD...
 *  
 */
void T_close_attr(int attr)
{
    if(attr & TABLE_CELL_BOLD)      W_printfReplEsc("~B");
    if(attr & TABLE_CELL_ITALIC)    W_printfReplEsc("~I");
    if(attr & TABLE_CELL_UNDERLINE) W_printfReplEsc("~U");
}


/**
 *  Prints a Table cell on a specific GSample column. 
 *  
 *  @param [in] TableCell*  cell        table cell to print
 *  @param [in] COL*    cl          GSample column definition with the value already calculated
 *  @param [in] int     straddle    nb of spanned columns int the resulting a2m table 
 *  
 */
void T_print_cell(TableCell* cell, COL* cl, int straddle)
{
    if(cell == nullptr || cell->is_null()) 
    {
        //W_printf("%c1R", KT_sep); 
        W_printf("%c1R", A2M_SEPCH); 
        return;                
    }

    TableCellType cell_type = cell->get_type();
    std::string content = cell->get_content(false, false);

    if(cell_type == TABLE_CELL_STRING && content.find('#') != std::string::npos)
        cell->set_align(TABLE_CELL_RIGHT);
    
    if(cell_type == TABLE_CELL_LEC)
        cell->set_align(TABLE_CELL_DECIMAL);

    int attribute = (int) cell->get_attribute();
    T_open_cell(attribute, straddle, (int) cell_type);
    T_open_attr(attribute);

    if(cl == NULL || cell_type == TABLE_CELL_STRING)
        T_print_string(cl, (char*) content.c_str());
    else 
        T_print_val(cl->cl_res);

    T_close_attr(attribute);
}


/**
 *  Prints one table line for all columns defined in cls.
 *  
 *  @param [in] Table*    tbl     source table
 *  @param [in] int     i       line to print
 *  @param [in] COLS*   cls     columns to print = compiled GSample
 *  @return     int             0 on success, -1 on error.        
 *  
 */

int T_print_line(Table* tbl, int i, COLS* cls)
{
    COL_clear(cls);
    if(COL_exec(tbl, i, cls) < 0)   
        return(-1);

    int     d;
    COL*    cl;
    TableCell*  cell;
    TableLine&  line = tbl->lines[i];

    for(int j = 0; j < cls->cl_nb; j++) 
    {
        d = j % T_NC(tbl);
        if(tbl->repeat_columns == 0 && d == 0 && j != 0) 
            continue;
        if(line.cells.size() > d)
        {
            cl = cls->cl_cols + j;
            cell = &line.cells[d];
            T_print_cell(cell, cl, 1);
        }
    }
    W_printf("\n");

    return(0);
}


/**
 *  Retrieves the filenames used in the COLS (from GSample) needed to print the special table line TABLE_LINE_FILES. 
 *   
 *  @param [in] COLS*   cls     list of columns (from GSample)
 *  @return     char**          NULL if one of the ref files is not loaded in K_RWS
 *                              table of filenames in the form "[<file number>] <filename>" if all files are in mem
 *  
 */

char **T_find_files(COLS* cls)
{
    int     i, nf = 0;
    COL     *cl;
    int     files[K_MAX_FREF + 1];
    char    **names = 0, buf[K_MAX_FILE + 10];
    KDB     *kdb;

    memset(files, 0, (K_MAX_FREF + 1) * sizeof(int));
    for(i = 0; i < cls->cl_nb; i++) 
    {
        cl = cls->cl_cols + i;
        files[cl->cl_fnb[0]] = 1;
        files[cl->cl_fnb[1]] = 1;
    }

    for(i = 1; i < K_MAX_FREF + 1; i++) 
    {
        if(files[i] == 0) 
            continue;
        
        kdb = K_RWS[VARIABLES][i - 1];
        if(!kdb) 
        {
            std::string error_msg = "File " + std::to_string(i) + " not present";
            error_manager.append_error(error_msg);
            SCR_add_ptr((unsigned char***) &names, &nf, 0L);
            SCR_free_tbl((unsigned char**) names);
            return(NULL);
        }

        sprintf(buf, "[%d] %s", i, (char*) kdb->filepath.c_str());
        SCR_replace((unsigned char*) buf, (unsigned char*) "\\", (unsigned char*) "/");
        //B_path_change(buf);
        SCR_add_ptr((unsigned char***) &names, &nf, (unsigned char*) buf);
    }
    
    SCR_add_ptr((unsigned char***) &names, &nf, 0L);
    return(names);
}

/**
 *  Prints the special Table line of type TABLE_LINE_FILES.
 *  
 *  @param  [in] COLS*  cls         columns to print = compiled GSample
 *  @param  [in] int    dim         total number of columns in the resulting table (size of GSample x nb table cols)
 *  @global [in] char** KT_names    list of formatted filenames
 *  @global [in] int    KT_nbnames  number if filenames
 *  
 */
 
void T_print_files(COLS* cls, int dim)
{
    int     i;

    if(KT_nbnames <= 0) return;  

    for(i = 0; KT_names[i]; i++) {
        T_open_cell(TABLE_CELL_LEFT, dim, TABLE_CELL_STRING); /* JMP 17-12-93 */
        W_printf("%s\n", KT_names[i]);
    }
}


/**
 *  Prints the special Table line of type TABLE_LINE_MODE (growth rates, diff...).
 *  
 *  @param  [in] COLS*  cls         columns to print = compiled GSample
 *  @param  [in] int    dim         total number of columns in the resulting table (size of GSample x nb table cols)
 *  @global [in] char** KT_mode     list of modes used in COLS (computed in T_begin_tbl())
 *  
 */

void T_print_mode(COLS* cls, int dim)
{
    int    i;

    for(i = 0; i < MAX_MODE; i++) {
        if(KT_mode[i] == 0) continue;
        T_open_cell(TABLE_CELL_LEFT, dim, TABLE_CELL_STRING);  /* JMP 17-12-93 */
        //W_printf("(%s) %s\n", COL_OPERS[i + 1], KLG_OPERS_TEXTS[i + 1][B_LANG]);
        W_printf("(%s) %s\n", COL_OPERS[i + 1], KLG_OPERS_TEXTS[i + 1][K_LANG]); // JMP 18-04-2022
    }
}


/**
 *  Prints the special Table line of type TABLE_LINE_DATE.
 *  
 *  @param  [in] int    dim   total number of columns in the resulting table (size of GSample x nb table cols)
 *  
 */

void T_print_date(int dim)
{
    long    SCR_current_date();
    char    date[11];

    SCR_long_to_fdate(SCR_current_date(), date, "dd/mm/yy");
    T_open_cell(TABLE_CELL_LEFT, dim, TABLE_CELL_STRING); /* JMP 17-12-93 */
    W_printf("%s\n", date);
}


/**
 *  Prints a table header in A2M.
 *  Initialises globals KT_names, KT_nbnames and KT_mode.
 *  
 *  @param [in] int     dim     total number of columns in the resulting table (size of GSample x nb table cols)
 *  @param [in] COLS*   cls     columns to print = compiled GSample
 *  @return 
 *  
 */
 
int T_begin_tbl(int dim, COLS* cls)
{
    KT_names = T_find_files(cls);
    KT_nbnames = SCR_tbl_size((unsigned char**) KT_names);
    if(KT_nbnames == 0) return(-1); /* JMP 11-06-99 */
    COL_find_mode(cls, KT_mode, 2);

    W_printf(".tb %d\n", dim);
    //if(KT_sep == '\t') W_printf(".sep TAB\n");
    //else W_printf(".sep %c\n", KT_sep);
    if(A2M_SEPCH == '\t') W_printf(".sep TAB\n");
    else W_printf(".sep %c\n", A2M_SEPCH);
    
    return(0);
}


/**
 *  Prints a table footer in A2M and frees the temporary allocated variables. 
 */

void T_end_tbl()
{
    W_printf(".te \n");
    SCR_free_tbl((unsigned char**) KT_names);
    KT_names = NULL;
    KT_nbnames = 0;
}


/**
 *  Retrieves a Table title, i.e. the contents of the first line of type TABLE_LINE_TITLE.
 *  
 *  @param [in] Table*    tbl     pointer to a table
 *  @return     char*           local static buffer with the contents of the title or the text "No title"
 *                              if no line of type TABLE_LINE_TITLE can be found or if the first line of that type is empty.
 */

unsigned char* T_get_title(Table* tbl, const bool to_utf8)  
{
    int                     k;
    static unsigned char    buf[256];
    
    // get the first line of type TABLE_LINE_TITLE
    for(k = 0; k < T_NL(tbl); k++)
        if(tbl->lines[k].get_type() == TABLE_LINE_TITLE) 
            break;

    TableLine line = tbl->lines[k];
    TableCell cell = line.cells[0];
    std::string title = cell.get_content(false, to_utf8);

    // New version using local static buffer to solve link problems // JMP 11/04/2022

    if(k == T_NL(tbl) || title.empty())
        strcpy((char*) buf, "No title");
    else
        SCR_strlcpy(buf, (unsigned char*) title.c_str(), sizeof(buf) - 1);

    return(buf);
}


/**
 *  Computes a table on a GSample and saves the result in A2M format (see https://iode.plan.be/doku.php?id=le_langage_a2m for
 *  the syntax of a2m files).
 *  
 *  @param [in] Table*    tbl     table to print
 *  @param [in] char*   smpl    GSample on which the tbl must be computed
 *  @return     int             0 on success, -1 on error (smpl syntax error, files not defined...) 
 *  
 */
int T_print_tbl(Table* tbl, char* smpl)
{
    int     i, dim, rc = 0, first = 1;
    COLS    *cls;

    dim = T_prep_cls(tbl, smpl, &cls);
    if(dim < 0) 
        return(-1);

    // Anciennement
    // B_PrintRtfTopic(T_get_title(tbl));
    // Nouveau JMP 18/04/2022
    W_printf( ".topic %d %d %s\n", KT_CUR_TOPIC++, KT_CUR_LEVEL, T_get_title(tbl, false));
    //if(W_type == A2M_DESTRTF && W_rtfhelp) W_printf(".par1 tit_%d\n%s\n\n", KT_CUR_LEVEL, T_get_title(tbl));
    
    if(T_begin_tbl(dim, cls)) 
        return(-1);
    
    W_printf(".ttitle %s\n", T_get_title(tbl, false));  /* JMP 27-02-98 */
    
    TableLine* line;
    for(i = 0; rc == 0 && i < T_NL(tbl); i++) 
    {
        line = &tbl->lines[i];
        switch(line->get_type()) 
        {
            case TABLE_LINE_SEP   :
                W_printf(".tl\n");
                break;
            case TABLE_LINE_TITLE :
                if(first) {
                    first = 0;
                    break;
                }
                T_print_cell(&(line->cells[0]), NULL, dim);
                W_printf("\n");
                break;
            case TABLE_LINE_DATE  :
                T_print_date(dim);
                break;
            case TABLE_LINE_MODE  :
                T_print_mode(cls, dim);
                break;
            case TABLE_LINE_FILES :
                T_print_files(cls, dim);
                break;
            case TABLE_LINE_CELL  :
                if(T_print_line(tbl, i, cls) < 0) 
                {
                    std::string error_msg = "Unable to print line '" + std::to_string(i) + "'";
                    error_manager.append_error(error_msg);
                    rc = -1;
                }
                break;
        }
    }
    T_end_tbl();

    COL_free_cols(cls);
    return(rc);
}
