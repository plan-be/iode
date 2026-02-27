from qtpy.QtCore import Slot
from qtpy.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_search_text import Ui_MenuDataSearchText

from iode import (IodeType, comments, equations, identities, 
                  lists, scalars, tables, variables)


class MenuDataSearchText(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuDataSearchText()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_iode_types = list(IodeType)
        v_iode_type_names = [iode_type.name for iode_type in self.v_iode_types]
        self.ui.comboBox_iode_types.addItems(v_iode_type_names)
        self.ui.comboBox_iode_types.setCurrentIndex(0)

        self.ui.checkBox_whole_word.setChecked(False)
        self.ui.checkBox_exact_case.setChecked(False)
        self.ui.checkBox_search_names.setChecked(True)
        self.ui.checkBox_search_formulas.setChecked(True)
        self.ui.checkBox_search_free_text.setChecked(True)

        self.ui.lineEdit_text.setPlaceholderText("Enter text to search for...")
        self.ui.lineEdit_save_list.setText("_RES")
        
        self.load_settings()

    @Slot()
    def search(self):
        try:
            i_iode_type = self.ui.comboBox_iode_types.currentIndex()
            iode_type: IodeType = self.v_iode_types[i_iode_type]
            pattern: str = self.ui.lineEdit_text.text().strip()
            word: bool = self.ui.checkBox_whole_word.isChecked()
            case_sensitive: bool = self.ui.checkBox_exact_case.isChecked()
            in_name: bool = self.ui.checkBox_search_names.isChecked()
            in_formula: bool = self.ui.checkBox_search_formulas.isChecked()
            in_text: bool = self.ui.checkBox_search_free_text.isChecked()
            list_result_name: str = self.ui.lineEdit_save_list.text().strip()

            if pattern == "":
                raise ValueError("Text to search for is empty")

            if iode_type == IodeType.COMMENTS:
                results_list = comments.search(pattern, word, case_sensitive, in_name, in_formula, in_text, list_result_name)
            elif iode_type == IodeType.EQUATIONS:
                results_list = equations.search(pattern, word, case_sensitive, in_name, in_formula, in_text, list_result_name)
            elif iode_type == IodeType.IDENTITIES:
                results_list = identities.search(pattern, word, case_sensitive, in_name, in_formula, in_text, list_result_name)
            elif iode_type == IodeType.LISTS:
                results_list = lists.search(pattern, word, case_sensitive, in_name, in_formula, in_text, list_result_name)
            elif iode_type == IodeType.SCALARS:
                results_list = scalars.search(pattern, word, case_sensitive, in_name, in_formula, in_text, list_result_name)
            elif iode_type == IodeType.TABLES:
                results_list = tables.search(pattern, word, case_sensitive, in_name, in_formula, in_text, list_result_name)
            elif iode_type == IodeType.VARIABLES:
                results_list = variables.search(pattern, word, case_sensitive, in_name, in_formula, in_text, list_result_name)

            self.ui.textEdit_result.setPlainText("; ".join(results_list))

        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))
