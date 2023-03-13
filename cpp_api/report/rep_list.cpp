#include "rep_list.h"


/**
 * @brief Set the up $/#-functions list object
 * 
 * @note see b_rep_syntax.c for the list.
 * 
 */
std::vector<std::string> build_command_functions_list(const int group, const bool gui)
{
    std::vector<std::string> func_list;

    std::vector<std::string> v_binary_ext_upper;
    for(std::string type_short: v_binary_ext)
    {
        type_short[0] = toupper(type_short[0]);
        v_binary_ext_upper.push_back(type_short);
    }

    std::vector<std::string> v_ext_upper;
    for(std::string ext: v_ext)
    {
        ext[0] = toupper(ext[0]);
        v_ext_upper.push_back(ext);
    }

    std::string prefix = gui ? "#" : "$";
    
    std::string func_name;
    int func_group;
    bool test_func;
    bool test_group;
    int i = 0;
    while(B_fns[i].keyw != 0)
    {
        func_name = prefix + std::string(B_fns[i].key_camel);
        func_group = B_fns[i].group;

        test_func = gui ? B_fns[i].sfn != NULL : B_fns[i].fn != NULL;
        test_group = (group < 0) ? true : func_group == group;
        if(test_func && test_group)
        {
            // see documentation of BFNS structure in iode.h
            switch (B_fns[i].type)
            {
            // no suffix required
            case 0:
                func_list.push_back(func_name);
                break;
            // ws type required: cmt, idt, ... (see k_ext.c)
            case 1:
                for(const std::string& type_short: v_binary_ext_upper) func_list.push_back(func_name + type_short);
                break;
            // file extensions defined in k_ext.c
            case 2:
                for(const std::string& ext: v_ext_upper) func_list.push_back(func_name + ext);
                break;
            // ws type required in non GUI, no extension in GUI
            case 3:
                if(gui)
                    func_list.push_back(func_name);
                else
                    for(const std::string& type_short: v_binary_ext_upper) func_list.push_back(func_name + type_short);
                break;
            // file extensions defined in k_ext if non GUI, not required in GUI
            case 4:
                if(gui)
                    func_list.push_back(func_name);
                else
                    for(const std::string& ext: v_ext_upper) func_list.push_back(func_name + ext);
                break;
            default:
                break;
            }
        }

        i++;
    }

    return func_list;
}

/**
 * @brief Set the up @-functions list object
 * 
 * @note see b_rep_syntax.c for the list.
 * 
 */
std::vector<std::string> build_report_functions_list()
{
    std::vector<std::string> func_list;

    int i = 0;
    while(RP_FNS[i].name != 0)
    {
        func_list.push_back("@" + std::string((char*) RP_FNS[i].name));
        i++;
    } 

    return func_list;
}

/**
 * @brief Set the list of functions available in LEC expression
 * 
 * @return std::vector<std::string> 
 * 
 * @note see l_token.c
 */
std::vector<std::string> build_lec_functions_list()
{
    std::vector<std::string> func_list;

    std::string name;
    std::regex re("^\\w{2,}");
    for(int i=0; i < L_nb_tokens(); i++)
    {
        name = std::string((char*) L_TABLE[i].yk_word);
        if(std::regex_match(name, re)) func_list.push_back(name);
    }

    return func_list;
}
