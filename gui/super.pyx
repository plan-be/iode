import sys
import warnings
import iode
from PySide6.QtWidgets import QMainWindow, QMessageBox
from iode_gui.main_window import MainWindow
from iode_gui.main_widgets.tab_widget.tab_widget import IodeTabWidget
from iode_gui.iode_objs.misc.unit_root import UnitRootDialog
from iode_gui.iode_objs.misc.dyn_adjustment import DynAdjustmentDialog
from iode_gui.tabs.iode_objs.tab_computed_table import ComputedTableNumericalDialog

from gui.super cimport TBL
from gui.super cimport KT_WS
from gui.super cimport K_end_ws
from gui.super cimport K_find
from gui.super cimport K_add
from gui.super cimport K_del

cdef extern from "py_super.h":
	cdef int error_super_GUI(const int level, const char* msg) except? -1
	cdef void warning_super_GUI(const char* msg) noexcept
	cdef void msg_super_GUI(const char* msg) noexcept
	cdef int confirm_super_GUI(const char* msg) noexcept
	cdef void panic_super_GUI() except *

	cdef int PrintObjDef_super_GUI() except? -1
	cdef int ViewPrintGr_super_GUI() except? -1
	cdef int ViewPrintTbl_super_GUI() except? -1
	cdef int ViewByTbl_super_GUI() except? -1
	cdef int DataSearch_super_GUI() except? -1
	cdef int DataDuplicate_super_GUI() except? -1
	cdef int DataList_super_GUI() except? -1
	cdef int DataCompare_super_GUI() except? -1
	cdef int DataCalcLst_super_GUI() except? -1
	cdef int DataListSort_super_GUI() except? -1
	cdef int DataEditGraph_super_GUI() except? -1
	cdef int DataScan_super_GUI() except? -1
	cdef int StatUnitRoot_super_GUI() except? -1
	cdef int WsLoad_super_GUI() except? -1
	cdef int WsSave_super_GUI() except? -1
	cdef int WsMerge_super_GUI() except? -1
	cdef int WsDescr_super_GUI() except? -1
	cdef int WsCopy_super_GUI() except? -1
	cdef int WsClear_super_GUI() except? -1
	cdef int WsSample_super_GUI() except? -1
	cdef int WsExtrapolate_super_GUI() except? -1
	cdef int WsHtoL_super_GUI() except? -1
	cdef int WsLtoH_super_GUI() except? -1
	cdef int WsSeasonAdj_super_GUI() except? -1
	cdef int WsTrend_super_GUI() except? -1
	cdef int ModelSimulate_super_GUI() except? -1
	cdef int ModelCompile_super_GUI() except? -1
	cdef int IdtExecute_super_GUI() except? -1
	cdef int EqsEstimate_super_GUI() except? -1
	cdef int XodeRuleImport_super_GUI() except? -1
	cdef int view_tbl_super_GUI(TBL* tbl, char* smpl, char* name) except? -1

cdef extern from "py_super.cpp": 
	cdef void c_gui_assign_super() noexcept


main_window: MainWindow = None


# ============================================================================================ #
# https://cython.readthedocs.io/en/stable/src/userguide/language_basics.html#function-pointers #
# ============================================================================================ #


cdef int error_super_GUI(const int level, const char* msg):
	cdef bytes b_msg = msg
	if level > 0:
		QMessageBox.critical(None, "ERROR", "Error: An unexpected error occurred!\n"
							 "Will exit the program now")
		main_window.close()
		sys.exit(level)
		return -1
	else:
		QMessageBox.critical( None, "ERROR", b_msg.decode())
		return 0

cdef void warning_super_GUI(const char* msg) noexcept:
	cdef bytes b_msg = msg
	QMessageBox.warning(None, "WARNING", b_msg.decode())

cdef void msg_super_GUI(const char* msg) noexcept:
	cdef bytes b_msg = msg
	main_window.display_output(b_msg.decode())

cdef int confirm_super_GUI(const char* msg) noexcept:
	cdef bytes b_msg = msg
	answer: QMessageBox.StandardButton = QMessageBox.question(None, "CONFIRM", b_msg.decode())
	return 1 if answer == QMessageBox.StandardButton.Ok else 0

