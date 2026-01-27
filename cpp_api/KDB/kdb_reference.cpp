#include "kdb_reference.h"


void load_reference_kdb(const int ref, std::string& filepath)
{
    filepath = check_filepath(filepath, FILE_VARIABLES, "load", true);
    K_load_RWS(ref, (char*) filepath.c_str());
}

void clear_reference_kdb(const int ref)
{
    if(global_ref_var[ref - 1])
        delete global_ref_var[ref - 1];
    global_ref_var[ref - 1] = nullptr;
}

void clear_all_reference_kdbs()
{
    for(int ref = 2; ref <= 5; ref++)
        clear_reference_kdb(ref);
}
