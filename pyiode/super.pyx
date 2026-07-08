from libc.string cimport strlen
from libcpp.string cimport string

from pyiode.super cimport K_end_ws
from pyiode.objects.table cimport CTable


cdef extern from "super.h":
    cdef void c_kwarning_super(const char* msg) noexcept
    cdef void c_kmsg_super(const char* msg) noexcept
    cdef int c_kconfirm_super(const char* msg) noexcept
    cdef void c_kpanic_super() except *
    cdef void c_kpause_super() noexcept
    cdef int c_kmsgbox_super(const unsigned char* title, const unsigned char* msg, 
                             const unsigned char** buts) noexcept
    cdef bint contain_table(string& name) except +
    cdef bint add_table(string& name, CTable* value) except +
    cdef void remove_table(string& name) except +

    cdef int c_PrintObjDef_super(char* arg, int unused) except? -1
    cdef int c_ViewPrintGr_super(char* arg, int unused) except? -1
    cdef int c_ViewPrintTbl_super(char* arg, int unused) except? -1
    cdef int c_ViewByTbl_super(char* arg, int unused) except? -1
    cdef int c_DataSearch_super(char* arg, int unused) except? -1
    cdef int c_DataDuplicate_super(char* arg, int unused) except? -1
    cdef int c_DataList_super(char* arg, int unused) except? -1
    cdef int c_DataCompare_super(char* arg, int unused) except? -1
    cdef int c_DataCalcLst_super(char* arg, int unused) except? -1
    cdef int c_DataListSort_super(char* arg, int unused) except? -1
    cdef int c_DataEditGraph_super(char* arg, int unused) except? -1
    cdef int c_DataScan_super(char* arg, int unused) except? -1
    cdef int c_StatUnitRoot_super(char* arg, int unused) except? -1
    cdef int c_WsLoad_super(char* arg, int unused) except? -1
    cdef int c_WsSave_super(char* arg, int unused) except? -1
    cdef int c_WsMerge_super(char* arg, int unused) except? -1
    cdef int c_WsDescr_super(char* arg, int unused) except? -1
    cdef int c_WsCopy_super(char* arg, int unused) except? -1
    cdef int c_WsClear_super(char* arg, int unused) except? -1
    cdef int c_WsSample_super(char* arg, int unused) except? -1
    cdef int c_WsExtrapolate_super(char* arg, int unused) except? -1
    cdef int c_WsHtoL_super(char* arg, int unused) except? -1
    cdef int c_WsLtoH_super(char* arg, int unused) except? -1
    cdef int c_WsSeasonAdj_super(char* arg, int unused) except? -1
    cdef int c_WsTrend_super(char* arg, int unused) except? -1
    cdef int c_ModelSimulate_super(char* arg, int unused) except? -1
    cdef int c_ModelCompile_super(char* arg, int unused) except? -1
    cdef int c_IdtExecute_super(char* arg, int unused) except? -1
    cdef int c_EqsEstimate_super(char* arg, int unused) except? -1
    cdef int c_XodeRuleImport_super(char* arg, int unused) except? -1

    cdef int c_ViewTable_super(CTable* tbl, char* smpl, char* name) except? -1


# Create an empty dictionary to serve as the registry
__registry_funcs = {}

# Define a set of allowed keys
allowed_keys = {'error', 'warning', 'message', 'confirm', 'panic', 'pause', 'msgbox',
                'PrintObjDef', 'ViewPrintGr', 'ViewPrintTbl', 'ViewByTbl', 
                'DataSearch', 'DataDuplicate', 'DataList', 'DataCompare', 
                'DataCalcLst', 'DataListSort', 'DataEditGraph', 'DataScan', 
                'StatUnitRoot', 
                'WsLoad', 'WsSave', 'WsMerge', 'WsDescr', 'WsCopy', 'WsClear', 
                'WsSample', 'WsExtrapolate', 'WsHtoL', 'WsLtoH', 'WsSeasonAdj', 'WsTrend', 
                'ModelSimulate', 'ModelCompile', 'IdtExecute', 'EqsEstimate', 
                'XodeRuleImport', 'ViewTable'}

# Define a decorator function that registers functions with a given name
def register_super_function(name):
    # Check if the name is in the set of allowed keys
    if name not in allowed_keys:
        raise ValueError(f"Invalid key '{name}'. Possible keys are: {', '.join(allowed_keys)}")
    def decorator(func):
        __registry_funcs[name] = func
        return func
    return decorator  

def backup_super_functions() -> dict():
    """
    Backup the current state of the super-functions registry.
    """
    return {key: func for key, func in __registry_funcs.items()}

def restore_super_functions(registry_funcs_backup: dict) -> None:
    """
    Restore the super-functions registry from a previously captured snapshot.
    """
    global __registry_funcs
    if not isinstance(registry_funcs_backup, dict):
        raise TypeError("registry_funcs_backup must be a dictionary.")
    __registry_funcs.clear()
    __registry_funcs = {key: func for key, func in registry_funcs_backup.items()}


cdef void c_kwarning_super(const char* msg) noexcept:
    cdef size_t length = strlen(msg)
    cdef bytes b_msg = bytes(msg[:length])
    __registry_funcs['warning'](b_msg.decode('cp850'))

