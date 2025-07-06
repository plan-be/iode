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

#include "api/report/engine/engine.h"
#include <stdarg.h>

// GUI report functions (#fn_name)
// Prendre les fonctions 1 à 1 et checker sb_gui.c qui contient des fns inutiles

int (*SB_FileDelete_super    )();
int (*SB_FileRename_super    )();
int (*SB_FileCopy_super      )();
int (*SB_FileList_super      )();
int (*SB_FileImport_super    )();
int (*SB_ReportExec_super    )();
int (*SB_ReportEdit_super    )();
int (*SB_ReportPrompt_super  )();
int (*SB_PrintObjDef_super   )();
int (*SB_PrintCnf_super      )();
int (*SB_ViewPrintGr_super   )();
int (*SB_ViewPrintTbl_super  )();
int (*SB_ViewByTbl_super     )();
int (*SB_DataEdit_super      )();
int (*SB_DataEditScroll_super)();
int (*SB_DataSearch_super    )();
int (*SB_DataDuplicate_super )();
int (*SB_DataList_super      )();
int (*SB_DataCompare_super   )();
int (*SB_DataCalcLst_super   )();
int (*SB_DataListSort_super  )();
int (*SB_DataEditGraph_super )();
int (*SB_DataEditCnf_super   )();
int (*SB_DataScan_super      )();
int (*SB_StatUnitRoot_super  )();
int (*SB_WsLoad_super        )();
int (*SB_WsSave_super        )();
int (*SB_WsMerge_super       )();
int (*SB_WsDescr_super       )();
int (*SB_WsCopy_super        )();
int (*SB_WsClear_super       )();
int (*SB_WsSample_super      )();
int (*SB_WsExtrapolate_super )();
int (*SB_WsHtoL_super        )();
int (*SB_WsLtoH_super        )();
int (*SB_WsSeasonAdj_super   )();
int (*SB_WsTrend_super       )();
int (*SB_WsAggregate_super   )();
int (*SB_ModelSimulate_super )();
int (*SB_ModelCompile_super  )();
int (*SB_IdtExecute_super    )();
int (*SB_EqsEstimate_super   )();
int (*SB_Dir_super           )();
int (*SB_XodeRuleImport_super)();


// Non-GUI functions ($fn_name)
//int (*B_DataDisplayGraph_super)();
//int (*B_DataPrintGraph_super  )();   
int (*B_WindowMinimize_super  )();   
int (*B_WindowMaximize_super  )(); 

int (*B_ScrollVarW_super)(char *arg);
int (*B_ScrollVarN_super)(char *arg);
int (*B_ScrollVarM_super)(char *arg);
int (*B_ScrollVarS_super)(char *arg);
int (*B_ScrollSclW_super)(char *arg);
int (*B_ScrollSclN_super)(char *arg);
int (*B_ScrollTblW_super)(char *arg);
int (*B_ScrollVTW_super )(char *arg);
int (*B_ScrollVTW0_super)(char *arg);
int (*B_ScrollVTN_super )(char *arg);

/* b_view.c */
int (*ODE_scroll_super) (KDB *kdb, char **lst);
int (*T_view_tbl_super) (TBL *tbl, char *smpl, char* name);
// int (*T_view_tbl_super) (char* name, char *smpl, char** vars_names); // TEMP version for IODE-QT

