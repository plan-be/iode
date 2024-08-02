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


# Mixin class for handling settings in derived class.
class IodeSettingsDialog(QDialog):
    def __init__(self, class_name: str, parent: QWidget=None, 
                 f=Qt.WindowType.WindowMinimizeButtonHint | Qt.WindowType.WindowCloseButtonHint):
        super().__init__(parent, f)
        self.class_name = class_name
        self.map_fields = {}

    def save_settings(self):
        project_settings = ProjectSettings.project_settings
        project_settings.beginGroup(self.class_name)
        for key, field in self.map_fields.items():
            project_settings.setValue(key, field.value)
        project_settings.endGroup()

    def load_settings(self):
        project_settings = ProjectSettings.project_settings
        project_settings.beginGroup(self.class_name)
        for key, field in self.map_fields.items():
            if project_settings.contains(key):
                field.value = project_settings.value(key)
        project_settings.endGroup()

    def closeEvent(self, event: QCloseEvent):
        self.save_settings()
        event.accept()

    @pyqtSlot()
    def help(self):
        QDesktopServices.openUrl(URL_MANUAL)

    def accept(self):
        self.save_settings()
        super().accept()

    def reject(self):
        self.save_settings()
        super().reject()
