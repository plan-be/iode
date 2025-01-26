from PySide6.QtCore import Qt, Slot, Signal, QSettings
from PySide6.QtWidgets import QDialog, QMessageBox, QAbstractItemView, QLineEdit
from PySide6.QtGui import QShortcut, QKeySequence, QContextMenuEvent, QAction
from PySide6.QtPrintSupport import QPrintPreviewDialog

from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.settings import ProjectSettings, PRINT_DESTINATION

from iode_gui.iode_objs.delegates.comments_delegate import CommentsDelegate
from iode_gui.iode_objs.delegates.equations_delegate import EquationsDelegate
from iode_gui.iode_objs.delegates.identities_delegate import IdentitiesDelegate
from iode_gui.iode_objs.delegates.lists_delegate import ListsDelegate
from iode_gui.iode_objs.delegates.scalars_delegate import ScalarsDelegate
from iode_gui.iode_objs.delegates.tables_delegate import TablesDelegate
from iode_gui.iode_objs.delegates.variables_delegate import VariablesDelegate

from iode_gui.iode_objs.new.add_object import (AddCommentDialog, AddIdentityDialog, AddListDialog, 
                                      AddScalarDialog, AddTableDialog, AddVariableDialog)
from iode_gui.iode_objs.edit.edit_iode_obj import EditCommentDialog, EditIdentityDialog, EditListDialog
from iode_gui.iode_objs.edit.edit_equation import EditEquationDialog
from iode_gui.iode_objs.edit.edit_table import EditTableDialog

from iode_gui.iode_objs.models.abstract_table_model import IodeAbstractTableModel
from iode_gui.iode_objs.models.table_model import IdentitiesModel
from iode_gui.iode_objs.models.table_model import VariablesModel

from iode_gui.print.print_file_dialog import PrintFileDialog
from iode_gui.plot.plot_table import PlotTableDialog
from iode_gui.plot.plot_vars import PlotVariablesDialog

from .numerical_view import NumericalTableView
from .abstract_table_view import IodeAbstractTableView

from typing import Tuple
from pathlib import Path
from iode import IodeType, tables, variables, Table, Sample


class CommentsView(IodeAbstractTableView):
    def __init__(self, parent=None):
        super().__init__(IodeType.COMMENTS, CommentsDelegate(parent), parent)
        
        # headers
        self.horizontalHeader().setStretchLastSection(True)
        self.verticalHeader().setStretchLastSection(False)

    def _open_edit_dialog(self, obj_name: str):
        model: IodeAbstractTableModel = self.model()
        self.edit_dialog = EditCommentDialog(obj_name, model.displayed_database, self)
        self.edit_dialog.database_modified.connect(self.database_modified)

    def _open_add_dialog(self) -> QDialog:
        model: IodeAbstractTableModel = self.model()
        return AddCommentDialog(model.displayed_database, self)


class EquationsView(IodeAbstractTableView):
    def __init__(self, parent=None):
        super().__init__(IodeType.COMMENTS, EquationsDelegate(parent), parent)
    
        # headers
        self.horizontalHeader().setStretchLastSection(True)
        self.verticalHeader().setStretchLastSection(False)

    def _open_edit_dialog(self, obj_name: str):
        model: IodeAbstractTableModel = self.model()
        self.edit_dialog = EditEquationDialog(obj_name, model.displayed_database, self.main_window, self)
        self.edit_dialog.database_modified.connect(self.database_modified)
        self.edit_dialog.database_modified.connect(self.update_filter)
        self.edit_dialog.new_object_inserted.connect(self.new_object_inserted)

    def _open_add_dialog(self) -> QDialog:
        model: IodeAbstractTableModel = self.model()
        dialog: EditEquationDialog = EditEquationDialog("", model.displayed_database, self.main_window, self)
        dialog.new_object_inserted.connect(self.new_object_inserted)
        dialog.database_modified.connect(self.database_modified)
        dialog.database_modified.connect(self.update_filter)
        return dialog

    @Slot()
    def update_filter(self):
        model: IodeAbstractTableModel = self.model()
        if model.filter_active:
            self.filter()


