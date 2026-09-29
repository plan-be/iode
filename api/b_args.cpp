/**
 * @header4iode
 *
 * Basic functions for managing function and report arguments.
 *
 *  Main functions
 *  --------------
 *      std::vector<std::string> expand_args(const std::string& arg, const int nb, const std::string& separators) : expands an argument by replacing @filename and $listname by their contents
 *      char **B_vtom_chk(char* arg, int nb)                                : splits a string (generally a function argument) into a table of strings. 
 *      int B_loop(char *argv[], int (*fn)(char*, void*), char* client)     : executes the function fn(char*, char*) for each string in the table of strings argv.
 *      int B_ainit_loop(char* arg, int (*fn)(char*, void*), char* client)  : calls expand_args() to expand arg, then calls B_loop() on the resulting table of strings.
 *      int B_get_arg0(char* arg0, char*arg, int lg)                        : computes arg0, the first arg ('word') of max lg bytes, in the string arg. 
 *      int B_argpos(char* str, int ch)                                     : returns the position of a char in a string. 
 *   
 */
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/lists.h"

#define MAX_LENGTH_ARG  128
#define MAX_FILES_OPEN  10

int NB_FILES_OPEN;


/**
 * @brief Expand an IODE list (declared as $listname)
 * 
 * @param  listname     name of the IODE list to expand
 * @return std::string  expanded IODE list content
 */
static std::string expand_list(const std::string& listname)
{
    if(!global_ws_lst->contains(listname))
        return "";
    
    std::shared_ptr<List> lst = global_ws_lst->get_obj_ptr(listname);
    if(!lst)
        return "";
    
    return *lst;
}


/**
 * @brief Read the next word from a stream.
 * 
 * @param input
 * @param word
 * @param max_lg
 * @param separators
 */
static bool read_next_word(std::istream& input, std::string& word, const int max_lg,
    const std::string& separators)
{
    char c;
    bool quoted = false;
    word.clear();

    while(input.get(c))
    {
        // Check if 'c' is a separator character
        if(!quoted && separators.find(c) != std::string::npos)
        {
            if(!word.empty())
                return true;
        }
        else if(word.size() < static_cast<size_t>(max_lg))
        {
            if(c == '"')
            {
                if(word.empty())
                    quoted = true;
                else if(quoted)
                    return true;
            }
            else
                word += c;
        }
    }

    return !word.empty();
}


/**
 * @brief Parse an argument and add each resulting value to v_expanded_args.
 * 
 * @filename : replaces @filename by the content of the file "filename"
 * $listname : replaces @listname by the content of the list "listname"
 * 
 * @param arg
 * @param separators
 */
static bool parse_arg(const std::string& arg, const std::string& separators,
    std::vector<std::string>& v_expanded_args)
{
    if(arg.empty())
        return true;

    std::istringstream arg_stream(arg);
    std::string word;

    switch(arg[0])
    {
    case '@':
    {
        if(NB_FILES_OPEN >= MAX_FILES_OPEN)
        {
            kwarning("parse_arg: Maximum 10 levels of nesting");
            return false;
        }

        read_next_word(arg_stream, word, MAX_LENGTH_ARG, separators);
        std::ifstream file(word.substr(1));
        if(!file.is_open())
        {
            std::string error_msg = "parse_arg: Cannot open file ";
            error_msg += "'" + word.substr(1) + "'";
            kwarning(error_msg.c_str());
            return false;
        }

        NB_FILES_OPEN++;
        while(read_next_word(file, word, MAX_LENGTH_ARG, separators))
        {
            if(!word.empty())
                parse_arg(word, separators, v_expanded_args);
        }
        NB_FILES_OPEN--;
        break;
    }

    case '$':
    {
        if(NB_FILES_OPEN >= MAX_FILES_OPEN)
        {
            kwarning("parse_arg: Maximum 10 levels of nesting");
            return false;
        }

        read_next_word(arg_stream, word, MAX_LENGTH_ARG, separators);
        std::string expanded_list = expand_list(word.substr(1));
        if(expanded_list.empty())
        {
            v_expanded_args.push_back(word);
            return true;
        }

        NB_FILES_OPEN++;
        std::istringstream list_stream(expanded_list);
        while(read_next_word(list_stream, word, MAX_LENGTH_ARG, separators))
        {
            if(!word.empty())
                parse_arg(word, separators, v_expanded_args);
        }
        NB_FILES_OPEN--;
        break;
    }

    default:
        break;
    }

    while(read_next_word(arg_stream, word, MAX_LENGTH_ARG, separators))
    {
        if(!word.empty())
        {
            if(word[0] == '@' || word[0] == '$')
                parse_arg(word, separators, v_expanded_args);
            else
                v_expanded_args.push_back(word);
        }
    }

    return true;
}


/**
 *  Expands an argument by replacing @filename and $listname by their contents : 
 *  
 *      @filename : replaces @filename by the content of the file "filename"
 *      $listname : replaces @listname by the content of the list "listname"
 *  
 *  Expansion can be recursive, i.e. if the file contains $LST1 or/and @otherfile, 
 *  $LST1 and @otherfile will be recursively replaced up to 10 levels of depth.
 *  
 *  The string resulting from the expansion is then split on separators, whose default value is
 *  " ,;\n\t\r".
 *  
 *  If nb is > 0, the function checks that, after expanding arg, the resulting number or arguments 
 *  equals nb (the expected value). 
 *  
 *  On error, IodeErrorManager::append_error() is called and the function returns an empty vector.
 *  
 *  @param [in] arg     const std::string&        arguments to be expanded
 *  @param [in] nb      const int                 0 or expected number of arguments after expansion
 *  @param [in] separators const std::string&      characters used to separate arguments
 *  @return             std::vector<std::string>  empty on error, otherwise the arguments after expansion
 *  
 */
