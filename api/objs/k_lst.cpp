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
#include "api/objs/scalars.h"
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
    if(kdb == nullptr || kdb->size() == 0) 
    {
        std::string error_msg = "scan : '";
        error_msg += v_iode_types[kdb->k_type];
        error_msg += "' database is empty";
        error_manager.append_error(error_msg);
        return -1;
    }

    KDBVariables* exo = new KDBVariables(false);
    KDBScalars* scal = new KDBScalars(false);

    if(exo == nullptr || scal == nullptr) 
    {
        if(scal) delete scal;
        if(exo)  delete exo;
        return -1;
    }

    for(int i = 0; i < kdb->size(); i++) 
    {
        switch(kdb->k_type) 
        {
            case IDENTITIES :
                KI_scan((KDBIdentities*) kdb, i, exo, scal);
                break;
            case EQUATIONS :
                KE_scan((KDBEquations*) kdb, i, exo, scal);
                break;
            case TABLES :
                KT_scan((KDBTables*) kdb, i, exo, scal);
                break;
        }
    }

    int rc = -1;
    char** c_lst;
    std::vector<std::string> lst;

    lst = scal->grep("*", true, true, false, false, '*');
    c_lst = vector_to_double_char(lst);
    rc =  KL_lst(l_scal, c_lst, 200);
    SCR_free_tbl((unsigned char**) c_lst);

    lst = exo->grep("*", true, true, false, false, '*');
    c_lst = vector_to_double_char(lst);
    rc = KL_lst(l_var, c_lst, 200);
    SCR_free_tbl((unsigned char**) c_lst);

    delete scal;
    delete exo;
    return 0;
}


/**
 *  Extracts exogenous variables and scalars from a CLEC expression. 
 *  Adds the result to 2 KDBs: exo for the exogenous and scal for the scalars.
 *  
 *  The KDB of EQs is needed to determine if a VAR is an endogenous (found in dbe) or an exogenous (not present in dbe). 
 *  Only the exogenous variables are saved in exo.
 *  
 *  @param [in]      KDB*  kdb      KDB of identities or equations used to check if a VAR is exo or endo
 *  @param [in]      CLEC* cl       compiled LEC expression or equation
 *  @param [in, out] KDB*  exo      KDB of exogenous (only the names, not the values)
 *  @param [in, out] KDB*  scal     KDB of scalars (only the names, not the values)
 *  @return          void 
 *  
 *  @details 
 */
