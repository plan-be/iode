from PyQt6.QtWidgets import QDialog


class PlotDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

    def plot(self) -> None:
        pass
