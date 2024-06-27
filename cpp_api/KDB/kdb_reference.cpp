#include "kdb_reference.h"


void load_reference_kdb(const int ref, EnumIodeFile iode_file, std::string& filepath)
{
    if(ref < 2 || ref > 5) 
        throw IodeExceptionFunction("Cannot load file " + filepath + "\nInvalid value " 
        + std::to_string(ref) + " for the ref argument. Must be in range [2, 5].");
    
    filepath = check_filepath(filepath, iode_file, "load", true);

    KDB* kdb = K_interpret(iode_file, to_char_array(filepath));
    if (kdb == NULL)
        throw IodeExceptionFunction("Cannot load " + vIodeTypes[iode_file] + "s from file " + filepath, "Unknown");

    K_free(K_RWS[iode_file][ref - 1]);
    K_RWS[iode_file][ref - 1] = kdb;
}

void clear_reference_kdb(const int ref, EnumIodeFile iode_file)
{
    K_free(K_RWS[iode_file][ref - 1]);
    K_RWS[iode_file][ref - 1] = NULL;
}

void clear_all_reference_kdbs()
{
    for(int iode_file = COMMENTS_FILE; iode_file <= VARIABLES_FILE; iode_file++)
        for(int ref = 2; ref <= 5; ref++)
            clear_reference_kdb(ref, (EnumIodeFile) iode_file);
}
