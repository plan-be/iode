/**
 *  @header4iode
 * 
 *  IODE report $foreach and $next implementation
 *  ---------------------------------------------
 *  
 *  $FOREACH and $NEXT mechanism
 *  -----------------------------
 *   
 *  $foreach index_name value1 value2...
 *     
 *  The function RP_foreach():
 *      1. Pushes the defines "index_name" and "FOREACH__index_name"
 *         (see b_rep_define.c for the meaning of "push" and "pop") 
 *      2. Defines "FOREACH__index_name" as "value1 value2..."
 *      3. Calls RP_foreach_next_index()
 *         The function RP_foreach_next_index():
 *              1. if FOREACH__index_name is  empty:
 *                  - undefines FOREACH__index_name and index_name
 *                  - pops (i.e. restores) the previous values of FOREACH__index_name and index_name
 *                  - returns -1 indicating the end of the loop to RP_foreach()
 *              2. if not:
 *                  - defines index_name as the first remaining element of FOREACH__index_name
 *                  - drops the first element of FOREACH__index_name
 *                  - returns the number of remaining elements in FOREACH__index_name
 *      4. if RP_foreach_next_index() returns a negative value (ie end of loop), 
 *          goto the line "$next index_name" 
 *                  
 *  $next index_name 
 *  
 *  The function RP_foreach_next() is called. 
 *  RP_foreach_next():
 *      1. checks that the syntax is valid ($next index_name). If not, returns -3 (error)
 *      2. calls RP_foreach_goto_next() that:
 *          1. reads the report backwards line/line (with interpretation) until 
 *              $foreach index_name is reached
 *          2. if no $foreach index_name line can be found, returns -3 (error)
 *          3. if found, calls RP_foreach_next_index() 
 *             if the end of the loop is reached, return -1, 0 else
 *      3. if rc == 0: jumps to the line after $foreach 
 *         else (end of the loop) jumps to the line after $next
 *  
 *  Example
 *  -------
 *      # printtbl: prints a list of variables on a specific period
 *      $procdef printtbl year vars
 *          $foreach var %vars%
 *              %var%[%year%] = {%var%[%year%]}
 *          $next var
 *      $procend
 *  
 *  List of functions
 *  -----------------
 *      static int RP_foreach_next_index(char *name)    Goes to the next element and decreases the number of elements in a foreach loop.
 *      int RP_foreach(char* arg, int unused)                       Implemention of $foreach <index_name> <list_of_values>
 *      int RP_foreach_break(char *name)                Exits the foreach block. Not implemented
 *      int RP_foreach_next(char* arg, int unused)                  Implementation of $next <index_name>
 *      static int RP_foreach_goto_next(char* label)    Reads a report backwards until the previous $foreach line then goes to the next index in the loop.
 *  
 */

#include "api/report/engine/engine.h"


/**
 *  Goes to the next element and decreases the number of elements in a foreach loop.
 *  See above for details.
 *  
 *  @param [in] char*   name    index_name
 *  @return     int             nb of remaining elements in the loop    
 *  
 *  TODO: replace B_ReportLine() by calls to RP_define_1()
 */
static int RP_foreach_next_index(char *name)
{
    unsigned char   buf[512];
    std::string     line;
    int             n = 0;

    // compte le nb d'éléments restant dans la liste FOREACH__name
    sprintf((char*) buf, "@vcount(%%FOREACH__%s%%)", name);
    RP_expand(line, (char*) buf);
    n = atol(line.c_str());
    if(n == 0) { // On a atteint la fin de la liste
        sprintf((char*) buf, "FOREACH__%s", name);
        RP_undef_1(name); // Needed ?
        RP_undef_1((char*) buf);  // Needed ?
        RP_define_restore(name);
        RP_define_restore((char*) buf);
        return -1;
    }
    // Place le premier élément dans l'index de la boucle
    sprintf((char*) buf, "$define %s @vtake(1,%%FOREACH__%s%%)", name, name);
    B_ReportLine((char*) buf, 1);

    // Drop le premier indice
    sprintf((char*) buf, "$define FOREACH__%s @vdrop(1,%%FOREACH__%s%%)", name, name);
    B_ReportLine((char*) buf, 1);

    return(n - 1);
}


