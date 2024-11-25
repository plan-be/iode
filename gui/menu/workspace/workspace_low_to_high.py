from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from settings import MixinSettingsDialog
from util.widgets.file_chooser import EnumFileMode
from .ui_workspace_low_to_high import Ui_MenuWorkspaceLowToHigh

from iode import IodeFileType, variables, LowToHighType, LowToHighMethod


class MenuWorkspaceLowToHigh(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuWorkspaceLowToHigh()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_low_to_high_methods = list(LowToHighMethod)
        v_low_to_high_method_names = [method.name.title() for method in self.v_low_to_high_methods]
        self.ui.comboBox_method.addItems(v_low_to_high_method_names)

        self.ui.fileChooser_input_file.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_input_file.enum_file_mode = EnumFileMode.EXISTING_FILE

    @Slot()
    def slot_low_to_high(self):
        try:
            filepath: str = self.ui.fileChooser_input_file.filepath
            i_method: int = self.ui.comboBox_method.currentIndex()
            method: LowToHighMethod = self.v_low_to_high_methods[i_method]
            stock_series: str = ";".join(self.ui.textEdit_stock_series.toPlainText().splitlines()).strip()
            flow_series: str = ";".join(self.ui.textEdit_stock_series.toPlainText().splitlines()).strip() 
            
            if not len(filepath):
                raise ValueError("'input file' is empty")

            # Check if all series names are empty
            if not stock_series and not flow_series:
                raise ValueError("The series 'stock', and 'flow' are both empty")

            if stock_series:
                variables.low_to_high(LowToHighType.STOCK, method, filepath, stock_series)
            if flow_series:
                variables.low_to_high(LowToHighType.FLOW, method, filepath, flow_series)

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "Failed to build series with higher periodicity.\n" + str(e))
