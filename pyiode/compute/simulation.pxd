# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

# Import necessary C++ types/classes
from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.memory cimport shared_ptr

from pyiode.common cimport SimuSortAlgorithm, VariablesInitialization


cdef extern from "api/all.h":
    int RPF_SimNIterInt(unsigned char** args)
    double RPF_SimNormReal(unsigned char** args)
    int B_ModelSimulateSaveNIters(char *arg)
    int B_ModelSimulateSaveNorms(char *arg)

cdef extern from "api/simulation/simulation.h":
    cdef cppclass CSimulation:
        double  epsilon
        double  relax
        int     max_iter
        int     nb_passes
        double  newton_epsilon
        double  newton_step
        int     newton_max_iter

        # factory method
        shared_ptr[CSimulation] Create() except +

        # utility methods
        void reset() except +

        # Getter and Setter for sort algorithm
        SimuSortAlgorithm get_sort_algorithm()
        string get_sort_algorithm_as_string()
        void set_sort(SimuSortAlgorithm sort_algorithm) except +

        # Getter and Setter for initialization method
        VariablesInitialization get_initialization_method()
        string get_initialization_method_as_string()
        void set_initialization_method(VariablesInitialization method) except +

        # Getter and Setter for debug mode
        bool is_debug_active()
        void set_debug(bint debug) except +

        # Getter and Setter for debug mode for Newton's method
        bool is_debug_newton_active()
        void set_debug_newton(bint debug) except +

        # Model simulation methods
        bool exchange(const string& list_exo) except +
        bool compile(const string& list_eqs) except +
        bool simulate(const string& from_period, const string& to_period, const string& list_eqs) except +
        bool calculate_SCC(const int nb_iterations, const string& pre_name, const string& inter_name, 
                                 const string& post_name, const string& list_eqs) except +
        bool simulate_SCC(const string& from_period, const string& to_period, const string& pre_name, 
                                const string& inter_name, const string& post_name) except +

    # Define the global Comments instance
    ctypedef shared_ptr[CSimulation] SimulationPtr
    cdef SimulationPtr global_simu
