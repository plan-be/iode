#include "py_ws.h"


void init_ws(nb::module_ &m)
{
    // Workspace functions

    m.def("ws_content", &ws_content, nb::arg("pattern"), nb::arg("iode_type") = (int) VARIABLES, 
        "Return the names of objects of a given IODE data type, satisfying a pattern specification.");
    m.def("ws_content_cmt", [](const std::string& pattern){ return ws_content(pattern, COMMENTS); }, nb::arg("pattern"),
        "Return the list of comment names corresponding to the given pattern");
    m.def("ws_content_eqs", [](const std::string& pattern){ return ws_content(pattern, EQUATIONS); }, nb::arg("pattern"),
        "Return the list of equation names corresponding to the given pattern");
    m.def("ws_content_idt", [](const std::string& pattern){ return ws_content(pattern, IDENTITIES); }, nb::arg("pattern"),
        "Return the list of identity names corresponding to the given pattern");
    m.def("ws_content_lst", [](const std::string& pattern){ return ws_content(pattern, LISTS); }, nb::arg("pattern"),
        "Return the list of list names corresponding to the given pattern");
    m.def("ws_content_scl", [](const std::string& pattern){ return ws_content(pattern, SCALARS); }, nb::arg("pattern"),
        "Return the list of scalar names corresponding to the given pattern");
    m.def("ws_content_tbl", [](const std::string& pattern){ return ws_content(pattern, TABLES); }, nb::arg("pattern"),
        "Return the list of table names corresponding to the given pattern");
    m.def("ws_content_var", [](const std::string& pattern){ return ws_content(pattern, VARIABLES); }, nb::arg("pattern"),
        "Return the list of variable names corresponding to the given pattern");

    m.def("ws_clear_all", &ws_clear_all, "Clear all workspaces");
    m.def("ws_clear", &ws_clear, nb::arg("iode_type"), "Clear the workspace for a given IODE data type");
    m.def("ws_clear_cmt", [](){ Comments.clear(); },   "Clear the workspace of comments");
    m.def("ws_clear_eqs", [](){ Equations.clear(); },  "Clear the workspace of equations");
    m.def("ws_clear_idt", [](){ Identities.clear(); }, "Clear the workspace of identities");
    m.def("ws_clear_lst", [](){ Lists.clear(); },      "Clear the workspace of lists");
    m.def("ws_clear_scl", [](){ Scalars.clear(); },    "Clear the workspace of scalars");
    m.def("ws_clear_tbl", [](){ Tables.clear(); },     "Clear the workspace of tables");
    m.def("ws_clear_var", [](){ Variables.clear(); },  "Clear the workspace of variables");

    m.def("ws_load", &ws_load, nb::arg("filename"), nb::arg("iode_type") = -1, 
        "Load an IODE file and return the number of read objects");
    m.def("ws_load_cmt", [](const std::string& filename){ Comments.load(filename);   return Comments.count(); }, nb::arg("filename"), 
        "Load a comment file and return the number of read objects");
    m.def("ws_load_eqs", [](const std::string& filename){ Equations.load(filename);  return Equations.count(); }, nb::arg("filename"), 
        "Load an equation file and return the number of read objects");
    m.def("ws_load_idt", [](const std::string& filename){ Identities.load(filename); return Identities.count(); }, nb::arg("filename"), 
        "Load a identity file and return the number of read objects");
    m.def("ws_load_lst", [](const std::string& filename){ Lists.load(filename);      return Lists.count(); }, nb::arg("filename"), 
        "Load a list file and return the number of read objects");
    m.def("ws_load_scl", [](const std::string& filename){ Scalars.load(filename);    return Scalars.count(); }, nb::arg("filename"), 
        "Load a scalar file and return the number of read objects");
    m.def("ws_load_tbl", [](const std::string& filename){ Tables.load(filename);     return Tables.count(); }, nb::arg("filename"), 
        "Load a table file and return the number of read objects");
    m.def("ws_load_var", [](const std::string& filename){ Variables.load(filename);  return Variables.count(); }, nb::arg("filename"), 
        "Load a variable file and return the number of read objects");

    m.def("ws_save", &ws_save, nb::arg("filename"), nb::arg("iode_type"), 
        "Save the current IODE workspace of a given IODE data type");
    m.def("ws_save_cmt", [](const std::string& filename){ Comments.save(filename); },   nb::arg("filename"), 
        "Save the current comments workspace");
    m.def("ws_save_eqs", [](const std::string& filename){ Equations.save(filename); },  nb::arg("filename"), 
        "Save the current equations workspace");
    m.def("ws_save_idt", [](const std::string& filename){ Identities.save(filename); }, nb::arg("filename"), 
        "Save the current identities workspace");
    m.def("ws_save_lst", [](const std::string& filename){ Lists.save(filename); },      nb::arg("filename"), 
        "Save the current lists workspace");
    m.def("ws_save_scl", [](const std::string& filename){ Scalars.save(filename); },    nb::arg("filename"), 
        "Save the current scalars workspace");
    m.def("ws_save_tbl", [](const std::string& filename){ Tables.save(filename); },     nb::arg("filename"), 
        "Save the current tables workspace");
    m.def("ws_save_var", [](const std::string& filename){ Variables.save(filename); },  nb::arg("filename"), 
        "Save the current variables workspace");

    // High To Low functions

    m.def("ws_htol", nb::overload_cast<const std::string&, const std::string&, const int>(&ws_htol), 
        nb::arg("filename"), nb::arg("varlist"), nb::arg("series_type"));
    m.def("ws_htol_last", [](const std::string& filename, const std::string& varlist) { ws_htol(filename, varlist, (int) IodeHighToLow::HTOL_LAST); }, 
        nb::arg("filename"), nb::arg("varlist"));
    m.def("ws_htol_mean", [](const std::string& filename, const std::string& varlist) { ws_htol(filename, varlist, (int) IodeHighToLow::HTOL_MEAN); }, 
        nb::arg("filename"), nb::arg("varlist"));
    m.def("ws_htol_sum",  [](const std::string& filename, const std::string& varlist) { ws_htol(filename, varlist, (int) IodeHighToLow::HTOL_SUM); }, 
        nb::arg("filename"), nb::arg("varlist"));

    m.def("ws_htol", nb::overload_cast<const std::string&, const std::vector<std::string>&, const int>(&ws_htol), 
        nb::arg("filename"), nb::arg("varlist"), nb::arg("series_type"));
    m.def("ws_htol_last", [](const std::string& filename, const std::vector<std::string>& varlist) { ws_htol(filename, varlist, (int) IodeHighToLow::HTOL_LAST); }, 
        nb::arg("filename"), nb::arg("varlist"));
    m.def("ws_htol_mean", [](const std::string& filename, const std::vector<std::string>& varlist) { ws_htol(filename, varlist, (int) IodeHighToLow::HTOL_MEAN); }, 
        nb::arg("filename"), nb::arg("varlist"));
    m.def("ws_htol_sum",  [](const std::string& filename, const std::vector<std::string>& varlist) { ws_htol(filename, varlist, (int) IodeHighToLow::HTOL_SUM); }, 
        nb::arg("filename"), nb::arg("varlist"));

    // Low To High functions

    m.def("ws_ltoh", nb::overload_cast<const std::string&, const std::string&, const int, const char>(&ws_ltoh), 
        nb::arg("filename"), nb::arg("varlist"), nb::arg("series_type"), nb::arg("method"));
    m.def("ws_ltoh_flow",  [](const std::string& filename, const std::string& varlist, const char method = 'C') 
        { ws_ltoh(filename, varlist, IodeLowToHigh::LTOH_FLOW, method); }, 
        nb::arg("filename"), nb::arg("varlist"), nb::arg("method") = 'C');
    m.def("ws_ltoh_stock", [](const std::string& filename, const std::string& varlist, const char method = 'C') 
        { ws_ltoh(filename, varlist, IodeLowToHigh::LTOH_STOCK, method); }, 
        nb::arg("filename"), nb::arg("varlist"), nb::arg("method") = 'C');

    m.def("ws_ltoh", nb::overload_cast<const std::string&, const std::vector<std::string>&, const int, const char>(&ws_ltoh), 
        nb::arg("filename"), nb::arg("varlist"), nb::arg("series_type"), nb::arg("method"));
    m.def("ws_ltoh_flow",  [](const std::string& filename, const std::vector<std::string>& varlist, const char method = 'C') 
        { ws_ltoh(filename, varlist, IodeLowToHigh::LTOH_FLOW, method); }, 
        nb::arg("filename"), nb::arg("varlist"), nb::arg("method") = 'C');
    m.def("ws_ltoh_stock", [](const std::string& filename, const std::vector<std::string>& varlist, const char method = 'C') 
        { ws_ltoh(filename, varlist, IodeLowToHigh::LTOH_STOCK, method); }, 
        nb::arg("filename"), nb::arg("varlist"), nb::arg("method") = 'C');
}


