/**
 * @header4iode
 *
 * IODE object management 
 * ----------------------
 *
 *     int K_key(char* name, int mode):                             Checks the validity of an object name and modify its "case" according to the value of mode.
 *     int K_dup(KDB* kdb1, char* name1, KDB* kdb2, char* name2):   Duplicates an IODE object. Copies name1 of kdb1 to name2 in kdb2.
 *     int K_ren(KDB* kdb, char* name1, char* name2):               Renames the object name1 in kdb into name2.
 *     int K_add_entry(KDB* kdb, char* name):                       Adds the entry name in kdb and returns its position in the kdb. 
 *     int K_find(KDB* kdb, char* name):                            Searches the position of an object name in a KDB.
 *     int K_del_entry(KDB* kdb, int pos):                          Deletes an entry in a KDB **without** deleting the referenced object. 
 *     int K_del(KDB* kdb, int pos):                                Deletes an object (and its data) from a KDB.
 *     int K_del_by_name(KDB* kdb, char* name):                     Deletes an object identified by its name from a KDB. 
 *     int K_upd_eqs(char* name, char* lec, char* cmt, int method, Sample* smpl, char* instr, char* blk, float* tests, int date)  Updates equation field(s). Creates the equation if it doesn't exist).
 *     int K_upd_tbl(char* name, char* arg)                         Creates a basic table with an optional TITLE and optional variable names and/or lec formulas separated by semi-colons.
 */
#include "scr4/s_args.h"
#include "scr4/s_prodt.h"

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/utils/buf.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"
#include "api/objs/grep.h"
#include "api/objs/objs.h"
#include "api/objs/equations.h"
#include "api/objs/tables.h"
#include <algorithm>    // for std::min, std::max


// Utilities 
// ---------

/**
 *  Compares a name with the name of an object (kobjs->o_name).
 *  
 *  @param [in] name    const void*     name to compare
 *  @param [in] kobjs   const KOBJ*     pointer to a general object (KOBJ)
 *  @return             int             0 if name of p1 = name of p2 (the names ares equal)
 *                                      -1 if p1 is < p2 or p1 is null
 *                                      1 if p1 is > p2 or p2 is null and is not null
 *  @return 
 *  
 *  @details 
 */

static int K_find_strcmp(const void *name, const void *kobjs)
{
    KOBJ    *ko2 = (KOBJ *)kobjs;
    return(strcmp((char *)name, ko2->o_name));
}

// API
// ---


/**
 *  Checks the validity of an object name and modify its "case" according to the value of mode.
 *  The name is truncated if it exceeds K_MAX_NAME characters.
 *  
 *  
 *  
 *  @param [in, out] name    char*   name to check
 *  @param [in]      mode    int     UPPER_CASE, LOWER_CASE of ASIS_CASE 
 *  @return                  int     0 if the name is valid
 *                                   -1 otherwise (illegal character)
 *  
 *  @details 
 */
 
int K_key(char* name, int mode)
{
    int     i;

    SCR_sqz((unsigned char*) name);
    if(!SCR_is_alpha(name[0]) && name[0] != '_') return(-1);
    if(strlen(name) > K_MAX_NAME) name[K_MAX_NAME] = 0;
    for(i = 1 ; name[i] ; i++)
        if(!SCR_is_anum(name[i]) && name[i] != '_' && name[i] != K_SECRETSEP) return(-1); // JMP 14/2/2013 pour les macros pushed A#n

    switch(mode) {
        case UPPER_CASE :
            SCR_upper((unsigned char*) name);
            break;
        case LOWER_CASE :
            SCR_lower((unsigned char*) name);
            break;
        case ASIS_CASE  :
            break;
    }

    return(0);
}


/**
 *  Duplicates an IODE object. Copies name1 of kdb1 to name2 in kdb2.
 *  If name2 exists in kdb2, it is replaced by the value of name1.
 *  
 *  @param [in]         kdb1    KDB*    KDB source    
 *  @param [in]         name1   char*   object name of the source in kdb1
 *  @param [in, out]    kdb2    KDB*    KDB target
 *  @param [in]         name2   char*   name of the new object
 *  @return                     int     position of name2 in kdb2 in case of success
 *                                      -1 if name1 does not exist in kdb1
 *                                      -2 if name2 cannot be added to the kdb2 (memory...)
 */

