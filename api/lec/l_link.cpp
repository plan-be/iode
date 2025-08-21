/**
 *  @header4iode
 *
 *  Functions to link CLEC variables and scalars to KDB's of variables and scalars.
 *  This process is required before computing the value of a CLEC instance.
 *
 *  Three functions, implemented in k_lec.c, are called during the link process: L_findvar(), L_findscl() L_getsmpl(). 
 *  The positions of variables and scalars returned by these functions will be used at execution time by L_getvar() and L_getscl().
 *   
 *  Main functions
 * 
 *      int L_link(KDB* dbv, KDB* dbs, CLEC* cl)    Links a CLEC expression to KDB's of variables and scalars. Aligns Period's to the Sample of dbv.
 *      void L_link_endos(KDB *dbe, CLEC *cl)       Pseudo linking used to calculate the strong connex components of a model (SCC).
 */
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/lec/lec.h"


/**
 * First step of linking CLEC to KDBs: each variable and scalar name is searched in the KDB's
 * and their positions are saved in CLEC->lnames.
 *
 * If a name is not found, an error message is added to the stack via IodeErrorManager::append_error 
 * and L_errno is set to L_NOT_FOUND_ERR.
 *
 * @param [in]      dbv     KDB*    KDB of variables
 * @param [in]      dbs     KDB*    KDB of scalars
 * @param [in, out] cl      CLEC*   Compiled LEC
 * @return                  int     0 on success,  L_NOT_FOUND_ERR on error
*/
static int L_link_names(KDB* dbv, KDB* dbs, CLEC* cl)
{
    int     i;

    for (i = 0; i < cl->nb_names; i++) {
        if (L_ISCOEF(cl->lnames[i].name))
            cl->lnames[i].pos = L_findscl(dbs, cl->lnames[i].name);
        else
            cl->lnames[i].pos = L_findvar(dbv, cl->lnames[i].name);
        if (cl->lnames[i].pos < 0) {
            error_manager.append_error(std::string(cl->lnames[i].name) + " : not found");
            return(L_errno = L_NOT_FOUND_ERR);
        }
    }
    return(0);
}


/**
 * Links a CLEC *sub-expression* to the sample of a KDB. 
 * 
 * For example, assume dbv's sample is 2000Y1:2030Y1. 
 * In the LEC expression "A[2001Y1] + 2002Y1":
 *      A[2001Y1] will be interpreted as the value of the 2d element of A
 *      2002Y1 will be replaced by 2 (2000Y1 == 0, 2001Y1 == 1,...)
 * 
 * @param [in]      dbv     KDB*    KDB of variables (only its Sample is needed here)
 * @param [in, out] expr    char*   pointer to the CLEC (sub-)expression
 * @param [in]      lg      int     length of expr
 * 
 * TODO: not implemented for MTFN's ???
*/
static void L_link_sample_expr(KDB* dbv, char* expr, short lg)
{
    int     j, keyw;
    short   len, s;
    CVAR    cvar;
    Sample  *smpl;
    Period  per;

    smpl = L_getsmpl(dbv);
    for(j = 0 ; j < lg ;) {
        keyw = expr[j++];
        switch(keyw) {
            case L_VAR   :
                memcpy(&cvar, expr + j, sizeof(CVAR));
                cvar.ref = cvar.lag;
                if(cvar.per.step != 0)
                    cvar.ref += cvar.per.difference(smpl->start_period);
                memcpy(expr + j, &cvar, sizeof(CVAR));
                j += sizeof(CVAR);
                break;
            case L_COEF      :
                j += sizeof(CVAR);
                break;
            case L_DCONST    :
                j += s_dbl;
                break;
            case L_LCONST    :
                j += s_long;
                break;
            case L_Period    :
                memcpy(&per, expr + j, sizeof(Period));
                s = per.difference(smpl->start_period);
                memcpy(expr + j + sizeof(Period), &s, sizeof(short));
                j += s_short + sizeof(Period);
                break;
            default :
                if(is_fn(keyw)) {
                    j++;
                    break;
                }
                if(is_tfn(keyw)) {
                    memcpy(&len, expr + j + 1, sizeof(short));
                    /*                    len = *(short *)(expr + j + 1); */
                    L_link_sample_expr(dbv, expr + j + 1 + s_short, len);
                    j += 1 + s_short + len;
                    break;
                }
                break;
        }
    }
}


/**
 * Second step of linking CLEC. Each time displacement in the CLEC struct is aligned to the dbv's Sample.
 * For example, the position of A[1970Y1] in the vector A depends on the sample of dbv.
 *
 * @param [in]      dbv     KDB*    KDB of variables
 * @param [in, out] cl      CLEC*   Compiled LEC
 */
static void L_link_sample(KDB* dbv, CLEC* cl)
{
    int     pos;

    if (cl == 0) return;
    pos = sizeof(CLEC) + (cl->nb_names - 1) * sizeof(LNAME);
    L_link_sample_expr(dbv, (char*)cl + pos, (short)(cl->tot_lg - pos));
    return;
}


/**
 * Links a CLEC expression to KDB's of variables and of scalars. If some Period's are present in CLEC,
 * they are aligned to the Sample of dbv.
 * 
 * The CLEC, although modified by L_link(), can be relinked later with other KDB's. 
 *
 * @param [in]      dbv  KDB*        KDB of variables to link to CLEC
 * @param [in]      dbs  KDB*        KDB of scalars to link to CLEC
 * @param [in, out] cl   CLEC*       CLEC expression whose content must be linked to dbv and dbs
 * @return               int         0 on success, L_errno on error
*/
int L_link(KDB* dbv, KDB* dbs, CLEC* cl)
{
    if (cl == 0) return(0);
    if (L_link_names(dbv, dbs, cl)) return(L_errno);
    L_link_sample(dbv, cl);
    return(0);
}


/*---- PSEUDO LINKING FOR THE CONSTRUCTION OF THE MODEL SCC ----*/

/**
 * For each variable in CLEC (lnames) that exists as equation in dbe, set its position (lnames.pos) to the position in dbe.
 *
 * @param [in]      dbe     KDB*     KDB of equations
 * @param [in, out] cl      CLEC*    Compiled lec
 *
 */
static void L_link1_endos(KDB* dbe, CLEC* cl)
{
    int     i;

    for (i = 0; i < cl->nb_names; i++) {
        if (L_ISCOEF(cl->lnames[i].name))
            cl->lnames[i].pos = 0;  // For the SCC construction, we do not need the coefficients (scalars)
        else
            cl->lnames[i].pos = K_find(dbe, cl->lnames[i].name);

        if (cl->lnames[i].pos < 0)  // Not found => exogenous var
            cl->lnames[i].pos = -1; // For the SCC construction, we do not need the exogenous vars positions 
    }
}

/**
 * Pseudo linking used to calculate the strong connex components of a model (SCC).
 * 
 * The endogenous variables are assigned the position of their equation in dbe. 
 * This process constructs a sort of incidence matrix of the model by assigning to the position of the endogenous variables 
 * in the CLEC l_names of each equation (therefore, the KDB of vars and scalars are not needed here).
 *  
 * @param [in]      dbe     KDB*    KDB of equations
 * @param [in, out] cl      CLEC*   compiled LEC expression
 * @return                  int     0 on success, L_errno on error
 * 
 * @author JMP 16/3/2012
 */
void L_link_endos(KDB *dbe, CLEC *cl)
{
    if (cl == 0) return;
    L_link1_endos(dbe, cl);
}
