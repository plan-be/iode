#include "kdb_reference.h"


void load_reference_kdb(const int ref, IodeType iode_type, std::string& filepath)
{
    filepath = check_filepath(filepath, (IodeFileType) iode_type, "load", true);
    K_load_RWS(ref, (char*) filepath.c_str());
}

void clear_reference_kdb(const int ref, IodeType iode_type)
{
    if(K_RWS[iode_type][ref - 1])
        delete K_RWS[iode_type][ref - 1];
    K_RWS[iode_type][ref - 1] = nullptr;
}

void clear_all_reference_kdbs()
{
    for(int iode_type = COMMENTS; iode_type <= VARIABLES; iode_type++)
        for(int ref = 2; ref <= 5; ref++)
            clear_reference_kdb(ref, (IodeType) iode_type);
}