class IdentitiesView(IodeAbstractTableView):
    def __init__(self, parent=None):
        super().__init__(IodeType.COMMENTS, IdentitiesDelegate(parent), parent)

        # headers
        self.horizontalHeader().setStretchLastSection(True)
        self.verticalHeader().setStretchLastSection(False)

        # ---- keyboard shortcuts ----
        self.shortcut_execute_current_idt = QShortcut(QKeySequence(Qt.Key.Key_F7), self)
        self.shortcut_execute_idts = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Key.Key_F7), self)

        self.shortcut_execute_current_idt.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_execute_idts.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
    
    # override IodeAbstractTableView method 
    def setup(self, main_window: AbstractMainWindow):
        super().setup(main_window)
        self.shortcut_execute_current_idt.activated.connect(self.execute_current_identity)
        self.shortcut_execute_idts.activated.connect(main_window.open_compute_identities_dialog)

    def _open_edit_dialog(self, obj_name: str):
        model: IodeAbstractTableModel = self.model()
        self.edit_dialog = EditIdentityDialog(obj_name, model.displayed_database, self)
        self.edit_dialog.database_modified.connect(self.database_modified)

    def _open_add_dialog(self) -> QDialog:
        model: IodeAbstractTableModel = self.model()
        return AddIdentityDialog(model.displayed_database, self)

    @Slot()
    def execute_current_identity(self):
        # Get the selected object
        selection = self.selectionModel().selectedRows()
        if not selection:
            return
        index = selection[0]

        # Check if global sample is defined
        if not self.check_global_sample():
            return

        # Computes the selected identity + updates other IODE objects tabs if necessary
        self.main_window.compute_hash(True)
        model: IdentitiesModel = self.model()
        model.execute_identity(index.row())
        self.main_window.update_tab_and_completer()


class ListsView(IodeAbstractTableView):
    def __init__(self, parent=None):
        super().__init__(IodeType.COMMENTS, ListsDelegate(parent), parent)
    
        # headers
        self.horizontalHeader().setStretchLastSection(True)
        self.verticalHeader().setStretchLastSection(False)

    def _open_edit_dialog(self, obj_name: str):
        model: IodeAbstractTableModel = self.model()
        self.edit_dialog = EditListDialog(obj_name, model.displayed_database, self)
        self.edit_dialog.database_modified.connect(self.database_modified)

    def _open_add_dialog(self) -> QDialog:
        model: IodeAbstractTableModel = self.model()
        return AddListDialog(model.displayed_database, self)


class ScalarsView(IodeAbstractTableView, NumericalTableView):
    def __init__(self, parent=None):
        IodeAbstractTableView.__init__(self, IodeType.COMMENTS, ScalarsDelegate(parent), parent)
        NumericalTableView.__init__(self)
        NumericalTableView.setup(self, allow_to_paste=False)

        # headers
        self.horizontalHeader().setStretchLastSection(False)
        self.verticalHeader().setStretchLastSection(False)

    def _open_edit_dialog(self, obj_name: str):
        pass

    def _open_add_dialog(self) -> QDialog:
        model: IodeAbstractTableModel = self.model()
        return AddScalarDialog(model.displayed_database, self)

    # override QTableView method
    @Slot(QContextMenuEvent)
    def contextMenuEvent(self, event: QContextMenuEvent):
        self.popup_context_menu(event)
        event.accept()


