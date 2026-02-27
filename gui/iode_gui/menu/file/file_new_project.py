from qtpy.QtCore import Slot, QSettings, QDir
from qtpy.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.util.widgets.file_chooser import EnumFileMode
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_file_new_project import Ui_MenuFileNewProject

from pathlib import Path
from iode import IodeFileType


class MenuFileNewProject(MixinSettingsDialog):
    def __init__(self, project_path: str, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuFileNewProject()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)
        
        self.ui.fileChooser_new_project.enum_file_type = IodeFileType.DIRECTORY
        self.ui.fileChooser_new_project.enum_file_mode = EnumFileMode.EXISTING_FILE

        if not project_path: 
            new_project_dir = Path(QDir.homePath()).resolve()
        else:
            new_project_dir = Path(project_path).resolve() if project_path else None

        if new_project_dir and new_project_dir.is_dir():
            self.ui.fileChooser_new_project.filepath = new_project_dir

    @property
    def project_path(self) -> str:
        return self.ui.fileChooser_new_project.filepath.strip()

    @Slot()
    def create(self):
        try:
            new_project_path = Path(self.ui.fileChooser_new_project.filepath.strip())
            parent_dir = Path(new_project_path).parent.resolve()
            new_project_dir_name = new_project_path.name

            error_msg = f"Cannot create the new project directory {new_project_dir_name} in {str(parent_dir)}"

            # check if the parent dir exists
            if not parent_dir.exists():
                QMessageBox.warning(self, "WARNING", f"{error_msg}.\nThe directory {new_project_path} does not exist.")
                return
            # check if passed path represents a path to a directory
            suffix = new_project_path.suffix
            if new_project_path.suffix:
                QMessageBox.warning(self, "WARNING", f"{error_msg}.\nThe name {new_project_dir_name} does not represent a directory.")
                return
            # check if new project directory already exists
            if new_project_path.exists():
                QMessageBox.warning(self, "WARNING", f"{error_msg}.\nThe directory {new_project_path} already exists.")
                return
            
            try:
                new_project_path.mkdir()
            except Exception as e:
                QMessageBox.warning(self, "WARNING", f"{error_msg}\n{str(e)}")

            self.accept()
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))
