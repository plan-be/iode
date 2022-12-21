#pragma once
#include "utils/utils.h"


enum EnumIodeLtoH
{
    LTOH_STOCK,
    LTOH_FLOW
};

enum EnumIodeHtoL
{
    HTOL_LAST,
    HTOL_MEAN,
    HTOL_SUM 
};

const std::map<char, std::string> mLowToHigh = 
{
    {'L', "Linear interpolation"},
    {'C', "Cubic Splines"},
    {'S', "Interpolation by Steps"}
};


void load_global_kdb(EnumIodeType iode_type, std::string& filepath);

void save_global_kdb(EnumIodeType iode_type, std::string& filepath);

void clear_global_kdb(EnumIodeType iode_type);

char** filter_kdb_names(const EnumIodeType iode_type, const std::string& pattern);

// QUESTION FOR JMP: is there a K function that already create a hard copy of subset of a KDB ?
KDB* hard_copy_kdb(KDB* source_kdb, char** names);

// require to be compiled using -Zp1 option 
// std::string get_kdb_filename(KDB* kdb); (replaced by C API function K_get_kdb_nameptr())

void set_kdb_filename(KDB* kdb, const std::string& filename);

/**
 * @brief import comments from file.
 * 
 *  Syntax: $FileImportCmt rule infile outfile language [trace]
 *  where:
 *      rule = rulefile to use to translate names
 *      infile = file to import
 *      outfile = IODE-file with imported series
 *      language =  {E,F,D}
 *      trace = debug file (optional)
 *  
 *  @see https://iode.plan.be/doku.php?id=fileimportcmt
 */
void import_cmt(const std::string& input_file, const std::string& save_file, const std::string& rule_file, 
                const EnumLang lang = IT_ENGLISH, const std::string& debug_file = "");

// Note: for unit test, see test_c_api.cpp and files: 
//       - tests/data/rules.txt 
//       - tests/data/fun2.ref.tsp
//       - tests/data/fun_xode.av.ref
/**
 * @brief import variables from file.
 * 
 *  Syntax: $FileImportVar rule infile outfile from to  [trace]
 *  where:
 *      rule = rulefile to use to translate names
 *      infile = file to import
 *      outfile = IODE-file with imported series
 *      from = begin of sample
 *      to   = end of sample
 *      trace = debug file (optional)
 *  
 *  @see https://iode.plan.be/doku.php?id=fileimportcmt
 */
void import_var(const std::string& input_file, const std::string& save_file, const std::string& from, const std::string& to, 
                const std::string& rule_file, const std::string& debug_file = "");

// Note: for unit test, see test_c_api.cpp and files: 
//       - tests/data/rules.txt 
//       - tests/data/fun2.ref.tsp
//       - tests/data/fun_xode.av.ref
/**
 * @brief import variables from file.
 * 
 *  Syntax: $FileImportVar rule infile outfile from to  [trace]
 *          
 *  where:
 *      rule = rulefile to use to translate names
 *      infile = file to import
 *      outfile = IODE-file with imported series
 *      from = begin of sample
 *      to   = end of sample
 *      trace = debug file (optional)
 *  
 *  @see https://iode.plan.be/doku.php?id=fileimportvar
 */
void import_var(const std::string& input_file, const std::string& save_file, const Period& from, const Period& to, 
                const std::string& rule_file, const std::string& debug_file = "");

void low_to_high(const EnumIodeLtoH type, const char method, std::string& filepath, const std::string& var_list);

void high_to_low(const EnumIodeHtoL type, std::string& filepath, const std::string& var_list);
