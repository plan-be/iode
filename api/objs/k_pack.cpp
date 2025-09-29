/**
 * @author Jean-Marc PAUL 
 * @author Geert BRYON
 * 
 * Functions for "packing" and "unpacking" IODE objects.
 * 
 * IODE stores objects in a "packed" form both in memory and on disk. 
 *   - to minimize memory usage
 *   - to avoid memory fragmentation (alloc/realloc/free...)
 * 
 * Object are unpacked "just in time" and repacked when they are not immediately needed.

 * A "packed" object is independent from the architecture (32|64 bits).
 * An object in its "struct" form, at the contrary, is different in 64 bits and 32 bits 
 * because of the different pointer lengths (4 bytes vs 8 bytes). 
 * Consequently, translations 64-32 and 32-64 are required both to pack and unpack objects.
 *
 * @note The 32-64 and 64-32 "translation" functions are local and hidden from the exposed API.
 * 
 * Packing functions
 * ----------------- 
 *      int K_cpack(char **pack, char *a1)                  Packs an IODE CMT object 
 *      int K_epack(char **pack, char *a1, char *a2)        Packs an IODE EQ object 
 *      int K_ipack(char **pack, char *a1)                  Packs an IODE IDT object 
 *      int K_lpack(char** pack, char* a1)                  Packs an IODE LST object 
 *      int K_spack(char **pack, char *a1)                  Packs an IODE Scalar object 
 *      int K_tpack(char** pack, char* a1)                  Packs an IODE TBL object 
 *      int K_vpack(char **pack, double *a1, int *a2)    Packs an IODE VAR object. 
 *      int K_opack(char** pack, char* a1, int* a2)         Reserved for future new objects
 * 
 * Unpacking functions
 * -------------------- 
 *      TBL* K_tunpack(char *pack)                   Creates a TBL struct from a packed TBL 
 *      Equation*  K_eunpack(char *pack, char *name)       Creates an EQ struct from a packed EQ
 *      Identity* K_iunpack(char *pack)                   Creates an IDT struct from a packed IDT
 * 
 * Allocation functions (VAR & Scalar only)
 * -------------------------------------
 *      int KS_alloc_scl()          Allocates space for a new Scalar (0.9, 1.0, NaN) in the the "swap area". Returns the "swap" handle.
 *      int KV_alloc_var(int nb)    Allocates space for a new VAR of length nb in the swap area, initialises it to IODE_NAN and returns the "swap" handle.
 * 
 * @note The "modern" terminology for pack and unpack is "serialize" and "deserialize".
 * @see scr4/s_swap.h (http://www.xon.be/scr4/libs1/libs1236.htm) for more details.
 */
#include "api/constants.h"
#include "api/lec/lec.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/structs_32.h"

#include <string>

bool debug_pack = false;
bool debug_unpack = false;


/**
 * Packs an IODE VAR object. 
 * 
 * If a1 is NULL, creates a VAR of length *a2 filled with IODE_NAN.
 * 
 * If a1 is defined, packs a1.
 * 
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object 
 * @param [in]  a1      (double*)    pointer to the first VAR element 
 * @param [in]  a2      (int *)         pointer to the number of elements in the VAR
 * 
 * @return int -1 if the variable is empty, 0 if ok
*/
int K_vpack(char **pack, double *a1, int *a2)
{
    int     i;
    double* ptr;

    if(*a2 == 0) 
        return(-1);
    *pack = (char*) P_create();
    if(a1 == NULL) 
    {
        *pack = (char*) P_add(*pack, NULL, sizeof(double) * *a2);
        ptr = (double*)P_get_ptr(*pack, 0);
        for(i = 0; i < *a2; i++) 
            ptr[i] = IODE_NAN;
    }
    else 
        *pack = (char*) P_add(*pack, (char*)a1, sizeof(double) * *a2);
    return(0);
}

/*
 * Function to pack TBL (in 32 and 64 bits)
 * -------------------------------------------
*/

/**
 *   Function to pack a TBL cell (in 32 and 64 bits)
 *  
 *  @param [in, out]    pack pointer to the pack before adding the cell (can be NULL)
 *  @param [in]         cell 
 *  @return             pointer to the new pack
 */
