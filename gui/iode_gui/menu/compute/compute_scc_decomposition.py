from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.text_edit.completer import IodeCompleter
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_compute_scc_decomposition import Ui_MenuComputeSCCDecomposition

import warnings
from iode import IodeType, Simulation


class MenuComputeSCCDecomposition(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuComputeSCCDecomposition()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)  

        self.ui.textEdit_equations_list.setup_completer(iode_types=IodeType.EQUATIONS)

        self.ui.spinBox_triangulation_iterations.setValue(5)
        self.ui.lineEdit_pre_recursive_list_name.setText("_PRE")
        self.ui.lineEdit_inter_recursive_list_name.setText("_INTER")
        self.ui.lineEdit_post_recursive_list_name.setText("_POST")

        self.load_settings()

    @Slot()
    def compute(self):
        try:
            equations_list: str = self.ui.textEdit_equations_list.toPlainText().strip()
            nb_iterations: int = self.ui.spinBox_triangulation_iterations.value()
            pre_recursive_list_name: str = self.ui.lineEdit_pre_recursive_list_name.text().strip()
            inter_recursive_list_name: str = self.ui.lineEdit_inter_recursive_list_name.text().strip()
            post_recursive_list_name: str = self.ui.lineEdit_post_recursive_list_name.text().strip()

            warnings.simplefilter("error")
            simu = Simulation()
            simu.model_calculate_SCC(nb_iterations, pre_recursive_list_name, inter_recursive_list_name, 
                                     post_recursive_list_name, equations_list)
            warnings.simplefilter("default")

            self.accept()
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))
            warnings.simplefilter("default")
