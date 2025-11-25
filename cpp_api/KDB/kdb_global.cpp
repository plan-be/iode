#pragma once
#include "kdb_global.h"


bool is_global_database_loaded(const IodeType iodeType) 
{ 
    if(!get_global_db(iodeType)) 
        return false;
    if(get_global_db(iodeType)->size() == 0) 
        return false;
    return true; 
}

void set_kdb_filename(KDB* kdb, const std::string& filename)
{
    kdb->filepath = filename;
}

void import_cmt(const std::string& input_file, const std::string& save_file, const std::string& rule_file, 
                const TableLang lang, const std::string& debug_file)
{
    // $FileImportCmt format rule infile outfile language [trace]

    std::string args = "";
    std::string caller_name = "import comments";
    std::string error_msg = "Cannot import comments from file " + input_file;

    // format = ASCII
    args += "0 ";

    std::string rule_file_ = check_file_exists(rule_file, caller_name);
    args += rule_file_ + " ";

    std::string input_file_ = check_file_exists(input_file, caller_name);
    args += input_file_ + " ";

    std::string save_file_ = check_filepath(save_file, FILE_COMMENTS, caller_name, false);
    args += save_file_ + " ";

    args += std::to_string((int) (lang - TABLE_ENGLISH)) + " ";

    if (!debug_file.empty())
    {
        std::string debug_file_ = check_filepath(debug_file, FILE_LOG, caller_name, false);
        args += debug_file_ + " ";
    }

    int res = B_FileImportCmt(to_char_array(args));
    if (res < 0) 
        throw std::runtime_error(error_msg);
}

void import_var(const std::string& input_file, const std::string& save_file, const std::string& from, const std::string& to, 
                const std::string& rule_file, const std::string& debug_file)
{
	// $FileImportVar format rule infile outfile from to  [trace]

    std::string args = "";
    std::string caller_name = "import variables";
    std::string error_msg = "Cannot import variables from file " + input_file;

    // fromat = ASCII
    args += "0 ";

    std::string rule_file_ = check_file_exists(rule_file, caller_name);
    args += rule_file_ + " ";

    std::string input_file_ = check_file_exists(input_file, caller_name);
    args += input_file_ + " ";

    std::string save_file_ = check_filepath(save_file, FILE_VARIABLES, caller_name, false);
    args += save_file_ + " ";

    // raise error if not valid
    Period from_per(from);
	args+= from_per.to_string() + " ";

    Period to_per(to);
	args += to_per.to_string() + " ";

    if (!debug_file.empty())
    {
        std::string debug_file_ = check_filepath(debug_file, FILE_LOG, caller_name, false);
        args += debug_file_ + " ";
    }

    int res = B_FileImportVar(to_char_array(args));
    if (res < 0) 
        throw std::runtime_error(error_msg);
}

void import_var(const std::string& input_file, const std::string& save_file, const Period& from, const Period& to, 
                const std::string& rule_file, const std::string& debug_file)
{
    import_var(input_file, rule_file, save_file, from.to_string(), to.to_string(), debug_file);
}

