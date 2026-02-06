# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.set cimport set
from libcpp.map cimport map
from libcpp.memory cimport unique_ptr

from libcpp cimport bool

from pyiode.common cimport (IodeType, IodeFileType, TableLang, IodeEquationMethod, 
                            IodeEquationTest, IodeLowToHigh, IodeHighToLow, 
                            TableCellType, TableCellAlign, TableCellFont, TableLineType, 
                            TableTextAlign, TableGraphAxis, TableGraphGrid, TableGraphType, 
                            IodeVarMode, VariablesInitialization)
from pyiode.time.period cimport CPeriod
from pyiode.time.sample cimport CSample
from pyiode.objects.equation cimport CEquation
from pyiode.objects.identity cimport CIdentity
from pyiode.objects.scalar cimport CScalar
from pyiode.objects.table cimport CTable


cdef extern from "api/all.h":
    ctypedef char OFNAME[64]
    ctypedef char FNAME[512]
    ctypedef char ONAME[21]

    int K_NBDEC
    int B_TABLE_TITLE
    int B_EQS_INFOS
    int B_EQS_LEC
    double K_COMPARE_EPS

    int B_PrintNbDec(char* nbdec)
    int B_PrintLang(char* lang)
    int B_PrintObjTblTitle(char* arg)
    int B_PrintObjEqsInfos(char* arg)
    int B_PrintObjLec(char* arg)

    int B_PrintObjDef(char* objs_list, int iode_type)
    int B_PrintDest(char* arg) # arg = 'filepath format'

    int W_dest(char* filename, int file_type)
    int W_flush()
    int W_close()

    int B_DataCompareEps(char* arg)
    int B_DataCompare(char* arg, int iode_type)
    int B_FileImportCmt(char* arg)
    int B_FileImportVar(char* arg)
    int EXP_RuleExport(char* trace, char* rule, char* out, char* vfile, char* cfile, 
                       char* from_period, char* to_period, char* na, char* sep, int fmt)
    int RasExecute(char* pattern, char* xdim, char* ydim, CPeriod* ref_period, 
                   CPeriod* sum_period, int maxit, double eps)
    int B_DataPattern(char* arg, int iode_type)

    cdef cppclass KDB:
        short               k_type
        short               k_mode
        string              k_arch
        string              description
        CSample*            sample
        char                k_compressed
        char                k_db_type
        string              filepath

        # ==== methods ====

        int size() except +
        int get_iode_type() except +
        string get_iode_type_str() except +
        bool is_global_database() except +
        bool is_detached_database() except +
        bool is_subset_database() except +
        string get_filename_utf8() except +
        void set_filename_utf8(const string& filename) except +
        string get_description_utf8() except +
        void set_description_utf8(const string& description) except +
        int index_of(const string& name) except +
        bool contains(const string& name) except +

        # object names
        string get_name(const int pos) except +
        bool set_name(const int pos, const string& new_name) except +
        bool rename(const string& old_name, const string& new_name, const bool overwrite) except +
        string expand(const string& pattern, const char ch_all) except +
        set[string] filter_names(const string& pattern, const bool must_exist) except +
        set[string] get_names() except +
        string get_names_as_string() except +

        # delete
        void remove(const string& name) except +

        # Other methods
        void merge(const KDB& other, const bool overwrite) except +
        void copy_from(const string& input_files, const string objects_names) except +
        void merge_from(const string& input_file) except +
        vector[string] search(const string& pattern, const bool word, const bool case_sensitive, 
            const bool in_name, const bool in_formula, const bool in_text, const string& list_result) except +

        # load - save - clear
        bool load(const string& filepath) except +
        bool save(const string& filepath, const bool compress) except +
        void clear() except +

    # k_wsvar.c
    int KV_add(KDBVariables* kdb, char* varname)
    double KV_get(KDBVariables* kdb, string name, int t, int mode)
    void KV_set(KDBVariables* kdb, string name, int t, int mode, double value)

    # k_grep.c
    char* K_expand(int iode_type, char* filepath, char* pattern, int _all)


# C++ classes
# see https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html 

# WARNING: - to declare functions/methods with default values, you must use =* in a pxd file
#            https://cython.readthedocs.io/en/latest/src/userguide/language_basics.html#optional-arguments 
#          - combination of base class and renaming is not possible. You cannot declare derived C++ class as:
#               cdef cppclass Derived(Base) "DerivedRealName": (...)
#            The solution is to cimport from pxd file using the keyword as:
#               from derived cimport DerivedRealName as Derived   
#            https://stackoverflow.com/a/23455514