static char* K_tcell_pack(char *pack, TableCell *cell, int& p, int i, int j)
{
    int len;

    if(cell->get_type() == TABLE_CELL_LEC)
    {
        std::string lec = cell->get_content();
        if(lec.empty())
            return(pack);
        char* c_lec = (char*) lec.c_str();
        char* pack_idt = NULL;
        K_ipack(&pack_idt, c_lec);
        len = P_len(pack_idt);
        pack = (char*) P_add(pack, pack_idt, len);
        p++;
        debug_packing("cell", "LEC  ", p, i, j, lec, len);
    }
    else
    {
        std::string text = cell->get_content(false, false);
        char* c_text = (char*) text.c_str();
        len = (int) strlen(c_text) + 1;
        pack = (char*) P_add(pack, c_text, len);
        p++;
        debug_packing("cell", "STR  ", p, i, j, text, len);
    }

    return(pack);
}

/**
 *  Convert a TableCell64 struct (64 bits architecture) into a TableCell32 (32 bits arch)
 *  
 *  @param [in, out] tc64 pointer to a TableCell32 (Table cell in 32 bits arch)
 *  @param [in]      tc32 pointer to a TableCell64 (Table cell in 64 bits arch)
 *  @return void
 *  
 *  @details Copy the the first bytes of tc64 up to type, then set tc32->content as 1 or 0 
 *           according to the value of tc64->content.
 */
 
static void K_tcell64_32(TableCell* tc64, TableCell32* tc32, int p, int i, int j)
{
    TableCellType type64 = tc64->get_type();
    if(type64 == TABLE_CELL_STRING)
    {
        // NOTE: always set content to 1, even if the string is empty.
        tc32->content = 1; 
        tc32->type = (char) type64;
        tc32->attribute = tc64->get_attribute();
    }
    else
    {
        tc32->content = tc64->is_null() ? 0 : 1;
        tc32->type = (char) type64;
        tc32->attribute = tc64->get_attribute();
    }
    memset(tc32->pad, '\0', 2);
}

/**
 *  Convert a TableLine64 into TableLine32 struct.
 *  
 *  @param [in] tl64 TableLine in 64 bits arch
 *  @param [in] tl32 TableLine in 32 bit arch
 *  @return void
 *  
 *  @details See K_tcell64_32()
 */
 
static void K_tline64_32(TableLine* tl64, TableLine32* tl32)
{
    tl32->type = (char) tl64->get_type();
    tl32->graph_type = (char) tl64->get_graph_type();
    tl32->right_axis = tl64->right_axis ? 1 : 0;
    tl32->pad[0] = '\0';
}

/**
 *  Convert a TBL64 into TBL32 struct.
 *  
 *  @param [in] tbl64 TBL in 64 bits arch
 *  @param [in] tbl32 TBL in 32 bit arch
 *  @return void
 *  
 *  @details See K_tcell64_32()
 */

static void K_tbl64_32(TBL* tbl64, TBL32* tbl32)
{
    tbl32->language = (short) tbl64->get_language();
    tbl32->repeat_columns = tbl64->repeat_columns;
    tbl32->nb_columns = tbl64->nb_columns;
    tbl32->nb_lines = (int) tbl64->lines.size();

    K_tline64_32((TableLine*) &tbl64->divider_line, (TableLine32*) &tbl32->divider_line);

    tbl32->z_min = tbl64->z_min;
    tbl32->z_max = tbl64->z_max;
    tbl32->y_min = tbl64->y_min;
    tbl32->y_max = tbl64->y_max;
    tbl32->attribute = tbl64->attribute;
    tbl32->chart_box = tbl64->chart_box;
    tbl32->chart_shadow = tbl64->chart_shadow;
    tbl32->chart_gridx = (char) tbl64->get_gridx();
    tbl32->chart_gridy = (char) tbl64->get_gridy();
    tbl32->chart_axis_type = (char) tbl64->get_graph_axis();
    tbl32->text_alignment = (char) tbl64->get_text_alignment();
    memset(tbl32->pad, '\0', sizeof(tbl32->pad));
}


