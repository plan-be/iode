from PySide6.QtCore import Slot, Signal, QLocale
from PySide6.QtWidgets import QWidget, QMessageBox, QDialog
from PySide6.QtGui import QDoubleValidator

from iode_gui.settings import MixinSettingsDialog
from iode_gui.text_edit.completer import IodeCompleter
from iode_gui.plot.plot_vars import PlotVariablesDialog
from iode_gui.menu.file.file_settings import MenuFileSettings
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_graph_variables import Ui_MenuGraphVariables

from typing import List, Union
import numpy as np
import pandas as pd
from iode import (IodeType, TableLang, VarsMode, TableGraphType, Variables, 
                  TableGraphGrid, Period, Sample, variables, reset_extra_files)


class MenuGraphVariables(MixinSettingsDialog):
    new_plot = Signal(QDialog)

    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuGraphVariables()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)
 
        self.ui.textEdit_variables.setup_completer(iode_types=IodeType.VARIABLES)

        self.v_variable_modes = list(VarsMode)
        v_variable_modes_names = [item.name.replace("_" , " ").title() for item in self.v_variable_modes]
        self.ui.comboBox_x_axis_type.addItems(v_variable_modes_names)
        self.ui.comboBox_x_axis_type.setCurrentIndex(0)

        self.v_chart_types = list(TableGraphType)
        v_chart_type_names = [chart_type.name.title() for chart_type in self.v_chart_types]
        self.ui.comboBox_chart_type.addItems(v_chart_type_names)
        self.ui.comboBox_chart_type.setCurrentIndex(0)

        self.v_chart_axis_ticks = list(TableGraphGrid)
        v_chart_axis_thicks = [axis_ticks.name.title() for axis_ticks in self.v_chart_axis_ticks]
        self.ui.comboBox_X_ticks.addItems(v_chart_axis_thicks)
        self.ui.comboBox_Y_ticks.addItems(v_chart_axis_thicks)
        self.ui.comboBox_X_ticks.setCurrentIndex(0)
        self.ui.comboBox_Y_ticks.setCurrentIndex(0)

        self.v_table_langs = list(TableLang)
        v_table_lang_names = [iode_type.name.title() for iode_type in self.v_table_langs]
        self.ui.comboBox_language.addItems(v_table_lang_names)
        self.ui.comboBox_language.setCurrentIndex(0)

        min_Y_validator = QDoubleValidator(self.ui.lineEdit_min_Y)
        # set the locale for the validator to ensure dot as decimal separator
        min_Y_validator.setLocale(QLocale("C"))
        self.ui.lineEdit_min_Y.setValidator(min_Y_validator)

        max_Y_validator = QDoubleValidator(self.ui.lineEdit_max_Y)
        # set the locale for the validator to ensure dot as decimal separator
        max_Y_validator.setLocale(QLocale("C"))
        self.ui.lineEdit_max_Y.setValidator(max_Y_validator)

        self.load_settings()

    def _free_extra_files(self):
        reset_extra_files()

    @property
    def variables_names(self) -> str:
        return self.ui.textEdit_variables.toPlainText().strip()

    @variables_names.setter
    def variables_names(self, names: Union[str, List[str]]):
        if not isinstance(names, str):
            names = ";".join(names)
        self.ui.textEdit_variables.setPlainText(names)

    @property
    def from_period(self) -> str:
        return self.ui.sampleEdit_sample_from.text()
    
    @from_period.setter
    def from_period(self, value: Union[str, Period]):
        if isinstance(value, Period):
            value = str(value)
        self.ui.sampleEdit_sample_from.setText(value)

    @property
    def to_period(self) -> str:
        return self.ui.sampleEdit_sample_to.text()
    
    @to_period.setter
    def to_period(self, value: Union[str, Period]):
        if isinstance(value, Period):
            value = str(value)
        self.ui.sampleEdit_sample_to.setText(value)

    @Slot()
    def display(self):
        try:
            i_table_lang = self.ui.comboBox_language.currentIndex()
            table_lang: TableLang = self.v_table_langs[i_table_lang]
            pattern_var_names: str = self.ui.textEdit_variables.toPlainText().strip()
            from_period: str = self.ui.sampleEdit_sample_from.text().strip()
            to_period: str = self.ui.sampleEdit_sample_to.text().strip()

            i_chart_type: int = self.ui.comboBox_chart_type.currentIndex()
            chart_type: TableGraphType = self.v_chart_types[i_chart_type]
            i_var_mode: int = self.ui.comboBox_x_axis_type.currentIndex()
            var_mode: VarsMode = self.v_variable_modes[i_var_mode]
            log_scale: bool = self.ui.checkBox_log_scale.isChecked()
            i_grid: int = self.ui.comboBox_X_ticks.currentIndex()
            grid: TableGraphGrid = self.v_chart_axis_ticks[i_grid]

            y_min = float(self.ui.lineEdit_min_Y.text()) if self.ui.lineEdit_min_Y.text() else None
            y_max = float(self.ui.lineEdit_max_Y.text()) if self.ui.lineEdit_max_Y.text() else None

            plot_dialog = PlotVariablesDialog(variables, pattern_var_names, from_period, to_period,
                                              chart_type, grid, var_mode, log_scale, y_min, y_max, 
                                              title="VARIABLES")
            self.new_plot.emit(plot_dialog)
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", f"Failed to plot variables '{pattern_var_names}':\n" + str(e))

    @Slot()
    def apply(self):
        QMessageBox.warning(self, "WARNING", "Apply is not yet implemented");

    @Slot()
    def setup(self):
        try:       
            dialog = MenuFileSettings(self)
            dialog.show_print_tab()
            dialog.exec()
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))
