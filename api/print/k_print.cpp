/**
 *  @header4iode
 *
 *  Functions to generate IODE tables in A2M format based on Table structures and GSample definition.
 *  Includes some A2M helper functions.
 *
 *  Note that the functions needed to generate graphs from tables can be found in k_graph.c.
 */
#include "scr4/s_a2m.h"

#include "api/pch.h"
#include "api/b_a2mini.h"
#include "api/k_lang.h"
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/print/print.h"
#include "api/write/write.h"


/**
 *  Compiles a GSample into a std::vector<COL> struct and resizes std::vector<COL> according to the nb of cols in Table.
 *
 *  @param [in] std::shared_ptr<Table> tbl_ptr   table to compute
 *  @param [in] const std::string&     gsample   GSample
 *  @param [out] std::vector<COL>& columns  column definitions for computing the table
 *  @return     int                              total number of columns for the computed table
 *
 */
int initialize_columns(std::shared_ptr<Table> tbl_ptr, const std::string& gsample, std::vector<COL>& columns)
{
    if(!tbl_ptr)
        return -1;

    columns = compile_gsample((char*) gsample.c_str());
    if(columns.empty())
    {
        std::string error_msg = "Illegal sample '" + gsample + "': syntax error";
        error_manager.append_error(error_msg);
        return -1;
    }

    int dim = resize_tbl_columns(*tbl_ptr, columns);
    return dim;
}


/**
 *  Retrieves the filenames used in the std::vector<COL> (from GSample) needed to print the special table line TABLE_LINE_FILES.
 *
 *  @param [in] const std::vector<COL>& columns  compiled GSample columns
 *  @return     char**          NULL if one of the ref files is not loaded in global_ref_xxx
 *                              table of filenames in the form "[<file number>] <filename>" if all files are in mem
 */
std::vector<std::string> extract_files_tbl(const std::vector<COL>& columns)
{
    std::vector<bool> files(K_MAX_FREF + 1, false);
    for(const COL& column : columns)
    {
        files[column.cl_fnb[0]] = true;
        files[column.cl_fnb[1]] = true;
    }

    KDBVariablesPtr kdb;
    std::string filename;
    std::vector<std::string> filenames;
    for(int i = 1; i < K_MAX_FREF + 1; i++)
    {
        if(!files[i])
            continue;

        kdb = global_ref_var[i - 1];
        if(!kdb)
        {
            std::string error_msg = "File " + std::to_string(i) + " not present";
            error_manager.append_error(error_msg);
            return std::vector<std::string>();
        }

        filename = "[" + std::to_string(i) + "] " + kdb->filepath;
        std::replace(filename.begin(), filename.end(), '\\', '/');
        filenames.push_back(filename);
    }

    return filenames;
}