/**
 *  Packs a 64 bits TBL struct by first converting the TBL64, TableLine64 and TableCell64 into 32 bits struct.
 *  
 *  @param [in] pack pointer to the packed table placeholder
 *  @param [in] a1   pointer to the 64 bits TBL
 *  @return     -1 if error (lack of memory), 0 if ok
 *  
 *  @details Sub function of K_tpack(). 
 */

static int K_tpack64(char **pack, char *a1)
{
    TBL*      tbl = (TBL*) a1;
    TBL32     tbl32;
    TableLine*    lines;
    TableLine32*  lines32;
    TableCell*    cell;
    TableCell*    cells;
    TableCell32*  cell32;
    TableCell32*  cells32;
    int       p=-1;

    *pack = (char*) P_create();
    if(a1 == NULL) 
        return(-1);

    /* tbl */
    K_tbl64_32(tbl, &tbl32);
    *pack = (char*) P_add(*pack, (char*) &tbl32, sizeof(TBL32));
    p++;
    debug_packing("TBL      ", "", p);

    /* div */
    /* 1. : [nc x TableCell32] */
    cells = tbl->divider_line.cells.data();
    cells32 = (TableCell32*) SW_nalloc(sizeof(TableCell32) * (int)T_NC(tbl));
    for(int j = 0; j < T_NC(tbl); j++)
        K_tcell64_32(cells + j, cells32 + j, p+1, -1, j);

    *pack = (char*) P_add(*pack, (char*) cells32, sizeof(TableCell32) * (int) T_NC(tbl));
    p++;
    debug_packing("line", "DIV  ", p);
    for(int j = 0; j < T_NC(tbl); j++)
        debug_packing("cell", "DIV  ", p+j+1, 0, j, (cells32[j].content == 1) ? "YES" : "NO");

    /* 2. : [cell] [cell] (nc x [TableCell->content]) */
    for(int j = 0; j < T_NC(tbl); j++)
        *pack = K_tcell_pack(*pack, cells + j, p, 0, j);

    /* pack lines */
    /* 1. : [nl x TableLine] */
    lines = (TableLine*) tbl->lines.data();
    lines32 = (TableLine32*) SW_nalloc(sizeof(TableLine32) * (int) T_NL(tbl));
    for(int j = 0; j < T_NL(tbl); j++)
        K_tline64_32(lines + j, lines32 + j);

    *pack = (char*) P_add(*pack, (char*) lines32, sizeof(TableLine32) * (int) T_NL(tbl));
    p++;

    debug_packing("LINES    ", "", p);

    /* 2. For each line and each col, pack cell [cell] [cell] ... */
    int i = 0;
    for(TableLine& line: tbl->lines) 
    {
        switch(line.get_type()) 
        {
            case TABLE_LINE_CELL:
                /* [TableLine32 * NC] */
                cells = line.cells.data();
                for(int j = 0; j < T_NC(tbl); j++) 
                    K_tcell64_32(cells + j, cells32 + j, p+1, i, j);
                *pack = (char*) P_add(*pack, (char*) cells32, sizeof(TableCell32) * (int) T_NC(tbl));
                p++;
                debug_packing("line", "CELL ", p, i);
                for(int j = 0; j < T_NC(tbl); j++)
                    debug_packing("content? ", "", p+j+1, i, j, (cells32[j].content == 1) ? "YES" : "NO");
                
                /* [TableCell32] [TableCell32] ... */
                for(int j = 0; j < T_NC(tbl); j++)
                    *pack = K_tcell_pack(*pack, cells + j, p, i, j);
                break;

            case TABLE_LINE_TITLE:
                cell32 = cells32;
                cell = &(line.cells[0]);
                /* [1 x TableCell] */
                K_tcell64_32(cell, cell32, p+1, i, 0);
                *pack = (char*) P_add(*pack, (char*) cell32, sizeof(TableCell32));
                p++;

                debug_packing("line", "TITLE", p, i);
                debug_packing("content? ", "", p+1, i, 0, (cell32[0].content == 1) ? "YES" : "NO");

                /* 1 x [TableCell->content] */
                *pack = K_tcell_pack(*pack, cell, p, i, 0);
                break;

            case TABLE_LINE_SEP:
                debug_packing("line", "SEP  ", p, i);
                break;
            case TABLE_ASCII_LINE_MODE:
                debug_packing("line", "MODE ", p, i);
                break;
            case TABLE_LINE_FILES:
                debug_packing("line", "FILES", p, i);
                break;
            case TABLE_LINE_DATE:
                debug_packing("line", "DATE ", p, i);
                break;

            default:
                std::string msg = "Packing table line: invalid line type " + std::to_string(line.get_type()); 
                msg += " at line " + std::to_string(i);
                kwarning(msg.c_str());
                break;
        }
        i++;
    }

    SW_nfree(cells32);
    SW_nfree(lines32);
    return(0);
}