std::vector<std::string> ws_content(const std::string& pattern, const int iode_type)
{
    if(iode_type < 0 || iode_type > VARIABLES)
        throw std::invalid_argument("Invalid value " + std::to_string(iode_type) + " for the argument " +
                                    "'iode_type'.\nThe value must be in range [0, " + 
                                    std::to_string(VARIABLES) + "]");

    char** cnt = IodeContents(const_cast<char*>(pattern.c_str()), iode_type);

    std::vector<std::string> list;
    if (cnt == NULL)
        return list;
    
    int nb = 0;
    std::string elem;
    while(cnt[nb] != NULL)
    {
        elem = std::string(cnt[nb]);
        list.push_back(elem);
        nb++;
    }
    return list;
}

// Clear WS

void ws_clear_all()
{
    Comments.clear();
    Equations.clear();
    Identities.clear();
    Lists.clear();
    Scalars.clear();
    Tables.clear();
    Variables.clear();
}

void ws_clear(const int iode_type)
{
    switch (iode_type)
    {
    case COMMENTS:
        Comments.clear();
        break;
    case EQUATIONS:
        Equations.clear();
        break;
    case IDENTITIES:
        Identities.clear();
        break;
    case LISTS:
        Lists.clear();
        break;
    case SCALARS:
        Scalars.clear();
        break;
    case TABLES:
        Tables.clear();
        break;
    case VARIABLES:
        Variables.clear();
        break;    
    default:
        throw std::invalid_argument("Invalid value " + std::to_string(iode_type) + " for the argument " +
                                    "'iode_type'.\nThe value must be in range [0, " + 
                                    std::to_string(VARIABLES) + "]");
        break;
    }
}

