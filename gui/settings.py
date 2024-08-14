from PyQt6.QtCore import QDir, QSettings
from PyQt6.QtWidgets import QDialog, QMainWindow


class ProjectSettings:
    project_settings: QSettings = None

    def __init__(self) -> None:
        raise NotImplementedError("This class should not be instantiated")
    
    @classmethod
    def change_project(cls, project_dir: QDir, main_window: QMainWindow) -> None:
        pass


class IodeSettingsDialog(QDialog):
    def __init__(self, parent=None):
        super(IodeSettingsDialog, self).__init__(parent)
