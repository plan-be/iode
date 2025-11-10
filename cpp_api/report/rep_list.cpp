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

    std::string binary_ext;
    std::vector<std::string> v_binary_ext_upper;
    for(int i=COMMENTS; i <= VARIABLES; i++)
    {
        binary_ext = v_file_types[i].v_ext[0];
        // remove dot in front of extension
        binary_ext = binary_ext.substr(1);
        // set first character as upper case
        binary_ext[0] = toupper(binary_ext[0]);
        v_binary_ext_upper.push_back(binary_ext);
    }

    std::string ext;
    std::vector<std::string> v_ext_upper;
    v_ext_upper.push_back("");
    for(const FileType& file_type: v_file_types)
    {
        for(const std::string& ext_: file_type.v_ext)
        {
            if(!ext_.empty())
            {
                ext = ext_;
                // remove dot in front of extension
                ext = ext.substr(1);
                // set first character as upper case
                ext[0] = toupper(ext[0]);
                v_ext_upper.push_back(ext);
            }
        }
    }

    std::string prefix = gui ? "#" : "$";
    
    std::string func_name;
    int func_group;
    bool test_func;
    bool test_group;
    int i = 0;
    for(auto& [name, fns] : B_fns)
    {
        func_name = prefix + std::string(fns.func_name_camel_case);
        func_group = fns.group;

        test_func = gui ? fns.sfn != NULL : fns.fn != NULL;
        test_group = (group < 0) ? true : func_group == group;
        if(test_func && test_group)
        {
            // see documentation of BFNS structure in iode.h
            switch (fns.type)
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

    for(auto& [name, fns] : RP_FNS)
        func_list.push_back("@" + name);
    
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
