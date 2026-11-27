#pragma once

#include "api/pch.h"
#include "api/lec/lec.h"
#include "api/objs/macros.h"
#include "api/objs/tables.h"
#include "api/report/reports.h"


/* b_rep_utils.c */
int RP_alloc_ptrs();
char *RP_alloc(int size);
int RP_free(char* ptr);
void RP_free_bufs();
int RP_is_cmd(const std::string& line);

/* b_rep_debug.c */
void RP_debug(const std::string& txt);

/* b_rep_engine.c */
std::vector<std::string> RP_read_file(const std::string& filename);
int RP_splitline(const std::string& text, std::string& cmd, std::string& arg, int lg);
BFNS* RP_find_fn(const std::string& name, int& type, int fs);
int RP_exec_fn(const std::string& name, const std::string& arg, int fs);
std::string RP_extract(const std::string& buf, int& i, int ch);
std::string RP_extractpar(const std::string& buf, int& i, const std::string& brackets);
std::string RP_gmacro(const std::string& str);
std::string RP_gcmd(const std::string& str);
int RP_evaltime();
double RP_evallec(const std::string& lec);
int RP_fmt(std::string& buf, const std::string& format, double value);
int RP_eval(std::string& res, const std::string& farg);
int RP_expand(std::string& line, const std::string& buf);
int RP_ReportExec_1(const std::string& file);
int B_ReportExec(const std::string& arg, int unused=-1);
int B_ReportLine(char* line, int cleanup);

/* b_rep_defs.c */
int RP_define_1(const std::string& name, const std::string& macro);
bool RP_macro_exists(const std::string& name);
std::string RP_get_macro(const std::string& name);
int RP_undef_1(const std::string& name);
int RP_define_calcdepth(const std::string& name);
int RP_define_save(const std::string& name);
int RP_define_restore(const std::string& name);
int RP_define_save_list(const std::vector<std::string>& list);
int RP_define_restore_list(const std::vector<std::string>& list);

int RP_define(const std::string& arg, int unused=-1);
int RP_undef(const std::string& arg, int unused=-1);

/* b_rep_cmds.c */
int RP_onerror_1(const std::string& arg);
int B_ReportPrompt(const std::string& arg);

int RP_vseps(const std::string& seps, int unused=-1);
int RP_repeatstring(const std::string& buf, int unused=-1);
int RP_repeat(const std::string& buf, int unused=-1);
int RP_onerror(const std::string& arg, int unused=-1);
int RP_abort(const std::string& arg, int unused=-1);
int RP_quitode(const std::string& arg, int unused=-1);
int RP_return(const std::string& arg, int unused=-1);
int RP_label(const std::string& arg, int unused=-1);
int RP_goto_label(char* command, char* parm);
int RP_goto(const std::string& arg, int unused=-1);
int RP_message(const std::string& arg, int unused=-1);
int RP_information(const std::string& arg, int unused=-1);
int RP_silent(const std::string& arg, int unused=-1);
int RP_beep(const std::string& arg, int unused=-1);
int RP_ask(const std::string& arg, int unused=-1);
int RP_setdebug(const std::string& arg, int unused=-1);
int RP_setindent(const std::string& arg, int unused=-1);
int RP_setmultiline(const std::string& arg, int unused=-1);
int RP_noparsing(const std::string& arg, int unused=-1);
int RP_shift_args(const std::string& arg, int unused=-1);
int RP_chdir(const std::string& arg, int unused=-1);
int RP_rmdir(const std::string& arg, int unused=-1);
int RP_mkdir(const std::string& arg, int unused=-1);
int RP_settime(const std::string& arg, int unused=-1);
int RP_incrtime(const std::string& arg, int unused=-1);
int RP_system(const std::string& arg, int unused=-1);
int B_shellexec(const std::string& arg, int unused=-1);
int B_Sleep(const std::string& arg, int unused=-1);
int B_GraphDefault(const std::string& type, int unused=-1);

