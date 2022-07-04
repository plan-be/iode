/**
 *  @header4iode
 *  
 *  Functions to generate IODE tables in A2M format based on TBL structures and GSAMPLE definition.
 *  Includes some A2M helper functions. 
 *  
 *  Note that the functions needed to generate graphs from tables can be found in k_graph.c.
 *  
 *  List of functions 
 *  -----------------
 *      int T_prep_cls(TBL* tbl, char* smpl, COLS** cls)            Compiles a GSAMPLE into a COLS struct and resizes COLS according to the nb of cols in TBL
 *      void T_fmt_val(char* buf, IODE_REAL val, int lg, int nd)    Formats a double value
 *      void T_print_val(IODE_REAL val)                             Prints a IODE_REAL value using W_printf()
 *      void T_open_cell(int attr, int straddle, int type)          Prints the header of an a2m table cell
 *      void T_open_attr(int attr)                                  Opens an A2M attribute sequence.
 *      void T_close_attr(int attr)                                 Closes an A2M attribute sequence.
 *      void T_print_cell(TCELL* cell, COL* cl, int straddle)       Prints a TBL cell on a specific GSAMPLE column. 
 *      char **T_find_files(COLS* cls)                              Retrieves the filenames used in the COLS (from GSAMPLE) needed to print the special table line KT_FILES.
 *      unsigned char *T_get_title(TBL* tbl)                        Retrieves a TBL title, i.e. the contents of the first line of type KT_TITLE
 *      int T_print_tbl(TBL* tbl, char* smpl)                       Computes a table on a GSAMPLE and saves the result in A2M format
 *  
 *  Global variables
 *  ----------------
 *      char     KT_sep = '&';                                      Table cell separator
 *      int      K_NBDEC = -1;                                      Default nb of decimals
 *  
 */
 
#include "iode.h"

char     KT_sep = '&';          // Table cell separator
int      K_NBDEC = -1;          // Default nb of decimals

char     **KT_names = NULL;     // Names of the files used in a GSAMPLE
int      KT_nbnames = 0;        // Number of names in KT_names
int      KT_mode[MAX_MODE];     // Modes used in a GSAMPLE
int      KT_CUR_TOPIC = 0;      // Used in A2M file generation
int      KT_CUR_LEVEL = 0;      // Used in A2M file generation



/**
 *  Compiles a GSAMPLE into a COLS struct and resizes COLS according to the nb of cols in TBL.
 *  
 *  @param [in] TBL*   tbl      table to compute
 *  @param [in] char*  smpl     GSAMPLE 
 *  @param [in] COLS** cls      result = column definition for computing of the table
 *  @return     int             total number of columns for the computed table
 *  
 */
int T_prep_cls(TBL* tbl, char* smpl, COLS** cls)
{
    int     dim;

    *cls = COL_cc(smpl);
    if(*cls == NULL) {
        B_seterror("Sample %.80s : syntax error", smpl);
        return(-1);
    }

    dim = COL_resize(tbl, *cls);
    return(dim);
}

/**
 *  Formats a double value. Uses SCR_fmt_dbl(). See http://xon.be/scr4/libs1/libs1167.htm.
 *  
 *  @param [in, out]    char*       buf     placeholder of the result
 *  @param [in]         IODE_REAL   val     input real value
 *  @param [in]         int         lg      max result string length
 *  @param [in]         int         nd      number of decimal places 
 *  
 */
 
void T_fmt_val(char* buf, IODE_REAL val, int lg, int nd) 
{
    if(L_ISAN(val)) SCR_fmt_dbl(val, buf, lg, nd);
    else strcpy(buf, "-.-");
    SCR_sqz(buf);
}


/**
 *  Prints a IODE_REAL value using W_printf().
 *  
 *  @param  [in] IODE_REAL  val      value to print
 *  @global [in] int        K_NBDEC  number of decimal places
 *  
 */
 
void T_print_val(IODE_REAL val)
{
    char    buf[64];

    T_fmt_val(buf, val, 30, K_NBDEC);   // JMP 18-04-2022
    W_printf(buf);
}


/**
 *  Translates a TCELL of type KT_TEXT into a text using COL_text(). Sends the result to W_printf().
 *  
 *  @param  [in] cl         the column of the GSAMPLE to be printed (period, file nb, operation...)
 *  @param  [in] string     the table column definition (ex "#s")  
 *  @global [in] KT_nbnames number of compared files in the COLS definition
 *  
 */
 
