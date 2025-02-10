from PySide6.QtCore import Slot, QSettings
from PySide6.QtWidgets import QWidget, QMessageBox, QDialog
from PySide6.QtGui import QCloseEvent, QTextDocument
from PySide6.QtPrintSupport import QPrinter, QPrintPreviewDialog

from iode_gui.utils import TMP_FILENAME
from iode_gui.settings import MixinSettingsDialog, ProjectSettings, PRINT_DESTINATION
from iode_gui.menu.file.file_settings import MenuFileSettings
from iode_gui.tabs.iode_objs.tab_computed_table import ComputedTableDialog
from iode_gui.print.print_file_dialog import PrintFileDialog
from .ui_print_tables import Ui_MenuPrintTables

from typing import List
from pathlib import Path
from iode import (IodeType, TableLang, IodeFileType, tables, 
                  load_extra_files, reset_extra_files, PrintTablesAs)


class MenuPrintTables(MixinSettingsDialog):
    def __init__(self, parent: QWidget = None) -> None:
        super().__init__(parent)
        self.ui = Ui_MenuPrintTables()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.textEdit_table_names.handle_iode_type(IodeType.TABLES)        
        self.ui.textEdit_table_names.include_iode_command(False)       
        self.ui.textEdit_table_names.include_lec_functions(False)

        self.v_table_langs = list(TableLang)
        v_table_lang_names = [iode_type.name.title() for iode_type in self.v_table_langs]
        self.ui.comboBox_language.addItems(v_table_lang_names)
        self.ui.comboBox_language.setCurrentIndex(0)

        self.ui.fileChooser_file_2.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_file_3.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_file_4.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_file_5.enum_file_type = IodeFileType.FILE_VARIABLES

        self.ui.spinBox_nb_decimals.setValue(5)

        self.table_views: List[ComputedTableDialog] = []
        self.load_settings()

    # override MixinSettingsDialog method
    @Slot()
    def closeEvent(self, event: QCloseEvent):
        self.save_settings()

        for dialog in self.table_views: 
            dialog.close()
        self.table_views.clear()
        reset_extra_files()

        event.accept()

    @Slot()
    def display(self):
        try:
            i_table_lang = self.ui.comboBox_language.currentIndex()
            table_lang: TableLang = self.v_table_langs[i_table_lang]
            pattern_table_names: str = self.ui.textEdit_table_names.toPlainText().strip()
            generalized_sample: str = self.ui.textEdit_sample.toPlainText().strip()
            nb_decimals: int = self.ui.spinBox_nb_decimals.value()
            extra_files: List[str] = []
            if self.ui.fileChooser_file_2.filepath:
                extra_files += [self.ui.fileChooser_file_2.filepath]
            if self.ui.fileChooser_file_3.filepath:
                extra_files += [self.ui.fileChooser_file_3.filepath]
            if self.ui.fileChooser_file_4.filepath:
                extra_files += [self.ui.fileChooser_file_4.filepath]
            if self.ui.fileChooser_file_5.filepath:
                extra_files += [self.ui.fileChooser_file_5.filepath]
            load_extra_files(extra_files)
            
            table_names: List[str] = tables.get_names(pattern_table_names)
            for table_name in table_names:
                computed_table_dialog = ComputedTableDialog(table_name, generalized_sample, nb_decimals, parent=self)
                self.table_views.append(computed_table_dialog)
                computed_table_dialog.open()    
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Failed to display tables '{pattern_table_names}':\n" + str(e))

    @Slot()
    def print(self):
        try:
            i_table_lang = self.ui.comboBox_language.currentIndex()
            table_lang: TableLang = self.v_table_langs[i_table_lang]
            pattern_table_names: str = self.ui.textEdit_table_names.toPlainText().strip()
            generalized_sample: str = self.ui.textEdit_sample.toPlainText().strip()
            nb_decimals: int = self.ui.spinBox_nb_decimals.value()
            extra_files: List[str] = []
            if self.ui.fileChooser_file_2.filepath:
                extra_files += [self.ui.fileChooser_file_2.filepath]
            if self.ui.fileChooser_file_3.filepath:
                extra_files += [self.ui.fileChooser_file_3.filepath]
            if self.ui.fileChooser_file_4.filepath:
                extra_files += [self.ui.fileChooser_file_4.filepath]
            if self.ui.fileChooser_file_5.filepath:
                extra_files += [self.ui.fileChooser_file_5.filepath]
            if extra_files:
                load_extra_files(extra_files)
            
            if not pattern_table_names:
                pattern_table_names = '*'
            
            # check list of tables is valid
            table_names: List[str] = tables.get_names(pattern_table_names)

            project_settings: QSettings = ProjectSettings.project_settings
            if project_settings is None:
                b_print_to_file = False
            else:
                b_print_to_file = project_settings.value(PRINT_DESTINATION, type=bool)

            if b_print_to_file:
                # Ask the user to set the output file and format
                dialog = PrintFileDialog(parent=self)
                if dialog.exec() == QDialog.DialogCode.Accepted:
                    output_file = dialog.output_file
                    _format = dialog.file_format
                else:
                    return
            else:
                # Note: here the table is first saved in a temporary HTML file to be then 
                # loaded by a QTextDocument object (see below)
                output_file = TMP_FILENAME + '.htm'
                _format = 'H'

            tables.print_tables_as = PrintTablesAs.COMPUTED
            tables.print_to_file(output_file, table_names, generalized_sample=generalized_sample, 
                                 nb_decimals=nb_decimals, format=_format)
            
            if not b_print_to_file:
                output_file = Path(output_file)
                if not output_file.exists():
                    raise RuntimeError("Couldn't export tables to a file")
                document = QTextDocument(self)
                with open(output_file, mode='r') as f:
                    document.setHtml(f.read())
                output_file.unlink()

                printer = QPrinter()
                dialog = QPrintPreviewDialog(printer)
                dialog.paintRequested.connect(lambda: document.print_(printer))
                dialog.exec()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Failed to print tables '{pattern_table_names}':\n" + str(e))

    @Slot()
    def setup(self):
        try:       
            dialog = MenuFileSettings(self)
            dialog.show_print_tab()
            dialog.exec()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", str(e))
