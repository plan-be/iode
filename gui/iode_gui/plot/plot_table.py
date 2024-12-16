from PySide6.QtCore import Slot
from PySide6.QtWidgets import QDialog

from iode.util import ComputedTable


class PlotTableDialog(QDialog):
    def __init__(self, computed_table: ComputedTable, parent=None):
        super().__init__(parent)
        raise NotImplementedError("PlotTableDialog is not implemented yet")

    @Slot()
    def plot(self):
        pass