/**
 * Packs a TBL object whether in 32 or 64 bits architecture. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed TBL
 * @param [in]  a1      (char *)        pointer to the TBL (cast to char *)
 *
 * @return int 0 
*/

int K_tpack(char** pack, char* a1, char* name)
{
    if(name != NULL)
        debug_packing("table " + std::string(name), "--------------------------------", -1);
    return(K_tpack64(pack, a1));
    return 0;
}


/**
 * Compiles an IDT and packs the resulting structure. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        pointer to the LEC form of the IDT
 *
 * @return int 0 if ok, -1 if a1 is NULL or the comp
 */
 
int K_ipack(char **pack, char *a1)
{
    CLEC* clec;

    *pack = 0;
    if(a1 == NULL) return(-1);
    clec = L_cc(a1);
    if(clec == 0)  return(-1);
    *pack = (char*) P_create();
    *pack = (char*) P_add(*pack, a1, (int)strlen(a1) + 1);
    *pack = (char*) P_add(*pack, (char*)clec, clec->tot_lg);
    SW_nfree(clec);
    return(0);
}

/**
 * Packs an EQ object.
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        pointer to the EQ (cast to EQ *)
 * @param [in]  endo    (char *)        pointer to the endogenous name
 *
 * @return int 
 *      0 if ok, 
 *      -1 if EQ does not contain a LEC formula, 
 *      -1 if the LEC formula cannot be compiled
 *  
 * @details The Endo (a2) is needed to "solve" the LEC equation. L_solve() try to analytically
 *          solve the equation to obtain a formula in the form :
 *              ENDO := f(x)
 *          If it is not possible, a Newton-Raphson like algorithm will be used to solve the equation
 *          for each iteration of the simulation process.
 */

int   K_epack(char **pack, char *a1, char *endo)
{
    Equation* eq;
    CLEC* clec;

    eq = (Equation*) a1;

    *pack = (char*) P_create();
    if(eq->lec.empty()) 
        return(-1);
    
    clec = L_solve((char*) eq->lec.c_str(), endo);
    if(clec == 0)  
        return(-1);

    *pack = (char*) P_add(*pack, (char*) eq->lec.c_str(), (int) eq->lec.size() + 1);  /* lec */
    *pack = (char*) P_add(*pack, (char*) clec, clec->tot_lg);                   /* clec */
    *pack = (char*) P_add(*pack, &(eq->solved), 1);                             /* solved */
    *pack = (char*) P_add(*pack, &(eq->method), 1);                             /* method */
    *pack = (char*) P_add(*pack, (char*) &(eq->sample), sizeof(Sample));        /* sample */

    if(eq->comment.empty()) 
        *pack = (char*) P_add(*pack, NULL, 1);
    else 
        *pack = (char*) P_add(*pack, (char*) eq->comment.c_str(), (int) eq->comment.size() + 1);          /* cmt */

    if(eq->block.empty()) 
        *pack = (char*) P_add(*pack, NULL, 1);
    else 
        *pack = (char*) P_add(*pack, (char*) eq->block.c_str(), (int) eq->block.size() + 1);              /* blk */

    if(eq->instruments.empty()) 
        *pack = (char*) P_add(*pack, NULL, 1);
    else 
        *pack = (char*) P_add(*pack, (char*) eq->instruments.c_str(), (int) eq->instruments.size() + 1);  /* instr */

    *pack = (char*) P_add(*pack, (char*)&(eq->date), sizeof(long));                     /* date */
    *pack = (char*) P_add(*pack, (char*)&(eq->tests), EQS_NBTESTS * sizeof(float));     /* tests*/ /* FLOAT 12-04-98 */

    SW_nfree(clec);
    return(0);
}


