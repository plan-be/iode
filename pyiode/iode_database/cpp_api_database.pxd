# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map

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
    ctypedef unsigned long SWHDL
    ctypedef char OFNAME[64]
    ctypedef char FNAME[512]
    ctypedef char ONAME[21]

    int K_NBDEC
    int B_TABLE_TITLE
    int B_EQS_INFOS
    int B_EQS_LEC
    double K_CMP_EPS

    char*   KCVAL(KDB* kdb, string name)
    char*   KIVAL(KDB* kdb, string name)
    char*   KLVAL(KDB* kdb, string name)
    double* KVVAL(KDB* kdb, string name)
    double* KVVAL(KDB* kdb, string name, int t)

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

    ctypedef struct KDB:
        map[string, SWHDL]  k_objs
        short               k_type
        short               k_mode
        string              k_arch
        string              description
        CSample*            sample
        char                k_compressed
        char                k_db_type
        string              filepath

        int size()
        int index_of(string& name)
        bool contains(string& name)
        string get_name(int pos)
        # variables
        bool set(string& name, double* var, int nb_obs) except +
        bool remove(string& name) except +

    # k_wsvar.c
    int KV_add(KDB* kdb, char* varname)
    double KV_get(KDB *kdb, string name, int t, int mode)
    void KV_set(KDB *kdb, string name, int t, int mode, double value)

    # k_grep.c
    char** K_grep(KDB* kdb, char* pattern, int ecase, int names, int forms, int texts, int _all)
    char*  K_expand(int iode_type, char* filepath, char* pattern, int _all)
    char*  K_expand_kdb(KDB* kdb, int iode_type, char* pattern, int _all)


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
    void load_reference_kdb(int index, IodeType iode_type, string& filepath) except +
    void clear_reference_kdb(int index, IodeType iode_type) except +
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

cdef extern from "cpp_api/KDB/kdb_abstract.h":
    cdef cppclass KDBAbstract:    
        # Public methods
        int get_iode_type() const
        bool is_global_database() const
        bool is_local_database() const
        bool is_shallow_copy_database() const
        int size() const

        KDB* get_database() except +

        int index_of(string& name)
        bool contains(string& name)
        string get_name(int pos)
        vector[string] get_names(string& pattern, bool must_exist) except +
        bool rename(string& old_name, string& new_name, bool overwrite) except +
        bool remove(string& name) except +

        string get_filename() const
        void set_filename(string& filename) except +

        string get_description() const
        void set_description(string& description) except +

        void merge(KDBAbstract& other, bool overwrite) except +
        void copy_from(string& input_file, string& objects_names) except +
        void merge_from(string& input_file) except +

        vector[string] search(string& pattern, bint word, bint case_sensitive, bint in_name, 
                              bint in_formula, bint in_text, string& list_result) except +

        void save(string& filepath, bool compress) except +
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
        bool add(string& name, string& comment) except +
        void update(string& name, string& comment) except +

    size_t hash_value(KDBComments&) except +

    # Define the global Comments instance
    cdef KDBComments Comments


cdef extern from "cpp_api/KDB/kdb_equations.h":
    cdef cppclass KDBEquations(KDBTemplate[string]):
        # Constructor
        KDBEquations(string& filepath) except +

        # Public methods
        KDBEquations* subset(string& pattern, bool deep_copy) except +
        CEquation* get(string& name) except +
        CEquation* copy(string& name) except +
        string get_lec(string& name) except +
        bool add(string& name, CEquation& equation) except +
        bool add(string& name, string& lec, string& method, string& from_period, string& to_period, 
                string& comment, string& instruments, string& block, bint date) except +
        void update(string& name, CEquation& equation) except +
        void update(string& name, string& lec, string& method, string& from_period, string& to_period, 
                    string& comment, string& instruments, string& block, bint date) except +

    size_t hash_value(KDBEquations&) except +

    # Define the global Equations instance
    cdef KDBEquations Equations


cdef extern from "cpp_api/KDB/kdb_identities.h":
    cdef cppclass KDBIdentities(KDBTemplate[string]):
        # Constructor
        KDBIdentities(string& filepath) except +

        # Public methods
        KDBIdentities* subset(string& pattern, bool deep_copy) except +
        CIdentity* get(string& name) except +
        CIdentity* copy(string& name) except +
        bool add(string& name, string& identity_lec) except +
        void update(string& name, string& identity_lec) except +
        string get_lec(string& name) except +
        bool execute_identities(string& from_period, string& to, string& identities_list, 
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
        bool add(string& name, string& list_) except +
        void update(string& name, string& list_) except +

    size_t hash_value(KDBLists&) except +

    # Define the global Lists instance
    cdef KDBLists Lists


cdef extern from "cpp_api/KDB/kdb_scalars.h":
    cdef cppclass KDBScalars(KDBTemplate[string]):
        # Constructor
        KDBScalars(string& filepath) except +

        # Public methods
        KDBScalars* subset(string& pattern, bool deep_copy) except +
        CScalar* get(string& name) except +
        CScalar* copy(string& name) except +
        bool add(string& name, CScalar& scalar) except +
        bool add(string& name, double value, double relax, double std) except +
        void update(string& name, CScalar& scalar) except +
        void update(string& name, double value, double relax, double std) except +

    size_t hash_value(KDBScalars&) except +

    # Define the global Scalars instance
    cdef KDBScalars Scalars


cdef extern from "cpp_api/KDB/kdb_tables.h":
    cdef cppclass KDBTables(KDBTemplate[string]):
        # Constructor
        KDBTables(string& filepath) except +

        # Public methods
        KDBTables* subset(string& pattern, bool deep_copy) except +
        CTable* get(string& name) except +
        CTable* copy(string& name) except +
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
    cdef KDBTables Tables


cdef extern from "cpp_api/KDB/kdb_variables.h":
    cdef cppclass KDBVariables(KDBTemplate[vector[double]]):
        # Constructor
        KDBVariables(string& filepath) except +

        # Public methods
        KDBVariables* subset(string& pattern, bool deep_copy) except +
        vector[double] get(string& name) except +
        vector[double] copy(string& name) except +
        bool add(string& name, vector[double]& values) except +
        bool add(string& name, string& lec) except +
        void update(string& name, vector[double]& values) except +
        void update(string& name, vector[double]& values, int t_first, int t_last) except +
        void update(string& name, string& lec) except +
        void update(string& name, string& lec, int t_first, int t_last) except +

        double* get_var_ptr(string name) except +
        double get_var(string name, int t, IodeVarMode mode) except +

        CSample* get_sample()
        void set_sample(string& from_period, string& to_period) except +

        int get_nb_periods()
        string get_period(int t) except +
        float get_period_as_float(int t) except +
        vector[string] get_list_periods(string& from_period, string& to_period) except +
        vector[float] get_list_periods_as_float(string& from_period, string& to_period) except +

        void copy_from(string& input_file, string& from_period, string& to_period, string objects_names) except +

        void extrapolate(VariablesInitialization method, string& from_period, string& to, string& variables_list) except +
        void seasonal_adjustment(string& input_file, string& series, double eps_test) except +
        void trend_correction(string& input_file, double lambda_, string& series, bint log) except +

    size_t hash_value(KDBVariables&) except +

    # Define the global Variables instance
    cdef KDBVariables Variables