/* b_rep_fns.c */ 
std::string RPF_IodeVersion(const std::vector<std::string>& args);
std::string RPF_take(const std::vector<std::string>& args);
std::string RPF_drop(const std::vector<std::string>& args);
std::string RPF_replace(const std::vector<std::string>& args);
std::string RPF_equal(const std::vector<std::string>& args);
std::string RPF_upper(const std::vector<std::string>& args);
std::string RPF_lower(const std::vector<std::string>& args);
std::string RPF_sqz(const std::vector<std::string>& args);
std::string RPF_strip(const std::vector<std::string>& args);
std::string RPF_fmtint(const std::vector<std::string>& args);
std::string RPF_ansi(const std::vector<std::string>& args);
std::string RPF_count(const std::vector<std::string>& args);
std::string RPF_index(const std::vector<std::string>& args);
std::string RPF_void(const std::vector<std::string>& args);
std::string RPF_date(const std::vector<std::string>& args);
std::string RPF_time(const std::vector<std::string>& args);
std::string RPF_month(const std::vector<std::string>& args);
std::string RPF_sstderr(const std::vector<std::string>& args);
std::string RPF_srelax(const std::vector<std::string>& args);
std::string RPF_ttitle(const std::vector<std::string>& args);
std::string RPF_cvalue(const std::vector<std::string>& args);
std::string RPF_vvalue(const std::vector<std::string>& args);
std::string RPF_lvalue(const std::vector<std::string>& args);
std::string RPF_ivalue(const std::vector<std::string>& args);
std::string RPF_evalue(const std::vector<std::string>& args);
std::string RPF_eqsample(const std::vector<std::string>& args);
std::string RPF_eqsamplefromto(const std::vector<std::string>& args, int fromto);
std::string RPF_eqsamplefrom(const std::vector<std::string>& args);
std::string RPF_eqsampleto(const std::vector<std::string>& args);
std::string RPF_eqlhsrhs(const std::vector<std::string>& args, int lhsrhs);
std::string RPF_eqlhs(const std::vector<std::string>& args);
std::string RPF_eqrhs(const std::vector<std::string>& args);
std::string RPF_sample(const std::vector<std::string>& args);
int RPF_vsliste1(const std::shared_ptr<CLEC> clec, std::vector<std::string>& tbl, const int type);
std::string RPF_vsliste(const std::vector<std::string>& args, int type);
std::vector<std::string> RPF_unique(const std::vector<std::string>& tbl);
std::string RPF_vliste(const std::vector<std::string>& args);
std::string RPF_sliste(const std::vector<std::string>& args);
std::string RPF_expand(const std::vector<std::string>& args, int type);
std::string RPF_cexpand(const std::vector<std::string>& args);
std::string RPF_eexpand(const std::vector<std::string>& args);
std::string RPF_iexpand(const std::vector<std::string>& args);
std::string RPF_lexpand(const std::vector<std::string>& args);
std::string RPF_sexpand(const std::vector<std::string>& args);
std::string RPF_texpand(const std::vector<std::string>& args);
std::string RPF_vexpand(const std::vector<std::string>& args);

int RPF_CalcPeriod(const std::vector<std::string>& args);
std::string RPF_SimMaxit(const std::vector<std::string>& args);
std::string RPF_SimEps(const std::vector<std::string>& args);
std::string RPF_SimRelax(const std::vector<std::string>& args);
std::string RPF_SimSortNbPasses(const std::vector<std::string>& args);
std::string RPF_SimSortAlgo(const std::vector<std::string>& args);
std::string RPF_SimInitValues(const std::vector<std::string>& args);

double RPF_SimNormReal(const std::vector<std::string>& args);
std::string RPF_SimNorm(const std::vector<std::string>& args);

int RPF_SimNIterInt(const std::vector<std::string>& args);
std::string RPF_SimNIter(const std::vector<std::string>& args);

int RPF_SimCpuInt(const std::vector<std::string>& args);
std::string RPF_SimCpu(const std::vector<std::string>& args);

std::string RPF_SimCpuSCC(const std::vector<std::string>& args);
std::string RPF_SimCpuSort(const std::vector<std::string>& args);

std::string RPF_vtake(const std::vector<std::string>& args);
std::string RPF_vdrop(const std::vector<std::string>& args);
std::string RPF_vcount(const std::vector<std::string>& args);
std::string RPF_memory(const std::vector<std::string>& args);
std::string RPF_ChronoReset(const std::vector<std::string>& args);
std::string RPF_ChronoGet(const std::vector<std::string>& args);
std::string RPF_fappend(const std::vector<std::string>& args);
std::string RPF_fdelete(const std::vector<std::string>& args);
std::string RPF_getdir(const std::vector<std::string>& args);
std::string RPF_chdir(const std::vector<std::string>& args);
std::string RPF_mkdir(const std::vector<std::string>& args);
std::string RPF_rmdir(const std::vector<std::string>& args);

