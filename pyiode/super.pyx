from libc.string cimport strlen

from pyiode.super cimport TBL
from pyiode.super cimport KT_WS
from pyiode.super cimport K_end_ws
from pyiode.super cimport K_find
from pyiode.super cimport K_add
from pyiode.super cimport K_del

cdef extern from "super.h":
    cdef int c_kerror_super(const int level, const char* msg) except? -1
    cdef void c_kwarning_super(const char* msg) noexcept
    cdef void c_kmsg_super(const char* msg) noexcept
    cdef int c_kconfirm_super(const char* msg) noexcept
    cdef void c_kpanic_super() except *

    cdef int c_PrintObjDef_super() except? -1
    cdef int c_ViewPrintGr_super() except? -1
    cdef int c_ViewPrintTbl_super() except? -1
    cdef int c_ViewByTbl_super() except? -1
    cdef int c_DataSearch_super() except? -1
    cdef int c_DataDuplicate_super() except? -1
    cdef int c_DataList_super() except? -1
    cdef int c_DataCompare_super() except? -1
    cdef int c_DataCalcLst_super() except? -1
    cdef int c_DataListSort_super() except? -1
    cdef int c_DataEditGraph_super() except? -1
    cdef int c_DataScan_super() except? -1
    cdef int c_StatUnitRoot_super() except? -1
    cdef int c_WsLoad_super() except? -1
    cdef int c_WsSave_super() except? -1
    cdef int c_WsMerge_super() except? -1
    cdef int c_WsDescr_super() except? -1
    cdef int c_WsCopy_super() except? -1
    cdef int c_WsClear_super() except? -1
    cdef int c_WsSample_super() except? -1
    cdef int c_WsExtrapolate_super() except? -1
    cdef int c_WsHtoL_super() except? -1
    cdef int c_WsLtoH_super() except? -1
    cdef int c_WsSeasonAdj_super() except? -1
    cdef int c_WsTrend_super() except? -1
    cdef int c_ModelSimulate_super() except? -1
    cdef int c_ModelCompile_super() except? -1
    cdef int c_IdtExecute_super() except? -1
    cdef int c_EqsEstimate_super() except? -1
    cdef int c_XodeRuleImport_super() except? -1

    cdef int c_ViewTable_super(TBL* tbl, char* smpl, char* name) except? -1


# Create an empty dictionary to serve as the registry
__registry_super_functions = {}

# Define a set of allowed keys
allowed_keys = {'error', 'warning', 'message', 'confirm', 'panic', 
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
        __registry_super_functions[name] = func
        return func
    return decorator


cdef int c_kerror_super(const int level, const char* msg):
    cdef size_t length = strlen(msg)
    cdef bytes b_msg = bytes(msg[:length])
    return __registry_super_functions['error'](level, b_msg.decode('utf-8'))   

cdef void c_kwarning_super(const char* msg) noexcept:
    cdef size_t length = strlen(msg)
    cdef bytes b_msg = bytes(msg[:length])
    __registry_super_functions['warning'](b_msg.decode('utf-8'))

cdef void c_kmsg_super(const char* msg) noexcept:
    cdef size_t length = strlen(msg)
    cdef bytes b_msg = bytes(msg[:length])
    __registry_super_functions['message'](b_msg.decode('utf-8'))

cdef int c_kconfirm_super(const char* msg) noexcept:
    cdef size_t length = strlen(msg)
    cdef bytes b_msg = bytes(msg[:length])
    return __registry_super_functions['confirm'](b_msg.decode('utf-8'))

cdef void c_kpanic_super():
    K_end_ws(1)
    __registry_super_functions['panic']()

cdef int c_PrintObjDef_super():
    return __registry_super_functions['PrintObjDef']()

cdef int c_ViewPrintGr_super():
    return __registry_super_functions['ViePrintGr']()

cdef int c_ViewPrintTbl_super():
    return __registry_super_functions['ViewPrintTbl']()

cdef int c_ViewByTbl_super():
    return __registry_super_functions['ViewByTbl']()

cdef int c_DataSearch_super():
    return __registry_super_functions['DaSearch']()

cdef int c_DataDuplicate_super():
    return __registry_super_functions['DataDuplicate']()

cdef int c_DataList_super():
    return __registry_super_functions['DataList']()

cdef int c_DataCompare_super():
    return __registry_super_functions['DataCompare']()

cdef int c_DataCalcLst_super():
    return __registry_super_functions['DataCalcLst']()

cdef int c_DataListSort_super():
    return __registry_super_functions['DataListSort']()

cdef int c_DataEditGraph_super():
    return __registry_super_functions['DataEditGraph']()

cdef int c_DataScan_super():
    return __registry_super_functions['DataScan']()

cdef int c_StatUnitRoot_super():
    return __registry_super_functions['StatUnitRoot']()

cdef int c_WsLoad_super():
    return __registry_super_functions['WsLoad']()

cdef int c_WsSave_super():
    return __registry_super_functions['WsSave']()

cdef int c_WsMerge_super():
    return __registry_super_functions['WsMerge']()

cdef int c_WsDescr_super():
    return __registry_super_functions['WsDescr']()

cdef int c_WsCopy_super():
    return __registry_super_functions['WsCopy']()

cdef int c_WsClear_super():
    return __registry_super_functions['WsClear']()

cdef int c_WsSample_super():
    return __registry_super_functions['WsSample']()

cdef int c_WsExtrapolate_super():
    return __registry_super_functions['WsExtrapolate']()

cdef int c_WsHtoL_super():
    return __registry_super_functions['WsHtoL']()

cdef int c_WsLtoH_super():
    return __registry_super_functions['WsLtoH']()

cdef int c_WsSeasonAdj_super():
    return __registry_super_functions['WsSeasonAdj']()

cdef int c_WsTrend_super():
    return __registry_super_functions['WsTrend']()

cdef int c_ModelSimulate_super():
    return __registry_super_functions['ModelSimulate']()

cdef int c_ModelCompile_super():
    return __registry_super_functions['ModelCompile']()

cdef int c_IdtExecute_super():
    return __registry_super_functions['IdtExecute']()

cdef int c_EqsEstimate_super():
    return __registry_super_functions['EqsEstimate']()

cdef int c_XodeRuleImport_super():
    return __registry_super_functions['XodeRuleImport']()

cdef int c_ViewTable_super(TBL* tbl, char* smpl, char* name):
    cdef int success = 0
    cdef int pos = K_find(KT_WS, name)
    cdef bytes b_table_name = name
    cdef bytes b_generalized_sample = smpl
    
    table_name: str = b_table_name.decode()
    generalized_sample: str = b_generalized_sample.decode()
    nb_decimals: int = 6

    table_found: bool = pos >= 0
    if not table_found:
        # temporary add the passed IODE table to the global database of tables
        pos = K_add(KT_WS, name, tbl)
	
    success = __registry_super_functions['ViewTable'](table_name, generalized_sample, nb_decimals)

    if not table_found:
        # remove the temporary added IODE table from the global database
        K_del(KT_WS, pos)

    return success