class TablesView(IodeAbstractTableView):
    def __init__(self, parent=None):
        super().__init__(IodeType.COMMENTS, TablesDelegate(parent), parent)

        # headers
        self.horizontalHeader().setStretchLastSection(True)
        self.verticalHeader().setStretchLastSection(False)

        # ---- keyboard shortcuts ----
        self.shortcut_display = QShortcut(QKeySequence(Qt.Key.Key_F7), self)
        self.shortcut_display.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_display.activated.connect(self.display)

        self.shortcut_plot = QShortcut(QKeySequence(Qt.Key.Key_F8), self)
        self.shortcut_plot.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_plot.activated.connect(self.plot)

    def _open_edit_dialog(self, obj_name: str):
        model: IodeAbstractTableModel = self.model()
        self.edit_dialog = EditTableDialog(obj_name, model.displayed_database, self.main_window, self)
        self.edit_dialog.database_modified.connect(self.database_modified)

    def _open_add_dialog(self) -> QDialog:
        model: IodeAbstractTableModel = self.model()
        return AddTableDialog(model.displayed_database, self)

    @Slot()
    def display(self):
        # get the selected object
        selection = self.selectedIndexes()
        if not selection:
            return
        index = selection[0]

        table_name = self.model().headerData(index.row(), Qt.Orientation.Vertical, Qt.ItemDataRole.DisplayRole)

        try:
            # check if global sample is defined
            if not self.check_global_sample():
                return

            # generates the ComputedTable representing the whole global sample
            sample = variables.sample
            generalized_sample = f"{sample.start}:{sample.nb_periods}"

            # computes and display the selected table
            # dialog = ComputedTableDialog(table_name, generalized_sample, 4, "", self)
            # self.main_window.append_dialog(dialog)
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Could not compute table {table_name}:\n{str(e)}")
            return

    @Slot()
    def plot(self):
        # get the selected object
        selection = self.selectedIndexes()
        if not selection:
            return
        index = selection[0]

        table_name = self.model().headerData(index.row(), Qt.Vertical, Qt.DisplayRole)

        try:
            # check if global sample is defined
            if not self.check_global_sample():
                return

            # generates the ComputedTable representing the whole global sample
            sample = variables.sample
            generalized_sample = f"{sample.start}:{sample.nb_periods}"

            # computes the tables and generates the associated graph
            table: Table = tables[table_name]
            computed_table_graph = table.compute(generalized_sample, nb_decimals=16)
            plot_dialog = PlotTableDialog(computed_table_graph)
            self.main_window.append_plot(plot_dialog)
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Could not compute table {table_name}:\n{str(e)}")
            return

