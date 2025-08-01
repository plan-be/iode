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

// Default implementation 
int SB_FileDelete       (char* arg, int unused) {if(SB_FileDelete_super      ) return((*SB_FileDelete_super    )(arg, unused)); return(0);}   
int SB_FileRename       (char* arg, int unused) {if(SB_FileRename_super      ) return((*SB_FileRename_super    )(arg, unused)); return(0);}   
int SB_FileCopy         (char* arg, int unused) {if(SB_FileCopy_super        ) return((*SB_FileCopy_super      )(arg, unused)); return(0);}     
int SB_FileList         (char* arg, int unused) {if(SB_FileList_super        ) return((*SB_FileList_super      )(arg, unused)); return(0);}   
int SB_FileImport       (char* arg, int unused) {if(SB_FileImport_super      ) return(*SB_FileImport_super     )(arg, unused); else return(0);}
int SB_ReportExec       (char* arg, int unused) {if(SB_ReportExec_super      ) return(*SB_ReportExec_super     )(arg, unused); else return(0);}
int SB_ReportEdit       (char* arg, int unused) {if(SB_ReportEdit_super      ) return(*SB_ReportEdit_super     )(arg, unused); else return(0);}
int SB_ReportPrompt     (char* arg, int unused) {if(SB_ReportPrompt_super    ) return(*SB_ReportPrompt_super   )(arg, unused); else return(0);}
int SB_PrintObjDef      (char* arg, int unused) {if(SB_PrintObjDef_super     ) return(*SB_PrintObjDef_super    )(arg, unused); else return(0);}
int SB_PrintCnf         (char* arg, int unused) {if(SB_PrintCnf_super        ) return(*SB_PrintCnf_super       )(arg, unused); else return(0);}
int SB_ViewPrintGr      (char* arg, int unused) {if(SB_ViewPrintGr_super     ) return(*SB_ViewPrintGr_super    )(arg, unused); else return(0);}
int SB_ViewPrintTbl     (char* arg, int unused) {if(SB_ViewPrintTbl_super    ) return(*SB_ViewPrintTbl_super   )(arg, unused); else return(0);}
int SB_ViewByTbl        (char* arg, int unused) {if(SB_ViewByTbl_super       ) return(*SB_ViewByTbl_super      )(arg, unused); else return(0);}
int SB_DataEdit         (char* arg, int unused) {if(SB_DataEdit_super        ) return(*SB_DataEdit_super       )(arg, unused); else return(0);}
int SB_DataEditScroll   (char* arg, int unused) {if(SB_DataEditScroll_super  ) return(*SB_DataEditScroll_super )(arg, unused); else return(0);}
int SB_DataSearch       (char* arg, int unused) {if(SB_DataSearch_super      ) return(*SB_DataSearch_super     )(arg, unused); else return(0);}
int SB_DataDuplicate    (char* arg, int unused) {if(SB_DataDuplicate_super   ) return(*SB_DataDuplicate_super  )(arg, unused); else return(0);}
int SB_DataList         (char* arg, int unused) {if(SB_DataList_super        ) return(*SB_DataList_super       )(arg, unused); else return(0);}
int SB_DataCompare      (char* arg, int unused) {if(SB_DataCompare_super     ) return(*SB_DataCompare_super    )(arg, unused); else return(0);}
int SB_DataCalcLst      (char* arg, int unused) {if(SB_DataCalcLst_super     ) return(*SB_DataCalcLst_super    )(arg, unused); else return(0);}
int SB_DataListSort     (char* arg, int unused) {if(SB_DataListSort_super    ) return(*SB_DataListSort_super   )(arg, unused); else return(0);}
int SB_DataEditGraph    (char* arg, int unused) {if(SB_DataEditGraph_super   ) return(*SB_DataEditGraph_super  )(arg, unused); else return(0);}
int SB_DataEditCnf      (char* arg, int unused) {if(SB_DataEditCnf_super     ) return(*SB_DataEditCnf_super    )(arg, unused); else return(0);}
int SB_DataScan         (char* arg, int unused) {if(SB_DataScan_super        ) return(*SB_DataScan_super       )(arg, unused); else return(0);}
int SB_StatUnitRoot     (char* arg, int unused) {if(SB_StatUnitRoot_super    ) return(*SB_StatUnitRoot_super   )(arg, unused); else return(0);}
int SB_WsLoad           (char* arg, int unused) {if(SB_WsLoad_super          ) return(*SB_WsLoad_super         )(arg, unused); else return(0);}
int SB_WsSave           (char* arg, int unused) {if(SB_WsSave_super          ) return(*SB_WsSave_super         )(arg, unused); else return(0);}
int SB_WsMerge          (char* arg, int unused) {if(SB_WsMerge_super         ) return(*SB_WsMerge_super        )(arg, unused); else return(0);}
int SB_WsDescr          (char* arg, int unused) {if(SB_WsDescr_super         ) return(*SB_WsDescr_super        )(arg, unused); else return(0);}
int SB_WsCopy           (char* arg, int unused) {if(SB_WsCopy_super          ) return(*SB_WsCopy_super         )(arg, unused); else return(0);}
int SB_WsClear          (char* arg, int unused) {if(SB_WsClear_super         ) return(*SB_WsClear_super        )(arg, unused); else return(0);}
int SB_WsSample         (char* arg, int unused) {if(SB_WsSample_super        ) return(*SB_WsSample_super       )(arg, unused); else return(0);}
int SB_WsExtrapolate    (char* arg, int unused) {if(SB_WsExtrapolate_super   ) return(*SB_WsExtrapolate_super  )(arg, unused); else return(0);}
int SB_WsHtoL           (char* arg, int unused) {if(SB_WsHtoL_super          ) return(*SB_WsHtoL_super         )(arg, unused); else return(0);}
int SB_WsLtoH           (char* arg, int unused) {if(SB_WsLtoH_super          ) return(*SB_WsLtoH_super         )(arg, unused); else return(0);}
int SB_WsSeasonAdj      (char* arg, int unused) {if(SB_WsSeasonAdj_super     ) return(*SB_WsSeasonAdj_super    )(arg, unused); else return(0);}
int SB_WsTrend          (char* arg, int unused) {if(SB_WsTrend_super         ) return(*SB_WsTrend_super        )(arg, unused); else return(0);}
int SB_WsAggregate      (char* arg, int unused) {if(SB_WsAggregate_super     ) return(*SB_WsAggregate_super    )(arg, unused); else return(0);}
int SB_ModelSimulate    (char* arg, int unused) {if(SB_ModelSimulate_super   ) return(*SB_ModelSimulate_super  )(arg, unused); else return(0);}
int SB_ModelCompile     (char* arg, int unused) {if(SB_ModelCompile_super    ) return(*SB_ModelCompile_super   )(arg, unused); else return(0);}
int SB_IdtExecute       (char* arg, int unused) {if(SB_IdtExecute_super      ) return(*SB_IdtExecute_super     )(arg, unused); else return(0);}
int SB_EqsEstimate      (char* arg, int unused) {if(SB_EqsEstimate_super     ) return(*SB_EqsEstimate_super    )(arg, unused); else return(0);}
int SB_Dir              (char* arg, int unused) {if(SB_Dir_super             ) return(*SB_Dir_super            )(arg, unused); else return(0);}
int SB_XodeRuleImport   (char* arg, int unused) {if(SB_XodeRuleImport_super  ) return(*SB_XodeRuleImport_super )(arg, unused); else return(0);}

