#pragma once

#include "api/constants.h"
#include "api/lec/lec.h"
#include "api/objs/tables.h"
#include "api/report/reports.h"

inline KDB* RP_MACRO = nullptr;     // KDB containing the macros during a report execution


/* b_rep_utils.c */
int RP_alloc_ptrs();
char *RP_alloc(int size);
int RP_find_ptr(char *ptr);
int RP_free(char *ptr);
void RP_free_bufs();
char *RP_stracpy(char *ptr);
unsigned char **RP_vtoms(unsigned char* str, unsigned char *seps);
unsigned char **RP_vtom(unsigned char* str, int sep);
int RP_free_tbl(unsigned char **tbl);
int RP_tbl_size(unsigned char **tbl);
int RP_is_cmd(char *line);
U_ch **SCR_vtomsq(char* str, char* seps, int quote);

/* b_rep_debug.c */
void RP_debug(char* txt);

/* b_rep_engine.c */
REPFILE *RP_create_repfile(char *filename, unsigned char **tbl);
int RP_free_repfile(REPFILE *rf);
unsigned char **RP_read_file(char* filename);
char* RP_read_multiline(REPFILE* rf);
int RP_readline(REPFILE* rf, char** line, int mode);
int RP_chk_ignore(char* line);
int RP_splitline(char* text, char* cmd, char** arg, int lg);
BFNS* RP_find_fn(char* name, int* type, int fs);
int RP_exec_fn(char* name, char* arg, int fs);
int RP_err_dump(char* name, char* arg);
char *RP_extract(char* buf, int* i, int ch);
char *RP_gmacro(char* str);
char *RP_gcmd(char* str);
int RP_evaltime();
double RP_evallec(char* lec);
int RP_fmt(char* buf, char* format, double value);
int RP_eval(char** res, char* farg);
int RP_add(char** line, int* lg, int* j, char* res);
int RP_expand(char** line, char* buf);
int RP_ReportExec_tbl(REPFILE *rf);
int RP_ReportExec_1(char* file);
int B_ReportExec(char* arg, int unused=-1);
int B_ReportLine(char* line, int cleanup);

/* b_rep_defs.c */
int RP_macro_createdb();
int RP_macro_deletedb();
int RP_define_1(char *name, char *macro);
int RP_define(char* arg, int unused=-1);
char* RP_get_macro_ptr(char* macro_name);
int RP_undef_1(char *name);
int RP_undef(char *arg, int unused=-1);
int RP_define_calcdepth(char *name);
int RP_define_save(char *name);
int RP_define_restore(char *name);
int RP_define_save_list(char **list);
int RP_define_restore_list(char **list);

/* b_rep_cmds.c */
int RP_vseps(char* seps, int unused=-1);
int RP_repeatstring(char* buf, int unused=-1);
int RP_repeat(char* buf, int unused=-1);
int RP_onerror_1(char* arg);
int RP_onerror(char* arg, int unused=-1);
int RP_abort(char* arg, int unused=-1);
int RP_quitode(char* arg, int unused=-1);
int RP_return(char* arg, int unused=-1);
int RP_label(char* arg, int unused=-1);
int RP_goto_label(char *command, char *parm);
int RP_goto(char* arg, int unused=-1);
int RP_message(char* arg, int unused=-1);
int RP_warning(char* arg, int unused=-1);
int RP_silent(char* arg, int unused=-1);
int RP_beep(char* arg, int unused=-1);
int RP_ask(char* arg, int unused=-1);
int B_ReportPrompt(char* arg);
int RP_setdebug(char* arg, int unused=-1);
int RP_setindent(char* arg, int unused=-1);
int RP_setmultiline(char* arg, int unused=-1);
int RP_noparsing(char* arg, int unused=-1);
int RP_shift_args(char* arg, int unused=-1);
int RP_chdir(char* arg, int unused=-1);
int RP_rmdir(char* arg, int unused=-1);
int RP_mkdir(char* arg, int unused=-1);
int RP_settime(char* arg, int unused=-1);
int RP_incrtime(char* arg, int unused=-1);
int RP_system(char* arg, int unused=-1);
int B_shellexec(char *arg, int unused=-1);
int B_Sleep(char* arg, int unused=-1);
int B_GraphDefault(char* type, int unused=-1);

