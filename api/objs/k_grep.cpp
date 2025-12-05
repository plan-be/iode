/**
 *  @header4iode
 *
 *  Functions to search strings in KDB objects.
 *  
 *  List of functions 
 *  -----------------
 *      char *K_expand(int type, char* file, char* pattern, int all)                                    Retrieves all object names matching one or more patterns in a workspace or an object file.                                Retrieves all object names matching one or more patterns in a KDB.
 *      int K_aggr(char* pattern, char* ename, char* nname) *                                           Transforms a variable name based on an "aggregation" pattern.
 *  
 */
#include "scr4/s_args.h"
#include "scr4/s_prost.h"

#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/utils/utils.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/tables.h"
#include "api/objs/grep.h"
#include "api/report/commands/commands.h"       // K_AggrChar


extern "C" int SCR_ADD_PTR_CHUNCK;


static bool wrap_grep_gnl(const std::string& pattern, const std::string& text, 
    const bool ecase, const char all)
{
    return SCR_grep_gnl((char*) pattern.c_str(), (char*) text.c_str(), 
                        (int) ecase, (int) all) == 0;
}

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
 *  @param [in] ecase    bool      true if case sensitive
 *  @param [in] names    bool      true to search also in object names
 *  @param [in] forms    bool      true to search also in LEC expressions (for EQ, IDT, Table LEC cells)
 *  @param [in] texts    bool      true to search also in texts (for CMT, LST, EQS comments, Table text cells)
 *  @param [in] all      char      character indicating "any sequence" (normally '*')
 *                       
 *  @return              vector<string> list of object names where the string has been found 
 *                                      empty if none were found 
 */
std::vector<std::string> KDB::grep(const std::string& pattern, const bool ecase, const bool names, 
    const bool forms, const bool texts, const char all)
{
    bool pattern_is_all = pattern.size() == 1 && pattern[0] == all;
    if(names && !texts && !forms && pattern_is_all) 
        return this->get_names();
    
    bool found;
    std::vector<std::string> lst;
    for(const auto& [name, handle] : this->k_objs)
    {
        found = false;
        if(names) 
            found = wrap_grep_gnl(pattern, name, ecase, all);

        if(!found) 
        {
            switch(this->k_type) 
            {
                case COMMENTS :
                    if(texts) 
                        found = wrap_grep_gnl(pattern, KCVAL(this, handle), ecase, all);
                    break;
                case LISTS :
                    if(texts) 
                        found = wrap_grep_gnl(pattern, KLVAL(this, handle), ecase, all);
                    break;
                case IDENTITIES :
                    if(forms) 
                        found = wrap_grep_gnl(pattern, KILEC(this, handle), ecase, all);
                    break;
                case EQUATIONS :
                {
                    std::string lec = KELEC(this, name);
                    std::string cmt = KECMT(this, name);
                    if(forms) 
                        found = wrap_grep_gnl(pattern, lec, ecase, all);
                    if(!found && texts)
                        found = wrap_grep_gnl(pattern, cmt, ecase, all);
                    break;
                }
                case TABLES:
                {
                    Table* tbl = KTVAL(this, name);

                    found = false;
                    std::string text;
                    for(const TableLine& tline : tbl->lines) 
                    {
                        if(found) 
                            break;
                        
                        switch(tline.get_type()) 
                        {
                            case TABLE_LINE_SEP   :
                            case TABLE_LINE_MODE  :
                            case TABLE_LINE_DATE  :
                            case TABLE_LINE_FILES :
                                break;
                            case TABLE_LINE_TITLE :
                            {
                                TableCell cell = tline.cells[0];
                                if(texts)
                                {
                                    text = cell.get_content(true);
                                    found = wrap_grep_gnl(pattern, text, ecase, all);
                                } 
                                break;
                            }
                            case TABLE_LINE_CELL :
                            {
                                found = false;
                                for(const TableCell& cell : tline.cells)
                                {
                                    if(found) 
                                        break;
                                    
                                    if((texts && cell.get_type() == TABLE_CELL_STRING) || 
                                       (forms && cell.get_type() == TABLE_CELL_LEC))
                                       {
                                            text = cell.get_content(true);
                                            found = wrap_grep_gnl(pattern, text, ecase, all);
                                       }
                                }
                                break;
                            }
                        }
                    }

                    delete tbl;
                    break;
                }
            }
        }

        if(found) 
            lst.push_back(name);
    }

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
 */
char *K_expand(int type, char* file, char* c_pattern, int all)
{
    KDB* kdb;
    if(file == NULL) 
        kdb = get_global_db(type);
    else 
    {
        kdb = new KDB((IodeType) type, DB_STANDALONE);
        bool success = kdb->load(std::string(file));
        if(!success) 
            return NULL;
    }

    std::string lst = kdb->expand(std::string(c_pattern), (char) all);

    if(file != NULL)
    {
        delete kdb;
        kdb = nullptr;
    }

    char* c_lst = new char[lst.size() + 1];
    strcpy(c_lst, lst.c_str());
    return c_lst;
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
 */
std::string KDB::expand(const std::string& pattern, const char all)
{
    if(pattern.empty()) 
        return "";

    std::vector<std::string> patterns = split_multi(pattern, std::string(A_SEPS));
    
    std::string sub_pattern;
    std::vector<std::string> v_names;
    for(const std::string& _pattern_ : patterns) 
    {
        sub_pattern = trim(_pattern_);
        if(sub_pattern.empty()) 
            continue;
        
        if(sub_pattern[0] != '"' && (string_contains(sub_pattern, all) || string_contains(sub_pattern, '?'))) 
        {
            std::vector<std::string> sub_v_names = this->grep(sub_pattern, false, true, false, false, all);
            for(const std::string& name : sub_v_names) 
                v_names.push_back(name);
        }
        else
            v_names.push_back(sub_pattern);
    }
    
    std::string lst = join(v_names, ";");
    return lst;
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
