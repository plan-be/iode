#include "kdb_reference.h"


void load_reference_kdb(const int ref, IodeType iode_type, std::string& filepath)
{
    if(ref < 2 || ref > 5) 
        throw std::invalid_argument("Cannot load file '" + filepath + "'.\n" + 
                "Invalid value '" + std::to_string(ref) + "' for the ref argument. " + 
                "Must be in range [2, 5].");

    filepath = check_filepath(filepath, (IodeFileType) iode_type, "load", true);

    KDB* kdb = K_interpret(iode_type, to_char_array(filepath));
    if (kdb == NULL)
        throw std::runtime_error("Cannot load " + v_iode_types[iode_type] + "s from file '" + filepath + "'");

    K_free(K_RWS[iode_type][ref - 1]);
    K_RWS[iode_type][ref - 1] = kdb;
}

void clear_reference_kdb(const int ref, IodeType iode_type)
{
    K_free(K_RWS[iode_type][ref - 1]);
    K_RWS[iode_type][ref - 1] = NULL;
}

void clear_all_reference_kdbs()
{
    for(int iode_type = COMMENTS; iode_type <= VARIABLES; iode_type++)
        for(int ref = 2; ref <= 5; ref++)
            clear_reference_kdb(ref, (IodeType) iode_type);
}
