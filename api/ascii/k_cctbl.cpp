/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE Table objects.
 *
 *      bool load_asc(const std::string& filename)
 *      bool save_asc(const std::string& filename)
 */
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/tables.h"
#include "api/ascii/ascii.h"


/**
 *  Reads a TableCell definition on a YY stream.
 *  
 *  If mode is not null, the next token read on yy must be of type mode. For example, if mode == YY_STRING, the 
 *  next token must be a string. If not, the stream yy is "rewound".
 *  
 *  @param [in, out] TableCell*     cell    cell to be read
 *  @param [in, out] YYFILE*    yy      stream 
 *  @param [in]      int        mode    if not null, type of the next token on the stream
 *  @return          void
 *  
 */
static TableCell* read_cell(YYFILE* yy, int mode)
{
    int keyw;

    keyw = YY_lex(yy);
    if(mode != 0 && mode != keyw) 
    {
        YY_unread(yy);
        return nullptr;
    }

    int ok = 0;
    std::string content;
    TableCell* cell = new TableCell(TABLE_CELL_STRING);
    while(1) 
    {
        switch(keyw) 
        {
            case TABLE_CELL_RIGHT  :
                cell->set_align(TABLE_CELL_RIGHT);
                break;
            case TABLE_CELL_LEFT   :
                cell->set_align(TABLE_CELL_LEFT);
                break;
            case TABLE_CELL_CENTER :
                cell->set_align(TABLE_CELL_CENTER);
                break;
            case TABLE_CELL_DECIMAL :
                cell->set_align(TABLE_CELL_DECIMAL);
                break;
            case TABLE_CELL_ITALIC :
                cell->set_italic(true);
                break;
            case TABLE_CELL_BOLD   :
                cell->set_bold(true);
                break;
            case TABLE_CELL_UNDERLINE:
                cell->set_underline(true);
                break;
            case TABLE_CELL_NORMAL :
                cell->set_italic(false);
                cell->set_bold(false);
                cell->set_underline(false);
                break;
            case TABLE_ASCII_CELL_LEC    :
                if(ok == 1) 
                    goto ret;
                ok = 1;
                if(YY_lex(yy) != YY_STRING) 
                {
                    YY_unread(yy);
                    break;
                }
                content = std::string((char*) yy->yy_text);
                cell->set_lec(content);
                cell->set_align(TABLE_CELL_DECIMAL);
                break;

            case YY_STRING :
                if(ok == 1) 
                    goto ret;
                ok = 1;
                content = std::string((char*) yy->yy_text);
                cell->set_text(content, false);
                break;

            case YY_EOF   :
            case TABLE_ASCII_BREAK :
            default       :
                goto ret;
        }
        keyw = YY_lex(yy);
    }

ret :
    YY_unread(yy);
    return cell;
}

/**
 *  Reads the Table divisors on a YY stream. Each column of a table has its own divisor.
 *  
 *  Syntax of the section DIV
 *  -------------------------
 *  DIV div_col_1 div_col_2 ...
 *  where div_col_n = {LEC "lec expression" | "free text"}
 *  
 *  Example for a 2-columns table
 *  -----------------------------
 *  DIV LEC "1" LEC "PIB[2010]"
 *    
 *  @param [in, out] Table*       tbl    Table whose divisors (one by column) is to be read
 *  @param [in, out] YYFILE*    yy     stream 
 *  @return          void
 *  
 */
static void read_div(Table* tbl, YYFILE* yy)
{
    TableCell* cell;
    for(int i = 0; i < tbl->nb_columns; i++)
    {
        cell = read_cell(yy, 0);
        tbl->divider_line.cells[i] = *cell;
    }
}