cdef void panic_super_GUI():
	msg: str = "Attempting to save Data saved in ws.*files\n"
	msg += "To restart iode with the last data, type :\n"
	msg += "    iode -ws"
	QMessageBox.critical(None, "SWAP PANIC", msg)
	K_end_ws(1)
	main_window.close()
	sys.exit(msg)


# ========================================================================= #
# See C API -> report/engine/b_rep_super.c and report/engine/b_rep_syntax.c #
# ========================================================================= #

cdef int PrintObjDef_super_GUI():
	main_window.open_print_objects_definitions_dialog()
	return 0

cdef int ViewPrintGr_super_GUI():
	main_window.open_graphs_tables_dialog()
	return 0

cdef int ViewPrintTbl_super_GUI():
	main_window.open_print_tables_dialog()
	return 0

cdef int ViewByTbl_super_GUI():
	main_window.open_edit_tables_variables_dialog()
	return 0

cdef int DataSearch_super_GUI():
	main_window.open_search_text_dialog()
	return 0

cdef int DataDuplicate_super_GUI():
	main_window.open_duplicate_objects_dialog()
	return 0

cdef int DataList_super_GUI():
	main_window.open_file_contents_dialog()
	return 0

cdef int DataCompare_super_GUI():
	main_window.open_file_compare_dialog()
	return 0

cdef int DataCalcLst_super_GUI():
	main_window.open_list_calculus_dialog()
	return 0

cdef int DataListSort_super_GUI():
	main_window.open_sort_and_calculus_dialog()
	return 0

cdef int DataEditGraph_super_GUI():
	main_window.open_graphs_variables_dialog()
	return 0

cdef int DataScan_super_GUI():
	main_window.open_scan_objects_dialog()
	return 0

cdef int StatUnitRoot_super_GUI():
	dialog = UnitRootDialog(main_window)
	dialog.open()
	return 0

cdef int WsLoad_super_GUI():
	main_window.open_project()
	return 0

cdef int WsSave_super_GUI():
	main_window.save_all_tabs()
	return 0

cdef int WsMerge_super_GUI():
	main_window.open_merge_into_workspace_dialog()
	return 0

cdef int WsDescr_super_GUI():
	main_window.open_change_workspace_descriptions_dialog()
	return 0

cdef int WsCopy_super_GUI():
	main_window.open_copy_into_workspace_dialog()
	return 0

cdef int WsClear_super_GUI():
	main_window.clear_workspace()
	return 0

cdef int WsSample_super_GUI():
	main_window.open_change_variables_sample_dialog()
	return 0

cdef int WsExtrapolate_super_GUI():
	main_window.open_extrapolate_variables_dialog()
	return 0

cdef int WsHtoL_super_GUI():
	main_window.open_high_to_low_dialog()
	return 0

cdef int WsLtoH_super_GUI():
	main_window.open_low_to_high_dialog()
	return 0

cdef int WsSeasonAdj_super_GUI():
	lec = ""
	dialog = DynAdjustmentDialog(lec, main_window)
	dialog.open()
	return 0

cdef int WsTrend_super_GUI():
	main_window.open_trend_correction_dialog()
	return 0

cdef int ModelSimulate_super_GUI():
	main_window.open_compute_simulation_dialog()
	return 0

cdef int ModelCompile_super_GUI():
	main_window.open_compute_model_dialog()
	return 0

cdef int IdtExecute_super_GUI():
	main_window.open_compute_identities_dialog()
	return 0

cdef int EqsEstimate_super_GUI():
	tab_widget: IodeTabWidget = main_window.tab_widget
	tab_widget.open_edit_dialog(iode.IodeType.EQUATIONS)
	return 0

cdef int XodeRuleImport_super_GUI():
	main_window.open_import_variables_dialog()
	return 0

cdef int view_tbl_super_GUI(TBL* tbl, char* smpl, char* name):
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


# NOTE: must call AFTER importing the 'iode' package since it will run python_assign_super() 
#       which assign the super C function kerror_super, kwarning_super and kmsg_super
def gui_assign_super(arg_main_window: MainWindow):
	global main_window

	if not isinstance(arg_main_window, MainWindow):
		QMessageBox.critical(None, "ERROR", "The main window is not initialized yet.")
		return
	
	main_window = arg_main_window
	c_gui_assign_super()
