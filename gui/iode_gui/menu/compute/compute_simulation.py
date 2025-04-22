from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox
from PySide6.QtGui import QDoubleValidator

from iode_gui.settings import MixinSettingsDialog
from iode_gui.text_edit.completer import IodeCompleter
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_compute_simulation import Ui_MenuComputeSimulation

import warnings
from iode import (IodeType, Simulation, SimulationInitialization, SimulationSort,
                  SIMULATION_INITIALIZATION_METHODS, SIMULATION_SORT_ALGORITHMS, 
                  Sample, variables)


class MenuComputeSimulation(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuComputeSimulation()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)  

        self.ui.textEdit_equations_list.setup_completer(iode_types=IodeType.EQUATIONS)
        self.ui.textEdit_exchange.setup_completer(iode_types=IodeType.VARIABLES)

        self.v_simulation_initialization = list(SimulationInitialization)
        self.ui.comboBox_initialization.addItems(SIMULATION_INITIALIZATION_METHODS)
        self.ui.comboBox_initialization.setCurrentIndex(0)

        self.v_simulation_sort_algorithm = list(SimulationSort)
        self.ui.comboBox_sort_algorithm.addItems(SIMULATION_SORT_ALGORITHMS)
        self.ui.comboBox_sort_algorithm.setCurrentIndex(0)

        self.ui.lineEdit_convergence.setValidator(QDoubleValidator(self))
        self.ui.lineEdit_convergence.setText("0.001")

        self.ui.lineEdit_relaxation.setValidator(QDoubleValidator(0.0, 1.0, 2, self))
        self.ui.lineEdit_relaxation.setText("1.0")

        self.ui.spinBox_max_iterations.setValue(100)
        self.ui.spinBox_nb_passes.setValue(5)
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
            equations_list: str = self.ui.textEdit_equations_list.toPlainText().strip()
            exchange_vars_list: str = self.ui.textEdit_exchange.toPlainText().strip()
            convergence: float = float(self.ui.lineEdit_convergence.text())
            max_iterations: int = self.ui.spinBox_max_iterations.value()
            debug: bool = self.ui.checkBox_debug.isChecked()
            relaxation: float = float(self.ui.lineEdit_relaxation.text())
            i_initialization_method: int = self.ui.comboBox_initialization.currentIndex()
            initialization_method: SimulationInitialization = self.v_simulation_initialization[i_initialization_method]
            i_sort_algorithm: int = self.ui.comboBox_sort_algorithm.currentIndex()
            sort_algorithm: SimulationSort = self.v_simulation_sort_algorithm[i_sort_algorithm]
            nb_passes: int = self.ui.spinBox_nb_passes.value()

            if not 0.0 < relaxation <= 1.0:
                raise ValueError("Relaxation factor must be between 0.0 and 1.0")

            if convergence <= 0.0:
                raise ValueError("Convergence threshold must be greater than 0")

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
            simu.sort_algorithm = sort_algorithm
            simu.nb_passes = nb_passes

            if exchange_vars_list:
                simu.model_exchange(exchange_vars_list)

            simu.model_simulate(from_period, to_period, equations_list)
            warnings.simplefilter("default")

            self.accept()
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))
            warnings.simplefilter("default")