std::vector<std::string> expand_args(const std::string& arg, const int nb,
    const std::string& separators)
{
    NB_FILES_OPEN = 0;

    std::vector<std::string> v_expanded_args;
    bool success = parse_arg(arg, separators, v_expanded_args);
    if(!success)
    {
        for(const std::string& arg : v_expanded_args)
        {
            if(!arg.empty() && arg[0] == '$')
            {
                std::string error_msg = "expand_args: '" + arg + "' cannot be expanded";
                kwarning(error_msg.c_str());
            }
        }

        v_expanded_args.clear();
        return v_expanded_args;
    }

    if(nb > 0 && v_expanded_args.size() != static_cast<size_t>(nb))
    {
        std::string error_msg = "expand_args: Could not parse properly arguments ";
        error_msg += "'" + arg + "'.\nExpected " + std::to_string(nb) + " arguments ";
        error_msg += "but got " + std::to_string(v_expanded_args.size()) + " arguments.";
        kwarning(error_msg.c_str());
        return {};
    }

    return v_expanded_args;
}


/**
 *  Splits a string (generally a function argument) into a table of strings. 
 *  The possible string separators are " ,\n\t".
 *  
 *  If nb is not null, checks that the number of parameters is equal to n. 
 *  
 *  On error, IodeErrorManager::append_error() is called and the function returns NULL.
 *  
 *  @param [in] arg char*   argument
 *  @param [in] nb  int     if not null, expected nb of args after splitting arg
 *  @return         char**  NULL on error or if arg is NULL        
 *                          table of string (split arg)
 *  
 */
char** B_vtom_chk(char* arg, int nb)
{
    std::vector<std::string> v_args = split_multi_quoted(std::string(arg), " ,\n\t", '"');
    if(v_args.empty()) 
        return NULL;

    if((nb > 0 && v_args.size() != nb)) 
    {
        std::string error_msg = "Failed to extract arguments: expected " + std::to_string(nb) + " ";
        error_msg += "arguments but got " + std::to_string(v_args.size()) + " arguments instead.";
        error_manager.append_error(error_msg);
        return NULL;
    }

    char** c_args = vector_to_double_char(v_args);
    return c_args;
}


/**
 *  Executes the function fn(char*, char*) for each string in the table of strings argv.
 *  Stops as soon as fn(arg) returns a non null value.
 *  
 *  Syntax of fn if client is not null:
 *      int fn(char* arg, char* client)
 *  
 *  Syntax of fn if client is null:
 *      int fn(char* arg)
 *   
 *  @param [in] argv    char**                  table of strings
 *  @param [in] fn      int (*fn)(char*, char*) fn pointer
 *  @param [in] client  char*                   2d param of fn() 
 *  @return             int                     result of the last call to fn()
 *  
 */
int B_loop(char *argv[], int (*fn)(char*, void*), char* client)
{
    int     i, rc;

    for(i = 0 ; argv[i] ; i++) 
    {
        if(client == NULL) 
            rc = (*fn)(argv[i], NULL);
        else 
            rc = (*fn)(argv[i], client);
        
        if(rc) 
            return rc;
    }

    return 0;
}


/**
 *  Calls expand_args() to expand arg, then calls B_loop() on the resulting table of strings.
 *  
 *  @see expand_args() and B_loop().
 *  
 *  @param [in] arg     char*                   argument 
 *  @param [in] fn      int (*fn)(char*, char*) fn pointer
 *  @param [in] client  char*                   2d param of fn()
 *  @return             int                     result of the last call to fn()
 *  
 */
int B_ainit_loop(char* arg, int (*fn)(char*, void*), char* client)
{
    std::vector<std::string> args = expand_args(arg, 0);
    if(args.empty()) return -1;

    std::vector<char*> argv;
    argv.reserve(args.size() + 1);
    for(std::string& item : args)
        argv.push_back(item.data());
    argv.push_back(NULL);

    int rc = B_loop(argv.data(), fn, client);
    return rc;
}


/**
 *  Computes arg0, the first arg ('word') of max lg bytes, in the string arg. 
 *  
 *  Example: 
 *      char arg0[21];
 *      B_get_arg0(arg0 , " ACAF ACAG XYZ ", sizeof(arg0));  
 *      printf("'%s'", txt);
 * 
 *      returns 'ACAF'
 *  
 *  @param [out] arg0    char*  first arg in the string arg 
 *  @param [in]  arg     char*  any string 
 *  @param [in]  lg      int    max length of arg0
 *  @return              int    length of arg0
 *  
 *  @details 
 */
int B_get_arg0(char* arg0, char* arg, int lg)
{
    SCR_replace((unsigned char*) arg, (unsigned char*) "\t", (unsigned char*) " ");
    U_ljust_text((unsigned char*) arg);

    int i;
    for(i = 0; i < lg - 1 && arg[i]; i++) 
    {
        if(U_is_in(arg[i], (char*) " ,\n\t")) 
            break;
        arg0[i] = arg[i];
    }
    arg0[i] = 0;
    return i;
}


/**
 *  Returns the position of a char in a string. 
 *  If not found, returns 0, which is considered as the default position.
 *  
 *  ch is translated in uppercase before searching. str must therefore be in uppercase.
 *  
 *  @param [in] str     char*   list of uppercase characters
 *  @param [in] ch      int     character to search in str
 *  @return             int     position of upper(ch) in str or 0 if not found
 *  
 */
int B_argpos(char* str, int ch)
{
    int     pos;

    ch = SCR_upper_char(ch);
    pos = get_pos_in_char_array(str, ch);
    pos = std::max(0, pos);
    return(pos);
}
