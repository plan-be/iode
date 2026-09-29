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
#include "api/report/engine/engine.h"       // SCR_vtomsq

#define MAX_LENGTH_ARG  128
#define MAX_FILES_OPEN  10

char** A_VAL;
int NB_ARGS; 
int NB_FILES_OPEN;


/**
 * @brief Add an argument in the table A_VAL and increment NB_ARGS.
 * 
 * @param arg 
 */
static void add_to_vector_of_args(char* arg)
{
    SCR_add_ptr((unsigned char***) &A_VAL, &NB_ARGS, (unsigned char*) arg);
    if(arg == NULL) 
        NB_ARGS--;
}

/**
 * @brief Expand an IODE list (declared as $listname)
 * 
 * @param listname 
 * @return char* 
 */
char* expand_list(char* listname)
{
    if(!global_ws_lst->contains(listname)) 
        return NULL;
    
    std::shared_ptr<List> lst = global_ws_lst->get_obj_ptr(listname);
    if(!lst) 
        return NULL;
    
    return (char*) lst->c_str();
}


/**
 * @brief Read next word in file.
 * 
 * @param type 
 * @param buf 
 * @param word 
 * @param max_lg 
 */
static bool read_next_word(int type, char** buf, char* word, int max_lg,
    const std::string& separators)
{
    int lg = 0, c, q = 0;

    while(1) 
    {
        // Gets next char
        if(type == 0) 
        {
            c = **buf;
            if(c == 0) 
                c = EOF;
            else 
                (*buf)++;
        }
        else 
            c = getc((FILE*) buf);

        // Check if 'c' is EOF
        if(c == EOF) 
        {
            word[lg] = 0;
            return (lg == 0) ? false : true;
        }

        // Check if 'c' is a separator character
        if(q == 0 && separators.find(c) != std::string::npos)
        {
            if(lg > 0) 
            {
                word[lg] = 0;
                return true;
            }
        }
        else if(lg < max_lg) 
        {
            if(c == '"') 
            {
                if(lg == 0) 
                    q = 1;
                else if(q == 1) 
                {
                    word[lg] = 0;
                    return true;
                }
            }
            else 
                word[lg++] = c;
        }
	}
}


/**
 * @brief Parse an argument. Then add the argument in the table A_VAL 
 * and increment NB_ARGS.
 * 
 * @filename : replaces @filename by the content of the file "filename"
 * $listname : replaces @listname by the content of the list "listname"
 * 
 * @param arg 
 */
static bool parse_arg(char* arg, const std::string& separators)
{
    if(arg == NULL) 
    {
	    add_to_vector_of_args(arg);
	    return true;
    }

    FILE* fd = NULL;
    char* tmp = NULL; 
    char word[MAX_LENGTH_ARG + 1];
    switch(arg[0]) 
    {
	case '@':
    {
	    if(NB_FILES_OPEN >= MAX_FILES_OPEN) 
        {
		    kwarning("parse_arg: Maximum 10 levels of nesting");
		    return false;
	    }
	    tmp = arg;
	    read_next_word(0, &tmp, word, MAX_LENGTH_ARG, separators);
	    fd = fopen(word + 1, "r");
	    if(fd == NULL) 
        {
            std::string error_msg = "parse_arg: Cannot open file ";
            error_msg += "'" + std::string(word + 1) + "'";
		    kwarning(error_msg.c_str());
		    return false;
	    }
	    NB_FILES_OPEN++;

	    while(read_next_word(1, (char **)fd, word, MAX_LENGTH_ARG, separators))
        {
            if(word[0] != 0) 
                parse_arg(word, separators);
        } 
	    fclose(fd);
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
	    tmp = arg;
	    read_next_word(0, &tmp, word, MAX_LENGTH_ARG, separators);
	    char* exp = expand_list(word + 1);
	    if(exp == NULL) 
        {
		    add_to_vector_of_args(word);
		    return true;
	    }
	    NB_FILES_OPEN++;

	    while(read_next_word(0, &exp, word, MAX_LENGTH_ARG, separators))
		    if(word[0] != 0) parse_arg(word, separators);
	    NB_FILES_OPEN--;
	    break;
    }

	default:
    {
	    tmp = arg;
	    break;
    }

    // end switch
	}

    while(read_next_word(0, &tmp, word, MAX_LENGTH_ARG, separators))
    {
        if(word[0] != 0) 
        {
            if(word[0] == '@' || word[0] == '$') 
                parse_arg(word, separators);
            else
                add_to_vector_of_args(word);
        }
    }

    return true;
}


