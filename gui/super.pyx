import sys
import warnings
import iode
from PySide6.QtWidgets import QMainWindow, QMessageBox
from main_window import MainWindow
from main_widgets.tab_widget.tab_widget import IodeTabWidget
from iode_objs.misc.unit_root import UnitRootDialog
from iode_objs.misc.dyn_adjustment import DynAdjustmentDialog
from tabs.iode_objs.tab_computed_table import ComputedTableNumericalDialog

from gui.super cimport TBL
from gui.super cimport KT_WS
from gui.super cimport K_end_ws
from gui.super cimport K_find
from gui.super cimport K_add
from gui.super cimport K_del

from gui.super cimport kerror_super
from gui.super cimport kwarning_super
from gui.super cimport kmsg_super
from gui.super cimport kconfirm_super
from gui.super cimport kpanic_super
from gui.super cimport SB_PrintObjDef_super
from gui.super cimport SB_ViewPrintGr_super
from gui.super cimport SB_ViewPrintTbl_super
from gui.super cimport SB_ViewByTbl_super
from gui.super cimport SB_DataSearch_super
from gui.super cimport SB_DataDuplicate_super
from gui.super cimport SB_DataList_super
from gui.super cimport SB_DataCompare_super
from gui.super cimport SB_DataCalcLst_super
from gui.super cimport SB_DataListSort_super
from gui.super cimport SB_DataEditGraph_super
from gui.super cimport SB_DataScan_super
from gui.super cimport SB_StatUnitRoot_super
from gui.super cimport SB_WsLoad_super
from gui.super cimport SB_WsSave_super
from gui.super cimport SB_WsMerge_super
from gui.super cimport SB_WsDescr_super
from gui.super cimport SB_WsCopy_super
from gui.super cimport SB_WsClear_super
from gui.super cimport SB_WsSample_super
from gui.super cimport SB_WsExtrapolate_super
from gui.super cimport SB_WsHtoL_super
from gui.super cimport SB_WsLtoH_super
from gui.super cimport SB_WsSeasonAdj_super
from gui.super cimport SB_WsTrend_super
from gui.super cimport SB_ModelSimulate_super
from gui.super cimport SB_ModelCompile_super
from gui.super cimport SB_IdtExecute_super
from gui.super cimport SB_EqsEstimate_super
from gui.super cimport SB_XodeRuleImport_super
from gui.super cimport T_view_tbl_super


main_window: MainWindow = None


cdef void c_gui_error_super(const int level, const char* msg):
	cdef bytes b_msg = msg
	if level > 0:
		QMessageBox.critical(None, "ERROR", "Error: An unexpected error occurred!\n"
							 "Will exit the program now")
		if main_window is None:
			QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		else:
			main_window.close()
		sys.exit(level)
	else:
		QMessageBox.critical( None, "ERROR", b_msg.decode())

cdef void c_gui_warning_super(const char* msg):
	cdef bytes b_msg = msg
	QMessageBox.information(None, "INFORMATION", b_msg.decode())

cdef void c_gui_msg_super(const char* msg):
	cdef bytes b_msg = msg
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
	else:
		main_window.display_output(b_msg.decode())

cdef int c_gui_confirm_super(const char* msg):
	cdef bytes b_msg = msg
	answer: QMessageBox.StandardButton = QMessageBox.question(None, "CONFIRM", b_msg.decode())
	return 1 if answer == QMessageBox.StandardButton.Ok else 0

cdef void c_gui_panic_super():
	msg: str = "Attempting to save Data saved in ws.*files\n"
	msg += "To restart iode with the last data, type :\n"
	msg += "    iode -ws"
	QMessageBox.critical(None, "SWAP PANIC", msg)
	K_end_ws(1)
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
	else:
		main_window.close()
	sys.exit(msg)


# ========================================================================= #
# See C API -> report/engine/b_rep_super.c and report/engine/b_rep_syntax.c #
# ========================================================================= #

cdef int c_gui_PrintObjDef_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_print_objects_definitions_dialog()
		return 0

cdef int c_gui_ViewPrintGr_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_graphs_tables_dialog()
		return 0

cdef int c_gui_ViewPrintTbl_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_print_tables_dialog()
		return 0

cdef int c_gui_ViewByTbl_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_edit_tables_variables_dialog()
		return 0

cdef int c_gui_DataSearch_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_search_text_dialog()
		return 0

cdef int c_gui_DataDuplicate_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_duplicate_objects_dialog()
		return 0

cdef int c_gui_DataList_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_file_contents_dialog()
		return 0

cdef int c_gui_DataCompare_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_file_compare_dialog()
		return 0

cdef int c_gui_DataCalcLst_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:	
		main_window.open_list_calculus_dialog()
		return 0

cdef int c_gui_DataListSort_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_sort_and_calculus_dialog()
		return 0

cdef int c_gui_DataEditGraph_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_graphs_variables_dialog()
		return 0

cdef int c_gui_DataScan_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_scan_objects_dialog()
		return 0

cdef int c_gui_StatUnitRoot_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		dialog = UnitRootDialog(main_window)
		dialog.open()
		return 0

cdef int c_gui_WsLoad_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:	
		main_window.open_project()
		return 0

cdef int c_gui_WsSave_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:	
		main_window.save_all_tabs()
		return 0

cdef int c_gui_WsMerge_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_merge_into_workspace_dialog()
		return 0

cdef int c_gui_WsDescr_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:	
		main_window.open_change_workspace_descriptions_dialog()
		return 0

