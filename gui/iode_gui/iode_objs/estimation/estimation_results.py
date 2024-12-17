from PySide6.QtCore import Qt, Signal, Slot, QAbstractTableModel
from PySide6.QtWidgets import QDialog, QMessageBox, QHeaderView
from PySide6.QtGui import (QKeySequence, QShortcut, QTextCursor, QTextCharFormat, 
                           QFont, QTextTableFormat, QTextFrameFormat, QTextDocument, 
                           QDesktopServices)
from PySide6.QtPrintSupport import QPrinter, QPrintPreviewDialog

from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.utils import URL_MANUAL
from .correlation_matrix_model import CorrelationMatrixModel
from .tests_eqs_model import TestsEqsModel
from iode_gui.iode_objs.models.table_model import ScalarsModel
from iode_gui.plot.plot_vars import PlotVariablesDialog
from .ui_estimation_results import Ui_EstimationResultsDialog

from typing import List, Dict
from iode import Sample, Scalars, Equation, EQ_TEST_NAMES, EqTest, variables
from iode.util import EditAndEstimateEquations


class EstimationResultsDialog(QDialog):
    new_plot = Signal(QDialog)

    def __init__(self, edit_est_eqs: EditAndEstimateEquations, parent=None):
        super().__init__(parent)
        self.edit_est_eqs: EditAndEstimateEquations = edit_est_eqs

        self.ui = Ui_EstimationResultsDialog()
        self.ui.setupUi(self)

        # Set up the header view style
        self.stylesheet_header = "QHeaderView::section { background-color: lightGray font: bold border: 0.5px solid }"
        self.ui.tableView_coefs.setStyleSheet(self.stylesheet_header)

        # Set up tabs
        self._set_coefficients_tab()
        self._set_correlation_matrix_tab()
        self._set_tests_tab()

        # Get the sample and update the UI
        sample: Sample = self.edit_est_eqs.sample
        self.from_period = str(sample.start)
        self.to_period = str(sample.end)
        self.variables_names: List[str] = self.edit_est_eqs.equations_list

        # Set up shortcut for full screen
        self.full_screen_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_X), self)
        self.full_screen_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.full_screen_shortcut.activated.connect(self.showMaximized)

    def setup(self, main_window: AbstractMainWindow):
        self.new_plot.connect(main_window.append_dialog)

    def _set_coefficients_tab(self):
        """Sets the coefficients tab."""
        self.ui.tableView_coefs.setStyleSheet(self.stylesheet_header)

        # NOTE: make a copy of the estimation scalars database because the database 
        #       passed to the constructor of ScalarsModel is deleted in its destructor 
        scalars_db: Scalars = self.edit_est_eqs.scalars_db.copy()
        scalars_model: ScalarsModel = ScalarsModel(scalars_db=scalars_db, parent=self)
        self.ui.tableView_coefs.setModel(scalars_model)

    def _set_correlation_matrix_tab(self):
        """Sets the correlation matrix tab."""
        try:
            self.ui.tableView_corr_matrix.setStyleSheet(self.stylesheet_header)
            model: CorrelationMatrixModel = CorrelationMatrixModel(self.edit_est_eqs.correlation_matrix, self)
            self.ui.tableView_corr_matrix.setModel(model)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    def _set_tests_tab(self):
        """Sets the tests tab."""
        try:
            self.ui.tableView_tests.setStyleSheet(self.stylesheet_header)
            self.ui.tableView_tests.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.ResizeToContents)
            testsEqsModel: TestsEqsModel = TestsEqsModel(self.edit_est_eqs, self)
            self.ui.tableView_tests.setModel(testsEqsModel)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    def _dump_table(self, table_model: QAbstractTableModel, cursor: QTextCursor) -> QTextCursor:
        """Dumps the table to the cursor.

        Args:
            table_model (QAbstractTableModel): The table model.
            cursor (QTextCursor): The text cursor.
        """
        try:
            # Create a bold format for the headers
            bold_format = QTextCharFormat()
            bold_format.setFontWeight(QFont.Weight.Bold)

            # Create a table and set its format
            table = cursor.insertTable(table_model.rowCount() + 1, table_model.columnCount() + 1)
            table_format = table.format()
            table_format.setHeaderRowCount(1)
            table_format.setCellPadding(4)
            table_format.setCellSpacing(0)
            table.setFormat(table_format)

            # top left cell
            header_cell = table.cellAt(0, 0)
            cell_format = header_cell.format().toTableCellFormat()
            cell_format.setRightBorderStyle(QTextTableFormat.BorderStyle.BorderStyle_Solid)
            cell_format.setBottomBorderStyle(QTextTableFormat.BorderStyle.BorderStyle_Solid)
            header_cell.setFormat(cell_format)

            # columns header
            for col in range(table_model.columnCount()):
                text: str = table_model.headerData(col, Qt.Orientation.Horizontal, 
                                                   Qt.ItemDataRole.DisplayRole)
                header_cell = table.cellAt(0, col + 1)
                cursor = header_cell.firstCursorPosition()
                cursor.insertText(text, bold_format)
                cell_format = header_cell.format().toTableCellFormat()
                cell_format.setBottomBorderStyle(QTextTableFormat.BorderStyle.BorderStyle_Solid)
                header_cell.setFormat(cell_format)

            # table rows
            for row in range(table_model.rowCount()):
                # row header
                text: str = table_model.headerData(row, Qt.Orientation.Vertical, 
                                                   Qt.ItemDataRole.DisplayRole)
                header_cell = table.cellAt(row + 1, 0)
                cursor = header_cell.firstCursorPosition()
                cursor.insertText(text, bold_format)
                cell_format = header_cell.format().toTableCellFormat()
                cell_format.setRightBorderStyle(QTextTableFormat.BorderStyle.BorderStyle_Solid)
                header_cell.setFormat(cell_format)

                # row cells content
                for col in range(table_model.columnCount()):
                    text: str = table_model.data(table_model.index(row, col), 
                                                 Qt.ItemDataRole.DisplayRole)
                    cell = table.cellAt(row + 1, col + 1)
                    cursor = cell.firstCursorPosition()
                    cursor.insertText(text)

            cursor.movePosition(QTextCursor.MoveOperation.End)
            return cursor
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    # same as o_estgr() in DOS o_gr.c + see ODE_blk_res_fn() case 6
    @Slot()
    def plot_yobs_yest(self):
        """Plots the observed and estimated values."""
        try:
            # Get the current equation
            current_eq: Equation = self.edit_est_eqs.current_equation

            # Get the equation name
            eq_name = current_eq.endogenous

            # Split the equation
            lhs, _ = current_eq.split_equation()

            # get estimation sample + list of periods
            sample: Sample = self.edit_est_eqs.sample
            periods = sample.get_period_list(astype=float)

            # Observed + Fitted values
            title = f"Equation {eq_name} : observed and fitted values"
            observed: List[float] = self.edit_est_eqs.get_observed_values(eq_name)
            fitted: List[float] = self.edit_est_eqs.get_fitted_values(eq_name)
            data = {f"{lhs} : observed": observed, f"{lhs} : fitted": fitted}
            plot_dialog = PlotVariablesDialog(periods, data, title=title)
            self.new_plot.emit(plot_dialog)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    # same as o_estgr() in DOS o_gr.c + see ODE_blk_res_fn() case 7
    @Slot()
    def plot_residual(self):
        """Plots the residual."""
        try:
            # Get the current equation
            current_eq: Equation = self.edit_est_eqs.current_equation

            # Get the equation name
            eq_name = current_eq.endogenous

            # Split the equation
            lhs, _ = current_eq.split_equation()

            # get estimation sample + list of periods
            sample: Sample = self.edit_est_eqs.sample
            periods = sample.get_period_list(astype=float)

            # Residual values
            title = f"Equation {eq_name}: residuals"
            values: List[float] = self.edit_est_eqs.get_residual_values(eq_name)
            data = {f"{lhs} : observed": values}
            plot_dialog = PlotVariablesDialog(periods, data, title=title)
            self.new_plot.emit(plot_dialog)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def print_output(self):
        """Prints the output."""
        try:
            document = QTextDocument()
            cursor = QTextCursor(document)

            # Dump the coefficients matrix
            cursor.insertHtml("<h3>Coefficients</h3>")
            cursor.insertText("\n\n")

            scalars_model: ScalarsModel = self.ui.tableView_coefs.model()
            cursor = self._dump_table(scalars_model, cursor)
            cursor.insertText("\n\n")

            # Dump the correlation matrix
            cursor.insertHtml("<h3>Correlation Matrix</h3>")
            cursor.insertText("\n\n")

            coeffs_model: CorrelationMatrixModel = self.ui.tableView_corr_matrix.model()
            cursor = self._dump_table(coeffs_model, cursor)
            cursor.insertText("\n\n")

            # Dump the test results
            cursor.insertHtml("<h3>Tests By Equation</h3>")
            cursor.insertText("\n\n")

            equation: Equation = self.edit_est_eqs.current_equation
            tests: Dict[str, float] = equation.tests

            table = cursor.insertTable(len(EQ_TEST_NAMES) + 1, 2)
            table_format = table.format()
            table_format.setHeaderRowCount(1)
            table_format.setCellPadding(4)
            table_format.setCellSpacing(0)
            table.setFormat(table_format)

            # Columns header
            header_cell = table.cellAt(0, 0)
            cell_format = header_cell.format().toTableCellFormat()
            bold_format = QTextCharFormat()
            bold_format.setFontWeight(QFont.Weight.Bold)
            cursor = header_cell.firstCursorPosition()
            cursor.insertText("Test Name", bold_format)
            cell_format.setBottomBorderStyle(QTextFrameFormat.BorderStyle.BorderStyle_Solid)
            header_cell.setFormat(cell_format)

            # Rows
            row = 0
            v_tests_names = [member.name.lower() for member in EqTest]
            for row, test_full_name in enumerate(EQ_TEST_NAMES):
                cell = table.cellAt(row + 1, 0)
                cursor = cell.firstCursorPosition()
                cursor.insertText(test_full_name)

                cell = table.cellAt(row + 1, 1)
                cursor = cell.firstCursorPosition()
                test_name = v_tests_names[row]
                cursor.insertText(f"{tests[test_name]:.8g}")

                row += 1

            cursor.movePosition(QTextCursor.MoveOperation.End)

            # Print
            printer = QPrinter()
            dialog = QPrintPreviewDialog(printer)
            dialog.paintRequested.connect(lambda: document.print_(printer))
            dialog.exec()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def toggle_fullscreen(self):
        """Toggles the full screen mode."""
        if self.isFullScreen():
            self.showNormal()
        else:
            self.showFullScreen()

    @Slot()
    def help(self):
        """Shows the help dialog."""
        QDesktopServices.openUrl(URL_MANUAL)
