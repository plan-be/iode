from PySide6.QtCore import Slot, QLocale
from PySide6.QtWidgets import QWidget, QMessageBox
from PySide6.QtGui import QDoubleValidator

from iode_gui.settings import MixinSettingsDialog
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_compute_scc_simulation import Ui_MenuComputeSCCSimulation

import warnings
from iode import (Simulation, SimulationInitialization, SIMULATION_INITIALIZATION_METHODS, 
                  Sample, lists, variables)


class MenuComputeSCCSimulation(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuComputeSCCSimulation()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)  

        self.v_simulation_initialization = list(SimulationInitialization)
        self.ui.comboBox_initialization.addItems(SIMULATION_INITIALIZATION_METHODS)
        self.ui.comboBox_initialization.setCurrentIndex(0)

        self.ui.lineEdit_pre_recursive_list_name.setText("_PRE")
        self.ui.lineEdit_inter_recursive_list_name.setText("_INTER")
        self.ui.lineEdit_post_recursive_list_name.setText("_POST")

        convergence_validator = QDoubleValidator(self)
        # set the locale for the validator to ensure dot as decimal separator
        convergence_validator.setLocale(QLocale("C"))
        self.ui.lineEdit_convergence.setValidator(convergence_validator)
        self.ui.lineEdit_convergence.setText("0.001")

        relaxation_validator = QDoubleValidator(0.0, 1.0, 2, self)
        # set the locale for the validator to ensure dot as decimal separator
        relaxation_validator.setLocale(QLocale("C"))
        self.ui.lineEdit_relaxation.setValidator(relaxation_validator)
        self.ui.lineEdit_relaxation.setText("1.0")

        self.ui.spinBox_max_iterations.setValue(100)
        self.ui.checkBox_debug.setChecked(False)

        self.load_settings()

        vars_sample: Sample = variables.sample
        if vars_sample.nb_periods:
            from_period = str(vars_sample.start)
            to_period = str(vars_sample.end)
            if not self.ui.sampleEdit_sample_from.text():
                self.ui.sampleEdit_sample_from.setText(from_period)
            if not self.ui.sampleEdit_sample_to.text():
                self.ui.sampleEdit_sample_to.setText(to_period)

    @Slot()
    def compute(self):
        try:
            pre_recursive_list_name: str = self.ui.lineEdit_pre_recursive_list_name.text().strip()
            inter_recursive_list_name: str = self.ui.lineEdit_inter_recursive_list_name.text().strip()
            post_recursive_list_name: str = self.ui.lineEdit_post_recursive_list_name.text().strip()
            convergence: float = float(self.ui.lineEdit_convergence.text())
            max_iterations: int = self.ui.spinBox_max_iterations.value()
            debug: bool = self.ui.checkBox_debug.isChecked()
            relaxation: float = float(self.ui.lineEdit_relaxation.text())
            i_initialization_method: int = self.ui.comboBox_initialization.currentIndex()
            initialization_method: SimulationInitialization = self.v_simulation_initialization[i_initialization_method]

            if not 0.0 < relaxation <= 1.0:
                raise ValueError("Relaxation factor must be between 0.0 and 1.0")

            if convergence <= 0.0:
                raise ValueError("Convergence threshold must be greater than 0")

            if not pre_recursive_list_name in lists:
                raise ValueError(f"List '{pre_recursive_list_name}' not found in the IODE lists database")
            if not inter_recursive_list_name in lists:
                raise ValueError(f"List '{inter_recursive_list_name}' not found in the IODE lists database")
            if not post_recursive_list_name in lists:
                raise ValueError(f"List '{post_recursive_list_name}' not found in the IODE lists database")

            from_period: str = self.ui.sampleEdit_sample_from.text().strip()
            to_period: str = self.ui.sampleEdit_sample_to.text().strip()
            # throw an error if from_period and/or to_period are/is invalid
            Sample(from_period, to_period)

            warnings.simplefilter("error")
            simu = Simulation()
            simu.convergence_threshold = convergence
            simu.max_nb_iterations = max_iterations
            simu.debug = debug
            simu.relax = relaxation
            simu.initialization_method = initialization_method

            simu.model_simulate_SCC(from_period, to_period, pre_recursive_list_name, 
                                    inter_recursive_list_name, post_recursive_list_name)
            warnings.simplefilter("default")

            self.accept()
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))
            warnings.simplefilter("default")
        
