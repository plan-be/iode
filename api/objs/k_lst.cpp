/**
 * @header4iode
 *
 *  Basic functions to manipulate lists and to extract lists of VARs and Scalars from IODE objects.
 *  
 *  List of functions 
 *  -----------------
 *    int K_scan(KDB* kdb, char* l_var, char* l_scal)       Analyses a KDB content and creates 2 lists with all VAR and all Scalar found in the kdb objects (limited to IDT, EQ or Table).
 *    void KE_scan(KDB* dbe, int i, KDB* exo, KDB* scal)    Analyses object i from a KDB of EQs and extracts all VARs and all Scalars from the CLEC struct.
 *    void KI_scan(KDB* dbi, int i, KDB* exo, KDB* scal)    Analyses object i from a KDB dbi of IDTs and extracts all VARs and all Scalars from the LEC expression.
 *    void KT_scan(KDB* dbt, int i, KDB* exo, KDB* scal)    Analyses object i from a KDB of Tables and extracts all VARs and all Scalars from the LEC expressions found in the TableCells.
 *    int KL_lst(char* name, char** lst, int chunck)        Creates a list from a table of strings. The elements in the new list are separated by semi-colons.
 *    unsigned char **KL_expand(char *str)                  Replaces recursively list names in a string. Returns a table containing all terms in the string after replacement.
 */ 
#include "api/b_errors.h"
#include "api/lec/lec.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/grep.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/tables.h"


/**
 *  Analyses a KDB content and creates 2 lists with all VAR and all Scalar found in the kdb objects (limited to IDT, EQ or Table).
 *  
 *  @param [in] KDB*  kdb    KDB to be analysed. Only possible for IDT, EQ or Table KDB.
 *  @param [in] char* l_var  name of the list that will contain the resulting list of VAR
 *  @param [in] char* l_scal name of the list that will contain the resulting list of Scalar
 *  @return     int        
 *  
 */
int K_scan(KDB* kdb, char* l_var, char* l_scal)
{
    int     rc = 0, i;
    char    **lst;
    KDB     *exo = NULL, *scal = NULL;

    if(kdb == nullptr || kdb->size() == 0) 
    {
        std::string error_msg = "scan : '";
        error_msg += v_iode_types[kdb->k_type];
        error_msg += "' database is empty";
        error_manager.append_error(error_msg);
        return(-1);
    }

    exo = new KDB(VARIABLES, DB_STANDALONE);
    scal = new KDB(SCALARS, DB_STANDALONE);

    if(exo == nullptr || scal == nullptr) 
    {
        rc = -1;
        goto done;
    }

    for(i = 0; i < kdb->size(); i++) 
    {
        switch(kdb->k_type) 
        {
            case IDENTITIES :
                KI_scan(kdb, i, exo, scal);
                break;
            case EQUATIONS :
                KE_scan(kdb, i, exo, scal);
                break;
            case TABLES :
                KT_scan(kdb, i, exo, scal);
                break;
        }

    }

    lst = K_grep(scal, "*", 1, 1, 0, 0, '*');
    rc =  KL_lst(l_scal, lst, 200);
    SCR_free_tbl((unsigned char**) lst);

    lst = K_grep(exo, "*", 1, 1, 0, 0, '*');
    rc = KL_lst(l_var, lst, 200);
    SCR_free_tbl((unsigned char**) lst);

done:
    delete scal;
    delete exo;
    return rc;
}


/**
 *  Extracts exogenous variables and scalars from a CLEC expression. 
 *  Adds the result to 2 KDBs: exo for the exogenous and scal for the scalars.
 *  
 *  The KDB of EQs is needed to determine if a VAR is an endogenous (found in dbe) or an exogenous (not present in dbe). 
 *  Only the exogenous variables are saved in exo.
 *  
 *  @param [in]      KDB*  dbe      KDB of equations used to check if a VAR is exo or endo
 *  @param [in]      CLEC* cl       compiled LEC expression or equation
 *  @param [in, out] KDB*  exo      KDB of exogenous (only the names, not the values)
 *  @param [in, out] KDB*  scal     KDB of scalars (only the names, not the values)
 *  @return          void 
 *  
 *  @details 
 */