int K_dup(const KDB* kdb1, char* name1, KDB* kdb2, char* name2)
{
    int     pos1, pos2, lg;
    char    *pack, *ptr;

    if(kdb1 == kdb2 && strcmp(name1, name2) == 0) 
        return(-2);   // ALD 19/09/2022

    pos1 = K_find((KDB*) kdb1, name1);
    if(pos1 < 0) 
        return(-1);

    pos2 = K_find(kdb2, name2);
    if(pos2 >= 0) 
    {
        if(KSOVAL(kdb2, pos2) != 0) 
            SW_free(KSOVAL(kdb2, pos2));
    }
    else 
    {
        pos2 = K_add_entry(kdb2, name2);
        pos1 = K_find((KDB*) kdb1, name1);
    }

    if(pos2 < 0) 
        return(-2);

    pack = KGOVAL(kdb1, pos1);
    if(pack == NULL)
        return(-2);
    
    lg = * (OSIZE *) pack;
    ptr = SW_nalloc(lg);
    memcpy(ptr, pack, lg);

    KSOVAL(kdb2, pos2) = SW_alloc(lg);
    memcpy(KGOVAL(kdb2, pos2), ptr, lg);

    SW_nfree(ptr);
    return(pos2);
}


/**
 *  Renames in kdb the object name1 into name2.
 *  If name2 exists, returns -1.
 *  
 *  @param [in, out]    kdb     KDB*    KDB source and target
 *  @param [in]         name1   char*   original object name 
 *  @param [in]         name2   char*   new object name
 *  @return                     int     position of name2 in kdb
 *                                      -1 if name1 does not exist in kdb
 *                                      -2 if name2 exists in kdb
 *                                      -3 if name2 cannot be created in kdb
 *  TODO: Reject equations (cannot be renamed)
 */
 
int K_ren(KDB* kdb, char* name1, char* name2)
{
    int     pos1, pos2;

    if(kdb == NULL) return(-1);
    pos1 = K_find(kdb, name1);
    if(pos1 < 0) return(-1);

    pos2 = K_find(kdb, name2);
    if(pos2 >= 0) return(-2);

    pos2 = K_add_entry(kdb, name2);
    if(pos2 < 0) return(-3);
    pos1 = K_find(kdb, name1); /* object name1 may have changed after add name2 */

    KSOVAL(kdb, pos2) = KSOVAL(kdb, pos1);
    K_del_entry(kdb, pos1);
    
    pos2 = K_find(kdb, name2); // JMP 16/1/2022 suite à une erreur détectée par ALD
    
    return(pos2);
}


/**
 *  Adds the new entry newname in kdb and returns its position in the kdb. 
 *  If newname exists, returns its position in kdb and if K_WARN_UP is not null, displays an error message.
 *
 *  The kdb can be of any type but the name must comply to the naming conventions of kdb's type (UPPER, LOWER...).
 *  
 *  @detail  The object names are stored in the table KOBJS(kdb). To avoid a reallocation on each new insertion, K_CHUNCK elements 
 *          are added to KOBJS(kdb) each time that more place is needed to store object names.
 *
 *          Names in KOBJS are stored in alphabetic order to speed up the retrieval of an object by its name. 
 *          Consequently, K_add_entry has to calculate the place where the name must be inserted. For (even) more speed when 
 *          elements are added in alphabetic order, the function first checks that the new name must not be placed at the end or the beginning 
 *          of the table in which case, no search is required.
 * 
 *  @param [in, out]    kdb     KDB*    KDB source and target
 *  @param [in]         newname char*   new object name 
 *  @return                     int     position of name in kdb on success
 *                                      -1 if name does not comply to the name syntax in the kdb 
 */
 
