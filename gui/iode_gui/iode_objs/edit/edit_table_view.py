from PySide6.QtCore import Slot
from PySide6.QtWidgets import QTableView, QHeaderView

from .edit_table_delegate import EditTableDelegate
from .edit_table_model import EditTableModel

from iode import TableLineType


class EditTableView(QTableView):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.horizontalHeader().setStyleSheet("font: bold; border: 0.5px solid")
        self.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.ResizeToContents)
        self.verticalHeader().setStyleSheet("border: none")
        self.setStyleSheet("QHeaderView::section { background-color: lightGray }")

    def setup_model(self, table_name: str):
        model = EditTableModel(table_name, self.parent())
        self.setModel(model)
        self.setItemDelegate(EditTableDelegate())
        model.display_data.connect(self.update_row)

    @Slot()
    def update_row(self, row: int, line_type: int, nb_columns: int):
        if line_type != TableLineType.CELL:
            self.setSpan(row, 0, 1, nb_columns)
        if line_type == TableLineType.SEP:
            self.setRowHeight(row, 3)
