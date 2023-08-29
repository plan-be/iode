#pragma once
#include "super.h"
#include <QMessageBox>


void gui_error_super(int level, char* msg)
{
	QMessageBox::critical(nullptr, "ERROR", QString(msg));
	if (level > 0)
	{
		QMainWindow* main_window = static_cast<QMainWindow*>(get_main_window_ptr());
		main_window->close();
		exit(level);
	}
}

void gui_warning_super(char* msg)
{
	QMessageBox::information(nullptr, "INFORMATION", QString(msg));
}

void gui_msg_super(char* msg)
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->display_output(QString(msg));
}

int gui_confirm_super(char* msg)
{
	QMessageBox::StandardButton answer = QMessageBox::question(nullptr, "CONFIRM", QString(msg));
	if (answer == QMessageBox::Ok) return 1;
	else return 0;
}

void gui_panic_super()
{
	QString msg("Attempting to save Data saved in ws.*files\n");
	msg += "To restart iode with the last data, type :\n";
	msg += "    iode -ws";
	QMessageBox::critical(nullptr, "SWAP PANIC", msg);
	K_end_ws(1);
	QMainWindow* main_window = static_cast<QMainWindow*>(get_main_window_ptr());
	main_window->close();
	exit(2);
}

// ========================================================================= //
// See C API -> report/engine/b_rep_super.c and report/engine/b_rep_syntax.c //
// ========================================================================= //

int gui_PrintObjDef_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_print_objects_definitions_dialog();
	return 0;
}

int gui_ViewPrintGr_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_graphs_tables_dialog();
	return 0;
}

int gui_ViewPrintTbl_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_print_tables_dialog();
	return 0;
}

int gui_ViewByTbl_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_edit_tables_variables_dialog();
	return 0;
}

int gui_DataSearch_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_search_text_dialog();
	return 0;
}

int gui_DataDuplicate_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_duplicate_objects_dialog();
	return 0;
}

int gui_DataList_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_file_contents_dialog();
	return 0;
}

int gui_DataCompare_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_file_compare_dialog();
	return 0;
}

int gui_DataCalcLst_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_list_calculus_dialog();
	return 0;
}

int gui_DataListSort_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_sort_and_calculus_dialog();
	return 0;
}

int gui_DataEditGraph_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_graphs_variables_dialog();
	return 0;
}

int gui_DataScan_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_scan_objects_dialog();
	return 0;
}

int gui_StatUnitRoot_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	QIodeUnitRoot dialog(main_window);
	return dialog.exec();
}

int gui_WsLoad_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_project();
	return 0;
}

int gui_WsSave_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->saveAllTabs();
	return 0;
}

int gui_WsMerge_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_merge_into_workspace_dialog();
	return 0;
}

int gui_WsDescr_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_change_workspace_descriptions_dialog();
	return 0;
}

int gui_WsCopy_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_copy_into_workspace_dialog();
	return 0;
}

int gui_WsClear_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->clear_workspace();
	return 0;
}

int gui_WsSample_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_change_variables_sample_dialog();
	return 0;
}

int gui_WsExtrapolate_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_extrapolate_variables_dialog();
	return 0;
}

int gui_WsHtoL_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_high_to_low_dialog();
	return 0;
}

int gui_WsLtoH_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_low_to_high_dialog();
	return 0;
}

int gui_WsSeasonAdj_super()
{
	QString lec = "";
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	QIodeDynAdjustment dialog(lec, main_window);
	return dialog.exec();
}

int gui_WsTrend_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_trend_correction_dialog();
	return 0;
}

int gui_ModelSimulate_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_compute_simulation_dialog();
	return 0;
}

int gui_ModelCompile_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_compute_model_dialog();
	return 0;
}

int gui_IdtExecute_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_compute_identities_dialog();
	return 0;
}

int gui_EqsEstimate_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	EditEquationDialog dialog("", main_window);
	return dialog.exec();
}

int gui_XodeRuleImport_super()
{
	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	main_window->open_import_variables_dialog();
	return 0;
}

int gui_view_tbl(char* name, char *smpl, char** vars_names)
{
	QString tableName = QString(name);
	QString gsample = QString(smpl);
	int nb_decimals = 6;
	
	QString variables;
	for(int i = 0; i < SCR_tbl_size((unsigned char**) vars_names); i++)
		variables += QString(vars_names[i]) + ";";

	MainWindow* main_window = static_cast<MainWindow*>(get_main_window_ptr());
	GSampleNumericalDialog view(tableName, gsample, nb_decimals, variables, main_window);
	return view.exec();
}

// ============================================================================== //

void gui_assign_super_API()
{
	IODE_assign_super_API();
	kerror_super = gui_error_super;
	kwarning_super = gui_warning_super;
	kmsg_super = gui_msg_super;
	kconfirm_super = gui_confirm_super;
	kpanic_super = gui_panic_super;

	SB_PrintObjDef_super    = gui_PrintObjDef_super;
	SB_ViewPrintGr_super    = gui_ViewPrintGr_super;
	SB_ViewPrintTbl_super   = gui_ViewPrintTbl_super;
	SB_ViewByTbl_super      = gui_ViewByTbl_super;
	SB_DataSearch_super     = gui_DataSearch_super;
	SB_DataDuplicate_super  = gui_DataDuplicate_super;
	SB_DataList_super       = gui_DataList_super;
	SB_DataCompare_super    = gui_DataCompare_super;
	SB_DataCalcLst_super    = gui_DataCalcLst_super;
	SB_DataListSort_super   = gui_DataListSort_super;
	SB_DataEditGraph_super  = gui_DataEditGraph_super;
	SB_DataScan_super       = gui_DataScan_super;
	SB_StatUnitRoot_super   = gui_StatUnitRoot_super;
	SB_WsLoad_super         = gui_WsLoad_super;
	SB_WsSave_super         = gui_WsSave_super;
	SB_WsMerge_super        = gui_WsMerge_super;
	SB_WsDescr_super        = gui_WsDescr_super;
	SB_WsCopy_super         = gui_WsCopy_super;
	SB_WsClear_super        = gui_WsClear_super;
	SB_WsSample_super       = gui_WsSample_super;
	SB_WsExtrapolate_super  = gui_WsExtrapolate_super;
	SB_WsHtoL_super         = gui_WsHtoL_super;
	SB_WsLtoH_super         = gui_WsLtoH_super;
	SB_WsSeasonAdj_super    = gui_WsSeasonAdj_super;
	SB_WsTrend_super        = gui_WsTrend_super;
	SB_ModelSimulate_super  = gui_ModelSimulate_super;
	SB_ModelCompile_super   = gui_ModelCompile_super;
	SB_IdtExecute_super     = gui_IdtExecute_super;
	SB_EqsEstimate_super    = gui_EqsEstimate_super;
	SB_XodeRuleImport_super = gui_XodeRuleImport_super;

	T_view_tbl_super        = gui_view_tbl;
}