/**
 *  Reads a Table line on a YY stream. Each line has as many Cells as the number 
 *  of columns in the table.
 *  
 * for the full syntax.
 *
 *  Partial syntax of a TABLE LINE 
 *  -------------------------------
 *  - TITLE "text accross all columns of the table"
 *  - LINE  
 *  - <cell> ... <cell> 
 *  where cell1 ::= "text" <align> | LEC "lec expression" <align> <axis> <graph_type>
 *  - FILES 
 *  - MODE
 *  - DATE 
 *  
 *  Example for a 2-columns table
 *  -----------------------------
 *  - TITLE "Table title" BOLD CENTER        
 *  - LINE        
 *  - "" "#s" CENTER LAXIS GRLINE
 *  - LINE
 *  - "line A" LEFT LEC "A" DECIMAL  LAXIS GRLINE        
 *  - "line B" LEFT LEC "B" DECIMAL  LAXIS GRLINE
 *  
 *    
 *  @param [in, out] Table*       tbl    Table where a new line will be added 
 *  @param [in, out] YYFILE*    yy     stream 
 *  @return          int                0 if success
 *                                      -1 if the Table cannot be created
 *  
 */
static int read_line(Table* tbl, YYFILE* yy)
{
    int     keyw;
    TableCell*  cell = nullptr;
    TableLine*  line = nullptr;

    while(1) 
    {
        keyw = YY_lex(yy);
        switch(keyw) 
        {
            case YY_EOF    :
                YY_unread(yy);
                return 0;
            case TABLE_CELL_RIGHT  :
            case TABLE_CELL_LEFT   :
            case TABLE_CELL_CENTER :
            case TABLE_CELL_DECIMAL:
            case TABLE_CELL_ITALIC :
            case TABLE_CELL_BOLD   :
            case TABLE_CELL_UNDERLINE:
            case TABLE_CELL_NORMAL :
                kerror(0, "Attribute definition %s, skipped", YY_error(yy));
                break;
            case TABLE_ASCII_LINE_SEP   :
            case TABLE_ASCII_LINE_MODE  :
            case TABLE_ASCII_LINE_DATE  :
            case TABLE_ASCII_LINE_FILES :
                // line already read
                if(line) 
                {
                    YY_unread(yy);
                    return 0;
                }
                tbl->lines.push_back(TableLine((TableLineType) keyw));
                line = &(tbl->lines.back());
                break;
            case TABLE_ASCII_LINE_TITLE :
                // line already read
                if(line) 
                {
                    YY_unread(yy);
                    return 0;
                }
                tbl->lines.push_back(TableLine((TableLineType) keyw));
                line = &(tbl->lines.back());
                cell = read_cell(yy, YY_STRING);
                line->cells.push_back(*cell);
                break;
            case TABLE_ASCII_BREAK :
                // line already read
                if(line) 
                {
                    YY_unread(yy);
                    return 0;
                }
                // empty string
                tbl->lines.push_back(TableLine(TableLineType::TABLE_LINE_TITLE));
                line = &(tbl->lines.back());
                cell = new TableCell(TABLE_CELL_STRING);
                line->cells.push_back(*cell);
                YY_unread(yy);
                return 0;
            case TABLE_ASCII_LEFT_AXIS :
                line->right_axis = false;
                break;
            case TABLE_ASCII_RIGHT_AXIS :
                line->right_axis = true;
                break;
            case TABLE_ASCII_GRAPH_LINE :
                line->set_graph_type(TableGraphType::TABLE_GRAPH_LINE);
                break;
            case TABLE_ASCII_GRAPH_SCATTER:
                line->set_graph_type(TableGraphType::TABLE_GRAPH_SCATTER);
                break;
            case TABLE_ASCII_GRAPH_BAR:
                line->set_graph_type(TableGraphType::TABLE_GRAPH_BAR);
                break;
            default: 
                // line already read
                if(line) 
                {
                    YY_unread(yy);
                    return 0;
                }
                YY_unread(yy);
                // Assume it is a line of type CELL
                tbl->lines.push_back(TableLine(TableLineType::TABLE_LINE_CELL));
                line = &(tbl->lines.back());
                line->set_graph_type(T_GRAPHDEFAULT);
                line->cells.clear();
                for(int i = 0; i < T_NC(tbl); i++)
                {
                    cell = read_cell(yy, 0);
                    line->cells.push_back(*cell);
                }  
                break;
        }
    }
}

/**
 *  Reads on the stream yy the full definition of a Table.
 *  
 * for the full syntax.
 *  
 *  @param [in]     yy  YYFILE*     stream to be read
 *  @return             Table*        new allocated table
 *  
 */
