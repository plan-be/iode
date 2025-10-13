/**
 *  @header4iode
 *
 *  Functions to search strings in KDB objects.
 *  
 *  List of functions 
 *  -----------------
 *      char **K_grep(KDB* kdb, char* pattern, int ecase, int names, int forms, int texts, int all)     Creates a list of all objects in a KDB having a specific pattern in their names or LEC expression, comment...
 *      char *K_expand(int type, char* file, char* pattern, int all)                                    Retrieves all object names matching one or more patterns in a workspace or an object file.
 *      char *K_expand_kdb(KDB* kdb, int type, char* pattern, int all)                                  Retrieves all object names matching one or more patterns in a KDB.
 *      int K_aggr(char* pattern, char* ename, char* nname) *                                           Transforms a variable name based on an "aggregation" pattern.
 *  
 */
#include "scr4/s_args.h"
#include "scr4/s_prost.h"

#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/tables.h"
#include "api/objs/grep.h"
#include "api/report/commands/commands.h"       // K_AggrChar


extern "C" int SCR_ADD_PTR_CHUNCK;

/**
 *  Creates a list of all objects in a KDB having a specific pattern in their names or LEC expression, comment...
 *    
 *  The following characters in pattern have a special meaning:
 *  
 *      '*' : any character sequence, even empty
 *      '?' : any character (one and only one)
 *      '@' : any alphanumerical char
 *      '&' : any non alphanumerical char
 *      '|' : any alphanumeric character or none at the beginning and end of a string 
 *      '!' : any non-alphanumeric character or none at the beginning and end of a string 
 *      '\' : escape the next character
 *  
 *  @param [in] kdb      KDB*      KDB of any type 
 *  @param [in] pattern  char*     string to search
 *  @param [in] ecase    int       1 if case sensitive, 0 otherwise
 *  @param [in] names    int       1 to search also in object names
 *  @param [in] forms    int       1 to search also in LEC expressions (for EQ, IDT, Table LEC cells)
 *  @param [in] texts    int       1 to search also in texts (for CMT, LST, EQS comments, Table text cells)
 *  @param [in] all      int       character indicating "any sequence" (normally '*')
 *                       
 *  @return              char**    NULL terminated list of object names where the string has been found 
 *                                 NULL if non were found
 *  
 */
