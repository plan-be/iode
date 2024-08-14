from PyQt6.QtCore import pyqtSlot, pyqtSignal
from PyQt6.QtWidgets import QTabWidget, QDialog

from typing import List
from iode import IodeTypes


class IodeTabWidget(QTabWidget):
    fileContentModified = pyqtSignal(str, bool)
    newObjsListDialog = pyqtSignal(QDialog)

    def __init__(self, parent=None):
        super(IodeTabWidget, self).__init__(parent)

    def setup(self) -> None:
        pass

    def load_file(self, filepath: str, displayTab: bool = True, forceOverwrite: bool = False,
                  moveToPosition: int = -1, forceAsText: bool = False) -> int:
        pass

    def show_tab(self, index: int) -> None:
        pass

    @pyqtSlot(int)
    def save_tab(self, index: int = -1) -> None:
        pass

    @pyqtSlot()
    def save_all_tabs(self) -> None:
        pass

    @pyqtSlot()
    def clear_workspace(self) -> None:
        pass

    @pyqtSlot(str)
    def update_project_dir(self, project_dir_path: str):
        pass

    @pyqtSlot()
    @pyqtSlot(IodeTypes)
    def update_object_tab(self, iode_type: IodeTypes = None) -> None:
        pass

    @pyqtSlot()
    @pyqtSlot(IodeTypes)
    def reset_filter(self, iode_type: IodeTypes = None) -> None:
        pass

    @pyqtSlot(IodeTypes)
    def get_selected_objects_names(self, iode_type: IodeTypes) -> List[str]:
        pass

    @pyqtSlot(str, str)
    def file_moved(self, old_filepath: str, new_filepath: str):
        pass

    @pyqtSlot(str, str, str)
    def file_renamed(self, path: str, old_name: str, new_name: str):
        pass

    @pyqtSlot()
    def save_settings(self) -> None:
        pass

    @pyqtSlot(bool)
    def compute_hash(self, value: bool):
        pass
