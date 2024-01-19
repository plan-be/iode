#pragma once
#include "kdb_global.h"


void load_global_kdb(EnumIodeType iode_type, const std::string& filepath)
{
    std::string filepath_ = check_filepath(filepath, (EnumIodeFile) iode_type, "load", true);

    char* c_filepath = to_char_array(filepath_);

    int res = B_WsLoad(c_filepath, iode_type);
    if (res != EXIT_SUCCESS)
        throw IodeExceptionFunction("Cannot load " + vIodeTypes[iode_type] + "s from file " + filepath, "Unknown");
}

void save_global_kdb(EnumIodeType iode_type, const std::string& filepath)
{
    std::string filepath_ = check_filepath(filepath, (EnumIodeFile) iode_type, "save", false);

    char* c_filepath = to_char_array(filepath_);

    int res = B_WsSave(c_filepath, iode_type);
    if (res != EXIT_SUCCESS)
        throw IodeExceptionFunction("Cannot save " + vIodeTypes[iode_type] + "s to file " + filepath, "Unknown");
}

void clear_global_kdb(EnumIodeType iode_type)
{
    int res = B_WsClear("", iode_type);
    if (res != EXIT_SUCCESS)
        throw IodeExceptionFunction("Cannot clear " + vIodeTypes[iode_type] + "s database", "Unknown");
}

bool is_global_kdb_loaded(const EnumIodeType iodeType) 
{ 
    if (K_WS[iodeType] == NULL) return false;
    if (K_WS[iodeType]->k_nb == 0) return false;
    return true; 
}

KDB* hard_copy_kdb(KDB* source_kdb, char** names)
{
    short iode_type = source_kdb->k_type;
    int nb_objs = (names != NULL) ? SCR_tbl_size((unsigned char**) names) : source_kdb->k_nb;

    // K_create() sets:
    // - k_arch = #define ARCH
    // - k_type = first argument of K_create()
    // - k_mode = second argument of K_create()
    // - k_nameptr = I_DEFAULT_FILENAME
    KDB* dest_kdb = K_create(iode_type, source_kdb->k_mode);
    if (dest_kdb == NULL) return NULL;
    // copy char k_desc[K_MAX_DESC]
    strncpy(dest_kdb->k_desc, source_kdb->k_desc, K_MAX_DESC);
    // copy char k_data[K_MAX_DESC]
    memcpy(dest_kdb->k_data, source_kdb->k_data, sizeof(char) * K_MAX_DESC);
    // copy char k_compressed
    dest_kdb->k_compressed = source_kdb->k_compressed;
    // copy char* k_nameptr
    dest_kdb->k_nameptr = copy_char_array(source_kdb->k_nameptr);
    // copy k_magic[LMAGIC]
    memcpy(dest_kdb->k_magic, source_kdb->k_magic, sizeof(char) * LMAGIC);
    // copy k_oname[OK_MAX_FILE]  
    strncpy(dest_kdb->k_oname, source_kdb->k_oname, OK_MAX_FILE);

    int pos_dest_kdb;
    char* c_obj_name;
    for (int i=0; i<nb_objs; i++)
    {
        c_obj_name = (names != NULL) ? names[i] : KONAME(source_kdb, i);
        pos_dest_kdb = K_dup(source_kdb, c_obj_name, dest_kdb, c_obj_name);
        if (pos_dest_kdb < 0)
        {
            std::string obj_name = std::string(c_obj_name);
            std::string action = "Trying to copy " + vIodeTypes[iode_type] + " named " + obj_name + " in new database";
            if (pos_dest_kdb == -1) throw IodeExceptionFunction(action, "Object with name " + 
                std::string(obj_name) + " does not exist in the source database");
            else throw IodeExceptionFunction(action, "Unknown");
        }
    }

    return dest_kdb;
}

// Replaced by K_get_kdb_nameptr()
// 
// std::string get_kdb_filename(KDB* kdb)
// {
//     return std::string(kdb->k_nameptr);
// }

void set_kdb_filename(KDB* kdb, const std::string& filename)
{
    K_set_kdb_name(kdb, (unsigned char*) filename.c_str());
}

void import_cmt(const std::string& input_file, const std::string& save_file, const std::string& rule_file, 
                const EnumLang lang, const std::string& debug_file)
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

    std::string save_file_ = check_filepath(save_file, I_COMMENTS_FILE, caller_name, false);
    args += save_file_ + " ";

    args += std::to_string((int) lang) + " ";

    if (!debug_file.empty())
    {
        std::string debug_file_ = check_filepath(debug_file, I_LOGS_FILE, caller_name, false);
        args += debug_file_ + " ";
    }

    int res = B_FileImportCmt(to_char_array(args));
    if (res < 0) throw IodeExceptionFunction(error_msg);
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

    std::string save_file_ = check_filepath(save_file, I_VARIABLES_FILE, caller_name, false);
    args += save_file_ + " ";

    // raise error if not valid
    Period from_per(from);
	args+= from_per.to_string() + " ";

    Period to_per(to);
	args += to_per.to_string() + " ";

    if (!debug_file.empty())
    {
        std::string debug_file_ = check_filepath(debug_file, I_LOGS_FILE, caller_name, false);
        args += debug_file_ + " ";
    }

    int res = B_FileImportVar(to_char_array(args));
    if (res < 0) throw IodeExceptionFunction(error_msg);
}

