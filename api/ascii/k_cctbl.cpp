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
 *  Reads a TCELL definition on a YY stream.
 *  
 *  If mode is not null, the next token read on yy must be of type mode. For example, if mode == YY_STRING, the 
 *  next token must be a string. If not, the stream yy is "rewound".
 *  
 *  @param [in, out] TCELL*     cell    cell to be read
 *  @param [in, out] YYFILE*    yy      stream 
 *  @param [in]      int        mode    if not null, type of the next token on the stream
 *  @return          void
 *  
 */
static void read_cell(TCELL* cell, YYFILE* yy, int mode)
{
    int     keyw, ok = 0, align = TABLE_CELL_LEFT;

    keyw = YY_lex(yy);
    if(mode != 0 && mode != keyw) {
        YY_unread(yy);
        return;
    }

    cell->type = TABLE_CELL_STRING;
    while(1) {
        switch(keyw) {
            case TABLE_CELL_RIGHT  :
            case TABLE_CELL_LEFT   :
            case TABLE_CELL_CENTER :
            case TABLE_CELL_DECIMAL :
                align = keyw;
                break;
            case TABLE_CELL_ITALIC :
            case TABLE_CELL_BOLD   :
            case TABLE_CELL_UNDERLINE:
            case TABLE_CELL_NORMAL :
                cell->attribute |= keyw;
                break;

            case TABLE_ASCII_CELL_LEC    :
                if(ok == 1) goto ret;
                ok = 1;
                if(YY_lex(yy) != YY_STRING) {
                    YY_unread(yy);
                    break;
                }
                T_free_cell(cell);
                if(K_ipack(&(cell->content), (char*) yy->yy_text) < 0)
                    cell->content = NULL;
                cell->type = TABLE_CELL_LEC;
                align = TABLE_CELL_DECIMAL;
                break;

            case YY_STRING :
                if(ok == 1) goto ret;
                ok = 1;
                T_free_cell(cell);
                /*            cell->attribute = TABLE_CELL_LEFT; */
                if(U_is_in('#', (char*) yy->yy_text)) cell->attribute = TABLE_CELL_CENTER;
                K_stracpy(&(cell->content), (char*) yy->yy_text);
                cell->type = TABLE_CELL_STRING;
                break;


            case YY_EOF   :
            case TABLE_ASCII_BREAK :
            default       :
                goto ret;
        }
        keyw = YY_lex(yy);
    }

ret :
    cell->attribute |= align;
    YY_unread(yy);
    return;
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
    int     i;
    TCELL   *cell;

    cell = (TCELL *) (tbl->t_div).tl_val;
    for(i = 0; i < tbl->t_nc; i++)
        read_cell(cell + i, yy, 0);
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
    int     keyw, i;
    TLINE   *c_line;
    TCELL   *cell;

    if(T_add_line(tbl) < 0) return(-1);
    c_line = tbl->t_line + (tbl->t_nl - 1);

    while(1) {
        keyw = YY_lex(yy);
        switch(keyw) {
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
                if(c_line->tl_type != 0) {
                    YY_unread(yy);
                    return(0);
                }
                c_line->tl_type = keyw;
                break;

            case TABLE_ASCII_LINE_TITLE :
                if(c_line->tl_type != 0) {
                    YY_unread(yy);
                    return(0);
                }
                c_line->tl_type = keyw;
                c_line->tl_val = SW_nalloc(sizeof(TCELL));
                read_cell((TCELL *) c_line->tl_val, yy, YY_STRING);
                break;

            case TABLE_ASCII_BREAK :
                if(c_line->tl_type != 0) {
                    YY_unread(yy);
                    return(0);
                }
                c_line->tl_type = TABLE_ASCII_LINE_TITLE; /* empty string */
                c_line->tl_val = SW_nalloc(sizeof(TCELL));
                YY_unread(yy);
                return(0);

            case TABLE_ASCII_LEFT_AXIS :
                c_line->tl_axis = 0;
                break;
            case TABLE_ASCII_RIGHT_AXIS :
                c_line->tl_axis = 1;
                break;

            case TABLE_ASCII_GRAPH_LINE :
                c_line->tl_graph= 0;
                break;
            case TABLE_ASCII_GRAPH_SCATTER:
                c_line->tl_graph= 1;
                break;
            case TABLE_ASCII_GRAPH_BAR  :
                c_line->tl_graph= 2;
                break;

            default       :
                if(c_line->tl_type != 0) {
                    YY_unread(yy);
                    return(0);
                }
                YY_unread(yy);
                cell = T_create_cell(tbl, c_line);
                for(i = 0; i < T_NC(tbl); i++)  read_cell(cell + i, yy, 0);
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
    if(keyw != TABLE_ASCII_OPEN) {
        kerror(0, "%s: Expected { - Got %s ", yy->yy_text, YY_error(yy));
        return(NULL);
    }

    keyw = YY_lex(yy);
    if(keyw != TABLE_ASCII_DIM) YY_unread(yy);
    else {
        if(YY_lex(yy) != YY_LONG)  YY_unread(yy);
        else dim = yy->yy_long;
    }

    tbl = T_create(dim);
    if(tbl == NULL) return(NULL);

    while(1) {
        keyw = YY_lex(yy);
        switch(keyw) {
            case TABLE_ASCII_CLOSE   :
                return(tbl);

            case TABLE_ASCII_DUTCH   :
            case TABLE_ASCII_FRENCH  :
            case TABLE_ASCII_ENGLISH :
                tbl->t_lang = keyw;
                break;

            case TABLE_ASCII_DIM     :
                kerror(0, "Double dim definition skipped : %s", YY_error(yy));
                break;

            case TABLE_ASCII_DIVIDER     :
                read_div(tbl, yy);
                break;

            case TABLE_ASCII_BOX     :
                tbl->t_box = (char)K_read_long(yy);
                break;
            case TABLE_ASCII_AXIS     :
                tbl->t_axis = (char)K_read_long(yy);
                break;
            case TABLE_ASCII_XGRID    :
                tbl->t_gridx = (char)K_read_long(yy);
                break;
            case TABLE_ASCII_YGRID    :
                tbl->t_gridy = (char)K_read_long(yy);
                break;

            case TABLE_ASCII_YMIN     :
                tbl->t_ymin  = (float)K_read_real(yy);
                break;
            case TABLE_ASCII_YMAX     :
                tbl->t_ymax  = (float)K_read_real(yy);
                break;
            case TABLE_ASCII_ZMIN     :
                tbl->t_zmin  = (float)K_read_real(yy);
                break;
            case TABLE_ASCII_ZMAX     :
                tbl->t_zmax  = (float)K_read_real(yy);
                break;

            case TABLE_ASCII_ALIGN:
                tbl->t_align = K_read_align(yy);
                break;

            case TABLE_ASCII_BREAK   :
                if(read_line(tbl, yy)< 0) {
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

    if(sorted == 0) {
        qsort(TABLE, sizeof(TABLE) / sizeof(YYKEYS), sizeof(YYKEYS), ascii_compare);
        sorted = 1;
    }

    SCR_strip((unsigned char*) filename);
    yy = YY_open(filename, TABLE, sizeof(TABLE) / sizeof(YYKEYS),
                 (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);
    if(yy == 0) {
        kerror(0, "Cannot open '%s'", filename);
        return(kdb);
    }

    /* READ FILE */
    kdb = K_create(TABLES, UPPER_CASE);
    K_set_kdb_fullpath(kdb, (U_ch*)filename); // JMP 30/11/2022
    
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                if(cmpt) {
                    char    asc_filename[1024];
                    K_set_ext_asc(asc_filename, filename, TABLES);
                    K_set_kdb_fullpath(kdb, (U_ch*)asc_filename); // JMP 03/12/2022
                }
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                if((tbl = read_tbl(yy)) == NULL) {
                    kerror(0, "%s : table defined", YY_error(yy));
                    goto err;
                }
                if(K_add(kdb, name, tbl) < 0)  goto err;
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
void align(FILE* fd, int align)
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
static void grinfo(FILE *fd, int axis, int type)
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
static void print_axis(FILE* fd, char* str, float value)
{
    if(IODE_IS_A_NUMBER(value)) fprintf(fd, "\n%s %f ", str, value);
}

/**
 *  Prints the font attribute and the alignement of a cell. 
 *  
 *  @param [in] fd      FILE*     output stream
 *  @param [in] attr    int       cell alignment and font attribute
 *  @return             void
 *  
 */
static void print_attr(FILE* fd, int attr)
{
    if(attr & TABLE_CELL_BOLD)      fprintf(fd, "BOLD ");
    if(attr & TABLE_CELL_ITALIC)    fprintf(fd, "ITALIC ");
    if(attr & TABLE_CELL_UNDERLINE) fprintf(fd, "UNDERLINE ");

    if(attr & TABLE_CELL_CENTER)    fprintf(fd, "CENTER ");
    if(attr & TABLE_CELL_DECIMAL)   fprintf(fd, "DECIMAL ");
    if(attr & TABLE_CELL_LEFT)      fprintf(fd, "LEFT ");
    if(attr & TABLE_CELL_RIGHT)     fprintf(fd, "RIGHT ");
    return;
}

/**
 *  Prints a table cell definition. 
 *  
 *  @param [in] fd      FILE*       output stream
 *  @param [in] cell    TCELL*      cell to print
 *  @return             void
 *  
 */
static void print_cell(FILE *fd, TCELL *cell)
{
    if((cell->content) == NULL) {
        fprintf(fd, "\"\" ");
        return;
    }
    switch(cell->type) {
        case TABLE_ASCII_CELL_STRING :
            //fprintf(fd, "\"%s\" ", cell->content);
            // JMP 8/4/2015 -> escape "
            SCR_fprintf_esc(fd, cell->content, 1);
            fprintf(fd, " ");
            break;

        case TABLE_ASCII_CELL_LEC :
            fprintf(fd, "LEC \"%s\" ", (char *)P_get_ptr(cell->content, 0));
            break;
        default :
            fprintf(fd, "\"\" ");
            break;
    }
    if(cell->content[0] != '\0') 
        print_attr(fd, cell->attribute);
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
    TCELL   *cell;
    int     i, j;

    /* tbl */
    fprintf(fd, "\nDIM %d\n", T_NC(tbl));
    K_wrdef(fd, TABLE, T_LANG(tbl));
    /* GB 2/3/00 */
    fprintf(fd, "\nBOX %d AXIS %d XGRID %d YGRID %d ",
            tbl->t_box, tbl->t_axis, tbl->t_gridx, tbl->t_gridy);
    align(fd, tbl->t_align);
    print_axis(fd, "YMIN", tbl->t_ymin);
    print_axis(fd, "YMAX", tbl->t_ymax);
    print_axis(fd, "ZMIN", tbl->t_zmin);
    print_axis(fd, "ZMAX", tbl->t_zmax);

    fprintf(fd, "\nDIV ");
    /* div */
    for(i = 0; i < T_NC(tbl); i++)
        print_cell(fd, (TCELL *)(tbl->t_div.tl_val) + i);

    /* lines */

    for(j = 0; j < T_NL(tbl); j++) {
        fprintf(fd, "\n- ");
        switch(tbl->t_line[j].tl_type) {
            case TABLE_ASCII_LINE_CELL :
                cell = (TCELL *) tbl->t_line[j].tl_val;
                for(i = 0; i < T_NC(tbl); i++)
                    print_cell(fd, cell + i);

                /* append GR info */
                grinfo(fd, tbl->t_line[j].tl_axis, tbl->t_line[j].tl_graph);
                break;

            case TABLE_ASCII_LINE_TITLE :
                K_wrdef(fd, TABLE, tbl->t_line[j].tl_type);
                print_cell(fd, (TCELL *) tbl->t_line[j].tl_val);
                break;

            case TABLE_ASCII_LINE_SEP   :
            case TABLE_ASCII_BOLD_LINE  :
            case TABLE_ASCII_LINE_MODE  :
            case TABLE_ASCII_LINE_DATE  :
            case TABLE_ASCII_LINE_FILES :
                K_wrdef(fd, TABLE, tbl->t_line[j].tl_type);
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

    for(i = 0 ; i < KNB(kdb); i++) {
        fprintf(fd, "%s {", KONAME(kdb, i));
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
