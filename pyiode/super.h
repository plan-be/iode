#include "api/all.h"

int c_kerror_super(const int level, const char* msg);
void c_kwarning_super(const char* msg);
void c_kmsg_super(const char* msg);
int c_kconfirm_super(const char* msg);
void c_kpanic_super();
void c_kpause_super();
int c_kmsgbox_super(const unsigned char* title, const unsigned char* msg, const unsigned char** buts);

// ========================================================================= //
// See C API -> report/engine/b_rep_super.c and report/engine/b_rep_syntax.c //
// ========================================================================= //

int c_PrintObjDef_super();
int c_ViewPrintGr_super();
int c_ViewPrintTbl_super();
int c_ViewByTbl_super();
int c_DataSearch_super();
int c_DataDuplicate_super();
int c_DataList_super();
int c_DataCompare_super();
int c_DataCalcLst_super();
int c_DataListSort_super();
int c_DataEditGraph_super();
int c_DataScan_super();
int c_StatUnitRoot_super();
int c_WsLoad_super();
int c_WsSave_super();
int c_WsMerge_super();
int c_WsDescr_super();
int c_WsCopy_super();
int c_WsClear_super();
int c_WsSample_super();
int c_WsExtrapolate_super();
int c_WsHtoL_super();
int c_WsLtoH_super();
int c_WsSeasonAdj_super();
int c_WsTrend_super();
int c_ModelSimulate_super();
int c_ModelCompile_super();
int c_IdtExecute_super();
int c_EqsEstimate_super();
int c_XodeRuleImport_super();

int c_ViewTable_super(TBL* tbl, char* smpl, char* name);

// ========================================= //

int error_as_cpp_exception(const int level, const char* msg);

void _c_api_error_as_exception(const bool value);
void python_assign_super();
