#  IODE EXTENSION FOR PYTHON
#  =========================
#  
#     @header4iode
#  
#  PYIODE API
#  ==========
#  
#  
#  Interface IODE - Python - IODE
#  ------------------------------
#  During a PYIODE session, IODE data are grouped in workspaces, one by object type (variables, scalars, equations...) 
#  and stored in memory using a proprietary memory management system (IODE-SWAP).
#  
#  IODE functions operate inside IODE-SWAP memory. 
#  Exchanges between IODE objects (in IODE-SWAP) and python objects (larray, numpy or pandas objects in python memory) 
#  are made possible via interface functions.
#  
#  Source files (*.pyx, *.pxi and *.c)
#  -----------------------------------
#  The python functions are split according to their specific topics in pyiode_*.pyx 
#  where * can be ws, sample, objects, larray, reports...
#  Some utility functions have also been added in the pyiode_util.pyx module.
#  
#  The called C-api function signatures are found in iode.pxi.
#  These functions are declared in iode.h (mostly in the sub file iodeapi.h) 
#  and defined (again for the most part) in b_api.c.
# 
#
#  How to add a new Py-function based on a C-function
#  --------------------------------------------------
#  
#  1. In C
#      Create the new function in one of the iode_c_api C modules, for example in api/b_api.c:
#           int IodeMyFn(char* name) 
#      Declare the new function in api/iodeapi.h
#  
#      For Visual Studio (CMake) version:
#        - Regenerate iode_c_api.lib (using CMake)
#  
#      For non Visual Studio version (standard Microsoft nmake / cl compiler):
#        - Goto .../iode/api/vc64 
#        - Execute set64.bat to create the environment variables for VC64 nmake
#        - Execute nmake to create 64 bits iode_c_api.lib => api/vc64/nmake
#  
#  2. In Python (*.pyx files)
#      Add the new function declaration in iode.pxi in the section "cdef extern from "iode.h":":
#          cdef int IodeMyFn(char* name)  
#      (Note that there is no semi-colon at the end of the line !)
#      
#      Create the Python equivalent to IodeMyFn() in the appropriate module pyiode_*.pyx.
#          def myfn(arg):
#              return IodeMyFn(cstr(arg))
#  
#      If needed, use the python utility functions:
#          - cstr() to translate python strings (utf8) to C char* (ansi code cp850)
#          - pystr() to do the reverse
#          - pylist(char** c_list) to convert C char** to python lists 
#               (don't forget to free c_list afterwards, e.g. by a call to SCR_free_tbl(c_list))
#          - pyfloats(double *cvar, int lg) to convert a vector of doubles of length lg 
#               into a python list of doubles
#  
#  3. In a Developer Command Prompt:
#      c:\> cd  <path_to_iode>/pyiode
#      c:<path_to_iode>/pyiode> conda activate <conda_env>
#      c:<path_to_iode>/pyiode> makepy.bat <conda_env>
   
#  cython: binding=True, language_level=3, embedsignature=True

# distutils: language = c++
from iode_python cimport IodeInit, ODE_assign_super_PYIODE


# PYIODE API
# ----------

# IODE defines used in python functions
# -------------------------------------
# (from #defines in iode.h)

# Object and file types
K_CMT = 0
K_EQS = 1
K_IDT = 2
K_LST = 3
K_SCL = 4
K_TBL = 5
K_VAR = 6

# Simulation parameters
SORT_CONNEX = 0
SORT_BOTH   = 1
SORT_NONE   = 2

KV_INIT_TM1      = 0
KV_INIT_TM1_A    = 1
KV_INIT_EXTRA    = 2
KV_INIT_EXTRA_A  = 3
KV_INIT_ASIS     = 4
KV_INIT_TM1_NA   = 5
KV_INIT_EXTRA_NA = 6

# Print outputs
W_GDI   = 1
W_A2M   = 7
W_MIF   = 6
W_HTML  = 5
W_RTF   = 4
W_CSV   = 8
W_DUMMY = 9

# HTOL methods
HTOL_LAST = 0
HTOL_MEAN = 1
HTOL_SUM  = 2

# LTOH defines 
LTOH_STOCK  = 0
LTOH_FLOW   = 1
LTOH_LIN    = 'L'
LTOH_CS     = 'C'
LTOH_STEP   = 'S'

