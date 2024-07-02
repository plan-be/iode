# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp.pair cimport pair
from libcpp cimport bool

from pyiode.common cimport IodeEquationMethod, IodeEquationTest
from pyiode.time.sample cimport CSample


cdef extern from "cpp_api/objects/equation.h":

    # declare C++ Equation class
    # see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
    cdef cppclass CEquation "Equation":
        float tests[20]
        long date

        CEquation(const string&, const string&, const IodeEquationMethod, const string&, const string&, 
                  const string&, const string&, const string&, const bool date) except +
        CEquation(const CEquation& other) except +

        string get_lec()
        void set_lec(const string&, const string&) except +

        int get_method_as_int()
        string get_method()
        void set_method(const IodeEquationMethod) except +

        string get_block()
        void set_block(const string&) except +

        CSample get_sample()
        void set_sample(string, string) except +

        string get_comment()
        void set_comment(const string&) except +

        string get_instruments()
        void set_instruments(const string&) except +

        long get_date()
        string get_date_as_string(const string&)
        void set_date(const long date) except +
        void set_date(string&, string&) except +
        void update_date()
        void reset_date()

        map[string, float] get_tests_as_map()
        void set_test(const IodeEquationTest i, const float value) except +

        vector[string] get_coefficients_list(const bool create_if_not_exit) except +
        vector[string] get_variables_list(const bool create_if_not_exit) except +
        pair[string, string] split_equation() except +

        bool operator==(const CEquation&) except +

        string to_string()

    size_t hash_value(CEquation&)

    # declare C++ NamedEquation struct
    cdef cppclass NamedEquation:
        # attributes
        string name
        CEquation eq

        # constructors
        NamedEquation(const string& name)
        NamedEquation(const string& name, const CEquation& eq)
