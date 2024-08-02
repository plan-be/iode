from PyQt6.QtCore import QDir
from PyQt6.QtWidgets import QDialog, QMainWindow


class ProjectSettings:
    def change_project(project_dir: QDir, main_window: QMainWindow) -> None:
        pass


class IodeSettingsDialog(QDialog):
    def __init__(self, parent=None):
        super(IodeSettingsDialog, self).__init__(parent)
