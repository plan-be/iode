#pragma once

#include "api/constants.h"
#include "api/lec/lec.h"
#include "api/objs/tables.h"
#include "api/report/reports.h"

#ifdef __cplusplus
extern "C" {
#endif

/* b_rep_utils.c */
extern int RP_alloc_ptrs();
extern char *RP_alloc(int size);
extern int RP_find_ptr(char *ptr);
extern int RP_free(char *ptr);
extern void RP_free_bufs();
extern char *RP_stracpy(char *ptr);
extern unsigned char **RP_vtoms(unsigned char* str, unsigned char *seps);
extern unsigned char **RP_vtom(unsigned char* str, int sep);
extern int RP_free_tbl(unsigned char **tbl);
extern int RP_tbl_size(unsigned char **tbl);
extern int RP_is_cmd(char *line);
extern U_ch **SCR_vtomsq(char* str, char* seps, int quote);

/* b_rep_debug.c */
extern void RP_debug(char* txt);

/* b_rep_engine.c */
extern REPFILE *RP_create_repfile(char *filename, unsigned char **tbl);
extern int RP_free_repfile(REPFILE *rf);
extern unsigned char **RP_read_file(char* filename);
extern char* RP_read_multiline(REPFILE* rf);
extern int RP_readline(REPFILE* rf, char** line, int mode);
extern int RP_chk_ignore(char* line);
extern int RP_splitline(char* text, char* cmd, char** arg, int lg);
extern int RP_find_fn(char* name, int* type, int fs);
extern int RP_exec_fn(char* name, char* arg, int fs);
extern int RP_err_dump(char* name, char* arg);
extern char *RP_extract(char* buf, int* i, int ch);
extern char *RP_gmacro(char* str);
extern char *RP_gcmd(char* str);
extern int RP_evaltime();
extern double RP_evallec(char* lec);
extern int RP_fmt(char* buf, char* format, double value);
extern int RP_eval(char** res, char* farg);
extern int RP_add(char** line, int* lg, int* j, char* res);
extern int RP_expand(char** line, char* buf);
extern int RP_ReportExec_tbl(REPFILE *rf);
extern int RP_ReportExec_1(char* file);
extern int B_ReportExec(char* arg);
extern int B_ReportLine(char* line, int cleanup);

/* b_rep_defs.c */
extern int RP_macro_createdb();
extern int RP_macro_deletedb();
extern int RP_define_1(char *name, char *macro);
extern int RP_define(char* arg);
extern char* RP_get_macro_ptr(char* macro_name);
extern int RP_undef_1(char *name);
extern int RP_undef(char *arg);
extern int RP_define_calcdepth(char *name);
extern int RP_define_save(char *name);
extern int RP_define_restore(char *name);
extern int RP_define_save_list(char **list);
extern int RP_define_restore_list(char **list);

/* b_rep_cmds.c */
extern int RP_vseps(char* seps);
extern int RP_repeatstring(char* buf);
extern int RP_repeat(char* buf);
extern int RP_onerror_1(char* arg);
extern int RP_onerror(char* arg);
extern int RP_abort(char* arg);
extern int RP_quitode(char* arg);
extern int RP_return(char* arg);
extern int RP_label(char* arg);
extern int RP_goto_label(char *command, char *parm);
extern int RP_goto(char* arg);
extern int RP_message(char* arg);
extern int RP_warning(char* arg);
extern int RP_silent(char* arg);
extern int RP_beep(char* arg);
extern int RP_ask(char* arg);
extern int B_ReportPrompt(char* arg);
extern int RP_setdebug(char* arg);
extern int RP_setindent(char* arg);
extern int RP_setmultiline(char* arg);
extern int RP_noparsing(char* arg);
extern int RP_shift_args(char* arg);
extern int RP_chdir(char* arg);
extern int RP_rmdir(char* arg);
extern int RP_mkdir(char* arg);
extern int RP_settime(char* arg);
extern int RP_incrtime(char* arg);
extern int RP_system(char* arg);
extern int B_shellexec(char *arg);
extern int B_Sleep(char* arg);
extern int B_GraphDefault(char* type);

/* b_rep_fns.c */ 
extern U_ch *RPF_IodeVersion();
extern U_ch *RPF_take(U_ch** args);
extern U_ch *RPF_drop(U_ch** args);
extern U_ch *RPF_replace(U_ch** args);
extern U_ch *RPF_equal(U_ch** args);
extern U_ch *RPF_upper(U_ch** args);
extern U_ch *RPF_lower(U_ch** args);
extern U_ch *RPF_sqz(U_ch** args);
extern U_ch *RPF_strip(U_ch** args);
extern U_ch *RPF_fmtint(U_ch** args);
extern U_ch *RPF_ansi(U_ch** args);
extern U_ch *RPF_count(U_ch** args);
extern U_ch *RPF_index(U_ch** args);
extern U_ch *RPF_void(U_ch **args);
extern U_ch *RPF_date(U_ch** args);
extern U_ch *RPF_time(U_ch** args);
extern U_ch *RPF_month(U_ch** args);
extern U_ch *RPF_sstderr(U_ch** args);
extern U_ch *RPF_srelax(U_ch** args);
extern U_ch *RPF_ttitle(U_ch** args);
extern U_ch *RPF_cvalue(U_ch** args);
extern U_ch *RPF_vvalue(U_ch** args);
extern U_ch *RPF_lvalue(U_ch** args);
extern U_ch *RPF_ivalue(U_ch** args);
extern U_ch *RPF_evalue(U_ch** args);
extern U_ch *RPF_eqsample(U_ch** args);
extern U_ch *RPF_eqsamplefromto(U_ch** args, int fromto);
extern U_ch *RPF_eqsamplefrom(U_ch **args);
extern U_ch *RPF_eqsampleto(U_ch **args);
extern U_ch *RPF_eqlhsrhs(U_ch** args, int lhsrhs);
extern U_ch *RPF_eqlhs(U_ch **args);
extern U_ch *RPF_eqrhs(U_ch **args);
extern U_ch *RPF_sample(U_ch** args);
extern int RPF_vsliste1(CLEC* cl, U_ch*** tbl, int* nb, int type);
extern U_ch *RPF_vsliste(U_ch** args, int type);
extern U_ch **RPF_unique(U_ch** tbl);
extern U_ch *RPF_vliste(U_ch** args);
extern U_ch *RPF_sliste(U_ch** args);
extern U_ch *RPF_expand(U_ch** args, int type);
extern U_ch *RPF_cexpand(U_ch **args);
extern U_ch *RPF_eexpand(U_ch **args);
extern U_ch *RPF_iexpand(U_ch **args);
extern U_ch *RPF_lexpand(U_ch **args);
extern U_ch *RPF_sexpand(U_ch **args);
extern U_ch *RPF_texpand(U_ch **args);
extern U_ch *RPF_vexpand(U_ch **args);

extern int RPF_CalcPeriod(U_ch** args);
extern U_ch *RPF_SimMaxit();
extern U_ch *RPF_SimEps();
extern U_ch *RPF_SimRelax();
extern U_ch* RPF_SimSortNbPasses();
extern U_ch* RPF_SimSortAlgo();
extern U_ch* RPF_SimInitValues();

extern double RPF_SimNormReal(U_ch** args);
extern U_ch *RPF_SimNorm(U_ch** args);

extern int RPF_SimNIterInt(U_ch** args);
extern U_ch *RPF_SimNIter(U_ch** args);

extern int RPF_SimCpuInt(U_ch** args);
extern U_ch* RPF_SimCpu(U_ch** args);

extern U_ch* RPF_SimCpuSCC();
extern U_ch* RPF_SimCpuSort();

extern U_ch *RPF_vtake(U_ch** args);
extern U_ch *RPF_vdrop(U_ch** args);
extern U_ch *RPF_vcount(U_ch** args);
extern U_ch *RPF_memory(U_ch** args);
extern U_ch *RPF_ChronoReset();
extern U_ch *RPF_ChronoGet();
extern U_ch *RPF_fappend(U_ch** args);
extern U_ch *RPF_fdelete(U_ch** args);
extern U_ch *RPF_getdir();
extern U_ch *RPF_chdir(U_ch **args);
extern U_ch *RPF_mkdir(U_ch **args);
extern U_ch *RPF_rmdir(U_ch **args);

/* b_rep_foreach.C */ 
int RP_foreach(char* arg);
int RP_foreach_break(char *name);
int RP_foreach_next(char* arg);

/* b_rep_proc.c */
extern void RP_proc_free_all();
extern int RP_procdef(char* arg);
extern int RP_procexec(char* arg);

/* b_rep_super.c - function pointer that can be superseeded */
extern int (*SB_FileDelete_super    )();
extern int (*SB_FileRename_super    )();
extern int (*SB_FileCopy_super      )();
extern int (*SB_FileList_super      )();
extern int (*SB_FileImport_super    )();
extern int (*SB_ReportExec_super    )();
extern int (*SB_ReportEdit_super    )();
extern int (*SB_ReportPrompt_super  )();
extern int (*SB_PrintObjDef_super   )();
extern int (*SB_PrintCnf_super      )();
extern int (*SB_ViewPrintGr_super   )();
extern int (*SB_ViewPrintTbl_super  )();
extern int (*SB_ViewByTbl_super     )();
extern int (*SB_DataEdit_super      )();
extern int (*SB_DataEditScroll_super)();
extern int (*SB_DataSearch_super    )();
extern int (*SB_DataDuplicate_super )();
extern int (*SB_DataList_super      )();
extern int (*SB_DataCompare_super   )();
extern int (*SB_DataCalcLst_super   )();
extern int (*SB_DataListSort_super  )();
extern int (*SB_DataEditGraph_super )();
extern int (*SB_DataEditCnf_super   )();
extern int (*SB_DataScan_super      )();
extern int (*SB_StatUnitRoot_super  )();
extern int (*SB_WsLoad_super        )();
extern int (*SB_WsSave_super        )();
extern int (*SB_WsMerge_super       )();
extern int (*SB_WsDescr_super       )();
extern int (*SB_WsCopy_super        )();
extern int (*SB_WsClear_super       )();
extern int (*SB_WsSample_super      )();
extern int (*SB_WsExtrapolate_super )();
extern int (*SB_WsHtoL_super        )();
extern int (*SB_WsLtoH_super        )();
extern int (*SB_WsSeasonAdj_super   )();
extern int (*SB_WsTrend_super       )();
extern int (*SB_WsAggregate_super   )();
extern int (*SB_ModelSimulate_super )();
extern int (*SB_ModelCompile_super  )();
extern int (*SB_IdtExecute_super    )();
extern int (*SB_EqsEstimate_super   )();
extern int (*SB_Dir_super           )();
extern int (*SB_XodeRuleImport_super)();

//extern int (*B_DataDisplayGraph_super)();
//extern int (*B_DataPrintGraph_super  )();   
extern int (*B_WindowMinimize_super  )();   
extern int (*B_WindowMaximize_super  )(); 

extern int (*B_ScrollVarW_super)(char *arg);
extern int (*B_ScrollVarN_super)(char *arg);
extern int (*B_ScrollVarM_super)(char *arg);
extern int (*B_ScrollVarS_super)(char *arg);
extern int (*B_ScrollSclW_super)(char *arg);
extern int (*B_ScrollSclN_super)(char *arg);
extern int (*B_ScrollTblW_super)(char *arg);
extern int (*B_ScrollVTW_super )(char *arg);
extern int (*B_ScrollVTW0_super)(char *arg);
extern int (*B_ScrollVTN_super )(char *arg);

extern int (*ODE_scroll_super) (KDB *kdb, char **lst);
extern int (*T_view_tbl_super) (TBL *tbl, char *smpl, char* name);
// extern int (*T_view_tbl_super) (char* name, char *smpl, char** vars_names); // TEMP version for IODE-QT

/* b_rep_super.c - function declarations */
extern int SB_FileDelete       ();
extern int SB_FileRename       ();
extern int SB_FileCopy         ();
extern int SB_FileList         ();
extern int SB_FileImport       ();
extern int SB_ReportExec       ();
extern int SB_ReportEdit       ();
extern int SB_ReportPrompt     ();
extern int SB_PrintObjDef      ();
extern int SB_PrintCnf         ();
extern int SB_ViewPrintGr      ();
extern int SB_ViewPrintTbl     ();
extern int SB_ViewByTbl        ();
extern int SB_DataEdit         ();
extern int SB_DataEditScroll   ();
extern int SB_DataSearch       ();
extern int SB_DataDuplicate    ();
extern int SB_DataList         ();
extern int SB_DataCompare      ();
extern int SB_DataCalcLst      ();
extern int SB_DataListSort     ();
extern int SB_DataEditGraph    ();
extern int SB_DataEditCnf      ();
extern int SB_DataScan         ();
extern int SB_StatUnitRoot     ();
extern int SB_WsLoad           ();
extern int SB_WsSave           ();
extern int SB_WsMerge          ();
extern int SB_WsDescr          ();
extern int SB_WsCopy           ();
extern int SB_WsClear          ();
extern int SB_WsSample         ();
extern int SB_WsExtrapolate    ();
extern int SB_WsHtoL           ();
extern int SB_WsLtoH           ();
extern int SB_WsSeasonAdj      ();
extern int SB_WsTrend          ();
extern int SB_WsAggregate      ();
extern int SB_ModelSimulate    ();
extern int SB_ModelCompile     ();
extern int SB_IdtExecute       ();
extern int SB_EqsEstimate      ();
extern int SB_Dir              ();
extern int SB_XodeRuleImport   ();
//extern int B_DataDisplayGraph  ();
//extern int B_DataPrintGraph    ();  
extern int B_WindowMinimize    ();  
extern int B_WindowMaximize    ();

extern int B_ScrollVarW(char* arg);
extern int B_ScrollVarN(char* arg);
extern int B_ScrollVarM(char* arg);
extern int B_ScrollVarS(char* arg);
extern int B_ScrollSclW(char* arg);
extern int B_ScrollSclN(char* arg);
extern int B_ScrollTblW(char* arg);
extern int B_ScrollVTW (char* arg);
extern int B_ScrollVTW0(char* arg);
extern int B_ScrollVTN (char* arg);

extern int ODE_scroll(KDB *kdb, char **lst);
extern int T_view_tbl(TBL *tbl, char *smpl, char* name);
// extern int T_view_tbl(char* name, char *smpl, char** vars_names); // Temp version for IODE-QT

#ifdef __cplusplus
}
#endif
