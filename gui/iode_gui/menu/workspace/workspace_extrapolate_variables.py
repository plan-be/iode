from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.text_edit.completer import IodeCompleter
from .ui_workspace_extrapolate_variables import Ui_MenuWorkspaceExtrapolateVariables


from iode import (IodeType, SimulationInitialization, SIMULATION_INITIALIZATION_METHODS, 
                  variables)


class MenuWorkspaceExtrapolateVariables(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuWorkspaceExtrapolateVariables()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.textEdit_variables_list.setup_completer(iode_types=IodeType.VARIABLES)     

        self.v_simulation_initialization = list(SimulationInitialization)
        self.ui.comboBox_method.addItems(SIMULATION_INITIALIZATION_METHODS)
        self.ui.comboBox_method.setCurrentIndex(0)

        self.load_settings()

    @Slot()
    def extrapolate_variables(self):
        try:
            from_period: str = self.ui.sampleEdit_sample_from.text() 
            to_period: str = self.ui.sampleEdit_sample_to.text() 
            i_simu_method: int = self.ui.comboBox_method.currentIndex()
            simu_method: SimulationInitialization = self.v_simulation_initialization[i_simu_method]
            vars_list: str = ";".join(self.ui.textEdit_variables_list.toPlainText().splitlines()).strip()

            variables.extrapolate(simu_method, from_period, to_period, vars_list)

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "Failed to extrapolate the variables.\n" + str(e))
