from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox
from PySide6.QtGui import QCloseEvent

from settings import MixinSettingsDialog
from tabs.iode_objs.tab_computed_table import ComputedTableNumericalDialog
from .ui_edit_tables import Ui_MenuDataEditTables

from typing import List
from iode import IodeType, tables, variables, Sample, Table, reset_extra_files
from iode.util import ComputedTable


class MenuDataEditTables(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuDataEditTables()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.textEdit_table_names.handle_iode_type(IodeType.TABLES)        
        self.ui.textEdit_table_names.include_iode_command(False)        
        self.ui.textEdit_table_names.include_lec_functions(False)  

        self.table_views: List[ComputedTableNumericalDialog] = []
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
            sample: Sample = variables.sample
            generalized_sample: str = f"{str(sample.start)}:{sample.nb_periods}"
            pattern: str = ";".join(self.ui.textEdit_table_names.toPlainText().splitlines()).strip()

            table_names: List[str] = tables.get_names(pattern)
            for table_name in table_names:
                computed_table_dialog = ComputedTableNumericalDialog(table_name, generalized_sample, 4, parent=self)
                self.table_views.append(computed_table_dialog)
                computed_table_dialog.open()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Failed to edit table(s) .\n" + str(e))
