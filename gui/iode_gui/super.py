import sys
from iode import (IodeType, comments, equations, identities, lists, scalars, 
				  tables, variables)
from iode.util import register_super_function, c_api_error_as_exception
from iode.super import skip_pause, skip_msg_box

from PySide6.QtWidgets import QMainWindow, QMessageBox

from iode_gui.main_window import MainWindow
from iode_gui.main_widgets.tab_widget.tab_widget import IodeTabWidget
from iode_gui.iode_objs.misc.unit_root import UnitRootDialog
from iode_gui.iode_objs.misc.dyn_adjustment import DynAdjustmentDialog
from iode_gui.tabs.iode_objs.tab_computed_table import ComputedTableDialog


__all__ = ['gui_assign_super']

main_window: MainWindow = None

def gui_assign_super(arg_main_window: MainWindow):
	global main_window

	if not isinstance(arg_main_window, MainWindow):
		QMessageBox.critical(None, "ERROR", "The main window is not initialized yet.")
		return
	
	main_window = arg_main_window
	c_api_error_as_exception(True)

@register_super_function('error')
def error_super_GUI(level: int, msg: str) -> int:
	if level > 0:
		QMessageBox.critical(None, "ERROR", "Error: An unexpected error occurred!\n"
							 "Will exit the program now")
		if main_window is not None:
			main_window.close()
		sys.exit(level)
		return -1
	else:
		main_window.display_output(f"ERROR -> {msg}")
		QMessageBox.critical(None, "ERROR", msg)
		return 0
	
@register_super_function('warning')
def warning_super_GUI(msg: str):
	main_window.display_output(f"WARNING -> {msg}")
	QMessageBox.warning(None, "WARNING", msg)

@register_super_function('message')
def msg_super_GUI(msg: str):
	if main_window is not None:
		main_window.display_output(msg)
	else:
		print(msg)

@register_super_function('confirm')
def confirm_super_GUI(msg: str) -> int:
	answer: QMessageBox.StandardButton = QMessageBox.question(None, "CONFIRM", msg)
	return 1 if answer == QMessageBox.StandardButton.Ok else 0

@register_super_function('panic')
def panic_super_GUI():
	msg: str = "Attempting to save Data saved in ws.*files\n"
	comments.save('ws')
	equations.save('ws')
	identities.save('ws')
	lists.save('ws')
	scalars.save('ws')
	tables.save('ws')
	variables.save('ws')

	msg += "To restart iode with the last data, type :\n"
	msg += "    iode -ws"
	QMessageBox.critical(None, "SWAP PANIC", msg)
	main_window.close()
	sys.exit(msg)

@register_super_function('pause')
def pause_super_GUI():
	QMessageBox.information(None, "PAUSE", "Click OK to continue")

@register_super_function('msgbox')
def msgbox_super_GUI(title: str, msg: str) -> int:
	QMessageBox.information(None, title, msg)
	return 1

# ========================================================================= #
# See C API -> report/engine/b_rep_super.c and report/engine/b_rep_syntax.c #
# ========================================================================= #

def check_main_window_exists(func):
	def wrapper(*args, **kwargs):
		if main_window is not None:
			return func(*args, **kwargs)
		else:
			name = func.__name__.replace('_super_GUI', '')
			QMessageBox.critical(None, "ERROR", f"Cannot run '{name}'. The graphical " 
						         "user interface has not been initialized yet.")
	return wrapper

@register_super_function('PrintObjDef')
@check_main_window_exists
def PrintObjDef_super_GUI() -> int:
	main_window.open_print_objects_definitions_dialog()
	return 0

@register_super_function('ViewPrintGr')
@check_main_window_exists
def ViewPrintGr_super_GUI() -> int:
	main_window.open_graphs_tables_dialog()
	return 0

@register_super_function('ViewPrintTbl')
@check_main_window_exists
def ViewPrintTbl_super_GUI() -> int:
	main_window.open_print_tables_dialog()
	return 0

@register_super_function('ViewByTbl')
@check_main_window_exists
def ViewByTbl_super_GUI() -> int:
	main_window.open_edit_tables_variables_dialog()
	return 0

@register_super_function('DataSearch')
@check_main_window_exists
def DataSearch_super_GUI() -> int:
	main_window.open_search_text_dialog()
	return 0

@register_super_function('DataDuplicate')
@check_main_window_exists
def DataDuplicate_super_GUI() -> int:
	main_window.open_duplicate_objects_dialog()
	return 0

@register_super_function('DataList')
@check_main_window_exists
def DataList_super_GUI() -> int:
	main_window.open_file_contents_dialog()
	return 0