//int B_DataDisplayGraph  () {if(B_DataDisplayGraph_super ) return(*B_DataDisplayGraph_super)(); else return(0);}
//int B_DataPrintGraph    () {if(B_DataPrintGraph_super   ) return(*B_DataPrintGraph_super  )(); else return(0);}  
int B_WindowMinimize    (char* unused, int unused2) {if(B_WindowMinimize_super   ) return(*B_WindowMinimize_super  )(); else return(0);}  
int B_WindowMaximize    (char* unused, int unused2) {if(B_WindowMaximize_super   ) return(*B_WindowMaximize_super  )(); else return(0);}

int B_ScrollVarW(char* arg, int unused) {if(B_ScrollVarW_super       ) return(*B_ScrollVarW_super      )(arg); else return(0);}
int B_ScrollVarN(char* arg, int unused) {if(B_ScrollVarN_super       ) return(*B_ScrollVarN_super      )(arg); else return(0);}
int B_ScrollVarM(char* arg, int unused) {if(B_ScrollVarM_super       ) return(*B_ScrollVarM_super      )(arg); else return(0);}
int B_ScrollVarS(char* arg, int unused) {if(B_ScrollVarS_super       ) return(*B_ScrollVarS_super      )(arg); else return(0);}
int B_ScrollSclW(char* arg, int unused) {if(B_ScrollSclW_super       ) return(*B_ScrollSclW_super      )(arg); else return(0);}
int B_ScrollSclN(char* arg, int unused) {if(B_ScrollSclN_super       ) return(*B_ScrollSclN_super      )(arg); else return(0);}
int B_ScrollTblW(char* arg, int unused) {if(B_ScrollTblW_super       ) return(*B_ScrollTblW_super      )(arg); else return(0);}
int B_ScrollVTW (char* arg, int unused) {if(B_ScrollVTW_super        ) return(*B_ScrollVTW_super       )(arg); else return(0);}
int B_ScrollVTW0(char* arg, int unused) {if(B_ScrollVTW0_super       ) return(*B_ScrollVTW0_super      )(arg); else return(0);}
int B_ScrollVTN (char* arg, int unused) {if(B_ScrollVTN_super        ) return(*B_ScrollVTN_super       )(arg); else return(0);}

int ODE_scroll  (KDB *kdb, char **lst)             {if(ODE_scroll_super         ) return(*ODE_scroll_super        )(kdb, lst);         else return(0);}
int T_view_tbl  (TBL *tbl, char *smpl, char* name) {if(T_view_tbl_super         ) return(*T_view_tbl_super        )(tbl, smpl, name);  else return(0);}
// int T_view_tbl  (char* name, char *smpl, char** vars_names) {if(T_view_tbl_super         ) return(*T_view_tbl_super        )(name, smpl, vars_names);  else return(0);} // TEMP version for IODE-QT