/**
 * Packs a Scalar object. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        pointer to the Scalar. If NULL, a1 is replaced by a default Scalar (.9, 1.0, NaN).
 *
 * @return int 0 
 *
 * @note No pointer in Scalar, thus the pack == binary copy of the Scalar
 */

int K_spack(char **pack, char *a1)
{
    static    Scalar scl = { 0.9, 1.0, IODE_NAN };

    *pack = (char*) P_create();
    if(a1 == NULL)
        *pack = (char*) P_add(*pack, &scl, sizeof(Scalar));
    else
        *pack = (char*) P_add(*pack, a1, sizeof(Scalar));
    return(0);
}


/**
 * Packs a CMT object. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        NULL or pointer to the CMT. If NULL, the CMT is replaced by "".
 *
 * @return int 0 
 *
 * @note A CMT is a simple char *, thus the pack == binary copy of the CMT
 */

int K_cpack(char **pack, char *a1)
{
    *pack = (char*) P_create();
    if(a1 == NULL) 
        *pack = (char*) P_add(*pack, (char*) "", 1);
    else 
        *pack = (char*) P_add(*pack, a1, (int)strlen(a1) + 1);
    return(0);
}


/**
 * Packs a LST object. 
 *
 * @param [out] pack    (char **)       placeholder for the pointer to the packed object
 * @param [in]  a1      (char *)        NULL or pointer to the LST. If NULL, the LST is replaced by "".
 *
 * @return int 0 
 *
 * @note A LST is a simple char *, thus the pack == binary copy of the struct.
 */

int K_lpack(char** pack, char* a1)
{
    *pack = (char*) P_create();
    if(a1 == NULL) 
        *pack = (char*) P_add(*pack, (char*) "", 1);
    else 
        *pack = (char*) P_add(*pack, a1, (int)strlen(a1) + 1);
    return(0);
}

/**
 *  Reserved for future new objects.
 *  
 *  @param [out] pack  char **      placeholder for the pointer to the packed object
 *  @param [in]  a1    char*        NULL or pointer the the object.
 *  @param [in]  a2    char*        extra info 
 *  @return 0 for compatibily with other functions 
 */
 
int K_opack(char** pack, char* a1, int* a2)
{
    *pack = (char*) P_create();
    if(a1 == NULL || *a2 == 0) 
        *pack = (char*) P_add(*pack, (char*) "", 1);
    else 
        *pack = (char*) P_add(*pack, a1, *a2);
    return(0);
}


/* 
 * UNPACK functions
 * ----------------
 * From a packed object (architecture insensitive, see above), creates an IODE object (VAR, EQ, TBL, ...) 
 * in the current archicture (32|64).
 */

// see T_create() for the initialization of the special 'div' line
static void K_tcell_div_sanitize(TableCell* cell, int j)
{
    if(cell->get_type() == TABLE_CELL_STRING)
    {
        std::string lec = cell->get_content();
        cell->set_lec(lec);
    }

    if(j == 0 && cell->is_null())
        cell->set_lec("1");
}

/**
 *  Converts a 32 bits table **cell** (TableCell32) to a 64 bits TableCell. 
 *  
 *  @param [in]         tc32 TableCell32 * pointer to the TableCell32 
 *  @param [in, out]    tc64 TableCell *   pointer to the resulting TableCell (64 bits)
 *  @return void
 *  
 *  @note Only used in 64 bit architecture.
 *  
 */
static void K_tcell32_64(TableCell32* tc32, TableCell* tc64)
{
    if(tc32->type == TABLE_CELL_STRING)
        tc64->set_text("");
    // NOTE: if tc32->type == 0, assume it is a LEC cell with content = ""
    //       This can happen when reading binary formatted files
    else if(tc32->content != 0)
        tc64->set_lec("1");
    else
        tc64->set_lec("");
}

