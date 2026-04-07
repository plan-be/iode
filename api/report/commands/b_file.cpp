/**
 *  @header4iode
 * 
 *  Functions acting on files called by the report engine (see b_rep_syntax.c). 
 *  
 *  List of Functions
 *  ------------------
 *      int B_FileCopy(char* arg, int type)    $FileCopy<type> source_file dest_file
 *      int B_FileRename(char* arg, int type)  $FileRename<type> source_file dest_file
 *      int B_FileDelete(char* arg, int type)  $FileDelete<type> file1 [file2...]
 */


#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/write/write.h"

#include "api/report/commands/commands.h"

/**
 *  Report function to copy a file. 
 *  For IODE object files, the filename in the KDB header is modified to reflect the
 *  new filename (calls update_filename()).
 *  
 *  Syntax:
 *  
 *       $FileCopy<type> source_file dest_file
 *
 * for details
 *  
 *  @params See b_data.c for details
 *  @return int     0 on success, -1 on error (file not found, syntax error...)
 */

int B_FileCopy(char* arg, int type)
{
    char** args = B_ainit_chk(arg, 0L, 2);
    if(!args) 
    {
        kwarning("Syntax error: no argument passed to $FileCopy<type>");
        return -1;
    }

    int nb_args = SCR_tbl_size((unsigned char**) args);
    if(nb_args != 2)
    {
        A_free((unsigned char**) args);
        kwarning("Syntax error: expected '$FileCopy<type> source_file dest_file'");
        return -1;
    }

    std::string buf_from(args[0]);
    std::string buf_to(args[1]);
    A_free((unsigned char**) args);

    buf_from = set_file_extension(buf_from, type);
    buf_to = set_file_extension(buf_to, type);

    std::filesystem::path p_filepath_from(buf_from);
    if(!std::filesystem::exists(p_filepath_from))
    {
        std::string error_msg = "$FileCopy: File '" + buf_from + "' not found";
        kwarning(error_msg.c_str());
        return -1;
    }

    // copy file p_filepath_from to p_filepath_to, overwrite if already exists
    try
    {
        std::filesystem::path p_filepath_to(buf_to);
        std::filesystem::copy_file(p_filepath_from, p_filepath_to, std::filesystem::copy_options::overwrite_existing);
    }
    catch(const std::filesystem::filesystem_error& e)
    {
        std::string error_msg = "Failed to copy file '" + buf_from + "' to '" + buf_to + "':\n";
        error_msg += e.what();
        kwarning(error_msg.c_str());
        return -1;
    }

    // update the filename attribute of the IODE database if the file represents 
    // an IODE database (type <= VARIABLES)
    if(type <= VARIABLES) 
        update_filename(buf_to, buf_to);

    return 0;
}


/**
 *  Report function to rename a file. 
 *  For IODE object files, the filename in the KDB header is modified to reflect the
 *  new filename (calls update_filename()).
 *  
 *  Syntax:
 *  
 *      $FileRename<type> source_file dest_file
 *
 * for details
 *  
 *  @params See b_data.c for details
 *  @return int     0 on success, -1 on error (file not found, syntax error...)
 */
int B_FileRename(char* arg, int type)
{
    char** args = B_ainit_chk(arg, 0L, 2);
    if(!args) 
    {
        kwarning("Syntax error: no argument passed to $FileRename<type>");
        return -1;
    }

    int nb_args = SCR_tbl_size((unsigned char**) args);
    if(nb_args != 2)
    {
        A_free((unsigned char**) args);
        kwarning("Syntax error: expected '$FileRename<type> source_file dest_file'");
        return -1;
    }

    std::string buf_from(args[0]);
    std::string buf_to(args[1]);
    A_free((unsigned char**) args);

    buf_from = set_file_extension(buf_from, type);
    buf_to = set_file_extension(buf_to, type);

    std::filesystem::path p_filepath_from(buf_from);
    if(!std::filesystem::exists(p_filepath_from))
    {
        std::string error_msg = "$FileRename: File '" + buf_from + "' not found";
        kwarning(error_msg.c_str());
        return -1;
    }

    // rename file p_filepath_from to p_filepath_to, overwrite if already exists
    try
    {
        std::filesystem::path p_filepath_to(buf_to);
        std::filesystem::rename(p_filepath_from, p_filepath_to);
    }
    catch(const std::filesystem::filesystem_error& e)
    {
        std::string error_msg = "Failed to rename file '" + buf_from + "' to '" + buf_to + "':\n";
        error_msg += e.what();
        kwarning(error_msg.c_str());
        return -1;
    }

    return 0;
}

/**
 *  Report function to delete a file(s) of a specified type. 
 *  
 *  Syntax:  
 *  
 *      $FileDelete<type> filename [filename...]
 *  
 * for details
 *  
 *  @params see b_data.c 
 *  @return int 0 always 
 */
int B_FileDelete(char* arg, int type)
{
    char** args = B_ainit_chk(arg, 0L, 2);
    if(!args) 
    {
        kwarning("Syntax error: no argument passed to $FileDelete<type>");
        return -1;
    }

    int nb_args = SCR_tbl_size((unsigned char**) args);
    std::vector<std::string> files_to_delete;
    for(int i = 0; i < nb_args; i++)
    {
        std::string buf(args[i]);

        buf = set_file_extension(buf, type);
        files_to_delete.push_back(buf);
    }
    A_free((unsigned char**) args);

    for(const std::string& file : files_to_delete)
    {
        std::filesystem::path p_filepath(file);
        if(std::filesystem::exists(p_filepath))
        {
            try
            {
                std::filesystem::remove(p_filepath);
            }
            catch(const std::filesystem::filesystem_error& e)
            {
                std::string error_msg = "Failed to delete file '" + file + "':\n";
                error_msg += e.what();
                kwarning(error_msg.c_str());
            }
        }
        else
        {
            std::string error_msg = "$FileDelete: File '" + file + "' not found";
            kwarning(error_msg.c_str());
        }
    }

    return 0;
}
