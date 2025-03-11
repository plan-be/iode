from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.text_edit.completer import IodeCompleter
from .ui_compute_model import Ui_MenuComputeModel

import warnings
from iode import IodeType, Simulation


class MenuComputeModel(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuComputeModel()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)  

        self.ui.textEdit_equations_list.setup_completer(iode_types=IodeType.EQUATIONS)

        self.load_settings()

    @Slot()
    def compute(self):
        try:
            equations_list: str = self.ui.textEdit_equations_list.toPlainText().strip()

            warnings.simplefilter("error")
            simu = Simulation()
            simu.model_compile(equations_list)         
            warnings.simplefilter("default")

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", str(e))
            warnings.simplefilter("default")
