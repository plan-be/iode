/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE TBL objects.
 *
 *      KDB *load_asc(char* filename)
 *      int save_asc(KDB* kdb, char* filename)
 *      int save_csv(KDB *kdb, char *filename)
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
 *  Reads the TBL divisors on a YY stream. Each column of a table has its own divisor.
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
 *  @param [in, out] TBL*       tbl    TBL whose divisors (one by column) is to be read
 *  @param [in, out] YYFILE*    yy     stream 
 *  @return          void
 *  
 */
static void read_div(TBL* tbl, YYFILE* yy)
{
    TableCell* cell;
    for(int i = 0; i < tbl->nb_columns; i++)
    {
        cell = read_cell(yy, 0);
        tbl->divider_line.cells[i] = *cell;
    }
}

/**
 *  Reads a TBL line on a YY stream. Each line has as many Cells as the number 
 *  of columns in the table.
 *  
 *  @see https://iode.plan.be/doku.php?id=format_ascii_des_tableaux for the full syntax.
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
 *  @param [in, out] TBL*       tbl    TBL where a new line will be added 
 *  @param [in, out] YYFILE*    yy     stream 
 *  @return          int                0 if success
 *                                      -1 if the TBL cannot be created
 *  
 */
static int read_line(TBL* tbl, YYFILE* yy)
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
                return(0);
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
                    return(0);
                }
                tbl->lines.push_back(TableLine((TableLineType) keyw));
                line = &(tbl->lines.back());
                break;
            case TABLE_ASCII_LINE_TITLE :
                // line already read
                if(line) 
                {
                    YY_unread(yy);
                    return(0);
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
                    return(0);
                }
                // empty string
                tbl->lines.push_back(TableLine(TableLineType::TABLE_LINE_TITLE));
                line = &(tbl->lines.back());
                cell = new TableCell(TABLE_CELL_STRING);
                line->cells.push_back(*cell);
                YY_unread(yy);
                return(0);
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
                    return(0);
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
 *  Reads on the stream yy the full definition of a TBL.
 *  
 *  @see https://iode.plan.be/doku.php?id=format_ascii_des_tableaux for the full syntax.
 *  
 *  @param [in]     yy  YYFILE*     stream to be read
 *  @return             TBL*        new allocated table
 *  
 */
