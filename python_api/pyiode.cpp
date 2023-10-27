#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/pair.h>

namespace nb = nanobind;

#include "cpp_api/iode_cpp_api.h"

#include "py_constants.h"
#include "py_numpy.h"
#include "py_ws.h"
#include "py_sample.h"


// To see how to split build of a large Python module, read the link below:
// https://nanobind.readthedocs.io/en/latest/faq.html#how-can-i-reduce-build-time 

NB_MODULE(iode, m) 
{
      std::string doc;

      init_constants(m);

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

      // IODE object classes

      // Read https://nanobind.readthedocs.io/en/latest/classes.html#classes 
      // to see how to export C++ classes to Python

      nb::class_<Scalar>(m, "Scalar")
            .def_rw("value", &Scalar::val)
            .def_rw("relax", &Scalar::relax)
            .def_ro("std", &Scalar::std)
            .def(nb::init<const std::string&>())
            .def(nb::init<const IODE_REAL, const IODE_REAL, const IODE_REAL>())
            .def(nb::self == nb::self)
            .def("__repr__", &Scalar::to_string);

      // IODE objects getters

      m.def("get_cmt", [](const std::string& name) { return Comments.get(name); }, nb::arg("name"), "Get an IODE comment");
      // m.def("get_eqs", [](const std::string& name) { return Equations.get(name); }, nb::arg("name"), "Get an IODE equation");
      m.def("get_idt", [](const std::string& name) { return Identities.get_lec(name); }, nb::arg("name"),"Get an IODE identity");
      m.def("get_lst", [](const std::string& name) { return Lists.get(name); }, nb::arg("name"), "Get an IODE list");
      m.def("get_scl", [](const std::string& name) { return Scalars.get(name); }, nb::arg("name"), "Get an IODE scalar");
      // m.def("get_tbl", [](const std::string& name) { return Tables.get(name); }, nb::arg("name"),   "Get an IODE table");
      m.def("get_var", [](const std::string& name) { return Variables.get(name); }, nb::arg("name"), "Get an IODE variable");
      
      // IODE objects setters

      m.def("set_cmt", [](const std::string& name, const Comment& cmt) { 
                  if(!Comments.contains(name)) Comments.add(name, cmt); 
                  else Comments.update(name, cmt); 
            }, nb::arg("name"), nb::arg("cmt"), "Add or create an IODE comment");
      // m.def("set_eqs", [](const std::string& name, const Equation& eq) { 
      //            if(!Equations.contains(name)) Equations.add(name, eq); 
      //            else Equations.update(name, eq); 
      //      }, nb::arg("name"), nb::arg("eq"), "Get an IODE equation");
      m.def("set_idt", [](const std::string& name, const std::string& idt) { 
                  if(!Identities.contains(name)) Identities.add(name, idt); 
                  else Identities.update(name, idt); 
            }, nb::arg("name"), nb::arg("idt"), "Add or create an IODE identity");
      m.def("set_lst", [](const std::string& name, const List& lst) { 
                  if(!Lists.contains(name)) Lists.add(name, lst); 
                  else Lists.update(name, lst); 
            }, nb::arg("name"), nb::arg("lst"), "Add or create an IODE list");
      m.def("set_scl", [](const std::string& name, const Scalar& scl) { 
                  if(!Scalars.contains(name)) Scalars.add(name, scl); 
                  else Scalars.update(name, scl.val, scl.relax, scl.std); 
            }, nb::arg("name"), nb::arg("scl"), "Add or create an IODE scalar");
      // m.def("set_tbl", [](const std::string& name, const Table& tbl) { 
      //            if(!Tables.contains(name)) Tables.add(name, tbl); 
      //            else Tables.update(name, tbl); 
      //         }, nb::arg("name"), nb::arg("tbl"), "Add or create an IODE table");
      m.def("set_var", [](const std::string& name, const Variable& var) { 
                  if(!Variables.contains(name)) Variables.add(name, var); 
                  else Variables.update(name, var);  
            }, nb::arg("name"), nb::arg("var"), "Add or create an IODE variable");

      // Equations

      m.def("get_eqs_lec", [](const std::string& name) { return Equations.get_lec(name); }, nb::arg("name"),  "Get LEC of an equation");
      m.def("set_eqs_lec", [](const std::string& name) { return Equations.get_lec(name); }, nb::arg("name"),  "Update LEC of an equation");

      // Variables

      m.def("get_var_as_ndarray", &get_var_as_ndarray, nb::rv_policy::reference, nb::arg("name"),     "Get an IODE variable in a numpy ndarray");
      m.def("get_var_as_ndarray_copy", &get_var_as_ndarray, nb::rv_policy::copy, nb::arg("name"),     "Get a copy of an IODE variable in a numpy ndarray");

      // IODE objects delete

      m.def("delete_cmt", [](const std::string& name) { Comments.remove(name); },   nb::arg("name"), "delete the comment named `name`");
      m.def("delete_eqs", [](const std::string& name) { Equations.remove(name); },  nb::arg("name"), "delete the equation named `name`");
      m.def("delete_idt", [](const std::string& name) { Identities.remove(name); }, nb::arg("name"), "delete the identity named `name`");
      m.def("delete_lst", [](const std::string& name) { Lists.remove(name); },      nb::arg("name"), "delete the list named `name`");
      m.def("delete_scl", [](const std::string& name) { Scalars.remove(name); },    nb::arg("name"), "delete the scalar named `name`");
      m.def("delete_tbl", [](const std::string& name) { Tables.remove(name); },     nb::arg("name"), "delete the table named `name`");
      m.def("delete_var", [](const std::string& name) { Variables.remove(name); },  nb::arg("name"), "delete the variable named `name`");

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

      // Estimation
      doc = "Estimate an equation or a block of equations on the given sample.\n";
      doc += "The estimation method and the instruments must be specified in the equation before the estimation.";
      m.def("eqs_estimate", nb::overload_cast<const std::string&, const std::string&, const std::string&>(&eqs_estimate), 
            nb::arg("eqs"), nb::arg("from") = "", nb::arg("to") = "", doc.c_str());
      m.def("eqs_estimate", nb::overload_cast<const std::vector<std::string>&, const std::string&, const std::string&>(&eqs_estimate), 
            nb::arg("eqs"), nb::arg("from") = "", nb::arg("to") = "", doc.c_str());

}