static void K_clecscan(KDB* dbe, CLEC* cl, KDB* exo, KDB* scal)
{
    if(cl == NULL) 
        return;
    
    int rc = 0;
    char* c_name;
    std::string name;
    for(int j = 0 ; j < cl->nb_names ; j++) 
    {
        c_name = cl->lnames[j].name;
        if(is_coefficient(c_name))
            K_add(scal, c_name, NULL, &rc);
        else 
        {
            name = std::string(c_name);
            if(dbe != nullptr && dbe->contains(name)) 
                continue;
            K_add(exo, cl->lnames[j].name, NULL, &rc);
        }
    }
}


/**
 *  Analyses object i from a KDB of EQs and extracts all VARs and all Scalars from the CLEC struct.
 *  The result is added to 2 KDB of type OBJECTS (i.e.: no type), no type meaning that only the object names are relevant.
 *  
 *  @param [in]      KDB* dbe    KDB of equations
 *  @param [in]      int  i      position of the equation in the dbe
 *  @param [in, out] KDB* exo    KDB (of type OBJECTS == no type) containing all VAR names found in dbe[i]
 *  @param [in, out] KDB* scal   KDB (of type OBJECTS == no type) containing all Scalar names found in dbe[i]
 *  @return          void
 *  
 */
void KE_scan(KDB* dbe, int i, KDB* exo, KDB* scal)
{
    std::string name = dbe->get_name(i);
    Equation* eq = KEVAL(dbe, name);
    CLEC* cl = eq->clec;
    K_clecscan(dbe, cl, exo, scal);
    delete eq;
    eq = nullptr;
}


/**
 *  Analyses object i from a KDB dbi of IDTs and extracts all VARs and all Scalars from the LEC expression.
 *  
 *  The result is added to 2 KDB of type OBJECTS (i.e.: no type), no type meaning that only the object names are relevant.
 *  
 *  @param [in]      KDB* dbi    KDB of identities
 *  @param [in]      int  i      position of the identity in the dbi
 *  @param [in, out] KDB* exo    KDB (of type OBJECTS == no type) containing all VAR names found in dbi[i]
 *  @param [in, out] KDB* scal   KDB (of type OBJECTS == no type) containing all Scalar names found in dbi[i]
 *  @return          void
 *  
 */
void KI_scan(KDB* dbi, int i, KDB* exo, KDB* scal)
{
    int     lg;
    CLEC    *cl = NULL;
    std::string name = dbi->get_name(i);

    lg = KICLEC(dbi, name)->tot_lg;
    cl = (CLEC *) SW_nalloc(lg);
    memcpy(cl, KICLEC(dbi, name), lg);

    K_clecscan(dbi, cl, exo, scal);
    SW_nfree(cl);
}


/**
 *  Analyses object i from a KDB of Tables and extracts all VARs and all Scalars from the LEC expressions found in the TableCells.
 *  
 *  The result is added to 2 KDB of type OBJECTS (i.e.: no type), no type meaning that only the object names are relevant.
 *  
 *  @param [in]      KDB* dbt    KDB of tables
 *  @param [in]      int  i      position of the table in the dbt
 *  @param [in, out] KDB* exo    KDB (of type OBJECTS == no type) containing all VAR names found in dbt[i]
 *  @param [in, out] KDB* scal   KDB (of type OBJECTS == no type) containing all Scalar names found in dbt[i]
 *  @return          void
 *  
 */
void KT_scan(KDB* dbt, int i, KDB* exo, KDB* scal)
{
    std::string name = dbt->get_name(i);
    Table* tbl = KTVAL(dbt, name);

    CLEC* clec = NULL;
    for(int k = 0; k < T_NL(tbl); k++)   
    {
        if(tbl->lines[k].get_type() != TABLE_LINE_CELL) 
            continue;
        
        for(TableCell& cell: tbl->lines[k].cells) 
        {
            if(cell.get_type() != TABLE_CELL_LEC) 
                continue;

            clec = cell.get_compiled_lec();
            K_clecscan(NULL, clec, exo, scal);
        }
    }

    delete tbl;
    tbl = nullptr;
}


/**
 *  Creates a list from a table of strings. The elements in the new list are separated by semi-colons.
 *  If the list exceeds chunck strings, it is split into sublists with each max chunck elements. The
 *  sublists have the same name with a numerical suffix.
 *  
 *  Example: 
 *      char** tbl;
 *      tbl = SCR_vtom("A,B,C,D,E,F,G,H", ',');
 *      KL_lst("MYLIST", tbl, 5); // Creates 3 lists : 
 *                                //     MYLIST  => "$MYLIST1;$MYLIST2"
 *                                //     MYLIST1 => "A,B,C,D,E"
 *                                //     MYLIST2 => "F,G,H"
 *    
 *  @param [in] char*   name    name or the resulting list
 *  @param [in] char**  lst     table of strings
 *  @param [in] int     chunck  max number of strings per (sub-)list
 *  @return     int             0 on success, < 0 on, error (rc from K_add())
 *  
 */
