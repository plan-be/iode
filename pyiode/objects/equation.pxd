# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp cimport bool

from pyiode.pyiode_sample cimport CSample


cdef extern from "objects/equation.h":

    cdef int EQS_NBTESTS

    cdef enum EnumIodeEquationTest:
        IE_CORR,
        IE_STDEV,
        IE_MEANY, 
        IE_SSRES, 
        IE_STDERR,
        IE_STDERRP,
        IE_FSTAT, 
        IE_R2,
        IE_R2ADJ, 
        IE_DW,
        IE_LOGLIK

    # declare C++ Equation class
    # see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html#declaring-a-c-class-interface 
    cdef cppclass CEquation "Equation":
        float tests[20]

        CEquation(const string&, const string&, const int, const string&, const string&, 
                  const string&, const string&, const string&, const bool date) except +

        string get_lec()
        void set_lec(const string&, const string&) except +

        int get_method_as_int()
        string get_method()
        void set_method(const int) except +
        void set_method(const string&) except +

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

        # Note: Cython does not support std::array
        #       We then cannot use get_tests() and set_tests()
        float get_test_stdev() 
        float get_test_meany() 
        float get_test_ssres() 
        float get_test_stderr() 
        float get_test_fstat() 
        float get_test_r2() 
        float get_test_r2adj() 
        float get_test_dw() 
        float get_test_loglik() 

        void set_test(const EnumIodeEquationTest, const float) except +

        vector[string] get_coefficients_list(const bool create_if_not_exit) except +
        vector[string] get_variables_list(const bool create_if_not_exit) except +
        pair[string, string] split_equation() except +

        bool operator==(const CEquation&) except +

        string to_string()

    size_t hash_value(CEquation&)
