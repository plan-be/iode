/** 
 *  @header4iode
 *  
 *  IODE report super function default implementations
 *  --------------------------------------------------
 *  
 *  Some report functions differ according to the context: for example, the function SB_*() only
 *  makes sense in the GUI version(s) of IODE (#WsLoad in a report). 
 *  To deal with these differences, we use the same logic as described in k_super.c: a default 
 *  function is defined and called in the non-GUI version.
 *  To replace that function by an alternative one (tailored to the context), a pointer to that 
 *  alternative function must be assigned to the <fn>_super variable.
 *  
 *  For example, in a report, the command "#WsLoad" calls the GUI function SB_WsLoad() which opens 
 *  the Dialog "Workspace / Load" and waits for a user action. In constrast, the command "$WsLoadVar" 
 *  calls B_WsLoad(filename, VARIABLES) which loads a WS, but without user interaction.
 *  
 *  Note that the GUI default functions defined in this file are empty (SB_*(), B_Scroll*()...
 *  
 *  List of functions that can be superseeded
 *  -----------------------------------------
 *  See the list of default implementation below.
 *  
 *  List of function pointers that can replace the standard implementation  
 *  ----------------------------------------------------------------------
 *  See list below: int (*<function_name>_super)()
 */ 

#include <stdarg.h>
#include "api/report/engine/engine.h"

/**
 *  Calls the super function fn (if not null) with a modifiable copy of arg.
 *  The super functions (see engine.h) still take a char* argument.
 *
 *  @param [in] fn      int (*)(char*, int)     super function (may be null)
 *  @param [in] arg     const std::string&      argument of the report command
 *  @param [in] unused  int                     2d argument of fn
 *  @return             int                     result of fn(), 0 if fn is null
 */
static int call_super(int (*fn)(char*, int), const std::string& arg, int unused)
{
    if(!fn) 
        return 0;
    std::string buf(arg);
    return fn(buf.data(), unused);
}

/**
 *  Same as call_super() for the super functions taking only a char* argument (B_Scroll*).
 */
static int call_super(int (*fn)(char*), const std::string& arg)
{
    if(!fn) 
        return 0;
    std::string buf(arg);
    return fn(buf.data());
}

// Default implementation 
int SB_FileDelete       (const std::string& arg, int unused) {return call_super(SB_FileDelete_super, arg, unused);}
int SB_FileRename       (const std::string& arg, int unused) {return call_super(SB_FileRename_super, arg, unused);}
int SB_FileCopy         (const std::string& arg, int unused) {return call_super(SB_FileCopy_super, arg, unused);}
int SB_FileList         (const std::string& arg, int unused) {return call_super(SB_FileList_super, arg, unused);}
int SB_FileImport       (const std::string& arg, int unused) {return call_super(SB_FileImport_super, arg, unused);}
int SB_ReportExec       (const std::string& arg, int unused) {return call_super(SB_ReportExec_super, arg, unused);}
int SB_ReportEdit       (const std::string& arg, int unused) {return call_super(SB_ReportEdit_super, arg, unused);}
int SB_ReportPrompt     (const std::string& arg, int unused) {return call_super(SB_ReportPrompt_super, arg, unused);}
int SB_PrintObjDef      (const std::string& arg, int unused) {return call_super(SB_PrintObjDef_super, arg, unused);}
int SB_PrintCnf         (const std::string& arg, int unused) {return call_super(SB_PrintCnf_super, arg, unused);}
int SB_ViewPrintGr      (const std::string& arg, int unused) {return call_super(SB_ViewPrintGr_super, arg, unused);}
int SB_ViewPrintTbl     (const std::string& arg, int unused) {return call_super(SB_ViewPrintTbl_super, arg, unused);}
int SB_ViewByTbl        (const std::string& arg, int unused) {return call_super(SB_ViewByTbl_super, arg, unused);}
int SB_DataEdit         (const std::string& arg, int unused) {return call_super(SB_DataEdit_super, arg, unused);}
int SB_DataEditScroll   (const std::string& arg, int unused) {return call_super(SB_DataEditScroll_super, arg, unused);}
int SB_DataSearch       (const std::string& arg, int unused) {return call_super(SB_DataSearch_super, arg, unused);}
int SB_DataDuplicate    (const std::string& arg, int unused) {return call_super(SB_DataDuplicate_super, arg, unused);}
int SB_DataList         (const std::string& arg, int unused) {return call_super(SB_DataList_super, arg, unused);}
int SB_DataCompare      (const std::string& arg, int unused) {return call_super(SB_DataCompare_super, arg, unused);}
int SB_DataCalcLst      (const std::string& arg, int unused) {return call_super(SB_DataCalcLst_super, arg, unused);}
int SB_DataListSort     (const std::string& arg, int unused) {return call_super(SB_DataListSort_super, arg, unused);}
int SB_DataEditGraph    (const std::string& arg, int unused) {return call_super(SB_DataEditGraph_super, arg, unused);}
int SB_DataEditCnf      (const std::string& arg, int unused) {return call_super(SB_DataEditCnf_super, arg, unused);}
int SB_DataScan         (const std::string& arg, int unused) {return call_super(SB_DataScan_super, arg, unused);}
int SB_StatUnitRoot     (const std::string& arg, int unused) {return call_super(SB_StatUnitRoot_super, arg, unused);}
int SB_WsLoad           (const std::string& arg, int unused) {return call_super(SB_WsLoad_super, arg, unused);}
int SB_WsSave           (const std::string& arg, int unused) {return call_super(SB_WsSave_super, arg, unused);}
int SB_WsMerge          (const std::string& arg, int unused) {return call_super(SB_WsMerge_super, arg, unused);}
int SB_WsDescr          (const std::string& arg, int unused) {return call_super(SB_WsDescr_super, arg, unused);}
int SB_WsCopy           (const std::string& arg, int unused) {return call_super(SB_WsCopy_super, arg, unused);}
int SB_WsClear          (const std::string& arg, int unused) {return call_super(SB_WsClear_super, arg, unused);}
int SB_WsSample         (const std::string& arg, int unused) {return call_super(SB_WsSample_super, arg, unused);}
int SB_WsExtrapolate    (const std::string& arg, int unused) {return call_super(SB_WsExtrapolate_super, arg, unused);}
int SB_WsHtoL           (const std::string& arg, int unused) {return call_super(SB_WsHtoL_super, arg, unused);}
int SB_WsLtoH           (const std::string& arg, int unused) {return call_super(SB_WsLtoH_super, arg, unused);}
int SB_WsSeasonAdj      (const std::string& arg, int unused) {return call_super(SB_WsSeasonAdj_super, arg, unused);}
int SB_WsTrend          (const std::string& arg, int unused) {return call_super(SB_WsTrend_super, arg, unused);}
int SB_WsAggregate      (const std::string& arg, int unused) {return call_super(SB_WsAggregate_super, arg, unused);}
int SB_ModelSimulate    (const std::string& arg, int unused) {return call_super(SB_ModelSimulate_super, arg, unused);}
int SB_ModelCompile     (const std::string& arg, int unused) {return call_super(SB_ModelCompile_super, arg, unused);}
int SB_IdtExecute       (const std::string& arg, int unused) {return call_super(SB_IdtExecute_super, arg, unused);}
int SB_EqsEstimate      (const std::string& arg, int unused) {return call_super(SB_EqsEstimate_super, arg, unused);}
int SB_Dir              (const std::string& arg, int unused) {return call_super(SB_Dir_super, arg, unused);}
int SB_XodeRuleImport   (const std::string& arg, int unused) {return call_super(SB_XodeRuleImport_super, arg, unused);}

