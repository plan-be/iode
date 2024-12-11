from PySide6.QtCore import Slot
from PySide6.QtWidgets import QDialog


class PlotTableDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

    @Slot()
    def plot(self):
        pass