char **K_grep(KDB* kdb, char* pattern, int ecase, int names, int forms, int texts, int all)
{
    int         i, j, k, n = 0, found;
    char        **lst = NULL;
    Table       *tbl;
    TableLine   *tline;
    int         old_SCR_ADD_PTR_CHUNCK = SCR_ADD_PTR_CHUNCK;
    std::string lec;
    std::string cmt;
    std::string text;
    
    if(names && !texts && !forms && pattern && pattern[0] == all && pattern[1] == 0) 
    {
        n = kdb->size();
        lst = (char**) SCR_malloc((n + 1) * sizeof(char*));
        i = 0;
        for(const auto& [name, _] : kdb->k_objs)
            lst[i++] = (char*) SCR_stracpy((unsigned char*) name.c_str());
        return lst;
    }

    TableCell* cell;
    SCR_ADD_PTR_CHUNCK = 1000;
    i = 0;
    for(const auto& [name, _] : kdb->k_objs)
    {
        found = 0;
        if(names) 
            found = !SCR_grep_gnl(pattern, (char*) name.c_str(), ecase, all);

        if(!found) 
        {
            switch(kdb->k_type) 
            {
                case COMMENTS :
                    if(texts) 
                        found = !SCR_grep_gnl(pattern, KCVAL(kdb, i), ecase, all);
                    break;
                case LISTS :
                    if(texts) 
                        found = !SCR_grep_gnl(pattern, KLVAL(kdb, i), ecase, all);
                    break;
                case IDENTITIES :
                    if(forms) 
                        found = !SCR_grep_gnl(pattern, KILEC(kdb, i), ecase, all);
                    break;
                case EQUATIONS :
                    lec = KELEC(kdb, i);
                    cmt = KECMT(kdb, i);
                    if(forms) 
                        found = !SCR_grep_gnl(pattern, (char*) lec.c_str(), ecase, all);
                    if(!found && texts)
                        found = !SCR_grep_gnl(pattern, (char*) cmt.c_str(), ecase, all);
                    break;
                case TABLES:
                    tbl = KTVAL(kdb, i);
                    for(k = 0; k < T_NL(tbl) && !found; k++) 
                    {
                        tline = &tbl->lines[k];
                        switch(tline->get_type()) 
                        {
                            case TABLE_LINE_SEP   :
                            case TABLE_LINE_MODE  :
                            case TABLE_LINE_DATE  :
                            case TABLE_LINE_FILES :
                                break;
                            case TABLE_LINE_TITLE :
                                cell = &(tline->cells[0]);
                                if(texts)
                                {
                                    text = cell->get_content(true);
                                    found = !SCR_grep_gnl(pattern, (char*) text.c_str(), ecase, all);
                                } 
                                break;
                            case TABLE_LINE_CELL  :
                                for(j = 0; j < T_NC(tbl) && !found; j++)
                                {
                                    cell = &(tline->cells[j]);
                                    if((texts && cell->get_type() == TABLE_CELL_STRING) || 
                                       (forms && cell->get_type() == TABLE_CELL_LEC))
                                       {
                                            text = cell->get_content(true);
                                            found = !SCR_grep_gnl(pattern, (char*) text.c_str(), ecase, all);
                                       }
                                }
                                break;
                        }
                    }
                    delete tbl;
                    tbl = nullptr;
                    break;
            }
        }

        if(found) 
            SCR_add_ptr((unsigned char***) &lst, &n, (unsigned char*) name.c_str());
        i++;
    }

    if(lst != NULL) 
        SCR_add_ptr((unsigned char***) &lst, &n, NULL);
    
    SCR_ADD_PTR_CHUNCK = old_SCR_ADD_PTR_CHUNCK;
    return lst;
}

/**
 *  Retrieves all object names matching one or more patterns in a workspace or an object file.
 *  
 *  @param [in] type    int     Object type (COMMENTS -> VARIABLES)
 *  @param [in] file    char*   filename to search into or null to look in the workspace 
 *  @param [in] pattern char*   list of patterns separated by one of A_SEPS chars
 *  @param [in] all     int     character meaning "any char sequence" (normally '*')
 *  
 *  @return             char*   allocated semi-colon separated string with all matching names
 *                              if no name found return allocated string of length 0 ("").
 *  
 */
char *K_expand(int type, char* file, char* pattern, int all)
{

    int     i, np;
    char    **ptbl, **tbl, *lst = NULL;
    KDB     *kdb;

    if(pattern == 0 || pattern[0] == 0 || type < COMMENTS || type > VARIABLES) 
        return(NULL);

    if(file == NULL) 
        kdb = K_WS[type];
    else 
    {
        kdb = K_interpret(type, file, 0);
        if(!kdb) 
            return(lst);
    }

    ptbl = (char**) SCR_vtoms((unsigned char*) pattern, (unsigned char*) A_SEPS); /* JMP 14-08-98 */
    np = SCR_tbl_size((unsigned char**) ptbl);
    for(i = 0; i < np; i++) 
    {
        if(ptbl[i][0] != '"' && (U_is_in(all, ptbl[i]) || U_is_in('?', ptbl[i]))) 
        {
            tbl = K_grep(kdb, ptbl[i], 0, 1, 0, 0, all);
            SCR_free(ptbl[i]);
            if(tbl != NULL) 
            {
                ptbl[i] = (char*) SCR_mtov((unsigned char**) tbl, ';');
                SCR_free_tbl((unsigned char**) tbl);
            }
            else 
                ptbl[i] = (char*) SCR_stracpy((unsigned char*) "");
        }
    }

    lst = (char*) SCR_mtov((unsigned char**) ptbl, ';');
    SCR_free_tbl((unsigned char**) ptbl);

    if(file != NULL)
    {
        delete kdb;
        kdb = nullptr;
    }
    
    return lst;
}

