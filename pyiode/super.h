#include "api/all.h"

void c_kwarning_super(const char* msg);
void c_kmsg_super(const char* msg);
int c_kconfirm_super(const char* msg);
void c_kpanic_super();
void c_kpause_super();
int c_kmsgbox_super(const unsigned char* title, const unsigned char* msg, const unsigned char** buts);

// ========================================================================= //
// See C API -> report/engine/b_rep_super.c and report/engine/b_rep_syntax.c //
// ========================================================================= //

int c_PrintObjDef_super(char* arg, int unused);
int c_ViewPrintGr_super(char* arg, int unused);
int c_ViewPrintTbl_super(char* arg, int unused);
int c_ViewByTbl_super(char* arg, int unused);
int c_DataSearch_super(char* arg, int unused);
int c_DataDuplicate_super(char* arg, int unused);
int c_DataList_super(char* arg, int unused);
int c_DataCompare_super(char* arg, int unused);
int c_DataCalcLst_super(char* arg, int unused);
int c_DataListSort_super(char* arg, int unused);
int c_DataEditGraph_super(char* arg, int unused);
int c_DataScan_super(char* arg, int unused);
int c_StatUnitRoot_super(char* arg, int unused);
int c_WsLoad_super(char* arg, int unused);
int c_WsSave_super(char* arg, int unused);
int c_WsMerge_super(char* arg, int unused);
int c_WsDescr_super(char* arg, int unused);
int c_WsCopy_super(char* arg, int unused);
int c_WsClear_super(char* arg, int unused);
int c_WsSample_super(char* arg, int unused);
int c_WsExtrapolate_super(char* arg, int unused);
int c_WsHtoL_super(char* arg, int unused);
int c_WsLtoH_super(char* arg, int unused);
int c_WsSeasonAdj_super(char* arg, int unused);
int c_WsTrend_super(char* arg, int unused);
int c_ModelSimulate_super(char* arg, int unused);
int c_ModelCompile_super(char* arg, int unused);
int c_IdtExecute_super(char* arg, int unused);
int c_EqsEstimate_super(char* arg, int unused);
int c_XodeRuleImport_super(char* arg, int unused);

int c_ViewTable_super(Table* tbl, char* smpl, char* name);

// ========================================= //

int error_as_cpp_exception(const int level, const char* msg);
void python_assign_super();

bool contain_table(const std::string& name);
bool add_table(const std::string& name, Table* value);
bool remove_table(const std::string& name);
