from PyQt6.QtCore import Qt, QSettings, QDir, pyqtSlot
from PyQt6.QtWidgets import QWidget, QDialog
from PyQt6.QtGui import QDesktopServices, QCloseEvent

from utils import URL_MANUAL


class ProjectSettings:
    project_settings: QSettings = None

    def __init__(self) -> None:
        raise NotImplementedError()

    @classmethod
    def change_project(cls, project_dir: QDir, parent: QWidget=None) -> QSettings:
        filepath = project_dir.absoluteFilePath("iode_gui_settings.ini")
        cls.project_settings = QSettings(filepath, QSettings.Format.IniFormat, parent)
        return cls.project_settings