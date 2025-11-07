from PySide6.QtCore import Qt, Slot, Signal, QSettings
from PySide6.QtWidgets import QDialog, QMessageBox, QAbstractItemView, QLineEdit, QMenu
from PySide6.QtGui import QShortcut, QKeySequence, QContextMenuEvent, QAction

from iode_gui.abstract_main_window import AbstractMainWindow

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

from pathlib import Path
import numpy as np
from typing import Union, Tuple, List, Dict, Any
from iode import IodeType, equations, scalars, tables, variables, Table, Sample, Period


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
        super().__init__(IodeType.EQUATIONS, EquationsDelegate(parent), parent)
    
        # headers
        self.horizontalHeader().setStretchLastSection(True)
        self.verticalHeader().setStretchLastSection(False)

    def _open_edit_dialog(self, obj_name: str):
        model: IodeAbstractTableModel = self.model()
        self.edit_dialog = EditEquationDialog(obj_name, model.displayed_database, self.main_window, self)
        self.edit_dialog.database_modified.connect(self.database_modified)
        self.edit_dialog.new_object_inserted.connect(self.new_object_inserted)

    def _open_add_dialog(self) -> QDialog:
        model: IodeAbstractTableModel = self.model()
        return EditEquationDialog("", model.displayed_database, self.main_window, self)

    # override not implemented in abstract view class
    def _print_to_file_kwargs(self, dialog: PrintFileDialog) -> Dict[str, Any]:        
        print_equations_as = dialog.print_equations_as
        if print_equations_as is not None:
            equations.print_equations_as = print_equations_as

        print_equations_lec_as = dialog.print_equations_lec_as
        if print_equations_lec_as is not None:
            equations.print_equations_lec_as = print_equations_lec_as

        nb_decimals = dialog.print_nb_decimals
        if nb_decimals is not None:
            equations.print_nb_decimals = nb_decimals

        return dict()


