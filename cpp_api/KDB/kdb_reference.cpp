#include "kdb_reference.h"


void load_reference_kdb(const int ref, std::string& filepath)
{
    if(ref < 2 || ref > 5) 
    {
        std::string msg = "Invalid Reference number. Value must be between 2 and 5 included. ";
        msg += "You provided " + std::to_string(ref);
        throw std::invalid_argument(msg);
    }

    filepath = check_filepath(filepath, FILE_VARIABLES, "load", true);
    K_load_RWS(ref, (char*) filepath.c_str());
}

void clear_reference_kdb(const int ref)
{
    if(ref < 2 || ref > 5) 
    {
        std::string msg = "Invalid Reference number. Value must be between 2 and 5 included. ";
        msg += "You provided " + std::to_string(ref);
        throw std::invalid_argument(msg);
    }

    if(global_ref_var[ref - 1])
        delete global_ref_var[ref - 1];
    global_ref_var[ref - 1] = nullptr;
}

void clear_all_reference_kdbs()
{
    global_ref_var[0] = nullptr;
    for(int ref = 2; ref <= 5; ref++)
        clear_reference_kdb(ref);
}
