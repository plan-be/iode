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
   
#  cython: binding=True, language_level=3, embedsignature=True

# distutils: language = c++
from iode_python cimport IodeInit, ODE_assign_super_PYIODE

# MAIN
# ----

# Numpy must be initialized before we can use it safely.
import numpy as np
np.import_array()

# IODE ws must initialize SWAP memory and create empty workspaces 
IodeInit(NULL)

# Super fns could be replaced here (optional)
ODE_assign_super_PYIODE() 

# PYIODE API
# ----------

# Included modules by topic
# -------------------------

include "common.pyx"
include "util.pyx"
include "lec.pyx"

include "time/period.pyx"
include "time/sample.pyx"

include "objects/equation.pyx"
include "objects/scalar.pyx"
include "objects/table.pyx"

include "computed_table/computed_table.pyx"

include "iode_database/abstract_database.pyx"
include "iode_database/comments_database.pyx"
include "iode_database/equations_database.pyx"
include "iode_database/identities_database.pyx"
include "iode_database/lists_database.pyx"
include "iode_database/scalars_database.pyx"
include "iode_database/tables_database.pyx"
include "iode_database/variables_database.pyx"

include "compute/estimation.pyx"
include "compute/simulation.pyx"

include "reports.pyx"
include "wrt.pyx"


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
