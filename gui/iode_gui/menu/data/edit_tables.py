from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox
from PySide6.QtGui import QCloseEvent

from iode_gui.settings import MixinSettingsDialog
from iode_gui.text_edit.completer import IodeCompleter
from iode_gui.tabs.iode_objs.tab_computed_table import ComputedTableDialog
from .ui_edit_tables import Ui_MenuDataEditTables

from typing import List
from iode import IodeType, tables, variables, Sample, Table, reset_extra_files
from iode.computed_table.computed_table import ComputedTable


class MenuDataEditTables(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuDataEditTables()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.textEdit_table_names.setup_completer(iode_types=IodeType.TABLES) 

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
            sample: Sample = variables.sample
            generalized_sample: str = f"{str(sample.start)}:{sample.nb_periods}"
            pattern: str = ";".join(self.ui.textEdit_table_names.toPlainText().splitlines()).strip()

            table_names: List[str] = tables.get_names(pattern)
            for table_name in table_names:
                computed_table_dialog = ComputedTableDialog(table_name, generalized_sample, 4, parent=self)
                self.table_views.append(computed_table_dialog)
                computed_table_dialog.open()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Failed to edit table(s) .\n" + str(e))