//int B_DataDisplayGraph  () {if(B_DataDisplayGraph_super ) return(*B_DataDisplayGraph_super)(); else return 0;}
//int B_DataPrintGraph    () {if(B_DataPrintGraph_super   ) return(*B_DataPrintGraph_super  )(); else return 0;}  
int B_WindowMinimize    (const std::string& unused, int unused2) {if(B_WindowMinimize_super   ) return(*B_WindowMinimize_super  )(); else return 0;}  
int B_WindowMaximize    (const std::string& unused, int unused2) {if(B_WindowMaximize_super   ) return(*B_WindowMaximize_super  )(); else return 0;}

int B_ScrollVarW(const std::string& arg, int unused) {return call_super(B_ScrollVarW_super, arg);}
int B_ScrollVarN(const std::string& arg, int unused) {return call_super(B_ScrollVarN_super, arg);}
int B_ScrollVarM(const std::string& arg, int unused) {return call_super(B_ScrollVarM_super, arg);}
int B_ScrollVarS(const std::string& arg, int unused) {return call_super(B_ScrollVarS_super, arg);}
int B_ScrollSclW(const std::string& arg, int unused) {return call_super(B_ScrollSclW_super, arg);}
int B_ScrollSclN(const std::string& arg, int unused) {return call_super(B_ScrollSclN_super, arg);}
int B_ScrollTblW(const std::string& arg, int unused) {return call_super(B_ScrollTblW_super, arg);}
int B_ScrollVTW (const std::string& arg, int unused) {return call_super(B_ScrollVTW_super, arg);}
int B_ScrollVTW0(const std::string& arg, int unused) {return call_super(B_ScrollVTW0_super, arg);}
int B_ScrollVTN (const std::string& arg, int unused) {return call_super(B_ScrollVTN_super, arg);}

int ODE_scroll  (KDB* kdb, char **lst)             {if(ODE_scroll_super         ) return(*ODE_scroll_super        )(kdb, lst);         else return 0;}
int T_view_tbl  (Table *tbl, char *smpl, char* name) {if(T_view_tbl_super         ) return(*T_view_tbl_super        )(tbl, smpl, name);  else return 0;}
// int T_view_tbl  (char* name, char *smpl, char** vars_names) {if(T_view_tbl_super         ) return(*T_view_tbl_super        )(name, smpl, vars_names);  else return 0;} // TEMP version for IODE-QT