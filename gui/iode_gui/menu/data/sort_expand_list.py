from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from .ui_sort_expand_list import Ui_MenuDataSortExpandList

from iode import IodeType, lists


class MenuDataSortExpandList(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuDataSortExpandList()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.lineEdit_sort.handle_iode_type(IodeType.LISTS)        
        self.ui.lineEdit_sort.include_iode_command(False)        
        self.ui.lineEdit_sort.include_lec_functions(False)

        self.load_settings()

    @Slot()
    def sort(self):
        try:
            list_to_sort_name: str = self.ui.lineEdit_sort.text().strip()
            new_name: str = self.ui.lineEdit_save.text().strip()

            if not list_to_sort_name:
                raise ValueError("Please enter a list name to sort")
            
            if not new_name:
                new_name = list_to_sort_name
            
            if list_to_sort_name not in lists:
                raise ValueError(f"List '{list_to_sort_name}' does not exist")

            proceed: bool = True
            if new_name in lists and new_name != list_to_sort_name:
                answer = QMessageBox.question(self, "Confirmation", f"List '{new_name}' already exists. "
                                              "Do you want to overwrite it?", 
                                              QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
            
                proceed = answer == QMessageBox.StandardButton.Yes
            
            if proceed:
                lists[new_name] = sorted(lists[list_to_sort_name])

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", str(e))
