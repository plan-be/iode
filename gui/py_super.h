#pragma once
#include "api/iode.h"


int error_super_GUI(const int level, const char* msg);

void warning_super_GUI(const char* msg);

void msg_super_GUI(const char* msg);

int confirm_super_GUI(const char* msg);

void panic_super_GUI();

// ========================================================================= //
// See C API -> report/engine/b_rep_super.c and report/engine/b_rep_syntax.c //
// ========================================================================= //

int PrintObjDef_super_GUI();

int ViewPrintGr_super_GUI();

int ViewPrintTbl_super_GUI();

int ViewByTbl_super_GUI();

int DataSearch_super_GUI();

int DataDuplicate_super_GUI();

int DataList_super_GUI();

int DataCompare_super_GUI();

int DataCalcLst_super_GUI();

int DataListSort_super_GUI();

int DataEditGraph_super_GUI();

int DataScan_super_GUI();

int StatUnitRoot_super_GUI();

int WsLoad_super_GUI();

int WsSave_super_GUI();

int WsMerge_super_GUI();

int WsDescr_super_GUI();

int WsCopy_super_GUI();

int WsClear_super_GUI();

int WsSample_super_GUI();

int WsExtrapolate_super_GUI();

int WsHtoL_super_GUI();

int WsLtoH_super_GUI();

int WsSeasonAdj_super_GUI();

int WsTrend_super_GUI();

int ModelSimulate_super_GUI();

int ModelCompile_super_GUI();

int IdtExecute_super_GUI();

int EqsEstimate_super_GUI();

int XodeRuleImport_super_GUI();

int view_tbl_super_GUI(TBL* tbl, char* smpl, char* name);

// ========================================= //

void c_gui_assign_super();