class IdentitiesView(IodeAbstractTableView):
    def __init__(self, parent=None):
        super().__init__(IodeType.IDENTITIES, IdentitiesDelegate(parent), parent)

        self.context_menu: QMenu = None

        # to store the execution sample for identities
        self.execution_sample: str = ""

        # headers
        self.horizontalHeader().setStretchLastSection(True)
        self.verticalHeader().setStretchLastSection(False)


        # main window will be set in setup() method
        self.main_window: AbstractMainWindow = None

        # ---- keyboard shortcuts ----
        self.shortcut_execute_current_idt = QShortcut(QKeySequence(Qt.Key.Key_F7), self)
        self.shortcut_execute_current_idt.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)

        self.shortcut_execute_idts = QShortcut(QKeySequence(Qt.Modifier.SHIFT | Qt.Key.Key_F7), self)
        self.shortcut_execute_idts.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
    
    # override IodeAbstractTableView method 
    def setup(self, main_window: AbstractMainWindow):
        super().setup(main_window)
        self.main_window = main_window
        self.shortcut_execute_current_idt.activated.connect(self.execute_current_identity)
        self.shortcut_execute_idts.activated.connect(self.main_window.open_compute_identities_dialog)
        self.setup_context_menu()

    def _open_edit_dialog(self, obj_name: str):
        model: IodeAbstractTableModel = self.model()
        self.edit_dialog = EditIdentityDialog(obj_name, model.displayed_database, self)
        self.edit_dialog.database_modified.connect(self.database_modified)

    def _open_add_dialog(self) -> QDialog:
        model: IodeAbstractTableModel = self.model()
        return AddIdentityDialog(model.displayed_database, self)

    def setup_context_menu(self):
        self.context_menu = QMenu(self)

        # Create a new action for executing the selected identity
        action = QAction("Execute Identity", self)
        action.setShortcut(self.shortcut_execute_current_idt.key())
        action.setToolTip("Execute the selected identity")
        action.setVisible(True)
        action.triggered.connect(self.execute_current_identity)
        self.context_menu.addAction(action)

        # Create a new action for opening the compute identities dialog
        action = QAction("Open Execute Identities Menu", self)
        action.setShortcut(self.shortcut_execute_idts.key())
        action.setToolTip("Open the 'Execute Identities' menu")
        action.setVisible(True)
        action.triggered.connect(self.main_window.open_compute_identities_dialog)
        self.context_menu.addAction(action)

    @Slot(QContextMenuEvent)
    def popup_context_menu(self, event: QContextMenuEvent):
        """
        Popup context menu at the event's global position.

        :param event: The context menu event.
        :type event: QContextMenuEvent
        """
        self.context_menu.popup(event.globalPos())

    # override QTableView method
    @Slot(QContextMenuEvent)
    def contextMenuEvent(self, event: QContextMenuEvent):
        self.popup_context_menu(event)
        event.accept()

    @Slot(str)
    def set_execution_sample(self, execution_sample: str):
        self.execution_sample = execution_sample.strip()

    @Slot()
    def execute_current_identity(self):
        # Get the selected object
        selection = self.selectionModel().selectedRows()
        if not selection:
            return
        index = selection[0]

        # Check if global sample is defined
        if not self.check_global_sample():
            QMessageBox.warning(self, "WARNING", "The Variables sample is not defined. "
                                "Please set the Variables sample from the menu "
                                "'Workspace > Set Variables Sample' before executing identities.")
            return
        
        # extract the execution sample
        try:
            variables_sample = variables.sample
            if not self.execution_sample:
                from_period = variables_sample.start
                to_period = variables_sample.end
            else:
                if ':' not in self.execution_sample:
                    QMessageBox.warning(self, "WARNING", "The execution sample is not valid. "
                                        "The semicolon ':' is missing.")
                    return
                from_period, to_period = self.execution_sample.split(':')
                from_period = from_period.strip()
                from_period = Period(from_period) if from_period else variables_sample.start
                to_period = to_period.strip()
                to_period = Period(to_period) if to_period else variables_sample.end
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Could not parse the execution sample:\n{str(e)}")
            return

        if from_period > to_period:
            QMessageBox.warning(self, "WARNING", "The execution sample is invalid. "
                                "The start period must be less than or equal to the end period.")
            return
        if from_period < variables_sample.start:
            QMessageBox.warning(self, "WARNING", "The execution sample is invalid. "
                                "The start period must be greater than or equal to the variables sample "
                                f"start period {variables_sample.start}.")
            return
        if to_period > variables_sample.end:
            QMessageBox.warning(self, "WARNING", "The execution sample is invalid. "
                                "The end period must be less than or equal to the variables sample "
                                f"end period {variables_sample.end}.")
            return

        # Computes the selected identity + updates other IODE objects tabs if necessary
        self.main_window.compute_hash(True)
        model: IdentitiesModel = self.model()
        model.execute_identity(index.row(), from_period, to_period)
        self.main_window.update_tab_and_completer()


class ListsView(IodeAbstractTableView):
    def __init__(self, parent=None):
        super().__init__(IodeType.LISTS, ListsDelegate(parent), parent)
    
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
        IodeAbstractTableView.__init__(self, IodeType.SCALARS, ScalarsDelegate(parent), parent)
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

    # override not implemented in abstract view class
    def _print_to_file_kwargs(self, dialog: PrintFileDialog) -> Dict[str, Any]:  
        nb_decimals = dialog.print_nb_decimals
        if nb_decimals is not None:
            scalars.print_nb_decimals = nb_decimals
        return dict()

