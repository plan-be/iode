/**
 *  @header4iode
 *
 *  Functions to generate IODE tables in A2M format based on Table structures and GSample definition.
 *  Includes some A2M helper functions.
 *
 *  Note that the functions needed to generate graphs from tables can be found in k_graph.c.
 */
#include "scr4/s_a2m.h"

#include "api/pch.h"
#include "api/b_a2mini.h"
#include "api/k_lang.h"
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/print/print.h"
#include "api/write/write.h"


/**
 *  Compiles a GSample into a std::vector<COL> struct and resizes std::vector<COL> according to the nb of cols in Table.
 *
 *  @param [in] std::shared_ptr<Table> tbl_ptr   table to compute
 *  @param [in] const std::string&     gsample   GSample
 *  @param [out] std::vector<COL>& columns  column definitions for computing the table
 *  @return     int                              total number of columns for the computed table
 *
 */
int initialize_columns(std::shared_ptr<Table> tbl_ptr, const std::string& gsample, std::vector<COL>& columns)
{
    if(!tbl_ptr)
        return -1;

    columns = compile_gsample((char*) gsample.c_str());
    if(columns.empty())
    {
        std::string error_msg = "Illegal sample '" + gsample + "': syntax error";
        error_manager.append_error(error_msg);
        return -1;
    }

    int dim = resize_tbl_columns(tbl_ptr.get(), columns);
    return dim;
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
 *  @global [in] int        tbl_nb_decimals  number of decimal places
 *
 */

void T_print_val(double val)
{
    char    buf[64];

    T_fmt_val(buf, val, 30, tbl_nb_decimals);   // JMP 18-04-2022
    W_printf(buf);
}


/**
 *  Translates a TableCell of type KT_TEXT into a text using col_to_text(). Sends the result to W_printf().
 *
 *  @param  [in] cl         the column of the GSample to be printed (period, file nb, operation...)
 *  @param  [in] string     the table column definition (ex "#s")
 *
 */
void T_print_string(COL* cl, char* string)
{
    char* ptr = (char *) col_to_text(cl, string, (int) v_tbl_filenames.size());
    if(ptr != NULL) W_printf((char*) "%s", ptr);
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

    W_printf((char*) "%c%d%c", A2M_SEPCH, straddle, align);
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
 * @brief Prints a Table line of type TITLE
 *
 * @param cell
 * @param straddle
 * @return int
 */
void T_print_title(TableCell* cell, int straddle)
{
    if(cell == nullptr || cell->is_null())
    {
        W_printf((char*) "%c1R", A2M_SEPCH);
        return;
    }

    std::string content = cell->get_content(false);
    // NOTE: W_Print(...) functions expect OEM encoding, so convert content
    //       from UTF-8 to OEM before printing
    content = utf8_to_oem(content);

    int attribute = (int) cell->get_attribute();
    T_open_cell(attribute, straddle, TABLE_CELL_STRING);
    T_open_attr(attribute);

    W_printf((char*) "%s", (char*) content.c_str());

    T_close_attr(attribute);
}


/**
 *  Prints a Table cell on a specific GSample column.
 *
 *  @param [in] TableCell*  cell        table cell to print
 *  @param [in] COL*        cl          GSample column definition with the value already calculated
 *  @param [in] int         straddle    nb of spanned columns int the resulting a2m table
 *
 */
void T_print_cell(TableCell* cell, COL* cl, int straddle)
{
    if(cell == nullptr || cell->is_null())
    {
        W_printf((char*) "%c1R", A2M_SEPCH);
        return;
    }

    TableCellType cell_type = cell->get_type();

    std::string content = cell->get_content(false);
    // NOTE: W_Print(...) functions expect OEM encoding, so convert content
    //       from UTF-8 to OEM before printing
    content = utf8_to_oem(content);

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
 *  @param [in, out] std::vector<COL>& columns  compiled GSample columns
 *  @return     int             0 on success, -1 on error.
 *
 */

int T_print_line(std::shared_ptr<Table> tbl_ptr, int i, std::vector<COL>& columns)
{
    clear_tbl_columns(columns);
    if(execute_tbl_columns(tbl_ptr.get(), i, columns) < 0)
        return -1;

    int     d;
    COL*    cl;
    TableCell*  cell;
    TableLine&  line = tbl_ptr->lines[i];

    for(int j = 0; j < columns.size(); j++)
    {
        d = j % tbl_ptr->nb_columns;
        if(tbl_ptr->repeat_columns == 0 && d == 0 && j != 0)
            continue;
        if(line.cells.size() > d)
        {
            cl = &columns[j];
            cell = &line.cells[d];
            T_print_cell(cell, cl, 1);
        }
    }

    return 0;
}


/**
 *  Retrieves the filenames used in the std::vector<COL> (from GSample) needed to print the special table line TABLE_LINE_FILES.
 *
 *  @param [in] const std::vector<COL>& columns  compiled GSample columns
 *  @return     char**          NULL if one of the ref files is not loaded in global_ref_xxx
 *                              table of filenames in the form "[<file number>] <filename>" if all files are in mem
 */
std::vector<std::string> T_find_files(const std::vector<COL>& columns)
{
    std::vector<bool> files(K_MAX_FREF + 1, false);
    for(const COL& column : columns)
    {
        files[column.cl_fnb[0]] = true;
        files[column.cl_fnb[1]] = true;
    }

    KDBVariablesPtr kdb;
    std::string filename;
    std::vector<std::string> filenames;
    for(int i = 1; i < K_MAX_FREF + 1; i++)
    {
        if(!files[i])
            continue;

        kdb = global_ref_var[i - 1];
        if(!kdb)
        {
            std::string error_msg = "File " + std::to_string(i) + " not present";
            error_manager.append_error(error_msg);
            return std::vector<std::string>();
        }

        filename = "[" + std::to_string(i) + "] " + kdb->filepath;
        std::replace(filename.begin(), filename.end(), '\\', '/');
        filenames.push_back(filename);
    }

    return filenames;
}

/**
 *  Prints the special Table line of type TABLE_LINE_FILES.
 *
 *  @param [in] const std::vector<COL>& columns  compiled GSample columns
 *  @param  [in] int    dim         total number of columns in the resulting table (size of GSample x nb table cols)
 */
void T_print_files(const std::vector<COL>& columns, int dim)
{
    if(v_tbl_filenames.empty())
        return;

    for(const std::string& filename : v_tbl_filenames)
    {
        T_open_cell(TABLE_CELL_LEFT, dim, TABLE_CELL_STRING);
        W_printf((char*) "%s", filename.c_str());
    }
}


/**
 *  Prints the special Table line of type TABLE_LINE_MODE (growth rates, diff...).
 *
 *  @param [in] const std::vector<COL>& columns  compiled GSample columns
 *  @param  [in] int    dim         total number of columns in the resulting table (size of GSample x nb table cols)
 *  @global [in] char** tbl_mode     list of modes used in std::vector<COL> (computed in T_begin_tbl())
 *
 */

void T_print_mode(const std::vector<COL>& columns, int dim)
{
    int    i;

    for(i = 0; i < MAX_MODE; i++)
    {
        if(tbl_mode[i] == 0)
            continue;
        T_open_cell(TABLE_CELL_LEFT, dim, TABLE_CELL_STRING);
        W_printf((char*) "(%s) %s", COL_OPERS[i + 1], KLG_OPERS_TEXTS[i + 1][K_LANG]);
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
    T_open_cell(TABLE_CELL_LEFT, dim, TABLE_CELL_STRING);
    W_printf((char*) "%s", date);
}


/**
 *  Prints a table header in A2M.
 *  Initialises globals v_tbl_filenames and tbl_mode.
 *
 *  @param [in] int     dim     total number of columns in the resulting table (size of GSample x nb table cols)
 *  @param [in] const std::vector<COL>& columns  compiled GSample columns
 *  @return
 */
int T_begin_tbl(int dim, const std::vector<COL>& columns)
{
    v_tbl_filenames = T_find_files(columns);
    if(v_tbl_filenames.empty())
        return -1;

    tbl_find_mode(columns, tbl_mode, 2);

    W_printf((char*) ".tb %d\n", dim);

    if(A2M_SEPCH == '\t')
        W_printf((char*) ".sep TAB");
    else
        W_printf((char*) ".sep %c", A2M_SEPCH);
    W_printf("\n");

    return 0;
}


/**
 *  Prints a table footer in A2M and frees the temporary allocated variables.
 */
void T_end_tbl()
{
    W_printf((char*) ".te \n");
    v_tbl_filenames.clear();
}
