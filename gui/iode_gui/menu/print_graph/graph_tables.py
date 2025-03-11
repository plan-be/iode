from PySide6.QtCore import Slot, Signal
from PySide6.QtWidgets import QWidget, QMessageBox, QDialog

from iode_gui.settings import MixinSettingsDialog
from iode_gui.text_edit.completer import IodeCompleter
from iode_gui.plot.plot_table import PlotTableDialog
from iode_gui.menu.file.file_settings import MenuFileSettings
from .ui_graph_tables import Ui_MenuGraphTables

from typing import List
from iode import (IodeType, TableLang, IodeFileType, Table, tables, reset_extra_files)


class MenuGraphTables(MixinSettingsDialog):
    new_plot = Signal(QDialog)

    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuGraphTables()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.textEdit_table_names.setup_completer(iode_types=IodeType.TABLES)

        self.v_table_langs = list(TableLang)
        v_table_lang_names = [iode_type.name.title() for iode_type in self.v_table_langs]
        self.ui.comboBox_language.addItems(v_table_lang_names)
        self.ui.comboBox_language.setCurrentIndex(0)

        self.ui.fileChooser_file_2.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_file_3.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_file_4.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_file_5.enum_file_type = IodeFileType.FILE_VARIABLES

        self.load_settings()

    def _free_extra_files(self):
        reset_extra_files()

    @Slot()
    def display(self):
        try:
            i_table_lang = self.ui.comboBox_language.currentIndex()
            table_lang: TableLang = self.v_table_langs[i_table_lang]
            pattern_table_names: str = self.ui.textEdit_table_names.toPlainText().strip()
            generalized_sample: str = self.ui.textEdit_sample.toPlainText().strip()
            extra_files: List[str] = []
            if self.ui.fileChooser_file_2.filepath:
                extra_files += [self.ui.fileChooser_file_2.filepath]
            if self.ui.fileChooser_file_3.filepath:
                extra_files += [self.ui.fileChooser_file_3.filepath]
            if self.ui.fileChooser_file_4.filepath:
                extra_files += [self.ui.fileChooser_file_4.filepath]
            if self.ui.fileChooser_file_5.filepath:
                extra_files += [self.ui.fileChooser_file_5.filepath]

            table_names: List[str] = tables.get_names(pattern_table_names)
            for table_name in table_names:
                table_obj: Table = tables[table_name]
                computed_table = table_obj.compute(generalized_sample, extra_files, nb_decimals=8)
                plot_dialog = PlotTableDialog(computed_table)
                self.new_plot.emit(plot_dialog)

        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Failed to plot tables '{pattern_table_names}':\n" + str(e))

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
            QMessageBox.warning(self, "WARNING", str(e))