/**
 *  Converts a 32 bits table **line** (TableLine32) to a 64 bits TableLine. 
 *  
 *  @param [in]         tc32 TableLine32 * pointer to the TableLine32 
 *  @param [in, out]    tc64 TableLine *   pointer to the resulting TableLine (64 bits)
 *  @return void
 *  
 *  @note Only used in 64 bit architecture.
 *  
 */
static void K_tline32_64(TableLine32* tl32, TableLine* tl64)
{
    tl64->set_graph_type((TableGraphType) tl32->graph_type);
    tl64->right_axis = (bool) tl32->right_axis;
}

/**
 *  Converts a 32 bits table (TBL32) to a 64 bits TBL. 
 *  
 *  @param [in]         tc32 TBL32 * pointer to the TBL32 
 *  @param [in, out]    tc64 TBL *   pointer to the resulting TBL (64 bits)
 *  @return void
 *  
 *  @note Only used in 64 bit architecture.
 *  
 */
static void K_tbl32_64(TBL32* tbl32, TBL* tbl64)
{
    tbl64->set_language((TableLang) tbl32->language);
    tbl64->repeat_columns = tbl32->repeat_columns;
    tbl64->nb_columns = tbl32->nb_columns;

    K_tline32_64((TableLine32*) &tbl32->divider_line, (TableLine*) &tbl64->divider_line);

    tbl64->z_min = tbl32->z_min;
    tbl64->z_max = tbl32->z_max;
    tbl64->y_min = tbl32->y_min;
    tbl64->y_max = tbl32->y_max;
    tbl64->attribute = tbl32->attribute;
    tbl64->chart_box = tbl32->chart_box;
    tbl64->chart_shadow = tbl32->chart_shadow;
    tbl64->set_gridx((TableGraphGrid) tbl32->chart_gridx);
    tbl64->set_gridy((TableGraphGrid) tbl32->chart_gridy);
    tbl64->set_graph_axis((TableGraphAxis) tbl32->chart_axis_type);
    tbl64->set_text_alignment((TableTextAlign) tbl32->text_alignment);
}

/**
 *   Function to unpack a TBL cell
 */
static int K_tcell_unpack(char *pack, int& p, TableCell *cell, int i, int j)
{
    char* value = (char*) P_get_ptr(pack, p);

    if(cell->get_type() == TABLE_CELL_STRING)
    {
        std::string content = (value == NULL) ? "" : std::string(value);
        debug_unpacking("cell", "STR  ", p, i, j, content);
        cell->set_text(content, false);
        p++;
        return 0;
    }
    else
    {   
        // NOTE: 'value' is a packed IDT
        char* c_lec = (char*) P_get_ptr(value, 0);
        std::string lec = (c_lec == NULL) ? "" : std::string(c_lec);
        debug_unpacking("cell", "LEC  ", p, i, j, lec);
        cell->set_lec(lec);
        p++;
        return 0;
    }
}

/**
 * Unpacks a packed TBL into a TBL structure in the 64 bits architecture.
 * @note Only used in 64 bit architecture.
 * 
 * @param [in]      pack char * packed TBL
 *
 * @return TBL *    allocated TBL (64 bits)
*/

