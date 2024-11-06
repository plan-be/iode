from PySide6.QtCore import Qt, QSettings, QDir, Slot
from PySide6.QtWidgets import (QWidget, QDialog, QComboBox, QFontComboBox, QCheckBox, 
                               QRadioButton, QSpinBox, QDoubleSpinBox, QLineEdit, QTextEdit, 
                               QPlainTextEdit)
from PySide6.QtGui import QDesktopServices, QCloseEvent

from util.widgets.file_chooser import IodeFileChooser
from util.widgets.sample_edit import IodeSampleEdit


import re
from utils import URL_MANUAL

RUN_REPORTS_FROM_PROJECT_DIR = "run_reports_from_project_dir"
PRINT_DESTINATION = "print_destination"


class ProjectSettings:
    project_settings: QSettings = None

    def __init__(self) -> None:
        raise NotImplementedError()

    @classmethod
    def change_project(cls, project_dir: QDir, parent: QWidget=None) -> QSettings:
        filepath = project_dir.absoluteFilePath("iode_gui_settings.ini")
        cls.project_settings = QSettings(filepath, QSettings.Format.IniFormat, parent)
        return cls.project_settings


class MenuItemSettings(QDialog):
    def __init__(self, parent: QWidget=None) -> None:
        super().__init__(parent)
        self.ui_obj = None

    def prepare_settings(self, ui_obj) -> None:
        self.ui_obj = ui_obj

        menu_class_name = ui_obj.__class__.__name__
        # remove the Ui_ part of the class name
        menu_class_name = menu_class_name.replace("Ui_", "")
        # CamelCase -> snake_case
        # a) use a regular expression to find all uppercase letters that are preceded by a lowercase letter
        menu_class_name = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', menu_class_name)
        # b) use a regular expression to find all uppercase letters that are followed by a lowercase letter
        menu_class_name = re.sub('([a-z0-9])([A-Z])', r'\1_\2', menu_class_name)
        # Convert the string to uppercase
        self.menu_class_name = menu_class_name.upper()

    # override QDialog method
    @Slot()
    def closeEvent(self, event: QCloseEvent):
        self.save_settings()
        event.accept()

    # override QDialog method
    @Slot()
    def accept(self):
        self.save_settings()
        super().accept()

    # override QDialog method
    @Slot()
    def reject(self):
        self.save_settings()
        super().reject()

    @Slot()
    def load_settings(self):
        project_settings = ProjectSettings.project_settings
        if not project_settings:
            return

        project_settings.beginGroup("MENU")
        project_settings.beginGroup(self.menu_class_name)

        for attr_name, attr_value in vars(self.ui_obj).items():
            if isinstance(attr_value, (QComboBox, QFontComboBox)):
                attr_value.setCurrentIndex(project_settings.value(attr_name, type=int))
            elif isinstance(attr_value, (QCheckBox, QRadioButton)):
                attr_value.setChecked(project_settings.value(attr_name, type=bool))
            elif isinstance(attr_value, QSpinBox):
                attr_value.setValue(project_settings.value(attr_name, type=int))
            elif isinstance(attr_value, QDoubleSpinBox):
                attr_value.setValue(project_settings.value(attr_name, type=float))
            elif isinstance(attr_value, (QLineEdit, QTextEdit)):
                attr_value.setText(project_settings.value(attr_name, type=str))
            elif isinstance(attr_value, QPlainTextEdit):
                attr_value.setPlainText(project_settings.value(attr_name, type=str))
            elif isinstance(attr_value, IodeFileChooser):
                attr_value.filepath = project_settings.value(attr_name, type=str)
            elif isinstance(attr_value, IodeSampleEdit):
                attr_value.setText(project_settings.value(attr_name, type=str))

        project_settings.endGroup()
        project_settings.endGroup()

    @Slot()
    def save_settings(self):
        project_settings = ProjectSettings.project_settings
        if not project_settings:
            return

        project_settings.beginGroup("MENU")
        project_settings.beginGroup(self.menu_class_name)

        for attr_name, attr_value in vars(self.ui_obj).items():
            if isinstance(attr_value, (QComboBox, QFontComboBox)):
                project_settings.setValue(attr_name, attr_value.currentIndex())
            elif isinstance(attr_value, (QCheckBox, QRadioButton)):
                project_settings.setValue(attr_name, attr_value.isChecked())
            elif isinstance(attr_value, (QSpinBox, QDoubleSpinBox)):
                project_settings.setValue(attr_name, attr_value.value())
            elif isinstance(attr_value, (QLineEdit, QTextEdit)):
                project_settings.setValue(attr_name, attr_value.text())
            elif isinstance(attr_value, QPlainTextEdit):
                project_settings.setValue(attr_name, attr_value.toPlainText())
            elif isinstance(attr_value, IodeFileChooser):
                project_settings.setValue(attr_name, attr_value.filepath)
            elif isinstance(attr_value, IodeSampleEdit):
                project_settings.setValue(attr_name, attr_value.text())

        project_settings.endGroup()
        project_settings.endGroup()

    @Slot()
    def help(self):
        """Shows the help dialog."""
        QDesktopServices.openUrl(URL_MANUAL)
