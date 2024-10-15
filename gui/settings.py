from PySide6.QtCore import Qt, QSettings, QDir, Slot
from PySide6.QtWidgets import QWidget, QDialog
from PySide6.QtGui import QDesktopServices, QCloseEvent

from utils import URL_MANUAL

RUN_REPORTS_FROM_PROJECT_DIR = "run_reports_from_project_dir"


class ProjectSettings:
    project_settings: QSettings = None

    def __init__(self) -> None:
        raise NotImplementedError()

    @classmethod
    def change_project(cls, project_dir: QDir, parent: QWidget=None) -> QSettings:
        filepath = project_dir.absoluteFilePath("iode_gui_settings.ini")
        cls.project_settings = QSettings(filepath, QSettings.Format.IniFormat, parent)
        return cls.project_settings
