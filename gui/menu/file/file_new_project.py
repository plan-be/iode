
from PyQt6.QtWidgets import QDialog, QWidget


class MenuFileNewProject(QDialog):
    def __init__(self, project_path: str, parent: QWidget = None) -> None:
        super().__init__(parent)
        self.project_path = project_path
