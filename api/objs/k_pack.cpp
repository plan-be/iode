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
static char* K_tcell_pack(char *pack, TCELL *cell, int& p, int i, int j)
{
    int len;

    if(cell->type == TABLE_CELL_LEC)
    {
        if(cell->idt == nullptr) 
            return(pack);
        char* c_lec = (char*) cell->idt->lec.c_str();
        char* pack_idt = NULL;
        K_ipack(&pack_idt, c_lec);
        len = P_len(pack_idt);
        pack = (char*) P_add(pack, pack_idt, len);
        p++;
        debug_packing("cell", "LEC  ", p, i, j, cell->idt->lec, len);
    }
    else
    {
        char* c_content = (char*) cell->content.c_str();
        len = (int) strlen(c_content) + 1;
        pack = (char*) P_add(pack, c_content, len);
        p++;
        debug_packing("cell", "STR  ", p, i, j, cell->content, len);
    }

    return(pack);
}

/**
 *  Convert a TCELL64 struct (64 bits architecture) into a TCELL32 (32 bits arch)
 *  
 *  @param [in, out] tc64 pointer to a TCELL32 (Table cell in 32 bits arch)
 *  @param [in]      tc32 pointer to a TCELL64 (Table cell in 64 bits arch)
 *  @return void
 *  
 *  @details Copy the the first bytes of tc64 up to type, then set tc32->content as 1 or 0 
 *           according to the value of tc64->content.
 */
 
static void K_tcell64_32(TCELL* tc64, TCELL32* tc32, int p, int i, int j)
{
    if(tc64->type == TABLE_CELL_STRING)
    {
        tc32->content = 1; 
        tc32->type = tc64->type;
        tc32->attribute = tc64->attribute;
    }
    else if(tc64->type == TABLE_CELL_LEC)
    {
        tc32->content = (tc64->idt == nullptr) ? 0 : 1;
        tc32->type = tc64->type;
        tc32->attribute = tc64->attribute;
    }
    else if(tc64->type == 0)
    {
        // old files may have empty cells with type = 0
        tc32->content = (tc64->idt == nullptr) ? 0 : 1;
        tc32->type = TABLE_CELL_LEC;
        tc32->attribute = TABLE_CELL_LEFT;
    }
    else
    {
        std::string msg = "Packing table cell: invalid cell type " + std::to_string(tc32->type); 
        msg += " at line " + std::to_string(i) + ", column " + std::to_string(j); 
        kwarning(msg.c_str());
        tc32->content = 0;
        tc32->type = TABLE_CELL_LEC;
        tc32->attribute = TABLE_CELL_LEFT;
    }
    memset(tc32->pad, '\0', 2);
}

/**
 *  Convert a TLINE64 into TLINE32 struct.
 *  
 *  @param [in] tl64 TLINE in 64 bits arch
 *  @param [in] tl32 TLINE in 32 bit arch
 *  @return void
 *  
 *  @details See K_tcell64_32()
 */
 
static void K_tline64_32(TLINE* tl64, TLINE32* tl32)
{
    tl32->type = tl64->type;
    tl32->graph_type = tl64->graph_type;
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
    tbl32->language = tbl64->language;
    tbl32->repeat_columns = tbl64->repeat_columns;
    tbl32->nb_columns = tbl64->nb_columns;
    tbl32->nb_lines = tbl64->nb_lines;

    K_tline64_32((TLINE*) &tbl64->divider_line, (TLINE32*) &tbl32->divider_line);

    tbl32->z_min = tbl64->z_min;
    tbl32->z_max = tbl64->z_max;
    tbl32->y_min = tbl64->y_min;
    tbl32->y_max = tbl64->y_max;
    tbl32->attribute = tbl64->attribute;
    tbl32->chart_box = tbl64->chart_box;
    tbl32->chart_shadow = tbl64->chart_shadow;
    tbl32->chart_gridx = tbl64->chart_gridx;
    tbl32->chart_gridy = tbl64->chart_gridy;
    tbl32->chart_axis_type = tbl64->chart_axis_type;
    tbl32->text_alignment = tbl64->text_alignment;
    memset(tbl32->pad, '\0', sizeof(tbl32->pad));
}

/**
 *  Pack a 32 bits TBL struct.
 *  
 *  @param [in] pack pointer to the packed table placeholder
 *  @param [in] a1   pointer to the 32 bits TBL
 *  @return -1 if error (lack of memory), 0 if ok
 *  
 *  @details Sub function of K_tpack(). 
 */

