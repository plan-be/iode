#pragma once
#include "utils/utils.h"
#include "time/sample.h"


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


void load_global_kdb(EnumIodeType iode_type, const std::string& filepath);

void save_global_kdb(EnumIodeType iode_type, const std::string& filepath);

void clear_global_kdb(EnumIodeType iode_type);

/**
 * @brief check if data for IODE objects of type iodeType has been loaded in global K_WS 
 * 
 * @param iodeType EnumIodeType type of Iode object.
 * 
 * @return bool Wether or not K_WS corresponding to iodeType is empty.
 */
bool is_global_kdb_loaded(const EnumIodeType iodeType);

char** filter_kdb_names_char_table(const EnumIodeType iode_type, const std::string& pattern);

std::vector<std::string> filter_kdb_names(const EnumIodeType iode_type, const std::string& pattern);

// QUESTION FOR JMP: is there a K function that already create a hard copy of subset of a KDB ?
KDB* hard_copy_kdb(KDB* source_kdb, char** names=NULL);

// NOTE: remove function below -> IodeContents()
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

// NOTE: The unit test Tests_IMP_EXP() in test1.c calls EXP_RuleExport() many times and 
//       generates output files in output/fun_xode.. 
//       These files are compared with data/fun_xode.ref..
//       The last call to EXP_RuleExport() uses the simple rule file data/rules.txt and 
//       creates another example of exported file.
// NOTE: export is a C++ reserved keyword
/**
 * @brief export variable from file.
 * 
 *  Syntax: $FileExport vfile cfile from to format outfile rule nan separator [trace]
 *  where:
 *      vfile = variables file to export
 *      cfile = variables file to export
 *      from = begin of sample
 *      to   = end of sample
 *      format = {CSV, DIF, WKS, TSP, Reverse CSV}
 *      outfile = exported file
 *      rule = rulefile to use to translate names
 *      nan = string to use for NaN values (CSV only)
 *      separator = character used to separate value (CSV only)
 *      trace = debug file (optional)
 *  
 *  @see https://iode.plan.be/doku.php?id=export
 */
void export_as(const std::string& var_file, const std::string cmt_file, const std::string& from, const std::string& to, 
               const EnumIodeExportFormat format, const std::string& save_file, const std::string& rule_file, 
               const std::string& nan, const std::string& separator, const std::string& debug_file = "");

void low_to_high(const EnumIodeLtoH type, const char method, const std::string& filepath, const std::string& var_list);

void high_to_low(const EnumIodeHtoL type, const std::string& filepath, const std::string& var_list);