/**
 * @brief Parses the arguments and performs the expand of $ and @
 * 
 * @param argv 
 */
static bool prepare_vector_of_args(char** argv, const std::string& separators)
{
    NB_ARGS = 0;
    A_VAL = 0;
    NB_FILES_OPEN = 0;

    bool success = true;
    for(int i = 0; argv[i] != 0 ; i++) 
		success &= parse_arg(argv[i], separators);
    success &= parse_arg(0L, separators);
    return success;
}


static char** sub_expand_args(char** argv, const std::string& separators)
{
    bool success = prepare_vector_of_args(argv, separators);
    if(success) 
        return A_VAL;

    for(int i = 0; i < NB_ARGS; i++)
    {
        if(A_VAL[i][0] == '$')
        {
            std::string error_msg = "expand_args: " + std::string(A_VAL[i]) + " cannot be expanded";
            kwarning(error_msg.c_str());
        }
    }

    SCR_free_tbl((unsigned char**) A_VAL);
    return NULL;
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
 *  On error, IodeErrorManager::append_error() is called and the function returns NULL.
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
    std::vector<std::string> args;

    char* argv[12];
    argv[0] = (char*) arg.c_str();
    argv[1] = 0L;
    char** c_args = sub_expand_args(argv, separators);
    if(c_args == NULL)
        return args;

    if(nb > 0 && SCR_tbl_size((unsigned char**) c_args) != nb)
    {
        error_manager.append_error("Illegal argument(s)");
        SCR_free_tbl((unsigned char**) c_args);
        return args;
    }

    int nb_args = SCR_tbl_size((unsigned char**) c_args);
    args.reserve(nb_args);
    for(int i = 0; i < nb_args; i++)
        args.emplace_back(c_args[i]);

    SCR_free_tbl((unsigned char**) c_args);
    return args;
}


/**
 *  Splits a string (generally a function argument) into a table of strings. 
 *  The possible string separators are the chars in B_SEPS (by default " ,\n\t").
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
char **B_vtom_chk(char* arg, int nb)
{
    unsigned char **args;
    char *tmp = (char*) SCR_stracpy((unsigned char*) arg);       // need to create a copy of arg to avoid segmentation fault 
                                        // when called from C++/cython: 

    args = SCR_vtomsq(tmp, B_SEPS, '"');
    if(args == 0) return((char**) args);
    if((nb > 0 && SCR_tbl_size(args) != nb)) {
        error_manager.append_error("Illegal argument(s)");
        SCR_free_tbl(args);
        args = 0;
    }

    return((char**) args);
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
 *  
 *      B_get_arg0(arg0 , " ACAF ACAG XYZ ", sizeof(arg0));  
 *      printf("'%s'", txt); // 'ACAF'
 *  
 *  @param [out] arg0    char*  first arg in the string arg 
 *  @param [in]  arg     char*  any string 
 *  @param [in]  lg      int    max length of arg0
 *  @return              int    length of arg0
 *  
 *  @details 
 */
int B_get_arg0(char* arg0, char*arg, int lg)
{
    int     i;

    SCR_replace((unsigned char*) arg, (unsigned char*) "\t", (unsigned char*) " ");
    U_ljust_text((unsigned char*) arg);
    for(i = 0; i < lg - 1 && arg[i] ; i++) {
        if(U_is_in(arg[i], B_SEPS)) break;
        arg0[i] = arg[i];
    }
    arg0[i] = 0;
    return(i);
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
