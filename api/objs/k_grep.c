/**
 *  @header4iode
 *
 *  Functions to search strings in KDB objects.
 *  
 *  List of functions 
 *  -----------------
 *      char **K_grep(KDB* kdb, char* pattern, int ecase, int names, int forms, int texts, int all)     Creates a list of all objects in a KDB having a specific pattern in their names or LEC expression, comment...
 *      char *K_expand(int type, char* file, char* pattern, int all)                                    Retrieves all object names matching one or more patterns in a workspace or an object file.
 *      int K_aggr(char* pattern, char* ename, char* nname) *                                           Transforms a variable name based on an "aggregation" pattern.
 *  
 */
 
#include "iode.h"

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
 *  @param [in] forms    int       1 to search also in LEC expressions (for EQ, IDT, TBL LEC cells)
 *  @param [in] texts    int       1 to search also in texts (for CMT, LST, EQS comments, TBL text cells)
 *  @param [in] all      int       character indicating "any sequence" (normally '*')
 *                       
 *  @return              char**    NULL terminated list of object names where the string has been found 
 *                                 NULL if non were found
 *  
 */
char **K_grep(KDB* kdb, char* pattern, int ecase, int names, int forms, int texts, int all)
{
    int     i, j, k, n = 0, found;
    char    **lst = NULL;
    TBL     *tbl;
    TLINE   *tline;
    TCELL   *tcell;
    int     old_SCR_ADD_PTR_CHUNCK = SCR_ADD_PTR_CHUNCK;
    extern int SCR_ADD_PTR_CHUNCK;
    
    if(names && !texts && !forms && pattern && pattern[0] == all && pattern[1] == 0) {     // JMP 19/01/2023 => speed
        n = KNB(kdb);
        lst = (char**) SCR_malloc((n + 1) * sizeof(char*));
        for(i = 0; i < n ; i++) 
            lst[i] = SCR_stracpy(KONAME(kdb, i));
        return(lst);
    }


    SCR_ADD_PTR_CHUNCK = 1000;
    for(i = 0; i < KNB(kdb); i++) {
        found = 0;
        if(names) found = !SCR_grep_gnl(pattern, KONAME(kdb, i), ecase, all);

        if(!found) {
            switch(KTYPE(kdb)) {
                case K_CMT :
                    if(texts) found = !SCR_grep_gnl(pattern, KCVAL(kdb, i), ecase, all);
                    break;
                case K_LST :
                    if(texts) found = !SCR_grep_gnl(pattern, KLVAL(kdb, i), ecase, all);
                    break;
                case K_IDT :
                    if(forms) found = !SCR_grep_gnl(pattern, KILEC(kdb, i), ecase, all);
                    break;
                case K_EQS :
                    if(forms) found = !SCR_grep_gnl(pattern, KELEC(kdb, i), ecase, all);
                    if(!found && texts)
                        found = !SCR_grep_gnl(pattern, KECMT(kdb, i), ecase, all);
                    break;

                case K_TBL:
                    tbl = KTVAL(kdb, i);
                    for(k = 0; k < T_NL(tbl) && !found; k++) {
                        tline = tbl->t_line + k;
                        tcell = (TCELL *) tline->tl_val;
                        switch(tline->tl_type) {
                            case KT_LINE  :
                            case KT_MODE  :
                            case KT_DATE  :
                            case KT_FILES :
                                break;
                            case KT_TITLE :
                                if(texts) found = !SCR_grep_gnl(pattern, T_cell_cont(tcell, 1), ecase, all);
                                break;
                            case KT_CELL  :
                                for(j = 0; j < T_NC(tbl) && !found; j++)
                                    if((texts && tcell[j].tc_type == KT_STRING) ||
                                            (forms && tcell[j].tc_type == KT_LEC))
                                        found = !SCR_grep_gnl(pattern, T_cell_cont(tcell + j, 1), ecase, all);
                                break;
                        }
                    }
                    T_free(tbl);
                    break;
            }
        }

        if(found) SCR_add_ptr(&lst, &n, KONAME(kdb, i));
    }

    if(lst != NULL) SCR_add_ptr(&lst, &n, NULL);
    
    SCR_ADD_PTR_CHUNCK = old_SCR_ADD_PTR_CHUNCK;    // JMP 19/01/2023
    return(lst);
}

/**
 *  Retrieves all object names matching one or more patterns in a workspace or an object file.
 *  
 *  @param [in] type    int     Object type (K_CMT -> K_VAR)
 *  @param [in] file    char*   filename to search into or null to look in the workpace 
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

    if(pattern == 0 || pattern[0] == 0 ||
            type < K_CMT || type > K_VAR) return(NULL);

    if(file == NULL) kdb = K_WS[type];
    else {
        kdb = K_interpret(type, file);
        if(kdb == NULL) return(lst);  /* JMP 05-01-99 */
    }

    ptbl = SCR_vtoms(pattern, A_SEPS); /* JMP 14-08-98 */
    np = SCR_tbl_size(ptbl);
    for(i = 0; i < np; i++) {
        if(ptbl[i][0] != '"' &&
                (U_is_in(all, ptbl[i]) || U_is_in('?', ptbl[i]))) {
            tbl = K_grep(kdb, ptbl[i], 0, 1, 0, 0, all);
            SCR_free(ptbl[i]);
            if(tbl != NULL) {
                ptbl[i] = SCR_mtov(tbl, ';');
                SCR_free_tbl(tbl);
            }
            else ptbl[i] = SCR_stracpy("");
        }
    }

    lst = SCR_mtov(ptbl, ';');
    SCR_free_tbl(ptbl);

    if(file != NULL) K_free(kdb);
    return(lst);
}


char    K_AggrChar;     // Replacement character in the aggregated names

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
    SCR_sqz(nname);
    rc = 0;
done:
    return(rc);
}