cdef extern from "cpp_api/KDB/kdb_global.h":
    void low_to_high(IodeLowToHigh type_, char method, string& filepath, string& var_list) except +
    void high_to_low(IodeHighToLow type_, string& filepath, string& var_list) except +

cdef extern from "cpp_api/KDB/kdb_reference.h":
    void load_reference_kdb(int index, string& filepath) except +
    void clear_reference_kdb(int index) except +
    void clear_all_reference_kdbs() except +

cdef extern from "cpp_api/compute/simulation.h":
    void eqs_estimate(const string& eqs, const string& from_period, const string& to_period, 
                      const int maxit, const double eps) except +

cdef extern from "pyiode/iode_database/variables_database.cpp":
    void _c_add_var_from_other(const string& name, KDBVariables* dest, KDBVariables* source, 
                               const int source_t_first, const int source_t_last) except +
    void _c_copy_var_content(const string& dest_name, KDBVariables* dest, const int dest_t_first, const int dest_t_last,
                             const string& source_name, KDBVariables* source, const int source_t_first, const int source_t_last) except +

    cdef enum BinaryOperation:
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_POW

    void _c_operation_scalar(const int op, KDBVariables* database, int t_first, int t_last, const double value) except +
    void _c_operation_one_period(const int op, KDBVariables* database, const int t, const double* values, const int nb_values) except +
    void _c_operation_one_var(const int op, KDBVariables* database, const string& name, int t_first, int t_last, const double* values) except +
    void _c_operation_between_two_vars(const int op, KDBVariables* database, const string& name, const int t_first, const int t_last, 
                                       KDBVariables* other, const string& other_name, const int other_t_first, const int other_t_last) except +


cdef extern from "api/b_errors.h":
    cdef cppclass IodeErrorManager:
        void append_error(string& msg)
        void display_last_error()
        string get_last_error()
        void print_last_error()
        void clear()

    # Define the global IodeErrorManager instance
    cdef IodeErrorManager error_manager  


cdef extern from "api/objs/comments.h":
    cdef cppclass KDBComments(KDB):
        map[string, string*] k_objs

        # Constructor
        # subset (shallow or deep copy) 
        KDBComments(KDBComments* db_parent, string pattern, bool copy) 

        # Public methods
        string get(string& name) except +
        bool add(string& name, string& comment) except +
        void update(string& name, string& comment) except +

    size_t hash_value(KDBComments&) except +

    # Define the global Comments instance
    ctypedef unique_ptr[KDBComments] KDBCommentsPtr
    cdef KDBCommentsPtr global_ws_cmt


cdef extern from "api/objs/equations.h":
    cdef cppclass KDBEquations(KDB):
        map[string, CEquation*] k_objs

        # Constructor
        # subset (shallow or deep copy) 
        KDBEquations(KDBEquations* db_parent, string pattern, bool copy) 

        # Public methods
        CEquation* get(string& name) except +
        string get_lec(string& name) except +
        bool add(string& name, CEquation& equation) except +
        void update(string& name, CEquation& equation) except +

        string get_lec(string& name) except +
        bool add(string& name, string& lec) except +
        void update(string& name, string& lec) except +

    size_t hash_value(KDBEquations&) except +

    # Define the global Equations instance
    ctypedef unique_ptr[KDBEquations] KDBEquationsPtr
    cdef KDBEquationsPtr global_ws_eqs


cdef extern from "api/objs/identities.h":
    cdef cppclass KDBIdentities(KDB):
        map[string, CIdentity*] k_objs

        # Constructor
        # subset (shallow or deep copy) 
        KDBIdentities(KDBIdentities* db_parent, string pattern, bool copy) 

        # Public methods
        CIdentity* get(string& name) except +
        bool add(string& name, string& identity_lec) except +
        void update(string& name, string& identity_lec) except +

        string get_lec(string& name) except +
        bool add(string& name, string& lec) except +
        void update(string& name, string& lec) except +

        bool execute_identities(string& from_period, string& to, string& identities_list, 
                                string& var_files, string& scalar_files, bint trace) except +

    size_t hash_value(KDBIdentities&) except +

    # Define the global Identities instance
    ctypedef unique_ptr[KDBIdentities] KDBIdentitiesPtr
    cdef KDBIdentitiesPtr global_ws_idt


