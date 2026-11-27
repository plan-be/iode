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
 *  Executes the function fn(const std::string&) for each string in the vector argv.
 *  Stops as soon as fn(arg) returns a non null value.
 *
 *  fn can be any callable: a function pointer or a lambda capturing the extra parameters
 *  it needs. Example:
 *
 *      B_loop(names, [type](const std::string& name) { return B_DataCreate_1(name, type); });
 *
 *  @param [in] argv    const std::vector<std::string>&                     vector of strings
 *  @param [in] fn      const std::function<int(const std::string&)>&       function to call
 *  @return             int                                                 result of the last call to fn()
 *
 */
int B_loop(const std::vector<std::string>& argv, const std::function<int(const std::string&)>& fn)
{
    for(const std::string& arg : argv)
    {
        int rc = fn(arg);
        if(rc) 
            return rc;
    }

    return 0;
}


/**
 *  Calls expand_args() to expand arg, then calls B_loop() on the resulting vector of strings.
 *  
 *  @see expand_args() and B_loop().
 *  
 *  @param [in] arg     const std::string&                                  argument
 *  @param [in] fn      const std::function<int(const std::string&)>&       function to call (see B_loop())
 *  @return             int                                                 result of the last call to fn(),
 *                                                                          -1 if arg cannot be expanded
 *
 */
int B_ainit_loop(const std::string& arg, const std::function<int(const std::string&)>& fn)
{
    std::vector<std::string> args = expand_args(arg, 0);
    if(args.empty())
        return -1;

    return B_loop(args, fn);
}


/**
 *  Extracts (consumes) the first argument (= word) of max 'lg - 1' chars from the string 'args'.
 *
 *  On return, args contains the rest of the arguments: the first argument and the separator
 *  that follows it (' ', ',' or '\n') are removed from args.
 *
 *  Before extracting the argument, the tabs are replaced by spaces and the leading white
 *  spaces are moved to the end of args (as U_ljust_text() did): the rest of args may
 *  therefore end with spaces.
 *
 *  If the argument is longer than lg - 1 chars, it is truncated and the remaining chars stay
 *  at the beginning of args.
 *
 *  Example:
 *      std::string args = " ACAF ACAG XYZ";
 *      std::string arg0 = get_next_arg(args, 21);
 *      // => arg0 = "ACAF", args = "ACAG XYZ "
 *      arg0 = get_next_arg(args, 21);
 *      // => arg0 = "ACAG", args = "XYZ "
 *
 *  @param [in, out] args    std::string&   any string. On return: the rest of the arguments
 *  @param [in]      lg      int            arg0 contains at most lg - 1 chars
 *  @return                  std::string    first argument of args ("" if args is empty,
 *                                          contains only white spaces or starts with ',')
 */
std::string get_next_arg(std::string& args, int lg)
{
    // tabs -> spaces
    std::replace(args.begin(), args.end(), '\t', ' ');

    // leading white spaces moved to the end (same as U_ljust_text(): nothing changes if
    // args contains only white spaces)
    size_t n = 0;
    while(n < args.size() && SCR_is_space(args[n]))
        n++;
    if(n > 0 && n < args.size())
        args = args.substr(n) + std::string(n, ' ');

    // first word: ends at the first separator, at the end of args (or at a '\0') or
    // after lg - 1 chars
    const std::string separators = " ,\n\t";
    size_t i = 0;
    while((int) i < lg - 1 && i < args.size() && args[i] && separators.find(args[i]) == std::string::npos)
        i++;

    std::string arg0 = args.substr(0, i);

    // removes the first word and the separator that follows it
    if(i < args.size() && separators.find(args[i]) != std::string::npos)
        i++;
    args.erase(0, i);

    return arg0;
}


/**
 *  Returns the position of a char in a string. 
 *  If not found, returns 0, which is considered as the default position.
 *  
 *  ch is translated in uppercase before searching. str must therefore be in uppercase.
 *  
 *  @param [in] str     const std::string&  list of uppercase characters
 *  @param [in] ch      int                 character to search in str
 *  @return             int                 position of upper(ch) in str or 0 if not found
 *
 */
int B_argpos(const std::string& str, int ch)
{
    ch = SCR_upper_char(ch);
    if(ch == 0)
        return 0;
    size_t pos = str.find((char) ch);
    return (pos == std::string::npos) ? 0 : (int) pos;
}