static TBL* read_tbl(YYFILE* yy)
{
    int     keyw, dim = 2;
    TBL     *tbl = NULL;

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

    tbl = T_create(dim);
    if(tbl == NULL) 
        return(NULL);

    while(1) 
    {
        keyw = YY_lex(yy);
        switch(keyw) 
        {
            case TABLE_ASCII_CLOSE   :
                return(tbl);

            case TABLE_ASCII_DUTCH   :
            case TABLE_ASCII_FRENCH  :
            case TABLE_ASCII_ENGLISH :
                tbl->language = keyw;
                break;

            case TABLE_ASCII_DIM     :
                kerror(0, "Double dim definition skipped : %s", YY_error(yy));
                break;

            case TABLE_ASCII_DIVIDER     :
                read_div(tbl, yy);
                break;

            case TABLE_ASCII_BOX     :
                tbl->chart_box = (char)K_read_long(yy);
                break;
            case TABLE_ASCII_AXIS     :
                tbl->chart_axis_type = (char)K_read_long(yy);
                break;
            case TABLE_ASCII_XGRID    :
                tbl->chart_gridx = (char)K_read_long(yy);
                break;
            case TABLE_ASCII_YGRID    :
                tbl->chart_gridy = (char)K_read_long(yy);
                break;

            case TABLE_ASCII_YMIN     :
                tbl->y_min  = (float)K_read_real(yy);
                break;
            case TABLE_ASCII_YMAX     :
                tbl->y_max  = (float)K_read_real(yy);
                break;
            case TABLE_ASCII_ZMIN     :
                tbl->z_min  = (float)K_read_real(yy);
                break;
            case TABLE_ASCII_ZMAX     :
                tbl->z_max  = (float)K_read_real(yy);
                break;

            case TABLE_ASCII_ALIGN:
                tbl->text_alignment = K_read_align(yy);
                break;

            case TABLE_ASCII_BREAK   :
                if(read_line(tbl, yy)< 0) 
                {
                    T_free(tbl);
                    return(NULL);
                }
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

    T_free(tbl);
    return(NULL);
}

/**
 *  Loads TBLs definition from an ASCII file into a new KDB of TBLs.
 *  
 *  @see https://iode.plan.be/doku.php?id=format_ascii_des_tableaux for the full syntax.
 *  
 *  Errors are displayed by a call to the function kerror().
 *  For each read TBL, kmsg() is called to send a message to the user. 
 *  
 *  The implementations of kerror() and kmsg() depend on the context.
 *  
 *  @param [in] filename    char*   ascii file to be read or
 *                                  string containing the definition of the TBLs
 *  @return                 KDB*    NULL or allocated KDB of TBLs
 *  
 */
KDB* AsciiTables::load_asc(char* filename)
{
    static  int sorted;

    int     cmpt = 0;
    KDB     *kdb = 0;
    TBL     *tbl = NULL;
    YYFILE  *yy;
    ONAME   name;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    if(sorted == 0) 
    {
        qsort(TABLE, sizeof(TABLE) / sizeof(YYKEYS), sizeof(YYKEYS), ascii_compare);
        sorted = 1;
    }

    SCR_strip((unsigned char*) filename);
    yy = YY_open(filename, TABLE, sizeof(TABLE) / sizeof(YYKEYS),
                 (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);
    if(yy == 0) 
    {
        kerror(0, "Cannot open '%s'", filename);
        return(kdb);
    }

    /* READ FILE */
    kdb = K_create(TABLES, UPPER_CASE);
    K_set_kdb_fullpath(kdb, (U_ch*)filename); // JMP 30/11/2022

    char asc_filename[1024];
    while(1) 
    {
        switch(YY_lex(yy)) 
        {
            case YY_EOF :
                if(cmpt) 
                {
                    K_set_ext_asc(asc_filename, filename, TABLES);
                    K_set_kdb_fullpath(kdb, (U_ch*)asc_filename); // JMP 03/12/2022
                }
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                if((tbl = read_tbl(yy)) == NULL) 
                {
                    kerror(0, "%s : table defined", YY_error(yy));
                    goto err;
                }
                if(K_add(kdb, name, tbl) < 0)  
                    goto err;
                kmsg("Reading object %d : %s", ++cmpt, name);
                T_free(tbl);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err:
    K_free(kdb);
    YY_close(yy);
    return((KDB *)0);
}

/*------- SAVE IN ASCII -------------------*/

/**
 *  Prints the alignment value of the TBL.
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
 *  Prints the axis position (left or right) and type (BAR or LINE) of a TBL line.
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
 *  Prints the axis (min or max, left or right) of the TBL.
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
 *  Prints a TBL definition.
 *  
 *  @param [in] fd      FILE*     output stream
 *  @param [in] tbl     TBL*      table to print
 *  @return             void
 *  
 */
static void print_tbl(FILE* fd, TBL* tbl)
{
    /* tbl */
    fprintf(fd, "\nDIM %d\n", T_NC(tbl));
    K_wrdef(fd, TABLE, T_LANG(tbl));
    /* GB 2/3/00 */
    fprintf(fd, "\nBOX %d AXIS %d XGRID %d YGRID %d ",
            tbl->chart_box, tbl->chart_axis_type, tbl->chart_gridx, tbl->chart_gridy);
    print_align(fd, tbl->text_alignment);
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
 *  Saves a KDB of TBLs into an ascii file (.at) or to the stdout.
 *  
 *  @see load_asc() for the syntax of the ascii tables. 
 *  
 *  @param [in] kdb         KDB*    KDB of TBLs
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
int AsciiTables::save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    TBL     *tbl;
    int     i;

    if(filename[0] == '-') fd = stdout;
    else {
        fd = fopen(filename, "w+");
        if(fd == 0) {
            kerror(0, "Cannot create '%s'", filename);
            return(-1);
        }
    }

    char* name;
    for(i = 0 ; i < KNB(kdb); i++) 
    {
        name = KONAME(kdb, i);
        fprintf(fd, "%s {", name);
        tbl = KTVAL(kdb, i);
        print_tbl(fd, tbl);
        fprintf(fd, "}\n");
        T_free(tbl);
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}

/* 
 * Save a KDB of TBLs in a .csv file.
 * NOT IMPLEMENTED.
 */
int AsciiTables::save_csv(KDB *kdb, char *filename, Sample* sample, char** varlist)
{
    return(-1);
}