# Included modules by topic
# -------------------------

include "pyiode_cdef.pyx"
include "pyiode_util.pyx"
include "pyiode_lec.pyx"
include "pyiode_period.pyx"
include "pyiode_sample.pyx"

include "objects/equation.pyx"
include "objects/scalar.pyx"

include "pyiode_objs.pyx"
include "pyiode_ws.pyx"
include "pyiode_data.pyx"
include "pyiode_estim.pyx"
include "pyiode_model.pyx"
include "pyiode_exec.pyx"
include "pyiode_print.pyx"
include "pyiode_reports.pyx"
include "pyiode_larray.pyx"
include "pyiode_pandas.pyx"
include "pyiode_wrt.pyx"


# MAIN
# ----

# Numpy must be initialized before we can use it safely.
import numpy as np
np.import_array()

# IODE ws must initialize SWAP memory and create empty workspaces 
IodeInit()

# Super fns could be replaced here (optional)
ODE_assign_super_PYIODE() 


# ------------------------------------------------------------------------------------------
# API to be implemented (tentative list - to be reviewed)
# -------------------------------------------------------
#
# - file_import_var",            B_FileImportVar,     
# - file_import_cmt",            B_FileImportCmt,     
# 
# x ws_content_*  
# x ws_sample_*
# x ws_load_*",                 B_WsLoad,            
# - ws_copy_*",                 B_WsCopy,            
# x ws_save_*",                 B_WsSave,            
# - ws_savecmp_*",              B_WsSaveCmp,         
# - ws_import_*",               B_WsImport,          
# - ws_export_*",               B_WsExport,          
# - ws_merge_*",                B_WsMerge,           
# x ws_clear_*",                B_WsClear,           
# x ws_clear_all",              B_WsClearAll,        
# - ws_descr",                  B_WsDescr,           
# - ws_extrapolate",            B_WsExtrapolate,     
# - ws_aggr_char",              B_WsAggrChar,        
# - ws_aggr_sum",               B_WsAggrSum,         
# - ws_aggr_mean",              B_WsAggrMean,        
# - ws_aggr_prod",              B_WsAggrProd,        
# x ws_htol_last",              B_WsHtoLLast,        
# x ws_htol_sum",               B_WsHtoLSum,         
# x ws_htol_mean",              B_WsHtoLMean,        
# x ws_ltoh_flow",              B_WsLtoHFlow,        
# x ws_ltoh_stock",             B_WsLtoHStock,       
# - csv_save",                  B_CsvSave,           
# - csv_digits",                B_CsvNbDec,          
# - csv_sep",                   B_CsvSep,            
# - csv_dec",                   B_CsvDec,            
# - csv_nan",                   B_CsvNaN,            
# - csv_axes",                  B_CsvAxes,           
# - stat_unit_root",            B_StatUnitRoot,      
# 
# - ws_seasonadj",              B_WsSeasonAdj,       
# - ws_trend",                  B_WsTrend,           
# - ws_trendstd",               B_WsTrendStd,        
#   
# - ws_import_eviews",			B_WsImportEviews,   
# 
# 
# b_data.c 
# --------
# x data_update_*",             B_DataUpdate,      
# - data_delete_*",             B_DataDelete,      
# - data_rename_*",             B_DataRename,      
# - data_search",               B_DataSearch,      
# - data_duplicate_*",          B_DataDuplicate,   
# - data_scan_*",               B_DataScan,        
# - data_list",                 B_DataList,        
# - data_compare_*",            B_DataCompare,     
# - data_compare_eps",          B_DataCompareEps,  
# - data_list_sort",            B_DataListSort,    
# - data_calc_var",             B_DataCalcVar,     
# - data_calc_lst",             B_DataCalcLst,     
# - data_pattern",              B_DataPattern,     
# - data_display_graph",        B_DataDisplayGraph,
# - data_print_graph",          B_DataPrintGraph,  
# 
#  b_ras.c
#  -------
# - data_ras_var",              B_DataRasVar,           
#  
#  b_est.c
#  -------
# - eqs_estimate",              B_EqsEstimate,         
# 
#  b_step.c
#  -------- 
#  - eqsstepwise",              B_EqsStepWise,        
#  
#  b_print.c
#  ---------
#  - print_objtitle",           B_PrintObjTblTitle,   
#  - print_objlec",             B_PrintObjLec,        
#  - print_objinfos",           B_PrintObjEqsInfos,   
#   
#  - print_obj_def",            B_PrintObjDef,        
#  - printdest",                B_PrintDest,          
#  - printdestnew",             B_PrintDestNew,       
#  - printmulti",               B_PrintMulti,         
#  - printnbdec",               B_PrintNbDec,         
#  - printlang",                B_PrintLang,          
#  - printtblfile",             B_ViewTblFile,        
#  - printtbl",                 B_PrintTbl,           
#  - printvar",                 B_PrintVar,           
#   
#  - graphdefault",             B_GraphDefault,       
#   
#  - printgrall",               B_PrintGr,            
#  - printgr",                  B_PrintGr,            
#     
#  x model_simulate",           B_ModelSimulate,      
#  - model_compile",            B_ModelCompile, 	  
#  
#  - model_calcscc",            B_ModelCalcSCC,       
#  - model_simulate_scc",       B_ModelSimulateSCC,   
# 
#  - model_simulate_save_niters",  B_ModelSimulateSaveNIters,
#  - model_simulate_save_norms",   B_ModelSimulateSaveNorms, 
# 
#  - idtexecute",               B_IdtExecute,           
#  - idtexecutevarfiles",       B_IdtExecuteVarFiles,   
#  - idtexecutesclfiles",       B_IdtExecuteSclFiles,   
#  - idtexecutetrace",          B_IdtExecuteTrace,      
#                                                       
#  X report_exec",              B_ReportExec,           
#  X report_exec_line",         B_ReportExecLine,       
#                                                       
#  - print_a2mappend",          B_PrintA2mAppend,       
#  - print_font",               B_PrintFont,            
#  - print_table_font",         B_PrintTFont,           
#  - print_table_box",          B_PrintTBox,            
#  - print_table_color",        B_PrintTColor,          
#  - print_table_width",        B_PrintTWidth,          
#  - print_table_break",        B_PrintTBreak,          
#  - print_table_page",         B_PrintTPage,           
#  -                                                    
#  - print_html_table_class",   B_PrintHtmlTableClass,  
#  - print_html_tr_class",      B_PrintHtmlTRClass,     
#  - print_html_th_class",      B_PrintHtmlTHClass,     
#  - print_html_td_class",      B_PrintHtmlTDClass,     
#  -                                                    
#  - printbackground",          B_PrintGColor,          
#  - printgraphbox",            B_PrintGBox,            
#  - printgraphbrush",          B_PrintGBrush,          
#  - printgraphsize",           B_PrintGSize,           
#  - printgraphpage",           B_PrintGPage,           
#  - printgraphtheme",          B_PrintGTheme,          
#  - printgraphband",           B_PrintGBand,           
#  -                                                    
#  - print_rtfhelp",             B_PrintRtfHelp,         
#  - print_htmlhelp",            B_PrintHtmlHelp,        
#  - print_rtftopic",            B_PrintRtfTopic,        
#  - print_rtflevel",            B_PrintRtfLevel,        
#  - print_rtftitle",            B_PrintRtfTitle,        
#  - print_rtfcopyright",        B_PrintRtfCopy,         
#  - print_paranum",             B_PrintParaNum,         
#  - print_pageheader",          B_PrintPageHeader,      
#  - print_pagefooter",          B_PrintPageFooter,      
#  - print_orientation",         B_PrintGdiOrient,       
#  - print_duplex",              B_PrintGdiDuplex,       
#  - set_printer",               B_PrintGdiPrinter,      
#
#  - print_html_strip",           B_PrintHtmlStrip,       
#  - print_html_style",           B_PrintHtmlStyle,       
#                                                       
#  - sys_ansitoutf8",            B_SysAnsiToUTF8,        
#  - sys_oemtoutf8",             B_SysOemToUTF8,         
#  - sys_ansitooem",             B_SysAnsiToOem,         
#  - sys_oemtoansi",             B_SysOemToAnsi,         
#                                                       
#  - a2m_to_html",                B_A2mToHtml,            
#  - a2m_to_mif",                 B_A2mToMif,             
#  - a2m_to_csv",                 B_A2mToCsv,             
#  - a2m_to_rtf",                 B_A2mToRtf,             
#  - a2m_to_printer",             B_A2mToPrinter,         
