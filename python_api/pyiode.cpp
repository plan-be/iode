#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/pair.h>

namespace nb = nanobind;

#include "cpp_api/iode_cpp_api.h"

#include "py_numpy.h"
#include "py_constants.h"
#include "py_sample.h"
#include "py_ws.h"
#include "py_objs.h"


// To see how to split build of a large Python module, read the link below:
// https://nanobind.readthedocs.io/en/latest/faq.html#how-can-i-reduce-build-time 

NB_MODULE(iode_python_api, m) 
{
      init_constants(m);
      init_sample(m);
      init_ws(m);
      init_iode_objs(m);

      // LEC
      m.def("exec_lec", nb::overload_cast<const std::string&, const int>(&execute_lec), nb::arg("lec"), nb::arg("t"));
      m.def("exec_lec", nb::overload_cast<const std::string&, const std::string&>(&execute_lec), nb::arg("lec"), nb::arg("period"));
      m.def("exec_lec", nb::overload_cast<const std::string&>(&execute_lec), nb::arg("lec"));

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
      std::string doc = "Estimate an equation or a block of equations on the given sample.\n";
      doc += "The estimation method and the instruments must be specified in the equation before the estimation.";
      m.def("eqs_estimate", nb::overload_cast<const std::string&, const std::string&, const std::string&>(&eqs_estimate), 
            nb::arg("eqs"), nb::arg("from") = "", nb::arg("to") = "", doc.c_str());
      m.def("eqs_estimate", nb::overload_cast<const std::vector<std::string>&, const std::string&, const std::string&>(&eqs_estimate), 
            nb::arg("eqs"), nb::arg("from") = "", nb::arg("to") = "", doc.c_str());

}
