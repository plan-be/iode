# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

from pyiode.common cimport EnumIodeType


# C++ classes
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html 

# WARNING: - to declare functions/methods with default values, you must use =* in a pxd file
#            https://cython.readthedocs.io/en/latest/src/userguide/language_basics.html#optional-arguments 
#          - combination of base class and renaming is not possible. You cannot declare derived C++ class as:
#               cdef cppclass Derived(Base) "DerivedRealName": (...)
#            The solution is to cimport from pxd file using the keyword as:
#               from derived cimport DerivedRealName as Derived   
#            https://stackoverflow.com/a/23455514


cdef extern from "KDB/kdb_abstract.h":
    cdef cppclass KDBAbstract:    
        # Public methods
        int get_iode_type() const
        bool is_global_database() const
        bool is_local_database() const
        bool is_shallow_copy_database() const
        int count() const

        string get_filename() const

        string get_description() const
        void set_description(const string& description) except +

        vector[string] get_names(const string& pattern, bool must_exist) except +
        int rename(const string& old_name, const string& new_name) except +
        bool contains(const string& name) except +
        void remove(const string& name) except +

        void merge(const KDBAbstract& other, bool overwrite) except +
        void copy_into(const string& input_file, const string& objects_names) except +
        void merge_into(const string& input_file) except +

        vector[string] get_associated_objects_list(const string& name, EnumIodeType other_type) except +

        void save(const string& filepath) except +
        void clear() except +

cdef extern from "KDB/kdb_template.h":
    cdef cppclass KDBTemplate[T](KDBAbstract):        
        pass

cdef extern from "KDB/kdb_comments.h":
    cdef cppclass KDBComments(KDBTemplate[string]):
        # Constructor
        KDBComments(const string& filepath) except +

        # Public methods
        KDBComments* subset(const string& pattern, const bool deep_copy) except +
        string get(const string& name) except +
        string copy(const string& name) except +
        int add(const string& name, const string& comment) except +
        void update(const string& name, const string& comment) except +

    size_t hash_value(KDBComments&) except +

    # Define the global Comments instance
    cdef KDBComments Comments