/* b_rep_foreach.C */ 
int RP_foreach_break(char* name);
int RP_foreach(const std::string& arg, int unused=-1);
int RP_foreach_next(const std::string& arg, int unused=-1);

/* b_rep_proc.c */
void RP_proc_free_all();
int RP_procdef(const std::string& arg, int unused=-1);
int RP_procexec(const std::string& arg, int unused=-1);

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

inline int (*ODE_scroll_super) (KDB* kdb, char **lst) = nullptr;
inline int (*T_view_tbl_super) (Table *tbl, char *smpl, char* name) = nullptr;
// inline int (*T_view_tbl_super) (char* name, char *smpl, char** vars_names) = nullptr; // TEMP version for IODE-QT

/* b_rep_super.c - function declarations */
int SB_FileDelete       (const std::string& arg, int unused=-1);
int SB_FileRename       (const std::string& arg, int unused=-1);
int SB_FileCopy         (const std::string& arg, int unused=-1);
int SB_FileList         (const std::string& arg, int unused=-1);
int SB_FileImport       (const std::string& arg, int unused=-1);
int SB_ReportExec       (const std::string& arg, int unused=-1);
int SB_ReportEdit       (const std::string& arg, int unused=-1);
int SB_ReportPrompt     (const std::string& arg, int unused=-1);
int SB_PrintObjDef      (const std::string& arg, int unused=-1);
int SB_PrintCnf         (const std::string& arg, int unused=-1);
int SB_ViewPrintGr      (const std::string& arg, int unused=-1);
int SB_ViewPrintTbl     (const std::string& arg, int unused=-1);
int SB_ViewByTbl        (const std::string& arg, int unused=-1);
int SB_DataEdit         (const std::string& arg, int unused=-1);
int SB_DataEditScroll   (const std::string& arg, int unused=-1);
int SB_DataSearch       (const std::string& arg, int unused=-1);
int SB_DataDuplicate    (const std::string& arg, int unused=-1);
int SB_DataList         (const std::string& arg, int unused=-1);
int SB_DataCompare      (const std::string& arg, int unused=-1);
int SB_DataCalcLst      (const std::string& arg, int unused=-1);
int SB_DataListSort     (const std::string& arg, int unused=-1);
int SB_DataEditGraph    (const std::string& arg, int unused=-1);
int SB_DataEditCnf      (const std::string& arg, int unused=-1);
int SB_DataScan         (const std::string& arg, int unused=-1);
int SB_StatUnitRoot     (const std::string& arg, int unused=-1);
int SB_WsLoad           (const std::string& arg, int unused=-1);
int SB_WsSave           (const std::string& arg, int unused=-1);
int SB_WsMerge          (const std::string& arg, int unused=-1);
int SB_WsDescr          (const std::string& arg, int unused=-1);
int SB_WsCopy           (const std::string& arg, int unused=-1);
int SB_WsClear          (const std::string& arg, int unused=-1);
int SB_WsSample         (const std::string& arg, int unused=-1);
int SB_WsExtrapolate    (const std::string& arg, int unused=-1);
int SB_WsHtoL           (const std::string& arg, int unused=-1);
int SB_WsLtoH           (const std::string& arg, int unused=-1);
int SB_WsSeasonAdj      (const std::string& arg, int unused=-1);
int SB_WsTrend          (const std::string& arg, int unused=-1);
int SB_WsAggregate      (const std::string& arg, int unused=-1);
int SB_ModelSimulate    (const std::string& arg, int unused=-1);
int SB_ModelCompile     (const std::string& arg, int unused=-1);
int SB_IdtExecute       (const std::string& arg, int unused=-1);
int SB_EqsEstimate      (const std::string& arg, int unused=-1);
int SB_Dir              (const std::string& arg, int unused=-1);
int SB_XodeRuleImport   (const std::string& arg, int unused=-1);
int B_WindowMinimize    (const std::string& unused, int unused2);  
int B_WindowMaximize    (const std::string& unused, int unused2);

int ODE_scroll(KDB* kdb, char **lst);
int T_view_tbl(Table *tbl, char *smpl, char* name);