void import_var(const std::string& input_file, const std::string& save_file, const Period& from, const Period& to, 
                const std::string& rule_file, const std::string& debug_file)
{
    import_var(input_file, rule_file, save_file, from.to_string(), to.to_string(), debug_file);
}

void export_as(const std::string& var_file, const std::string cmt_file, const std::string& from, const std::string& to, 
			   const EnumIodeExportFormat format, const std::string& save_file, const std::string& rule_file, 
			   const std::string& nan, const std::string& separator, const std::string& debug_file)
{
    std::string caller_name = "export_as";
    std::string error_msg = "Cannot export ";
        
    SAMPLE* smpl;
    if(from.empty() || to.empty()) 
        smpl = NULL;
    else 
    {
        // raise an error if not valid
        Sample sample(from, to);

        memcpy(&(smpl->s_p1), &(sample.s_p1), sizeof(PERIOD));
        memcpy(&(smpl->s_p2), &(sample.s_p2), sizeof(PERIOD));
        smpl->s_nb = sample.s_nb;
    }

    KDB* dbc;
    if(!cmt_file.empty())
    {
        std::string cmt_file_ = check_file_exists(cmt_file, caller_name);
        error_msg += "Comments file " + cmt_file_;
        dbc = K_interpret(K_CMT, to_char_array(cmt_file));
        if(dbc == NULL)
        {
            IodeExceptionInvalidArguments error(error_msg);
            error.add_argument("Comment file", cmt_file);
            throw error;
        }
    } 

    KDB* dbv;
    if(!var_file.empty()) 
    {
        std::string var_file_ = check_file_exists(var_file, caller_name);
        if (dbc != NULL) error_msg += "and";
        error_msg += "Variables file " + var_file_;
        dbv = K_interpret(K_VAR, to_char_array(var_file));
        if(dbv == NULL)
        {
            IodeExceptionInvalidArguments error(error_msg);
            error.add_argument("Variable file", var_file);
            throw error;
        }
        
        if(smpl != NULL) 
            KV_sample(dbv, smpl);
    }

    std::string rule_file_ = check_file_exists(rule_file, caller_name);

    std::string save_file_ = check_filepath(save_file, I_COMMENTS_FILE, caller_name, false);

    if(!debug_file.empty()) 
    {
        std::string debug_file_ = check_filepath(debug_file, I_LOGS_FILE, caller_name, false);
        K_WARN_DUP = 0;
        W_dest(to_char_array(debug_file_), W_A2M);
    }
    else 
    {
        K_WARN_DUP = 1;
        IMP_trace = 0;
    }

    EXPDEF  *expdef;
    switch(format) {
        case I_EXP_FMT_CSV:
            expdef = &EXPCSV;
            break;
        case I_EXP_FMT_DIF:
            expdef = &EXPDIF;
            break;
        case I_EXP_FMT_WKS:
            expdef = &EXPWKS;
            break;
        case I_EXP_FMT_TSP:
            expdef = &EXPTSP;
            break;
        default:
            expdef = &EXPRCSV;
    }

    int res;
    if(format < I_EXP_FMT_RCSV)
        res = EXP_Ws(expdef, dbv, dbc, to_char_array(rule_file_), to_char_array(save_file_), 
                     to_char_array(nan), to_char_array(separator));
    else
        res = EXP_Rev_Ws(expdef, dbv, dbc, to_char_array(rule_file_), to_char_array(save_file_), 
                         to_char_array(nan), to_char_array(separator));


    if(dbv != NULL) K_free(dbv);
    if(dbc != NULL) K_free(dbc);

    K_WARN_DUP = 0;

    if(res != 0) 
        B_display_last_error();
}

void low_to_high(const EnumIodeLtoH type, const char method, const std::string& filepath, const std::string& var_list)
{
    int res;

    check_filepath(filepath, I_VARIABLES_FILE, "low_to_high", true);
    std::string method_name = mLowToHigh.at(method);

    std::string arg = std::string(1, method) + " " + filepath + " " + var_list;
    char* c_arg = to_char_array(arg);

    if (type == LTOH_STOCK) res = B_WsLtoHStock(c_arg);
    else res = B_WsLtoHFlow(c_arg);

    if (res < 0)
    {
        IodeExceptionFunction error("Cannot Cannot transforms low periodicity variables into high periodicity variables");
        error.add_argument("type", (type == LTOH_STOCK) ? "Stock" : "Flow");
        error.add_argument("method", method_name);
        error.add_argument("file", filepath);
        error.add_argument("variables list", var_list);
        throw error;
    }
}

void high_to_low(const EnumIodeHtoL type, const std::string& filepath, const std::string& var_list)
{
    int res;
    std::string type_name; 

    check_filepath(filepath, I_VARIABLES_FILE, "high_to_low", true);

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
        IodeExceptionFunction error("Cannot transforms high periodicity variables into low periodicity variables");
        error.add_argument("type", type_name);
        error.add_argument("file", filepath);
        error.add_argument("variables list", var_list);
        throw error;
    }
}
