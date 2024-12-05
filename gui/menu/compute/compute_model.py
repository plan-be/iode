from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from settings import MixinSettingsDialog
from .ui_compute_model import Ui_MenuComputeModel

from iode import IodeType, Simulation


class MenuComputeModel(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuComputeModel()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)  

        self.ui.textEdit_equations_list.handle_iode_type(IodeType.EQUATIONS)        
        self.ui.textEdit_equations_list.include_iode_command(False)        
        self.ui.textEdit_equations_list.include_lec_functions(False)

        self.load_settings()

    @Slot()
    def compute(self):
        try:
            equations_list: str = self.ui.textEdit_equations_list.toPlainText().strip()

            simu = Simulation()
            simu.model_compile(equations_list)         

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", str(e))