class TablesView(IodeAbstractTableView):
    def __init__(self, parent=None):
        super().__init__(IodeType.TABLES, TablesDelegate(parent), parent)

        self.context_menu: QMenu = None

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

    # override IodeAbstractTableView method 
    def setup(self, main_window: AbstractMainWindow):
        super().setup(main_window)
        self.setup_context_menu()

    def _open_edit_dialog(self, obj_name: str):
        model: IodeAbstractTableModel = self.model()
        self.edit_dialog = EditTableDialog(obj_name, model.displayed_database, self.main_window, self)
        self.edit_dialog.database_modified.connect(self.database_modified)

    def _open_add_dialog(self) -> QDialog:
        model: IodeAbstractTableModel = self.model()
        return AddTableDialog(model.displayed_database, self)

    # override not implemented in abstract view class
    def _print_to_file_kwargs(self, dialog: PrintFileDialog) -> Dict[str, Any]: 
        print_tables_as = dialog.print_tables_as
        if print_tables_as is not None:
            tables.print_tables_as = print_tables_as
        
        nb_decimals = dialog.print_nb_decimals
        if nb_decimals is not None:
            tables.print_nb_decimals = nb_decimals
        
        generalized_sample = dialog.generalized_sample
        if generalized_sample is not None:
            return {'generalized_sample': dialog.generalized_sample}
        else:
            return dict()

    def setup_context_menu(self):
        self.context_menu = QMenu(self)

        # Create a new action for plotting selected variables
        action = QAction("Display", self)
        action.setShortcut(self.shortcut_display.key())
        action.setToolTip("Display the edition window of the selected table")
        action.setVisible(True)
        action.triggered.connect(self.display)
        self.context_menu.addAction(action)

        # Create a new action for opening the plot dialog
        action = QAction("Plot", self)
        action.setShortcut(self.shortcut_plot.key())
        action.setToolTip("Compute and plot the selected table")
        action.setVisible(True)
        action.triggered.connect(self.plot)
        self.context_menu.addAction(action)

    @Slot(QContextMenuEvent)
    def popup_context_menu(self, event: QContextMenuEvent):
        """
        Popup context menu at the event's global position.

        :param event: The context menu event.
        :type event: QContextMenuEvent
        """
        self.context_menu.popup(event.globalPos())

    # override QTableView method
    @Slot(QContextMenuEvent)
    def contextMenuEvent(self, event: QContextMenuEvent):
        self.popup_context_menu(event)
        event.accept()

    @Slot()
    def display(self):
        # import here to avoid circular import
        from iode_gui.tabs.iode_objs.tab_computed_table import ComputedTableDialog

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
            dialog = ComputedTableDialog(table_name, generalized_sample, nb_decimals=4, parent=self)
            self.main_window.append_dialog(dialog)
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
        IodeAbstractTableView.__init__(self, IodeType.VARIABLES, VariablesDelegate(parent), parent)
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

    # override method from NumericalTableView
    def _paste_special(self, names: List[str], columns: List[str], values: List[List[str]]):
        nb_periods = len(columns)
        try:
            # columns = periods
            periods = [Period(period.strip()) for period in columns]
            # if range of contiguous periods -> tuple (from_period, to_period)
            if columns == Sample(periods[0], periods[-1]).periods:
                periods = slice(periods[0], periods[-1], 1)
        # columns = values of the first variable ?
        except:
            sample = self._ask_sample()
            if sample is None:
                return
            from_period, to_period = sample
            paste_sample = Sample(from_period, to_period)
            # check that the passed sample has the size equal to len(cells) - 1
            if len(periods) != nb_periods:
                QMessageBox.warning(None, "WARNING", "Can't paste values.\n" + 
                                    f"The sample '{str(paste_sample)}' of the values to paste does not " + 
                                    f"correspond to the number of values per variable: '{len(nb_periods)}'")
                return
            periods = slice(from_period, to_period, 1)
        
        # if contiguous periods -> numpy array
        if isinstance(periods, slice):
            values = np.asarray(values, dtype=np.float64)
        
        model: VariablesModel = self.model()
        model.paste(names, periods, values)

    # override QTableView method
    @Slot(QContextMenuEvent)
    def contextMenuEvent(self, event: QContextMenuEvent):
        self.popup_context_menu(event)
        event.accept()

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