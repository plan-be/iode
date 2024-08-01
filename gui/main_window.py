from typing import List
from PyQt6.QtWidgets import QMainWindow, QWidget


class MainWindow(QMainWindow):
    def __init__(self, parent: QWidget=None, files_to_load: List[str]=None):
        super().__init__(parent)