static TBL* K_tunpack64(char *pack)
{
    // 64 bits structs
    TBL*     tbl;
    TableLine*   line;
    TableCell*   cell;
    // 32 bits struct (iode std)
    TBL32*   tbl32;
    TableLine32* line32;
    TableLine32* lines32;
    TableCell32* cell32;
    TableCell32* cells32;
    int      p=0;

    /* tbl */
    tbl32 = (TBL32*) P_get_ptr(pack, 0);
    tbl = new TBL(tbl32->nb_columns);
    K_tbl32_64(tbl32, tbl);
    debug_unpacking("TBL      ", "", 0);

    /* div (TableLine) */
    cells32 = (TableCell32*) P_get_ptr(pack, 1);
    debug_unpacking("line", "DIV  ", 1);

    p = 2;
    // NOTE: new TBL(tbl32->nb_columns) above creates a table with a 'divider' line 
    //       with nb_columns cells of type TABLE_CELL_LEC and content = ""
    for(int j = 0; j < T_NC(tbl); j++) 
    {
        cell32 = cells32 + j;
        cell = &(tbl->divider_line.cells[j]);
        K_tcell32_64(cell32, cell);
        debug_unpacking("cell", "DIV  ", p, 0, j, (cell32->content == 0) ? " NO" : " YES");
        if(cell32->content != 0)
            K_tcell_unpack(pack, p, cell, 0, j);
        cell->set_attribute(cell32->attribute);
        K_tcell_div_sanitize(cell, j);
    }
    
    /* lines */
    lines32 = (TableLine32*) P_get_ptr(pack, p);
    debug_unpacking("LINES    ", "", p);
    p++;

    for(int i = 0; i < tbl32->nb_lines; i++) 
    {
        line32 = lines32 + i;
        switch(line32->type) 
        {
            case TABLE_LINE_CELL:
                tbl->lines.push_back(TableLine(TableLineType::TABLE_LINE_CELL));
                line = &tbl->lines[i];
                K_tline32_64(line32, line);

                cells32 = (TableCell32*) P_get_ptr(pack, p);
                debug_unpacking("line", "CELL ", p, i);
                p++;

                for(int j = 0; j < T_NC(tbl); j++) 
                {
                    cell32 = cells32 + j;
                    // if cell type not properly set, assume it is a LEC cell
                    // -> may happen when reading binary formatted files <-
                    if(!(cell32->type == TABLE_CELL_STRING || cell32->type == TABLE_CELL_LEC))
                        cell32->type = TABLE_CELL_LEC;
                    if(cell32->type == TABLE_CELL_STRING)
                        cell = new TableCell(TABLE_CELL_STRING);
                    else
                        cell = new TableCell(TABLE_CELL_LEC, "", j);
                    K_tcell32_64(cell32, cell);
                    if(cell32->content != 0) 
                        K_tcell_unpack(pack, p, cell, i, j);
                    cell->set_attribute(cell32->attribute);
                    line->cells.push_back(*cell);
                }
                break;

            case TABLE_LINE_TITLE:
                tbl->lines.push_back(TableLine(TableLineType::TABLE_LINE_TITLE));
                line = &tbl->lines[i];
                K_tline32_64(line32, line);

                cell32 = (TableCell32*) P_get_ptr(pack, p);
                debug_unpacking("line", "TITLE", p, i);
                p++;

                cell = new TableCell(TABLE_CELL_STRING);
                K_tcell32_64(cell32, cell);
                if(cell32->content != 0) 
                    K_tcell_unpack(pack, p, cell, i, 0);
                cell->set_attribute(cell32->attribute);
                line->cells.push_back(*cell);
                break;

            case TABLE_LINE_SEP:
                tbl->lines.push_back(TableLine(TableLineType::TABLE_LINE_SEP));
                debug_unpacking("line", "SEP  ", p, i);
                break;
            case TABLE_LINE_MODE:
                tbl->lines.push_back(TableLine(TableLineType::TABLE_LINE_MODE));
                debug_unpacking("line", "MODE ", p, i);
                break;
            case TABLE_LINE_FILES:
                tbl->lines.push_back(TableLine(TableLineType::TABLE_LINE_FILES));
                debug_unpacking("line", "FILES", p, i);
                break;
            case TABLE_LINE_DATE:
                tbl->lines.push_back(TableLine(TableLineType::TABLE_LINE_DATE));
                debug_unpacking("line", "DATE ", p, i);
                break;

            default:
                std::string msg = "Unpacking table line: invalid line type " + std::to_string(line32->type); 
                msg += " at line " + std::to_string(i);
                kwarning(msg.c_str());
                break;
        }
    }
    return(tbl);
}


/**
 * Unpacks a packed TBL into a TBL structure in the current (32|64 bits) architecture.
 * 
 * @param [in]      pack char * packed TBL
 *
 * @return TBL *    allocated TBL (32|64 bits according to the current architecture)
*/

TBL* K_tunpack(char *pack, char* name)
{
    if(name != NULL)
        debug_unpacking("table " + std::string(name), "--------------------------------", -1);
    return(K_tunpack64(pack));
}