// Default implementation 
int SB_FileDelete       () {if(SB_FileDelete_super) return((*SB_FileDelete_super)()); return(0);}   
int SB_FileRename       () {if(SB_FileRename_super) return((*SB_FileRename_super)()); return(0);}   
int SB_FileCopy         () {if(SB_FileCopy_super)   return((*SB_FileCopy_super)  ()); return(0);}     
int SB_FileList         () {if(SB_FileList_super)   return((*SB_FileList_super)  ()); return(0);}   
int SB_FileImport       () {if(SB_FileImport_super      ) return(*SB_FileImport_super     )(); else return(0);}
int SB_ReportExec       () {if(SB_ReportExec_super      ) return(*SB_ReportExec_super     )(); else return(0);}
int SB_ReportEdit       () {if(SB_ReportEdit_super      ) return(*SB_ReportEdit_super     )(); else return(0);}
int SB_ReportPrompt     () {if(SB_ReportPrompt_super    ) return(*SB_ReportPrompt_super   )(); else return(0);}
int SB_PrintObjDef      () {if(SB_PrintObjDef_super     ) return(*SB_PrintObjDef_super    )(); else return(0);}
int SB_PrintCnf         () {if(SB_PrintCnf_super        ) return(*SB_PrintCnf_super       )(); else return(0);}
int SB_ViewPrintGr      () {if(SB_ViewPrintGr_super     ) return(*SB_ViewPrintGr_super    )(); else return(0);}
int SB_ViewPrintTbl     () {if(SB_ViewPrintTbl_super    ) return(*SB_ViewPrintTbl_super   )(); else return(0);}
int SB_ViewByTbl        () {if(SB_ViewByTbl_super       ) return(*SB_ViewByTbl_super      )(); else return(0);}
int SB_DataEdit         () {if(SB_DataEdit_super        ) return(*SB_DataEdit_super       )(); else return(0);}
int SB_DataEditScroll   () {if(SB_DataEditScroll_super  ) return(*SB_DataEditScroll_super )(); else return(0);}
int SB_DataSearch       () {if(SB_DataSearch_super      ) return(*SB_DataSearch_super     )(); else return(0);}
int SB_DataDuplicate    () {if(SB_DataDuplicate_super   ) return(*SB_DataDuplicate_super  )(); else return(0);}
int SB_DataList         () {if(SB_DataList_super        ) return(*SB_DataList_super       )(); else return(0);}
int SB_DataCompare      () {if(SB_DataCompare_super     ) return(*SB_DataCompare_super    )(); else return(0);}
int SB_DataCalcLst      () {if(SB_DataCalcLst_super     ) return(*SB_DataCalcLst_super    )(); else return(0);}
int SB_DataListSort     () {if(SB_DataListSort_super    ) return(*SB_DataListSort_super   )(); else return(0);}
int SB_DataEditGraph    () {if(SB_DataEditGraph_super   ) return(*SB_DataEditGraph_super  )(); else return(0);}
int SB_DataEditCnf      () {if(SB_DataEditCnf_super     ) return(*SB_DataEditCnf_super    )(); else return(0);}
int SB_DataScan         () {if(SB_DataScan_super        ) return(*SB_DataScan_super       )(); else return(0);}
int SB_StatUnitRoot     () {if(SB_StatUnitRoot_super    ) return(*SB_StatUnitRoot_super   )(); else return(0);}
int SB_WsLoad           () {if(SB_WsLoad_super          ) return(*SB_WsLoad_super         )(); else return(0);}
int SB_WsSave           () {if(SB_WsSave_super          ) return(*SB_WsSave_super         )(); else return(0);}
int SB_WsMerge          () {if(SB_WsMerge_super         ) return(*SB_WsMerge_super        )(); else return(0);}
int SB_WsDescr          () {if(SB_WsDescr_super         ) return(*SB_WsDescr_super        )(); else return(0);}
int SB_WsCopy           () {if(SB_WsCopy_super          ) return(*SB_WsCopy_super         )(); else return(0);}
int SB_WsClear          () {if(SB_WsClear_super         ) return(*SB_WsClear_super        )(); else return(0);}
int SB_WsSample         () {if(SB_WsSample_super        ) return(*SB_WsSample_super       )(); else return(0);}
int SB_WsExtrapolate    () {if(SB_WsExtrapolate_super   ) return(*SB_WsExtrapolate_super  )(); else return(0);}
int SB_WsHtoL           () {if(SB_WsHtoL_super          ) return(*SB_WsHtoL_super         )(); else return(0);}
int SB_WsLtoH           () {if(SB_WsLtoH_super          ) return(*SB_WsLtoH_super         )(); else return(0);}
int SB_WsSeasonAdj      () {if(SB_WsSeasonAdj_super     ) return(*SB_WsSeasonAdj_super    )(); else return(0);}
int SB_WsTrend          () {if(SB_WsTrend_super         ) return(*SB_WsTrend_super        )(); else return(0);}
int SB_WsAggregate      () {if(SB_WsAggregate_super     ) return(*SB_WsAggregate_super    )(); else return(0);}
int SB_ModelSimulate    () {if(SB_ModelSimulate_super   ) return(*SB_ModelSimulate_super  )(); else return(0);}
int SB_ModelCompile     () {if(SB_ModelCompile_super    ) return(*SB_ModelCompile_super   )(); else return(0);}
int SB_IdtExecute       () {if(SB_IdtExecute_super      ) return(*SB_IdtExecute_super     )(); else return(0);}
int SB_EqsEstimate      () {if(SB_EqsEstimate_super     ) return(*SB_EqsEstimate_super    )(); else return(0);}
int SB_Dir              () {if(SB_Dir_super             ) return(*SB_Dir_super            )(); else return(0);}
int SB_XodeRuleImport   () {if(SB_XodeRuleImport_super  ) return(*SB_XodeRuleImport_super )(); else return(0);}

