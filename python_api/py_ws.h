#pragma once
#include <string>
#include <vector>
#include <exception>

#include "cpp_api/iode_cpp_api.h"

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

namespace nb = nanobind;


void init_ws(nb::module_ &m);


/*
 *  TODO
 *  ----
 *   ws_copy(filename, filetype, list)
 *   ws_copy_cmt(filename, list)
 *   ws_copy_eqs(filename, list)
 *   ws_copy_idt(filename, list)
 *   ws_copy_lst(filename, list)
 *   ws_copy_scl(filename, list)
 *   ws_copy_tbl(filename, list)
 *   ws_copy_var(filename, list, sample) *
 *   ws_merge(filename, filetype, list)
 *   ws_merge_cmt(filename, list)
 *   ws_merge_eqs(filename, list)
 *   ws_merge_idt(filename, list)
 *   ws_merge_lst(filename, list)
 *   ws_merge_scl(filename, list)
 *   ws_merge_tbl(filename, list)
 *   ws_merge_var(filename, list)
 *
 *   ws_extrapolate(...)
 *
 *   ws_extrapolate
 *   ws_seasonadj
 *   ws_seasadj
 *   ws_trend
 *   ws_trendstd
 *   ws_import
 *   ws_export
 */

// TODO: rewrite IodeContents with regex or a better algorithm because K_grep() is VERY slow for large databases

/*
    r"""Return the names of objects of a given type, satisfying a pattern specification.

    Parameters
    ----------
    pattern: string
        string containing wildcards characters like '*' or '?'.
        Default '*', meaning "all objects".

    objtype: int
        IODE object type (0-6, 0 for comments...)
        Default 6 for

    Returns
    -------
    List[str]
        List of object names

    Examples
    -------
    >>> iode.ws_load_cmt("../fun.cmt")
    >>> result = iode.ws_content("ACA*", 0)
    >>> print(result)
    ['ACAF', 'ACAG']
    """
*/
std::vector<std::string> ws_content(const std::string& pattern = "*", const int iode_type = VARIABLES);

// Clear WS

void ws_clear_all();
void ws_clear(const int iode_type);

// Load WS

int ws_load(const std::string& filename, const int iode_type = -1);

// Save WS

void ws_save(const std::string& filename, const int iode_type);

// High to Low

void ws_htol(const std::string& filename, const std::string& varlist, const int series_type);
void ws_htol(const std::string& filename, const std::vector<std::string>& varlist, const int series_type);

// Low to High

void ws_ltoh(const std::string& filename, const std::string& varlist, const int series_type, const char method);
void ws_ltoh(const std::string& filename, const std::vector<std::string>& varlist, const int series_type, const char method);