cdef void c_kmsg_super(const char* msg) noexcept:
    cdef size_t length = strlen(msg)
    cdef bytes b_msg = bytes(msg[:length])
    __registry_funcs['message'](b_msg.decode('cp850'))

cdef int c_kconfirm_super(const char* msg) noexcept:
    cdef size_t length = strlen(msg)
    cdef bytes b_msg = bytes(msg[:length])
    return __registry_funcs['confirm'](b_msg.decode('cp850'))

cdef void c_kpanic_super():
    K_end_ws(1)
    __registry_funcs['panic']()

cdef void c_kpause_super() noexcept:
    __registry_funcs['pause']()

cdef int c_kmsgbox_super(const unsigned char* title, const unsigned char* msg, 
                         const unsigned char** buts) noexcept:
    cdef size_t length_title = strlen(<char*>title)
    cdef size_t length_msg = strlen(<char*>msg)
    cdef bytes b_title = bytes(title[:length_title])
    cdef bytes b_msg = bytes(msg[:length_msg])
    return __registry_funcs['msgbox'](b_title.decode('cp850'), b_msg.decode('cp850'))

cdef int c_PrintObjDef_super(char* arg, int unused):
    return __registry_funcs['PrintObjDef']()

cdef int c_ViewPrintGr_super(char* arg, int unused):
    return __registry_funcs['ViePrintGr']()

cdef int c_ViewPrintTbl_super(char* arg, int unused):
    return __registry_funcs['ViewPrintTbl']()

cdef int c_ViewByTbl_super(char* arg, int unused):
    return __registry_funcs['ViewByTbl']()

cdef int c_DataSearch_super(char* arg, int unused):
    return __registry_funcs['DaSearch']()

cdef int c_DataDuplicate_super(char* arg, int unused):
    return __registry_funcs['DataDuplicate']()

cdef int c_DataList_super(char* arg, int unused):
    return __registry_funcs['DataList']()

cdef int c_DataCompare_super(char* arg, int unused):
    return __registry_funcs['DataCompare']()

cdef int c_DataCalcLst_super(char* arg, int unused):
    return __registry_funcs['DataCalcLst']()

cdef int c_DataListSort_super(char* arg, int unused):
    return __registry_funcs['DataListSort']()

cdef int c_DataEditGraph_super(char* arg, int unused):
    return __registry_funcs['DataEditGraph']()

cdef int c_DataScan_super(char* arg, int unused):
    return __registry_funcs['DataScan']()

cdef int c_StatUnitRoot_super(char* arg, int unused):
    return __registry_funcs['StatUnitRoot']()

cdef int c_WsLoad_super(char* arg, int unused):
    return __registry_funcs['WsLoad']()

cdef int c_WsSave_super(char* arg, int unused):
    return __registry_funcs['WsSave']()

cdef int c_WsMerge_super(char* arg, int unused):
    return __registry_funcs['WsMerge']()

cdef int c_WsDescr_super(char* arg, int unused):
    return __registry_funcs['WsDescr']()

cdef int c_WsCopy_super(char* arg, int unused):
    return __registry_funcs['WsCopy']()

cdef int c_WsClear_super(char* arg, int unused):
    return __registry_funcs['WsClear']()

cdef int c_WsSample_super(char* arg, int unused):
    return __registry_funcs['WsSample']()

cdef int c_WsExtrapolate_super(char* arg, int unused):
    return __registry_funcs['WsExtrapolate']()

cdef int c_WsHtoL_super(char* arg, int unused):
    return __registry_funcs['WsHtoL']()

cdef int c_WsLtoH_super(char* arg, int unused):
    return __registry_funcs['WsLtoH']()

cdef int c_WsSeasonAdj_super(char* arg, int unused):
    return __registry_funcs['WsSeasonAdj']()

cdef int c_WsTrend_super(char* arg, int unused):
    return __registry_funcs['WsTrend']()

cdef int c_ModelSimulate_super(char* arg, int unused):
    return __registry_funcs['ModelSimulate']()

cdef int c_ModelCompile_super(char* arg, int unused):
    return __registry_funcs['ModelCompile']()

cdef int c_IdtExecute_super(char* arg, int unused):
    return __registry_funcs['IdtExecute']()

cdef int c_EqsEstimate_super(char* arg, int unused):
    return __registry_funcs['EqsEstimate']()

cdef int c_XodeRuleImport_super(char* arg, int unused):
    return __registry_funcs['XodeRuleImport']()

cdef int c_ViewTable_super(CTable* tbl, char* smpl, char* c_name):
    cdef bint success = False
    cdef int i_success = 0
    cdef bytes b_name = c_name
    cdef string s_name = <string>b_name
    cdef bytes b_generalized_sample = smpl

    name: str = b_name.decode()
    generalized_sample: str = b_generalized_sample.decode()
    nb_decimals: int = 6

    table_found: bool = contain_table(name)
    if not table_found:
        # temporary add the passed IODE table to the global database of tables
        success = add_table(s_name, tbl)
        if not success:
            return -1
	
    i_success = __registry_funcs['ViewTable'](name, generalized_sample, nb_decimals)

    if not table_found:
        # remove the temporary added IODE table from the global database
        remove_table(s_name)

    return i_success