// Load WS

int ws_load(const std::string& filename, const int iode_type)
{
    int iode_type_ = iode_type;

    if(iode_type_ < 0)
        iode_type_ = (int) get_iode_file_type(filename);

    switch (iode_type_)
    {
    case COMMENTS:
        Comments.load(filename);
        return Comments.count();
        break;
    case EQUATIONS:
        Equations.load(filename);
        return Equations.count();
        break;
    case IDENTITIES:
        Identities.load(filename);
        return Identities.count();
        break;
    case LISTS:
        Lists.load(filename);
        return Lists.count();
        break;
    case SCALARS:
        Scalars.load(filename);
        return Scalars.count();
        break;
    case TABLES:
        Tables.load(filename);
        return Tables.count();
        break;
    case VARIABLES:
        Variables.load(filename);
        return Variables.count();
        break;
    default:
        throw std::invalid_argument("Invalid value " + std::to_string(iode_type) + " for the argument " +
                                    "'iode_type'.\nThe value must be in range [0, " + 
                                    std::to_string(VARIABLES) + "]");
        break;
    }
    return -1;
}

// Save WS

void ws_save(const std::string& filename, const int iode_type)
{
    switch (iode_type)
    {
    case COMMENTS:
        Comments.save(filename);
        break;
    case EQUATIONS:
        Equations.save(filename);
        break;
    case IDENTITIES:
        Identities.save(filename);
        break;
    case LISTS:
        Lists.save(filename);
        break;
    case SCALARS:
        Scalars.save(filename);
        break;
    case TABLES:
        Tables.save(filename);
        break;
    case VARIABLES:
        Variables.save(filename);
        break;    
    default:
        throw std::invalid_argument("Invalid value " + std::to_string(iode_type) + " for the argument " +
                                    "'iode_type'.\nThe value must be in range [0, " + 
                                    std::to_string(VARIABLES) + "]");
        break;
    }
}


// High to Low

void ws_htol(const std::string& filename, const std::string& varlist, const int series_type)
{
    if(series_type < 0 || series_type > IodeHighToLow::HTOL_SUM)
        throw std::invalid_argument("The value " + std::to_string(series_type) + " is invalid.\n" + 
                                    "The value must be in range [0, " + std::to_string(IodeHighToLow::HTOL_SUM) + "]");

    high_to_low((IodeHighToLow) series_type, filename, varlist);
}

void ws_htol(const std::string& filename, const std::vector<std::string>& varlist, const int series_type)
{        
    std::string s_varlist = join(varlist, " ");
    ws_htol(filename, s_varlist, series_type);
}


// Low to High

void ws_ltoh(const std::string& filename, const std::string& varlist, const int series_type, const char method)
{
    if(series_type < 0 || series_type > IodeLowToHigh::LTOH_FLOW)
    throw std::invalid_argument("The value " + std::to_string(series_type) + " is invalid.\n" + 
                                "The value must be in range [0, " + std::to_string(IodeLowToHigh::LTOH_FLOW) + "]");

    low_to_high((IodeLowToHigh) series_type, method, filename, varlist);
}

void ws_ltoh(const std::string& filename, const std::vector<std::string>& varlist, const int series_type, const char method)
{
    std::string s_varlist = join(varlist, " ");
    ws_ltoh(filename, s_varlist, series_type, method);
}
