from PySide6.QtCore import Qt, Signal, Slot, QModelIndex
from PySide6.QtWidgets import QDialog, QMessageBox
from PySide6.QtGui import QShortcut, QKeySequence

from iode_gui.abstract_main_window import AbstractMainWindow
from .edit_iode_obj import AbstractEditObjDialog
from .edit_table_model import EditTableModel
from .ui_edit_table import Ui_EditTableDialog
from iode_gui.plot.plot_table import PlotTableDialog

from enum import Enum
from iode import tables, variables, TableLineType


class EnumInsertWhere(Enum):
    INSERT_AT_THE_END = 0
    INSERT_AFTER_CURRENT = 1
    INSERT_BEFORE_CURRENT = 2


class EditTableDialog(AbstractEditObjDialog):
    new_plot = Signal(QDialog)

    def __init__(self, name: str, database, main_window: AbstractMainWindow, parent=None):
        super().__init__(name, database, tables, parent)
        self.ui = Ui_EditTableDialog()
        self.ui.setupUi(self)

        try:
            self.previous_value = database[name]
            self.ui.tableView.setup_model(name)

            self.ui.lineEdit_name.setText(name)
            self.ui.lineEdit_name.setReadOnly(True)

            self.v_line_types = list(TableLineType)
            line_types = [name.title() for name in TableLineType.__members__]
            self.ui.comboBox_insert_line_type.addItems(line_types)

            self.v_insert_where = list(EnumInsertWhere)
            insert_where = [name.replace('_', ' ').title().replace('Insert ', '') 
                            for name in EnumInsertWhere.__members__]
            self.ui.comboBox_insert_where.addItems(insert_where)

            self.delete_shortcut = QShortcut(QKeySequence(Qt.Key.Key_Delete), self)
            self.delete_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
            
            self.delete_shortcut.activated.connect(self.delete_line)
            self.new_plot.connect(main_window.append_plot)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def edit(self):
        name = self.ui.lineEdit_name.text()
        edit_table_model: EditTableModel = self.ui.tableView.model()
        table = edit_table_model.table
        self._edit(name, table)

    @Slot()
    def plot(self):
        try:
            edit_table_model: EditTableModel = self.ui.tableView.model()
            table = edit_table_model.table
            sample = variables.sample
            generalized_sample = f"{str(sample.start)}:{sample.nb_periods}"
            computed_table = table.compute(generalized_sample)
            dialog = PlotTableDialog(computed_table)
            self.new_plot.emit(dialog)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def insert_line(self):
        try:
            edit_table_model: EditTableModel = self.ui.tableView.model()
            selection = self.ui.tableView.selectionModel().selectedRows()
            position = selection[0].row() if selection else -1
            i_insert_line_type: int = self.ui.comboBox_insert_line_type.currentIndex()
            line_type = self.v_line_types[i_insert_line_type]
            i_insert_where = self.ui.comboBox_insert_where.currentIndex()
            insert_where = self.v_insert_where[i_insert_where]

            if insert_where == EnumInsertWhere.INSERT_AT_THE_END:
                edit_table_model.append_line(line_type)
            elif insert_where == EnumInsertWhere.INSERT_AFTER_CURRENT:
                if position < 0:
                    return
                edit_table_model.insert_line(line_type, position, after=True)
            elif insert_where == EnumInsertWhere.INSERT_BEFORE_CURRENT:
                if position < 0:
                    return
                edit_table_model.insert_line(line_type, position, after=False)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def delete_line(self):
        try:
            edit_table_model: EditTableModel = self.ui.tableView.model()
            selection = self.ui.tableView.selectionModel().selectedRows()
            if not selection:
                return
            row = selection[0].row()
            # if divider line selected -> do nothing
            if row == 0:
                return
            edit_table_model.removeRow(row, QModelIndex())
            self.update()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