static int K_tpack32(char **pack, char *a1)
{
    TBL* tbl = (TBL*)a1;
    //TLINE* lines;
    TCELL*  cells;
    int     i, j, p=-1;

    *pack = (char*) P_create();
    if(a1 == NULL) 
        return(-1);

    /* tbl */
    *pack = (char*) P_add(*pack, (char*) tbl, sizeof(TBL));
    p++;

    /* div */
    /* 1. : [nc x TCELL] */
    cells = (TCELL*) tbl->divider_line.cells;
    *pack = (char*) P_add(*pack, (char*) cells, sizeof(TCELL) * (int) T_NC(tbl));
    p++;

    /* 2. : nc x [TCELL->content] */
    for(j = 0; j < T_NC(tbl); j++)
        *pack = K_tcell_pack(*pack, cells + j, p, -1, j);

    /* lines */
    /* 1. : [nl x TLINE] */
    *pack = (char*) P_add(*pack, (char*) tbl->lines, sizeof(TLINE) * (int) T_NL(tbl));
    p++;
    for(i = 0; i < T_NL(tbl); i++) 
    {
        switch(tbl->lines[i].type) 
        {
            case TABLE_LINE_CELL:
                cells = (TCELL*) tbl->lines[i].cells;
                /* [nc x TCELL] */
                *pack = (char*) P_add(*pack, (char*) cells, sizeof(TCELL) * (int) T_NC(tbl));
                p++;

                /* 2. : nc x [TCELL->content] */
                for(j = 0; j < T_NC(tbl); j++)
                    *pack = K_tcell_pack(*pack, cells + j, p, i, j);
                break;
            case TABLE_LINE_TITLE:
                cells = (TCELL*) tbl->lines[i].cells;
                /* [1 x TCELL] */
                *pack = (char*) P_add(*pack, (char*) cells, sizeof(TCELL));
                p++;

                /* 1 x [TCELL->content] */
                *pack = K_tcell_pack(*pack, cells, p, i, 0);
                break;
        }
    }

    return(0);
}

