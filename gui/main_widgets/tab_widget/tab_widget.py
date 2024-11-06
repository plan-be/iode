from PySide6.QtCore import Slot, Signal
from PySide6.QtWidgets import QTabWidget, QDialog

from typing import List
from iode import IodeType


class IodeTabWidget(QTabWidget):
    fileContentModified = Signal(str, bool)
    newObjsListDialog = Signal(QDialog)

    def __init__(self, parent=None):
        super(IodeTabWidget, self).__init__(parent)

    def setup(self) -> None:
        pass

    def load_file(self, filepath: str, displayTab: bool = True, forceOverwrite: bool = False,
                  moveToPosition: int = -1, forceAsText: bool = False) -> int:
        pass

    def show_tab(self, index: int) -> None:
        pass

    @Slot(int)
    def save_tab(self, index: int = -1) -> None:
        pass

    @Slot()
    def save_all_tabs(self) -> None:
        pass

    @Slot()
    def clear_workspace(self) -> None:
        pass

    @Slot(str)
    def update_project_dir(self, project_dir_path: str):
        pass

    @Slot()
    @Slot(IodeType)
    def update_object_tab(self, iode_type: IodeType = None) -> None:
        pass

    @Slot()
    @Slot(IodeType)
    def reset_filter(self, iode_type: IodeType = None) -> None:
        pass

    @Slot(IodeType)
    def get_selected_objects_names(self, iode_type: IodeType) -> List[str]:
        pass

    @Slot(str, str)
    def file_moved(self, old_filepath: str, new_filepath: str):
        pass

    @Slot(str, str, str)
    def file_renamed(self, path: str, old_name: str, new_name: str):
        pass

    @Slot()
    def save_settings(self) -> None:
        pass

    @Slot(bool)
    def compute_hash(self, value: bool):
        pass
