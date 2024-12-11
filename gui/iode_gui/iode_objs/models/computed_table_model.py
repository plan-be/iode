from PySide6.QtCore import Qt, QModelIndex, QAbstractTableModel, QObject, Slot
from PySide6.QtGui import QCloseEvent
from PySide6.QtWidgets import QMessageBox

from .numerical_table_model import IodeNumericalTableModel  
from iode_gui.utils import NAN_REP  

from iode import tables, Table
from iode.iode_cython import ComputedTable


class ComputedTableModel(QAbstractTableModel, IodeNumericalTableModel):
    def __init__(self, table_name: str, generalized_sample: str, nb_decimals: int, 
                 variables: str=None, parent: QObject=None):
        QAbstractTableModel.__init__(self, parent)
        IodeNumericalTableModel.__init__(self, precision=nb_decimals)
        self.table_name: str = table_name
        self.variables: str = variables
        if variables is not None and variables != "":
            tables[table_name] = Table(table_name, 2, "", variables, False, False, False)

        table: Table = tables[table_name]
        self.computed_table: ComputedTable = table.compute(generalized_sample, variables, self.precision)
        self.destroyed.connect(self.maybe_remove_table)
        self.precision_changed.connect(self.update_nb_decimals)

    @Slot()
    def maybe_remove_table(self):
        if self.variables is not None and self.variables != "":
            del tables[self.table_name]

    @Slot()
    def update_nb_decimals(self, nb_decimals: int):
        self.computed_table.nb_decimals = nb_decimals
        self.update_display_values()

    def rowCount(self, parent: QModelIndex = QModelIndex()):
        return self.computed_table.nb_lines

    def columnCount(self, parent: QModelIndex = QModelIndex()):
        return self.computed_table.nb_columns

    def flags(self, index: QModelIndex):
        if not index.isValid():
            return Qt.ItemFlag.ItemIsEnabled
        
        if self.computed_table.is_editable(index.row(), index.column()):
            flag = Qt.ItemFlag.ItemIsEditable 
        else: 
            flag = Qt.ItemFlag.NoItemFlags
        
        return QAbstractTableModel.flags(self, index) | flag

    def headerData(self, section: int, orientation: Qt.Orientation, role: int = Qt.ItemDataRole.DisplayRole):
        if role != Qt.ItemDataRole.DisplayRole:
            return None

        if orientation == Qt.Orientation.Horizontal:
            return self.computed_table.columns[section]
        else:
            return self.computed_table.lines[section]

    def data(self, index: QModelIndex, role: int = Qt.ItemDataRole.DisplayRole):
        if not index.isValid():
            return ""

        if role == Qt.ItemDataRole.TextAlignmentRole:
            return Qt.AlignmentFlag.AlignRight

        if role == Qt.ItemDataRole.DisplayRole or role == Qt.ItemDataRole.EditRole:
            return self.value_to_string(self.computed_table[index.row(), index.column()])

        return None

    def setData(self, index: QModelIndex, value: str, role: int):
        if index.isValid() and role == Qt.ItemDataRole.EditRole:
            if self.data(index, Qt.ItemDataRole.DisplayRole) == value:
                return False
            if value == NAN_REP:
                return False
            try:
                self.computed_table[index.row(), index.column()] = value
                self.dataChanged.emit(index, index, [role])
                return True
            except Exception as e:
                QMessageBox.warning(None, "WARNING", str(e))
                return False
        else:
            return False

    # override IodeNumericalTableModel method
    def update_display_values(self):
        self.reset()

    # override IodeNumericalTableModel method
    def reset(self):
        self.beginResetModel()
        self.endResetModel()
