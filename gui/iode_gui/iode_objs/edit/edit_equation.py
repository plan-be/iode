from PySide6.QtCore import Qt, Signal, Slot
from PySide6.QtWidgets import QPlainTextEdit, QMessageBox, QDialog
from PySide6.QtGui import QShortcut, QKeySequence

from iode_gui.settings import MixinSettingsDialog
from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.iode_objs.estimation.estimation_coefs import EstimationCoefsDialog
from iode_gui.iode_objs.estimation.estimation_results import EstimationResultsDialog
from iode_gui.iode_objs.misc.unit_root import UnitRootDialog
from iode_gui.iode_objs.misc.dyn_adjustment import DynAdjustmentDialog
from iode_gui.text_edit.completer import IodeCompleter

from .ui_edit_equation import Ui_EditEquationDialog

from typing import Dict
import warnings
from iode import equations, Equation, EqMethod, Sample, IodeType
from iode.compute.estimation import EditAndEstimateEquations


class EditEquationDialog(MixinSettingsDialog):
    database_modified = Signal()
    new_object_inserted = Signal(str)

    def __init__(self, equation_name: str, database, main_window: AbstractMainWindow, parent=None):
        super().__init__(parent)

        self.ui = Ui_EditEquationDialog()
        self.ui.setupUi(self)

        self.database = database
        self.main_window = main_window

        self.ui.textEdit_lec.setup_completer(main_window=main_window, iode_types=[IodeType.SCALARS, IodeType.VARIABLES])
        self.ui.textEdit_lec.setLineWrapMode(QPlainTextEdit.LineWrapMode.WidgetWidth)

        self.full_screen_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_X), self)
        self.full_screen_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.full_screen_shortcut.activated.connect(self.showMaximized)

        # initialize combobox listing the possible estimation methods
        self.v_estimation_methods_names = [name.upper() for name in EqMethod.__members__]
        self.v_estimation_methods = list(EqMethod)
        self.ui.comboBox_method.addItems(self.v_estimation_methods_names)

        # initialize fields and EditAndEstimateEquations instance
        try:
            if equation_name and equation_name in equations:
                # Existing equation
                self.setWindowTitle(f"Editing equation {equation_name}")
                self.ui.lineEdit_name.setReadOnly(True)

                self.eq: Equation = self.database[equation_name]
                self._display_equation()

                estimation_method = self.eq.method
                if estimation_method is not None:
                    estimation_method = estimation_method.upper()
                    estimation_method_index = self.v_estimation_methods_names.index(estimation_method)
                else:
                    estimation_method_index = 0
                self.ui.comboBox_method.setCurrentIndex(estimation_method_index)

                sample: Sample = self.eq.sample
                from_period: str = str(sample.start) if sample.start is not None else ""
                self.ui.sampleEdit_from.setText(from_period)
                to_period: str = str(sample.end) if sample.end is not None else ""
                self.ui.sampleEdit_to.setText(to_period)

                self.ui.lineEdit_block.setText(self.eq.block)
                self.ui.lineEdit_instruments.setText(self.eq.instruments)

                self.edit_est_eqs = EditAndEstimateEquations(from_period, to_period)
                self._update_list_of_equations_to_estimate()
            else:
                # New equation
                self.setWindowTitle("Add a new equation")
                self.ui.lineEdit_name.setReadOnly(False)
                self.edit_est_eqs = EditAndEstimateEquations()
        except Exception as e:
            self.edit_est_eqs = EditAndEstimateEquations()
            QMessageBox.warning(None, "WARNING", str(e))

    # same as ODE_blk_save_cur() from o_est.c from the old GUI
    def _save_current_equation(self):
        """Saves the current equation."""
        lec = self.ui.textEdit_lec.toPlainText()
        comment = self.ui.lineEdit_comment.text()
        self.edit_est_eqs.update_current_equation(lec, comment)

    # same as ODE_blk_check() from o_est.c from the old GUI
    def _update_list_of_equations_to_estimate(self):
        """Updates the list of equations to estimate."""
        # Extract equation name from lineEdit_name
        current_eq_name = self.ui.lineEdit_name.text()
        # Check if the equation name is empty, raise an exception if it is
        if not current_eq_name:
            raise ValueError("Empty equation name")

        # Extract block from lineEdit_block
        block = self.ui.lineEdit_block.text()
        # If block is empty, use the equation name as the block
        if not block:
            block = current_eq_name

        # Update block in edit_est_eqs
        self.edit_est_eqs.block = block, current_eq_name

        # The list of equations to estimate (block) may have been changed.
        # See EditAndEstimateEquations.block
        block = self.edit_est_eqs.block
        self.ui.lineEdit_block.setText(block)

    # same as ODE_blk_cur() from o_est.c from the old GUI
    def _display_equation(self):
        """Displays the equation."""
        if not self.eq:
            return
        
        self.ui.lineEdit_name.setText(self.eq.endogenous)

        # editable values
        self.ui.textEdit_lec.setPlainText(self.eq.lec)
        self.ui.lineEdit_comment.setText(self.eq.comment)

        # read-only values
        tests: Dict[str, float] = self.eq.tests
        self.ui.lineEdit_tests_r2adj.setText(f'{tests["r2adj"]:.3g}')
        self.ui.lineEdit_tests_durbw.setText(f'{tests["dw"]:.3g}')
        self.ui.lineEdit_tests_fstat.setText(f'{tests["fstat"]:.3g}')
        self.ui.lineEdit_tests_loglk.setText(f'{tests["loglik"]:.3g}')

    # same as ODE_blk_save_fn() from o_est.c from old GUI
    @Slot()
    def edit(self):
        """Edits the equation."""
        try:
            self._save_current_equation()
            self._update_list_of_equations_to_estimate()

            if not self.edit_est_eqs.is_done:
                self.edit_est_eqs.update_scalars()
                instruments = self.ui.lineEdit_instruments.text()
                self.edit_est_eqs.instruments = instruments

            hash_before = hash(equations)

            from_period = self.ui.sampleEdit_from.text()
            to_period = self.ui.sampleEdit_to.text()

            v_new_eqs = self.edit_est_eqs.save(from_period, to_period)
            for name in v_new_eqs:
                self.new_object_inserted.emit(name)

            hash_after = hash(equations)
            if hash_after != hash_before:
                self.database_modified.emit()

            self.accept()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    # same as ODE_blk_coef_fn() from o_est.c from the old GUI
    @Slot()
    def display_coefs(self):
        """Displays the coefficients."""
        try:
            self._save_current_equation()
            self._update_list_of_equations_to_estimate()

            self.edit_est_eqs.update_scalars()

            dialog = EstimationCoefsDialog(self.edit_est_eqs)
            dialog.exec()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    # same as ODE_blk_est_fn() from o_est.c from the old GUI
    @Slot()
    def estimate(self):
        """Estimates the equation."""
        warnings.simplefilter("error")
        try:
            self._save_current_equation()

            # set method
            method_index = self.ui.comboBox_method.currentIndex()
            if method_index < 0:
                raise ValueError("Invalid method. Please choose a method in the drop-down list")
            self.edit_est_eqs.method = self.v_estimation_methods[method_index]

            # set sample
            from_period = self.ui.sampleEdit_from.text()
            if not from_period:
                raise ValueError("Starting period for the sample is empty")

            to_period = self.ui.sampleEdit_to.text()
            if not to_period:
                raise ValueError("Ending period for the sample is empty")

            self.edit_est_eqs.sample = f"{from_period}:{to_period}"

            # set instruments
            instruments = self.ui.lineEdit_instruments.text()
            self.edit_est_eqs.instruments = instruments

            # update list of equations to estimate
            self._update_list_of_equations_to_estimate()

            # update list of coefficients
            self.edit_est_eqs.update_scalars()

            # process estimation
            self.edit_est_eqs.estimate()

            # refresh the values for the tests of the current equation
            self.eq = self.edit_est_eqs.current_equation
            self._display_equation()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
        warnings.simplefilter("default")

    # same as ODE_blk_next_fn() from o_est.c from old GUI
    @Slot()
    def next(self):
        """Moves to the next equation."""
        try:
            # Save the current equation
            self._save_current_equation()

            # Update the list of equations to estimate
            self._update_list_of_equations_to_estimate()

            # Get the next equation from our list
            self.eq = self.edit_est_eqs.next_equation

            # Display the equation
            self._display_equation()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def dynamic_adjustment(self):
        """Performs dynamic adjustment."""
        try:
            lec = self.ui.textEdit_lec.toPlainText()
            dialog = DynAdjustmentDialog(lec, self)
            if dialog.exec() == QDialog.DialogCode.Accepted:
                self.ui.textEdit_lec.setPlainText(dialog.lec)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    # same as ODE_blk_res_fn() function from o_est.c from the old GUI
    @Slot()
    def results(self):
        """Displays the results."""
        if self.edit_est_eqs.is_done:
            try:
                dialog = EstimationResultsDialog(self.edit_est_eqs, self)
                dialog.setup(self.main_window)
                dialog.exec()
            except Exception as e:
                QMessageBox.warning(None, "WARNING", str(e))
        else:
            QMessageBox.warning(None, "WARNING", "No estimation has been done yet.")

    @Slot()
    def unit_root(self):
        """Performs unit root test."""
        try:
            dialog = UnitRootDialog(self)
            dialog.exec()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def toggle_full_screen(self):
        """Toggles full screen mode."""
        if self.isFullScreen():
            self.showNormal()
        else:
            self.showFullScreen()
