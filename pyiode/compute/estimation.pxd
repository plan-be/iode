# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

# Import necessary C++ types/classes
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.set cimport set

from pyiode.common cimport IodeEquationMethod, IodeAdjustmentMethod
from pyiode.time.sample cimport CSample
from pyiode.objects.equation cimport CEquation
from pyiode.iode_database.cpp_api_database cimport KDBEquations as KDBEquations
from pyiode.iode_database.cpp_api_database cimport KDBScalars as KDBScalars


cdef extern from "cpp_api/compute/estimation.h":
    string dynamic_adjustment(const IodeAdjustmentMethod method, const string& eqs, 
                              const string& c1, const string& c2) except +
    KDBScalars* dickey_fuller_test(const string& lec, bint drift, bint trend, int order) except +


    # Declaration of the CorrelationMatrix class
    cdef cppclass CCorrelationMatrix "CorrelationMatrix":
        # public member variables
        const int nb_coeffs

        # public methods
        string get_name(const int i)
        double get_value(const int row, const int col)


    # Declaration of the EditAndEstimateEquations class
    # For GUI only. Must by hidded to users.
    cdef cppclass CEditAndEstimateEquations "EditAndEstimateEquations":
        # Constructor
        CEditAndEstimateEquations(const string& from_period, const string& to_period) except +
        
        # getter and setter methods
        CSample* get_sample()
        void set_sample() except +
        void set_sample(const CSample* sample) except +
        void set_sample(const string& from_period, const string& to_period) except +
        
        string get_block()
        void set_block(const string& block, const string& current_eq_name) except +
        
        string get_method()
        void set_method(const IodeEquationMethod method) except +
        void set_method(const string& method) except +
        
        string get_instruments()
        void set_instruments(const string& instruments) except +

        # Public methods
        KDBScalars* get_scalars()
        void update_scalars() except +

        KDBEquations* get_equations()
        set[string] get_list_equations()
        void update_current_equation(const string& lec, const string& comment) except +

        CEquation* current_equation() except +
        CEquation* next_equation() except +

        CCorrelationMatrix* get_correlation_matrix()
        vector[double] get_observed_values(const string& name) except +
        vector[double] get_fitted_values(const string& name) except +
        vector[double] get_residual_values(const string& name) except +

        void estimate(int maxit, float epsilon) except +
        bint is_estimation_done()

        vector[string] save(const string& from_period, const string& to_period) except +