static Table* read_tbl(YYFILE* yy)
{
    int     keyw, dim = 2;
    Table     *tbl = NULL;

    keyw = YY_lex(yy);
    if(keyw != TABLE_ASCII_OPEN) 
    {
        kerror(0, "%s: Expected { - Got %s ", yy->yy_text, YY_error(yy));
        return(NULL);
    }

    keyw = YY_lex(yy);
    if(keyw != TABLE_ASCII_DIM) 
        YY_unread(yy);
    else 
    {
        if(YY_lex(yy) != YY_LONG)  
            YY_unread(yy);
        else 
            dim = yy->yy_long;
    }

    tbl = new Table(dim);
    if(!tbl) 
        return nullptr;

    while(1) 
    {
        keyw = YY_lex(yy);
        switch(keyw) 
        {
            case TABLE_ASCII_CLOSE   :
                return tbl;
            case TABLE_ASCII_DUTCH   :
                tbl->set_language(TableLang::TABLE_DUTCH);
                break;
            case TABLE_ASCII_FRENCH  :
                tbl->set_language(TableLang::TABLE_FRENCH);
                break;
            case TABLE_ASCII_ENGLISH :
                tbl->set_language(TableLang::TABLE_ENGLISH);
                break;
            case TABLE_ASCII_DIM     :
                kerror(0, "Double dim definition skipped : %s", YY_error(yy));
                break;
            case TABLE_ASCII_DIVIDER     :
                read_div(tbl, yy);
                break;
            case TABLE_ASCII_BOX     :
                tbl->chart_box = (char) K_read_long(yy);
                break;
            case TABLE_ASCII_AXIS     :
                tbl->set_graph_axis((TableGraphAxis) K_read_long(yy));
                break;
            case TABLE_ASCII_XGRID    :
                tbl->set_gridx((TableGraphGrid) K_read_long(yy));
                break;
            case TABLE_ASCII_YGRID    :
                tbl->set_gridy((TableGraphGrid) K_read_long(yy));
                break;
            case TABLE_ASCII_YMIN     :
                tbl->y_min  = (float) K_read_real(yy);
                break;
            case TABLE_ASCII_YMAX     :
                tbl->y_max  = (float) K_read_real(yy);
                break;
            case TABLE_ASCII_ZMIN     :
                tbl->z_min  = (float) K_read_real(yy);
                break;
            case TABLE_ASCII_ZMAX     :
                tbl->z_max  = (float) K_read_real(yy);
                break;
            case TABLE_ASCII_ALIGN:
                tbl->set_text_alignment((TableTextAlign) K_read_align(yy));
                break;
            case TABLE_ASCII_BREAK   :
                if(read_line(tbl, yy) < 0) 
                {
                    delete tbl;
                    return nullptr;
                }
                break;
            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

    delete tbl;
    return nullptr;
}

/**
 *  Loads Tables definition from an ASCII file into a new KDB of Tables.
 *  
 * for the full syntax.
 *  
 *  Errors are displayed by a call to the function kerror().
 *  For each read Table, kmsg() is called to send a message to the user. 
 *  
 *  The implementations of kerror() and kmsg() depend on the context.
 *  
 *  @param [in] filename     char*   ascii file to be read or
 *                                   string containing the definition of the Tables
 *  @param [in]   db_global  int     1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                  KDB*    NULL or allocated KDB of Tables
 *  
 */
bool KDBTables::load_asc(const std::string& filename)
{
    static  int sorted;

    int     cmpt = 0;
    Table   *tbl = NULL;
    YYFILE  *yy;
    ONAME   name;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    if(sorted == 0) 
    {
        qsort(TABLE, sizeof(TABLE) / sizeof(YYKEYS), sizeof(YYKEYS), ascii_compare);
        sorted = 1;
    }

    std::string trim_filename = trim(filename);
    char* c_filename = (char*) trim_filename.c_str();
    yy = YY_open(c_filename, TABLE, sizeof(TABLE) / sizeof(YYKEYS),
                 (!K_ISFILE(c_filename)) ? YY_STDIN : YY_FILE);

    if(yy == 0)
    {
        std::string error_msg = "Cannot open file '" + trim_filename + "'";
        kwarning(error_msg.c_str());
        return false;
    }

    clear();  /* clear current KDB */

    /* READ FILE */
    this->set_fullpath(c_filename);
    char asc_filename[1024];
    while(1) 
    {
        switch(YY_lex(yy)) 
        {
            case YY_EOF :
                if(cmpt) 
                {
                    K_set_ext_asc(asc_filename, c_filename, TABLES);
                    this->set_fullpath(asc_filename); // JMP 03/12/2022
                }
                YY_close(yy);
                return true;

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                if((tbl = read_tbl(yy)) == NULL) 
                {
                    kwarning("%s : table defined", YY_error(yy));
                    goto err;
                }
                try
                {
                    this->set_obj_ptr(name, tbl); 
                    cmpt++;
                    kmsg("Reading object %d : %s", cmpt, name);
                }
                catch(const std::exception& e)
                {
                    delete tbl;
                    tbl = nullptr;
                    kwarning(e.what());
                    goto err;
                }
                break;

            default :
                kwarning("Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err:
    YY_close(yy);
    return false;
}

/*------- SAVE IN ASCII -------------------*/

/**
 *  Prints the alignment value of the Table.
 *  
 *  @param [in] fd      FILE*     output stream
 *  @param [in] align   int       alignment keyword value
 *  @return             void
 *  
 */
static void print_align(FILE* fd, int align)
{
    fprintf(fd, "\nALIGN ");
    switch(align) {
        case 0:
            fprintf(fd, "LEFT ");
            break;
        case 1:
            fprintf(fd, "CENTER ");
            break;
        case 2:
            fprintf(fd, "RIGHT ");
            break;
    }

}

/**
 *  Prints the axis position (left or right) and type (BAR or LINE) of a Table line.
 *  
 *  @param [in] fd      FILE*     output stream
 *  @param [in] axis    int       0 for left axis, right axis otherwise
 *  @param [in] type    int       2 for BAR type, LINE type otherwise
 *  @return             void
 *  
 */
static void print_graph_info(FILE *fd, int axis, int type)
{
    if(axis == 0) fprintf(fd, " LAXIS ");
    else          fprintf(fd, " RAXIS ");

    if(type == 2) fprintf(fd, " GRBAR ");
    else          fprintf(fd, " GRLINE ");
}

/**
 *  Prints the axis (min or max, left or right) of the Table.
 *  
 *  @param [in] fd      FILE*     output stream
 *  @param [in] align   int       alignment keyword value
 *  @return             void
 *  
 */
static void print_chart_axis_type(FILE* fd, const char* str, float value)
{
    if(IODE_IS_A_NUMBER(value)) fprintf(fd, "\n%s %f ", str, value);
}

/**
 *  Prints the font attribute and the alignement of a cell. 
 *  
 *  @param [in] fd      FILE*       output stream
 *  @param [in] cell    TableCell*
 *  @return             void
 *  
 */
static void print_attribute(FILE* fd, const TableCell* cell)
{
    if(cell->is_bold())      fprintf(fd, "BOLD ");
    if(cell->is_italic())    fprintf(fd, "ITALIC ");
    if(cell->is_underline()) fprintf(fd, "UNDERLINE ");

    switch (cell->get_align())
    {
    case TABLE_CELL_CENTER:
        fprintf(fd, "CENTER ");
        break;
    case TABLE_CELL_DECIMAL:
        fprintf(fd, "DECIMAL ");
        break;
    case TABLE_CELL_LEFT:
        fprintf(fd, "LEFT ");
        break;
    case TABLE_CELL_RIGHT:
        fprintf(fd, "RIGHT ");
        break;
    default:
        break;
    }
}

/**
 *  Prints a table cell definition. 
 *  
 *  @param [in] fd      FILE*       output stream
 *  @param [in] cell    TableCell*      cell to print
 *  @return             void
 *  
 */
static void print_cell(FILE *fd, const TableCell* cell)
{ 
    std::string content = cell->get_content(false, false);
    if(content.empty())
    {
        fprintf(fd, "\"\" ");
        return;
    }

    char* c_content = (char*) content.c_str();
    switch(cell->get_type()) 
    {
        case TABLE_CELL_STRING :
            SCR_fprintf_esc(fd, c_content, 1);
            fprintf(fd, " ");
            print_attribute(fd, cell);
            break;
        case TABLE_CELL_LEC :
            fprintf(fd, "LEC \"%s\" ", c_content);
            print_attribute(fd, cell);
            break;
        default :
            fprintf(fd, "\"\" ");
            break;
    }
}

/**
 *  Prints a Table definition.
 *  
 *  @param [in] fd      FILE*     output stream
 *  @param [in] tbl     Table*      table to print
 *  @return             void
 *  
 */
static void print_tbl(FILE* fd, Table* tbl)
{
    /* tbl */
    fprintf(fd, "\nDIM %d\n", T_NC(tbl));
    K_wrdef(fd, TABLE, (int) tbl->get_language());
    /* GB 2/3/00 */
    fprintf(fd, "\nBOX %d AXIS %d XGRID %d YGRID %d ", tbl->chart_box, 
            (int) tbl->get_graph_axis(), (int) tbl->get_gridx(), (int) tbl->get_gridy());
    print_align(fd, (int) tbl->get_text_alignment());
    print_chart_axis_type(fd, "YMIN", tbl->y_min);
    print_chart_axis_type(fd, "YMAX", tbl->y_max);
    print_chart_axis_type(fd, "ZMIN", tbl->z_min);
    print_chart_axis_type(fd, "ZMAX", tbl->z_max);

    fprintf(fd, "\nDIV ");
    /* div */
    for(int i = 0; i < T_NC(tbl); i++)
        print_cell(fd, &(tbl->divider_line.cells[i]));

    /* lines */
    for(const TableLine& line : tbl->lines) 
    {
        fprintf(fd, "\n- ");
        switch(line.get_type()) 
        {
            case TABLE_ASCII_LINE_CELL:
                for(const TableCell& cell : line.cells)
                    print_cell(fd, &cell);

                /* append GR info */
                print_graph_info(fd, line.right_axis, line.get_graph_type());
                break;

            case TABLE_ASCII_LINE_TITLE :
                K_wrdef(fd, TABLE, line.get_type());
                print_cell(fd, &(line.cells[0]));
                break;

            case TABLE_ASCII_LINE_SEP   :
            case TABLE_ASCII_BOLD_LINE  :
            case TABLE_ASCII_LINE_MODE  :
            case TABLE_ASCII_LINE_DATE  :
            case TABLE_ASCII_LINE_FILES :
                K_wrdef(fd, TABLE, line.get_type());
                break;

            default       :
                break;
        }
    }
    fprintf(fd, "\n/* END OF TABLE */\n");
}

/**
 *  Saves a KDB of Tables into an ascii file (.at) or to the stdout.
 *  
 *  @see load_asc() for the syntax of the ascii tables. 
 *  
 *  @param [in] kdb         KDB*    KDB of Tables
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
bool KDBTables::save_asc(const std::string& filename)
{
    FILE* fd;
    std::string error_msg;

    if(filename[0] == '-') 
        fd = stdout;
    else 
    {
        std::string trim_filename = trim(filename);
        std::string error_msg = "Cannot create '" + trim_filename + "'"; 
        char* c_filename = (char*) trim_filename.c_str();
        fd = fopen(c_filename, "w+");
        if(fd == 0)
        {
            kwarning(error_msg.c_str());
            return false;
        }
    }

    bool success = true;
    for(auto& [name, tbl_ptr] : k_objs) 
    {
        try
        {
            fprintf(fd, "%s {", (char*) name.c_str());
            print_tbl(fd, tbl_ptr);
            fprintf(fd, "}\n");
        }
        catch(const std::exception& e) 
        {
            error_msg += std::string(e.what());
            kwarning(error_msg.c_str());
            success = false;
        }
    }

    if(filename[0] != '-') 
        fclose(fd);

    return success;
}