/**
 *  Retrieves all object names matching one or more patterns in a KDB.
 *  
 *  @param [in] kdb     KDB*    KDB to search into or null to look in the workpace 
 *  @param [in] type    int     Object type (COMMENTS -> VARIABLES)
 *  @param [in] pattern char*   list of patterns separated by one of A_SEPS chars
 *  @param [in] all     int     character meaning "any char sequence" (normally '*')
 *  
 *  @return             char*   allocated semi-colon separated string with all matching names
 *                              if no name found return allocated string of length 0 ("").
 *  
 */
char *K_expand_kdb(KDB* kdb, int type, char* pattern, int all)
{
    int     i, np;
    char    **ptbl, **tbl, *lst = NULL;

    if(pattern == 0 || pattern[0] == 0 || type < COMMENTS || type > VARIABLES) 
        return(NULL);

    if(kdb == NULL) 
        kdb = K_WS[type];

    ptbl = (char**) SCR_vtoms((unsigned char*) pattern, (unsigned char*) A_SEPS);
    np = SCR_tbl_size((unsigned char**) ptbl);
    for(i = 0; i < np; i++) 
    {
        if(ptbl[i][0] != '"' && (U_is_in(all, ptbl[i]) || U_is_in('?', ptbl[i]))) 
        {
            tbl = K_grep(kdb, ptbl[i], 0, 1, 0, 0, all);
            SCR_free(ptbl[i]);
            if(tbl != NULL) 
            {
                ptbl[i] = (char*) SCR_mtov((unsigned char**) tbl, ';');
                SCR_free_tbl((unsigned char**) tbl);
            }
            else 
                ptbl[i] = (char*) SCR_stracpy((unsigned char*) "");
        }
    }

    lst = (char*) SCR_mtov((unsigned char**) ptbl, ';');
    SCR_free_tbl((unsigned char**) ptbl);

    return(lst);
}

/**
 *  Transforms a variable name based on an "aggregation" pattern. 
 *  The characters between () are kept as is whereas the chars between [] are
 *  replaced by K_AggrChar.
 *  
 *  @param [in]         pattern char*   pattern defining the aggregation
 *  @param [in]         ename   char*   initial variable name
 *  @param [in, out]    nname   char*   resulting name 
 *  @return             int
 *  
 *  @see KV_aggregate() or https://iode.plan.be/doku.php?id=wsaggrsum
 */
int K_aggr(char* pattern, char* ename, char* nname)
{
    int     rc = -1, e, n, p, lg, inp = 0, inb = 0;

    lg = (int)strlen(pattern);
    for(e = 0, n = 0, p = 0; p < lg; p++) {
        if(ename[e] == 0 && !U_is_in(pattern[p], "])"))
            goto done; /* premature end of name */

        switch(pattern[p]) {
            case '(' :
                inp = 1;
                break;
            case ')' :
                inp = 0;
                break;
            case '[' :
                inb = 1;
                break;
            case ']' :
                inb = 0;
                break;

            case '?' :
                if(inb) nname[n] = K_AggrChar;
                else nname[n] = ename[e];
                if(inb && ename[e] == K_AggrChar) goto done;
                n++;
                e++;
                break;
            default :
                if(pattern[p] == ename[e]) nname[n] = ename[e];
                else goto done;
                n++;
                e++;
        }

    }
    if(ename[e]) goto done; /* premature end of pattern */

    nname[n] = 0;
    SCR_sqz((unsigned char*) nname);
    rc = 0;
done:
    return(rc);
}
