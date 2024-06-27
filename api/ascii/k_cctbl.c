/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE TBL objects.
 *
 *      KDB *KT_load_asc(char* filename)
 *      int KT_save_asc(KDB* kdb, char* filename)
 *      int KT_save_csv(KDB *kdb, char *filename)
 */

#include "iode.h"

/**
 * Table of keywords recognized by YY in the context of a TBLs ascii file (.at).
 * See s_yy function group for more informations (http://www.xon.be/scr4/libs1/libs157.htm).
 */

YYKEYS KT_TABLE[] = {
    "BOLD",         KT_BOLD,
    "UNDERLINE",    KT_UNDERLINE,
    "CENTER",       KT_CENTER,
    "DATE",         KT_DATE,
    "DECIMAL",      KT_DECIMAL,
    "DIM",          KT_DIM,
    "DIV",          KT_DIV,
    "DUTCH",        KT_DUTCH,
    "ENGLISH",      KT_ENGLISH,
    "FILES",        KT_FILES,
    "FRENCH",       KT_FRENCH,
    "ITALIC",       KT_ITALIC,
    "LEC",          KT_LEC,
    "LEFT",         KT_LEFT,
    "LINE",         KT_LINE,
    "LINE BOLD",    KT_DLINE,
    "MODE",         KT_MODE,
    "NORMAL",       KT_NORMAL,
    "RIGHT",        KT_RIGHT,
    "TITLE",        KT_TITLE,
    "{",            KT_OPEN,
    "}",            KT_CLOSE,
    "-",            KT_BREAK, /* GB 2/3/2000 */
    "YMIN",         KT_YMIN,
    "YMAX",         KT_YMAX,
    "ZMIN",         KT_ZMIN,
    "ZMAX",         KT_ZMAX,
    "XGRID",        KT_XGRID,
    "YGRID",        KT_YGRID,
    "BOX",          KT_BOX,
    "AXIS",         KT_AXIS,
    "ALIGN",        KT_ALGN,
    "LAXIS",        KT_LAXIS,
    "RAXIS",        KT_RAXIS,
    "GRLINE",       KT_GRLINE,
    "GRBAR",        KT_GRBAR,
    "GRSCATTER",    KT_GRSCATTER
};


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