static void T_print_string(COL* cl, char* string)
{
    char   *ptr = NULL;
    char    *COL_text();

    ptr = (char *) COL_text(cl, string, KT_nbnames);
    if(ptr != NULL) W_printf("%s", ptr);
    SW_nfree(ptr);
}


/**
 *  Prints the header of an a2m table cell: <cellsep><span><align>.
 *  Example: "@2C" if @ is the cell separator, 2 the number of spanned columns and the text must be centered in the column.
 *   
 *  @param  [in] int attr        alignment attribute: KT_CENTER, KT_RIGHT, KT_DECIMAL, KT_LEFT.
 *  @param  [in] int straddle    number of spanned columns
 *  @param  [in] int type        column type (KT_STRING, KT_LEC...)
 *  @global [in] int KT_sep      a2m table cell separator
 *  
 */
 
void T_open_cell(int attr, int straddle, int type) /* JMP 17-12-93 */
{
    char    align = 'L';

    if(attr & KT_CENTER)    align = 'C';
    if(attr & KT_RIGHT)     align = 'R';
    if(type != KT_STRING && (attr & KT_DECIMAL))   align = 'D'; /* JMP 17-12-93 */

    W_printf("%c%d%c", KT_sep, straddle, align);
}


/**
 *  Opens an A2M attribute sequence.
 *  
 *  @param [in] int     attr    Cell attribute: KT_BOLD...
 *  
 */
void T_open_attr(int attr)
{

    if(attr & KT_BOLD)      W_printf("\\b");
    if(attr & KT_ITALIC)    W_printf("\\i");
    if(attr & KT_UNDERLINE) W_printf("\\u");
}


/**
 *  Closes an A2M attribute sequence.
 *  
 *  @param [in] int     attr    Cell attribute: KT_BOLD...
 *  
 */
void T_close_attr(int attr)
{
    if(attr & KT_BOLD)      W_printf("\\B");
    if(attr & KT_ITALIC)    W_printf("\\I");
    if(attr & KT_UNDERLINE) W_printf("\\U");
}


/**
 *  Prints a TBL cell on a specific GSAMPLE column. 
 *  
 *  @param [in] TCELL*  cell        table cell to print
 *  @param [in] COL*    cl          GSAMPLE column definition with the value already calculated
 *  @param [in] int     straddle    nb of spanned columns int the resulting a2m table 
 *  
 */
void T_print_cell(TCELL* cell, COL* cl, int straddle)
{
    if(cell == 0 || cell->tc_val == 0) {                /* JMP 24-06-98 */
        W_printf("%c1R", KT_sep);  /* JMP 24-06-98 */
        return;                 /* JMP 24-06-98 */
    }
    if(cell->tc_type == KT_STRING && U_is_in('#', cell->tc_val))
        //cell->tc_attr = KT_ALIGN(cell->tc_attr, KT_CENTER); /* JMP 05-01-02 */
        cell->tc_attr = KT_ALIGN(cell->tc_attr, KT_RIGHT); /* JMP 05-01-02 */
    if(cell->tc_type == KT_LEC)
        cell->tc_attr = KT_ALIGN(cell->tc_attr, KT_DECIMAL);

    T_open_cell(cell->tc_attr, straddle, cell->tc_type);  /* JMP 17-12-93 */
    T_open_attr(cell->tc_attr);

    if(cell->tc_type != 0) {
        if(cl == NULL || cell->tc_type == KT_STRING) {
            T_print_string(cl, cell->tc_val);
        }
        else T_print_val(cl->cl_res);
    }

    T_close_attr(cell->tc_attr);
}


/**
 *  Prints one table line for all columns defined in cls.
 *  
 *  @param [in] TBL*    tbl     source table
 *  @param [in] int     i       line to print
 *  @param [in] COLS*   cls     columns to print = compiled GSAMPLE
 *  @return     int             0 on success, -1 on error.        
 *  
 */

