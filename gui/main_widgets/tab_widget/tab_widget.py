from PyQt6.QtCore import pyqtSlot
from PyQt6.QtWidgets import QTabWidget

from typing import List
from iode import IodeTypes


class IodeTabWidget(QTabWidget):
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

    @pyqtSlot()
    def save_settings(self) -> None:
        pass

    @pyqtSlot(bool)
    def compute_hash(self, value: bool):
        pass