static void KT_read_cell(TCELL* cell, YYFILE* yy, int mode)
{
    int     keyw, ok = 0, align = KT_LEFT;

    keyw = YY_lex(yy);
    if(mode != 0 && mode != keyw) {
        YY_unread(yy);
        return;
    }

    cell->tc_type = KT_STRING;
    while(1) {
        switch(keyw) {
            case KT_RIGHT  :
            case KT_LEFT   :
            case KT_CENTER :
            case KT_DECIMAL :
                align = keyw;
                break;
            case KT_ITALIC :
            case KT_BOLD   :
            case KT_UNDERLINE:
            case KT_NORMAL :
                cell->tc_attr |= keyw;
                break;

            case KT_LEC    :
                if(ok == 1) goto ret;
                ok = 1;
                if(YY_lex(yy) != YY_STRING) {
                    YY_unread(yy);
                    break;
                }
                T_free_cell(cell);
                if(K_ipack(&(cell->tc_val), yy->yy_text) < 0)
                    cell->tc_val = NULL;
                cell->tc_type = KT_LEC;
                align = KT_DECIMAL;
                break;

            case YY_STRING :
                if(ok == 1) goto ret;
                ok = 1;
                T_free_cell(cell);
                /*            cell->tc_attr = KT_LEFT; */
                if(U_is_in('#', yy->yy_text)) cell->tc_attr = KT_CENTER;
                K_stracpy(&(cell->tc_val), yy->yy_text);
                cell->tc_type = KT_STRING;
                break;


            case YY_EOF   :
            case KT_BREAK :
            default       :
                goto ret;
        }
        keyw = YY_lex(yy);
    }

ret :
    cell->tc_attr |= align;
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

static void KT_read_div(TBL* tbl, YYFILE* yy)
{
    int     i;
    TCELL   *cell;

    cell = (TCELL *) (tbl->t_div).tl_val;
    for(i = 0; i < tbl->t_nc; i++)
        KT_read_cell(cell + i, yy, 0);
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

static int KT_read_line(TBL* tbl, YYFILE* yy)
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

            case KT_RIGHT  :
            case KT_LEFT   :
            case KT_CENTER :
            case KT_DECIMAL:
            case KT_ITALIC :
            case KT_BOLD   :
            case KT_UNDERLINE:
            case KT_NORMAL :
                kerror(0, "Attribute definition %s, skipped", YY_error(yy));
                break;

            case KT_LINE  :
            case KT_MODE  :
            case KT_DATE  :
            case KT_FILES :
                if(c_line->tl_type != 0) {
                    YY_unread(yy);
                    return(0);
                }
                c_line->tl_type = keyw;
                break;

            case KT_TITLE :
                if(c_line->tl_type != 0) {
                    YY_unread(yy);
                    return(0);
                }
                c_line->tl_type = keyw;
                c_line->tl_val = SW_nalloc(sizeof(TCELL));
                KT_read_cell((TCELL *) c_line->tl_val, yy, YY_STRING);
                break;

            case KT_BREAK :
                if(c_line->tl_type != 0) {
                    YY_unread(yy);
                    return(0);
                }
                c_line->tl_type = KT_TITLE; /* empty string */
                c_line->tl_val = SW_nalloc(sizeof(TCELL));
                YY_unread(yy);
                return(0);

            case KT_LAXIS :
                c_line->tl_axis = 0;
                break;
            case KT_RAXIS :
                c_line->tl_axis = 1;
                break;

            case KT_GRLINE :
                c_line->tl_graph= 0;
                break;
            case KT_GRSCATTER:
                c_line->tl_graph= 1;
                break;
            case KT_GRBAR  :
                c_line->tl_graph= 2;
                break;

            default       :
                if(c_line->tl_type != 0) {
                    YY_unread(yy);
                    return(0);
                }
                YY_unread(yy);
                cell = T_create_cell(tbl, c_line);
                for(i = 0; i < T_NC(tbl); i++)  KT_read_cell(cell + i, yy, 0);
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
static TBL *KT_read_tbl(YYFILE* yy)
{
    int     keyw, dim = 2;
    TBL     *tbl = NULL;

    keyw = YY_lex(yy);
    if(keyw != KT_OPEN) {
        kerror(0, "%s: Expected { - Got %s ", yy->yy_text, YY_error(yy));
        return(NULL);
    }

    keyw = YY_lex(yy);
    if(keyw != KT_DIM) YY_unread(yy);
    else {
        if(YY_lex(yy) != YY_LONG)  YY_unread(yy);
        else dim = yy->yy_long;
    }

    tbl = T_create(dim);
    if(tbl == NULL) return(NULL);

    while(1) {
        keyw = YY_lex(yy);
        switch(keyw) {
            case KT_CLOSE   :
                return(tbl);

            case KT_DUTCH   :
            case KT_FRENCH  :
            case KT_ENGLISH :
                tbl->t_lang = keyw;
                break;

            case KT_DIM     :
                kerror(0, "Double dim definition skipped : %s", YY_error(yy));
                break;

            case KT_DIV     :
                KT_read_div(tbl, yy);
                break;

            case KT_BOX     :
                tbl->t_box = (char)K_read_long(yy);
                break;
            case KT_AXIS     :
                tbl->t_axis = (char)K_read_long(yy);
                break;
            case KT_XGRID    :
                tbl->t_gridx = (char)K_read_long(yy);
                break;
            case KT_YGRID    :
                tbl->t_gridy = (char)K_read_long(yy);
                break;

            case KT_YMIN     :
                tbl->t_ymin  = (float)K_read_real(yy);
                break;
            case KT_YMAX     :
                tbl->t_ymax  = (float)K_read_real(yy);
                break;
            case KT_ZMIN     :
                tbl->t_zmin  = (float)K_read_real(yy);
                break;
            case KT_ZMAX     :
                tbl->t_zmax  = (float)K_read_real(yy);
                break;

            case KT_ALGN:
                tbl->t_align = K_read_align(yy);
                break;

            case KT_BREAK   :
                if(KT_read_line(tbl, yy)< 0) {
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

KDB *KT_load_asc(char* filename)
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
        qsort(KT_TABLE, sizeof(KT_TABLE) / sizeof(YYKEYS), sizeof(YYKEYS), YY_strcmp);
        sorted = 1;
    }

    SCR_strip(filename);
    yy = YY_open(filename, KT_TABLE, sizeof(KT_TABLE) / sizeof(YYKEYS),
                 (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);
    if(yy == 0) {
        kerror(0, "Cannot open '%s'", filename);
        return(kdb);
    }

    /* READ FILE */
    kdb = K_create(K_TBL, UPPER_CASE);
    K_set_kdb_fullpath(kdb, (U_ch*)filename); // JMP 30/11/2022
    
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                if(cmpt) {
                    char    asc_filename[1024];
                    K_set_ext_asc(asc_filename, filename, K_TBL);
                    K_set_kdb_fullpath(kdb, (U_ch*)asc_filename); // JMP 03/12/2022
                }
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, yy->yy_text);
                if((tbl = KT_read_tbl(yy)) == NULL) {
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
 
void KT_align(FILE* fd, int align)
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
 
static void KT_grinfo(fd, axis, type)
FILE *fd;
char axis, type;
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
 
static void KT_min(FILE* fd, char* str, float value)
{
    if(L_ISAN(value)) fprintf(fd, "\n%s %f ", str, value);
}


/**
 *  Prints the font attribute and the alignement of a cell. 
 *  
 *  @param [in] fd      FILE*     output stream
 *  @param [in] attr    int       cell alignment and font attribute
 *  @return             void
 *  
 */
static void KT_print_attr(FILE* fd, int attr)
{
    if(attr & KT_BOLD)      fprintf(fd, "BOLD ");
    if(attr & KT_ITALIC)    fprintf(fd, "ITALIC ");
    if(attr & KT_UNDERLINE) fprintf(fd, "UNDERLINE ");

    if(attr & KT_CENTER)    fprintf(fd, "CENTER ");
    if(attr & KT_DECIMAL)   fprintf(fd, "DECIMAL ");
    if(attr & KT_LEFT)      fprintf(fd, "LEFT ");
    if(attr & KT_RIGHT)     fprintf(fd, "RIGHT ");
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
static void KT_print_cell(FILE *fd, TCELL *cell)
{
    if((cell->tc_val) == NULL) {
        fprintf(fd, "\"\" ");
        return;
    }
    switch(cell->tc_type) {
        case KT_STRING :
            //fprintf(fd, "\"%s\" ", cell->tc_val);
            // JMP 8/4/2015 -> escape "
            SCR_fprintf_esc(fd, cell->tc_val, 1);
            fprintf(fd, " ");
            break;

        case KT_LEC :
            fprintf(fd, "LEC \"%s\" ", (char *)P_get_ptr(cell->tc_val, 0));
            break;
        default :
            fprintf(fd, "\"\" ");
            break;
    }
    if(cell->tc_val[0] != '\0') 
        KT_print_attr(fd, cell->tc_attr);
}


/**
 *  Prints a TBL definition.
 *  
 *  @param [in] fd      FILE*     output stream
 *  @param [in] tbl     TBL*      table to print
 *  @return             void
 *  
 */

static void KT_print_tbl(FILE* fd, TBL* tbl)
{
    TCELL   *cell;
    int     i, j;

    /* tbl */
    fprintf(fd, "\nDIM %d\n", T_NC(tbl));
    K_wrdef(fd, KT_TABLE, T_LANG(tbl));
    /* GB 2/3/00 */
    fprintf(fd, "\nBOX %d AXIS %d XGRID %d YGRID %d ",
            tbl->t_box, tbl->t_axis, tbl->t_gridx, tbl->t_gridy);
    KT_align(fd, tbl->t_align);
    KT_min(fd, "YMIN", tbl->t_ymin);
    KT_min(fd, "YMAX", tbl->t_ymax);
    KT_min(fd, "ZMIN", tbl->t_zmin);
    KT_min(fd, "ZMAX", tbl->t_zmax);

    fprintf(fd, "\nDIV ");
    /* div */
    for(i = 0; i < T_NC(tbl); i++)
        KT_print_cell(fd, (TCELL *)(tbl->t_div.tl_val) + i);

    /* lines */

    for(j = 0; j < T_NL(tbl); j++) {
        fprintf(fd, "\n- ");
        switch(tbl->t_line[j].tl_type) {
            case KT_CELL :
                cell = (TCELL *) tbl->t_line[j].tl_val;
                for(i = 0; i < T_NC(tbl); i++)
                    KT_print_cell(fd, cell + i);

                /* append GR info */
                KT_grinfo(fd, tbl->t_line[j].tl_axis, tbl->t_line[j].tl_graph);
                break;

            case KT_TITLE :
                K_wrdef(fd, KT_TABLE, tbl->t_line[j].tl_type);
                KT_print_cell(fd, (TCELL *) tbl->t_line[j].tl_val);
                break;

            case KT_LINE  :
            case KT_DLINE  :
            case KT_MODE  :
            case KT_DATE  :
            case KT_FILES :
                K_wrdef(fd, KT_TABLE, tbl->t_line[j].tl_type);
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
 *  @see KT_load_asc() for the syntax of the ascii tables. 
 *  
 *  @param [in] kdb         KDB*    KDB of TBLs
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
int KT_save_asc(KDB* kdb, char* filename)
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
        KT_print_tbl(fd, tbl);
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

int KT_save_csv(KDB *kdb, char *filename)
{
    return(-1);
}
