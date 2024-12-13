#pragma once
#include "utils.h"
#include "main_window.h"
#include "main_widgets/tab_widget/tab_widget.h"

#include <QString>
#include <QMessageBox>
#include <QMainWindow>
#include <QStatusBar>


int gui_error_super(const int level, const char* msg);

void gui_warning_super(const char* msg);

void gui_msg_super(const char* msg);

int gui_confirm_super(const char* msg);

void gui_panic_super();

// ========================================================================= //
// See C API -> report/engine/b_rep_super.c and report/engine/b_rep_syntax.c //
// ========================================================================= //

int gui_PrintObjDef_super();

int gui_ViewPrintGr_super();

int gui_ViewPrintTbl_super();

int gui_ViewByTbl_super();

int gui_DataSearch_super();

int gui_DataDuplicate_super();

int gui_DataList_super();

int gui_DataCompare_super();

int gui_DataCalcLst_super();

int gui_DataListSort_super();

int gui_DataEditGraph_super();

int gui_DataScan_super();

int gui_StatUnitRoot_super();

int gui_WsLoad_super();

int gui_WsSave_super();

int gui_WsMerge_super();

int gui_WsDescr_super();

int gui_WsCopy_super();

int gui_WsClear_super();

int gui_WsSample_super();

int gui_WsExtrapolate_super();

int gui_WsHtoL_super();

int gui_WsLtoH_super();

int gui_WsSeasonAdj_super();

int gui_WsTrend_super();

int gui_ModelSimulate_super();

int gui_ModelCompile_super();

int gui_IdtExecute_super();

int gui_EqsEstimate_super();

int gui_XodeRuleImport_super();

// Note: argument name is the name of the table
int gui_view_tbl(char* name, char *smpl, char** vars_names);

// ========================================= //

void gui_assign_super_API();