/**
 * Unpacks a packed EQ into a EQ structure in the current (32|64 bits) architecture.
 * 
 * @param [in]      pack    char *     packed EQ
 *
 * @return EQ *     allocated EQ (32|64 bits according to the current architecture)
*/

Equation* K_eunpack(char *pack, char *name)
{
    int len = 0;

    std::string endo(name);

    len = P_get_len(pack, 0);
    char* c_lec = SW_nalloc(len);
    memcpy(c_lec, P_get_ptr(pack, 0), len);
    std::string lec(c_lec);
    SW_nfree(c_lec);

    // len = P_get_len(pack, 1);
    // CLEC* clec = (CLEC*) SW_nalloc(len);
    // memcpy(eq->clec, P_get_ptr(pack, 1), len);

    char char_method = *(char*)(P_get_ptr(pack, 3));
    int i_method = (int) char_method;
    IodeEquationMethod method = EQ_LSQ;
    if(i_method >= 0 && i_method < IODE_NB_EQ_METHODS)
        method = (IodeEquationMethod) i_method;

    len = P_get_len(pack, 5);
    char* c_cmt = SW_nalloc(len);
    memcpy(c_cmt, P_get_ptr(pack, 5), len);
    std::string comment(c_cmt);
    SW_nfree(c_cmt);

    len = P_get_len(pack, 6);
    char* c_block = SW_nalloc(len);
    memcpy(c_block, P_get_ptr(pack, 6), len);
    std::string block(c_block);
    SW_nfree(c_block);

    len = P_get_len(pack, 7);
    char* c_instr = SW_nalloc(len);
    memcpy(c_instr, P_get_ptr(pack, 7), len);
    std::string instruments(c_instr);
    SW_nfree(c_instr); 

    Sample* smpl = (Sample*) SW_nalloc(sizeof(Sample));
    memcpy(smpl, P_get_ptr(pack, 4), sizeof(Sample));
    Period from_period = (smpl !=  NULL) ? smpl->start_period : Period();
    Period to_period = (smpl !=  NULL) ? smpl->end_period : Period();

    Equation* eq = new Equation(endo, lec, method, from_period.to_string(), to_period.to_string(), 
                                comment, instruments, block, false);
    eq->solved = *((char*) (P_get_ptr(pack, 2)));
    eq->date = *((long*) (P_get_ptr(pack, 8)));    
    memcpy(eq->tests.data(), P_get_ptr(pack, 9), EQS_NBTESTS * sizeof(float)); /* FLOAT 12-04-98 */
    return(eq);
}


/**
 * Unpacks a packed IDT into an allocated IDT structure..
 * 
 * @param [in]  char*     pack   packed IDT
 * @return      IDT *            allocated IDT structure 
*/

Identity* K_iunpack(char* pack)
{
    int len = P_get_len(pack, 0);
    std::string lec = std::string((char*) SCR_stracpy((unsigned char*) P_get_ptr(pack, 0)));
    Identity* idt = new Identity(lec);
    return idt;
}

/*-- ALLOC SCALAR AND VAR --*/

/**
 *  Allocates space for a new scalar (0.9, 1.0, NaN) in the the "swap area" and returns the allocated handle.
 *  
 *  @return Handle to the new scalar.
 *
 */

int KS_alloc_scl()
{
    char* pack, * ptr;
    int     pos;

    K_spack(&pack, NULL);
    pos = SW_alloc(P_len(pack));
    ptr = SW_getptr(pos);
    memcpy(ptr, pack, P_len(pack));
    SW_nfree(pack);
    return(pos);
}


/**
 * Allocates space for a VAR of length nb in the swap area, initialises it to IODE_NAN and returns the swap handle.
 * 
 * @param nb  int length of the VAR element.
 *
 * @return int handle to the VAR in the swap space.
*/

int KV_alloc_var(int nb)
{
    char* pack, * ptr;
    int     pos;

    K_vpack(&pack, NULL, &nb);
    pos = SW_alloc(P_len(pack));
    ptr = SW_getptr(pos);
    memcpy(ptr, pack, P_len(pack));
    SW_nfree(pack);
    return(pos);
}