class VariablesView(IodeAbstractTableView, NumericalTableView):
    new_plot = Signal(QDialog)
    new_graph_dialog = Signal(list, str, str)
    
    def __init__(self, parent=None):
        IodeAbstractTableView.__init__(self, IodeType.COMMENTS, VariablesDelegate(parent), parent)
        NumericalTableView.__init__(self)

        # ---- keyboard shortcuts ----
        self.plot_series_shortcut = QShortcut(QKeySequence(Qt.Key.Key_F8), self)
        self.plot_series_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.plot_series_shortcut.activated.connect(self.plot_series)

        self.graphs_dialog_shortcut = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Key.Key_F8), self)
        self.graphs_dialog_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.graphs_dialog_shortcut.activated.connect(self.open_graphs_dialog)

        NumericalTableView.setup(self, allow_to_paste=True)

        # headers
        self.horizontalHeader().setStretchLastSection(False)
        self.verticalHeader().setStretchLastSection(False)

        # ---- Selection ----
        # See: - https://doc.qt.io/qt-5/model-view-programming.html#handling-selections-in-item-views
        #      - https://doc.qt.io/qt-5/model-view-programming.html#selections

        # -- Behavior --
        # Selecting single items.
        # See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionBehavior-enum
        self.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectItems)

        # -- Mode --
        # When the user selects an item in the usual way, the selection is cleared and the new item selected.
        # However, if the user presses the Shift key while clicking on an item, all items between the current
        # item and the clicked item are selected or unselected, depending on the state of the clicked item.
        # See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionMode-enum
        self.setSelectionMode(QAbstractItemView.SelectionMode.ContiguousSelection)

    def setup(self,main_window: AbstractMainWindow):
        super().setup(main_window)
        self.new_plot.connect(main_window.append_plot)
        self.new_graph_dialog.connect(main_window.open_graphs_variables_dialog_from_vars_view)

    def setup_context_menu(self):
        super().setup_context_menu()

        # Create a new action for plotting selected variables
        action = QAction("Plot", self)
        action.setShortcut(self.plot_series_shortcut.key())
        action.setToolTip("Plot selected variables")
        action.setVisible(True)
        action.triggered.connect(self.plot_series)
        self.context_menu.addAction(action)

        # Create a new action for opening the plot dialog
        action = QAction("Plot dialog", self)
        action.setShortcut(self.graphs_dialog_shortcut.key())
        action.setToolTip("Open the plot dialog")
        action.setVisible(True)
        action.triggered.connect(self.open_graphs_dialog)
        self.context_menu.addAction(action)

    def _open_edit_dialog(self, obj_name: str):
        pass

    def _open_add_dialog(self) -> QDialog:
        if not self.check_global_sample():
            return None
        model: IodeAbstractTableModel = self.model()
        return AddVariableDialog(model.displayed_database, self)

    def set_filter_periods(self, filter_line_edit: QLineEdit):
        self.filter_line_edit_periods = filter_line_edit

    def _get_key_filter(self) -> Tuple[str, str]:
        names = self.filter_line_edit.text().strip()
        periods = self.filter_line_edit_periods.text().strip()
        if not names and not periods:
            return None
        if not names:
            names = '*'
        if not periods:
            periods = None
        return (names, periods)

    # override QTableView method
    @Slot(QContextMenuEvent)
    def contextMenuEvent(self, event: QContextMenuEvent):
        self.popup_context_menu(event)
        event.accept()

    # override AbstractTableView method
    @Slot()
    def print(self):
        try:
            project_settings: QSettings = ProjectSettings.project_settings
            print_to_file = project_settings.value(PRINT_DESTINATION, type=bool)

            if print_to_file:
                # Set up the output file using the filepath associated with the Variable objects
                model: VariablesModel = self.model()
                filepath = model.filepath
                output_file = Path(filepath).with_suffix("")

                # Ask the user to set the output file and format
                dialog = PrintFileDialog(output_file, 'D', self)
                if dialog.exec() == QDialog.DialogCode.Accepted:
                    output_file = dialog.output_file
                    _format = dialog.file_format
                else:
                    return

                # Set the number of decimals and the language
                nb_decimals = model.precision
                language = "ENGLISH"

                # Build the generalized sample
                sample = variables.sample
                generalized_sample = f"{sample.start}:{sample.nb_periods}"

                # List of names = filter pattern or '*' if pattern is empty
                pattern = self.filter_line_edit.text().strip()
                if not pattern:
                    pattern = "*"
                vars_names = variables.get_names(pattern)

                # create a temporary table to print the (selected) variables
                table = Table(2, "Variables to print", vars_names)
                table.language = language
                computed_table = table.compute(generalized_sample, None, nb_decimals)
                computed_table.print_to_file(output_file, _format)
            else:
                dialog = QPrintPreviewDialog(self.printer)
                dialog.paintRequested.connect(self.render_for_printing)
                self.dump_table_in_document()
                dialog.exec()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def plot_series(self):
        """
        Plot the series of variables.
        A new PlotVariablesDialog is created and the series are added to it.
        Finally, the plot is shown and a newPlot signal is emitted with the dialog as argument.
        """
        try:
            variable_names = self._extract_objects_names()
            periods = self._extract_periods()
            from_period = periods[0]
            to_period = periods[-1]
            vars_sample = Sample(from_period, to_period)

            if from_period == to_period:
                raise RuntimeError("Please select more than 1 period to make a plot")

            plot_dialog = PlotVariablesDialog(variables, variable_names, from_period, to_period,
                                              title="VARIABLES SUBSET")
            self.new_plot.emit(plot_dialog)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def open_graphs_dialog(self):
        """
        Open a new graphs dialog.
        A newGraphsDialog signal is emitted with the variable names, the start and end of the sample as arguments.
        """
        try:
            variable_names = self._extract_objects_names()
            periods = self._extract_periods()
            from_period = periods[0]
            to_period = periods[-1]
            self.new_graph_dialog.emit(variable_names, from_period, to_period)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))