/**
 *  Packs a 64 bits TBL struct by first converting the TBL64, TLINE64 and TCELL64 into 32 bits struct.
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
    TLINE*    line;
    TLINE*    lines;
    TLINE32*  lines32;
    TCELL*    cells;
    TCELL32*  cells32;
    int       i, j, p=-1;

    *pack = (char*) P_create();
    if(a1 == NULL) 
        return(-1);

    /* tbl */
    K_tbl64_32(tbl, &tbl32);
    *pack = (char*) P_add(*pack, (char*) &tbl32, sizeof(TBL32));
    p++;
    debug_packing("TBL      ", "", p);

    /* div */
    /* 1. : [nc x TCELL32] */
    cells = (TCELL*) tbl->divider_line.cells;
    cells32 = (TCELL32*) SW_nalloc(sizeof(TCELL32) * (int)T_NC(tbl));
    for(j = 0; j < T_NC(tbl); j++)
        K_tcell64_32(cells + j, cells32 + j, p+1, -1, j);

    *pack = (char*) P_add(*pack, (char*) cells32, sizeof(TCELL32) * (int) T_NC(tbl));
    p++;
    debug_packing("line", "DIV  ", p);
    for(j = 0; j < T_NC(tbl); j++)
        debug_packing("content? ", "", p+j+1, 0, j, (cells32[j].content == 1) ? "YES" : "NO");

    /* 2. : [cell] [cell] (nc x [TCELL->content]) */
    for(j = 0; j < T_NC(tbl); j++)
        *pack = K_tcell_pack(*pack, cells + j, p, 0, j);

    /* pack lines */
    /* 1. : [nl x TLINE] */
    lines = (TLINE*) tbl->lines;
    lines32 = (TLINE32*) SW_nalloc(sizeof(TLINE32) * (int)T_NL(tbl));
    for(j = 0; j < T_NL(tbl); j++)
        K_tline64_32(lines + j, lines32 + j);

    *pack = (char*) P_add(*pack, (char*) lines32, sizeof(TLINE32) * (int) T_NL(tbl));
    p++;

    debug_packing("LINES    ", "", p);

    /* 2. For each line and each col, pack cell [cell] [cell] ... */
    for(i = 0; i < T_NL(tbl); i++) 
    {
        line = tbl->lines + i;
        switch(line->type) 
        {
            case TABLE_LINE_CELL:
                /* [TLINE32 * NC] */
                cells = (TCELL*) line->cells;
                for(j = 0; j < T_NC(tbl); j++) 
                    K_tcell64_32(cells + j, cells32 + j, p+1, i, j);
                *pack = (char*) P_add(*pack, (char*) cells32, sizeof(TCELL32) * (int) T_NC(tbl));
                p++;
                debug_packing("line", "CELL ", p, i);
                for(j = 0; j < T_NC(tbl); j++)
                    debug_packing("content? ", "", p+j+1, i, j, (cells32[j].content == 1) ? "YES" : "NO");
                
                /* [TCELL32] [TCELL32] ... */
                for(j = 0; j < T_NC(tbl); j++)
                    *pack = K_tcell_pack(*pack, cells + j, p, i, j);
                break;

            case TABLE_LINE_TITLE:
                cells = (TCELL*) line->cells;
                /* [1 x TCELL] */
                K_tcell64_32(cells + 0, cells32 + 0, p+1, i, 0);
                *pack = (char*) P_add(*pack, (char*) cells32, sizeof(TCELL32));
                p++;

                debug_packing("line", "TITLE", p, i);
                debug_packing("content? ", "", p+1, i, 0, (cells32[0].content == 1) ? "YES" : "NO");

                /* 1 x [TCELL->content] */
                *pack = K_tcell_pack(*pack, cells, p, i, 0);
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
                std::string msg = "Packing table line: invalid line type " + std::to_string(line->type); 
                msg += " at line " + std::to_string(i);
                kwarning(msg.c_str());
                break;
        }
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

int K_tpack(char** pack, char* a1)
{
    if(X64)
        return(K_tpack64(pack, a1));
    else
        return(K_tpack32(pack, a1));
    return(0);
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
static void K_tcell_div_sanitize(TCELL* cell, int j)
{
    // if cell type not set, assume it is a LEC cell
    if(cell->type == 0)
    {
        cell->type = TABLE_CELL_LEC;
        cell->content = "";
        cell->idt = nullptr;
    }

    if(cell->type == TABLE_CELL_STRING)
    {
        cell->type = TABLE_CELL_LEC;
        std::string content = cell->content;
        if(!trim(content).empty())
        {
            cell->idt = new Identity(content);
            cell->content = "";
        }
        else
            cell->idt = nullptr;
    }

    if(j == 0 && cell->idt == nullptr)
        cell->idt = new Identity("1");
}

static void K_tcell_sanitize(TCELL* cell, int j)
{
    // if cell type not set, assume it is a STRING cell
    if(cell->type == 0)
    {
        cell->type = TABLE_CELL_STRING;
        cell->content = "";
        cell->idt = nullptr;
        cell->attribute = TABLE_CELL_LEFT;
        return;
    }

    if(cell->type == TABLE_CELL_STRING)
        cell->idt = nullptr;

    if(cell->type == TABLE_CELL_LEC)
        cell->content = "";
}

/**
 * Unpacks a packed TBL into a TBL structure in the current architecture (32|64 bits).
 * Converts simultaneously TLINE and TCELL to 32 bits structs.
 * 
 * @param [in]      pack char * packed TBL
 *
 * @return TBL *    Allocated TBL in 32 or 64 bits
*/
static TBL* K_tunpack32(char *pack)
{
    TBL* ptbl, * tbl;
    //TLINE* line;
    TCELL* pcell, * cell;
    int     i, j, p, len = 0;

    /* tbl */
    len = P_get_len(pack, 0);
    ptbl = (TBL*) P_get_ptr(pack, 0);
    tbl = (TBL*) SW_nalloc(len);
    memcpy((char*) tbl, (char*) ptbl, len);

    /* div */
    len = P_get_len(pack, 1);
    ptbl->divider_line.cells = (char*) P_get_ptr(pack, 1);
    pcell = (TCELL*) ptbl->divider_line.cells;

    tbl->divider_line.cells = SW_nalloc(len);
    cell = (TCELL*) tbl->divider_line.cells;
    memcpy((char*) cell, (char*) pcell, len);

    for(j = 0, p = 2; j < T_NC(tbl); j++) 
    {
        if(!pcell[j].content.empty()) 
        {
            pcell[j].content = (char*) P_get_ptr(pack, p);
            cell[j].content = pcell[j].content;
            p++;
        }
        K_tcell_div_sanitize(cell + j, j);
    }

    /* lines */
    len = P_get_len(pack, p);
    T_L(ptbl) = (TLINE*) P_get_ptr(pack, p);
    p++;

    /* alloc must be a multiple of KT_CHUNCK */
    T_L(tbl) = (TLINE*) SW_nalloc(sizeof(TLINE) * ((int)T_NL(tbl) / KT_CHUNCK + 1) * KT_CHUNCK);
    memcpy(T_L(tbl), T_L(ptbl), len);

    for(i = 0; i < T_NL(tbl); i++) 
    {
        switch(ptbl->lines[i].type) 
        {
            case TABLE_LINE_CELL:
                len = P_get_len(pack, p);
                pcell = (TCELL*) P_get_ptr(pack, p);
                ptbl->lines[i].cells = (char *)pcell;
                cell = (TCELL*) SW_nalloc(len);
                tbl->lines[i].cells = (char *)cell;
                memcpy(cell, pcell, len);
                p++;

                for(j = 0; j < T_NC(tbl); j++)
                {
                    if(!cell[j].content.empty()) 
                    {
                        pcell[j].content = (char*) P_get_ptr(pack, p);
                        cell[j].content = pcell[j].content;
                        p++;
                    }
                    K_tcell_sanitize(cell + j, j);
                }
                break;
            case TABLE_LINE_TITLE:
                len = P_get_len(pack, p);
                /*
                pcell = (TCELL *) ptbl->lines[i].cells = P_get_ptr(pack, p);
                tbl->lines[i].cells = SW_nalloc(len);
                cell = (TCELL*)tbl->lines[i].cells;
                */
                pcell = (TCELL*) P_get_ptr(pack, p);
                ptbl->lines[i].cells = (char*) pcell;
                cell = (TCELL*) SW_nalloc(len);
                tbl->lines[i].cells = (char*) cell;
                memcpy(cell, pcell, len);
                p++;

                if(!cell->content.empty()) 
                {
                    pcell->content = std::string((char*) P_get_ptr(pack, p));
                    cell->content = pcell->content;
                    p++;
                }
                K_tcell_sanitize(cell, 0);
                break;
        }
    }
    return(tbl);
}

/**
 *  Converts a 32 bits table **cell** (TCELL32) to a 64 bits TCELL. 
 *  
 *  @param [in]         tc32 TCELL32 * pointer to the TCELL32 
 *  @param [in, out]    tc64 TCELL *   pointer to the resulting TCELL (64 bits)
 *  @return void
 *  
 *  @note Only used in 64 bit architecture.
 *  
 */
static void K_tcell32_64(TCELL32* tc32, TCELL* tc64)
{
    tc64->content = "";
    tc64->idt = nullptr;
    tc64->type = tc32->type;
    tc64->attribute = tc32->attribute;
}

/**
 *  Converts a 32 bits table **line** (TLINE32) to a 64 bits TLINE. 
 *  
 *  @param [in]         tc32 TLINE32 * pointer to the TLINE32 
 *  @param [in, out]    tc64 TLINE *   pointer to the resulting TLINE (64 bits)
 *  @return void
 *  
 *  @note Only used in 64 bit architecture.
 *  
 */
static void K_tline32_64(TLINE32* tl32, TLINE* tl64)
{
    tl64->type = tl32->type;
    tl64->graph_type = tl32->graph_type;
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
    tbl64->language = tbl32->language;
    tbl64->repeat_columns = tbl32->repeat_columns;
    tbl64->nb_columns = tbl32->nb_columns;
    tbl64->nb_lines = tbl32->nb_lines;

    K_tline32_64((TLINE32*) &tbl32->divider_line, (TLINE*) &tbl64->divider_line);

    tbl64->z_min = tbl32->z_min;
    tbl64->z_max = tbl32->z_max;
    tbl64->y_min = tbl32->y_min;
    tbl64->y_max = tbl32->y_max;
    tbl64->attribute = tbl32->attribute;
    tbl64->chart_box = tbl32->chart_box;
    tbl64->chart_shadow = tbl32->chart_shadow;
    tbl64->chart_gridx = tbl32->chart_gridx;
    tbl64->chart_gridy = tbl32->chart_gridy;
    tbl64->chart_axis_type = tbl32->chart_axis_type;
    tbl64->text_alignment = tbl32->text_alignment;
}

/**
 *   Function to unpack a TBL cell
 */
static int K_tcell_unpack(char *pack, int& p, TCELL *cell, int i, int j)
{
    char* value = (char*) P_get_ptr(pack, p);

    if(cell->type == TABLE_CELL_STRING)
    {
        debug_unpacking("cell", "STR  ", p, i, j, std::string(value));
        cell->content = (value == NULL) ? "" : std::string(value);
        cell->idt = nullptr;
        p++;
        return 0;
    }
    
    if(cell->type == TABLE_CELL_LEC)
    {
        cell->content = "";
        
        // 'value' is a packed IDT
        char* c_lec = (char*) P_get_ptr(value, 0);
        std::string lec = (c_lec == NULL) ? "" : std::string(c_lec);
        debug_unpacking("cell", "LEC  ", p, i, j, lec);
        cell->idt = new Identity(lec);

        // if(!lec.empty())
        // {
        //     CLEC* clec = (CLEC*) P_get_ptr(value, 1);
        //     cell->idt->clec = clec_deep_copy(clec);
        // }

        p++;
        return 0;
    }

    std::string msg = "Unpacking table cell: invalid cell type " + std::to_string(cell->type); 
    msg += " (line " + std::to_string(i) + ", column " + std::to_string(j) + ")";
    kwarning(msg.c_str());
    return 1;
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
    TBL* tbl;
    TCELL* cell;
    TCELL* cells;
    // 32 bits struct (iode std)
    TLINE*   line;
    TLINE32* line32;
    TLINE32* lines32;
    TCELL32* cell32;
    TCELL32* cells32;
    int      i, j, p=0;

    /* tbl */
    tbl = (TBL*) SW_nalloc(sizeof(TBL));
    K_tbl32_64((TBL32*) P_get_ptr(pack, 0), tbl); // Inclut la transposition de divider_line (sans divider_line.cells)
    debug_unpacking("TBL      ", "", 0);

    /* div (TLINE) */
    cells32 = (TCELL32*) P_get_ptr(pack, 1);
    cells = (TCELL*) SW_nalloc(T_NC(tbl) * sizeof(TCELL));
    tbl->divider_line.cells = (char*) cells;
    debug_unpacking("line", "DIV  ", 1);

    for(j = 0, p = 2; j < T_NC(tbl); j++) 
    {
        cell32 = cells32 + j;
        cell = cells + j;
        K_tcell32_64(cell32, cell);
        debug_unpacking("line", "DIV  ", p, -1, j, (cell32->content == 0) ? " NO" : " YES");
        if(cell32->content != 0)
            K_tcell_unpack(pack, p, cell, 0, j);
        else
        {
            cell->content = "";
            cell->idt = nullptr;
        }
        K_tcell_div_sanitize(cells + j, j);
    }
    
    /* lines */
    /* alloc must be a multiple of KT_CHUNCK */
    T_L(tbl) = (TLINE*) SW_nalloc(sizeof(TLINE) * ((int)T_NL(tbl) / KT_CHUNCK + 1) * KT_CHUNCK);
    lines32 = (TLINE32*) P_get_ptr(pack, p);
    debug_unpacking("LINES    ", "", p);
    p++;

    for(i = 0; i < T_NL(tbl); i++) 
    {
        line32 = lines32 + i;
        line = tbl->lines + i;
        K_tline32_64(line32, line);
        switch(line->type) 
        {
            case TABLE_LINE_CELL:
                cells = (TCELL*) SW_nalloc(T_NC(tbl) * sizeof(TCELL));
                line->cells = (char*) cells;
                cells32 = (TCELL32*) P_get_ptr(pack, p);
                debug_unpacking("line", "CELL ", p, i);
                p++;

                for(j = 0; j < T_NC(tbl); j++) 
                {
                    cell32 = cells32 + j;
                    cell = cells + j;
                    K_tcell32_64(cell32, cell);
                    if(cell32->content != 0) 
                        K_tcell_unpack(pack, p, cell, i, j);
                    else
                    {
                        cell->content = "";
                        cell->idt = nullptr;
                    }
                    K_tcell_sanitize(cells + j, j);
                }
                break;

            case TABLE_LINE_TITLE:
                cells = (TCELL*) SW_nalloc(sizeof(TCELL));
                line->cells = (char*) cells;
                cells32 = (TCELL32*) P_get_ptr(pack, p);
                debug_unpacking("line", "TITLE", p, i);
                p++;

                cell32 = cells32 + 0;
                cell = cells + 0;
                K_tcell32_64(cell32, cell);
                if(cell32->content != 0) 
                    K_tcell_unpack(pack, p, cell, i, 0);
                else
                {
                    cell->content = "";
                    cell->idt = nullptr;
                }
                K_tcell_sanitize(cells, 0);
                break;

            case TABLE_LINE_SEP:
                debug_unpacking("line", "SEP  ", p, i);
                break;
            case TABLE_ASCII_LINE_MODE:
                debug_unpacking("line", "MODE ", p, i);
                break;
            case TABLE_LINE_FILES:
                debug_unpacking("line", "FILES", p, i);
                break;
            case TABLE_LINE_DATE:
                debug_unpacking("line", "DATE ", p, i);
                break;

            default:
                std::string msg = "Unpacking table line: invalid line type " + std::to_string(line->type); 
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

TBL* K_tunpack(char *pack)
{
    if(X64)
        return(K_tunpack64(pack));
    else
        return(K_tunpack32(pack));
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