int K_add_entry(KDB* kdb, char* newname)
{

    int     lg, pos, maxpos, minpos, nbobjs;
    KOBJ    *ktmp;
    ONAME   name;

    if(kdb == NULL) return(-1);
    SCR_strlcpy((unsigned char*) name, (unsigned char*) newname, K_MAX_NAME);  /* JMP 13-02-2013 */
    if(K_key(name, kdb->k_mode) < 0) return(-1);
    pos = K_find(kdb, name);
    if(pos >= 0) {
        if(K_WARN_DUP)
            kerror(0, "%s defined more than once", name);
        return(pos);
    }

    if((KNB(kdb)) % K_CHUNCK == 0)
        //KOBJS(kdb) = (char *) SW_nrealloc(KOBJS(kdb),
        KOBJS(kdb) = (KOBJ*) SW_nrealloc(KOBJS(kdb),
                                         (unsigned int)(sizeof(KOBJ) * KNB(kdb)),
                                         (unsigned int)(sizeof(KOBJ) * (KNB(kdb) + K_CHUNCK)));
    if(KOBJS(kdb) == 0) return(-1);


    if(KNB(kdb) == 0) { /* JMP 09-06-00 */
        maxpos = 0;
        goto done;
    }

    if(K_find_strcmp(name, KOBJS(kdb) + KNB(kdb) - 1) > 0) {
        maxpos = KNB(kdb);
        goto done;
    }

    if(K_find_strcmp(name, KOBJS(kdb)) < 0) {
        /* add before */
        maxpos = 0;
        goto done;
    }

    /* insert */
    maxpos = KNB(kdb);
    minpos = 0;
    while(maxpos - minpos > 1) {
        pos = minpos + (maxpos - minpos)/2;
        if(K_find_strcmp(name, KOBJS(kdb) + pos) < 0) maxpos = pos;
        else minpos = pos;
    }

done :
   nbobjs =  KNB(kdb) - maxpos;
    if(nbobjs != 0) {
        ktmp = (KOBJ *) BUF_alloc(nbobjs * sizeof(KOBJ));
        memcpy((char *)ktmp, (char *)(KOBJS(kdb) + maxpos), nbobjs * sizeof(KOBJ));
        memcpy((char *)(KOBJS(kdb) + maxpos + 1), (char *)ktmp, nbobjs * sizeof(KOBJ));
        //SCR_free(ktmp);
    }

    lg = std::min((int) strlen(name), K_MAX_NAME);
    memcpy(KONAME(kdb, maxpos), name, lg + 1);
    KSOVAL(kdb, maxpos) = 0;

    KNB(kdb)++;

    return(maxpos);
}


/**
 *  Searches the position of an object in a KDB by its name.
 *  
 *  @param [in] kdb     KDB*    where to search for name
 *  @param [in] name    char*   name to be searched
 *  @return             int     position of the name in kdb
 *                              -1 if not found or if the name does not comply to the rules of kdb->k_type.
 */
 
int K_find(KDB* kdb, char* name)
{
    char    *res;
    ONAME   oname;

    if(kdb == NULL || KNB(kdb) == 0) return(-1);

    SCR_strlcpy((unsigned char*) oname, (unsigned char*) name, K_MAX_NAME);  
    if(K_key(oname, kdb->k_mode) < 0) return(-1);

    res = (char *) bsearch(oname, KOBJS(kdb), (int) KNB(kdb),
                           sizeof(KOBJ), K_find_strcmp);
    if(res != 0) return((int)((res - (char *) KOBJS(kdb)) / sizeof(KOBJ)));
    else return(-1);
}


/**
 *  Deletes an entry in a KDB __without__ deleting the referenced object. 
 *
 *  Warning : for interal use (no check is made on pos).
 *  
 *  @param [in, out]    kdb     KDB*    KDB source and target
 *  @param [in]         pos     int     object position in kdb
 *  @return                     int     0 (should be void fn)
 */

int K_del_entry(KDB* kdb, int pos)
{
    memcpy(KOBJS(kdb) + pos, KOBJS(kdb) + (pos + 1),
           (int)(KNB(kdb) - pos - 1) * sizeof(KOBJ));
    KNB(kdb)--;
    if(KNB(kdb) > 0) {
        memset(KOBJS(kdb) + (int) KNB(kdb), 0, sizeof(KOBJ));
        if(KNB(kdb) % K_CHUNCK == 0)
            KOBJS(kdb) = (KOBJ *) SW_nrealloc((char *)KOBJS(kdb),
                                              (unsigned int)(sizeof(KOBJ) * (KNB(kdb) + K_CHUNCK)),
                                              (unsigned int)(sizeof(KOBJ) * KNB(kdb)));
    }
    else {
        SW_nfree(KOBJS(kdb));
        KOBJS(kdb) = NULL;
    }
    return(0);
}


/**
 *  Deletes an object identified by its position from a KDB. 
 *  The object content is also deleted.
 *
 *  @param [in, out] kdb    KDB*    source and target KDB
 *  @param [in]      pos    int     object position in kdb
 *  @return                 int     0 or 
 *                                  -1 if pos does not exist in kdb
 */
 
int K_del(KDB* kdb, int pos)
{
    if(kdb == NULL) return(-1);
    if(pos < 0 || pos >= KNB(kdb)) return(-1);
    SW_free(KSOVAL(kdb, pos));
    K_del_entry(kdb, pos);
    return(0);
}


/**
 *  Deletes an object identified by its name from a KDB. 
 *  The object content is also deleted. 
 *  
 *  @param [in] kdb     KDB*    where to search for name
 *  @param [in] name    char*   object name
 *  @return             int     0 or 
 *                              -1 if pos does not exist in kdb
 * 
 */
 