static int T_print_line(TBL* tbl, int i, COLS* cls)
{
    int     j, d;
    TLINE   *line = T_L(tbl) + i;
    TCELL   *cell = (TCELL *) line->tl_val;

    COL_clear(cls);
    if(COL_exec(tbl, i, cls) < 0)   return(-1);

    for(j = 0; j < cls->cl_nb; j++) {
        d = j % T_NC(tbl);
        if(tbl->t_free == 0 && d == 0 && j != 0) continue;
        T_print_cell(cell + d, cls->cl_cols + j, 1);
    }
    W_printf("\n");

    return(0);
}


/**
 *  Retrieves the filenames used in the COLS (from GSAMPLE) needed to print the special table line KT_FILES. 
 *   
 *  @param [in] COLS*   cls     list of columns (from GSAMPLE)
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
    for(i = 0; i < cls->cl_nb; i++) {
        cl = cls->cl_cols + i;
        files[cl->cl_fnb[0]] = 1;
        files[cl->cl_fnb[1]] = 1;
    }

    for(i = 1; i < K_MAX_FREF + 1; i++) {
        if(files[i] == 0) continue;
        kdb = K_RWS[K_VAR][i - 1];
        if(kdb == NULL) {
            B_seterror("File %d not present", i);
            SCR_add_ptr(&names, &nf, 0L);
            SCR_free_tbl(names);
            return(NULL);
        }
        sprintf(buf, "[%d] %s", i, KNAMEPTR(kdb));
        SCR_replace(buf, "\\", "/");
        //B_path_change(buf);
        SCR_add_ptr(&names, &nf, buf);
    }
    SCR_add_ptr(&names, &nf, 0L);
    return(names);
}

/**
 *  Prints the special TBL line of type KT_FILES.
 *  
 *  @param  [in] COLS*  cls         columns to print = compiled GSAMPLE
 *  @param  [in] int    dim         total number of columns in the resulting table (size of GSAMPLE x nb table cols)
 *  @global [in] char** KT_names    list of formatted filenames
 *  @global [in] int    KT_nbnames  number if filenames
 *  
 */
 
static void T_print_files(COLS* cls, int dim)
{
    int     i;

    if(KT_nbnames <= 0) return;  

    for(i = 0; KT_names[i]; i++) {
        T_open_cell(KT_LEFT, dim, KT_STRING); /* JMP 17-12-93 */
        W_printf("%s\n", KT_names[i]);
    }
}


/**
 *  Prints the special TBL line of type KT_MODE (growth rates, diff...).
 *  
 *  @param  [in] COLS*  cls         columns to print = compiled GSAMPLE
 *  @param  [in] int    dim         total number of columns in the resulting table (size of GSAMPLE x nb table cols)
 *  @global [in] char** KT_mode     list of modes used in COLS (computed in T_begin_tbl())
 *  
 */

static void T_print_mode(COLS* cls, int dim)
{
    int    i;
    extern char *KLG_OPERS_TEXTS[][3];

    for(i = 0; i < MAX_MODE; i++) {
        if(KT_mode[i] == 0) continue;
        T_open_cell(KT_LEFT, dim, KT_STRING);  /* JMP 17-12-93 */
        //W_printf("(%s) %s\n", COL_OPERS[i + 1], KLG_OPERS_TEXTS[i + 1][B_LANG]);
        W_printf("(%s) %s\n", COL_OPERS[i + 1], KLG_OPERS_TEXTS[i + 1][K_LANG]); // JMP 18-04-2022
    }
}


/**
 *  Prints the special TBL line of type KT_DATE.
 *  
 *  @param  [in] int    dim   total number of columns in the resulting table (size of GSAMPLE x nb table cols)
 *  
 */

static void T_print_date(int dim)
{
    long    SCR_current_date();
    char    date[11];

    SCR_long_to_fdate(SCR_current_date(), date, "dd/mm/yy");
    T_open_cell(KT_LEFT, dim, KT_STRING); /* JMP 17-12-93 */
    W_printf("%s\n", date);
}


/**
 *  Prints a table header in A2M.
 *  Initialises globals KT_names, KT_nbnames and KT_mode.
 *  
 *  @param [in] int     dim     total number of columns in the resulting table (size of GSAMPLE x nb table cols)
 *  @param [in] COLS*   cls     columns to print = compiled GSAMPLE
 *  @return 
 *  
 */
 
