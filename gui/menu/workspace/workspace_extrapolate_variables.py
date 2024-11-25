from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from settings import MixinSettingsDialog
from .ui_workspace_extrapolate_variables import Ui_MenuWorkspaceExtrapolateVariables


from iode import IodeType, SimulationInitialization, variables


class MenuWorkspaceExtrapolateVariables(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuWorkspaceExtrapolateVariables()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.textEdit_variables_list.handle_iode_type(IodeType.VARIABLES)        
        self.ui.textEdit_variables_list.include_iode_command(False)        
        self.ui.textEdit_variables_list.include_lec_functions(False)        

        self.v_simulation_initialization = list(SimulationInitialization)
        v_simulation_initialization_names = [v.name.upper() for v in self.v_simulation_initialization]
        self.ui.comboBox_method.addItems(v_simulation_initialization_names)

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
