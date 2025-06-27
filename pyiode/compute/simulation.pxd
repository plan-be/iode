# distutils: language = c++

from collections.abc import Iterable
from typing import Union, Tuple, List, Dict, Optional

# Import necessary C++ types/classes
from libcpp.string cimport string
from libcpp.vector cimport vector

from pyiode.common cimport SimuSortAlgorithm, VariablesInitialization


cdef extern from "api/all.h":
    int RPF_SimNIterInt(unsigned char** args)
    double RPF_SimNormReal(unsigned char** args)
    int B_ModelSimulateSaveNIters(char *arg)
    int B_ModelSimulateSaveNorms(char *arg)

cdef extern from "cpp_api/compute/simulation.h":
    cdef cppclass CSimulation "Simulation":
        CSimulation() except +

        # Getter and Setter for convergence threshold
        double get_convergence_threshold()
        void set_convergence_threshold(double threshold) except +

        # Getter and Setter for relax
        double get_relax()
        void set_relax(double relax) except +

        # Getter and Setter for max number of iterations
        int get_max_nb_iterations()
        void set_max_nb_iterations(int nb_iterations) except +

        # Getter and Setter for max number of iterations for Newton's method
        int get_max_nb_iterations_newton()
        void set_max_nb_iterations_newton(int nb_iterations) except +

        # Getter and Setter for sort algorithm
        SimuSortAlgorithm get_sort_algorithm()
        string get_sort_algorithm_as_string()
        void set_sort(SimuSortAlgorithm sort_algorithm) except +

        # Getter and Setter for initialization method
        VariablesInitialization get_initialization_method()
        string get_initialization_method_as_string()
        void set_initialization_method(VariablesInitialization method) except +

        # Getter and Setter for debug mode
        bint is_debug_active()
        void set_debug(bint debug) except +

        # Getter and Setter for debug mode for Newton's method
        bint is_debug_newton_active()
        void set_debug_newton(bint debug) except +

        # Getter and Setter for number of passes for the heuristic triangulation algorithm
        int get_nb_passes()
        void set_nb_passes(int nb_passes) except +

        # Model simulation methods
        void model_exchange(const string& list_exo) except +
        void model_compile(const string& list_eqs) except +
        void model_simulate(const string& from_period, const string& to_period, const string& list_eqs) except +
        void model_calculate_SCC(const int nb_iterations, const string& pre_name, const string& inter_name, 
                                 const string& post_name, const string& list_eqs) except +
        void model_simulate_SCC(const string& from_period, const string& to_period, const string& pre_name, 
                                const string& inter_name, const string& post_name) except +