static void K_clecscan(KDB* kdb, CLEC* cl, KDBVariables* exo, KDBScalars* scal)
{
    if(cl == NULL) 
        return;

    char* c_name;
    std::string name;
    for(int j = 0 ; j < cl->nb_names ; j++) 
    {
        c_name = cl->lnames[j].name;
        name = std::string(c_name);
        if(is_coefficient(name))
            // add dummy value for the scalar. The value is not relevant 
            // as only the name will be used in the list of scalars.
            scal->set_obj_ptr(name, new Scalar());
        else 
        {
            if(kdb != nullptr && kdb->contains(name)) 
                continue;
            // add dummy value for the variable. The value is not relevant 
            // as only the name will be used in the list of variables.
            exo->set_obj_ptr(name, new Variable());
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
void KE_scan(KDBEquations* dbe, int i, KDBVariables* exo, KDBScalars* scal)
{
    std::string name = dbe->get_name(i);
    Equation* eq = dbe->get_obj_ptr(name);
    CLEC* cl = eq->clec;
    K_clecscan(dbe, cl, exo, scal);
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
void KI_scan(KDBIdentities* dbi, int i, KDBVariables* exo, KDBScalars* scal)
{
    std::string name = dbi->get_name(i);
    CLEC* cl_idt = dbi->get_obj_ptr(name)->get_compiled_lec();

    int lg = cl_idt->tot_lg;
    CLEC* cl = (CLEC *) SW_nalloc(lg);
    memcpy(cl, cl_idt, lg);

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
void KT_scan(KDBTables* dbt, int i, KDBVariables* exo, KDBScalars* scal)
{
    std::string name = dbt->get_name(i);
    Table* tbl = dbt->get_obj_ptr(name);

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
            K_clecscan(nullptr, clec, exo, scal);
        }
    }
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
 *  @return     int             0 on success, < 0 on, error (rc from add())
 *  
 */
int KL_lst(char* name, char** lst, int chunck)
{
    int     rc = 0, i, j, nb;
    char    *str, *ptr, buf[30];

    nb = SCR_tbl_size((unsigned char**) lst);
    if(nb == 0) 
    {
        try
        {
            global_ws_lst->set_obj_ptr(name, new List(""));    
        }
        catch(const std::exception& e)
        {
            kwarning(e.what());
            rc = -1;
        }
        goto done;
    }

    if(nb < chunck || chunck < 0) 
    {
        str = (char*) SCR_mtov((unsigned char**) lst, (int) ';');
        try
        {
            global_ws_lst->set_obj_ptr(name, new List(str)); 
        }
        catch(const std::exception& e)
        {
            kwarning(e.what());
            rc = -1;
        }
        SCR_free(str);
        return rc;
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
        try
        {
            global_ws_lst->set_obj_ptr(buf, new List(str));
        }
        catch(const std::exception& e)
        {
            kwarning(e.what());
            rc = -1;
        }
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

    try
    {
        global_ws_lst->set_obj_ptr(name, new List(str));
    }
    catch(const std::exception& e)
    {
        kwarning(e.what());
        rc = -1;
    }

    SW_nfree(str);

done:
    return rc;
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
    List* list_ptr = nullptr;
    char* c_list = NULL;
    for(i = 0 ; tbl[i] ; i++) 
    {
        if(tbl[i][0] == '$') 
        {
            list_name = std::string((char*) tbl[i] + 1);
            if(global_ws_lst->contains(list_name))
            {
                SCR_free(tbl[i]); // plus besoin car remplacé par sa valeur
                list_ptr = global_ws_lst->get_obj_ptr(list_name);
                c_list = (char*) list_ptr->c_str();
                tbl2 = KL_expand(c_list);
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

bool KDBLists::binary_to_obj(const std::string& name, char* pack)
{
    size_t len = (size_t) P_get_len(pack, 0);
    char* value = new char[len];
    strncpy(value, (char*) P_get_ptr(pack, 0), len);

    List* list = new List(value);
    this->k_objs[name] = list;
    return true;
}

/**
 * Serializes a list object. 
 *
 * @param [out] pack    (char **)   placeholder for the pointer to the serialized object
 * @param [in]  name    string      list name
 * @return                          true if the serialization succeeded, false otherwise 
 */
bool KDBLists::obj_to_binary(char** pack, const std::string& name)
{
    List* list = this->get_obj_ptr(name);
    char* c_list = (char*) list->c_str();

    *pack = (char*) P_create();
    *pack = (char*) P_add(*pack, c_list, (int) strlen(c_list) + 1);
    return true;
}

bool KDBLists::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    bool found = false;
    if(texts)
    {
        List* list = this->get_obj_ptr(name);
        found = wrap_grep_gnl(pattern, *list, ecase, all);
    }
    return found;
}

char* KDBLists::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    List* list = this->get_obj_ptr(name);
    char* obj = (char*) list->c_str();
    return obj;
}

bool KDBLists::print_obj_def(const std::string& name)
{
    List* list = this->get_obj_ptr(name);
    char* c_list = (char*) list->c_str();
    print_definition_generic(name, c_list);
    return true;
}

void KDBLists::update_reference_db()
{
    if(global_ref_lst[0]) 
        delete global_ref_lst[0];
    global_ref_lst[0] = new KDBLists(this, "*", false);      
}