/**
 *  $foreach <index_name> <list_of_values> 
 *  
 *  See above for details.
 *  
 */
int RP_foreach(char* arg, int unused)
{
    unsigned char   buf[512];
    std::string     name;
    std::string     list;
    int             rc = 0;

    if(arg == NULL) return -1;

    // Crée l'index (name) et la liste de valeurs à traiter (list)
    if(RP_splitline(arg, name, list, 30) < 0) return -1;

    // Sauve la valeur avant la boucle de l'index de la boucle (name)
    RP_define_save(name.data());

    // Sauve de la même façon l'éventuel define FOREACH__<name>
    sprintf((char*) buf, "FOREACH__%s", name.c_str());
    RP_define_save((char*) buf);

    // Crée le define name avec la liste des indices de la boucle
    RP_define_1(name.data(), list.data());

    // Crée un define identique avec comme nom FOREACH__<nom>
    sprintf((char*) buf, "$define FOREACH__%s %%%s%%", name.c_str(), name.c_str());
    B_ReportLine((char*) buf, 1);

    // Place la première valeur dans name et supprime cette valeur de la liste
    // Si la liste est vide, saute à la fin ($next name)
    if(RP_foreach_next_index(name.data()) < 0) {
        //RP_define_restore(name);        // Restores the $define <name>
        //sprintf(buf, "FOREACH__%s", name);
        //RP_define_restore(buf);         // Restores the $define FOREACH__<name>
        RP_goto_label("next", name.data());    // move to the line "$next <name>"
    }    

    return rc;
}

/**
 *  $break index_name 
 *  
 *  Exits the foreach block. Not implemented
 *  
 *  TODO: implement this + $continue index_name
 */
int RP_foreach_break(char *name)
{
    return 0;
}


/**
 *  Reads a report backwards until reaching the previous $foreach index_name line.
 *  Goes to the next index in the loop.
 *  See above for details.
 *     
 *  @param [in] char*   name    index_name
 *  @return     int             < 0 on error (not found)
 *                              0 if the line is found
 *  
 *  TODO: Create a sub-fn to ensure that a identical treatment is applied here and in 
 *  REPFILE::execute() to analyse line[0], line[1]... 
 */
static int RP_foreach_goto_next(char* label)
{
    int     rc, beg = 1;
    std::string name;
    std::string line;
    std::string arg;

    SCR_sqz((unsigned char*) label);

    while(1) {
        // Stoppe si début du fichier atteint
        if(CUR_REPFILE->current_line <= 1) return(-3);
        CUR_REPFILE->current_line -= 2;
        line.clear();
        rc = CUR_REPFILE->read_line(line, 0);

        if((line[0] == '#' || line[0] == '$') && line[1] != line[0]) {
            if(line[1] == 0 || U_is_in(line[1], " \n\r\t")) continue;
            if(RP_splitline(line.substr(1), name, arg, 30) < 0) {
                rc = -1;
                goto done;
            }

            SCR_lower((unsigned char*) name.data());
            if(name == "foreach") {
                std::vector<std::string> parms = split_skip_empty(arg, " ");
                if(!parms.empty() && parms[0] == label) {
                    // Place la première valeur dans name et supprime cette valeur de la liste
                    rc = RP_foreach_next_index(label);
                    if(rc > 0) rc = 0;
                    goto done;
                }
            }
        }
    }

done:
    return rc;
}



/**
 *  $next <index_name>
 *  
 *  See above for details. 
 *  
 */
int RP_foreach_next(char* arg, int unused)
{
    int curline = CUR_REPFILE->current_line;

    int rc = 0;
    std::vector<std::string> v_args = split_multi_quoted(arg, " ,\n\t", '"');
    int nb = (int) v_args.size();
    switch(nb) 
    {
        case 1 :
            rc = RP_foreach_goto_next((char*) v_args[0].c_str());
            break;

        // $next must have exactly 1 arg
        default:
            rc = -3;
            break;
    }

    if(rc != 0) 
    {
        CUR_REPFILE->current_line = curline;
        rc = 0;
    }

    return rc;
}