/* b_rep_fns.c */ 
U_ch *RPF_IodeVersion(U_ch** unused);
U_ch *RPF_take(U_ch** args);
U_ch *RPF_drop(U_ch** args);
U_ch *RPF_replace(U_ch** args);
U_ch *RPF_equal(U_ch** args);
U_ch *RPF_upper(U_ch** args);
U_ch *RPF_lower(U_ch** args);
U_ch *RPF_sqz(U_ch** args);
U_ch *RPF_strip(U_ch** args);
U_ch *RPF_fmtint(U_ch** args);
U_ch *RPF_ansi(U_ch** args);
U_ch *RPF_count(U_ch** args);
U_ch *RPF_index(U_ch** args);
U_ch *RPF_void(U_ch **args);
U_ch *RPF_date(U_ch** args);
U_ch *RPF_time(U_ch** args);
U_ch *RPF_month(U_ch** args);
U_ch *RPF_sstderr(U_ch** args);
U_ch *RPF_srelax(U_ch** args);
U_ch *RPF_ttitle(U_ch** args);
U_ch *RPF_cvalue(U_ch** args);
U_ch *RPF_vvalue(U_ch** args);
U_ch *RPF_lvalue(U_ch** args);
U_ch *RPF_ivalue(U_ch** args);
U_ch *RPF_evalue(U_ch** args);
U_ch *RPF_eqsample(U_ch** args);
U_ch *RPF_eqsamplefromto(U_ch** args, int fromto);
U_ch *RPF_eqsamplefrom(U_ch **args);
U_ch *RPF_eqsampleto(U_ch **args);
U_ch *RPF_eqlhsrhs(U_ch** args, int lhsrhs);
U_ch *RPF_eqlhs(U_ch **args);
U_ch *RPF_eqrhs(U_ch **args);
U_ch *RPF_sample(U_ch** args);
int RPF_vsliste1(CLEC* cl, U_ch*** tbl, int* nb, int type);
U_ch *RPF_vsliste(U_ch** args, int type);
U_ch **RPF_unique(U_ch** tbl);
U_ch *RPF_vliste(U_ch** args);
U_ch *RPF_sliste(U_ch** args);
U_ch *RPF_expand(U_ch** args, int type);
U_ch *RPF_cexpand(U_ch **args);
U_ch *RPF_eexpand(U_ch **args);
U_ch *RPF_iexpand(U_ch **args);
U_ch *RPF_lexpand(U_ch **args);
U_ch *RPF_sexpand(U_ch **args);
U_ch *RPF_texpand(U_ch **args);
U_ch *RPF_vexpand(U_ch **args);

int RPF_CalcPeriod(U_ch** args);
U_ch *RPF_SimMaxit(U_ch** unused);
U_ch *RPF_SimEps(U_ch** unused);
U_ch *RPF_SimRelax(U_ch** unused);
U_ch* RPF_SimSortNbPasses(U_ch** unused);
U_ch* RPF_SimSortAlgo(U_ch** unused);
U_ch* RPF_SimInitValues(U_ch** unused);

double RPF_SimNormReal(U_ch** args);
U_ch *RPF_SimNorm(U_ch** args);

int RPF_SimNIterInt(U_ch** args);
U_ch *RPF_SimNIter(U_ch** args);

int RPF_SimCpuInt(U_ch** args);
U_ch* RPF_SimCpu(U_ch** args);

U_ch* RPF_SimCpuSCC(U_ch** unused);
U_ch* RPF_SimCpuSort(U_ch** unused);

U_ch *RPF_vtake(U_ch** args);
U_ch *RPF_vdrop(U_ch** args);
U_ch *RPF_vcount(U_ch** args);
U_ch *RPF_memory(U_ch** args);
U_ch *RPF_ChronoReset(U_ch** unused);
U_ch *RPF_ChronoGet(U_ch** unused);
U_ch *RPF_fappend(U_ch** args);
U_ch *RPF_fdelete(U_ch** args);
U_ch *RPF_getdir(U_ch** unused);
U_ch *RPF_chdir(U_ch **args);
U_ch *RPF_mkdir(U_ch **args);
U_ch *RPF_rmdir(U_ch **args);

/* b_rep_foreach.C */ 
int RP_foreach(char* arg, int unused=-1);
int RP_foreach_break(char *name);
int RP_foreach_next(char* arg, int unused=-1);

