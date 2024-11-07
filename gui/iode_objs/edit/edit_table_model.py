from PySide6.QtCore import QModelIndex, Qt, Signal, QAbstractTableModel
from PySide6.QtWidgets import QMessageBox, QWidget
from PySide6.QtGui import QFont, QColor

from iode import Table, TableLineType, TableCellAlign, tables
from iode.util import TableLine, TableCell

# NOTE FOR THE DEVELOPERS:
# Read-only model: https://doc.qt.io/qt-6/model-view-programming.html#read-only-access
# Editable model:  https://doc.qt.io/qt-6/model-view-programming.html#editable-items
# Resizable model: https://doc.qt.io/qt-6/model-view-programming.html#resizable-models


class EditTableModel(QAbstractTableModel):

    display_data = Signal(int, int, int)

    def __init__(self, table_name: str, parent=None):
        super().__init__(parent)
        try:
            self._table: Table = tables[table_name]
        except Exception as e:
            self._table = None
            QMessageBox.critical(parent, "ERROR", str(e))

    @property
    def table(self) -> Table:
        return self._table

    def line_type(self, row: int) -> TableLineType:
        return TableLineType[self._table[row - 1].line_type]

    def rowCount(self, parent: QModelIndex=QModelIndex()) -> int:
        return len(self._table) + 1

    def columnCount(self, parent: QModelIndex=QModelIndex()) -> int:
        return self._table.nb_columns

    def flags(self, index: QModelIndex) -> Qt.ItemFlag:
        if not index.isValid():
            return Qt.ItemFlag.ItemIsEnabled
        return super().flags(index) | Qt.ItemFlag.ItemIsEditable

    def headerData(self, section: int, orientation: Qt.Orientation, role: Qt.ItemDataRole):
        if role != Qt.ItemDataRole.DisplayRole:
            return None

        if orientation == Qt.Orientation.Vertical:
            if section == 0:
                return "DIVIS"
            else:
                line_type: TableLineType = TableLineType[self._table[section - 1].line_type]
                return " " if line_type == TableLineType.SEP else line_type.name
        else:
            if section == 0:
                return "Line Titles"
            elif section == 1:
                return "Formulas"
            else:
                return " "

    def data(self, index: QModelIndex, role: Qt.ItemDataRole=Qt.ItemDataRole.DisplayRole):
        if not index.isValid():
            return None

        is_divider = index.row() == 0
        line: TableLine = self._table.divider if is_divider else self._table[index.row() - 1]
        line_type: TableLineType = TableLineType[line.line_type]
        cell: TableCell = None
        if line_type == TableLineType.TITLE:
            cell = line[0]
        elif line_type == TableLineType.CELL:
            cell = line[index.column()]

        if role == Qt.ItemDataRole.TextAlignmentRole:
            cell_align: TableCellAlign = TableCellAlign[cell.align] if cell else TableCellAlign.LEFT 
            if cell_align == TableCellAlign.LEFT:
                return int(Qt.AlignmentFlag.AlignLeft)
            elif cell_align == TableCellAlign.CENTER:
                return int(Qt.AlignmentFlag.AlignCenter)
            elif cell_align == TableCellAlign.RIGHT or cell_align == TableCellAlign.DECIMAL:
                return int(Qt.AlignmentFlag.AlignRight)
            else:
                return int(Qt.AlignmentFlag.AlignLeft)

        if role == Qt.ItemDataRole.FontRole:
            font = QFont()
            if is_divider or not cell:
                return font
            else:
                if cell.bold:
                    font.setBold(True)
                if cell.italic:
                    font.setItalic(True)
                if cell.underline:
                    font.setUnderline(True)
                return font

        if role == Qt.ItemDataRole.BackgroundRole:
            if line_type == TableLineType.SEP:
                color = QColor("black")
            elif line_type == TableLineType.CELL:
                color = QColor("white")
            else:
                color = QColor("lightGray")
                color.setAlphaF(0.4)
            return color

        if role in (Qt.ItemDataRole.DisplayRole, Qt.ItemDataRole.EditRole):
            if line_type == TableLineType.TITLE or line_type == TableLineType.CELL:
                str_data = str(cell)
            else:
                str_data = " "
            self.display_data.emit(index.row(), line_type, self.columnCount())
            return str_data

        return None

    def setData(self, index: QModelIndex, value, role: Qt.ItemDataRole):
        if index.isValid() and role == Qt.ItemDataRole.EditRole:
            value = str(value).strip()

            try:
                is_divider = index.row() == 0
                line: TableLine = self._table.divider if is_divider else self._table[index.row() - 1]
                line_type: TableLineType = TableLineType[line.line_type]

                if line_type == TableLineType.TITLE: 
                    if not value.startswith('"'): 
                        value = '"' + value
                    if not value.endswith('"'):
                        value = value + '"'
                    line[0] = value
                elif line_type == TableLineType.CELL:
                    line[index.column()] = value
                else:
                    pass

                self.dataChanged.emit(index, index, [role])
                return True
            except Exception as e:
                QMessageBox.warning(None, "ERROR", str(e))

        return False

    def append_line(self, line_type: TableLineType):
        if line_type == TableLineType.TITLE:
            self._table += " "
        elif line_type == TableLineType.CELL:
            self._table += [''] * self._table.nb_columns
        else:
            self._table += line_type
        self.reset_model()

    def insert_line(self, line_type: TableLineType, position: int, after: bool):
        if after:
            position += 1
        if line_type == TableLineType.TITLE:
            self._table.insert(position, " ")
        elif line_type == TableLineType.CELL:
            self._table.insert(position, [''] * self._table.nb_columns)
        else:
            self._table.insert(position, line_type)
        self.reset_model()

    def removeRows(self, position: int, rows: int, index: QModelIndex=QModelIndex()):
        self.beginRemoveRows(QModelIndex(), position, position + rows - 1)
        try:
            # divider line doesn't count -> real table row = gui table row - 1
            for row in range(position, position + rows):
                del self._table[row - 1]
            self.endRemoveRows()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            self.endRemoveRows()
            return False
        return True

    def reset_model(self):
        self.beginResetModel()
        self.endResetModel()