int KL_lst(char* name, char** lst, int chunck)
{
    int     rc = 0, i, j, nb;
    char    *str, *ptr, buf[30];

    nb = SCR_tbl_size((unsigned char**) lst);
    if(nb == 0) 
    {
        if(!K_add(K_WS[LISTS], name, ""))    
            rc = -1;
        goto done;
    }

    if(nb < chunck || chunck < 0) 
    {
        str = (char*) SCR_mtov((unsigned char**) lst, (int) ';'); /* JMP 09-03-95 */
        if(!K_add(K_WS[LISTS], name, str))  
            rc = -1;
        SCR_free(str);
        return(rc);
    }

    for(i = 0, j = 0; i < nb && !rc; i+= chunck, j++) 
    {
        if(i + chunck < nb) 
        {
            ptr = lst[i + chunck];
            lst[i + chunck] = NULL;
        }

        str = (char*) SCR_mtov((unsigned char**) lst + i, ';');
        sprintf(buf, "%s%d", name, j);
        buf[K_MAX_NAME] = 0;
        if(!K_add(K_WS[LISTS], buf, str))  
            rc = -1;
        SCR_free(str);

        if(i + chunck < nb)  
            lst[i + chunck] = ptr;
    }
    if(rc < 0) 
        goto done;

    str = (char *) SW_nalloc(j * (K_MAX_NAME + 2));
    str[0] = 0;
    for(i = 0; i < j; i++) 
    {
        sprintf(buf, "$%s%d;", name, i); /* GB 23/01/98 */
        buf[K_MAX_NAME] = 0;
        strcat(str, buf);
    }
    if(!K_add(K_WS[LISTS], name, str)) 
        rc = -1;
    SW_nfree(str);

done:
    return(rc);
}


/**
 *  Replaces recursively list names in a string. Returns a table containing all terms in the string after replacement.
 *  The lists are searched in the current LST KDB.
 *  
 *  Example:
 *      Suppose we have the lists below in the current KDB of lists:
 *          A = "$A1;$A2"
 *          A1 = "X;Y"
 *          A2 = "Z"
 *  
 *      KL_expand("$A B C") => "X", "Y", "Z", "B", "C"
 *  
 *  @param [in] char*   str     list of terms separated by one of the chars in " ,;\t\n\r\f" and 
 *                              containing IODE lists under the form of $LISTNAME.
 *  @return     char**          table containing all terms in str after replacement.
 *  
 */
unsigned char **KL_expand(char *str)
{
    unsigned char 	**tbl, **tbl2;
    unsigned char*  seps = (unsigned char*) " ,;\t\n\r\f";
    int				i, nb, nb2;

    tbl = SCR_vtoms((unsigned char*) str, seps);
    nb = SCR_tbl_size(tbl);
    if(SCR_tbl_size(tbl) == 0) 
        return(tbl);
    
    std::string list_name;
    for(i = 0 ; tbl[i] ; i++) 
    {
        if(tbl[i][0] == '$') 
        {
            list_name = std::string((char*) tbl[i] + 1);
            if(K_WS[LISTS]->contains(list_name))
            {
                SCR_free(tbl[i]); // plus besoin car remplacé par sa valeur
                tbl2 = KL_expand(KLVAL(K_WS[LISTS], list_name));
                nb2 = SCR_tbl_size(tbl2);
                // Insertion dans tbl de la liste à la place de tbl[i]
                tbl = (unsigned char **) SCR_realloc(tbl, sizeof(char *), nb + 1, (nb + 1 - 1) + nb2);
                // décaler ceux qui suivent i (nb -i - 1 éléments) de nb2 places vers la droite
                memcpy(tbl + i + nb2, tbl + i + 1, sizeof(char *) * (nb - i - 1));
                // recopier tbl2 à la place de i et suivants
                memcpy(tbl + i, tbl2, sizeof(char *) * nb2);
                nb += nb2 - 1; // -1 car on remplace le courant
                SCR_free(tbl2); // pas tbl_free car ptrs copiés dans tbl
            }
        }
    }
    return(tbl);
}
