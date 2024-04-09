# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

from pyiode.common cimport EnumIodeType, EnumIodeVarMode, EnumSimulationInitialization
from pyiode.period cimport CPeriod
from pyiode.sample cimport CSample


# C++ classes
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html 

# WARNING: - to declare functions/methods with default values, you must use =* in a pxd file
#            https://cython.readthedocs.io/en/latest/src/userguide/language_basics.html#optional-arguments 
#          - combination of base class and renaming is not possible. You cannot declare derived C++ class as:
#               cdef cppclass Derived(Base) "DerivedRealName": (...)
#            The solution is to cimport from pxd file using the keyword as:
#               from derived cimport DerivedRealName as Derived   
#            https://stackoverflow.com/a/23455514


cdef extern from "cpp_api/KDB/kdb_abstract.h":
    cdef cppclass KDBAbstract:    
        # Public methods
        int get_iode_type() const
        bool is_global_database() const
        bool is_local_database() const
        bool is_shallow_copy_database() const
        int count() const

        string get_filename() const

        string get_description() const
        void set_description(string& description) except +

        vector[string] get_names(string& pattern, bool must_exist) except +
        int rename(string& old_name, string& new_name) except +
        bool contains(string& name) except +
        void remove(string& name) except +

        void merge(KDBAbstract& other, bool overwrite) except +
        void copy_into(string& input_file, string& objects_names) except +
        void merge_into(string& input_file) except +

        vector[string] get_associated_objects_list(string& name, EnumIodeType other_type) except +

        void save(string& filepath) except +
        void clear() except +

cdef extern from "cpp_api/KDB/kdb_template.h":
    cdef cppclass KDBTemplate[T](KDBAbstract):        
        pass

cdef extern from "cpp_api/KDB/kdb_comments.h":
    cdef cppclass KDBComments(KDBTemplate[string]):
        # Constructor
        KDBComments(string& filepath) except +

        # Public methods
        KDBComments* subset(string& pattern, bool deep_copy) except +
        string get(string& name) except +
        string copy(string& name) except +
        int add(string& name, string& comment) except +
        void update(string& name, string& comment) except +

    size_t hash_value(KDBComments&) except +

    # Define the global Comments instance
    cdef KDBComments Comments


cdef extern from "cpp_api/KDB/kdb_identities.h":
    cdef cppclass KDBIdentities(KDBTemplate[string]):
        # Constructor
        KDBIdentities(string& filepath) except +

        # Public methods
        KDBIdentities* subset(string& pattern, bool deep_copy) except +
        string get_lec(string& name) except +
        string copy(string& name) except +
        int add(string& name, string& identity_lec) except +
        void update(string& name, string& identity_lec) except +
        void execute_identities(string& from_, string& to, string& identities_list, 
                                string& var_files, string& scalar_files, bint trace) except +

    size_t hash_value(KDBIdentities&) except +

    # Define the global Identities instance
    cdef KDBIdentities Identities


cdef extern from "cpp_api/KDB/kdb_lists.h":
    cdef cppclass KDBLists(KDBTemplate[string]):
        # Constructor
        KDBLists(string& filepath) except +

        # Public methods
        KDBLists* subset(string& pattern, bool deep_copy) except +
        string get(string& name) except +
        string copy(string& name) except +
        int add(string& name, string& list_) except +
        void update(string& name, string& list_) except +

    size_t hash_value(KDBLists&) except +

    # Define the global Lists instance
    cdef KDBLists Lists


cdef extern from "cpp_api/KDB/kdb_variables.h":
    cdef cppclass KDBVariables(KDBTemplate[vector[double]]):
        # Constructor
        KDBVariables(string& filepath) except +

        # Public methods
        KDBVariables* subset(string& pattern, bool deep_copy) except +
        vector[double] get(string& name) except +
        vector[double] copy(string& name) except +
        int add(string& name, vector[double]& values) except +
        int add(string& name, string& lec) except +
        void update(string& name, vector[double]& values) except +
        void update(string& name, vector[double]& values, string& first_period, string& last_period) except +
        void update(string& name, vector[double]& values, int t_first, int t_last) except +
        void update(string& name, string& lec) except +
        void update(string& name, string& lec, string& first_period, string& last_period) except +
        void update(string& name, string& lec, int t_first, int t_last) except +

        double get_var(int pos, int t, EnumIodeVarMode mode) except +
        double get_var(string& name, int t, EnumIodeVarMode mode) except +
        double get_var(string& name, string& period, EnumIodeVarMode mode) except +
        double* get_var_ptr(string& name) except +

        void set_var(int pos, int t, double value, EnumIodeVarMode mode) except +
        void set_var(string& name, int t, double value, EnumIodeVarMode mode) except +
        void set_var(string& name, string& period, double value, EnumIodeVarMode mode) except +

        CSample get_sample()
        void set_sample(string& from_, string& to) except +

        int get_nb_periods()
        string get_period(int t) except +
        float get_period_as_float(int t) except +
        vector[string] get_list_periods(string& from_, string& to) except +
        vector[float] get_list_periods_as_float(string& from_, string& to) except +

        void extrapolate(EnumSimulationInitialization method, string& from_, string& to, string& variables_list) except +
        void seasonal_adjustment(string& input_file, string& series, double eps_test) except +
        void trend_correction(string& input_file, double lambda_, string& series, bint log) except +

    size_t hash_value(KDBVariables&) except +

    # Define the global Variables instance
    cdef KDBVariables Variables