@register_super_function('DataCompare')
@check_main_window_exists
def DataCompare_super_GUI() -> int:
	main_window.open_file_compare_dialog()
	return 0

@register_super_function('DataCalcLst')
@check_main_window_exists
def DataCalcLst_super_GUI() -> int:
	main_window.open_list_calculus_dialog()
	return 0

@register_super_function('DataListSort')
@check_main_window_exists
def DataListSort_super_GUI() -> int:
	main_window.open_sort_and_calculus_dialog()
	return 0

@register_super_function('DataEditGraph')
@check_main_window_exists
def DataEditGraph_super_GUI() -> int:
	main_window.open_graphs_variables_dialog()
	return 0

@register_super_function('DataScan')
@check_main_window_exists
def DataScan_super_GUI() -> int:
	main_window.open_scan_objects_dialog()
	return 0

@register_super_function('StatUnitRoot')
@check_main_window_exists
def StatUnitRoot_super_GUI() -> int:
	dialog = UnitRootDialog(main_window)
	dialog.open()
	return 0

@register_super_function('WsLoad')
@check_main_window_exists
def WsLoad_super_GUI() -> int:
	main_window.open_project()
	return 0

@register_super_function('WsSave')
@check_main_window_exists
def WsSave_super_GUI() -> int:
	main_window.save_all_tabs()
	return 0

@register_super_function('WsMerge')
@check_main_window_exists
def WsMerge_super_GUI() -> int:
	main_window.open_merge_into_workspace_dialog()
	return 0

@register_super_function('WsDescr')
@check_main_window_exists
def WsDescr_super_GUI() -> int:
	main_window.open_change_workspace_descriptions_dialog()
	return 0

@register_super_function('WsCopy')
@check_main_window_exists
def WsCopy_super_GUI() -> int:
	main_window.open_copy_into_workspace_dialog()
	return 0

@register_super_function('WsClear')
@check_main_window_exists 
def WsClear_super_GUI() -> int:
	main_window.clear_workspace()
	return 0

@register_super_function('WsSample')
@check_main_window_exists
def WsSample_super_GUI() -> int:
	main_window.open_change_variables_sample_dialog()
	return 0

@register_super_function('WsExtrapolate')
@check_main_window_exists
def WsExtrapolate_super_GUI() -> int:
	main_window.open_extrapolate_variables_dialog()
	return 0

@register_super_function('WsHtoL')
@check_main_window_exists
def WsHtoL_super_GUI() -> int:
	main_window.open_high_to_low_dialog()
	return 0

@register_super_function('WsLtoH')
@check_main_window_exists
def WsLtoH_super_GUI() -> int:
	main_window.open_low_to_high_dialog()
	return 0

@register_super_function('WsSeasonAdj')
@check_main_window_exists
def WsSeasonAdj_super_GUI() -> int:
	lec = ""
	dialog = DynAdjustmentDialog(lec, main_window)
	dialog.open()
	return 0

@register_super_function('WsTrend')
@check_main_window_exists
def WsTrend_super_GUI() -> int:
	main_window.open_trend_correction_dialog()
	return 0

@register_super_function('ModelSimulate')
@check_main_window_exists 
def ModelSimulate_super_GUI() -> int:
	main_window.open_compute_simulation_dialog()
	return 0

@register_super_function('ModelCompile')
@check_main_window_exists
def ModelCompile_super_GUI() -> int:
	main_window.open_compute_model_dialog()
	return 0

@register_super_function('IdtExecute')
@check_main_window_exists
def IdtExecute_super_GUI() -> int:
	main_window.open_compute_identities_dialog()
	return 0

@register_super_function('EqsEstimate')
@check_main_window_exists
def EqsEstimate_super_GUI() -> int:
	tab_widget: IodeTabWidget = main_window.tab_widget
	tab_widget.open_edit_dialog(IodeType.EQUATIONS)
	return 0

@register_super_function('XodeRuleImport')
@check_main_window_exists
def XodeRuleImport_super_GUI() -> int:
	main_window.open_import_variables_dialog()
	return 0

@register_super_function('ViewTable')
@check_main_window_exists
def ViewTable_super_GUI(table_name: str, generalized_sample: str, nb_decimals: int) -> int:
	if table_name not in tables:
		QMessageBox.warning(None, "WARNING", "Could not compute the table '" + table_name + 
							"' with sample '" + generalized_sample + "'")
		return 1
	
	try:
		view = ComputedTableDialog(table_name, generalized_sample, nb_decimals, parent=main_window)
		view.open()
		return 0
	except Exception as e:
		QMessageBox.critical(None, "ERROR", "An error occurred while trying to view the table '" +
											table_name + "' with sample '" + generalized_sample + 
											"':\n" + str(e))
		return 1