/* b_rep_proc.c */
void RP_proc_free_all();
int RP_procdef(char* arg, int unused=-1);
int RP_procexec(char* arg, int unused=-1);

/* b_rep_super.c - function pointer that can be superseeded */
inline int (*SB_FileDelete_super    )(char* arg, int unused) = nullptr;
inline int (*SB_FileRename_super    )(char* arg, int unused) = nullptr;
inline int (*SB_FileCopy_super      )(char* arg, int unused) = nullptr;
inline int (*SB_FileList_super      )(char* arg, int unused) = nullptr;
inline int (*SB_FileImport_super    )(char* arg, int unused) = nullptr;
inline int (*SB_ReportExec_super    )(char* arg, int unused) = nullptr;
inline int (*SB_ReportEdit_super    )(char* arg, int unused) = nullptr;
inline int (*SB_ReportPrompt_super  )(char* arg, int unused) = nullptr;
inline int (*SB_PrintObjDef_super   )(char* arg, int unused) = nullptr;
inline int (*SB_PrintCnf_super      )(char* arg, int unused) = nullptr;
inline int (*SB_ViewPrintGr_super   )(char* arg, int unused) = nullptr;
inline int (*SB_ViewPrintTbl_super  )(char* arg, int unused) = nullptr;
inline int (*SB_ViewByTbl_super     )(char* arg, int unused) = nullptr;
inline int (*SB_DataEdit_super      )(char* arg, int unused) = nullptr;
inline int (*SB_DataEditScroll_super)(char* arg, int unused) = nullptr;
inline int (*SB_DataSearch_super    )(char* arg, int unused) = nullptr;
inline int (*SB_DataDuplicate_super )(char* arg, int unused) = nullptr;
inline int (*SB_DataList_super      )(char* arg, int unused) = nullptr;
inline int (*SB_DataCompare_super   )(char* arg, int unused) = nullptr;
inline int (*SB_DataCalcLst_super   )(char* arg, int unused) = nullptr;
inline int (*SB_DataListSort_super  )(char* arg, int unused) = nullptr;
inline int (*SB_DataEditGraph_super )(char* arg, int unused) = nullptr;
inline int (*SB_DataEditCnf_super   )(char* arg, int unused) = nullptr;
inline int (*SB_DataScan_super      )(char* arg, int unused) = nullptr;
inline int (*SB_StatUnitRoot_super  )(char* arg, int unused) = nullptr;
inline int (*SB_WsLoad_super        )(char* arg, int unused) = nullptr;
inline int (*SB_WsSave_super        )(char* arg, int unused) = nullptr;
inline int (*SB_WsMerge_super       )(char* arg, int unused) = nullptr;
inline int (*SB_WsDescr_super       )(char* arg, int unused) = nullptr;
inline int (*SB_WsCopy_super        )(char* arg, int unused) = nullptr;
inline int (*SB_WsClear_super       )(char* arg, int unused) = nullptr;
inline int (*SB_WsSample_super      )(char* arg, int unused) = nullptr;
inline int (*SB_WsExtrapolate_super )(char* arg, int unused) = nullptr;
inline int (*SB_WsHtoL_super        )(char* arg, int unused) = nullptr;
inline int (*SB_WsLtoH_super        )(char* arg, int unused) = nullptr;
inline int (*SB_WsSeasonAdj_super   )(char* arg, int unused) = nullptr;
inline int (*SB_WsTrend_super       )(char* arg, int unused) = nullptr;
inline int (*SB_WsAggregate_super   )(char* arg, int unused) = nullptr;
inline int (*SB_ModelSimulate_super )(char* arg, int unused) = nullptr;
inline int (*SB_ModelCompile_super  )(char* arg, int unused) = nullptr;
inline int (*SB_IdtExecute_super    )(char* arg, int unused) = nullptr;
inline int (*SB_EqsEstimate_super   )(char* arg, int unused) = nullptr;
inline int (*SB_Dir_super           )(char* arg, int unused) = nullptr;
inline int (*SB_XodeRuleImport_super)(char* arg, int unused) = nullptr;

//int (*B_DataDisplayGraph_super)();
//int (*B_DataPrintGraph_super  )();   
inline int (*B_WindowMinimize_super  )() = nullptr;
inline int (*B_WindowMaximize_super  )() = nullptr; 