void export_as(const std::string& var_file, const std::string cmt_file, const std::string& from, const std::string& to, 
			   const IodeExportFormat format, const std::string& save_file, const std::string& rule_file, 
			   const std::string& nan, const std::string& separator, const std::string& debug_file)
{
    bool success = false;
    std::string caller_name = "export_as";
    std::string error_msg = "Cannot export ";
        
    Sample* smpl = NULL;
    if(!(from.empty() || to.empty())) 
    {
        // raise an error if not valid
        Sample sample(from, to);

        memcpy(&(smpl->start_period), &(sample.start_period), sizeof(Period));
        memcpy(&(smpl->end_period), &(sample.end_period), sizeof(Period));
        smpl->nb_periods = sample.nb_periods;
    }

    CKDBComments* dbc = new CKDBComments(DB_STANDALONE);
    if(!cmt_file.empty())
    {
        std::string cmt_file_ = check_file_exists(cmt_file, caller_name);
        error_msg += "Comments file " + cmt_file_;
        success = dbc->load(cmt_file_);
        if(!success)
            throw std::invalid_argument(error_msg + "\n" + "Comment file: '" + cmt_file + "'");
    } 

    KDB* dbv = new KDB(VARIABLES, false);
    if(!var_file.empty()) 
    {
        std::string var_file_ = check_file_exists(var_file, caller_name);
        if(dbc) error_msg += "and";
        error_msg += "Variables file " + var_file_;
        success = dbv->load(var_file);
        if(!success)
            throw std::invalid_argument(error_msg + "\n" + "Variable file: '" + var_file + "'");
        
        if(smpl) 
            KV_sample(dbv, smpl);
    }

    std::string rule_file_ = check_file_exists(rule_file, caller_name);

    std::string save_file_ = check_filepath(save_file, FILE_COMMENTS, caller_name, false);

    if(!debug_file.empty()) 
    {
        std::string debug_file_ = check_filepath(debug_file, FILE_LOG, caller_name, false);
        K_WARN_DUP = 0;
        W_dest(to_char_array(debug_file_), W_A2M);
    }
    else 
    {
        K_WARN_DUP = 1;
        IMP_trace = 0;
    }

    ExportToFile* expdef = export_handlers[format].get();

    int res;
    if(format < EXPORT_RCSV)
        res = EXP_Ws(expdef, dbv, dbc, to_char_array(rule_file_), to_char_array(save_file_), 
                     to_char_array(nan), to_char_array(separator));
    else
        res = EXP_Rev_Ws(expdef, dbv, dbc, to_char_array(rule_file_), to_char_array(save_file_), 
                         to_char_array(nan), to_char_array(separator));


    if(dbv) delete dbv;
    if(dbc) delete dbc;

    K_WARN_DUP = 0;

    if(res != 0)
    {
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            throw std::runtime_error("Cannot export variables\n" + last_error);
    }
}

void low_to_high(const IodeLowToHigh type, const char method, const std::string& filepath, const std::string& var_list)
{
    int res;

    check_filepath(filepath, FILE_VARIABLES, "low_to_high", true);
    std::string method_name = mLowToHigh.at(method);

    std::string arg = std::string(1, method) + " " + filepath + " " + var_list;
    char* c_arg = to_char_array(arg);

    if (type == LTOH_STOCK) res = B_WsLtoHStock(c_arg);
    else res = B_WsLtoHFlow(c_arg);

    if (res < 0)
    {
        std::string which_type = (type == LTOH_STOCK) ? "Stock" : "Flow";
        std::string error_msg = "Cannot Cannot transforms low periodicity variables into high periodicity variables\n";
        error_msg += "type: " + which_type + "\n";
        error_msg += "method: " + method_name + "\n";
        error_msg += "file: " + filepath + "\n";
        error_msg += "variables list: " + var_list;
        throw std::runtime_error(error_msg);
    }
}

void high_to_low(const IodeHighToLow type, const std::string& filepath, const std::string& var_list)
{
    int res;
    std::string type_name; 

    check_filepath(filepath, FILE_VARIABLES, "high_to_low", true);

    std::string arg = filepath + " " + var_list;
    char* c_arg = to_char_array(arg);

    switch (type)
    {
    case HTOL_LAST:
        res = B_WsHtoLLast(c_arg);
        type_name = "Last";
        break;
    case HTOL_MEAN:
        res = B_WsHtoLMean(c_arg);
        type_name = "Mean";
        break;
    case HTOL_SUM:
        res = B_WsHtoLSum(c_arg);
        type_name = "Sum";
        break;
    default:
        break;
    }

    if (res < 0)
    {
        std::string error_msg = "Cannot transforms high periodicity variables into low periodicity variables\n";
        error_msg += "type: " + type_name + "\n";
        error_msg += "file: " + filepath + "\n";
        error_msg += "variables list: " + var_list;
        throw std::runtime_error(error_msg);
    }
}