cdef int c_gui_WsCopy_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_copy_into_workspace_dialog()
		return 0

cdef int c_gui_WsClear_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.clear_workspace()
		return 0

cdef int c_gui_WsSample_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_change_variables_sample_dialog()
		return 0

cdef int c_gui_WsExtrapolate_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:	
		main_window.open_extrapolate_variables_dialog()
		return 0

cdef int c_gui_WsHtoL_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:	
		main_window.open_high_to_low_dialog()
		return 0

cdef int c_gui_WsLtoH_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:	
		main_window.open_low_to_high_dialog()
		return 0

cdef int c_gui_WsSeasonAdj_super():
	lec = ""
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		dialog = DynAdjustmentDialog(lec, main_window)
		dialog.open()
		return 0

cdef int c_gui_WsTrend_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_trend_correction_dialog()
		return 0

cdef int c_gui_ModelSimulate_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_compute_simulation_dialog()
		return 0

cdef int c_gui_ModelCompile_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_compute_model_dialog()
		return 0

cdef int c_gui_IdtExecute_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:	
		main_window.open_compute_identities_dialog()
		return 0

cdef int c_gui_EqsEstimate_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		tab_widget: IodeTabWidget = main_window.tab_widget
		tab_widget.open_edit_dialog(iode.IodeType.EQUATIONS)
		return 0

cdef int c_gui_XodeRuleImport_super():
	if main_window is None:
		QMessageBox.critical(None, "ERROR", "The Graphical User Interface for IODE is not initialized yet.")
		return 1
	else:
		main_window.open_import_variables_dialog()
		return 0

cdef int c_gui_view_tbl(TBL* tbl, char* smpl, char* name):
	cdef int success = 0
	cdef int pos = K_find(KT_WS, name)
	cdef bytes b_table_name = name
	cdef bytes b_generalized_sample = smpl

	table_name: str = b_table_name.decode()
	generalized_sample: str = b_generalized_sample.decode()
	nb_decimals: int = 6

	table_found: bool = pos >= 0
	if not table_found:
		# temporary add the passed IODE table to the global database of tables
		pos = K_add(KT_WS, name, tbl)
		if pos < 0:
			QMessageBox.warning(None, "WARNING", "Could not compute the table '" + table_name + 
								"' with sample '" + generalized_sample + "'")
			return 1
	
	try:
		view = ComputedTableNumericalDialog(table_name, generalized_sample, 4, parent=main_window)
		view.open()
	except Exception as e:
		QMessageBox.critical(None, "ERROR", "An error occurred while trying to view the table '" +
											table_name + "' with sample '" + generalized_sample + 
											"':\n" + str(e))
		success = 1

	if not table_found:
		# remove the temporary added IODE table from the global database
		K_del(KT_WS, pos)
	
	return success


# ============================================================================== #

def gui_assign_super_API(arg_main_window: MainWindow):
	global main_window

	kerror_super = c_gui_error_super
	kwarning_super = c_gui_warning_super
	kmsg_super = c_gui_msg_super
	kconfirm_super = c_gui_confirm_super
	kpanic_super = c_gui_panic_super

	SB_PrintObjDef_super    = c_gui_PrintObjDef_super
	SB_ViewPrintGr_super    = c_gui_ViewPrintGr_super
	SB_ViewPrintTbl_super   = c_gui_ViewPrintTbl_super
	SB_ViewByTbl_super      = c_gui_ViewByTbl_super
	SB_DataSearch_super     = c_gui_DataSearch_super
	SB_DataDuplicate_super  = c_gui_DataDuplicate_super
	SB_DataList_super       = c_gui_DataList_super
	SB_DataCompare_super    = c_gui_DataCompare_super
	SB_DataCalcLst_super    = c_gui_DataCalcLst_super
	SB_DataListSort_super   = c_gui_DataListSort_super
	SB_DataEditGraph_super  = c_gui_DataEditGraph_super
	SB_DataScan_super       = c_gui_DataScan_super
	SB_StatUnitRoot_super   = c_gui_StatUnitRoot_super
	SB_WsLoad_super         = c_gui_WsLoad_super
	SB_WsSave_super         = c_gui_WsSave_super
	SB_WsMerge_super        = c_gui_WsMerge_super
	SB_WsDescr_super        = c_gui_WsDescr_super
	SB_WsCopy_super         = c_gui_WsCopy_super
	SB_WsClear_super        = c_gui_WsClear_super
	SB_WsSample_super       = c_gui_WsSample_super
	SB_WsExtrapolate_super  = c_gui_WsExtrapolate_super
	SB_WsHtoL_super         = c_gui_WsHtoL_super
	SB_WsLtoH_super         = c_gui_WsLtoH_super
	SB_WsSeasonAdj_super    = c_gui_WsSeasonAdj_super
	SB_WsTrend_super        = c_gui_WsTrend_super
	SB_ModelSimulate_super  = c_gui_ModelSimulate_super
	SB_ModelCompile_super   = c_gui_ModelCompile_super
	SB_IdtExecute_super     = c_gui_IdtExecute_super
	SB_EqsEstimate_super    = c_gui_EqsEstimate_super
	SB_XodeRuleImport_super = c_gui_XodeRuleImport_super

	T_view_tbl_super        = c_gui_view_tbl

	if not isinstance(arg_main_window, MainWindow):
		QMessageBox.critical(None, "ERROR", "The main window is not initialized yet.")
	else:
		main_window = arg_main_window