inline int (*B_ScrollVarW_super)(char *arg) = nullptr;
inline int (*B_ScrollVarN_super)(char *arg) = nullptr;
inline int (*B_ScrollVarM_super)(char *arg) = nullptr;
inline int (*B_ScrollVarS_super)(char *arg) = nullptr;
inline int (*B_ScrollSclW_super)(char *arg) = nullptr;
inline int (*B_ScrollSclN_super)(char *arg) = nullptr;
inline int (*B_ScrollTblW_super)(char *arg) = nullptr;
inline int (*B_ScrollVTW_super )(char *arg) = nullptr;
inline int (*B_ScrollVTW0_super)(char *arg) = nullptr;
inline int (*B_ScrollVTN_super )(char *arg) = nullptr;

inline int (*ODE_scroll_super) (KDB *kdb, char **lst) = nullptr;
inline int (*T_view_tbl_super) (Table *tbl, char *smpl, char* name) = nullptr;
// inline int (*T_view_tbl_super) (char* name, char *smpl, char** vars_names) = nullptr; // TEMP version for IODE-QT

/* b_rep_super.c - function declarations */
int SB_FileDelete       (char* arg, int unused=-1);
int SB_FileRename       (char* arg, int unused=-1);
int SB_FileCopy         (char* arg, int unused=-1);
int SB_FileList         (char* arg, int unused=-1);
int SB_FileImport       (char* arg, int unused=-1);
int SB_ReportExec       (char* arg, int unused=-1);
int SB_ReportEdit       (char* arg, int unused=-1);
int SB_ReportPrompt     (char* arg, int unused=-1);
int SB_PrintObjDef      (char* arg, int unused=-1);
int SB_PrintCnf         (char* arg, int unused=-1);
int SB_ViewPrintGr      (char* arg, int unused=-1);
int SB_ViewPrintTbl     (char* arg, int unused=-1);
int SB_ViewByTbl        (char* arg, int unused=-1);
int SB_DataEdit         (char* arg, int unused=-1);
int SB_DataEditScroll   (char* arg, int unused=-1);
int SB_DataSearch       (char* arg, int unused=-1);
int SB_DataDuplicate    (char* arg, int unused=-1);
int SB_DataList         (char* arg, int unused=-1);
int SB_DataCompare      (char* arg, int unused=-1);
int SB_DataCalcLst      (char* arg, int unused=-1);
int SB_DataListSort     (char* arg, int unused=-1);
int SB_DataEditGraph    (char* arg, int unused=-1);
int SB_DataEditCnf      (char* arg, int unused=-1);
int SB_DataScan         (char* arg, int unused=-1);
int SB_StatUnitRoot     (char* arg, int unused=-1);
int SB_WsLoad           (char* arg, int unused=-1);
int SB_WsSave           (char* arg, int unused=-1);
int SB_WsMerge          (char* arg, int unused=-1);
int SB_WsDescr          (char* arg, int unused=-1);
int SB_WsCopy           (char* arg, int unused=-1);
int SB_WsClear          (char* arg, int unused=-1);
int SB_WsSample         (char* arg, int unused=-1);
int SB_WsExtrapolate    (char* arg, int unused=-1);
int SB_WsHtoL           (char* arg, int unused=-1);
int SB_WsLtoH           (char* arg, int unused=-1);
int SB_WsSeasonAdj      (char* arg, int unused=-1);
int SB_WsTrend          (char* arg, int unused=-1);
int SB_WsAggregate      (char* arg, int unused=-1);
int SB_ModelSimulate    (char* arg, int unused=-1);
int SB_ModelCompile     (char* arg, int unused=-1);
int SB_IdtExecute       (char* arg, int unused=-1);
int SB_EqsEstimate      (char* arg, int unused=-1);
int SB_Dir              (char* arg, int unused=-1);
int SB_XodeRuleImport   (char* arg, int unused=-1);
//int B_DataDisplayGraph  ();
//int B_DataPrintGraph    ();  
int B_WindowMinimize(char* unused, int unused2);  
int B_WindowMaximize(char* unused, int unused2);

int ODE_scroll(KDB *kdb, char **lst);
int T_view_tbl(Table *tbl, char *smpl, char* name);
// int T_view_tbl(char* name, char *smpl, char** vars_names); // Temp version for IODE-QT