//int B_DataDisplayGraph  () {if(B_DataDisplayGraph_super ) return(*B_DataDisplayGraph_super)(); else return(0);}
//int B_DataPrintGraph    () {if(B_DataPrintGraph_super   ) return(*B_DataPrintGraph_super  )(); else return(0);}  
int B_WindowMinimize    () {if(B_WindowMinimize_super   ) return(*B_WindowMinimize_super  )(); else return(0);}  
int B_WindowMaximize    () {if(B_WindowMaximize_super   ) return(*B_WindowMaximize_super  )(); else return(0);}

int B_ScrollVarW(char* arg) {if(B_ScrollVarW_super       ) return(*B_ScrollVarW_super      )(arg); else return(0);}
int B_ScrollVarN(char* arg) {if(B_ScrollVarN_super       ) return(*B_ScrollVarN_super      )(arg); else return(0);}
int B_ScrollVarM(char* arg) {if(B_ScrollVarM_super       ) return(*B_ScrollVarM_super      )(arg); else return(0);}
int B_ScrollVarS(char* arg) {if(B_ScrollVarS_super       ) return(*B_ScrollVarS_super      )(arg); else return(0);}
int B_ScrollSclW(char* arg) {if(B_ScrollSclW_super       ) return(*B_ScrollSclW_super      )(arg); else return(0);}
int B_ScrollSclN(char* arg) {if(B_ScrollSclN_super       ) return(*B_ScrollSclN_super      )(arg); else return(0);}
int B_ScrollTblW(char* arg) {if(B_ScrollTblW_super       ) return(*B_ScrollTblW_super      )(arg); else return(0);}
int B_ScrollVTW (char* arg) {if(B_ScrollVTW_super        ) return(*B_ScrollVTW_super       )(arg); else return(0);}
int B_ScrollVTW0(char* arg) {if(B_ScrollVTW0_super       ) return(*B_ScrollVTW0_super      )(arg); else return(0);}
int B_ScrollVTN (char* arg) {if(B_ScrollVTN_super        ) return(*B_ScrollVTN_super       )(arg); else return(0);}

int ODE_scroll  (KDB *kdb, char **lst)             {if(ODE_scroll_super         ) return(*ODE_scroll_super        )(kdb, lst);         else return(0);}
int T_view_tbl  (TBL *tbl, char *smpl, char* name) {if(T_view_tbl_super         ) return(*T_view_tbl_super        )(tbl, smpl, name);  else return(0);}
// int T_view_tbl  (char* name, char *smpl, char** vars_names) {if(T_view_tbl_super         ) return(*T_view_tbl_super        )(name, smpl, vars_names);  else return(0);} // TEMP version for IODE-QT