#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/pair.h>

namespace nb = nanobind;

#include "cpp_api/iode_cpp_api.h"
#include "py_ws.h"
#include "py_sample.h"


NB_MODULE(iode, m) 
{
      std::string doc;

      // IODE defines used in python functions
      // -------------------------------------

      // Object and file types
      m.attr("I_CMT") = (int) I_COMMENTS;
      m.attr("I_EQS") = (int) I_EQUATIONS;
      m.attr("I_IDT") = (int) I_IDENTITIES;
      m.attr("I_LST") = (int) I_LISTS;
      m.attr("I_SCL") = (int) I_SCALARS;
      m.attr("I_TBL") = (int) I_TABLES;
      m.attr("I_VAR") = (int) I_VARIABLES;

      // Simulation parameters
      m.attr("SORT_CONNEX") = (int) SORT_CONNEX;
      m.attr("SORT_BOTH")   = (int) SORT_BOTH;
      m.attr("SORT_NONE")   = (int) SORT_NONE;

      m.attr("IV_INIT_TM1")      = (int) IV_INIT_TM1;
      m.attr("IV_INIT_TM1_A")    = (int) IV_INIT_TM1_A;
      m.attr("IV_INIT_EXTRA")    = (int) IV_INIT_EXTRA;
      m.attr("IV_INIT_EXTRA_A")  = (int) IV_INIT_EXTRA_A;
      m.attr("IV_INIT_ASIS")     = (int) IV_INIT_ASIS;
      m.attr("IV_INIT_TM1_NA")   = (int) IV_INIT_TM1_NA;
      m.attr("IV_INIT_EXTRA_NA") = (int) IV_INIT_EXTRA_NA;

      // Print outputs
      m.attr("W_GDI")   = (int) W_GDI; 
      m.attr("W_A2M")   = (int) W_A2M;
      m.attr("W_MIF")   = (int) W_MIF;
      m.attr("W_HTML")  = (int) W_HTML;
      m.attr("W_RTF")   = (int) W_RTF;
      m.attr("W_CSV")   = (int) W_CSV;
      m.attr("W_DUMMY") = (int) W_DUMMY;

      // HTOL methods
      m.attr("HTOL_LAST") = (int) HTOL_LAST;
      m.attr("HTOL_MEAN") = (int) HTOL_MEAN;
      m.attr("HTOL_SUM")  = (int) HTOL_SUM;

      // LTOH defines 
      m.attr("LTOH_STOCK")  = (int) LTOH_STOCK;
      m.attr("LTOH_FLOW")   = (int) LTOH_FLOW;
      m.attr("LTOH_LIN")    = WS_LTOH_LIN; 
      m.attr("LTOH_CS")     = WS_LTOH_CS;
      m.attr("LTOH_STEP")   = WS_LTOH_STEP;

      // Workspace functions

      m.def("ws_content", &ws_content, nb::arg("pattern"), nb::arg("iode_type") = (int) I_VARIABLES, 
            "Return the names of objects of a given IODE data type, satisfying a pattern specification.");
      m.def("ws_content_cmt", [](const std::string& pattern){ return ws_content(pattern, I_COMMENTS); }, nb::arg("pattern"),
            "Return the list of comment names corresponding to the given pattern");
      m.def("ws_content_eqs", [](const std::string& pattern){ return ws_content(pattern, I_EQUATIONS); }, nb::arg("pattern"),
            "Return the list of equation names corresponding to the given pattern");
      m.def("ws_content_idt", [](const std::string& pattern){ return ws_content(pattern, I_IDENTITIES); }, nb::arg("pattern"),
            "Return the list of identity names corresponding to the given pattern");
      m.def("ws_content_lst", [](const std::string& pattern){ return ws_content(pattern, I_LISTS); }, nb::arg("pattern"),
            "Return the list of list names corresponding to the given pattern");
      m.def("ws_content_scl", [](const std::string& pattern){ return ws_content(pattern, I_SCALARS); }, nb::arg("pattern"),
            "Return the list of scalar names corresponding to the given pattern");
      m.def("ws_content_tbl", [](const std::string& pattern){ return ws_content(pattern, I_TABLES); }, nb::arg("pattern"),
            "Return the list of table names corresponding to the given pattern");
      m.def("ws_content_var", [](const std::string& pattern){ return ws_content(pattern, I_VARIABLES); }, nb::arg("pattern"),
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

      // IODE objects getters and setters

      m.def("get_cmt", [](const std::string& name) { return Comments.get(name); }, nb::arg("name"),          "get an IODE comment");
      // m.def("get_eqs", [](const std::string& name) { return Equations.get(name); }, nb::arg("name"),      "get an IODE equation");
      m.def("get_eqs_lec", [](const std::string& name) { return Equations.get_lec(name); }, nb::arg("name"), "get LEC of an equation");
      m.def("get_idt", [](const std::string& name) { return Identities.get_lec(name); }, nb::arg("name"),        "get an IODE identity");
      m.def("get_lst", [](const std::string& name) { return Lists.get(name); }, nb::arg("name"),             "get an IODE list");
      // m.def("get_scl", [](const std::string& name) { return Scalars.get(name); }, nb::arg("name"),        "get an IODE scalar");
      // m.def("get_tbl", [](const std::string& name) { return Tables.get(name); }, nb::arg("name"),         "get an IODE table");
      m.def("get_var", [](const std::string& name) { return Variables.get(name); }, nb::arg("name"),         "get an IODE variable");

      // LEC
      m.def("exec_lec", nb::overload_cast<const std::string&, const int>(&execute_lec), nb::arg("lec"), nb::arg("t"));
      m.def("exec_lec", nb::overload_cast<const std::string&, const std::string&>(&execute_lec), nb::arg("lec"), nb::arg("period"));
      m.def("exec_lec", nb::overload_cast<const std::string&>(&execute_lec), nb::arg("lec"));

      // High To Low functions

      m.def("ws_htol", nb::overload_cast<const std::string&, const std::string&, const int>(&ws_htol), 
            nb::arg("filename"), nb::arg("varlist"), nb::arg("series_type"));
      m.def("ws_htol_last", [](const std::string& filename, const std::string& varlist) { ws_htol(filename, varlist, (int) EnumIodeHtoL::HTOL_LAST); }, 
            nb::arg("filename"), nb::arg("varlist"));
      m.def("ws_htol_mean", [](const std::string& filename, const std::string& varlist) { ws_htol(filename, varlist, (int) EnumIodeHtoL::HTOL_MEAN); }, 
            nb::arg("filename"), nb::arg("varlist"));
      m.def("ws_htol_sum",  [](const std::string& filename, const std::string& varlist) { ws_htol(filename, varlist, (int) EnumIodeHtoL::HTOL_SUM); }, 
            nb::arg("filename"), nb::arg("varlist"));

      m.def("ws_htol", nb::overload_cast<const std::string&, const std::vector<std::string>&, const int>(&ws_htol), 
            nb::arg("filename"), nb::arg("varlist"), nb::arg("series_type"));
      m.def("ws_htol_last", [](const std::string& filename, const std::vector<std::string>& varlist) { ws_htol(filename, varlist, (int) EnumIodeHtoL::HTOL_LAST); }, 
            nb::arg("filename"), nb::arg("varlist"));
      m.def("ws_htol_mean", [](const std::string& filename, const std::vector<std::string>& varlist) { ws_htol(filename, varlist, (int) EnumIodeHtoL::HTOL_MEAN); }, 
            nb::arg("filename"), nb::arg("varlist"));
      m.def("ws_htol_sum",  [](const std::string& filename, const std::vector<std::string>& varlist) { ws_htol(filename, varlist, (int) EnumIodeHtoL::HTOL_SUM); }, 
            nb::arg("filename"), nb::arg("varlist"));

      // Low To High functions

      m.def("ws_ltoh", nb::overload_cast<const std::string&, const std::string&, const int, const char>(&ws_ltoh), 
            nb::arg("filename"), nb::arg("varlist"), nb::arg("series_type"), nb::arg("method"));
      m.def("ws_ltoh_flow",  [](const std::string& filename, const std::string& varlist, const char method = 'C') 
            { ws_ltoh(filename, varlist, EnumIodeLtoH::LTOH_FLOW, method); }, 
            nb::arg("filename"), nb::arg("varlist"), nb::arg("method") = 'C');
      m.def("ws_ltoh_stock", [](const std::string& filename, const std::string& varlist, const char method = 'C') 
            { ws_ltoh(filename, varlist, EnumIodeLtoH::LTOH_STOCK, method); }, 
            nb::arg("filename"), nb::arg("varlist"), nb::arg("method") = 'C');

      m.def("ws_ltoh", nb::overload_cast<const std::string&, const std::vector<std::string>&, const int, const char>(&ws_ltoh), 
            nb::arg("filename"), nb::arg("varlist"), nb::arg("series_type"), nb::arg("method"));
      m.def("ws_ltoh_flow",  [](const std::string& filename, const std::vector<std::string>& varlist, const char method = 'C') 
            { ws_ltoh(filename, varlist, EnumIodeLtoH::LTOH_FLOW, method); }, 
            nb::arg("filename"), nb::arg("varlist"), nb::arg("method") = 'C');
      m.def("ws_ltoh_stock", [](const std::string& filename, const std::vector<std::string>& varlist, const char method = 'C') 
            { ws_ltoh(filename, varlist, EnumIodeLtoH::LTOH_STOCK, method); }, 
            nb::arg("filename"), nb::arg("varlist"), nb::arg("method") = 'C');

      // Sample

      m.def("ws_sample_set", &ws_sample_set, nb::arg("from") = "", nb::arg("to") = "", "Set a new IODE sample and return it");
      m.def("ws_sample_get", &ws_sample_get, 
            "Return the current sample lower and upper bounds, e.g.: ('2000Y1', '2010Y1') or ('', '') if the sample is undefined.");
      m.def("ws_sample_nb_periods", []() { return Variables.get_nb_periods(); }, 
            "Return the number of periods in the current sample");
      m.def("ws_sample_to_string", &ws_sample_to_string, 
            "Return the current sample definition in a string: 'from to', e.g.: '2000Y1 2020Y1' or '' if the sample is undefined.");
      m.def("ws_sample_to_list", [](const std::string& from = "", const std::string& to = "") { return Variables.get_list_periods(from, to); }, 
            nb::arg("from") = "", nb::arg("to") = "",
            "Return the current sample definition as a list of periods.\ne.g.: ['2000Y1', '2001Y1', ..., '2010Y1'] or [] if the sample is undefined");
      doc = "Return the current sample definition as a list of periods converted to float.\n";
      doc += "e.g.: [2000.0, 2001.0, ..., 2010.0] or [] if the sample is undefined";
      m.def("ws_sample_to_float_list", [](const std::string& from = "", const std::string& to = "") { return Variables.get_list_periods_as_float(from, to); }, 
            nb::arg("from") = "", nb::arg("to") = "", doc.c_str());

      // Report

      m.def("report_exec", nb::overload_cast<const std::string&, const std::string&>(&execute_report), 
            nb::arg("filepath"), nb::arg("parameters") = "");
      m.def("report_exec", nb::overload_cast<const std::string&, const std::vector<std::string>&>(&execute_report),
            nb::arg("filepath"), nb::arg("parameters") = std::vector<std::string>());
      m.def("reportline_exec", nb::overload_cast<const std::string&>(&execute_report_line), 
            nb::arg("commands"));
      m.def("reportline_exec", nb::overload_cast<const std::vector<std::string>&>(&execute_report_line), 
            nb::arg("commands"));
}