int K_del_by_name(KDB* kdb, char* name)
{
    return(K_del(kdb, K_find(kdb, name))); 
}


/**
 *  Updates equation field(s). Creates the equation if it doesn't exist). All fields do not have to be updated during one call: only the non NULL
 *  parameters are taken into account.
 *  
 *  @param [in] char*   name    equation name (also endogenous)
 *  @param [in] char*   lec     NULL or LEC equation    
 *  @param [in] char*   cmt     NULL or free comment
 *  @param [in] int     method  -1 or estimation method
 *  @param [in] Sample* smpl    NULL or estimation sample
 *  @param [in] char*   instr   NULL or list of instruments 
 *  @param [in] char*   blk     NULL or list of simultaneously esstimated equations (block) 
 *  @param [in] float*  tests   NULL or vector of statistical tests
 *  @param [in] int     date    0 or last estimation date
 *  @return     int             0 on success, -1 on error
 *   
 */
int K_upd_eqs(char* name, char* c_lec, char* cmt, int i_method, Sample* smpl, char* instr, char* blk, float* tests, int i_date)
{
    int pos, rc;

    std::string endo(name);
    std::string lec;
    std::string comment;
    std::string instruments;
    std::string block;
    IodeEquationMethod method = EQ_LSQ;

    if(c_lec != NULL)
        lec = std::string(c_lec);

    if(cmt != NULL)
        comment = std::string(cmt);

    if(instr != NULL)
        instruments = std::string(instr);

    if(blk != NULL)
        block = std::string(blk);

    if(i_method >= 0 && i_method < IODE_NB_EQ_METHODS) 
        method = (IodeEquationMethod) i_method;

    Equation* eq;
    pos = K_find(KE_WS, name);
    if(pos < 0)
    {
        Period from_period = (smpl !=  NULL) ? smpl->start_period : Period();
        Period to_period = (smpl !=  NULL) ? smpl->end_period : Period();
        eq = new Equation(endo, lec, method, from_period.to_string(), to_period.to_string(), 
                          comment, instruments, block, false);
    } 
    else
    {
        eq = KEVAL(KE_WS, pos);
        // modify only if not empty
        if(!lec.empty())
            eq->set_lec(lec);
        // modify only if not null (negative)
        if(i_method >= 0)
            eq->set_method(method);
        // modify only if not empty
        if(!comment.empty()) 
            eq->set_comment(comment);
        // modify only if not empty
        if(!instruments.empty())
            eq->instruments = instruments;
        // modify only if not empty
        if(!block.empty())
            eq->block = block;
        // modify only if not null
        if(smpl) 
            eq->sample = *smpl;
    }

    if(tests != NULL)  
        memcpy(eq->tests.data(), tests, EQS_NBTESTS * sizeof(float));

    if(i_date > 0)
        eq->update_date();

    rc = K_add(K_WS[EQUATIONS], name, eq, name);
    delete eq;
    eq = nullptr;
    if(rc < 0) 
    {
        error_manager.append_error(std::string(L_error()));
        return -1;
    }
    
    return 0;
}


char T_SEPS[] = ";\n\t";


/**
 *  Creates a basic table with an optional TITLE and optional variable names and/or lec formulas separated by semi-colons.
 *  
 *  For the variables having a comment (with the same name) in the current CMT WS, the line title is replaced by the comment.
 *  For the LEC formulas and the other variables, the line titles are the formulas / var names.
 *  
 *  @param [in] char*  name     name of the new table
 *  @param [in] char*  arg      initial parameters separated by semi-colons: "Title;LEC_line1;LEC_line2..."
 *  @return     int             0 on success, -1 on error        
 *  
 */
int K_upd_tbl(char* name, char* arg)
{
    Table* tbl = nullptr;

    if(arg == NULL)
        tbl = new Table(2);
    else
    {
        char* OLD_SEPS = A_SEPS;
        A_SEPS = T_SEPS;
        std::vector<std::string> v_args = split_multi(std::string(arg), std::string(A_SEPS));
        A_SEPS = OLD_SEPS;

        if(v_args.size() == 0)
            tbl = new Table(2);
        else
        {
            std::string title = v_args.front();
            v_args.erase(v_args.begin());
            tbl = new Table(2, title, v_args, false, false, false);
        }
    }

    int pos = K_add(K_WS[TABLES], name, tbl);
    delete tbl;
    tbl = nullptr;

    return (pos >= 0) ? 0 : -1;
}