cdef extern from "api/objs/lists.h":
    cdef cppclass KDBLists(KDB):
        map[string, string*] k_objs

        # Constructor
        # subset (shallow or deep copy) 
        KDBLists(KDBLists* db_parent, string pattern, bool copy) 

        # Public methods
        string get(string& name) except +
        bool add(string& name, string& list_) except +
        void update(string& name, string& list_) except +

    size_t hash_value(KDBLists&) except +

    # Define the global Lists instance
    ctypedef unique_ptr[KDBLists] KDBListsPtr
    cdef KDBListsPtr global_ws_lst


cdef extern from "api/objs/scalars.h":
    cdef cppclass KDBScalars(KDB):
        map[string, CScalar*] k_objs

        # Constructor
        # subset (shallow or deep copy) 
        KDBScalars(KDBScalars* db_parent, string pattern, bool copy) 

        # Public methods
        CScalar* get(string& name) except +
        bool add(string& name, CScalar& scalar) except +
        void update(string& name, CScalar& scalar) except +

    size_t hash_value(KDBScalars&) except +

    # Define the global Scalars instance
    ctypedef unique_ptr[KDBScalars] KDBScalarsPtr
    cdef KDBScalarsPtr global_ws_scl


cdef extern from "api/objs/tables.h":
    cdef cppclass KDBTables(KDB):
        map[string, CTable*] k_objs

        # Constructor
        # subset (shallow or deep copy) 
        KDBTables(KDBTables* db_parent, string pattern, bool copy) 

        # Public methods
        CTable* get(string& name) except +
        string get_title(string& name) except +
        bool add(string name, CTable& table) except +
        bool add(string name, int nb_columns) except +
        bool add(string name, int nbColumns, string def_, vector[string] vars, 
                 bool mode=False, bool files=False, bool date=False) except +
        bool add(string name, int nbColumns, string def_, vector[string] titles, 
                 vector[string] lecs, bool mode=False, bool files=False, bool date=False) except +
        bool add(string name, int nbColumns, string def_, string lecs, bool mode=False, 
                 bool files=False, bool date=False) except +
        void update(string& name, CTable& table) except +
        void print_to_file(string& dest_file, string& gsample, string& names, int nb_decimals, 
                           char format_) except +

    size_t hash_value(KDBTables&) except +

    # Define the global Tables instance
    ctypedef unique_ptr[KDBTables] KDBTablesPtr
    cdef KDBTablesPtr global_ws_tbl


cdef extern from "api/objs/variables.h":
    cdef cppclass KDBVariables(KDB):
        map[string, vector[double]*] k_objs

        # Constructor
        # subset (shallow or deep copy) 
        KDBVariables(KDBVariables* db_parent, string pattern, bool copy) 

        # Public methods
        vector[double] get(string& name) except +
        bool add(string& name, vector[double]& values) except +
        bool add(string& name, string& lec) except +
        void update(string& name, vector[double]& values) except +
        void update(string& name, vector[double]& values, int t_first, int t_last) except +
        void update(string& name, string& lec) except +
        void update(string& name, string& lec, int t_first, int t_last) except +

        double* get_var_ptr(string& name, int t) except +
        double get_var(string name, int t, IodeVarMode mode) except +
        bool set_var(string& name, double* value) except +

        CSample* get_sample()
        void set_sample(string& from_period, string& to_period) except +

        int get_nb_periods()
        string get_period(int t) except +
        float get_period_as_float(int t) except +
        vector[string] get_list_periods(string& from_period, string& to_period) except +
        vector[float] get_list_periods_as_float(string& from_period, string& to_period) except +

        void copy_from(string& input_files, string& from_period, string& to_period, string objects_names) except +

        void extrapolate(VariablesInitialization method, string& from_period, string& to, string& variables_list) except +
        void seasonal_adjustment(string& input_file, string& series, double eps_test) except +
        void trend_correction(string& input_file, double lambda_, string& series, bint log) except +

    size_t hash_value(KDBVariables&) except +

    # Define the global Variables instance
    ctypedef unique_ptr[KDBVariables] KDBVariablesPtr
    cdef KDBVariablesPtr global_ws_var