static int T_begin_tbl(int dim, COLS* cls)
{
    KT_names = T_find_files(cls);
    KT_nbnames = SCR_tbl_size(KT_names);
    if(KT_nbnames == 0) return(-1); /* JMP 11-06-99 */
    COL_find_mode(cls, KT_mode, 2);

    W_printf(".tb %d\n", dim);
    if(KT_sep == '\t') W_printf(".sep TAB\n");
    else W_printf(".sep %c\n", KT_sep);
    return(0);
}


/**
 *  Prints a table footer in A2M and frees the temporary allocated variables. 
 */

static void T_end_tbl()
{
    W_printf(".te \n");
    SCR_free_tbl(KT_names);
    KT_names = NULL;
    KT_nbnames = 0;
}


/**
 *  Retrieves a TBL title, i.e. the contents of the first line of type KT_TITLE.
 *  
 *  @param [in] TBL*    tbl     pointer to a table
 *  @return     char*           local static buffer with the contents of the title or the text "No title"
 *                              if no line of type KT_TITLE can be found or if the first line of that type is empty.
 */

unsigned char *T_get_title(TBL* tbl)  
{
    int                     k;
    static unsigned char    buf[256];
    
    for(k = 0; k < T_NL(tbl); k++)
        if(tbl->t_line[k].tl_type == KT_TITLE) break;
    
// OLD VERSION USING STATIC_BUF
//    if(k == T_NL(tbl) ||
//            ((TCELL *) tbl->t_line[k].tl_val)->tc_val == 0)
//        strcpy(STATIC_BUF, "No title");
//    else
//        SCR_strlcpy(STATIC_BUF, (char *)((TCELL *) tbl->t_line[k].tl_val)->tc_val, STATIC_BUF_LG);
//
//    return(STATIC_BUF);


// New version using local static buffer to solve link problems // JMP 11/04/2022
    if(k == T_NL(tbl) ||
            ((TCELL *) tbl->t_line[k].tl_val)->tc_val == 0)
        strcpy(buf, "No title");
    else
        SCR_strlcpy(buf, (char *)((TCELL *) tbl->t_line[k].tl_val)->tc_val, sizeof(buf) - 1);

    return(buf);
}


/**
 *  Computes a table on a GSAMPLE and saves the result in A2M format (see https://iode.plan.be/doku.php?id=le_langage_a2m for
 *  the syntax of a2m files).
 *  
 *  @param [in] TBL*    tbl     table to print
 *  @param [in] char*   smpl    GSAMPLE on which the tbl must be computed
 *  @return     int             0 on success, -1 on error (smpl syntax error, files not defined...) 
 *  
 */
int T_print_tbl(TBL* tbl, char* smpl)
{
    int     i, dim, rc = 0, first = 1;
    COLS    *cls;
    TLINE   *line;
    unsigned char *T_get_title();

    dim = T_prep_cls(tbl, smpl, &cls);
    if(dim < 0) return(-1);

    // Anciennement
    // B_PrintRtfTopic(T_get_title(tbl));
    // Nouveau JMP 18/04/2022
    W_printf( ".topic %d %d %s\n", KT_CUR_TOPIC++, KT_CUR_LEVEL, T_get_title(tbl));
    //if(W_type == A2M_DESTRTF && W_rtfhelp) W_printf(".par1 tit_%d\n%s\n\n", KT_CUR_LEVEL, T_get_title(tbl));
    
    if(T_begin_tbl(dim, cls)) return(-1);
    W_printf(".ttitle %s\n", T_get_title(tbl));  /* JMP 27-02-98 */
    for(i = 0; rc == 0 && i < T_NL(tbl); i++) {
        line = T_L(tbl) + i;

        switch(line->tl_type) {
            case KT_LINE  :
                W_printf(".tl\n");
                break;
            case KT_TITLE :
                if(first) {
                    first = 0;
                    break;
                }
                T_print_cell((TCELL *) line->tl_val, NULL, dim);
                W_printf("\n");
                break;
            case KT_DATE  :
                T_print_date(dim);
                break;
            case KT_MODE  :
                T_print_mode(cls, dim);
                break;
            case KT_FILES :
                T_print_files(cls, dim);
                break;
            case KT_CELL  :
                if(T_print_line(tbl, i, cls) < 0) {
                    rc = -1;
                    B_seterror("Unable to print line %d ", i);
                }
                break;
        }
    }
    T_end_tbl();

    COL_free_cols(cls);
    return(rc);
}
